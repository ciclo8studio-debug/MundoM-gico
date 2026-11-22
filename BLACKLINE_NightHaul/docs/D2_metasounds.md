# D2 — MetaSounds: motor diésel, lluvia en techo metálico y oclusión de ventanilla

Bloque D, parte audio. UE 5.8.4. Cumple `CONTRATO.md` (módulo `Blackline`, sin Tick en actores, `UPROPERTY` en punteros UObject, valores en `EditAnywhere`).
Todo lo marcado `VERIFY-5.8` debe comprobarse en el editor (MetaSounds de 5.8 y Node Configuration son experimentales/cambiantes).

## 0. Convención de nombres

Assets (`[categoria]_[contexto]_[nombre]_[variante]`):

| Asset | Tipo |
|---|---|
| `msrc_veh_engine_diesel` | MetaSound Source (motor, 3D, adjunto a la furgoneta) |
| `msrc_amb_rain_roof_loop` | MetaSound Source (lluvia, 2D/cabina) |
| `mpatch_fx_cabin_filter` | MetaSound Patch (oclusión/LPF reutilizable) |
| `mpatch_veh_engine_cycle` | MetaSound Patch (un cilindro/ciclo, reutilizable) |
| `att_veh_engine` | Sound Attenuation (motor) |
| `sc_veh` / `sc_amb` | Sound Class (vehículo / ambiente) |

Wave Assets (los entrega audio/sound-designer; ver 4. Samples):
`sfx_veh_engine_idle_loop`, `sfx_veh_engine_low_loop`, `sfx_veh_engine_high_loop`, `sfx_veh_engine_exhaust_loop`, `sfx_veh_engine_start_01`, `sfx_veh_engine_stop_01`, `amb_env_rain_metal_roof_loop`, `amb_env_rain_drops_hard_loop`, `amb_env_rain_open_window_loop`.

## 1. Contrato de parámetros (nombres exactos, sensibles a mayúsculas)

Los Inputs de grafo son los "Audio Parameters". Se escriben desde C++ con `UAudioComponent::SetFloatParameter(FName, float)` / `SetBoolParameter` / `SetTriggerParameter` / `SetIntParameter`.
Declarados como `static const FName` en `Audio/BlacklineAudioParams.h` (sección 6).

### 1.1 `msrc_veh_engine_diesel`

| Parámetro | Tipo | Rango | Default | Descripción |
|---|---|---|---|---|
| `EngineOn` | Bool | — | false | Motor encendido. Controla el gate de salida. |
| `Ignition` | Trigger | — | — | Pulso al arrancar: reproduce `sfx_veh_engine_start_01` y rampa de RPM 0 a ralentí. |
| `Stop` | Trigger | — | — | Pulso al apagar: reproduce `sfx_veh_engine_stop_01` y rampa a 0. |
| `RPM` | Float | 0 – 4500 | 0 | Régimen real (rpm) leído del movimiento Chaos. |
| `IdleRPM` | Float | 500 – 1000 | 750 | Ralentí; define la normalización. |
| `MaxRPM` | Float | 3000 – 5500 | 4200 | Régimen máximo; define la normalización. |
| `Throttle` | Float | 0 – 1 | 0 | Acelerador suavizado. |
| `Load` | Float | 0 – 1 | 0 | Carga del motor: `Throttle * (1 - RPM01 * 0.5)` + bonus por barro/pendiente (calculado en C++). Mueve brillo y ruido de combustión. |
| `Turbo01` | Float | 0 – 1 | 0 | Silbido del turbo; sube con carga alta y RPM media. |
| `CabinMuffle01` | Float | 0 – 1 | 0.55 | Amortiguación base por estar en cabina (cámara Cockpit = 0.55; ThirdPerson = 0.15; ventanilla abierta lo reduce). |
| `WindowOpen01` | Float | 0 – 1 | 0 | Apertura de ventanilla (0 cerrada, 1 abierta). |
| `SurfaceRumble01` | Float | 0 – 1 | 0 | Vibración de chasis por superficie (grava/barro suben). No mezcla samples de rodadura: solo modula grave del motor. |
| `MasterGain` | Float | 0 – 1 | 1 | Ganancia final (ducking por checkpoint). |

