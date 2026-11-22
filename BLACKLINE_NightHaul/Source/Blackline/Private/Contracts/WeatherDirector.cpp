#include "Contracts/WeatherDirector.h"
#include "Blackline.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Contracts/BlacklineContractSubsystem.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/GameInstance.h"
#include "EngineUtils.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Materials/MaterialParameterCollection.h"

namespace BlacklineWeatherPrivate
{
	FBlacklineWeatherPreset Make(float Rain, float Wet, float Snow, float Wind, float Fog, bool bVol)
	{
		FBlacklineWeatherPreset P;
		P.RainIntensity01 = Rain;
		P.Wetness01 = Wet;
		P.SnowIntensity01 = Snow;
		P.Wind01 = Wind;
		P.FogDensity = Fog;
		P.bVolumetricFog = bVol;
		return P;
	}
}

AWeatherDirector::AWeatherDirector()
{
	PrimaryActorTick.bCanEverTick = false;
	BuildDefaultPresets();
}

void AWeatherDirector::BuildDefaultPresets()
{
	using namespace BlacklineWeatherPrivate;
	Presets.Reset();
	//                                                   lluvia mojado nieve viento niebla  vol
	Presets.Add(EBlacklineWeather::ClearNight,   Make(0.0f, 0.0f, 0.0f, 0.1f, 0.010f, false));
	Presets.Add(EBlacklineWeather::ColdNight,    Make(0.0f, 0.1f, 0.0f, 0.2f, 0.015f, false));
	Presets.Add(EBlacklineWeather::HumidNight,   Make(0.0f, 0.4f, 0.0f, 0.1f, 0.030f, false));
	Presets.Add(EBlacklineWeather::LightRain,    Make(0.3f, 0.6f, 0.0f, 0.2f, 0.020f, false));
	Presets.Add(EBlacklineWeather::ModerateRain, Make(0.6f, 0.8f, 0.0f, 0.3f, 0.025f, false));
	Presets.Add(EBlacklineWeather::HeavyRain,    Make(0.9f, 1.0f, 0.0f, 0.4f, 0.030f, false));
	Presets.Add(EBlacklineWeather::Storm,        Make(1.0f, 1.0f, 0.0f, 0.9f, 0.035f, false));
	Presets.Add(EBlacklineWeather::Windy,        Make(0.0f, 0.1f, 0.0f, 0.8f, 0.010f, false));
	Presets.Add(EBlacklineWeather::UrbanFog,     Make(0.0f, 0.3f, 0.0f, 0.1f, 0.060f, true));
	Presets.Add(EBlacklineWeather::CoastalFog,   Make(0.0f, 0.4f, 0.0f, 0.2f, 0.050f, true));
	Presets.Add(EBlacklineWeather::PortFog,      Make(0.0f, 0.5f, 0.0f, 0.2f, 0.070f, true));
	Presets.Add(EBlacklineWeather::DenseFog,     Make(0.0f, 0.4f, 0.0f, 0.1f, 0.100f, true));
	Presets.Add(EBlacklineWeather::Freezing,     Make(0.0f, 0.2f, 0.1f, 0.3f, 0.020f, false));
	Presets.Add(EBlacklineWeather::Blizzard,     Make(0.0f, 0.2f, 1.0f, 1.0f, 0.080f, true));
}

FBlacklineWeatherPreset AWeatherDirector::GetPreset(EBlacklineWeather Weather) const
{
	if (const FBlacklineWeatherPreset* P = Presets.Find(Weather))
	{
		return *P;
	}
	return FBlacklineWeatherPreset();
}

void AWeatherDirector::BeginPlay()
{
	Super::BeginPlay();

	ApplyWeather(DefaultWeather);

	if (bFollowActiveContract)
	{
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UBlacklineContractSubsystem* Contracts = GI->GetSubsystem<UBlacklineContractSubsystem>())
			{
				Contracts->OnContractStarted.AddUniqueDynamic(this, &AWeatherDirector::HandleContractStarted);
				if (Contracts->HasActiveContract())
				{
					HandleContractStarted(Contracts->GetActiveContractId());
				}
			}
		}
	}
}

void AWeatherDirector::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UBlacklineContractSubsystem* Contracts = GI->GetSubsystem<UBlacklineContractSubsystem>())
		{
			Contracts->OnContractStarted.RemoveDynamic(this, &AWeatherDirector::HandleContractStarted);
		}
	}
	Super::EndPlay(EndPlayReason);
}

void AWeatherDirector::HandleContractStarted(FName ContractID)
{
	UGameInstance* GI = GetGameInstance();
	UBlacklineContractSubsystem* Contracts = GI ? GI->GetSubsystem<UBlacklineContractSubsystem>() : nullptr;
	FBlacklineContractRow Row;
	if (Contracts && Contracts->GetContract(ContractID, Row))
	{
		ApplyWeather(Row.WeatherType);
	}
}

void AWeatherDirector::ApplyWeather(EBlacklineWeather Weather)
{
	CurrentWeather = Weather;
	const FBlacklineWeatherPreset Preset = GetPreset(Weather);

	// Materiales (asfalto mojado, lluvia en parabrisas, etc.).
	if (UMaterialParameterCollection* MPC = WeatherCollection.LoadSynchronous())
	{
		UKismetMaterialLibrary::SetScalarParameterValue(this, MPC, TEXT("RainIntensity"), Preset.RainIntensity01);
		UKismetMaterialLibrary::SetScalarParameterValue(this, MPC, TEXT("Wetness"), Preset.Wetness01);
		UKismetMaterialLibrary::SetScalarParameterValue(this, MPC, TEXT("SnowIntensity"), Preset.SnowIntensity01);
		UKismetMaterialLibrary::SetScalarParameterValue(this, MPC, TEXT("Wind"), Preset.Wind01);
	}

	// Niebla del nivel.
	if (UWorld* World = GetWorld())
	{
		for (TActorIterator<AExponentialHeightFog> It(World); It; ++It)
		{
			if (UExponentialHeightFogComponent* Fog = It->GetComponent())
			{
				Fog->SetFogDensity(Preset.FogDensity);
				Fog->SetVolumetricFog(Preset.bVolumetricFog);
			}
		}
	}

	UE_LOG(LogBlackline, Log, TEXT("Clima: %s (lluvia %.2f, mojado %.2f, nieve %.2f, niebla %.3f)"),
		*UEnum::GetValueAsString(Weather), Preset.RainIntensity01, Preset.Wetness01, Preset.SnowIntensity01, Preset.FogDensity);

	OnWeatherChanged.Broadcast(Weather, Preset);
}
