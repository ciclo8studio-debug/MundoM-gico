// Blackline: Night Haul - tablet de encargos de la cabina. Base C++ para WBP_ContractTablet.
// Nombres de widgets (BindWidgetOptional): TitleText, BrokerText, CargoText, PayoutText, WeightText, FragilityText,
// EMText, DocLevelText, WeatherText, TerrainText, CoverStoryText, NoteText, CreditsText, StatusText.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/BlacklineContractTypes.h"
#include "ContractTabletWidget.generated.h"

class UTextBlock;
class UBlacklineContractSubsystem;

UCLASS(Abstract)
class BLACKLINE_API UContractTabletWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** IDs de los contratos de un arco (1-5) para que el BP arme la lista. Arc=0 devuelve todos. */
	UFUNCTION(BlueprintCallable, Category = "Tablet")
	TArray<FName> GetContractIdsForArc(int32 Arc) const;

	/** Muestra el detalle del contrato (sin aceptarlo). */
	UFUNCTION(BlueprintCallable, Category = "Tablet")
	bool SelectContract(FName ContractID);

	/** Acepta el contrato seleccionado: carga la carga y exige documentos en retenes. */
	UFUNCTION(BlueprintCallable, Category = "Tablet")
	bool AcceptSelected();

	UFUNCTION(BlueprintPure, Category = "Tablet")
	FName GetSelectedContractId() const { return SelectedId; }

	UFUNCTION(BlueprintPure, Category = "Tablet")
	bool HasSelection() const { return !SelectedId.IsNone(); }

	/** Refresca todos los textos. */
	UFUNCTION(BlueprintCallable, Category = "Tablet")
	void RefreshAll();

	/** Para animaciones/sonidos en el BP. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Tablet")
	void BP_OnSelectionChanged(FName ContractID);

	UFUNCTION(BlueprintImplementableEvent, Category = "Tablet")
	void BP_OnContractAccepted(FName ContractID);

	UFUNCTION(BlueprintImplementableEvent, Category = "Tablet")
	void BP_OnContractFinished(FName ContractID);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UTextBlock> TitleText;
	UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UTextBlock> BrokerText;
	UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UTextBlock> CargoText;
	UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UTextBlock> PayoutText;
	UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UTextBlock> WeightText;
	UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UTextBlock> FragilityText;
	UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UTextBlock> EMText;
	UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UTextBlock> DocLevelText;
	UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UTextBlock> WeatherText;
	UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UTextBlock> TerrainText;
	UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UTextBlock> CoverStoryText;
	UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UTextBlock> NoteText;
	UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UTextBlock> CreditsText;
	UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UTextBlock> StatusText;

private:
	UBlacklineContractSubsystem* GetContracts() const;

	UFUNCTION()
	void HandleContractStarted(FName ContractID);

	UFUNCTION()
	void HandleContractFinished(FName ContractID);

	static void SetText(UTextBlock* Block, const FText& Text);

	FName SelectedId = NAME_None;
};
