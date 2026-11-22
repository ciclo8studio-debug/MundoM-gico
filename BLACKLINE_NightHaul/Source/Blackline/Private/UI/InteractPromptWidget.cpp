#include "UI/InteractPromptWidget.h"

#include "Components/TextBlock.h"
#include "Vehicle/BlacklineVanPawn.h"

#define LOCTEXT_NAMESPACE "BlacklineInteract"

void UInteractPromptWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (InteractKeyLabel.IsEmpty())
	{
		InteractKeyLabel = LOCTEXT("DefaultKey", "E");
	}

	if (ABlacklineVanPawn* Van = Cast<ABlacklineVanPawn>(GetOwningPlayerPawn()))
	{
		BoundVan = Van;
		Van->OnDashControlChanged.AddUniqueDynamic(this, &UInteractPromptWidget::HandleDashControlChanged);
	}

	SetVisibility(ESlateVisibility::Collapsed);
}

void UInteractPromptWidget::NativeDestruct()
{
	if (ABlacklineVanPawn* Van = BoundVan.Get())
	{
		Van->OnDashControlChanged.RemoveDynamic(this, &UInteractPromptWidget::HandleDashControlChanged);
	}
	BoundVan.Reset();
	Super::NativeDestruct();
}

void UInteractPromptWidget::ShowForControl(EDashControl Control)
{
	FocusedControl = Control;
	bActive = true;
	Refresh();
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UInteractPromptWidget::HidePrompt()
{
	bActive = false;
	SetVisibility(ESlateVisibility::Collapsed);
}

void UInteractPromptWidget::HandleDashControlChanged(EDashControl Control, bool bOn)
{
	if (bActive && Control == FocusedControl)
	{
		Refresh();
	}
}

void UInteractPromptWidget::Refresh()
{
	if (KeyText)
	{
		KeyText->SetText(InteractKeyLabel);
	}
	if (ActionText)
	{
		ActionText->SetText(GetControlName(FocusedControl));
	}
}

FText UInteractPromptWidget::GetControlName(EDashControl Control) const
{
	switch (Control)
	{
	case EDashControl::Headlights:   return LOCTEXT("Headlights", "Faros");
	case EDashControl::Wipers:       return LOCTEXT("Wipers", "Limpiaparabrisas");
	case EDashControl::Window:       return LOCTEXT("Window", "Ventanilla");
	case EDashControl::SignalJammer: return LOCTEXT("Jammer", "Inhibidor de senal");
	case EDashControl::DiffLock:     return LOCTEXT("DiffLock", "Bloqueo de diferencial");
	case EDashControl::Ignition:     return LOCTEXT("Ignition", "Encendido");
	default:                         return FText::GetEmpty();
	}
}

#undef LOCTEXT_NAMESPACE
