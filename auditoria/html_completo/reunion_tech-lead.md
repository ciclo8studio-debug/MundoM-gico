# Reunión "Replicar el HTML completo" · @tech-lead (18-11-2026)

Fuentes: `RECORRIDO_HTML.md`, CHANGELOG 0.3.40–0.3.67, `ARQUITECTURA_3_NIVELES.md`, `REINGENIERIA_2026-11-18.md`, `GUIA_TUNING.md`, `Probar.bat`, memorias de cierres de GPU. No se abrió Unreal.

**Hallazgo clave:** el parque **sí corría** en esta iGPU cuando todo era un solo mapa (v0.3.1 y v0.4.0, 9-11). Desde la separación en dos mapas (v0.3.42) **nunca se jugó estable por la ruta normal**: v0.3.57 tuvo GPUCrash (sombras de linterna, arreglado en 0.3.58) y v0.3.62 volvió a caerse. Hay dos sospechosos que se pueden medir sin tocar el proyecto. (a) **Memoria acumulada**: los assets del hospital AAA siguen cargados cuando entra el parque, y la iGPU solo tiene 128 MB propios. (b) **Coste de render** en BasePass: Lumen, VSM, ~45 luces del parque y niebla volumétrica. El crash es un TDR (`BasePass > ParallelDraw`) con `TdrDelay=10`.

## 1. Brechas HTML → UE (mi área: arquitectura, progresión, guardado, rendimiento)
| Pieza del HTML | Estado UE | Nota |
|---|---|---|
| 3 actos: Pabellón → hospital → parque → persecución inversa → expediente | **Parcial** | Hospital → TV/VHS → `L_Parque` existe. Falta volver a `L_Hospital` para la persecución y el expediente |
| Parque carga y corre en la iGPU | **Falta (S1)** | GPUCrash al cargar. Ver §2 |
| Flags persistentes (GI `Flags`, `AgregarFlag`, `GuardarFlags`) | **Hecho** | v0.3.46/0.3.48. Faltan los nombres del HTML (abajo) |
| Cadena de objetivos con prioridad (`objective()`) | **Parcial** | `MostrarObjetivo` usa coordenadas fijas en el BP del jugador. Falta una tabla de datos |
| Interruptores 4/4 + cortocircuito + silueta | **Parcial** | `BP_Interruptor` y `BP_SustoEnergia` existen. El cortocircuito se quitó en 0.3.1 y la silueta salta al llegar, no con 4/4 |
| 4 cintas VHS (+20 % de batería, flecha) | **Falta** | No hay actor de cinta. Reusar el patrón `REC_*` |
| Llave (bodega) → casa → fusible | **Parcial** | Existen la bodega abierta, `TieneFusible` y la caja fuerte. Hay que quitar el clímax "huida a la entrada" (v0.4.0) |
| Cabina VIDEO + video de 12 páginas + elección E/Q | **Falta** | Reusar `WBP_Transicion`/`WBP_Pagina` |
| Persecución del payaso en el pasillo | **Falta** | El payaso solo está en `L_Parque` |
| Expediente + 3 finales por páginas | **Parcial** | Existe la pantalla "CAPÍTULO 1 · COMPLETADO". Faltan los umbrales |
| Escalada de amenazas: muñeca → payaso ángel → Willy/León → persecución | **Parcial** | Los actores existen. Falta activarlos según el número de cintas |
| Guardado: nivel + punto de entrada + cintas + páginas | **Parcial** | `Guardar`/`Cargar` + flags. Falta `Version`, `NivelActual`, `PuntoEntrada` y `Cintas` |
| 62 páginas / 15 notas | **Parcial** | 62 `REC_Pagina`, 15 con texto (`DA_NovelaData`). Las notas del HTML no existen |

## 2. Plan del parque, de menor a mayor riesgo
Regla: cada paso tiene su interruptor en `DA_Tuning`, va en una build propia y pasa el humo H4 (llegada + **10 min en el parque**, fps anotados) antes del siguiente. Si un paso solo no basta, se suma el siguiente.

**P0 · Diagnóstico sin tocar el proyecto (19-11, solo `Probar.bat` y la build v0.3.66).** Son 6 corridas de 3 min cada una; el CTO anota si se cae y los fps:
1. `-ir=parque` directo (no pasa por el hospital). Si **no** se cae aquí pero sí por la ruta normal, el problema es memoria acumulada → ir a P1b.
2. Opción 9 (Perfil A en vivo). Si esto lo arregla → P1a basta.
3. Opción 9 + `r.ScreenPercentage=50,r.VolumetricFog=0,r.ViewDistanceScale=0.5`.
4. Opción 9 + `r.Streaming.PoolSize=256,r.Streaming.LimitPoolSizeToVRAM=1`.
5. Ruta normal con la opción 9 aplicada desde el inicio (`-dpcvars` en el arranque normal).
6. Si todo se cae, `-dpcvars=r.MaxAnisotropy=0,r.Shadow.MaxResolution=256,r.LightMaxDrawDistanceScale=0.3`.
Medir con el `.exe -dev`: `stat unit`, `stat scenerendering` (draw calls, luces) y `memreport -full` al llegar. Guardar el log con `Recoger_Logs.ps1`. **Todo esto se agrega como opciones nuevas al submenú P de `Probar.bat`**, sin abrir Unreal.

