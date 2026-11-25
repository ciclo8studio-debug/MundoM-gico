#include "Contracts/BlacklineContractSubsystem.h"
#include "Blackline.h"
#include "Cargo/CargoSimulationComponent.h"
#include "Checkpoint/CheckpointController.h"
#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Vehicle/BlacklineVanPawn.h"

UBlacklineContractSettings::UBlacklineContractSettings()
{
	CategoryName = TEXT("Game");
	ContractTable = TSoftObjectPtr<UDataTable>(FSoftObjectPath(TEXT("/Game/Data/DT_BlacklineContracts.DT_BlacklineContracts")));
}

void UBlacklineContractSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	CachedTable = ResolveTable();
	DogWellbeing01 = StartDogWellbeing01;
	UE_LOG(LogBlackline, Log, TEXT("ContractSubsystem: tabla %s (%d contratos)"),
		CachedTable ? *CachedTable->GetName() : TEXT("NO ENCONTRADA"),
		CachedTable ? CachedTable->GetRowMap().Num() : 0);
}

UDataTable* UBlacklineContractSubsystem::ResolveTable() const
{
	const UBlacklineContractSettings* Settings = GetDefault<UBlacklineContractSettings>();
	return Settings ? Settings->ContractTable.LoadSynchronous() : nullptr;
}

TArray<FBlacklineContractRow> UBlacklineContractSubsystem::GetAllContracts() const
{
	TArray<FBlacklineContractRow> Out;
	if (CachedTable)
	{
		CachedTable->ForeachRow<FBlacklineContractRow>(TEXT("GetAllContracts"),
			[&Out](const FName&, const FBlacklineContractRow& Row) { Out.Add(Row); });
		Out.Sort([](const FBlacklineContractRow& A, const FBlacklineContractRow& B)
		{
			return A.ContractID.LexicalLess(B.ContractID);
		});
	}
	return Out;
}

TArray<FBlacklineContractRow> UBlacklineContractSubsystem::GetContractsByArc(int32 Arc) const
{
	return GetAllContracts().FilterByPredicate([Arc](const FBlacklineContractRow& R) { return R.Arc == Arc; });
}

bool UBlacklineContractSubsystem::GetContract(FName ContractID, FBlacklineContractRow& OutRow) const
{
	if (!CachedTable)
	{
		return false;
	}
	if (const FBlacklineContractRow* Row = CachedTable->FindRow<FBlacklineContractRow>(ContractID, TEXT("GetContract")))
	{
		OutRow = *Row;
		return true;
	}
	return false;
}

bool UBlacklineContractSubsystem::StartContract(FName ContractID)
{
	FBlacklineContractRow Row;
	if (!GetContract(ContractID, Row))
	{
		UE_LOG(LogBlackline, Warning, TEXT("StartContract: contrato %s no existe."), *ContractID.ToString());
		return false;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	// Cierra el anterior sin pagar.
	if (HasActiveContract())
	{
		FinishContract(false);
	}

	// Carga en el furgon del jugador.
	ABlacklineVanPawn* Van = Cast<ABlacklineVanPawn>(UGameplayStatics::GetPlayerPawn(World, 0));
	if (!Van || !Van->Cargo)
	{
		UE_LOG(LogBlackline, Warning, TEXT("StartContract: no hay furgon con componente de carga."));
		return false;
	}
	if (!Van->Cargo->LoadContract(Row))
	{
		return false;
	}
	Van->Cargo->OnCargoDestroyed.AddUniqueDynamic(this, &UBlacklineContractSubsystem::HandleCargoDestroyed);
	BoundCargo = Van->Cargo;

	// Exigencia documental en todos los retenes.
	for (TActorIterator<ACheckpointController> It(World); It; ++It)
	{
		It->ApplyContract(Row);
	}

	ActiveContractId = ContractID;
	ActivePayout = Row.Payout;
	OnContractStarted.Broadcast(ContractID);
	return true;
}

void UBlacklineContractSubsystem::FinishContract(bool bSuccess)
{
	if (!HasActiveContract())
	{
		return;
	}
	if (UCargoSimulationComponent* Cargo = BoundCargo.Get())
	{
		Cargo->OnCargoDestroyed.RemoveDynamic(this, &UBlacklineContractSubsystem::HandleCargoDestroyed);
		if (bSuccess)
		{
			// Pago proporcional a la integridad entregada.
			Credits += FMath::RoundToInt(static_cast<float>(ActivePayout) * Cargo->GetCargoIntegrity01());
		}
	}
	else if (bSuccess)
	{
		Credits += ActivePayout;
	}

	// Lo que esta en juego: Chuño. A tiempo gana; atrasado no gana; fallido pierde.
	const bool bLate = IsContractLate();
	if (bSuccess)
	{
		AddDogWellbeing(bLate ? 0.f : OnTimeDeliveryBonus);
	}
	else
	{
		AddDogWellbeing(-FailurePenalty);
	}
	if (UGameInstance* GI = GetGameInstance())
	{
		GI->GetTimerManager().ClearTimer(DeadlineTimer);
	}
	bHasDeadline = false;

	const FName Finished = ActiveContractId;
	ActiveContractId = NAME_None;
	ActivePayout = 0;
	BoundCargo.Reset();
	OnContractFinished.Broadcast(Finished);
}

void UBlacklineContractSubsystem::HandleCargoDestroyed()
{
	UE_LOG(LogBlackline, Log, TEXT("Contrato %s fallido: carga destruida."), *ActiveContractId.ToString());
	FinishContract(false);
}

void UBlacklineContractSubsystem::AddDogWellbeing(float Delta01)
{
	const float Old = DogWellbeing01;
	DogWellbeing01 = FMath::Clamp(DogWellbeing01 + Delta01, 0.f, 1.f);
	if (!FMath::IsNearlyEqual(Old, DogWellbeing01))
	{
		UE_LOG(LogBlackline, Log, TEXT("Chuño: bienestar %.0f%% -> %.0f%%"), Old * 100.f, DogWellbeing01 * 100.f);
		OnDogWellbeingChanged.Broadcast(DogWellbeing01);
	}
}

void UBlacklineContractSubsystem::StartDeadline(float Seconds)
{
	UGameInstance* GI = GetGameInstance();
	UWorld* World = GetWorld();
	if (!GI || !World)
	{
		return;
	}
	GI->GetTimerManager().ClearTimer(DeadlineTimer);
	bHasDeadline = true;
	DeadlineEndTime = World->GetTimeSeconds() + FMath::Max(10.f, Seconds);
	GI->GetTimerManager().SetTimer(DeadlineTimer, this, &UBlacklineContractSubsystem::HandleDeadlineExpired, FMath::Max(10.f, Seconds), false);
}

float UBlacklineContractSubsystem::GetDeadlineRemainingSeconds() const
{
	const UWorld* World = GetWorld();
	if (!bHasDeadline || !World)
	{
		return 0.f;
	}
	return static_cast<float>(DeadlineEndTime - World->GetTimeSeconds());
}

void UBlacklineContractSubsystem::HandleDeadlineExpired()
{
	if (HasActiveContract())
	{
		UE_LOG(LogBlackline, Log, TEXT("Contrato %s: plazo vencido."), *ActiveContractId.ToString());
		AddDogWellbeing(-LatePenalty);
	}
}
