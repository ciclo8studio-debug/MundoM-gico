# @tech-lead · Reingeniería 18-11-2026: arquitectura, iGPU, pipeline y dirección

Este documento parte de `auditoria/estudio_2026-11-18/tech-lead.md` y `PLAN_ESTUDIO_2026-11-18.md`, y no repite el diagnóstico.
Lentes: technical-director, lead-programmer, unreal-specialist, engine-programmer, performance-analyst, devops-engineer, producer y creative-director.

## 1. Visión objetivo
1. **El jugador queda como orquestador delgado:** entrada, cámara, vida y la fachada `Guardar`/`Cargar`. Cada sistema vive en su propio componente (`AC_*`) y el mundo se maneja con volúmenes y etiquetas, sin coordenadas.
2. **Cada migración es reversible sin abrir Unreal:** el interruptor `Arq_<Sistema>` en `DA_Tuning` elige entre la ruta vieja y la nueva.
3. **La calidad la decide la máquina:** 3 niveles, cada uno con un perfil de cvars cerrado, y bajada automática con aviso. Las sombras se deciden por nivel de calidad, nunca por mapa.
4. **Una build solo vale si arranca:** la prueba de humo automática y la versión única son parte del build. El cook lleva solo lo que entra en la demo.
5. **Creativamente:** el juego de linterna, ruido y escondite de una enfermera‑sombra en un hospital. Todo lo que no sirve a eso se corta de la demo.

## 2. Diseño objetivo

### 2.1 Partición de `BP_ThirdPersonCharacter` (hoy pesa 11,3 MB; medido en el `.uasset`)
```
BP_ThirdPersonCharacter (orquestador)  ── Input → llama a los componentes; CambiarCamara; vida/Revivir; Guardar/Cargar (fachada)
 ├─ AC_Linterna     ─► OnLinternaCambio(Encendida, Bateria01)
 ├─ AC_Interaccion  ─► OnFoco(Actor, Texto) · usa BPI_Interaction (la del template, decisión previa)
 ├─ AC_Inventario   ─► OnObjeto(Nombre)  · los flags siguen en BP_GI_MundoMagico (no se duplican)
 ├─ AC_Sigilo       ─► OnEscondido(bool), OnAliento(0..1) · coopera con AC_Ruido (ya existe)
 ├─ AC_Opciones     ─► OnCalidad(Nivel)  · AplicarOpciones/CalidadExtra/menú de configuración
 └─ AC_HUDBridge    ─► el ÚNICO que toca WBP_HUD/WBP_Menu; se suscribe a los dispatchers de arriba
Mundo (fuera del jugador): BP_GestorNivel (1 por mapa) · BP_VolumenZona · BP_TransicionNivel · BP_DirectorMiedo
```
| Componente | Funciones que se mueven desde el jugador | Estado que pasa a ser suyo | Timer (no Tick) |
|---|---|---|---|
| `AC_Linterna` | `ColocarLinterna`, `RefrescarLinternaMano`, `ActualizarBateria`, enfoque L2, sombra por calidad, inercia (SpringArm con lag) | `TieneLinterna`, `Bateria`, `Encendida` | 0,1 s (batería) |
| `AC_Interaccion` | `Interactuar`, `InteractuarZonas`, `ActualizarCerca`, `ActualizarCandado`, `UsarPuerta`, `UsarInterruptor`, `RecogerObjeto` | Actor enfocado | 0,15 s (traza de foco) |
| `AC_Inventario` | Objetos sueltos (botella, páginas, conteo); delega las llaves en `GI.Flags` | `Objetos[]`, `PaginasN` | — |
| `AC_Sigilo` | `PonerEscondido`, respiración contenida, aliento y FOV al correr | `Escondido`, `Aliento` | 0,1 s |
| `AC_Opciones` | `AplicarOpciones`, `CalidadExtra`, `AjustarGeneral`, `CargarGeneral`, `LineaGeneral`, `CfgSel*`, `RefrescarFila*`, calidad automática | `NivelCalidad` | 0,25 s (fps) |
| `AC_HUDBridge` | `MostrarSusto`, `MostrarFinal`, `OcultarPagina`, `OcultarCarga`, panel `-dev`, mensajes y objetivo/pista | Referencias a los widgets | 0,25 s (objetivo) |
| **Sale del jugador** | `ActualizarZona`, `RevisarSalida`, `RevisarFinDemo`, `RevisarFinal`, `Vigilar`, `EventoCasa`, `EventoCarrusel`, `ActualizarLluvia`, `AplicarReverb`, `SonidoLluvia` | → `BP_VolumenZona` (overlaps) / `BP_TransicionNivel` / `BP_GestorNivel` | overlaps |