### 1.2 `msrc_amb_rain_roof_loop`

| Parámetro | Tipo | Rango | Default | Descripción |
|---|---|---|---|---|
| `RainIntensity` | Float | 0 – 1 | 0 | Intensidad de la lluvia (de meteorología / nivel). |
| `RoofCoverage01` | Float | 0 – 1 | 1 | 1 = bajo techo de la cabina; 0 = descubierto (se usa 1 salvo túneles/pórticos; 0.3 bajo marquesina, 1.0 normal). |
| `WindowOpen01` | Float | 0 – 1 | 0 | Misma señal que el motor. |
| `WiperOn` | Bool | — | false | Limpiaparabrisas activos (añade el golpe de escobilla). |
| `SpeedKmh` | Float | 0 – 160 | 0 | Velocidad; sube la parte frontal/viento y el siseo de neumáticos mojados. |
| `SurfaceWet01` | Float | 0 – 1 | 0 | 1 en `ESurfaceKind::WetAsphalt`/`Mud`. Siseo de rodadura. |
| `MasterGain` | Float | 0 – 1 | 1 | Ganancia final. |

### 1.3 `mpatch_fx_cabin_filter` (entradas del patch, cableadas dentro de los dos Sources)

| Entrada | Tipo | Descripción |
|---|---|---|
| `In L`, `In R` | Audio | Señal a filtrar. |
| `CabinMuffle01` | Float | Amortiguación base (0 a 1). |
| `WindowOpen01` | Float | Apertura de ventanilla. |
| `CutoffMaxHz` | Float (default 18000) | Corte con ventanilla abierta y sin muffle. |
| `CutoffMinHz` | Float (default 900) | Corte con ventanilla cerrada y muffle máximo. |
| `SmoothMs` | Float (default 120) | Tiempo de interpolación. |

## 2. Motor diésel: grafo `msrc_veh_engine_diesel`

Enfoque: **híbrido granular/loops crossfadeados + síntesis de refuerzo**. Un diésel de furgoneta de reparto necesita golpeteo de combustión y grave grueso; los osciladores solos suenan a sintetizador, así que la base son loops grabados en bandas de RPM y la síntesis añade el cuerpo y la reacción continua.

### 2.1 Normalización (nodos de control, sin audio)

1. `RPM01 = Clamp((RPM - IdleRPM) / (MaxRPM - IdleRPM), 0, 1)` con nodos `Subtract`, `Divide`, `Clamp`.
2. `RPMSmooth = InterpTo(RPM01, InterpTime 0.06 s)` (nodo `Interpolate To`) para quitar escalones de los 20 Hz del timer de C++.
3. `PitchRatio_Idle = MapRange(RPMSmooth, 0..1 -> 0.85..1.35)`.
4. `PitchRatio_Low = MapRange(RPMSmooth, 0..1 -> 0.75..1.6)`.
5. `PitchRatio_High = MapRange(RPMSmooth, 0..1 -> 0.7..1.3)`.
6. Frecuencia de disparo (4 cilindros, 4 tiempos): `FireHz = RPM / 60 * 2` (2 explosiones por revolución). Con RPM = 750 -> 25 Hz.

### 2.2 Capa A: loops crossfadeados (cuerpo, 3 bandas)

Tres `Wave Player` (modo Loop, nodos `Wave Player (Mono)` o estéreo según el asset; `VERIFY-5.8`: nombre del nodo y pin `Pitch Shift`):

| Banda | Wave | Centro de RPM01 | Pitch |
|---|---|---|---|
| Idle | `sfx_veh_engine_idle_loop` | 0.00 | `PitchRatio_Idle` |
| Low-Mid | `sfx_veh_engine_low_loop` | 0.35 | `PitchRatio_Low` |
| High | `sfx_veh_engine_high_loop` | 0.85 | `PitchRatio_High` |

Ganancias por banda con curvas de coseno (suma de potencia constante), calculadas con `Map Range` + `Clamp` + `Power(x, 0.5)`:

- `G_Idle = sqrt(Clamp(1 - RPMSmooth / 0.40, 0, 1))`
- `G_Low  = sqrt(1 - Clamp(Abs(RPMSmooth - 0.40) / 0.40, 0, 1))`
- `G_High = sqrt(Clamp((RPMSmooth - 0.40) / 0.45, 0, 1))`

