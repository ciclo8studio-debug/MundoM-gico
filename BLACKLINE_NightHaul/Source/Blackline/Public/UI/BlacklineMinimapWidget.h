// Blackline: Night Haul - minimapa y datos de manejo dibujados directamente (sin Blueprint):
// ruta alrededor del furgon con el frente hacia arriba, bencinera/restaurante, autos en contra, velocidad, combustible y mensajes.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BlacklineMinimapWidget.generated.h"

class AChileRouteActor;

UCLASS()
class BLACKLINE_API UBlacklineMinimapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetRoute(AChileRouteActor* InRoute) { Route = InRoute; }

	/** Lado del minimapa en pixeles. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap", meta = (ClampMin = "100"))
	float MapSizePx = 240.f;

	/** Radio de mundo que muestra el minimapa (cm). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap", meta = (ClampMin = "10000", Units = "cm"))
	float RadiusCm = 150000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	float MarginPx = 20.f;

protected:
	virtual void NativeConstruct() override;
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

private:
	TWeakObjectPtr<AChileRouteActor> Route;
};