**P1 · Cvars por mapa desde BP (20-11, riesgo bajo, sin recompilar shaders).**
- a) Función `AplicarPerfilMapa(Mapa)` en `BP_GestorNivel`. Se llama **en negro, antes de `OpenLevel(L_Parque)`** (en `WBP_Transicion.Paso`) y otra vez en `BeginPlay` del parque (`CalidadExtra`). Ejecuta con `ExecuteConsoleCommand` la lista que gane en P0: `r.DynamicGlobalIlluminationMethod 0`, `r.ReflectionMethod 2`, `r.Shadow.Virtual.Enable 0`, `r.MegaLights.Enable 0`, `r.Lumen.*.Allow 0`, `r.VolumetricFog 0`, `r.ScreenPercentage`… Al volver a `L_Hospital` se restaura el perfil del hospital. Interruptor nuevo: `Parque_PerfilLiviano` (activo).
- b) Si P0 apunta a memoria: pasar por un **mapa de paso vacío** (`L_Transicion`, solo la TV) con `OpenLevel`, más `ExecuteConsoleCommand("obj gc")` en negro, antes de abrir el parque. También bajar `r.Streaming.PoolSize`.
- Verificación en la build: un texto de depuración en el HUD solo con `-dev` (`GetConsoleVariableIntValue`) que confirme los valores, más H4.

**P2 · Recorte de contenido sin cambiar el diseño (21-11, riesgo bajo-medio, con edición headless).**
- Luces: todas las del parque **sin sombras** (solo la linterna, y apagada en el parque), `MaxDrawDistance` de 30–40 m y **≤10 luces visibles**. Los neones pasan a **material emisivo sin luz**.
- `CullDistanceVolume` en todo el parque (props chicos a 25 m, medianos a 50 m, grandes a 90 m). Los bosques ya cortan a 70–95 m.
- **Ocultar por zona:** `BP_VolumenZona` activa y oculta grupos por etiqueta (`Zona_Oeste`, `Zona_Este`, `Zona_Norte`), con `SetActorHiddenInGame` + `SetActorTickEnabled`. Son los 24 actores "vivos" (`NIE_*`, `FX_*`, `FAR_*`…) y las atracciones lejanas (IA dormida fuera de su zona).
- Lluvia del Niagara a la mitad en Baja y niebla volumétrica apagada.
- Meta del §6 de la arquitectura: < 1.200 draw calls y ≥ 25 fps en Baja.

**P3 · Sub-niveles con Level Streaming (26-11 → 1-12, riesgo medio). No World Partition: convertir con 8 GB es muy riesgoso.**
- `L_Parque` queda como persistente (terreno, entrada, avenida, lógica, jugador). Se suman 3 sub-niveles: Oeste (bodega, noria, jardín, tren fantasma), Este (casa, autos, carrusel) y Norte (castillo, teatro, montaña rusa).
- Se cargan con `Load Stream Level` desde `BP_VolumenZona`. Las coordenadas **no cambian**, así que las funciones con coordenadas fijas siguen funcionando.
- Mover los actores en headless con un script (hay riesgo de que el editor se cuelgue con miniaturas). Rama git propia.

**P4 · Perfil A en `DefaultEngine.ini` (último recurso).** Sigue vetado hasta después del 26-11 y necesita el OK del CTO (recompilación de shaders en un PC de 8 GB). Una alternativa más barata a evaluar: overrides de `sg.*@0` en `Config/DefaultScalability.ini` (calidad Baja), que no tocan `RendererSettings`.

**Plan B ya construido:** `Demo_SinParque` = verdadero. Con eso se vuelve a la demo de hospital + pantalla de fin, en un minuto y con `ajustar_tuning.py`.