Los tres Wave Players se sincronizan con un único `Trigger` de inicio (`On Play`) para que arranquen en fase. Los loops deben tener la misma longitud o ser múltiplos exactos (ver 4. Samples) para que no haya fase flotando.

Salida de la capa A: `Mixer(Mono, 3)` o suma de `Multiply(Audio, Float)` -> `A_Out`.

### 2.3 Capa B: refuerzo sintético (cuerpo grave y reacción inmediata)

1. **Sub oscilador:** `Sine` a `FireHz * 0.5` (fundamental del bloque), con `Clamp` de 20 a 90 Hz. Ganancia `0.25 * (0.4 + 0.6*Load)`. Da el peso en la cabina.
2. **Pulso de combustión:** `Saw` (o `Square` con PWM 0.2) a `FireHz`, pasado por `LPF (State Variable Filter, Resonance 0.15)` con corte `Lerp(250, 900, Load)`. Ganancia `0.18`.
3. **Ruido de combustión:** `Noise (Pink)` -> `BPF` centrado en `Lerp(600, 1800, RPMSmooth)` con Q 0.9 -> ganancia `0.10 * Load`. Se modula con un LFO `Sine` a `FireHz` (profundidad 0.6) para dar el pulso rítmico del golpeteo diésel.
4. **Irregularidad (marcha floja):** `Random Get (Float)` disparado por un `Trigger Rep` a 6 Hz con rango ±1.5 % que se suma al pitch de todas las capas (un diésel en ralentí nunca es estable). Se atenúa con `(1 - RPMSmooth)`, desaparece a RPM alto.
5. **Turbo:** `Sine` con `Lerp(1800, 6500, Turbo01)` + `Noise (White)` -> `HPF` 4 kHz, ganancia `0.07 * Turbo01`.

`B_Out = suma de 1..5`.

### 2.4 Capa C: vibración de superficie

`SurfaceRumble01` modula la ganancia de un `LPF` (150 Hz) sobre `Noise (Brown)` con ganancia `0.15 * SurfaceRumble01`. `C_Out` se suma antes del filtro de cabina.

### 2.5 Mezcla y salida

1. `Engine_Dry = A_Out*0.7 + B_Out*0.45 + C_Out` (valores iniciales; afinar en cabina con Audio Insights).
2. **Filtro de cabina** (patch `mpatch_fx_cabin_filter`, sección 3) con `CabinMuffle01` y `WindowOpen01`.
3. `Soft Clip` (nodo `Waveshaper (Tanh)`) con drive `1.0 + Load*0.6`. `VERIFY-5.8`: nombre del nodo.
4. Ganancia final: `MasterGain * Smooth(EngineOn ? 1 : 0, 80 ms)`.
5. `Ignition` / `Stop`: cada trigger dispara su Wave Player one-shot (`sfx_veh_engine_start_01` / `sfx_veh_engine_stop_01`) que se suma tras el filtro de cabina, y fuerza una rampa de `RPM` interna: al `Ignition` se mantiene un "RPM virtual" con `Interpolate To` desde 0 a `IdleRPM` en 1.4 s. La salida de RPM del grafo es `Max(RPM, RPMVirtual)`, de modo que C++ solo envía el valor real.
6. Salida estéreo: `Out L`, `Out R`. La fuente se coloca con atenuación 3D en el pawn (`att_veh_engine`: sphere, 3500 cm de falloff, Log, sin occlusión propia porque la hace el filtro).

### 2.6 Resumen de cableado (texto)

```
RPM,IdleRPM,MaxRPM -> RPM01 -> RPMSmooth --> PitchRatio_x --> WavePlayer_x --> *G_x --\
                                        \--> FireHz -> Sub/Pulse/Noise (Load) --------+--> Mix --> CabinFilter --> Tanh --> *Gain --> Out
Turbo01 ----------------------------------> Turbo ----------------------------------/
SurfaceRumble01 --> RumbleNoise ----------/
EngineOn,MasterGain ---------------------------------------------------------------> Gain
```

## 3. Oclusión / low-pass de ventanilla: patch `mpatch_fx_cabin_filter`

