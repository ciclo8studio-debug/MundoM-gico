#include "UI/BlacklineMinimapWidget.h"
#include "Fonts/FontMeasure.h"
#include "Fonts/SlateFontInfo.h"
#include "Framework/Application/SlateApplication.h"
#include "Rendering/DrawElements.h"
#include "Rendering/SlateRenderer.h"
#include "Styling/CoreStyle.h"
#include "Styling/SlateBrush.h"
#include "Vehicle/BlacklineVanPawn.h"
#include "World/ChileRouteActor.h"
#include "World/RoadsidePOI.h"

void UBlacklineMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ForceVolatile(true); // se repinta cada cuadro: sigue al furgon
}

int32 UBlacklineMinimapWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	int32 Layer = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	const ABlacklineVanPawn* Van = Cast<ABlacklineVanPawn>(GetOwningPlayerPawn());
	const AChileRouteActor* R = Route.Get();
	if (!Van || !R)
	{
		return Layer;
	}

	const FVector2f ScreenSize = AllottedGeometry.GetLocalSize();
	const FVector2f MapPos(ScreenSize.X - MapSizePx - MarginPx, MarginPx);
	const FVector2f Center = MapPos + FVector2f(MapSizePx * 0.5f, MapSizePx * 0.5f);
	const FSlateBrush* White = FCoreStyle::Get().GetBrush("GenericWhiteBox");

	auto Box = [&](const FVector2f& Pos, const FVector2f& Size, const FLinearColor& Color)
	{
		FSlateDrawElement::MakeBox(OutDrawElements, Layer, AllottedGeometry.ToPaintGeometry(Size, FSlateLayoutTransform(Pos)), White, ESlateDrawEffect::None, Color);
	};
	auto Text = [&](const FString& Str, const FVector2f& Pos, int32 Size, const FLinearColor& Color)
	{
		FSlateDrawElement::MakeText(OutDrawElements, Layer, AllottedGeometry.ToPaintGeometry(FVector2f(600.f, 40.f), FSlateLayoutTransform(Pos)),
			Str, FCoreStyle::GetDefaultFontStyle("Bold", Size), ESlateDrawEffect::None, Color);
	};

	// --- Fondo del minimapa
	Box(MapPos, FVector2f(MapSizePx, MapSizePx), FLinearColor(0.02f, 0.03f, 0.05f, 0.78f));
	++Layer;

	// --- Mundo -> minimapa (frente del furgon hacia arriba)
	const FVector VanPos = Van->GetActorLocation();
	const float Yaw = FMath::DegreesToRadians(static_cast<float>(Van->GetActorRotation().Yaw));
	const FVector2D Fwd(FMath::Cos(Yaw), FMath::Sin(Yaw));
	const FVector2D Rgt(-FMath::Sin(Yaw), FMath::Cos(Yaw));
	const float Scale = (MapSizePx * 0.5f) / RadiusCm;
	auto ToMap = [&](const FVector& P, bool& bInside) -> FVector2f
	{
		const FVector2D Rel(P.X - VanPos.X, P.Y - VanPos.Y);
		bInside = Rel.SizeSquared() < static_cast<double>(RadiusCm) * RadiusCm;
		return Center + FVector2f(static_cast<float>(FVector2D::DotProduct(Rel, Rgt)) * Scale, -static_cast<float>(FVector2D::DotProduct(Rel, Fwd)) * Scale);
	};

	// --- Ruta (tramos continuos dentro del radio)
	TArray<FVector2f> Run;
	auto Flush = [&]()
	{
		if (Run.Num() > 1)
		{
			FSlateDrawElement::MakeLines(OutDrawElements, Layer, AllottedGeometry.ToPaintGeometry(), Run, ESlateDrawEffect::None, FLinearColor(0.75f, 0.78f, 0.85f, 1.f), true, 3.f);
		}
		Run.Reset();
	};
	for (const FVector& P : R->GetMapPoints())
	{
		bool bIn = false;
		const FVector2f S = ToMap(P, bIn);
		if (bIn)
		{
			Run.Add(S);
		}
		else
		{
			Flush();
		}
	}
	Flush();
	++Layer;

	// --- Lugares y autos en contra
	for (const ARoadsidePOI* POI : R->GetPOIs())
	{
		bool bIn = false;
		if (POI)
		{
			const FVector2f S = ToMap(POI->GetActorLocation(), bIn);
			if (bIn)
			{
				Box(S - FVector2f(5.f, 5.f), FVector2f(10.f, 10.f), POI->GetMapColor());
			}
		}
	}
	TArray<FVector> Cars;
	R->GetTrafficLocations(Cars);
	for (const FVector& C : Cars)
	{
		bool bIn = false;
		const FVector2f S = ToMap(C, bIn);
		if (bIn)
		{
			Box(S - FVector2f(2.5f, 2.5f), FVector2f(5.f, 5.f), FLinearColor(1.f, 0.15f, 0.15f, 1.f));
		}
	}
	++Layer;

	// --- Furgon (flecha al centro)
	const TArray<FVector2f> Arrow = {
		Center + FVector2f(0.f, -10.f), Center + FVector2f(7.f, 8.f), Center + FVector2f(0.f, 3.f), Center + FVector2f(-7.f, 8.f), Center + FVector2f(0.f, -10.f)};
	FSlateDrawElement::MakeLines(OutDrawElements, Layer, AllottedGeometry.ToPaintGeometry(), Arrow, ESlateDrawEffect::None, FLinearColor(1.f, 0.55f, 0.1f, 1.f), true, 3.f);
	++Layer;

	// --- Velocidad, combustible, zona y avance
	const float BelowY = MapPos.Y + MapSizePx + 8.f;
	Text(FString::Printf(TEXT("%.0f km/h"), Van->GetSpeedKmh()), FVector2f(MapPos.X, BelowY), 26, FLinearColor::White);

	const float Fuel = Van->GetFuel01();
	const FLinearColor FuelColor = Fuel < 0.2f ? FLinearColor(1.f, 0.2f, 0.15f) : FLinearColor(0.95f, 0.8f, 0.15f);
	Box(FVector2f(MapPos.X, BelowY + 36.f), FVector2f(MapSizePx, 10.f), FLinearColor(0.f, 0.f, 0.f, 0.6f));
	Box(FVector2f(MapPos.X, BelowY + 36.f), FVector2f(MapSizePx * Fuel, 10.f), FuelColor);
	Text(TEXT("COMBUSTIBLE"), FVector2f(MapPos.X, BelowY + 48.f), 12, FLinearColor(0.85f, 0.85f, 0.85f, 1.f));
	Text(FString::Printf(TEXT("%s  %.0f%%"), *R->GetZoneName().ToString(), R->GetProgress01(VanPos) * 100.f), FVector2f(MapPos.X, BelowY + 66.f), 12, FLinearColor(0.7f, 0.85f, 1.f, 1.f));
	++Layer;

	// --- Mensaje del juego (centrado abajo)
	const FText Message = Van->GetHudMessage();
	if (!Message.IsEmpty())
	{
		const FSlateFontInfo Font = FCoreStyle::GetDefaultFontStyle("Bold", 22);
		const TSharedRef<FSlateFontMeasure> Measure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
		const FVector2f TextSize = FVector2f(Measure->Measure(Message.ToString(), Font));
		const FVector2f Pos((ScreenSize.X - TextSize.X) * 0.5f, ScreenSize.Y - 110.f);
		Box(Pos - FVector2f(14.f, 8.f), TextSize + FVector2f(28.f, 16.f), FLinearColor(0.f, 0.f, 0.f, 0.65f));
		++Layer;
		FSlateDrawElement::MakeText(OutDrawElements, Layer, AllottedGeometry.ToPaintGeometry(TextSize + FVector2f(20.f, 10.f), FSlateLayoutTransform(Pos)),
			Message.ToString(), Font, ESlateDrawEffect::None, FLinearColor(1.f, 0.95f, 0.8f, 1.f));
	}
	return Layer + 1;
}
