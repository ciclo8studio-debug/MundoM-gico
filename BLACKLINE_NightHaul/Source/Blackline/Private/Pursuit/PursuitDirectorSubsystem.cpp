// Blackline: Night Haul - Director de persecucion.
#include "Pursuit/PursuitDirectorSubsystem.h"

#include "AIController.h"
#include "Blackline.h"
#include "CollisionQueryParams.h"
#include "Engine/AssetManager.h"
#include "Engine/Engine.h"
#include "Engine/StreamableManager.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"

UPursuitDirectorSubsystem* UPursuitDirectorSubsystem::Get(const UObject* WorldContextObject)
{
	if (!GEngine || !WorldContextObject)
	{
		return nullptr;
	}
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	return World ? World->GetSubsystem<UPursuitDirectorSubsystem>() : nullptr;
}

bool UPursuitDirectorSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void UPursuitDirectorSubsystem::Deinitialize()
{
	CleanupRuntime();
	State = EPursuitState::None;
	Super::Deinitialize();
}

void UPursuitDirectorSubsystem::SetState(EPursuitState NewState)
{
	if (NewState == State)
	{
		return;
	}
	State = NewState;
	UE_LOG(LogBlackline, Log, TEXT("Pursuit: estado %d (calor %.2f)"), static_cast<int32>(State), Heat01);
	OnPursuitStateChanged.Broadcast(State);
}

void UPursuitDirectorSubsystem::CleanupRuntime()
{
	++Serial; // invalida cargas asincronas pendientes

	if (UWorld* World = GetWorld())
	{
		FTimerManager& TM = World->GetTimerManager();
		TM.ClearTimer(HeatTimer);
		TM.ClearTimer(LostCleanupTimer);
	}

	if (PatrolLoadHandle.IsValid())
	{
		PatrolLoadHandle->CancelHandle();
		PatrolLoadHandle.Reset();
	}

	for (APawn* Patrol : Patrols)
	{
		if (IsValid(Patrol))
		{
			Patrol->Destroy();
		}
	}
	Patrols.Reset();
	Target = nullptr;
	bReportedVisible = false;
}

void UPursuitDirectorSubsystem::StartPursuit(AActor* InTarget, int32 Level)
{
	UWorld* World = GetWorld();
	if (!World || !InTarget)
	{
		UE_LOG(LogBlackline, Warning, TEXT("Pursuit: StartPursuit sin mundo u objetivo"));
		return;
	}

	CleanupRuntime();

	const UPursuitSettings* Settings = UPursuitSettings::Get();
	if (Settings->Levels.Num() > 0)
	{
		const int32 Index = FMath::Clamp(Level - 1, 0, Settings->Levels.Num() - 1);
		ActiveConfig = Settings->Levels[Index];
	}
	else
	{
		ActiveConfig = FPursuitLevelConfig();
		ActiveConfig.PatrolCount = 0;
		UE_LOG(LogBlackline, Warning, TEXT("Pursuit: UPursuitSettings.Levels vacio, no hay patrullas"));
	}

	Target = InTarget;
	Heat01 = ActiveConfig.InitialHeat01;
	bReportedVisible = true;
	LastSeenTime = World->GetTimeSeconds();
	LastKnownTargetLocation = InTarget->GetActorLocation();
	LastOrderTime = -1000.0f;

	SetState(EPursuitState::Hunting);

	World->GetTimerManager().SetTimer(HeatTimer, this, &UPursuitDirectorSubsystem::HeatTick,
		FMath::Max(0.05f, Settings->HeatUpdateIntervalSeconds), true);

	// Carga asincrona de la clase de patrulla.
	if (ActiveConfig.PatrolCount > 0 && !ActiveConfig.PatrolClass.IsNull())
	{
		if (ActiveConfig.PatrolClass.IsValid())
		{
			SpawnPatrols();
		}
		else
		{
			const int32 MySerial = Serial;
			TWeakObjectPtr<UPursuitDirectorSubsystem> WeakThis(this);
			PatrolLoadHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(
				ActiveConfig.PatrolClass.ToSoftObjectPath(),
				FStreamableDelegate::CreateLambda([WeakThis, MySerial]()
				{
					if (UPursuitDirectorSubsystem* Self = WeakThis.Get())
					{
						Self->OnPatrolClassLoaded(MySerial);
					}
				}));
		}
	}
}

void UPursuitDirectorSubsystem::OnPatrolClassLoaded(int32 InSerial)
{
	if (InSerial != Serial || State == EPursuitState::None)
	{
		return;
	}
	SpawnPatrols();
}

