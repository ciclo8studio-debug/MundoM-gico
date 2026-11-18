# Avance "Replicar el HTML" · @tech-lead (18-11-2026, tarde)

## 1. Estado del plan
- **Hecho:** P0 (D1–D10 en `Probar.bat`), v0.3.67 (parque reconectado), v0.3.68 (luces del parque sin `CastShadows`). P2 parcial (sombras fuera).
- **Error de lectura:** el CHANGELOG 0.3.68 dice que D9/D10 no se caían. Los reportes dicen que **sí** (11:34 y 11:35), y la v0.3.68 se cayó en parque y teatro. **Las sombras no son la causa.** Hay que corregir el CHANGELOG.
- **Atrasado:** la causa raíz sigue abierta, P1 (`AplicarPerfilMapa`) no aplica (ningún cvar sirve) y la progresión del 22-11 no empezó.
- **Hechos que pesan:** con TdrDelay=10, un cuadro "lento" no explica el reset; esto es un **cuelgue** de shader o de driver, o algo que nace en runtime. El editor (sin jugador, sin BeginPlay) lo dibuja. `L_Parque` todavía contiene `NavMesh_Hospital` y `BP_IsidoraAparicion` (restos del mapa único) y Landscape. El proyecto tiene `r.Substrate=True` + `ProjectGBufferFormat=1`, Lumen, VSM y RayTracing activos sobre DX11.

## 2. Hipótesis del GPUCrash, de más a menos probable
| # | Hipótesis | Por qué encaja | Prueba que confirma o descarta |
|---|---|---|---|
| H1 | **Lógica heredada al empezar** (BeginPlay del jugador o de los BP del parque en la rama "parque" de `CalidadExtra`: lluvia, MID o material de post, glitch, `BP_IsidoraAparicion`) lanza un pase o efecto que cuelga la GPU | Ningún cvar cambia nada, el editor sin jugador no se cae y el CTO "parte en la sala de hospital" y se cae al cruzar el glitch | **PIE** de `L_Parque` en el editor liviano (`-dx11`, con jugador). Si se cae, es contenido o BP. En el log Development, mirar las últimas `LogBlueprintUserMessages`/`LogScript` antes de `DXGI_ERROR_DEVICE_HUNG/REMOVED` |
| H2 | **`Partida.sav` + `-ir`** dejan al jugador en coordenadas del hospital dentro del parque y la secuencia del glitch se repite (OpenLevel o carga en bucle) | "Vuelve como al inicio de la partida y se cierra". `-ir=teatro` también se cae porque carga la misma partida | `Probar.bat` → **Borrar partida** y después D1 (`-ir=parque`). Contar en el log las líneas `LogLoad: LoadMap`/`Browse`: más de 1 por llegada indica bucle |
| H3 | **Shader o material cocinado** que el driver Intel DX11 no termina (Substrate adaptativo en SM5, fallback de Nanite, Landscape, MI con displacement, decal) | El cuelgue no depende de la carga y el editor compila los shaders en local | Correr sin cocinar: `UnrealEditor.exe MUNDO_MÁGICO.uproject /Game/MundoMagico/Maps/L_Parque -game -dx11` con los cvars de D7. Si no se cae, el problema está en el cocinado. En el log buscar `Missing shader`, `Failed to compile`, `LogMaterial` y `WorldGridMaterial` (fallback) |
| H4 | **Linterna del jugador** (LightFunction/IES, cono de 55 m sobre unos 1.400 actores) y cámara 1ª persona | No existe en el editor y la v0.3.57 ya se cayó por la linterna en el parque | Build `-dev`: `-dpcvars=ShowFlag.LightFunctions=0` y entrar **sin encender** la linterna. Si no se cae → apagarla o simplificarla en el parque |
| H5 | **Pase de render puntual** (decals ×79, nubes, Landscape, translucidez, material de post) | Sería la causa si H1–H4 se descartan | Bisección con show flags en la build Development (D11–D16 nuevas): `ShowFlag.Decals=0`, `ShowFlag.Landscape=0`, `ShowFlag.Cloud=0`, `ShowFlag.Translucency=0`, `ShowFlag.PostProcessMaterial=0`, `ShowFlag.Particles=0`. La que no se cae señala el pase |
| H6 | Memoria o carga bruta | **Descartada por los hechos:** 800 MB libres, pool de 256, SP 50 y sin sombras se cayeron igual | — |

**Qué leer primero en el log Development:**
- La hora de `LoadMap L_Parque` contra la hora del crash: cae al cargar, en el primer cuadro o a los X s.
- La última línea de `LogBlueprintUserMessages`.
- `LogD3D11RHI`/`LogRHI`: código `DXGI_ERROR_*` y si hay "Hung" o "Removed".
- `LogStreaming` y `Missing`/`fallback`.

## 3. Próximos 3 pasos
1. **(19-11 a.m., CTO)** Build Development: Borrar partida → D1. Recoger el log con la opción 7. Lectura: H2 y la hora exacta del crash.
2. **(19-11, engine-programmer / unreal-specialist)** PIE de `L_Parque` y `-game` sin cocinar (H1/H3). En paralelo, agregar D11–D16 (show flags + linterna) a `Probar.bat`.
3. **(20-11)** Arreglo según el resultado:
   - H1/H2: guardia en BeginPlay y la secuencia del glitch, y reset de `Partida.sav`.
   - H3/H5: reemplazar el material o actor culpable en el mapa.
   - H4: linterna simple en el parque.
   - Además, corregir el CHANGELOG 0.3.68.

## 4. Riesgo para las fechas
- **G0 22-11: riesgo ALTO.** Sin causa raíz al cierre del 20-11 no hay tiempo para 3×15 min estables. El 20-11 a las 20:00 se decide (producer): parque sí o no.
- **rc1 25-11: riesgo MEDIO.** El Plan B `Demo_SinParque` está listo en 1 min. El **Acto 3 en `L_Hospital`** (persecución, expediente, finales) no depende del parque: se adelanta desde el 20-11 para que la rc1 tenga final aunque el parque falle.
- La progresión (flags, `DT_Objetivos`, guardado v2) también es independiente: sigue el 21-11 en paralelo al diagnóstico. Los actos 2 y 3 del parque (cintas, cabina) se mueven a 23–24-11 solo si G0 pasa.