Objetivo: al subir/bajar la ventanilla en un retén (`OpenWindowForInspection(bool)`) el mundo exterior (voz del inspector, lluvia, motor ajeno) pasa de apagado a nítido, y el motor propio se oye más fuerte hacia fuera.

Cadena:

1. `OpenCurve = Smooth(WindowOpen01)` con `Interpolate To` a `SmoothMs` (120 ms por defecto; el pawn ya interpola, esto es un segundo suavizado anti-click).
2. `OpenShaped = Power(OpenCurve, 1.5)` (la mayor parte de la apertura se percibe al final del recorrido).
3. `Cutoff = ExpLerp(CutoffMinHz, CutoffMaxHz, Max(OpenShaped, 1 - CabinMuffle01))`. Interpolación **exponencial en frecuencia** (nodo `Map Range` con `Clamp` y `Exp`, o `Lerp` sobre `Log2`), nunca lineal en Hz.
4. Filtro: dos `LPF (Biquad, Q 0.707)` en serie (24 dB/oct) con `Cutoff`. `VERIFY-5.8`: nodo `Biquad Filter` y pin `Frequency` en Hz.
5. Realce de ventana: `Low Shelf` -3 dB a 200 Hz proporcional a `OpenShaped` (cuando se abre, se pierde grave de cabina) y `High Shelf` +2 dB a 5 kHz proporcional a `OpenShaped`.
6. Cruce seco/mojado: `Out = Filtrada * (1 - 0.0) ` (sin cruce: el filtro está siempre aplicado, la apertura mueve el corte).

Valores de referencia:

| Estado | WindowOpen01 | CabinMuffle01 | Cutoff aproximado |
|---|---|---|---|
| Cabina, ventanilla cerrada | 0 | 0.55 | 1.8 kHz |
| Ventanilla a medias | 0.5 | 0.55 | 6 kHz |
| Ventanilla abierta (retén) | 1 | 0.55 | 18 kHz |
| Tercera persona | 0 | 0.15 | 12 kHz |

Para que el diseño sea coherente con la lluvia, el patch se instancia dos veces: motor y lluvia (con `CutoffMinHz` 900 en la lluvia para que el techo se oiga grave y sordo con ventanilla cerrada).

### 3.1 Capa de exteriores audible solo con ventanilla abierta

Sonidos como la voz del inspector, radio del retén o el generador del retén se reproducen como Audio Components separados en `ACheckpointController` y usan `UAudioComponent::SetLowPassFilterEnabled(true)` y `SetLowPassFilterFrequency(float)` (API nativa de `UAudioComponent`) con la misma curva. Ese código lo implementa lead-programmer; este documento solo fija la curva: `Cutoff = ExpLerp(900, 18000, WindowOpen01^1.5)`.

## 4. Lluvia en techo metálico: grafo `msrc_amb_rain_roof_loop`

La lluvia de techo se compone de tres elementos: **impactos sobre chapa** (golpes individuales con resonancia metálica), **cama de lluvia** (siseo denso) y **rodadura mojada**.

### 4.1 Impactos de gota sobre chapa (síntesis granular)

1. Un `Trigger Rep` con periodo aleatorio: `Rate = Lerp(2, 60, RainIntensity^1.3)` gotas/s. Se implementa con `Random Get (Float)` -> `Trigger Rep` (`Period = 1/Rate * Random(0.5, 1.5)`), usando `Trigger Random (Weighted)` si se necesita jitter. `VERIFY-5.8`: nodo exacto.
2. Cada trigger dispara un `Wave Player` de grano `amb_env_rain_drops_hard_loop` (loop largo, 30 s, 48 kHz, mono) con `Start Time` aleatorio (`Random Get (Float)` * duración) y envolvente `AD Envelope` de 5 ms / 55 ms. Se utilizan 8 voces en round-robin (`Trigger Route`).
3. Cada grano pasa por: `Random Get` de pitch ±12 %, `Random Get` de pan ±0.7, `Random Get` de ganancia 0.4 a 1.
4. **Resonancia de chapa:** `Biquad BPF` en paralelo con cortes fijos a 320, 780 y 1900 Hz (Q 6, 8, 5), alimentados por la suma de los granos, con ganancia total `0.35`. Simula los modos de la chapa del techo. Estos cortes son valores iniciales: afinar al oído contra la referencia.
5. Ganancia: `Lerp(0, 1, RainIntensity)`.

