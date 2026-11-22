// Blackline: Night Haul - Prompt de interacción con botones del tablero. Sin Tick.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/BlacklineTypes.h"
#include "InteractPromptWidget.generated.h"

class UTextBlock;
class ABlacklineVanPawn;

UCLASS(Abstract, Blueprintable)
class BLACKLINE_API UInteractPromptWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Texto de la tecla/botón (p. ej. "E"). Editable por diseño / rebind. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Prompt")
	FText InteractKeyLabel;

	/** Lo llama quien hace la traza "Interact" al apuntar a un botón del tablero. */
	UFUNCTION(BlueprintCallable, Category = "Blackline|Prompt")
	void ShowForControl(EDashControl Control);

	UFUNCTION(BlueprintCallable, Category = "Blackline|Prompt")
	void HidePrompt();

	UFUNCTION(BlueprintPure, Category = "Blackline|Prompt")
	bool IsPromptActive() const { return bActive; }

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> KeyText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ActionText;

private:
	UFUNCTION()
	void HandleDashControlChanged(EDashControl Control, bool bOn);

	void Refresh();
	FText GetControlName(EDashControl Control) const;

	TWeakObjectPtr<ABlacklineVanPawn> BoundVan;
	EDashControl FocusedControl = EDashControl::Headlights;
	bool bActive = false;
};