**Reglas de la partición:**
- Un componente nunca llama a otro: se comunican por dispatchers o a través del dueño.
- Los valores salen de `DA_Tuning`; sin coordenadas, solo etiquetas y volúmenes.

### 2.2 Guardado: cómo no romperlo
- **No se renombra ni se borra** ninguna variable de `BP_Partida`, `Flags` ni `Opciones`; solo se agregan.
- El paso 0 agrega `Version:int`. Al cargar una partida con una versión menor que `Version_MinCompatible` (en `DA_Tuning`), se descarta con un aviso.
- `Guardar`/`Cargar` del jugador quedan como **fachada**: copian de y hacia los componentes con `GetComponentByClass`. Los slots y los campos no cambian, así que una partida de la v0.3.61 carga en la v0.4.x.

### 2.3 Escalabilidad iGPU: perfil por nivel (en `AC_Opciones.AplicarNivel`, con Execute Console Command y GameUserSettings)
| cvar / ajuste | Baja (por defecto en iGPU) | Media | Alta |
|---|---|---|---|
| `SetOverallScalabilityLevel` (sg.*) | 0 | 1 | 2 |
| `r.ScreenPercentage` | 70 | 85 | 100 |
| `r.AntiAliasingMethod` | 2 (TAA) | 2 | 4 (TSR) |
| `t.MaxFPS` | 30 | 30 | 60 |
| Sombra de la linterna (`CastShadows`) | **0** | 1 | 1 |
| `r.ContactShadows` / `r.DistanceFieldAO` | 0 / 0 | 0 / 0 | 1 / 1 |
| `r.MegaLights.Enable` | 0 | 0 | 1 |
| `r.VolumetricFog` · `r.BloomQuality` | 0 · 0 | 0 · 3 | 1 · 4 |
| `r.ViewDistanceScale` | 0,6 | 0,8 | 1,0 |
| Haz de la Sombra / polvo del haz | sin polvo | polvo | polvo |

- Los cvars de Lumen y de renderer **no** van en esta tabla, porque son Perfil A y están vetados (ver decisión 3).
- **Calidad automática** (la del HTML `perfTick`, adaptada): un timer de 0,25 s guarda 8 muestras de `GetWorldDeltaSeconds`. Se ignoran los 4 s después de cargar un mapa o cambiar de calidad. Si el promedio da **menos de 27 fps durante 6 s**, baja un nivel y aparece "Calidad gráfica: X (automática)".
  - Nunca sube sola.
  - Al primer arranque se usa Baja; el jugador puede subirla en Opciones.
  - Interruptor: `Calidad_Auto`.

**Presupuesto por mapa** (`stat unit` y `stat rhi` en `-dev`, medido en Baja):
| Mapa | fps Baja (prom / 1 % bajo) | Draw calls | Actores | Luces con sombra | RAM proceso | Peso en el cook |
|---|---|---|---|---|---|---|
| `L_Hospital` (con el kit `Horror_Hospital`) | 30 / ≥20 | <1 000 | <900 | ≤1 (linterna, solo desde Media) | <2,6 GB | <700 MB |
| `L_Casona` (teaser) | 30 / ≥22 | <600 | <400 | 0 | <2,2 GB | <150 MB |
| `L_Parque` (fuera de la demo) | 30 / ≥20 | <1 200 (hoy unos 1.400 actores, hay que hacer merge o HLOD) | <1 000 | 0 | <3 GB | — |

### 2.4 Pipeline objetivo (`Construir_Version.ps1` → 6 etapas)
```
[1 Pre] Unreal cerrado · ≥20 GB libres · Build\v<ver> NO existe (obliga a subir versión) · git del proyecto UE sin cambios sin commit
[2 Cook] mapas desde herramientas\mapas_demo.txt (L_Hospital [+L_Casona]) · log → Build_log_v<ver>.txt
[3 Humo] arranca MundoMagico.exe L_Hospital -ir=hospital -humo ; espera ≤90 s el slot "Humo" (ver abajo) ; mata el proceso
[4 Medir] tamaño de Paks y del ZIP estimado → append a Build\metricas.csv (ver, MB, humo OK/FALLO, minutos)
[5 Limpiar] deja las 3 builds más nuevas + la publicada (lista "Conservar.txt"); el resto se borra SOLO con -Limpiar y confirmando
[6 Resultado] "BUILD OK + HUMO OK" o un código distinto por etapa (exit 2 = cook, 3 = humo)
```
- **La prueba de humo funciona sin logs**, porque Shipping no los escribe. Con `-humo`, `BP_GestorNivel` espera 20 s y luego graba el SaveGame `Humo`, con la versión, el mapa, el promedio de fps y un bool por componente (cada `AC_*` marca `Listo` en su BeginPlay). Después ejecuta `QuitGame`.
  - El script revisa que `%LOCALAPPDATA%\MundoMagico\Saved\SaveGames\Humo.sav` sea nuevo, que contenga la versión y que no haya ninguna carpeta nueva en `Saved\Crashes`.
