#include "Contracts/DeliveryZone.h"
#include "Blackline.h"
#include "Components/BoxComponent.h"
#include "Contracts/BlacklineContractSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Pursuit/PursuitDirectorSubsystem.h"
#include "TimerManager.h"
#include "Vehicle/BlacklineVanPawn.h"

#define LOCTEXT_NAMESPACE "BlacklineDelivery"

ADeliveryZone::ADeliveryZone()
{
	PrimaryActorTick.bCanEverTick = false;

	Zone = CreateDefaultSubobject<UBoxComponent>(TEXT("Zone"));
	SetRootComponent(Zone);
	Zone->SetBoxExtent(FVector(800.f, 600.f, 300.f));
	Zone->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Zone->SetGenerateOverlapEvents(true);
}

void ADeliveryZone::BeginPlay()
{
	Super::BeginPlay();
	Zone->OnComponentBeginOverlap.AddDynamic(this, &ADeliveryZone::OnZoneBeginOverlap);
	Zone->OnComponentEndOverlap.AddDynamic(this, &ADeliveryZone::OnZoneEndOverlap);
}

void ADeliveryZone::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopPolling();
	Super::EndPlay(EndPlayReason);
}

void ADeliveryZone::OnZoneBeginOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	ABlacklineVanPawn* Candidate = Cast<ABlacklineVanPawn>(OtherActor);
	if (!Candidate)
	{
		return;
	}
	Van = Candidate;
	StoppedSeconds = 0.f;
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(PollTimer, this, &ADeliveryZone::Poll, FMath::Max(0.05f, PollIntervalSeconds), true);
	}
}

void ADeliveryZone::OnZoneEndOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32)
{
	if (OtherActor == Van)
	{
		StopPolling();
		Van = nullptr;
	}
}

void ADeliveryZone::StopPolling()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PollTimer);
	}
}

void ADeliveryZone::Poll()
{
	UWorld* World = GetWorld();
	UGameInstance* GI = World ? World->GetGameInstance() : nullptr;
	UBlacklineContractSubsystem* Contracts = GI ? GI->GetSubsystem<UBlacklineContractSubsystem>() : nullptr;
	if (!Van || !Contracts || !Contracts->HasActiveContract())
	{
		return;
	}
	if (!RequiredContractId.IsNone() && Contracts->GetActiveContractId() != RequiredContractId)
	{
		return;
	}

	auto Block = [this, World](const FText& Reason)
	{
		// Avisa como maximo cada 4 s para no inundar la UI.
		if (World->GetTimeSeconds() - LastBlockedMessageTime > 4.f)
		{
			LastBlockedMessageTime = World->GetTimeSeconds();
			OnDeliveryBlocked.Broadcast(Reason);
		}
		StoppedSeconds = 0.f;
	};

	if (bBlockDuringPursuit)
	{
		const UPursuitDirectorSubsystem* Pursuit = UPursuitDirectorSubsystem::Get(this);
		if (Pursuit && (Pursuit->GetState() == EPursuitState::Hunting || Pursuit->GetState() == EPursuitState::Searching))
		{
			Block(LOCTEXT("BlockedPursuit", "Te siguen: pierde a las patrullas antes de entregar."));
			return;
		}
	}

	if (bRequireStop)
	{
		if (Van->GetSpeedKmh() > StopSpeedKmh)
		{
			StoppedSeconds = 0.f;
			return;
		}
		StoppedSeconds += PollIntervalSeconds;
		if (StoppedSeconds < StopHoldSeconds)
		{
			return;
		}
	}

	const FName Id = Contracts->GetActiveContractId();
	Contracts->FinishContract(true);
	StopPolling();
	UE_LOG(LogBlackline, Log, TEXT("Entrega completada: contrato %s"), *Id.ToString());
	OnDelivered.Broadcast(Id);
}

#undef LOCTEXT_NAMESPACE