bool UPursuitDirectorSubsystem::FindSpawnLocation(const FVector& TargetLocation, const FVector& TargetForward, FVector& OutLocation) const
{
	const UWorld* World = GetWorld();
	const UPursuitSettings* Settings = UPursuitSettings::Get();
	if (!World)
	{
		return false;
	}

	FCollisionQueryParams Params(SCENE_QUERY_STAT(PursuitSpawn), false);
	if (Target)
	{
		Params.AddIgnoredActor(Target);
	}

	bool bFound = false;
	FVector Fallback = TargetLocation;

	for (int32 Attempt = 0; Attempt < Settings->SpawnAttempts; ++Attempt)
	{
		const float Dist = FMath::FRandRange(Settings->SpawnDistanceMinCm, FMath::Max(Settings->SpawnDistanceMinCm, Settings->SpawnDistanceMaxCm));
		const float Angle = FMath::FRandRange(0.0f, 2.0f * PI);
		const FVector Flat(FMath::Cos(Angle), FMath::Sin(Angle), 0.0f);
		FVector Candidate = TargetLocation + Flat * Dist;

		// Suelo.
		FHitResult Ground;
		const FVector Top = Candidate + FVector(0, 0, Settings->SpawnGroundTraceHalfHeightCm);
		const FVector Bottom = Candidate - FVector(0, 0, Settings->SpawnGroundTraceHalfHeightCm);
		if (!World->LineTraceSingleByChannel(Ground, Top, Bottom, Settings->SightTraceChannel, Params))
		{
			continue;
		}
		Candidate = Ground.ImpactPoint + FVector(0, 0, Settings->SpawnLiftCm);

		Fallback = Candidate;
		bFound = true;

		// Preferir puntos sin linea de vista al objetivo.
		FHitResult Block;
		const bool bBlocked = World->LineTraceSingleByChannel(Block, Candidate, TargetLocation + FVector(0, 0, 100.0f), Settings->SightTraceChannel, Params);
		if (bBlocked)
		{
			OutLocation = Candidate;
			return true;
		}
	}

	OutLocation = Fallback;
	return bFound;
}

void UPursuitDirectorSubsystem::SpawnPatrols()
{
	UWorld* World = GetWorld();
	UClass* PatrolUClass = ActiveConfig.PatrolClass.Get();
	if (!World || !PatrolUClass || !Target)
	{
		return;
	}

	const FVector TargetLoc = Target->GetActorLocation();
	const FVector TargetFwd = Target->GetActorForwardVector();

	for (int32 i = 0; i < ActiveConfig.PatrolCount; ++i)
	{
		FVector Loc;
		if (!FindSpawnLocation(TargetLoc, TargetFwd, Loc))
		{
			UE_LOG(LogBlackline, Warning, TEXT("Pursuit: sin punto de spawn valido para patrulla %d"), i);
			continue;
		}

		const FRotator Rot = (TargetLoc - Loc).GetSafeNormal2D().Rotation();
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		APawn* Patrol = World->SpawnActor<APawn>(PatrolUClass, Loc, Rot, Params);
		if (!Patrol)
		{
			continue;
		}
		if (!Patrol->GetController())
		{
			Patrol->SpawnDefaultController();
		}
		Patrols.Add(Patrol);
	}

	LastOrderTime = -1000.0f;
	IssueOrders(World->GetTimeSeconds());
}

void UPursuitDirectorSubsystem::StopPursuit()
{
	const bool bWasActive = State != EPursuitState::None;
	CleanupRuntime();
	if (bWasActive)
	{
		SetState(EPursuitState::None);
	}
}

void UPursuitDirectorSubsystem::ReportTargetVisible(bool bVisible)
{
	bReportedVisible = bVisible;
}

void UPursuitDirectorSubsystem::ReportEngineAndLights(bool bInEngineOn, bool bInLightsOn)
{
	bEngineOn = bInEngineOn;
	bLightsOn = bInLightsOn;
}

bool UPursuitDirectorSubsystem::PatrolSeesTarget(const APawn* Patrol, bool& bOutHeard) const
{
	bOutHeard = false;
	const UWorld* World = GetWorld();
	if (!World || !IsValid(Patrol) || !Target)
	{
		return false;
	}

	const UPursuitSettings* Settings = UPursuitSettings::Get();
	const bool bHunting = (State == EPursuitState::Hunting);

	const FVector Eye = Patrol->GetPawnViewLocation();
	const FVector TargetLoc = Target->GetActorLocation();
	const FVector ToTarget = TargetLoc - Eye;
	const float DistSq = ToTarget.SizeSquared();

	// Esfera de oido: solo si el motor esta encendido.
	if (bEngineOn && DistSq <= FMath::Square(ActiveConfig.HearingRangeCm))
	{
		bOutHeard = true;
	}

	// Cono de vision / busqueda (luces encendidas amplian el alcance).
	float Range = bHunting ? ActiveConfig.SightRangeCm : ActiveConfig.SearchRangeCm;
	if (bLightsOn)
	{
		Range *= Settings->LightsOnSightMultiplier;
	}
	if (DistSq > FMath::Square(Range))
	{
		return false;
	}

	const float HalfAngle = bHunting ? ActiveConfig.SightHalfAngleDeg : ActiveConfig.SearchHalfAngleDeg;
	const FVector Dir = ToTarget.GetSafeNormal();
	const FVector Facing = Patrol->GetViewRotation().Vector();
	if (FVector::DotProduct(Facing, Dir) < FMath::Cos(FMath::DegreesToRadians(HalfAngle)))
	{
		return false;
	}

	// Linea de vista.
	FCollisionQueryParams Params(SCENE_QUERY_STAT(PursuitSight), false);
	Params.AddIgnoredActor(Patrol);
	Params.AddIgnoredActor(Target);
	FHitResult Hit;
	return !World->LineTraceSingleByChannel(Hit, Eye, TargetLoc, Settings->SightTraceChannel, Params);
}

