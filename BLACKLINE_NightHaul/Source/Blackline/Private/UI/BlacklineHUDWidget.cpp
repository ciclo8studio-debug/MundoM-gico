#include "UI/BlacklineHUDWidget.h"

#include "Blackline.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Vehicle/BlacklineVanPawn.h"
#include "Cargo/CargoSimulationComponent.h"
#include "Checkpoint/CheckpointController.h"
#include "Pursuit/PursuitDirectorSubsystem.h"

#define LOCTEXT_NAMESPACE "BlacklineHUD"

void UBlacklineHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	RebindToVan();
	BindCheckpoints();
	BindPursuit();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(PollTimer, this, &UBlacklineHUDWidget::Poll,
			FMath::Max(0.02f, PollIntervalSeconds), true);
	}
	Poll();
}

void UBlacklineHUDWidget::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PollTimer);
	}
	UnbindAll();
	Super::NativeDestruct();
}

void UBlacklineHUDWidget::RebindToVan()
{
	ABlacklineVanPawn* Van = Cast<ABlacklineVanPawn>(GetOwningPlayerPawn());
	if (Van != BoundVan.Get())
	{
		BindVan(Van);
	}
}

void UBlacklineHUDWidget::BindVan(ABlacklineVanPawn* Van)
{
	if (UCargoSimulationComponent* Old = BoundCargo.Get())
	{
		Old->OnCargoIntegrityChanged.RemoveDynamic(this, &UBlacklineHUDWidget::HandleCargoIntegrityChanged);
	}
	BoundCargo.Reset();
	BoundVan = Van;
	bHasDriveMode = false;
	LastSpeedInt = -1;
	bCriticalLatched = false;

	if (!Van)
	{
		return;
	}

	if (UCargoSimulationComponent* Cargo = Van->Cargo)
	{
		BoundCargo = Cargo;
		Cargo->OnCargoIntegrityChanged.AddUniqueDynamic(this, &UBlacklineHUDWidget::HandleCargoIntegrityChanged);
		RefreshCargo(Cargo->GetCargoIntegrity01());
	}
	RefreshSpeed(Van->GetSpeedKmh());
	RefreshDriveMode(Van->GetDriveMode());
}

void UBlacklineHUDWidget::BindCheckpoints()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(World, ACheckpointController::StaticClass(), Found);
	for (AActor* Actor : Found)
	{
		if (ACheckpointController* Checkpoint = Cast<ACheckpointController>(Actor))
		{
			Checkpoint->OnCheckpointStateChanged.AddUniqueDynamic(this, &UBlacklineHUDWidget::HandleCheckpointStateChanged);
			BoundCheckpoints.Add(Checkpoint);
		}
	}
	RefreshCheckpoint(ECheckpointState::Idle);
}

void UBlacklineHUDWidget::BindPursuit()
{
	if (UPursuitDirectorSubsystem* Pursuit = UPursuitDirectorSubsystem::Get(this))
	{
		BoundPursuit = Pursuit;
		Pursuit->OnPursuitStateChanged.AddUniqueDynamic(this, &UBlacklineHUDWidget::HandlePursuitStateChanged);
		RefreshHeat(Pursuit->GetHeat01());
	}
}

void UBlacklineHUDWidget::UnbindAll()
{
	if (UCargoSimulationComponent* Cargo = BoundCargo.Get())
	{
		Cargo->OnCargoIntegrityChanged.RemoveDynamic(this, &UBlacklineHUDWidget::HandleCargoIntegrityChanged);
	}
	for (const TWeakObjectPtr<ACheckpointController>& Weak : BoundCheckpoints)
	{
		if (ACheckpointController* Checkpoint = Weak.Get())
		{
			Checkpoint->OnCheckpointStateChanged.RemoveDynamic(this, &UBlacklineHUDWidget::HandleCheckpointStateChanged);
		}
	}
	if (UPursuitDirectorSubsystem* Pursuit = BoundPursuit.Get())
	{
		Pursuit->OnPursuitStateChanged.RemoveDynamic(this, &UBlacklineHUDWidget::HandlePursuitStateChanged);
	}
	BoundCargo.Reset();
	BoundVan.Reset();
	BoundPursuit.Reset();
	BoundCheckpoints.Reset();
}