- **`DirectoriesToNeverCook` concreto.** Se cruzó el `AssetRegistry.bin` del cook de la v0.3.61; estas carpetas tienen 0 referencias:
  `/Game/Developers`, `/Game/Collections`, `/Game/LevelPrototyping`, `/Game/Maps`, `/Game/AutoFootsteps`, `/Game/HeartBeats`. Si el CTO saca el parque, se agregan `/Game/MundoMagico/Parque` y `/Game/GV_FreeShrubsPack`, pero solo después de comprobar en el cook que no hay referencias desde `L_Hospital`.
  - Aviso: con `-map=` el cook ya lleva solo lo referenciado. NeverCook es un **candado** contra referencias accidentales, no un ahorro.
- **Ahorro real medido:**
  - El cook compila **SM5 y SM6** (`D3D12TargetedShaderFormats`), aunque el juego corre en DX11. Los shaders SM6 ocupan unos 250 MB sin comprimir (`ShaderArchive-*-SM6`). Quitar SM6 del cook es la mayor rebaja posible (ver decisión 3).
  - Además, sacar `L_Parque` del `-map` ahorra unos 400 MB.
- **Versión:** la versión manda desde `DefaultGame.ini`. El script se niega a sobrescribir una build. `project.yaml` y `CHANGELOG` se revisan al final: si no coinciden, solo avisa.

## 3. Plan de migración (una pieza por build; patrón "estrangulador")
**Patrón por pieza:**
1. Se crea el `AC_X` y se añade al jugador.
2. La función vieja del jugador conserva su firma, y su primer nodo es `Branch(DA_Tuning.Arq_X)`: si es verdadero llama al componente; si es falso ejecuta los nodos viejos.
3. Después de **2 builds verdes**, se borran los nodos viejos y queda solo la llamada.

| Paso | Cambio concreto en BP o asset | Riesgo | Cómo se verifica en la build | Interruptor |
|---|---|---|---|---|
| 0 | `BP_Partida` + `Version:int`; `Cargar` descarta las versiones viejas | Bajo | Continuar con una partida de la v0.3.61 → carga; con `Version_MinCompatible` = 999 → aviso | `Version_MinCompatible` |
| 1 | Pipeline: etapas 1, 3 y 4 + slot `Humo` en `BP_GestorNivel` | Bajo | La build termina con "HUMO OK" y la línea en `metricas.csv` | `-humo` (solo por CLI) |
| 2 | `AC_Opciones`: perfil de 3 niveles + calidad automática + sombra de linterna por nivel (reemplaza `Linterna_SombrasParque`) | Medio (GPU) | Opción 1 de `Probar.bat`: fps en Baja ≥ los de la v0.3.61; forzar Alta en una iGPU → aparece el aviso automático | `Arq_Opciones`, `Calidad_Auto` |
| 3 | `AC_HUDBridge`: mover las funciones `Mostrar*`/`Ocultar*` y el panel `-dev` | Medio (widgets nulos) | Susto del pasillo, páginas y final de la demo se ven; el panel `-dev` muestra fps y tensión | `Arq_HUD` |
| 4 | `AC_Linterna` (+ inercia con SpringArm) | Medio | Recoger la linterna, encenderla, L2, que se agote la batería, cambiar la cámara 1ª↔3ª | `Arq_Linterna`, `Linterna_Inercia` |
| 5 | `AC_Interaccion` sobre `BPI_Interaction` | **Alto** (todo el juego interactúa) | Llave → `BP_DoorLocked`, escondite, página, interruptor y el candado ausente en el hospital (bug de la v0.3.53) | `Arq_Interaccion` |
| 6 | `AC_Sigilo` (escondido + aliento + FOV) | Medio | Opción 2 de `Probar.bat`: la Sombra no te ve escondido; la barra de aliento baja al correr | `Arq_Sigilo`, `Aliento_Activo` |
| 7 | `AC_Inventario` | Bajo | Recoger la botella o una página → el contador sube; Continuar las conserva | `Arq_Inventario` |
| 8 | `ActualizarZona`/`RevisarSalida`/`RevisarFinDemo` → `BP_VolumenZona` y `BP_TransicionNivel` en `L_Hospital` | Medio | Recorrido completo: cambia la reverb y la lluvia por zona, y aparece el final de la demo | `Arq_Zonas` |
| 9 | Limpieza: borrar los nodos viejos de los pasos 2–8 que lleven 2 builds verdes; `Arq_*` queda fijo en verdadero | Bajo | El `.uasset` del jugador pesa menos de 5 MB y la prueba de humo pasa | — |

