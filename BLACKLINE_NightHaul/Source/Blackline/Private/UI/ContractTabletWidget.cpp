#include "UI/ContractTabletWidget.h"
#include "Components/TextBlock.h"
#include "Contracts/BlacklineContractSubsystem.h"
#include "Engine/GameInstance.h"

#define LOCTEXT_NAMESPACE "BlacklineTablet"

void UContractTabletWidget::SetText(UTextBlock* Block, const FText& Text)
{
	if (Block)
	{
		Block->SetText(Text);
	}
}

UBlacklineContractSubsystem* UContractTabletWidget::GetContracts() const
{
	const UGameInstance* GI = GetGameInstance();
	return GI ? GI->GetSubsystem<UBlacklineContractSubsystem>() : nullptr;
}

void UContractTabletWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (UBlacklineContractSubsystem* Contracts = GetContracts())
	{
		Contracts->OnContractStarted.AddUniqueDynamic(this, &UContractTabletWidget::HandleContractStarted);
		Contracts->OnContractFinished.AddUniqueDynamic(this, &UContractTabletWidget::HandleContractFinished);
		if (Contracts->HasActiveContract())
		{
			SelectedId = Contracts->GetActiveContractId();
		}
	}
	RefreshAll();
}

void UContractTabletWidget::NativeDestruct()
{
	if (UBlacklineContractSubsystem* Contracts = GetContracts())
	{
		Contracts->OnContractStarted.RemoveDynamic(this, &UContractTabletWidget::HandleContractStarted);
		Contracts->OnContractFinished.RemoveDynamic(this, &UContractTabletWidget::HandleContractFinished);
	}
	Super::NativeDestruct();
}

TArray<FName> UContractTabletWidget::GetContractIdsForArc(int32 Arc) const
{
	TArray<FName> Ids;
	if (const UBlacklineContractSubsystem* Contracts = GetContracts())
	{
		const TArray<FBlacklineContractRow> Rows = (Arc <= 0) ? Contracts->GetAllContracts() : Contracts->GetContractsByArc(Arc);
		for (const FBlacklineContractRow& Row : Rows)
		{
			Ids.Add(Row.ContractID);
		}
	}
	return Ids;
}

bool UContractTabletWidget::SelectContract(FName ContractID)
{
	const UBlacklineContractSubsystem* Contracts = GetContracts();
	FBlacklineContractRow Row;
	if (!Contracts || !Contracts->GetContract(ContractID, Row))
	{
		return false;
	}
	SelectedId = ContractID;
	RefreshAll();
	BP_OnSelectionChanged(ContractID);
	return true;
}

bool UContractTabletWidget::AcceptSelected()
{
	UBlacklineContractSubsystem* Contracts = GetContracts();
	if (!Contracts || SelectedId.IsNone())
	{
		SetText(StatusText, LOCTEXT("NoSelection", "Selecciona un encargo."));
		return false;
	}
	if (!Contracts->StartContract(SelectedId))
	{
		SetText(StatusText, LOCTEXT("StartFailed", "No se pudo aceptar el encargo."));
		return false;
	}
	return true; // HandleContractStarted refresca y avisa
}

void UContractTabletWidget::HandleContractStarted(FName ContractID)
{
	SelectedId = ContractID;
	RefreshAll();
	BP_OnContractAccepted(ContractID);
}

void UContractTabletWidget::HandleContractFinished(FName ContractID)
{
	RefreshAll();
	BP_OnContractFinished(ContractID);
}

void UContractTabletWidget::RefreshAll()
{
	const UBlacklineContractSubsystem* Contracts = GetContracts();
	if (!Contracts)
	{
		return;
	}

	SetText(CreditsText, FText::Format(LOCTEXT("Credits", "{0} cr"), FText::AsNumber(Contracts->GetCredits())));

	FBlacklineContractRow Row;
	if (SelectedId.IsNone() || !Contracts->GetContract(SelectedId, Row))
	{
		SetText(StatusText, LOCTEXT("Idle", "Sin encargo seleccionado."));
		return;
	}

	SetText(TitleText, Row.Title);
	SetText(BrokerText, Row.BrokerName);
	SetText(CargoText, Row.CargoDisplayName);
	SetText(PayoutText, FText::Format(LOCTEXT("Payout", "{0} cr"), FText::AsNumber(Row.Payout)));
	SetText(WeightText, FText::Format(LOCTEXT("Weight", "{0} kg"), FText::AsNumber(FMath::RoundToInt(Row.CargoWeightKg))));
	SetText(FragilityText, FText::Format(LOCTEXT("Fragility", "{0}%"), FText::AsNumber(FMath::RoundToInt(Row.Fragility * 100.f))));
	SetText(EMText, FText::Format(LOCTEXT("EM", "{0}%"), FText::AsNumber(FMath::RoundToInt(Row.EMSignature * 100.f))));
	SetText(DocLevelText, FText::Format(LOCTEXT("Doc", "Nivel {0}"), FText::AsNumber(Row.RequiredManifestLevel)));
	SetText(WeatherText, StaticEnum<EBlacklineWeather>()->GetDisplayNameTextByValue(static_cast<int64>(Row.WeatherType)));
	SetText(TerrainText, StaticEnum<EBlacklineTerrainType>()->GetDisplayNameTextByValue(static_cast<int64>(Row.TerrainType)));
	SetText(CoverStoryText, Row.CoverStoryManifest);
	SetText(NoteText, Row.DesignNote);

	const bool bActive = Contracts->HasActiveContract() && Contracts->GetActiveContractId() == SelectedId;
	SetText(StatusText, bActive ? LOCTEXT("Active", "En curso") : LOCTEXT("Available", "Disponible"));
}

#undef LOCTEXT_NAMESPACE
