#include "Audio/BlacklineProceduralAudio.h"
#include "Blackline.h"
#include "Components/AudioComponent.h"
#include "Contracts/WeatherDirector.h"
#include "Kismet/GameplayStatics.h"
#include "Pursuit/PursuitDirectorSubsystem.h"
#include "Sound/SoundWaveProcedural.h"
#include "TimerManager.h"
#include "Vehicle/BlacklineVanPawn.h"

UBlacklineProceduralAudio::UBlacklineProceduralAudio()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBlacklineProceduralAudio::BeginPlay()
{
	Super::BeginPlay();
	if (!bEnabled || !GetWorld() || !GetWorld()->IsGameWorld())
	{
		return;
	}

	Wave = NewObject<USoundWaveProcedural>(this);
	Wave->SetSampleRate(SampleRate);
	Wave->NumChannels = 1;
	Wave->Duration = INDEFINITELY_LOOPING_DURATION;
	Wave->SoundGroup = SOUNDGROUP_Default;
	Wave->bLooping = false;
	Wave->OnSoundWaveProceduralUnderflow.BindUObject(this, &UBlacklineProceduralAudio::GenerateSamples);

	Audio = NewObject<UAudioComponent>(GetOwner());
	Audio->SetupAttachment(GetOwner()->GetRootComponent());
	Audio->bAutoActivate = false;
	Audio->bIsUISound = true; // sin atenuacion por distancia: es el audio de la cabina
	Audio->SetSound(Wave);
	Audio->RegisterComponent();
	Audio->Play();

	GetWorld()->GetTimerManager().SetTimer(UpdateTimer, this, &UBlacklineProceduralAudio::UpdateFromWorld, 0.1f, true);
	UE_LOG(LogBlackline, Log, TEXT("Audio procedural iniciado (%d Hz)."), SampleRate);
}

void UBlacklineProceduralAudio::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(UpdateTimer);
	}
	if (Wave)
	{
		Wave->OnSoundWaveProceduralUnderflow.Unbind();
	}
	if (Audio)
	{
		Audio->Stop();
	}
	Super::EndPlay(EndPlayReason);
}

void UBlacklineProceduralAudio::SetEngine(float Rpm01, float Throttle01, bool bEngineOn)
{
	TargetRpm01 = FMath::Clamp(Rpm01, 0.f, 1.f);
	TargetThrottle = FMath::Clamp(Throttle01, 0.f, 1.f);
	TargetEngineOn = bEngineOn ? 1.f : 0.f;
}

void UBlacklineProceduralAudio::SetRain(float Intensity01) { TargetRain = FMath::Clamp(Intensity01, 0.f, 1.f); }
void UBlacklineProceduralAudio::SetWind(float Intensity01) { TargetWind = FMath::Clamp(Intensity01, 0.f, 1.f); }
void UBlacklineProceduralAudio::SetSiren(bool bOn) { TargetSiren = bOn ? 1.f : 0.f; }

void UBlacklineProceduralAudio::UpdateFromWorld()
{
	const ABlacklineVanPawn* Van = Cast<ABlacklineVanPawn>(GetOwner());
	if (!Van)
	{
		return;
	}
	const bool bRunning = Van->IsEngineOn() && Van->GetFuel01() > 0.f;
	SetEngine(Van->GetEngineRpm01(), Van->GetThrottleInput01(), bRunning);
	SetWind(FMath::Clamp(Van->GetSpeedKmh() / 160.f, 0.f, 1.f));

	if (const AWeatherDirector* Weather = Cast<AWeatherDirector>(UGameplayStatics::GetActorOfClass(this, AWeatherDirector::StaticClass())))
	{
		SetRain(Weather->GetPreset(Weather->GetCurrentWeather()).RainIntensity01);
	}
	if (const UPursuitDirectorSubsystem* Pursuit = UPursuitDirectorSubsystem::Get(this))
	{
		SetSiren(Pursuit->GetState() == EPursuitState::Hunting);
	}
}