## 3. Arquitectura de progresión (sobre lo que ya existe)
- **Flags** en `BP_GI_MundoMagico.Flags` (se reusa `AgregarFlag`): `Linterna`, `SalioPabellon`, `HospitalHecho`, `Interruptor_1..4`, `Energia`, `Cinta_1..4`, `LlaveCasa`, `CasaAbierta`, `Fusible`, `VideoVisto`, `EligioSala`, `Persecucion`, `Final`. Las cintas son flags, así que no hace falta otro arreglo. `NumCintas()` es una función pura.
- **Evento `FlagCambiado`** (dispatcher del GI): los actores lo escuchan. La cabina se enciende con `Cinta_4`, Willy y León despiertan con `Cinta_4`, la muñeca aparece con `Cinta_1` y el payaso ángel con `Cinta_3` (sin Tick).
- **Objetivos:** `DT_Objetivos` (fila = prioridad, `Requiere[]`, `Prohibe[]`, `Nivel`, clave de `ST_Mensajes`, etiqueta del destino de la flecha). `ObjetivoActual()` recorre la tabla de arriba abajo, igual que `objective()` del HTML, y se evalúa en `FlagCambiado` y al cambiar de zona. Reemplaza a `MostrarObjetivo` con el patrón `Arq_Objetivos` (estrangulador).
- **Guardado:** se mantiene la fachada `Guardar`/`Cargar`. A `BP_Partida` se le suman `Version:int=2`, `NivelActual`, `PuntoEntrada`, `Bateria`, `Vida`, `Paginas[62]` y `Notas[]`. El autoguardado se hace con cada `FlagCambiado` y al entrar a cada zona. **Continuar** = `OpenLevel(NivelActual)` + `PlayerStart` con la etiqueta `Entrada_<PuntoEntrada>`. Una partida con `Version` < 2 se carga sin los campos nuevos.
- **Persecución final:** al elegir E → `GI.PuntoEntrada=Persecucion` → `OpenLevel(L_Hospital)`. Un `BP_Persecucion` (payaso migrado o duplicado en el hospital, a 520 cm/s) corre solo con el flag `EligioSala`. El hospital ya es liviano, así que el acto 3 **no carga el parque**.
- **Limpieza obligatoria:** desactivar `EventoCasa` (huida a la entrada), `RevisarFinDemo` y `DemoCorta`, que contradicen el flujo HTML.

## 4. Plan por builds (una por día; el freeze del 25-11 se mantiene)
| Build | Fecha | Contenido | Verificación |
|---|---|---|---|
| — | 19-11 | P0: 6 corridas de diagnóstico con `Probar.bat` | Tabla de crash y fps por corrida |
| v0.3.68 | 20-11 | P1 (cvars por mapa / mapa de paso) | H4: 10 min en el parque sin cierre |
| v0.3.69 | 21-11 | P2 (luces, culling, ocultar por zona) | ≥25 fps en Baja; `stat scenerendering` |
| v0.3.70 | 22-11 | Progresión: flags, `FlagCambiado`, `DT_Objetivos`, guardado v2 | Continuar en hospital y en parque |
| v0.3.71 | 23-11 | Interruptores con cortocircuito, 4 cintas, llave → casa → fusible | Recorrido del acto 2 con objetivos correctos |
| v0.3.72 | 24-11 | Cabina + video + elección + persecución en `L_Hospital` + expediente/finales | Partida completa de 20–25 min |
| **v0.4.0-rc1** | **25-11** | Freeze → testers el 26-11 | Gate: 0 S1, sin GPUCrash |
| v0.4.x | 26-11 → 1-12 | P3 si hace falta; escalada (muñeca, ángel, Willy/León); puzle del teatro; notas | Humo por zona |
| rc2 / gold | 3-12 / 8-12 | Freeze de contenido / gold | Un tester con iGPU sin cierres |

**Si no alcanza, se recorta en este orden:** puzle del teatro → tren fantasma y notas → muñeca → payaso ángel → Willy y León se quedan en **un** enemigo → páginas del video = las 15 con texto + imágenes `T_Pag_*` → **Sprint B (sótano/morgue) completo**. Esto último choca con la hoja de ruta del 18-11: el parque y el sótano no caben juntos.
**Núcleo intocable:** linterna, hospital, energía, 4 cintas, llave/casa/fusible, cabina, persecución, expediente con finales.

## 5. Decisiones para el CTO (máx. 3)
1. **¿El parque HTML reemplaza al Sprint B (sótano/morgue) en la demo del 10-12?** Recomiendo **sí**. El sótano pasa al juego completo. No caben los dos con calidad.
2. **¿Se aprueban cvars por mapa desde BP y, si no bastan, overrides en `DefaultScalability.ini` (no `RendererSettings`) antes del 26-11?** Recomiendo **sí**: no recompila shaders. El Perfil A en el `.ini` sigue vetado.
3. **¿Gate del parque el 1-12?** Si un tester con iGPU no aguanta 10 min sin cierre, se publica con `Demo_SinParque` = verdadero (Plan B) y el parque sale en un parche. Recomiendo **sí**.