### 4.2 Cama de lluvia

1. `Wave Player` `amb_env_rain_metal_roof_loop` (loop estéreo 48 kHz, 45 s, sin patrones audibles).
2. `Gain = Lerp(0.05, 0.9, RainIntensity)` suavizada (150 ms).
3. `Pitch = Lerp(0.92, 1.08, RainIntensity)` (sube el tono con la intensidad).
4. Apertura de ventanilla: segundo Wave Player `amb_env_rain_open_window_loop` (lluvia sobre el exterior y la ventanilla, más agudo y abierto) con ganancia `WindowOpen01^1.2 * RainIntensity`.

### 4.3 Cobertura de techo

`RoofFilter`: `LPF` con corte `Lerp(18000, 3500, RoofCoverage01)` aplicado a los impactos y la cama (techo = más sordo). Cuando `RoofCoverage01 -> 0` el sonido se abre y el nivel de impactos baja `0.6x` (ya no hay chapa sobre la cabeza).

### 4.4 Rodadura mojada y viento

- `Noise (Pink)` -> `BPF 1.2 kHz Q 0.6` -> ganancia `SurfaceWet01 * Clamp(SpeedKmh/80, 0, 1) * 0.25`.
- `Noise (White)` -> `HPF 3 kHz` -> ganancia `Clamp(SpeedKmh/140, 0, 1)^2 * 0.08` (viento).

### 4.5 Limpiaparabrisas

`WiperOn` (bool) habilita un `Trigger Rep` a 0.9 Hz (`Sweep period = 1.1 s`) que dispara `sfx_veh_wiper_swipe_01` (goma sobre cristal, 200 ms) con ganancia `0.3` y `LPF` de cabina.

### 4.6 Mezcla

`Rain_Dry = (Impacts + Bed + Rolling + Wind) * RoofFilter` -> **`mpatch_fx_cabin_filter`** (`CutoffMinHz` 900) -> `Limiter` (threshold -3 dBFS) -> `* MasterGain` -> `Out L/R`.

Sound Class: `sc_amb`, reproducida 2D (sin spatialización) o con atenuación "cabina" (`att_cabin_2d`: sin falloff, sphere 0). Sin reverb propio: la cabina ya está en el sample.

## 5. Samples y specs

| Categoría | Formato | Frecuencia | Notas |
|---|---|---|---|
| Loops motor | WAV 24-bit -> OGG Vorbis (calidad 40 en import) | 48 kHz mono | 4 s de duración, cierre cero-cruce, múltiplos exactos entre bandas; grabados a RPM constante |
| One-shots (start/stop, escobilla) | WAV 24 bit | 48 kHz mono | Sin cola de reverb |
| Lluvia loops | WAV 24-bit -> OGG | 48 kHz estéreo (cama), mono (granos) | 30 a 45 s, sin patrones audibles |
| Loudness objetivo | -23 LUFS integrados (mezcla de cabina con motor + lluvia), picos de -1 dBTP | | Motor en ralentí ~ -30 LUFS, lluvia máxima ~ -26 LUFS |
| Presupuesto | <= 8 MB de samples para motor y <= 10 MB para lluvia | | Streaming desactivado para los loops de motor (latencia) |

## 6. C++: parámetros y envío desde `ABlacklineVanPawn`

### 6.1 `Source/Blackline/Public/Audio/BlacklineAudioParams.h`

