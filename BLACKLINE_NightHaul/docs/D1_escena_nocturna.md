# D1 — Escena nocturna (parte visual)

UE 5.8.4 (posterior al conocimiento del modelo). Fuente: `docs/engine-reference/unreal/modules/rendering.md`. Lo no cubierto ahí va marcado `// VERIFY-5.8`.
Objetivo: 60 FPS (16.6 ms/frame) en GPU media (clase GTX 1660 Super / RTX 3050 / RX 6600) a 1080p, escalado de pantalla 100% con TSR (`r.ScreenPercentage` 75 como fallback).
Los valores numéricos son punto de partida técnico; colores y tono final los decide art-director.

## 1. Presupuestos globales

| Presupuesto | Límite |
|---|---|
| Frame GPU total | 16.6 ms (objetivo medio 15 ms) |
| Lumen (GI + reflejos) | ≤ 4.5 ms |
| Shadow maps (VSM) | ≤ 2.0 ms |
| Niebla volumétrica | ≤ 1.2 ms |
| Niagara (GPU, suma) | ≤ 1.5 ms |
| Base pass + materiales | ≤ 4.0 ms |
| Post (TSR, bloom, tonemap) | ≤ 2.0 ms |
| Reserva | ≥ 1.4 ms |
| Draw calls | ≤ 2500 |
| Triángulos en pantalla | ≤ 6 M (Nanite en entorno estático) |
| Memoria de texturas | ≤ 3 GB VRAM |
| Instrucciones de shader (asfalto mojado) | ≤ 250 ALU |
| Overdraw de partículas | ≤ 2.5 capas promedio, ≤ 6 pico |
| Luces dinámicas con sombra visibles | ≤ 4 (faros 2 + checkpoint 1 + 1 libre) |

## 2. Iluminación base

**DirectionalLight (luna, tenue)**
- Intensity 0.35 lux (`Intensity` en lux), Temperature 8500 K (placeholder, art decide color).
- `AtmosphereSunLight = true`, `AtmosphereSunLightIndex = 0`.
- Cast Shadows ON, Virtual Shadow Maps; `Cast Cloud Shadows` OFF.
- `DynamicShadowDistance` no aplica a VSM; usar `r.Shadow.Virtual.MaxPhysicalPages=2048`.
- Light Shaft Occlusion/Bloom OFF (el volumétrico ya cubre).
- Rotación: Pitch -28, Yaw editable (`EditAnywhere`, sin coordenadas escritas).

**SkyAtmosphere + SkyLight**
- SkyAtmosphere: Ground Albedo (0.05, 0.05, 0.05); Mie Scattering Scale 0.004; Atmosphere Height 60 km (defaults), Multi Scattering 0.5.
- SkyLight: `Real Time Capture = ON`, Source Type "SLS Captured Scene", Intensity 0.25, `Cast Shadows` OFF, Lower Hemisphere Is Solid Color OFF. Resolución cubemap 128 (`r.SkylightCapture`-ver `// VERIFY-5.8`).
- Sin VolumetricClouds (ahorro ~0.8 ms). Cielo cubierto por capa de nubes en material de skydome estático opcional.

**ExponentialHeightFog + Volumetric Fog**
- Fog Density 0.018; Height Falloff 0.25; Fog Height Offset editable.
- Fog Inscattering Color: tomar del SkyAtmosphere (`Sky Atmosphere Ambient Contribution` 1.0).
- Start Distance 0; Fog Max Opacity 0.9; Fog Cutoff Distance 0.
- Volumetric Fog ON: Scattering Distribution 0.35, Albedo (0.75, 0.78, 0.85), Extinction Scale 1.0, View Distance **6000 cm** (60 m), Static Lighting Scattering Intensity 0.
- Cvars: `r.VolumetricFog.GridPixelSize=16`, `r.VolumetricFog.GridSizeZ=64`, `r.VolumetricFog.HistoryMissSupersampleCount=4`, `r.VolumetricFog.TemporalReprojection=1`.
- Solo luces con `Volumetric Scattering Intensity` ≥ 1 aportan: faros 1.5, checkpoint 2.0, luna 0.4.