- **Ritmo:**
  - Los pasos 0 y 1 van juntos en la primera build.
  - Desde ahí, **uno por build** e intercalados con las features del Sprint A y B. Nunca hay 2 migraciones en la misma build.
  - El paso 5 va **después** del freeze del 25-11 o en E6 si hay holgura. No conviene arriesgar la interacción antes de los testers.
- **Antes de cada paso:**
  - Se respalda `BP_ThirdPersonCharacter.uasset` (copia con la versión en el nombre).
  - En el DSL se borran los nodos de la función destino salvo la entrada.
  - Se mueve un nodo antes de guardar, para evitar que el MCP no marque el asset como modificado.
  - Se comprueba el `mtime`.

## 4. Qué eliminar o simplificar
- **Del jugador:** las funciones de sistemas que ya no existen en el mapa (candado, fonógrafo, reloj, caja fuerte y `EventoCasa`/`EventoCarrusel` cuando el parque salga de la demo). Las 8 filas `CfgRow*`/`CfgSel*` duplicadas pasan a **una sola función con índice**.
- **Duplicados:** las copias de `BP_ThirdPersonCharacter` en `FlashLight_System` y `Horror_Template` se mueven a una carpeta `_Archivo` con NeverCook, sin borrarlas. Hay que elegir una sola interfaz (`BPI_Interaction`) y no crear `BPI_Interactuable`.
- **`DA_Tuning`:** quitar `Linterna_SombrasParque`, porque lo reemplaza el nivel de calidad. Agrupar las claves con prefijos (`Arq_`, `Calidad_`, `Sombra_`…) y ordenar `GUIA_TUNING.md` igual.
- **Config:** borrar `DefaultEngine.ini.bak_basura` y las copias `*.antes_sprint1` cuando haya commit en la rama `unreal`. Quitar `+VulkanTargetedShaderFormats` (no se usa).
- **Repo:**
  - En `Build/` quedan 12 builds de 1,5 GB: conservar 3 + la publicada (unos 12 GB libres, con lista y OK).
  - Quitar del menú de `Probar.bat` las zonas del parque si sale de la demo.
- **Dirección creativa: 4 pilares**
  1. **La luz es tu vida y tu delator** (linterna, batería, el haz te delata y ahuyenta a la Sombra).
  2. **El sonido manda** (ruido del suelo, oído de la Sombra, drone de tensión y latido).
  3. **Esconderse y aguantar el aliento** (escondites y aliento).
  4. **El hospital cuenta la historia de Isidora** (páginas, flags y negatoscopio).
- **Qué se corta para la demo:**
  - Parque, payaso, noria, teatro, autos chocadores, el 2.º piso y la Casona jugable (queda el teaser).
  - Cámara en 3ª persona: deja de ser una opción probada; queda oculta detrás de `Camara_PrimeraPorDefecto`.
  - Apariciones de Isidora en el parque.
  - Cualquier feature que no sirva a un pilar se va al backlog de Steam.

## 5. Decisiones del CTO (máx. 3)
1. **Adoptar el patrón estrangulador `Arq_*`** (una migración por build, pasos 0–1 ya, el 5 después del 25-11). Recomiendo **sí**: es la única forma de partir el BP de 11 MB sin PIE y con vuelta atrás sin abrir Unreal.
2. **Demo = solo hospital + teaser de la Casona.** Saca `L_Parque` del cook y del `Probar.bat` público, y congela los 4 pilares. Recomiendo **sí** (unos 400 MB menos y sin el `GPUCrash` del parque).
3. **Cook sin SM6** (quitar `PCD3D_SM6` de `D3D12TargetedShaderFormats`; el juego ya arranca en DX11). Ahorra unos 250 MB de shaders y minutos de cook, y no toca la iluminación. Recomiendo **sí**, como excepción acotada del veto al Perfil A. Lumen y MegaLights siguen intactos hasta después del 10-12.