```cpp
#pragma once

#include "CoreMinimal.h"

/** Nombres exactos de los Audio Parameters de los MetaSounds. Ver docs/D2_metasounds.md, sección 1. */
namespace BlacklineAudio
{
	// msrc_veh_engine_diesel
	static const FName EngineOn(TEXT("EngineOn"));
	static const FName Ignition(TEXT("Ignition"));
	static const FName Stop(TEXT("Stop"));
	static const FName RPM(TEXT("RPM"));
	static const FName IdleRPM(TEXT("IdleRPM"));
	static const FName MaxRPM(TEXT("MaxRPM"));
	static const FName Throttle(TEXT("Throttle"));
	static const FName Load(TEXT("Load"));
	static const FName Turbo01(TEXT("Turbo01"));
	static const FName CabinMuffle01(TEXT("CabinMuffle01"));
	static const FName WindowOpen01(TEXT("WindowOpen01"));
	static const FName SurfaceRumble01(TEXT("SurfaceRumble01"));
	static const FName MasterGain(TEXT("MasterGain"));

	// msrc_amb_rain_roof_loop
	static const FName RainIntensity(TEXT("RainIntensity"));
	static const FName RoofCoverage01(TEXT("RoofCoverage01"));
	static const FName WiperOn(TEXT("WiperOn"));
	static const FName SpeedKmh(TEXT("SpeedKmh"));
	static const FName SurfaceWet01(TEXT("SurfaceWet01"));
}
```

### 6.2 Añadidos a `ABlacklineVanPawn` (`Vehicle/BlacklineVanPawn.h`)

```cpp
// Includes: #include "TimerManager.h" ya llega por Engine; adelantar clases:
class UAudioComponent;
class USoundBase;

// --- Audio (D2) ---
protected:
	/** MetaSound del motor (msrc_veh_engine_diesel). */
	UPROPERTY(EditAnywhere, Category = "Audio|Engine")
	TObjectPtr<USoundBase> EngineSound;

	/** MetaSound de lluvia (msrc_amb_rain_roof_loop). */
	UPROPERTY(EditAnywhere, Category = "Audio|Rain")
	TObjectPtr<USoundBase> RainSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<UAudioComponent> EngineAudio;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<UAudioComponent> RainAudio;

	/** Frecuencia de envio de parametros (Hz). Sin Tick: se usa FTimerManager. */
	UPROPERTY(EditAnywhere, Category = "Audio", meta = (ClampMin = "5", ClampMax = "60"))
	float AudioUpdateHz = 20.f;

	UPROPERTY(EditAnywhere, Category = "Audio|Engine")
	float IdleRPM = 750.f;

	UPROPERTY(EditAnywhere, Category = "Audio|Engine")
	float MaxRPM = 4200.f;

	/** Muffle de cabina segun camara. */
	UPROPERTY(EditAnywhere, Category = "Audio|Cabin", meta = (ClampMin = "0", ClampMax = "1"))
	float CockpitMuffle01 = 0.55f;

	UPROPERTY(EditAnywhere, Category = "Audio|Cabin", meta = (ClampMin = "0", ClampMax = "1"))
	float ThirdPersonMuffle01 = 0.15f;

	/** Segundos que tarda la ventanilla en subir/bajar por completo. */
	UPROPERTY(EditAnywhere, Category = "Audio|Cabin", meta = (ClampMin = "0.1"))
	float WindowTravelSeconds = 1.2f;

	/** Intensidad de lluvia (0 a 1). La fija el nivel o el director de clima. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Rain", meta = (ClampMin = "0", ClampMax = "1"))
	float RainIntensity = 0.6f;

	/** 1 = techo de cabina; bajar en tuneles/porticos. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Rain", meta = (ClampMin = "0", ClampMax = "1"))
	float RoofCoverage01 = 1.f;

	void InitAudio();
	void UpdateAudioParameters();
	void SetWiperAudio(bool bOn);

private:
	FTimerHandle AudioTimer;
	float WindowOpenTarget = 0.f;
	float WindowOpenCurrent = 0.f;
	float ThrottleSmoothed = 0.f;
	float LastRpm = 0.f;
	bool bLastEngineOn = false;
	bool bWipersOn = false;
```

### 6.3 `Vehicle/BlacklineVanPawn.cpp` (fragmentos completos)

Includes:

```cpp
#include "Audio/BlacklineAudioParams.h"
#include "Components/AudioComponent.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "Blackline.h"
```

Constructor (tras crear la cabina y el resto de componentes):

```cpp
EngineAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("EngineAudio"));
EngineAudio->SetupAttachment(GetMesh());
EngineAudio->bAutoActivate = false;
EngineAudio->bIsUISound = false;

RainAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("RainAudio"));
RainAudio->SetupAttachment(GetRootComponent());
RainAudio->bAutoActivate = false;
RainAudio->bAllowSpatialization = false; // 2D de cabina
```