## 3. Faros (SpotLight, sombras baratas)

Dos `USpotLightComponent` en `ABlacklineVanPawn` (sin Tick; se activan por `FOnDashControlChanged` con `EDashControl::Headlights`).

| Parámetro | Valor |
|---|---|
| Intensity | 4000 cd (`Units = Candelas`) |
| Inner / Outer cone | 14° / 34° |
| Attenuation Radius | 4500 cm |
| Source Radius / Length | 6 / 0 cm |
| Temperature | 4300 K (placeholder, art decide) |
| Volumetric Scattering Intensity | 1.5 |
| IES / Light Function | ninguno (ahorro) |
| Cast Shadows | **solo el faro izquierdo**; derecho OFF |
| Shadow método | VSM con `Shadow Resolution Scale` 0.5, `Shadow Bias` 0.5 |
| Dynamic Shadow Distance | VSM: culling por `Max Distance Fade Range` ≥ 3000 cm |
| Cast Translucent Shadows | OFF |
| Cast Volumetric Shadow | OFF (el costo es del volumétrico) |
| Affect Lumen | `bAffectGlobalIllumination` ON, pero `Indirect Lighting Intensity` 0.5 |

Alternativa si el presupuesto de sombras se pasa: MegaLights (producción en 5.8) para faros y luz de checkpoint con `r.MegaLights.EnableForProject=1`; sombras por ray tracing (`r.MegaLights.DefaultShadowMethod=0`). Costo esperado ~1.5 ms fijo; **requiere medir en la GPU objetivo** (`// VERIFY-5.8`: la ayuda del editor 5.8.1 aún lo marca experimental y la luz direccional necesita `r.MegaLights.DirectionalLights=1`).
Luz de `ACheckpointController::InspectionLight`: Intensity 12000 cd, cone 25/55°, radio 6000 cm, sombra ON solo cuando `ECheckpointState` ≥ `Approaching` (se activa por delegado).

## 4. Lumen (High)

Project Settings: GI = Lumen, Reflections = Lumen, Shadow Map Method = Virtual Shadow Maps. `r.Lumen.HardwareRayTracing=0` (la GPU media no lo sostiene).
Post Process Volume global (Unbound, Priority 0):

| Parámetro | Valor |
|---|---|
| Lumen Scene Lighting Quality | 1.0 |
| Lumen Scene Detail | 1.0 |
| Final Gather Quality | 1.0 |
| Max Trace Distance (Scene View Distance) | 20000 cm |
| Max Reflection Bounces | 1 |
| Reflection Quality | 1.0 |
| Lumen Scene Lighting Update Speed | 1.0 |
| Final Gather Lighting Update Speed | 1.0 |

Cvars de ajuste (`DefaultEngine.ini`, `[SystemSettings]`):
```
r.Lumen.ScreenProbeGather.DownsampleFactor=16
r.Lumen.ScreenProbeGather.RadianceCache.ProbeResolution=32
r.Lumen.Reflections.DownsampleFactor=2
r.Lumen.TraceMeshSDFs.Allow=1
r.LumenScene.SurfaceCache.CardCaptureRefreshFraction=0.25
r.Lumen.DiffuseColorBoost=1.0
r.Lumen.HeightFog=1
```
Fallback si Lumen > 4.5 ms: bajar a "Lumen Lite" (Beta en 5.8, ~2x más rápido) vía Post Process `Global Illumination Quality`; documentado en `// VERIFY-5.8` (nombre exacto del ajuste no verificado). Segundo fallback: `DownsampleFactor=32` en ScreenProbeGather.

Otras: `r.DefaultFeature.AutoExposure.Method=0` desactivado en favor de exposición manual: Min/Max EV100 = 1.0 / 5.0 (noche), bias 0 (art decide el look).
Bloom 0.4, Motion Blur 0.25 (se baja al activar `IA_Handbrake`), Ambient Occlusion intensidad 0.5.

## 5. Material de asfalto mojado + MPC

**`MPC_Weather`** (`UMaterialParameterCollection`, `/Game/Blackline/Materials/MPC_Weather`):