void UBlacklineHUDWidget::Poll()
{
	ABlacklineVanPawn* Van = BoundVan.Get();
	if (!Van)
	{
		RebindToVan(); // la posesión puede ocurrir después de construir el widget
		Van = BoundVan.Get();
	}
	if (Van)
	{
		RefreshSpeed(Van->GetSpeedKmh());
		const EDriveMode Mode = Van->GetDriveMode();
		if (!bHasDriveMode || Mode != LastDriveMode)
		{
			RefreshDriveMode(Mode);
		}
	}
	if (!BoundPursuit.IsValid())
	{
		BindPursuit();
	}
	if (const UPursuitDirectorSubsystem* Pursuit = BoundPursuit.Get())
	{
		RefreshHeat(Pursuit->GetHeat01());
	}
}

void UBlacklineHUDWidget::HandleCargoIntegrityChanged(float Integrity01, float Delta)
{
	RefreshCargo(Integrity01);
	const bool bCritical = Integrity01 <= CargoCriticalThreshold01;
	if (bCritical && !bCriticalLatched)
	{
		BP_OnCargoCritical(Integrity01);
	}
	bCriticalLatched = bCritical;
}

void UBlacklineHUDWidget::HandleCheckpointStateChanged(ECheckpointState OldState, ECheckpointState NewState)
{
	RefreshCheckpoint(NewState);
	BP_OnCheckpointStateChanged(NewState);
}

void UBlacklineHUDWidget::HandlePursuitStateChanged(EPursuitState NewState)
{
	if (const UPursuitDirectorSubsystem* Pursuit = BoundPursuit.Get())
	{
		RefreshHeat(Pursuit->GetHeat01());
	}
	BP_OnPursuitStateChanged(NewState);
}

void UBlacklineHUDWidget::RefreshSpeed(float SpeedKmh)
{
	const int32 Rounded = FMath::RoundToInt(FMath::Abs(SpeedKmh));
	if (Rounded == LastSpeedInt)
	{
		return;
	}
	LastSpeedInt = Rounded;
	if (SpeedText)
	{
		SpeedText->SetText(FText::Format(LOCTEXT("SpeedFmt", "{0} km/h"), FText::AsNumber(Rounded)));
	}
}

void UBlacklineHUDWidget::RefreshDriveMode(EDriveMode Mode)
{
	bHasDriveMode = true;
	LastDriveMode = Mode;
	if (DriveModeText)
	{
		DriveModeText->SetText(Mode == EDriveMode::FourWheel
			? LOCTEXT("Drive4x4", "4x4")
			: LOCTEXT("DriveRWD", "RWD"));
	}
}

void UBlacklineHUDWidget::RefreshCargo(float Integrity01)
{
	const float Clamped = FMath::Clamp(Integrity01, 0.f, 1.f);
	if (CargoIntegrityBar)
	{
		CargoIntegrityBar->SetPercent(Clamped);
	}
	if (CargoIntegrityText)
	{
		CargoIntegrityText->SetText(FText::Format(LOCTEXT("CargoFmt", "{0}%"),
			FText::AsNumber(FMath::RoundToInt(Clamped * 100.f))));
	}
}

void UBlacklineHUDWidget::RefreshCheckpoint(ECheckpointState State)
{
	if (!CheckpointStateText)
	{
		return;
	}
	FText Label;
	switch (State)
	{
	case ECheckpointState::Approaching:       Label = LOCTEXT("CpApproaching", "RETEN CERCA"); break;
	case ECheckpointState::Inspecting:        Label = LOCTEXT("CpInspecting", "INSPECCION"); break;
	case ECheckpointState::Cleared:           Label = LOCTEXT("CpCleared", "LIBRE"); break;
	case ECheckpointState::PursuitTriggered:  Label = LOCTEXT("CpPursuit", "PERSECUCION"); break;
	case ECheckpointState::Idle:
	default:                                  Label = FText::GetEmpty(); break;
	}
	CheckpointStateText->SetText(Label);
}

void UBlacklineHUDWidget::RefreshHeat(float Heat01)
{
	const float Clamped = FMath::Clamp(Heat01, 0.f, 1.f);
	if (FMath::IsNearlyEqual(Clamped, LastHeat01, 0.005f))
	{
		return;
	}
	LastHeat01 = Clamped;
	if (HeatBar)
	{
		HeatBar->SetPercent(Clamped);
	}
	if (HeatText)
	{
		HeatText->SetText(FText::Format(LOCTEXT("HeatFmt", "HEAT {0}%"),
			FText::AsNumber(FMath::RoundToInt(Clamped * 100.f))));
	}
}

#undef LOCTEXT_NAMESPACE