void UPursuitDirectorSubsystem::HeatTick()
{
	UWorld* World = GetWorld();
	if (!World || !IsValid(Target))
	{
		StopPursuit();
		return;
	}

	const UPursuitSettings* Settings = UPursuitSettings::Get();
	const float Dt = FMath::Max(0.05f, Settings->HeatUpdateIntervalSeconds);
	const float Now = World->GetTimeSeconds();

	bool bVisible = bReportedVisible;
	bool bHeard = false;
	for (const APawn* Patrol : Patrols)
	{
		bool bPatrolHeard = false;
		if (PatrolSeesTarget(Patrol, bPatrolHeard))
		{
			bVisible = true;
		}
		bHeard |= bPatrolHeard;
	}

	if (bVisible || bHeard)
	{
		LastKnownTargetLocation = Target->GetActorLocation();
	}

	if (bVisible)
	{
		LastSeenTime = Now;
		Heat01 = FMath::Clamp(Heat01 + Settings->HeatGainPerSecondVisible * Dt, 0.0f, 1.0f);
		SetState(EPursuitState::Hunting);
	}
	else
	{
		// Sin linea de vista: enfria; motor/luces apagados enfrian mas. Si lo oyen, no enfria.
		float Cool = Settings->BaseCoolPerSecond;
		if (!bEngineOn)
		{
			Cool *= Settings->EngineOffCoolMultiplier;
		}
		if (!bLightsOn)
		{
			Cool *= Settings->LightsOffCoolMultiplier;
		}
		if (bHeard)
		{
			Cool = 0.0f;
		}
		Heat01 = FMath::Clamp(Heat01 - Cool * Dt, 0.0f, 1.0f);

		if (State == EPursuitState::Hunting && Now - LastSeenTime >= Settings->LoseSightGraceSeconds)
		{
			SetState(EPursuitState::Searching);
		}
	}

	if (Heat01 <= Settings->LostHeatThreshold01 && !bVisible)
	{
		EnterLost();
		return;
	}

	IssueOrders(Now);
}

void UPursuitDirectorSubsystem::IssueOrders(float Now)
{
	const UPursuitSettings* Settings = UPursuitSettings::Get();
	if (Now - LastOrderTime < Settings->RepathIntervalSeconds)
	{
		return;
	}
	LastOrderTime = Now;

	for (APawn* Patrol : Patrols)
	{
		if (!IsValid(Patrol))
		{
			continue;
		}
		AAIController* AI = Cast<AAIController>(Patrol->GetController());
		if (!AI)
		{
			continue;
		}

		if (State == EPursuitState::Hunting && Target)
		{
			AI->MoveToActor(Target, Settings->AcceptanceRadiusCm);
		}
		else if (State == EPursuitState::Searching)
		{
			FVector Offset = FVector(FMath::FRandRange(-1.0f, 1.0f), FMath::FRandRange(-1.0f, 1.0f), 0.0f).GetSafeNormal();
			Offset *= FMath::FRand() * Settings->SearchRadiusCm;
			AI->MoveToLocation(LastKnownTargetLocation + Offset, Settings->AcceptanceRadiusCm);
		}
	}
}

void UPursuitDirectorSubsystem::EnterLost()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	World->GetTimerManager().ClearTimer(HeatTimer);
	for (APawn* Patrol : Patrols)
	{
		if (IsValid(Patrol))
		{
			if (AAIController* AI = Cast<AAIController>(Patrol->GetController()))
			{
				AI->StopMovement();
			}
		}
	}

	SetState(EPursuitState::Lost);

	const UPursuitSettings* Settings = UPursuitSettings::Get();
	World->GetTimerManager().SetTimer(LostCleanupTimer, this, &UPursuitDirectorSubsystem::StopPursuit,
		FMath::Max(0.1f, Settings->LostCleanupDelaySeconds), false);
}
