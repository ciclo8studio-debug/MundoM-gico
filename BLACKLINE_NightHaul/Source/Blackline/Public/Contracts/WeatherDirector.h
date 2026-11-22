// Blackline: Night Haul - director de clima. Aplica el WeatherType del contrato activo:
// parametros del MPC_Weather (materiales de asfalto mojado, lluvia), densidad de la niebla y evento para Niagara/MetaSounds.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/BlacklineContractTypes.h"
#include "WeatherDirector.generated.h"

class UMaterialParameterCollection;

USTRUCT(BlueprintType)
struct BLACKLINE_API FBlacklineWeatherPreset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather", meta = (ClampMin = "0", ClampMax = "1"))
	float RainIntensity01 = 0.f;

	/** Suelo mojado: alimenta rugosidad/charcos del material de asfalto. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather", meta = (ClampMin = "0", ClampMax = "1"))
	float Wetness01 = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather", meta = (ClampMin = "0", ClampMax = "1"))
	float SnowIntensity01 = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather", meta = (ClampMin = "0", ClampMax = "1"))
	float Wind01 = 0.1f;

	/** Densidad de ExponentialHeightFog. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather", meta = (ClampMin = "0"))
	float FogDensity = 0.02f;

	/** Niebla volumetrica solo donde aporta (costo de GPU). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	bool bVolumetricFog = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeatherChanged, EBlacklineWeather, Weather, FBlacklineWeatherPreset, Preset);

UCLASS()
class BLACKLINE_API AWeatherDirector : public AActor
{
	GENERATED_BODY()

public:
	AWeatherDirector();

	/** Aplica un clima ya (sin contrato). */
	UFUNCTION(BlueprintCallable, Category = "Weather")
	void ApplyWeather(EBlacklineWeather Weather);

	UFUNCTION(BlueprintPure, Category = "Weather")
	EBlacklineWeather GetCurrentWeather() const { return CurrentWeather; }

	UFUNCTION(BlueprintPure, Category = "Weather")
	FBlacklineWeatherPreset GetPreset(EBlacklineWeather Weather) const;

	/** Preset por clima (editable en el BP). Se rellena con defaults en el constructor. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	TMap<EBlacklineWeather, FBlacklineWeatherPreset> Presets;

	/** Coleccion de parametros de materiales (MPC_Weather). Parametros: RainIntensity, Wetness, SnowIntensity, Wind. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	TSoftObjectPtr<UMaterialParameterCollection> WeatherCollection;

	/** Clima inicial si no hay contrato activo. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	EBlacklineWeather DefaultWeather = EBlacklineWeather::ClearNight;

	/** Escucha el subsistema de contratos y cambia el clima al iniciar un contrato. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	bool bFollowActiveContract = true;

	UPROPERTY(BlueprintAssignable, Category = "Weather")
	FOnWeatherChanged OnWeatherChanged;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void BuildDefaultPresets();

	UFUNCTION()
	void HandleContractStarted(FName ContractID);

	EBlacklineWeather CurrentWeather = EBlacklineWeather::ClearNight;
};