`BeginPlay()`:

```cpp
InitAudio();
```

`EndPlay()`:

```cpp
GetWorldTimerManager().ClearTimer(AudioTimer);
```

Inicialización y bucle de parametros:

```cpp
void ABlacklineVanPawn::InitAudio()
{
	if (EngineSound)
	{
		EngineAudio->SetSound(EngineSound);
		EngineAudio->Play();
		EngineAudio->SetFloatParameter(BlacklineAudio::IdleRPM, IdleRPM);
		EngineAudio->SetFloatParameter(BlacklineAudio::MaxRPM, MaxRPM);
		EngineAudio->SetBoolParameter(BlacklineAudio::EngineOn, false);
	}
	else
	{
		UE_LOG(LogBlackline, Warning, TEXT("ABlacklineVanPawn: EngineSound sin asignar (msrc_veh_engine_diesel)."));
	}

	if (RainSound)
	{
		RainAudio->SetSound(RainSound);
		RainAudio->Play();
	}
	else
	{
		UE_LOG(LogBlackline, Warning, TEXT("ABlacklineVanPawn: RainSound sin asignar (msrc_amb_rain_roof_loop)."));
	}

	const float Period = 1.f / FMath::Max(AudioUpdateHz, 1.f);
	GetWorldTimerManager().SetTimer(AudioTimer, this, &ABlacklineVanPawn::UpdateAudioParameters, Period, true);
}

void ABlacklineVanPawn::UpdateAudioParameters()
{
	const float Dt = 1.f / FMath::Max(AudioUpdateHz, 1.f);

	UChaosWheeledVehicleMovementComponent* Move = GetVehicleMovementComponent(); // VERIFY-5.8: puede devolver UChaosVehicleMovementComponent
	const bool bOn = IsEngineOn();

	// Triggers de arranque y parada.
	if (bOn != bLastEngineOn)
	{
		if (EngineAudio)
		{
			EngineAudio->SetBoolParameter(BlacklineAudio::EngineOn, bOn);
			EngineAudio->SetTriggerParameter(bOn ? BlacklineAudio::Ignition : BlacklineAudio::Stop);
		}
		bLastEngineOn = bOn;
	}

	// Motor.
	float Rpm = 0.f;
	float ThrottleIn = 0.f;
	if (Move && bOn)
	{
		Rpm = Move->GetEngineRotationSpeed();   // rpm, VERIFY-5.8
		ThrottleIn = Move->GetThrottleInput();  // 0..1, VERIFY-5.8
	}

	ThrottleSmoothed = FMath::FInterpTo(ThrottleSmoothed, ThrottleIn, Dt, 8.f);
	const float Rpm01 = FMath::Clamp((Rpm - IdleRPM) / FMath::Max(MaxRPM - IdleRPM, 1.f), 0.f, 1.f);
	const float Load = FMath::Clamp(ThrottleSmoothed * (1.f - Rpm01 * 0.5f) + (GetSpeedKmh() < 8.f ? 0.15f * ThrottleSmoothed : 0.f), 0.f, 1.f);
	const float Turbo = FMath::Clamp(ThrottleSmoothed * FMath::Sin(Rpm01 * PI), 0.f, 1.f);

	// Ventanilla: interpolacion lineal a velocidad constante.
	const float Step = Dt / FMath::Max(WindowTravelSeconds, 0.1f);
	WindowOpenCurrent = FMath::FInterpConstantTo(WindowOpenCurrent, WindowOpenTarget, Dt, 1.f / FMath::Max(WindowTravelSeconds, 0.1f));
	(void)Step;

	const bool bCockpit = CockpitCamera && CockpitCamera->IsActive();
	const float Muffle = bCockpit ? CockpitMuffle01 : ThirdPersonMuffle01;

	if (EngineAudio)
	{
		EngineAudio->SetFloatParameter(BlacklineAudio::RPM, Rpm);
		EngineAudio->SetFloatParameter(BlacklineAudio::Throttle, ThrottleSmoothed);
		EngineAudio->SetFloatParameter(BlacklineAudio::Load, Load);
		EngineAudio->SetFloatParameter(BlacklineAudio::Turbo01, Turbo);
		EngineAudio->SetFloatParameter(BlacklineAudio::CabinMuffle01, Muffle);
		EngineAudio->SetFloatParameter(BlacklineAudio::WindowOpen01, WindowOpenCurrent);
		EngineAudio->SetFloatParameter(BlacklineAudio::SurfaceRumble01, /* ver D1/terreno */ 0.f);
	}

	if (RainAudio)
	{
		RainAudio->SetFloatParameter(BlacklineAudio::RainIntensity, RainIntensity);
		RainAudio->SetFloatParameter(BlacklineAudio::RoofCoverage01, RoofCoverage01);
		RainAudio->SetFloatParameter(BlacklineAudio::WindowOpen01, WindowOpenCurrent);
		RainAudio->SetFloatParameter(BlacklineAudio::SpeedKmh, GetSpeedKmh());
		RainAudio->SetBoolParameter(BlacklineAudio::WiperOn, bWipersOn);
		RainAudio->SetFloatParameter(BlacklineAudio::CabinMuffle01, Muffle);
	}
}

void ABlacklineVanPawn::SetWiperAudio(bool bOn)
{
	bWipersOn = bOn;
}
```

