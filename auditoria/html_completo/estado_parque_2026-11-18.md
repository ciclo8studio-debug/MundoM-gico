# Estado del cierre del parque (para la reunión de avance del 18-11)

**Hechos de los reportes de cierre** (`%LOCALAPPDATA%\MundoMagico\Saved\Crashes`, `CrashContext.runtime-xml`):
- Todas las entradas a `L_Parque` de hoy terminan en **GPUCrash** ("GPU Crash dump Triggered"), con 750–840 MB de RAM libre.
  - D2 (Perfil A en vivo), D3 (+ScreenPercentage 50, sin niebla volumétrica), D4 (pool de texturas 256), D6 (sombras de 256, LightMaxDrawDistanceScale 0,3), D7 (modo liviano del editor con RayTracing apagado) y D8 (+FX.AllowGPUParticles=0, VolumetricCloud=0, DFAO=0).
  - **D9 (r.ShadowQuality=0 + sin captura del cielo en tiempo real) y D10 (r.ShadowQuality=0) también se cayeron** (11:34 y 11:35). El CTO creyó que no; los reportes dicen que sí.
  - Con la v0.3.68 (parque sin `CastShadows` en 14 luces, el sol y el SkyLight): `-ir=parque` se cayó a las 11:54 y `-ir=teatro` a las 11:57.
- El juego ya corre en **DX11** (`DefaultGraphicsRHI_DX11`). El proyecto tiene `r.RayTracing=True`, `r.Shadow.Virtual.Enable=1` y Lumen en `DefaultEngine.ini`; el Perfil A sigue vetado.
- **El editor sí dibuja el parque** en modo liviano (`-dx11` + los mismos cvars de D7).
- El CTO dice: al elegir el parque **"parto en la sala de hospital"** y luego se cae. Al cruzar el glitch, "vuelve como al inicio de la partida" y se cierra.
- Contenido de `L_Parque`: 1 PPV (sin blendables), VolumetricCloud, SkyAtmosphere, ExponentialHeightFog, SkyLight (RealTimeCapture), 1 DirectionalLight, 48 PointLight, 2 SpotLight, 79 DecalActor, 6 Emitter (Cascade), unos 1.427 actores y BP_Bosque (ISM de árboles).
- `-ir=` carga la partida guardada (`Partida.sav`) si existe.
- **En curso:** build **Development v0.3.68_dev**, que escribe `Saved\Logs`, para ver el motivo exacto del `DXGI_ERROR_DEVICE_*` y qué pase estaba dibujando la GPU.

## Hallazgo 18-11 12:10 (build Development v0.3.68_dev)
- Con `Partida.sav` y `Flags.sav` **movidos a respaldo**, `L_Parque -ir=parque` **no se cayó** en más de 5 min (1,3 GB, respondiendo, sin reportes de cierre nuevos). Todos los cierres anteriores ocurrieron con `Partida.sav` presente (que se reescribe al entrar).
- Perfil GPU del cuadro 1: TSR RejectShading 295 ms y EndDrawingViewport 90 ms, que es solo la primera carga. `CalidadExtra` fuerza `r.AntiAliasingMethod 4`, así que `-dpcvars=...=2` no aplica.
- **H2 (partida guardada + `-ir` dentro de `L_Parque`) pasa a hipótesis principal.** Falta probar la ruta normal (el glitch guarda y el parque carga ese guardado).