| Parámetro | Tipo | Rango | Default | Efecto |
|---|---|---|---|---|
| `Wetness` | Scalar | 0–1 | 0.0 | oscurece Base Color x(1 - 0.35*W), baja Roughness hacia 0.12, sube Specular |
| `RainIntensity` | Scalar | 0–1 | 0.0 | ripples animados, spawn de Niagara, ruido de normal |
| `Puddles` | Scalar | 0–1 | 0.0 | umbral de máscara de charcos (Roughness 0.03, Normal aplanada) |

Actualización sin Tick: un componente/actor de clima (`AWeatherDirector`, a crear en D*) llama `UKismetMaterialLibrary::SetScalarParameterValue(World, MPC, "Wetness", v)` desde un `FTimerManager` a 10 Hz mientras interpola, y se detiene al llegar al objetivo. Interpolación de lluvia: 20 s subida, 40 s secado.

**`M_WetAsphalt`** (Substrate Slab, un solo material para `ESurfaceKind::Asphalt` y `WetAsphalt`; el grafo usa `Wetness`):
- Texturas: Base Color, Normal, ORM, todas 2048² BC1/BC5/BC1 (3 x 2.7 MB con mips = ~8 MB). Máscara de charcos: textura de ruido 512² BC4 (R) en coords mundo a escala 1/800 cm.
- Roughness = `Lerp(RoughTex, 0.12, Wetness)`, luego `Lerp(eso, 0.03, PuddleMask)`.
- `PuddleMask = saturate((Noise - (1 - Puddles)) * 8) * saturate(Wetness*2)`, y se multiplica por (1 - pendiente) para que no haya charcos en cuestas (`WorldNormal.z`^4).
- Ripples: 1 capa de normal animada 256² con `Time` (Panner), visibilidad = `RainIntensity * PuddleMask`; **no** usar WPO ni Tessellation.
- Specular 0.5 -> 0.7 con Wetness. Sin Translucency, sin refracción, Shading Rate normal.
- Presupuesto: ≤ 250 instrucciones ALU, ≤ 6 texture samples, 1 draw call por instancia de superficie, Nanite OK (sin WPO).
- Tipos de superficie: `Gravel`, `Mud`, `Forest` usan `M_Dirt` con la misma lógica reducida (solo Wetness y Puddles); en `Mud` Roughness mínimo 0.25 y Base Color se oscurece 0.45.
- Reflejos en charcos: Lumen Reflections; `Reflection Method` de SSR como respaldo con `r.SSR.Quality=2` no necesario.

Parámetros de superficie para `ATerrainModifierVolume`: nada se copia al material; el volumen solo cambia física.

## 6. Niagara (3 sistemas, todos GPU salvo chispas)

Costos medidos en GPU media a 1080p (estimados; confirmar con `stat Niagara` / `profilegpu`).

| Sistema | Máx. partículas | Sim | Emisión | Ms GPU máx. | Overdraw | Textura |
|---|---|---|---|---|---|---|
| `NS_Rain` | 3000 vivas | GPU | 1500/s x RainIntensity | 0.7 | ≤ 2.0 | 64x256 BC4, 1 sola |
| `NS_MudSplash` | 400 vivas por rueda trasera (800 total) | GPU | 0-250/s según velocidad y `ESurfaceKind::Mud` | 0.5 | ≤ 2.5 | 128² flipbook 4x4, BC1 alpha |
| `NS_Sparks` | 120 vivas | CPU | ráfagas de 20, máx. 4 ráfagas/s | 0.3 (CPU 0.1 ms) | ≤ 1.0 | sin textura (sprite aditivo) |

