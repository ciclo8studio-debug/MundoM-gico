#include "UI/BlacklineMinimapWidget.h"
#include "Fonts/FontMeasure.h"
#include "Fonts/SlateFontInfo.h"
#include "Framework/Application/SlateApplication.h"
#include "Rendering/DrawElements.h"
#include "Rendering/SlateRenderer.h"
#include "Styling/CoreStyle.h"
#include "Styling/SlateBrush.h"
#include "Cargo/CargoSimulationComponent.h"
#include "Contracts/BlacklineContractSubsystem.h"
#include "Engine/GameInstance.h"
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

	// --- Encargo de La Cofradia y Pancito (arriba a la izquierda)
	if (const UGameInstance* GI = GetGameInstance())
	{
		if (const UBlacklineContractSubsystem* Contracts = GI->GetSubsystem<UBlacklineContractSubsystem>())
		{
			const FVector2f P(MarginPx, MarginPx);
			Box(P - FVector2f(8.f, 6.f), FVector2f(380.f, 170.f), FLinearColor(0.f, 0.f, 0.f, 0.58f));
			++Layer;
			FBlacklineContractRow Row;
			if (Contracts->HasActiveContract() && Contracts->GetContract(Contracts->GetActiveContractId(), Row))
			{
				Text(FString::Printf(TEXT("ENCARGO: %s"), *Row.Title.ToString()), P, 16, FLinearColor(1.f, 0.85f, 0.4f, 1.f));
				Text(FString::Printf(TEXT("%s  -  %.0f kg"), *Row.CargoDisplayName.ToString(), Row.CargoWeightKg), P + FVector2f(0.f, 24.f), 12, FLinearColor::White);
				const float Integrity = Van->Cargo ? Van->Cargo->GetCargoIntegrity01() : 1.f;
				Text(FString::Printf(TEXT("Carga: %.0f%%"), Integrity * 100.f), P + FVector2f(0.f, 42.f), 12,
					Integrity < 0.5f ? FLinearColor(1.f, 0.4f, 0.3f, 1.f) : FLinearColor(0.7f, 1.f, 0.7f, 1.f));

				if (Contracts->HasDeadline())
				{
					const float Left = Contracts->GetDeadlineRemainingSeconds();
					const int32 Abs = FMath::Abs(FMath::RoundToInt(Left));
					const bool bLate = Left < 0.f;
					Text(FString::Printf(TEXT("%s %02d:%02d"), bLate ? TEXT("ATRASADO") : TEXT("Plazo"), Abs / 60, Abs % 60), P + FVector2f(150.f, 42.f), 12,
						(bLate || Left < 60.f) ? FLinearColor(1.f, 0.35f, 0.25f, 1.f) : FLinearColor(0.85f, 0.85f, 0.85f, 1.f));
				}
			}
			else
			{
				Text(TEXT("Sin encargo activo"), P, 14, FLinearColor(0.8f, 0.8f, 0.8f, 1.f));
			}

			// Estado del furgon (daño de los choques) y paciencia de La Cofradia (lo que esta en juego).
			const float Body = 1.f - Van->GetDamage01();
			const FLinearColor BodyColor = Body < 0.3f ? FLinearColor(1.f, 0.25f, 0.2f, 1.f) : (Body < 0.6f ? FLinearColor(1.f, 0.7f, 0.2f, 1.f) : FLinearColor(0.6f, 0.85f, 1.f, 1.f));
			Text(FString::Printf(TEXT("FURGON  %.0f%%"), Body * 100.f), P + FVector2f(0.f, 62.f), 13, BodyColor);
			Box(P + FVector2f(0.f, 82.f), FVector2f(360.f, 8.f), FLinearColor(0.f, 0.f, 0.f, 0.6f));
			Box(P + FVector2f(0.f, 82.f), FVector2f(360.f * Body, 8.f), BodyColor);

			const float Patience = Contracts->GetDogWellbeing01();
			const FLinearColor PatColor = Patience < 0.3f ? FLinearColor(1.f, 0.25f, 0.2f, 1.f) : (Patience < 0.6f ? FLinearColor(1.f, 0.7f, 0.2f, 1.f) : FLinearColor(0.45f, 0.95f, 0.5f, 1.f));
			Text(FString::Printf(TEXT("LA COFRADIA - paciencia  %.0f%%"), Patience * 100.f), P + FVector2f(0.f, 96.f), 13, PatColor);
			Box(P + FVector2f(0.f, 116.f), FVector2f(360.f, 8.f), FLinearColor(0.f, 0.f, 0.f, 0.6f));
			Box(P + FVector2f(0.f, 116.f), FVector2f(360.f * Patience, 8.f), PatColor);
			Text(FString::Printf(TEXT("Viaticos: %d cr"), Contracts->GetCredits()), P + FVector2f(0.f, 132.f), 12, FLinearColor(0.7f, 0.85f, 1.f, 1.f));
		}
	}
	++Layer;

	// --- Campaña: los 5 contratos, el actual resaltado (F5/F6 para elegir)
	if (const UGameInstance* CampGI = GetGameInstance())
	{
		if (const UBlacklineContractSubsystem* Camp = CampGI->GetSubsystem<UBlacklineContractSubsystem>())
		{
			FString Line = TEXT("CAMPAÑA:  ");
			for (int32 i = 0; i < Camp->CampaignOrder.Num(); ++i)
			{
				const bool bNow = (i == Camp->GetCampaignIndex());
				Line += FString::Printf(TEXT("%s%s%s%s"), bNow ? TEXT("[") : TEXT(""), *Camp->CampaignOrder[i].ToString(), bNow ? TEXT("]") : TEXT(""),
					i + 1 < Camp->CampaignOrder.Num() ? TEXT("  >  ") : TEXT(""));
			}
			Text(Line, FVector2f(MarginPx, ScreenSize.Y - 118.f), 12, FLinearColor(0.85f, 0.7f, 0.25f, 0.95f));
		}
	}
	++Layer;

	// --- Controles (abajo a la izquierda); H los oculta
	if (bShowHelp)
	{
		const FLinearColor Dim(0.85f, 0.85f, 0.9f, 0.85f);
		const float Y0 = ScreenSize.Y - 96.f;
		Text(TEXT("W/S acelerar-frenar   A/D girar   Espacio freno   V camara   F 4x4"), FVector2f(MarginPx, Y0), 11, Dim);
		Text(TEXT("L luces   I motor   J inhibidor   E interactuar   R volver a la carretera   F5/F6 contrato anterior/siguiente   H ocultar ayuda   Esc pausa"), FVector2f(MarginPx, Y0 + 16.f), 11, Dim);
		Text(TEXT("Mando: R2/L2 acelerar-frenar  stick girar  Cuadrado freno  Triangulo camara  L1 4x4  Cruceta: luces/motor/inhibidor"), FVector2f(MarginPx, Y0 + 32.f), 11, Dim);
	}
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