`OpenWindowForInspection(bool)` (ya existe en el contrato; añadir al final de su cuerpo):

```cpp
WindowOpenTarget = bOpen ? 1.f : 0.f; // el parametro del contrato se llama bOpen
```

Enganche del limpiaparabrisas: en `ActivateDashControl(EDashControl::Wipers)` alternar el estado y llamar `SetWiperAudio(nuevoEstado)`.

### 6.4 Superficie, humedad y barro

`SurfaceWet01` y `SurfaceRumble01` se derivan de la pila de `PushSurfaceModifier`/`PopSurfaceModifier`: el último modificador activo define `ESurfaceKind`. Mapeo recomendado, en `EditAnywhere` (`TMap<ESurfaceKind, float>`):

| ESurfaceKind | SurfaceWet01 | SurfaceRumble01 |
|---|---|---|
| Asphalt | 0.1 | 0.1 |
| WetAsphalt | 1.0 | 0.15 |
| Gravel | 0.2 | 0.7 |
| Mud | 1.0 | 0.5 |
| Forest | 0.5 | 0.4 |

## 7. Pruebas y verificación

1. **Banco de pruebas sin gameplay:** abrir el MetaSound y usar el Preview con sliders para `RPM` 0..4500; comprobar que no hay clicks al cruzar 0.40 de RPM01.
2. **Audio Insights** (producción en 5.8): verificar que `msrc_veh_engine_diesel` use <= 1 voz y el DSP < 0.6 ms por bloque.
3. **Ventanilla:** disparar `OpenWindowForInspection(true/false)` y confirmar que el corte sube de 1.8 a 18 kHz en 1.2 s, sin zumbidos de zipper.
4. **Lluvia:** `RainIntensity` 0 -> 1: sin saltos, techo sordo con `WindowOpen01 = 0`, "abierto" con 1.
5. **Perf (PC de 8 GB e iGPU):** limites de voces `au.MaxVoices`/concurrencia: motor y lluvia con `Max Count = 1`, `Resolution = Stop Quietest`. 5.8 activa `au.EnableRelativeRenderCostVoiceLimit` por defecto: no hay que tocarlo, pero hay que medir.
6. **Compilar C++ no verificado:** las firmas `GetEngineRotationSpeed`, `GetThrottleInput`, `GetVehicleMovementComponent` y la API de nodos de MetaSounds quedaron marcadas `VERIFY-5.8`.

## 8. Dudas abiertas

- Origen de `RainIntensity` y `RoofCoverage01` (nivel, volumen de clima o director): sin definir en `CONTRATO.md`.
- Tipo de retorno de `GetVehicleMovementComponent()` en 5.8 y nombres exactos de `GetEngineRotationSpeed()` / `GetThrottleInput()`.
- Disponibilidad de los nodos `Waveshaper`, `Biquad Filter` y `Trigger Rep` con los nombres usados (MetaSound Node Configuration es experimental en 5.8).
- Los Wave Assets listados no existen aún: los debe producir sound-designer o importarse desde librerías.