void UBlacklineProceduralAudio::GenerateSamples(USoundWaveProcedural* InWave, int32 SamplesRequired)
{
	// Se llama en el hilo de audio: solo toca el estado del sintetizador y lee atomicos.
	const int32 Count = FMath::Max(SamplesRequired, 1024);
	TArray<int16> Buffer;
	Buffer.SetNumUninitialized(Count);

	const float Dt = 1.f / static_cast<float>(SampleRate);
	const float Master = MasterVolume;
	const float TRpm = TargetRpm01.load();
	const float TThr = TargetThrottle.load();
	const float TEng = TargetEngineOn.load();
	const float TRain = TargetRain.load();
	const float TWind = TargetWind.load();
	const float TSiren = TargetSiren.load();
	const float Blend = 1.f - FMath::Exp(-static_cast<float>(Count) * Dt * 6.f); // suavizado por bloque

	CurRpm01 += (TRpm - CurRpm01) * Blend;
	CurThrottle += (TThr - CurThrottle) * Blend;
	CurEngine += (TEng - CurEngine) * Blend;
	CurRain += (TRain - CurRain) * Blend;
	CurWind += (TWind - CurWind) * Blend;
	CurSiren += (TSiren - CurSiren) * Blend;

	const float Freq = IdleFrequencyHz + (MaxFrequencyHz - IdleFrequencyHz) * CurRpm01;
	const float EngineAmp = CurEngine * (0.20f + 0.35f * CurThrottle);

	for (int32 i = 0; i < Count; ++i)
	{
		// ---- Motor: fundamental + armonicos con "golpe" de cilindros.
		EnginePhase += Freq * Dt;
		if (EnginePhase >= 1.f)
		{
			EnginePhase -= 1.f;
		}
		const float P = EnginePhase * TWO_PI;
		const float Pulse = 0.55f + 0.45f * FMath::Sin(P * 0.5f); // modulacion lenta: sonido de diesel
		float Engine = FMath::Sin(P) + 0.55f * FMath::Sin(2.f * P) + 0.35f * FMath::Sin(3.f * P + 0.7f)
			+ 0.22f * FMath::Sin(5.f * P + 1.3f) + 0.12f * FMath::Sin(8.f * P);
		Engine *= Pulse * EngineAmp * 0.45f;

		// ---- Ruido blanco (LCG).
		NoiseState = NoiseState * 1664525u + 1013904223u;
		const float Noise = (static_cast<float>(NoiseState >> 8) / 8388608.f) - 1.f;

		// ---- Lluvia: ruido con poco grave (techo metalico).
		RainLow += (Noise - RainLow) * 0.12f;
		const float Rain = (Noise - RainLow) * CurRain * 0.28f;

		// ---- Viento: ruido grave que sube con la velocidad.
		WindLow += (Noise - WindLow) * 0.02f;
		const float Wind = WindLow * CurWind * 0.9f;

		// ---- Sirena: tono que oscila entre 650 y 900 Hz.
		SirenLfo += 1.1f * Dt;
		if (SirenLfo >= 1.f)
		{
			SirenLfo -= 1.f;
		}
		const float SirenFreq = 775.f + 125.f * FMath::Sin(SirenLfo * TWO_PI);
		SirenPhase += SirenFreq * Dt;
		if (SirenPhase >= 1.f)
		{
			SirenPhase -= 1.f;
		}
		const float Siren = FMath::Sin(SirenPhase * TWO_PI) * CurSiren * 0.14f;

		const float Mix = (Engine + Rain + Wind + Siren) * Master;
		const float Soft = FMath::Tanh(Mix * 1.4f); // limitador suave
		Buffer[i] = static_cast<int16>(Soft * 30000.f);
	}

	InWave->QueueAudio(reinterpret_cast<const uint8*>(Buffer.GetData()), Count * sizeof(int16));
}