Detalles:
- **NS_Rain**: volumen que sigue la cámara (se adjunta al `CockpitCamera` / `ChaseCamera`; sin Tick propio; Niagara simula); caja 2400 x 2400 x 1800 cm, velocidad de caída 1800 cm/s, ángulo -8° por viento, vida 1.0 s, sprite alineado a velocidad 1.5 x 40 cm. Sin colisión por partícula: el impacto se finge con ripples del material (MPC). Fixed bounds ON. Culling por distancia 4500 cm.
- **NS_MudSplash**: vida 0.9 s, tamaño 15–40 cm, velocidad lateral 150–400 cm/s, gravedad -980, colisión **solo con profundidad de escena (GPU depth buffer)**, 1 rebote. Emisión pone `SpawnRate = Clamp(SpeedKmh*3, 0, 250) * bOnMud`. Se activa por `PushSurfaceModifier` con `Surface == Mud`; se desactiva en `Pop`.
- **NS_Sparks**: se dispara con `ApplyImpact` (≥ 6 m/s). Vida 0.25–0.5 s, 20 partículas por ráfaga, sin luces por partícula, sin sombras. Un solo `PointLight` de destello 1 frame, Intensity 800 cd, radio 600 cm, sombras OFF.
- Reglas comunes: `Scalability` con 3 niveles (`Effect Type`): Alto = 100%, Medio = 60% del máximo (partículas y emisión), Bajo = 30%. LOD por distancia: a >3000 cm de la cámara, la emisión de `NS_MudSplash` y chispas se divide por 2; a >6000 cm, desactivar. `Max Distance` de cull = 7000 cm. `Fixed Bounds` en todos. `Significance` por distancia.
- Total Niagara simultáneo: ≤ 4300 partículas, ≤ 1.5 ms GPU, ≤ 0.2 ms CPU.

## 7. Cvars de rendimiento (resumen `DefaultEngine.ini`)

```
r.ScreenPercentage=100
r.AntiAliasingMethod=4            ; TSR
r.TSR.History.ScreenPercentage=100
r.Shadow.Virtual.Enable=1
r.Shadow.Virtual.MaxPhysicalPages=2048
r.Shadow.Virtual.ResolutionLodBiasLocal=0.5
r.Shadow.Virtual.SMRT.RayCountLocal=4
r.VolumetricFog.GridPixelSize=16
r.VolumetricFog.GridSizeZ=64
r.Lumen.ScreenProbeGather.DownsampleFactor=16
r.Lumen.Reflections.DownsampleFactor=2
r.MotionBlurQuality=2
r.Streaming.PoolSize=2048
r.Nanite.MaxPixelsPerEdge=1.0
r.LightMaxDrawDistanceScale=1
```
Verificar cada cvar con `// VERIFY-5.8` antes de fijarla: los nombres de `r.Lumen.*` y `r.Shadow.Virtual.*` cambiaron entre versiones y solo `r.Lumen.HeightFog`, `r.MegaLights.*` y `r.Substrate` están confirmados en `rendering.md`.

## 8. Niveles de calidad

| Nivel | GPU | Cambios |
|---|---|---|
| Alto (60 FPS GPU media) | Lo anterior | — |
| Medio | iGPU / GTX 1050 | Lumen Lite, VolumetricFog GridPixelSize 24, sombra de faros OFF, Niagara 60%, `r.ScreenPercentage=75` |
| Bajo | mínimo | Lumen OFF (SkyLight estático + SSR), VolumetricFog OFF (solo ExponentialHeightFog), lluvia 30%, sin splash |

## 9. Verificación (lista corta)

1. `stat unit` + `stat gpu` en build empaquetada, ruta de 2 min con lluvia y barro: Frame ≤ 16.6 ms p95.
2. `profilegpu` en el checkpoint con faros + InspectionLight: sombras ≤ 2.0 ms, volumétrico ≤ 1.2 ms.
3. `stat Niagara`: ≤ 4300 partículas, ≤ 1.5 ms GPU.
4. `viewmode shadercomplexity` en asfalto: sin rojo, `ALU` ≤ 250.
5. `viewmode quadoverdraw` con lluvia a máxima intensidad: promedio ≤ 2.5 capas.

## Dudas abiertas

- Hardware real de "GPU media" para medir (modelo exacto).
- ¿MegaLights sí o no en faros (ahorra sombras, pero la luz direccional requiere cvar)?
- Nombre exacto del ajuste Lumen Lite en Post Process 5.8.
- Quién implementa `AWeatherDirector` (no está en CONTRATO.md).
