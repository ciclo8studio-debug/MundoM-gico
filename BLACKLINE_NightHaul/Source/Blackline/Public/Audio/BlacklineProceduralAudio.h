// Blackline: Night Haul - audio minimo sintetizado (sin assets): motor segun RPM, lluvia, viento y sirena.
// Una sola onda procedural mono mezcla las voces; los parametros se actualizan 10 veces por segundo (sin Tick).
#pragma once

#include <atomic>
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BlacklineProceduralAudio.generated.h"

class USoundWaveProcedural;
class UAudioComponent;

UCLASS(ClassGroup = (Blackline), meta = (BlueprintSpawnableComponent))
class BLACKLINE_API UBlacklineProceduralAudio : public UActorComponent
{
	GENERATED_BODY()

public:
	UBlacklineProceduralAudio();

	/** Volumen general (0..1). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (ClampMin = "0", ClampMax = "1"))
	float MasterVolume = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	bool bEnabled = true;

	/** Frecuencia fundamental del motor en ralenti y a maximas RPM (Hz). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Motor", meta = (ClampMin = "10"))
	float IdleFrequencyHz = 26.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Motor", meta = (ClampMin = "20"))
	float MaxFrequencyHz = 120.f;

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetEngine(float Rpm01, float Throttle01, bool bEngineOn);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetRain(float Intensity01);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetWind(float Intensity01);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetSiren(bool bOn);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void UpdateFromWorld();
	void GenerateSamples(USoundWaveProcedural* Wave, int32 SamplesRequired);

	UPROPERTY(Transient)
	TObjectPtr<USoundWaveProcedural> Wave;

	UPROPERTY(Transient)
	TObjectPtr<UAudioComponent> Audio;

	FTimerHandle UpdateTimer;

	// Parametros objetivo (escritos en el hilo de juego, leidos en el de audio).
	std::atomic<float> TargetRpm01{0.f};
	std::atomic<float> TargetThrottle{0.f};
	std::atomic<float> TargetEngineOn{0.f};
	std::atomic<float> TargetRain{0.f};
	std::atomic<float> TargetWind{0.f};
	std::atomic<float> TargetSiren{0.f};

	// Estado del sintetizador (solo hilo de audio).
	float CurRpm01 = 0.f;
	float CurThrottle = 0.f;
	float CurEngine = 0.f;
	float CurRain = 0.f;
	float CurWind = 0.f;
	float CurSiren = 0.f;
	float EnginePhase = 0.f;
	float SirenPhase = 0.f;
	float SirenLfo = 0.f;
	float RainLow = 0.f;
	float WindLow = 0.f;
	uint32 NoiseState = 22695477u;

	static constexpr int32 SampleRate = 22050;
};
