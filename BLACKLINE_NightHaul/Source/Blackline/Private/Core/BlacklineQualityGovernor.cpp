#include "Core/BlacklineQualityGovernor.h"
#include "Blackline.h"
#include "Engine/World.h"
#include "HAL/IConsoleManager.h"
#include "TimerManager.h"

bool UBlacklineQualityGovernor::ShouldCreateSubsystem(UObject* Outer) const
{
	// Solo en mundos de juego reales (no editor preview, no servidor dedicado).
	const UWorld* World = Cast<UWorld>(Outer);
	return World && World->IsGameWorld() && !IsRunningDedicatedServer() && Super::ShouldCreateSubsystem(Outer);
}

void UBlacklineQualityGovernor::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	const UBlacklineQualitySettings* S = UBlacklineQualitySettings::Get();
	if (!S->bEnableAdaptiveResolution)
	{
		return;
	}

	if (const IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ScreenPercentage")))
	{
		CurrentPercent = FMath::Clamp(FMath::RoundToInt(CVar->GetFloat()), S->MinScreenPercentage, S->MaxScreenPercentage);
	}
	LastFrameCounter = GFrameCounter;
	LastTimeSec = FPlatformTime::Seconds();
	InWorld.GetTimerManager().SetTimer(SampleTimer, this, &UBlacklineQualityGovernor::Sample,
		S->SampleIntervalSec, true);
}

void UBlacklineQualityGovernor::Deinitialize()
{
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(SampleTimer);
	}
	Super::Deinitialize();
}

void UBlacklineQualityGovernor::Sample()
{
	const UBlacklineQualitySettings* S = UBlacklineQualitySettings::Get();
	const double Now = FPlatformTime::Seconds();
	const double Dt = Now - LastTimeSec;
	const uint64 Frames = GFrameCounter - LastFrameCounter;
	LastTimeSec = Now;
	LastFrameCounter = GFrameCounter;
	if (Dt <= 0.0 || Frames == 0)
	{
		return;
	}

	MeasuredFps = static_cast<float>(Frames / Dt);

	if (MeasuredFps < S->TargetFps * 0.92f)
	{
		// Bajo el objetivo: baja de inmediato.
		HeadroomStreak = 0;
		Apply(CurrentPercent - S->StepPercent);
	}
	else if (MeasuredFps > S->TargetFps * 1.15f)
	{
		// Holgura sostenida: sube de a poco para evitar oscilar.
		if (++HeadroomStreak >= S->SamplesBeforeRaise)
		{
			HeadroomStreak = 0;
			Apply(CurrentPercent + S->StepPercent);
		}
	}
	else
	{
		HeadroomStreak = 0;
	}
}

void UBlacklineQualityGovernor::Apply(int32 NewPercent)
{
	const UBlacklineQualitySettings* S = UBlacklineQualitySettings::Get();
	NewPercent = FMath::Clamp(NewPercent, S->MinScreenPercentage, S->MaxScreenPercentage);
	if (NewPercent == CurrentPercent)
	{
		return;
	}
	if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ScreenPercentage")))
	{
		CVar->Set(static_cast<float>(NewPercent), ECVF_SetByGameSetting);
		UE_LOG(LogBlackline, Log, TEXT("Calidad adaptativa: %.0f FPS -> r.ScreenPercentage %d -> %d"),
			MeasuredFps, CurrentPercent, NewPercent);
		CurrentPercent = NewPercent;
	}
}
