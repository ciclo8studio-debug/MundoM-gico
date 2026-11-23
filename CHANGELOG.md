# Changelog — Mundo Mágico: Turno Nocturno

Formato basado en [Keep a Changelog](https://keepachangelog.com/es-ES/1.1.0/). Versionado `MAYOR.MENOR.PARCHE`.
La versión del juego vive en `Config/DefaultGame.ini` → `ProjectVersion`.

## Nota técnica — 2026-11-23 (noche) · El cuelgue tras el viaje parece falta de RAM (sin versión)

### Prueba de `v0.3.113_dev` (CTO: "se cerró al irse al parque")
- Hospital 13,5 min sin problemas; viaje a `L_Parque` a las 16:24:02; cuelgue de GPU a los 14 s. **Un solo fotograma del hilo de juego tardó 10,3 s** (del 185 al 186, de 16:24:10,9 a 16:24:21,2): la GPU integrada no contesta y Windows la reinicia (TDR de 10 s). Los pases de render cambian en cada cuelgue (decals, luces, `Substrate`), así que no es un efecto concreto.
- **Memoria en el cuelgue:** 790 MB libres de 7.931 MB; el juego usa 2.089 MB (pico 2.734 MB). En los cuatro cuelgues del 23-11 quedaban entre 790 y 1.240 MB libres. La GPU integrada comparte la RAM: con tan poca libre, sus recursos se paginan y un fotograma se alarga hasta tumbar el controlador.
- **Por qué solo tras un viaje:** el juego sube de ~260 MB a ~2,7 MB de pico; en una carga directa cabe, pero tras el viaje hay que cargar el mapa nuevo mientras se libera el anterior y se pasa el límite.
- **Línea base del PC con el juego cerrado:** 5.702 MB en uso y 2.229 MB libres. Mayores consumidores: app de Claude 1.362 MB, `svchost` 580, Edge 259, Defender 210, Epic Launcher y overlay ~200.
- **Prueba para confirmar:** repetir hospital → parque con la RAM libre ≥ 4 GB (cerrar Claude, Edge y el Epic Games Launcher). Si no se cuelga, la causa es la memoria y hay que bajar el consumo del juego tras el viaje (texturas, distancia de vista, follaje) y declarar un mínimo de 16 GB, o 8 GB con todo cerrado.
- **Linterna:** en 13,5 min no apareció ningún mensaje "LINTERNA: apuntando a la Sombra" (0 en el log): la condición de apuntado no se cumplió nunca.

## [0.3.113] — 2026-11-23 · Sprint A: sonido de recoger y diagnóstico de la linterna

### Pedido (feedback del CTO sobre el Sprint A)
- **Sonido de recoger (A8/pistas):** "cuando aprieto el botón suena aunque no agarre nada". `RecogerObjeto` reproducía `SW_Pickup_small` y `SFX_Pagina` al inicio, sin comprobar la distancia. Ahora la `Rama` inicial exige `ListaRecogibles` no vacía **y** el objeto más cercano a menos de 200 cm (`FindNearestActor.Distance < 200`); si no, no hace nada (ni sonido ni temporizador `AntesDePagina`).
- **A3 linterna (no ahuyentaba a la Sombra):** la lógica de `BP_AI_Sombra.RevisarLinterna` (timer 0,2 s) es correcta: exige `Sombra_RepeleLinterna`, linterna encendida, producto punto de la mira con la dirección a la Sombra > 0,96 (unos 16°) y distancia < 1800 cm; suma 0,2 s por revisión y resta 0,1 si dejas de apuntar; huye al llegar a `Sombra_RepeleSeg`. Valores en `DA_Tuning`: todo activo, `Sombra_VistaDist` 2200, `Sombra_PausaGolpe` 3.
  - **Cambio:** `Sombra_RepeleSeg` de 2,5 a **1,5 s** (`herramientas/ajustar_tuning.py`, confirmado en el log).
  - **Diagnóstico temporal:** dos `PrintString` (pantalla y log) en `RevisarLinterna`: "LINTERNA: apuntando a la Sombra" cuando se cumple la condición y "LINTERNA: la Sombra huye" al llegar al tiempo. Quitarlos cuando A3 pase el humo.
- **Pistas:** la tecla es **Y** (teclado); con mando, solo la pista automática a los 180 s. El plan decía H; el kit de testers ya dice Y. Botellas: **T** (R2 con mando). Pendiente ubicar dónde hay botellas en los mapas.
- **Probar:** apretar □/E sin nada cerca (sin sonido) y recoger algo (con sonido); en un encuentro con la Sombra, encender la linterna con F y apuntarla: deben salir los mensajes en pantalla.

## Nota técnica — 2026-11-23 · Cuelgue de GPU tras el viaje hospital → parque (sin versión)

### Prueba de `v0.3.112_dev` (CTO: "se cayó, se cerró solo")
- El viaje funcionó: a las 14:36:31 el juego cargó `L_Parque` justo después de la puerta (`Salida_Y` = -4600). Luego, a las 14:36:54 (23 s después), la GPU se colgó (`DXGI_ERROR_DEVICE_HUNG`, falla al crear texturas BC5 de 1024). Entre las 14:36:35 y las 14:36:54 el juego dibujó solo 11 fotogramas (~1,7 s cada uno) y un fotograma tuvo `Max iterations 8 hit` (>95 ms). RAM libre al abrir: 2,4 GB (no es falta de memoria).
- El volcado de calidad es idéntico en la carga directa del hospital, del parque y del parque por viaje (`ScreenPercentage` 80, `VolumetricFog` 1, `Shadow.MaxResolution` 2048): la calidad no cambia al viajar.
- Cuatro cuelgues del 23-11 comparten patrón: parque → Casona (70 s), hospital → hospital final (53 s), hospital → parque (23 s) y la sesión 2 del G0. Las cargas directas (hospital 32 min, parque 10 min, Casona 7 y 10 min) no se colgaron nunca.
- **Primer glitch (`GlitchLlegar`):** teletransporta a (0, -4300, 100) con la cámara en fundido: es la calle de afuera (parte del mapa del hospital). El segundo glitch (TV, `IniciarTV`) abre `L_Parque` al pasar `Salida_Y`. `IniciarTV` carga `DA_Tuning` con `LoadAsset_Blocking` y anima la TV con un temporizador de 0,02 s; no se encontró nada que explique la carga de GPU.
- **Prueba sugerida:** `Probar.bat` → P → **D5** (juego normal con el perfil sin Lumen/VSM/GI) y repetir el recorrido hasta el parque: si no se cuelga, la causa es la iluminación dinámica tras cambiar de mapa.

## [0.3.112] — 2026-11-23 · El viaje al parque ocurre justo después de la puerta de Urgencias

### Pedido (prueba de `v0.3.111_dev`, CTO: "sí, adelanta")
- **Prueba de la 0.3.111:** log limpio (2,6 min en `L_Hospital`, 0 cuelgues, 4 avisos menores). El glitch se activó al pasar la puerta (captura en Y -4302, afuera, de noche), pero el viaje a `L_Parque` esperaba hasta Y -12600, unos 83 m más adelante.
- **`Salida_Y`: -12600 → -4600** en `DA_Tuning` (`herramientas/ajustar_tuning.py`, confirmado: `TUNING_OK Salida_Y = -4600.0`). `BP_GestorNivel.VigilarSalida` manda a `L_Parque` al pasar esa Y con `SalidaHecha`, así que el viaje ocurre unos 3 m después de la puerta (el glitch parte en Y -3470; la puerta está cerca de Y -4300).
- **Riesgo:** el cambio de mapa es donde se colgaba la GPU (53 a 70 s después de cargar). Probar con la RAM despejada y anotar si se cuelga en el parque.
- **Si no basta con la puerta:** `Salida_Y` admite de -13000 a -4000; para volver al recorrido largo, poner -12600.

## [0.3.111] — 2026-11-23 · El glitch de salida del hospital ya no se salta

### Pedido (prueba de `v0.3.110_dev`, CTO: "no me mandó a ningún lado porque salté y llegué al hospital")
- **Causa:** `RevisarSalida` (jugador, cada tick) solo marcaba `SalidaHecha` si la Y del jugador estaba en la franja -3620 a -3470 (150 cm). Con un salto de prueba, un reinicio o al reaparecer más adelante (el CTO estaba en Y -5197) la franja se salta, `SalidaHecha` nunca se activa y `BP_GestorNivel.VigilarSalida` (que exige `SalidaHecha` y pasar `Salida_Y` = -12600) no manda al parque.
- **Arreglo:** el mínimo de la franja pasa de -3620 a **-30000**: el glitch se activa en cuanto la Y es -3470 o menor, sin importar cómo se llegó. Sin interruptor nuevo (es una guarda).
- **Diseño que se mantiene:** glitch desde Y -3470; el viaje a `L_Parque` ocurre al pasar `Salida_Y` (-12600), unos 70 m más adelante. Si se quiere que el viaje ocurra al salir por Urgencias, hay que bajar `Salida_Y` (pendiente de decisión del CTO: falta confirmar la Y de la puerta de Urgencias).
- **Riesgo:** en el Acto 3 (regreso al hospital) cualquier zona con Y menor a -3470 activaría el glitch al entrar; hoy el regreso apunta al Pabellón 4 (Y cercana a 0). Probar el regreso desde el parque.
- **Probar:** Iniciar juego → pasillo; saltar (o reaparecer) más allá de Y -3470 debe activar el glitch; seguir hasta Y -12600 manda al parque.

## [0.3.110] — 2026-11-23 · El parque vuelve a la demo (`Demo_SinParque` falso)

### Decisión del CTO ("reconecta") tras probar el Plan B
- Con `Demo_SinParque` activo (0.3.109) el glitch mandaba directo al hospital final y mostraba FIN. El CTO quiere el recorrido completo: hospital → glitch → **parque** → TV → hospital final → FIN. `Demo_SinParque` vuelve a **falso** (`herramientas/ajustar_tuning.py`, confirmado en el log: `TUNING_OK Demo_SinParque = False`).
- **Hallazgo del log de la sesión Plan B (13:27–13:32):** nuevo cuelgue de GPU (`DXGI_ERROR_DEVICE_HUNG`, pase `BatchedLights/DirectLighting`, frame 5573) a los 53 s de recargar `L_Hospital` para la escena final; 1,1 GB de RAM libre. Junto al cuelgue parque → Casona (70 s), el patrón es: **cuelgue de GPU ~1 minuto después de cualquier cambio de mapa**. Sin cambio de mapa nunca se colgó (hospital 32 min, Casona directa 7 y 10 min, parque 3 min). Plan B no evitaba el problema. El volcado de calidad es idéntico en la primera y la segunda carga, así que no es una subida de calidad.
- **Pendientes de esta versión (sin tocar aún):** título "PARQUE MUNDO MÁGICO" en la sala final del hospital (`ActualizarZona` elige el nombre por coordenadas y esa sala cae al valor por defecto) y minimapa del parque en esa sala (no ubicado todavía).
- **Prueba sugerida:** hospital → glitch → parque con la RAM despejada (navegador y apps cerrados) y anotar si el juego se cuelga ~1 minuto después de cargar el parque.

## [0.3.109] — 2026-11-23 · Plan B: el parque sale de la demo (`Demo_SinParque` activo)

### Decisión del CTO ("Plan B ya") tras el G0
- **G0 del parque no pasó.** Sesión 2 del G0 (13:05): el parque corrió 3 minutos bien (25 FPS o más); a las 13:08 el portón del castillo viajó a `L_Casona` y a las 13:09:15 la GPU se colgó (`DXGI_ERROR_DEVICE_HUNG`, pase `DeferredDecals Emissive`, 1,2 GB de RAM libre). Detalle en `auditoria/humo/g0_parque_2026-11-23.md`.
- **`Demo_SinParque` = verdadero** en `DA_Tuning` (con `herramientas/ajustar_tuning.py`, sin abrir el editor; confirmado en el log: `TUNING_OK Demo_SinParque = True`).
- **Recorrido de la demo:** hospital → pasillo de Urgencias → glitch y TV → pantalla "FIN DE LA DEMO" (con `UI_FinalDemo` activo) → menú. La Casona y el parque quedan fuera del recorrido normal y pasan a la versión completa. `Probar.bat` → 8 (Casona) y P (parque) siguen sirviendo para pruebas internas.
- **Pendiente de decisión del CTO:** si el adelanto de la Casona entra a la demo pública (por ejemplo TV → Casona directa, sin pasar por el parque). Hoy no entra.
- **Riesgo anotado:** el tránsito parque → Casona cuelga la GPU con poca RAM libre; hay que resolverlo antes de la versión completa (decals emisivos y memoria al cambiar de mapa).
- **Probar:** hospital completo hasta el glitch y la TV: aparece la pantalla FIN y vuelve al menú; "Continuar" no manda al parque.

## RC1 — 2026-11-23 · congelamiento de la Casona visual (contenido = v0.3.108)

### Decisión del CTO
- "Sí, arma la RC1": la Casona visual queda **congelada** desde la v0.3.108 (exterior de castillo, luna y estrellas, luz de ventanas, agachado con truco de escala). No se agregan cambios visuales hasta después de la demo del 10-12.
- Build RC1: `Build/v0.3.108/Windows` (Shipping) y `Build/v0.3.108_dev/Windows` (con panel de FPS). Mismo contenido; la versión del juego sigue en 0.3.108.
- Siguiente paso: prueba de humo A9 con `auditoria/humo/rc1_2026-11-24.md`, G0 del parque con `auditoria/humo/g0_parque_2026-11-23.md` y, si pasa, envío a testers con `auditoria/kit_testers_2026-11-26.md`.
- Solo entran a la RC2 las correcciones de lo que falle en el humo o en los testers.

## [0.3.108] — 2026-11-23 · Higiene de logs: `RecogerObjeto` sin avisos (sin build)

### Origen (log final de `v0.3.107_dev` y reunión corta del `producer`)
- 36 avisos `Accessed None ... CallFunc_FindNearestActor_ReturnValue` por sesión de ~10 min: al apretar □/E sin objetos recogibles (`ListaRecogibles` vacía, como en la Casona), `RecogerObjeto` seguía leyendo un actor nulo.
- Arreglo: `Rama` al inicio de `RecogerObjeto` con `ListaRecogibles` → `IsNotEmpty`; si está vacía no hace nada (tampoco suena el sonido de recoger). Si no está vacía, el flujo es el de siempre.
- No se tocó `AntesDePagina` (aviso `SetTimer` en cero, 3 por sesión, inocuo) ni `LimpiarMensaje`.
- Probar: en la Casona apretar □/E sin nada cerca (0 avisos y sin sonido) y en el hospital/parque recoger llave, página y botiquín (debe seguir igual).
- Sin interruptor en `DA_Tuning`: es una guarda de seguridad, no una feature.

## [0.3.107] — 2026-11-23 · Casona: luz de ventana sobre las ventanas con marco (sin build)

### Pedido (prueba de `v0.3.106_dev`, CTO: "aún no están bien posicionadas")
- Las arcadas (`EXT_Ventanas`, `SM_Cast_Ventanas`) quedaban entre las filas de ventanas con marco de la textura (`T_Building_Diffuse`, triplanar de 500 cm). Quedan ocultas (`bHidden`), no borradas.
- Medido en el editor con `CaptureViewport` (cámara y=-2000, z=400): las ventanas de la fachada sur están en x = +787, +289, -212, -713 (cada 500 cm) y en z ≈ 277 (planta baja) y z ≈ 780 (planta alta); la puerta ocupa x ≈ ±106.
- 6 cristales nuevos `VentLuz_A1..A4` (planta alta, las 4 ventanas) y `VentLuz_B1..B2` (planta baja, solo los extremos para no tapar la puerta): cubos 100×5×170 cm en y=-33 (pared sur en y=-30) con `MI_Ventana_Casona` (Intensidad 0,7), sin sombra.
- Si quedan muy brillantes: bajar `Intensidad` de `MI_Ventana_Casona` a 0,4. Para más ventanas: copiar un cristal a x/z de las otras (planta baja x=289 y x=-212; torres x=±1085).
- Pendiente: las caras este, oeste y norte siguen sin luz en ventanas (solo se hizo la sur).

## [0.3.106] — 2026-11-23 · Casona: las arcadas dejan de quemarse a blanco (sin build)

### Pedido (prueba de `v0.3.105_dev`, CTO: "las ventanas aún no las ajustas")
- Causa real del blanco: `M_Emisivo` multiplica el color por el parámetro `Intensidad` (por defecto **8**), así que el color tenue de `MI_Ventana_Casona` (0,5/0,26/0,08) salía a 4/2/0,64. La 0.3.103 no lo había visto.
- Arreglo: `Intensidad` = 0,7 en `MI_Ventana_Casona` (resultado ≈ 0,35/0,18/0,06: vidrio cálido tenue). `MI_Ventana_Calida` del Parque no se toca.
- Si aún se ven brillantes: bajar `Intensidad` a 0,4; si quedan apagadas: subir a 1,0.
- La forma de arco sigue distinta a las ventanas con marco de la pared (son malla aparte); si se quiere igualar, cambiar la malla.
- `v0.3.105_dev`: log limpio y Isidora ya no se mueve sola (el domo sin colisión funcionó); agachado aún sin confirmar.

## [0.3.105] — 2026-11-23 · Casona: Isidora se movía sola (colisión del domo de estrellas) (sin build)

### Hallazgo (prueba de `v0.3.104_dev`, CTO: "se mueve sola Isidora")
- Causa: la esfera `ESTRELLAS_Domo` (v0.3.102, `/Engine/BasicShapes/Sphere` ×500) conservaba la colisión (`collisionEnabled = QueryAndPhysics`) aunque el perfil decía `NoCollision`. Isidora aparece dentro de ese volumen sólido y el motor la empujaba hacia afuera (de ahí el "se pega y se mueve sola", el avance hacia Y negativo y la cámara metida en el brazo). Confirmado con una consulta de colisión en el editor sobre el punto de aparición: devolvía el domo.
- Arreglo: el domo usa `/Engine/EngineSky/SM_SkySphere` (sin colisión; la misma consulta ahora devuelve vacío), escala ×30 (radio ~123.000 cm), mismo material `M_Estrellas`.
- Log de `v0.3.104_dev`: 0 avisos de script, sin cuelgues, solo teclado y mouse (no era un mando con deriva).
- Notas: el agachado (0.3.104) y las ventanas (0.3.103) quedan por probar sin el empuje del domo.

## [0.3.104] — 2026-11-23 · Isidora: agachado visual con truco de escala (sin build)

### Pedido (CTO: "Truco ahora")
- `Agachar` (tras `Crouch`) escala la malla a (1, 1, 0,72); `Levantar` y la salida por salto (`UnCrouch` del EventGraph) la devuelven a (1, 1, 1). La escala parte de los pies, así que no se hunde.
- Es un truco: no reemplaza una animación real de agachado (ver 0.3.103).
- Pendiente: interruptor en `DA_Tuning` (hoy el valor 0,72 está fijo en el BP) y animación real por retarget.
- Probar: agacharse, caminar agachada, levantarse y saltar agachada; revisar que no quede achatada.

## [0.3.103] — 2026-11-23 · Casona: ventanas del castillo con vidrio cálido tenue (sin build)

### Pedido (prueba de `v0.3.102_dev`, CTO)
- "Más luz y estrellas se ven bien." Con la exposición nueva (0,55) las arcadas de `SM_Cast_Ventanas` se quemaban a blanco con `MI_Ventana_Calida` (emisivo 2,2/1,2/0,45) y no calzaban con las ventanas con marco de la fachada.
- Nuevo `MI_Ventana_Casona` (padre `M_Emisivo`, color 0,5/0,26/0,08) aplicado solo a `EXT_Ventanas` de la Casona; el material del Parque no se toca.
- Si aún se ven distintas a las otras ventanas: pasar a una malla con marco o bajar el color a 0,35.

### Hallazgos sin arreglar
- **Isidora no se agacha (visual):** `Agachar` sí llama a `Crouch` y `bCanCrouch` está en true (cápsula 40 de media altura, velocidad 200), pero `ABP_Isidora` no tiene variable ni animación de agachado y el proyecto no trae ninguna animación de agacharse. Solo cambia la cápsula y la cámara. Falta una animación (retarget) o un truco procedural.
- **Se pegó y se movía solo:** en el log de `v0.3.102_dev` hay un tirón de carga (`FlushAsyncLoading`) y un cambio de teclado (pérdida de foco) justo antes; probable tecla pegada al perder el foco. Sin confirmar.

## [0.3.102] — 2026-11-23 · Casona: más luz y cielo con estrellas (sin build)

### Pedido (prueba de `v0.3.101_dev`, CTO: "la luna se ve bien, pero está muy oscuro, quizás estrellas")
- Luna y halo se ven bien; FPS 28; sin cuelgues (log limpio).
- `PostProcessVolume_0`: exposición mín/máx 0,8 → 0,55 (más claro; el hospital usa 0,4).
- `LUNA_Luz`: intensidad 3 → 6 lux.
- Nuevo `M_Estrellas` (Unlit, aditivo, doble cara, solo en el emisivo: puntos por hash, más densos hacia arriba, sin textura ni Tick) en `ESTRELLAS_Domo` (esfera ×500 sin colisión ni sombra, carpeta `Luna`).
- Si queda muy claro: subir `autoExposure` a 0,65. Si las estrellas se ven pocas: bajar el umbral `0.9963` del material.
- Pendiente: interruptor en `DA_Tuning` (hoy es solo de mapa).

## [0.3.101] — 2026-11-23 · Casona: se apaga la niebla volumétrica (cuelgue de la GPU) (sin build)

### Hallazgo (prueba de `v0.3.100_dev`, CTO: "abre el juego dev")
- El juego se cerró solo a los ~2 min, las dos veces que se abrió. Log: `GameThread timed out waiting for RenderThread after 120.00 seconds`; breadcrumbs del RHI: `VolumetricFog::LightScattering → VolumetricFog → ComputeVolumetricFog`. La iGPU (Iris Plus, 8 GB) se cuelga calculando la dispersión de la niebla volumétrica con la luz direccional de 3 lux con sombras. El cuelgue empieza al cargar el mapa, antes de ver la pantalla.
- Pasar `-dpcvars=r.VolumetricFog=0` no sirve: el juego vuelve a poner `r.VolumetricFog` en 1 al arrancar (el log imprime `r.VolumetricFog = "1"`; probablemente la función de calidad del BP del jugador). Quedó el mismo cuelgue.
- Fue el riesgo que se anotó en la v0.3.100 ("la niebla volumétrica cuesta en la iGPU"), pero no pensé que fuera un cuelgue y no un bajón de FPS.

### Cambiado (`L_Casona`)
- `ExponentialHeightFog_0`: `bEnableVolumetricFog` en **falso** (vuelve a como estaba antes de la v0.3.100).
- `LUNA_Luz`: `VolumetricScatteringIntensity` de 4 a **0**.
- Siguen igual: la luna con relieve (`M_LunaReal`), el halo (`M_LunaHalo`), la luz de 3 lux con sombras de 2 cascadas y el corte de niebla a 20.000.

### Pendiente
- Los **haces de luna** reales (niebla volumétrica) quedan descartados en esta PC. Alternativas baratas sin volumétrica: `bEnableLightShaftBloom` en la luz (resplandor de pantalla alrededor de la luna, sin pase volumétrico), o mallas de rayos aditivas (conos translúcidos) junto a la puerta y detrás de la casa. Ninguna está hecha.
- Sin probar: hay que confirmar que ya no se cuelga, ver la luna y la luz, y los FPS (referencia 32 en `v0.3.99_dev`).

## [0.3.100] — 2026-11-22 · Casona: luna real, luz de luna y haces de luz (sin build)

### Hallazgo (prueba de `v0.3.99_dev`, CTO: "luna y luz de luna reales, como una noche de terror")
- La luna se veía como un disco blanco liso (`MI_Luna` y `MI_HaloLuna` son el mismo material emisivo plano, `M_Emisivo`, intensidad 6 y 1,2): sin relieve ni brillo alrededor. El exterior casi no recibía luz (0,6 lux) y la prueba bajó a 32 FPS (con 41 antes de la luz).
- Error mío en la v0.3.99: la Casona tiene la exposición fija en 0,8 y el hospital en 0,4 (más alto = más oscuro), o sea que la Casona necesita **más** lux que el hospital, no menos.
- No se pudo usar el cielo con luna de `SoulCity` (`T_UN_Sky_SM_Overcast01_Bonus` en `M_SkyClouds`): no hay forma de ver su textura ni de saber en qué punto del domo queda la luna.

### Añadido / cambiado (`L_Casona`, `Content/MundoMagico/Materials`)
- **`M_LunaReal`** (material nuevo, sin luz, sin Event Tick): nodo `Custom` (HLSL) sobre UV de la esfera, normal y vector de cámara. 5 octavas de ruido dan los mares oscuros y las zonas claras, puntos de cráter por hash, y oscurecimiento hacia el borde (limb). Brillo 0,9 (antes 6 plano). Compila sin errores.
- **`M_LunaHalo`** (material nuevo, aditivo, sin luz): resplandor azul frío con centro blanco que se desvanece hacia el borde (`pow(N·V)`).
- `LUNA_Disco` usa `M_LunaReal`; `LUNA_Halo` usa `M_LunaHalo` y quedó concéntrico con el disco (15000, −65000, 33000), escala 150.
- **`LUNA_Luz`**: intensidad de 0,6 a **3,0 lux**, color (0,60 / 0,72 / 1,0), orientada exacto desde la luna (`pitch −26,3°`, `yaw 103,0°`), `VolumetricScatteringIntensity` 4.
- **Niebla volumétrica** de `ExponentialHeightFog_0` activada (estaba apagada): dispersión 0,55 (hacia adelante, para que se vean los haces), extinción 0,5, distancia 4.000. La resolución del juego ya era baja (`r.VolumetricFog.GridPixelSize=16`, `GridSizeZ=64`).
- Captura del visor del editor: luna con halo azul suave, el relieve no se apreció (el editor tiene autoexposición y el disco salía quemado; el brillo se bajó de 2,6 a 0,9 después de esa captura).

### Pendiente
- Sin probar en el juego: (1) que el disco muestre manchas y cráteres, (2) que el exterior se vea azulado con la luz de 3 lux sin quemarse, (3) que se vean haces de luz en la niebla, (4) los **FPS** (32 en la `v0.3.99_dev`; la niebla volumétrica y sombras de 3 lux cuestan en la iGPU).
- Si pesa: `bEnableVolumetricFog` en falso (se pierden los haces), 1 cascada de sombra, o `VolumetricFogDistance` 2.500.
- Los "haces de luna" dentro de la casa por las ventanas no existen aún: las ventanas son mallas emisivas, no huecos. Solo se ven los haces fuera (siluetas en la niebla) y por la puerta.

## [0.3.99] — 2026-11-22 · Casona: luz de luna (sin build)

### Añadido (`L_Casona`, carpeta `Luna`)
- **`LUNA_Luz`**: `DirectionalLight` (la Casona no tenía ninguna). Azul (0,33 / 0,47 / 0,73, el mismo color de la del hospital), **0,6 lux** (el hospital usa 1,2; la Casona tiene la exposición fija en 0,8, o sea la mitad de brillante que el hospital, por eso la mitad de intensidad), `Mobility = Movable`, con sombras.
- Orientada desde la luna (que está al sur, `LUNA_Disco`): `pitch −27°`, `yaw 103°`. La luz cae hacia el norte, sobre el frente de la casa (el lado de la puerta).
- Para cuidar la iGPU (41 FPS en la `v0.3.97_dev`): sombras de **2 cascadas** y distancia de sombra **4.000** (por defecto eran 4 y 40.000). Los muros siguen proyectando sombra, así que el interior no debería recibir luz salvo por las ventanas y la puerta.
- Los valores por defecto de la luz nueva (10 lux, blanca, `Stationary`) se cambiaron antes de guardar.

### Pendiente
- Sin probar: hay que mirar (1) que el exterior se vea azulado y no se queme, (2) que el interior siga oscuro y (3) el costo en FPS (referencia 41). Si pesa: bajar a 1 cascada, a `CastShadows` falso solo si el interior no se ilumina, o quitar `LUNA_Luz`.
- Si el exterior queda muy oscuro, subir `Intensity` a 1,0.

## [0.3.98] — 2026-11-22 · Casona: luna en el cielo (sin build)

### Hallazgo (prueba de `v0.3.97_dev`, CTO: "podemos agregar una luna a ese nivel")
- Se salió por la puerta y se vio la fachada (41 FPS, log con 0 avisos de script y 0 errores de GPU). El cielo de la Casona estaba vacío. En el hospital la luna es un disco + un halo: `LUNA_Disco` (esfera `Sphere` 60 × 60 × 60, `MI_Luna`, en 0, 65500, 45900) y `LUNA_Halo` (esfera 150, `MI_HaloLuna`, un poco detrás), más una `DirectionalLight` "Luna" (azul, 1,2 lux, con sombras).

### Añadido (`L_Casona`, carpeta `Luna`)
- `LUNA_Disco` en (15000, −65000, 33000) y `LUNA_Halo` en (15800, −68500, 34760), mismas escalas y materiales que en el hospital. Quedan al **sur**, a ~74.000 de distancia y ~27° de altura, para verlas al salir por la puerta de frente. Desde la fachada mirando al norte no se ven (las tapa la casa).
- `ExponentialHeightFog_0`: `FogCutoffDistance` de 0 a **20000**. La niebla (densidad 0,09, opacidad máxima 1) habría tapado por completo la luna; nada más en el nivel está más allá de 20.000.
- **Sin `DirectionalLight`**: la luz azul con sombras del hospital no se copió (costo en la iGPU y riesgo de iluminar el interior). La luna es solo visual.

### Pendiente
- Sin ver en pantalla (el visor del editor no sirve para capturas). Si no se ve: revisar `MI_Luna` y el corte de niebla; si se quiere más cielo, subir la luna o moverla al norte.
- Opcional: luz de luna tenue y sin sombras sobre el exterior.

## [0.3.97] — 2026-11-22 · Texturas más nítidas: `MipFilter=linear` (sin build)

### Hallazgo
- En la Casona las texturas se veían por bloques (captura del CTO, `v0.3.92_dev`, 21 FPS). Los grupos de texturas de `DefaultDeviceProfiles.ini` usaban `MipFilter=point`: el paso entre niveles de detalle (mips) es abrupto y se nota como bloques o bandas.
- Prueba en `Build\v0.3.95_dev` (con el cambio solo durante el build): "quedó mejor", 24 FPS en el panel `-dev` (antes 21). Sin cierres de GPU en esa prueba.

### Cambiado (`Config/DefaultDeviceProfiles.ini`, autorizado por el CTO el 22-11)
- `MipFilter=point` → **`MipFilter=linear`** en los 13 grupos `TextureLODGroups` (World, Character, Weapon, Vehicle, Effects, etc. y sus mapas normal/specular). Sigue `r.Streaming.PoolSize=300`, `MipBias=1` y `MaxAnisotropy=4`: no cambia la memoria reservada.
- **Cómo volver atrás** (no hay interruptor en `DA_Tuning`: es configuración del perfil de dispositivo): `git revert` de este commit o poner otra vez `MipFilter=point`.

### Pendiente
- Verificar en una build (Shipping y `_dev`) que la mejora se mantiene en el hospital y el parque y que no suben los cierres de GPU. Si hace falta más nitidez, probar `PoolSize` 450 con OK del CTO.

## [0.3.96] — 2026-11-22 · Casona: la puerta del frente ya no se bloquea (sin build)

### Hallazgo (prueba de `v0.3.95_dev`, CTO)
- "No puedo salir de la casona, algo bloquea la entrada." Consulta de solapes con canales WorldStatic/WorldDynamic/Pawn en la zona de la puerta (x ±100, y −150…60, z 5…240): el único actor que aparecía era `EXT_Neon` (la malla `SM_Cast_Neon` de la v0.3.94, perfil `BlockAll`, ~14 cm de grosor y 14 m de ancho). Mis trazas verticales de la v0.3.94 no lo detectaron porque van por el canal de visibilidad.
- No fue posible desactivar su colisión desde el editor por MCP (`BodyInstance.collisionEnabled` no se aplica; el perfil `NoCollision` tampoco la quitó en el consulta).

### Cambiado (`L_Casona`)
- Se **eliminó `EXT_Neon`**. Quedan las torres, los techos azules, el revestimiento de piedra y `EXT_Ventanas` (perfil pasado a `NoCollision`, pero ahí tampoco se confirmó el efecto; está a z 405–597 y y −42…−38, fuera del paso).
- Verificado con la misma consulta: sin actores con colisión en la puerta (y −150…60), en el recibidor (y 10…400) ni en el exterior inmediato (y −900…−30).

### Pendiente
- La fachada pierde el letrero de neón rosa. Si se quiere recuperar: reponerlo con colisión desactivada desde el editor de la malla (`SM_Cast_Neon` → colisión: ninguna) y volver a colocarlo.
- Sin probar caminando hasta afuera. Fuera de la casa el suelo es el plano grande `StaticMeshActor_235` (z ≈ −22): hay un escalón hacia abajo de ~22 cm al salir.

## Prueba de nitidez — `Build\v0.3.95_dev` (22-11, no es una versión nueva)

- Esa carpeta es la `_dev` de la v0.3.95 compilada con **`MipFilter=linear`** en los 13 grupos de `DefaultDeviceProfiles.ini` (el proyecto tiene `point`). Autorizado por el CTO el 22-11 para probar los bloques de textura vistos en la Casona.
- El `ini` se cambió solo durante el build y se restauró (`git status` limpio): el proyecto y el repo siguen con `point`, `PoolSize=300` y `MipBias=1`.
- Qué mirar: ¿desaparecen los bloques de las texturas en la Casona? ¿Cuántos FPS (antes 21 con el panel)? Si mejora, se decide subir `PoolSize` a 450 y/o dejar `linear` en la v0.3.96 con interruptor.

## [0.3.95] — 2026-11-22 · Menos avisos de script: `RevisarCines`, `CineVer` y `AngelPayaso` (sin build)

### Hallazgo (log de `v0.3.92_dev`, Casona, 122 avisos en total tras los arreglos de 0.3.90–0.3.92)
- 108 avisos en `RevisarCines` y 1 en `CineVer` (`index 0` de `OutActors` con largo 0) y 13 en la variable `Payasos` (la función no figuraba en el log; era `AngelPayaso`). Mismo patrón que `MoverMuneca`: la `Rama` pide `IsValid(lista[0])` y el `Get(0)` se evalúa aunque la lista esté vacía (la Casona no tiene payasos ni cines).

### Cambiado (`BP_ThirdPersonCharacter`)
- **`RevisarCines`**, **`CineVer`** y **`AngelPayaso`**: antes de la `Rama` que lee el índice 0, una `Rama` nueva con `Utilidades|Matriz|IsNotEmpty` sobre la misma lista (`GetAllActorsWithTag` en las dos primeras, `Payasos` en la tercera). Con actores el camino es el de antes; sin ellos termina sin leer nada. Sin Event Tick nuevo.

### Pendiente
- Sin probar en juego. Con esto el log de la Casona debería quedar sin avisos de script; falta confirmarlo en una `_dev`.

## [0.3.94] — 2026-11-22 · Casona: exterior tipo castillo (sin build)

### Hallazgo
- El castillo del parque (`L_Parque`, frente al portón en 0,-12140) es de formas básicas: cuerpo `Cube` 14 × 10 × 10 m con `MI_Piedra`, 4 torres `Cylinder` (3 × 3 × 14 m, `MI_Piedra`) con techos `Cone` (`MI_Pintura_Azul`), puerta `MI_Oscuro` y dos mallas de fachada: `SM_Cast_Neon` (`MI_Neon_Rosa`) y `SM_Cast_Ventanas` (`MI_Ventana_Calida`).

### Añadido (`L_Casona`, carpeta del outliner `Castillo_Exterior`, 22 actores `EXT_*`)
- **Revestimiento de piedra por fuera de los muros** (`MI_Piedra`, 20 cm de grosor, pegado por fuera: oeste, este, norte y los 3 tramos del frente que dejan libre la puerta). El interior no cambia.
- **4 torres de esquina** (`Cylinder`, 4 × 4 × 15 → 4 m de diámetro, 15 m de alto) con **techos cónicos azules** (4,8 × 4,8 × 4). Centradas fuera de las esquinas: no entran al interior.
- **Fachada frontal** (la del lado de la puerta, y < 0): `SM_Cast_Neon` y `SM_Cast_Ventanas` del castillo, giradas 180°, a escala 1, con sus materiales originales.
- Sin Event Tick, sin BPs nuevos, sin tocar la colisión del interior.

### Verificado en el editor (sin ver la imagen)
- La puerta del frente queda libre: 9 trazas (x −90/0/90 × z 30/120/220) no chocan.
- Torres con bounds fuera del interior; neón y ventanas pegados a la fachada (y ≈ −40).

### Pendiente
- **Sin verlo en pantalla**: la captura del visor del editor sale ilegible (4 visores mezclados). Hay que mirarlo en una build (la Casona solo se ve desde afuera a través del ático y las ventanas).
- La fachada es a escala 1: el neón (±7,1 m) queda más angosto que el frente de la Casona (20 m). Si se ve desproporcionado, escalar `EXT_Neon` y `EXT_Ventanas` en X ×1,4.

## [0.3.93] — 2026-11-22 · Casona: la escalera al 2.º piso se puede subir (sin build)

### Hallazgo (prueba de `v0.3.92_dev`, captura del CTO)
- "No puedo caminar por el segundo piso." Trazas verticales en el editor: el suelo del piso 2 (z = 340) tiene colisión en todo el plano, pero desde y ≈ 1250 las trazas chocaban a z ≈ 460 sobre la escalera y el pasillo norte.
- Causa: 5 cuadros de `DetectiveOffice` (`StaticMeshActor_280`–`284`, mallas `SM_frame_1` y `SM_frame_14`) estaban a escala 1 con la malla acostada: placas de ~13,7 × 7,6 m flotando a z 428–464, con colisión (`QueryAndPhysics`). Con el suelo a 340 quedaban 90–120 cm de altura libre y la cápsula del jugador (~176 cm) no cabía al llegar arriba de la escalera.

### Cambiado (`L_Casona`)
- Los 5 cuadros: escala 0,1 y `roll` 90° (verticales), pegados a la pared norte (y = 1590, z ≈ 460). Quedan de 137 × 76 cm (`SM_frame_1`) y 51 × 41 cm (`SM_frame_14`).
- Verificado con trazas: la escalera principal (y 920–1430) y la del ático (x 900, y 520–1080) tienen ≥ 180 cm libres sobre cada peldaño; el piso 2 solo queda bloqueado por muebles.

### Pendiente
- Sin probar caminando (no hay build con este cambio).
- Pedido del CTO: que el exterior de la Casona se parezca al castillo. Se hace después de confirmar el piso 2.

## [0.3.92] — 2026-11-22 · Menos avisos de script: `PanelDev` (sin build)

### Cambiado (`BP_KitPrueba` → `PanelDev`, solo con `-dev`)
- Causa: `GetActorOfClass(BP_DirectorMiedo)` devuelve nada en la Casona y el parque (no hay director) y el panel leía `Tension` de ese actor igual (`Accessed None` en cada actualización de 0,5 s).
- Nueva variable `TensionDev` (float) en `BP_KitPrueba`. Después de `GetActorOfClass`, una `Rama` con `IsValid(director)`: con director guarda su `Tension`; sin director guarda 0. El texto del panel usa `TensionDev`. El panel (FPS, posición, batería, ruido) sigue mostrándose en la Casona. Sin Event Tick nuevo.

### Pendiente
- Con esto quedan atendidas las 4 fuentes de avisos del log de `v0.3.84_dev`. Sin probar en juego ni en build: confirmar en el log de una `_dev` que bajan.

## [0.3.91] — 2026-11-22 · Menos avisos de script: `MoverMuneca` (sin build)

### Cambiado (`BP_ThirdPersonCharacter` → `MoverMuneca`)
- Ubicada la función que faltaba: `Munecas` es una variable del BP del jugador y `MoverMuneca` la lee (índice 0, 10 veces). La rama de entrada ya pedía `Length > 0 Y IsValid(Munecas[0])`, pero el `Get(0)` se evalúa aunque la lista esté vacía (en la Casona y el parque no hay muñecas) y daba ~2.000 avisos por sesión.
- Se agregó una `Rama` previa con `Length > 0`: solo entra a la rama de antes si hay muñecas. Con muñecas no cambia nada. Sin Event Tick nuevo.

### Pendiente
- Sigue `PanelDev` de `BP_KitPrueba` (solo con `-dev`). Sin probar en juego ni en build; confirmar en el log de una `_dev`.

## [0.3.90] — 2026-11-22 · Menos avisos de script: `LimpiarMensaje` (B2) y `ActualizarCandado` (sin build)

### Cambiado (`BP_ThirdPersonCharacter`)
- **B2 · `MostrarMensaje`**: con texto vacío el `SetTimer` de `LimpiarMensaje` recibía tiempo 0 (~130 avisos por sesión). Ahora usa 0,05 s en ese caso (`SelectFloat` pin B); borra el mensaje igual y no genera aviso. Con texto no cambia nada.
- **`ActualizarCandado`**: en mapas sin candado (Casona, parque) leía el índice 0 de una lista vacía en cada llamada. Ahora una `Rama` (`Length > 0`) separa los dos casos: sin candado deja `CercaCandado = false` y sigue a `AvisoCandado`; con candado el camino es el de antes. Sin Event Tick nuevo.

### Pendiente
- Siguen sin tocar `Munecas` (función sin ubicar) y `PanelDev` de `BP_KitPrueba` (solo con `-dev`).
- Sin probar en juego ni en build; hay que revisar el log de una `_dev` para confirmar que bajan los avisos.

## [0.3.89] — 2026-11-22 · Casona: sin minimapa del hospital (sin build)

### Cambiado (`BP_ThirdPersonCharacter` → `AplicarHUD`)
- Al final de `AplicarHUD` se agregaron 5 nodos: `GetCurrentLevelName` → `Contains("Casona")` → `Rama` → `SetVisibility(MapaClip, Collapsed)`. En la Casona el panel del minimapa (`MapaClip`, con la imagen del hospital) queda colapsado; en los otros niveles no hace nada. Sin Event Tick nuevo ni coordenadas.
- La tecla de mapa grande (`ToggleMapa`) sigue funcionando sobre el panel, pero en la Casona no se ve nada.

### Pendiente
- Sin probar en juego ni en build. Sin interruptor en `DA_Tuning` (arreglo de HUD por nivel).
- Si más adelante la Casona tiene su propio mapa, hay que cambiar esta regla por una imagen propia.

## [0.3.88] — 2026-11-22 · Casona: texto de objetivo propio (sin build)

### Hallazgo
- En la Casona el HUD mostraba el texto de objetivo del hospital/parque. `MostrarObjetivo` arma `HObjetivo` con una cadena larga de condiciones del hospital y no distingue el nivel.

### Cambiado (`BP_ThirdPersonCharacter` → `MostrarObjetivo`)
- Antes de `SetHObjetivo` se agregaron 3 nodos: `GetCurrentLevelName` → `Contains("Casona")` → `SelectString`. En la Casona `HObjetivo` = "Explora la casona y busca la salida en el ático."; en los otros niveles queda el texto de siempre (sin Event Tick nuevo, sin coordenadas).
- Respaldo del BP previo en `Respaldos/BP_ThirdPersonCharacter_pre0.3.88.uasset` (carpeta ignorada por git).

### Pendiente
- **Sin interruptor en `DA_Tuning`**: es un arreglo de texto, no una feature; si el CTO lo exige, se agrega.
- El minimapa del hospital en el HUD de la Casona sigue visible (se ve en `ToggleMapa` / `ActualizarZona`; no se tocó).
- Sin probar en juego ni en build.

## [0.3.87] — 2026-11-22 · Casona: exposición 0,8 y `-ir=casona` (sin build)

### Hallazgo
- Prueba de `v0.3.86` (22-11): la Casona seguía muy clara con las luces en 15 cd. Causa: el `PostProcessVolume_0` de la Casona fija la exposición en 0,4 (copiada del hospital).
- `-ir=casona` no movía a nadie: `BP_KitPrueba` busca un actor con la etiqueta `Ir_<zona>` y la Casona no tenía `Ir_casona`. El jugador aparecía en el `PlayerStart` por defecto, en el recibidor (0, 150, 100).

### Cambiado (`L_Casona`)
- `PostProcessVolume_0`: `AutoExposureMin/MaxBrightness` de 0,4 a **0,8** (imagen ~2× más oscura, solo en la Casona).
- `PlayerStart_0` lleva la etiqueta **`Ir_casona`**: `-ir=casona` ya funciona y lleva al recibidor.
- Sin tocar luces (15 cd), `SkyLight` ni el hospital. Sin interruptor en `DA_Tuning` (ajuste de nivel).

### Pendiente
- Sin build ni prueba. Si queda oscura, subir la exposición de a 0,2 hacia 0,6; si sigue clara, revisar materiales emisivos.
- El recibidor sigue siendo el único punto de entrada para pruebas: si "la pieza sola" no es el recibidor, hay que revisar el mapa con una captura.

## [0.3.86] — 2026-11-21 · Casona: luces de 650 cd a 15 cd (sin build)

### Hallazgo
- En la captura de `v0.3.85_dev` la Casona se veía casi blanca. Causa: sus 16 `PointLight` eran de **650 cd**, y las del hospital son de 8 cd. La exposición está fija (`autoExposureMin/MaxBrightness` = 0,4 en `PostProcessVolume_0`, desde la v0.3.x para aclarar el hospital), así que el juego no puede oscurecerse ante luces tan fuertes.

### Cambiado (`L_Casona`)
- Las 16 `PointLight` (`_0`, `_1`, `_3`, `_4`, `_7` a `_18`) pasan de 650 a **15 cd**. Un poco más que el hospital porque las salas son más grandes (alcance de 7,5 m). Alcance, color y sombras sin cambios.
- No se tocó la exposición, el `SkyLight` (0,3, captura en tiempo real) ni los volúmenes de post-proceso.

### Pendiente
- Sin build ni prueba: es un ajuste a ojo. Si queda oscura, subir de a 5 cd; si sigue clara, revisar el `SkyLight` (0,3) y los materiales emisivos.
- Sin interruptor en `DA_Tuning`: es un ajuste de nivel, no de jugabilidad.

## [0.3.85] — 2026-11-21 · Arreglo B1: spam de avisos de script en Casona y Parque (sin build)

### Hallazgo
- Logs de `v0.3.84_dev`: ~46.000 avisos de script en 15 min en la Casona (`Accessed None` y `index 0 de un arreglo de largo 0`). Los actores buscados (cintas, luces viejas, fonógrafo, reloj, caja fuerte) no existen en ese mapa. No cierra el juego, pero llena el log y gasta CPU en la iGPU.

### Cambiado (`BP_ThirdPersonCharacter`)
- `ActualizarPosObjetivo` (el origen de ~28.000 avisos, llamada desde `ActualizarBateria`): si no hay actores con tag `Cinta`, usa la posición del propio jugador en vez de leer la lista vacía. Con cintas el resultado es el mismo de antes; sin cintas la flecha de objetivo queda a 0 m y se oculta.
- `ParpadeoParque` (timer de 0,12 s): no hace nada si no hay actores `LuzVieja`.
- `ActualizarCerca`: fonógrafo, reloj y caja fuerte solo miden distancia si existe el actor; si no, "cerca" = falso. Los tres mensajes se conservan.
- Sin Event Tick nuevo y sin coordenadas nuevas. Compila sin errores; queda el aviso de siempre (`GetCurrentLevelName was pruned`), que no es de este cambio.

### Verificado (21-11, `v0.3.85_dev`, Casona, 76 s de juego, 28 FPS en el panel)
- Sin cierres. Las 3 funciones arregladas ya no aparecen en el log; antes eran ~34.000 de ~46.000 avisos. Quedan ~1.500 avisos en 76 s (~20/s, antes ~53/s), de `PanelDev` (solo `-dev`), `ActualizarCandado` y `Munecas`.
- Parque, flecha de objetivo, fonógrafo, reloj y caja fuerte: sin probar en juego.
- Visto en la captura: la Casona se ve muy sobreexpuesta (blanco y verde) y el HUD muestra textos del hospital (minimapa y objetivo de la linterna).

### Pendiente
- Sin build: verificar en el Parque que el log baje de ~46.000 a casi 0 avisos, y que la flecha, el fonógrafo, el reloj, la caja fuerte y el parpadeo de luces del parque funcionen igual.
- Quedan sin tocar: `Munecas` (~2.000 avisos, función aún sin ubicar) y `PanelDev` de `BP_KitPrueba` (solo con `-dev`). `LimpiarMensaje` con `SetTimer` en cero (B2) sigue igual.
- Sin interruptor en `DA_Tuning`: es un arreglo de avisos, sin cambio de jugabilidad.

## [0.3.84] — 2026-11-20 · TSR solo en calidad Alta: arregla los 3 FPS en la iGPU

### Hallazgo
- Perfil de GPU (build Development, `ProfileGPU`): de 363 ms por cuadro, **315 ms eran `TSR RejectShading`**; el resto sumaba ~48 ms. El hilo del juego gastaba solo 23 ms (`stat unit`), así que no era lógica.
- Causa: `Opciones.sav` guardaba **Calidad = 1 (Media)**, y `CalidadExtra` usa TSR (`r.AntiAliasingMethod 4`) en todo lo que no sea Baja. Con la calidad en Baja (TAA) el hospital pasó de 3 a ~30 FPS en el PC del CTO (ASUS VivoBook X515JA, i7-1065G7).
- El `Parque_PerfilBajo` de la v0.3.70 ya lo evitaba en el parque, pero el hospital no.

### Añadido
- **`TSR_SoloAlta`** en `DA_Tuning` (activo). Con calidad Baja o Media, `CalidadExtra` fuerza `r.AntiAliasingMethod 2` (TAA) justo después del bloque que aplica la calidad. TSR queda solo para Alta. Apagado, vuelve al comportamiento anterior.
- `BP_AfinacionDatos.EsTSRSoloAlta`: función auxiliar de lectura (mismo patrón que `EsCalidadAuto`).

### Documentos (20-11)
- Build **v0.3.84** empaquetada (Shipping y Development), 0 errores. Panel del estudio al día (`auditoria/estudio_2026-11-18/panel_estudio.html`).
- **A9:** checklist de humo en `auditoria/humo/v0.3.84.md` (prueba de TSR, H1–H7 y casos nuevos). Sin ejecutar.

### Pendiente
- Verificar en build: hospital y parque con calidad Media guardada (antes 3 FPS) y con Alta (TSR sigue activo).
- Error visto en el playtest del 20-11: al salir del pasillo se mostró "Mundo Mágico" pero el juego siguió en el mismo mapa. Sin reproducir; sospecha: destino `L_Hospital` por `Demo_SinParque` o por un flag guardado.
- Media aún usa `MegaLights 1`, `t.MaxFPS 0` y `DistanceFieldAO 1`; medir si pesan una vez quitado TSR.

## [0.3.83] — 2026-11-20 · A8: interruptores del contador de páginas y de la pantalla FIN (sin build)

### Añadido
- **`UI_FinalDemo`** en `DA_Tuning` (activo). Apagado, `IniciarTV` no muestra la pantalla "FIN DE LA DEMO" ni alarga la transición (queda la TV corta de siempre), aunque `Demo_SinParque` esté activo. La decisión de a qué mapa viaja no cambia.
- **`UI_ContadorPaginas`** en `DA_Tuning` (activo). Apagado, el HUD vuelve al contador original de botellas y páginas "/62".
- `BP_AfinacionDatos.EsFinalDemo` y `EsContadorPaginas`: funciones auxiliares para leer los interruptores desde otros Blueprints.
- `BP_ThirdPersonCharacter.ContadorPaginasActivo` (copia local, por defecto activo), llenada en `CalidadExtra` junto a las demás.

### Cambiado
- `WBP_Transicion.IniciarTV`: las tres selecciones de la pantalla FIN usan `Demo_SinParque Y UI_FinalDemo`.
- `MostrarObjetivo`: el "/15 o /62" usa `Novela_Texto Y UI_ContadorPaginas`. Sin Event Tick nuevo.

### Pendiente
- Sin probar en build: verificar el contador "x/15" y la pantalla FIN con los interruptores activos y apagados.

## [0.3.82] — 2026-11-20 · A6 (parte 1): sombras de linterna por calidad, no por mapa (sin build)

> Amplía lo hecho en v0.3.62: allí la sombra ya dependía de la calidad pero conservaba la condición "no es el parque". Esta versión añade el interruptor para quitarla. A2 ya estaba hecha en v0.3.62.

### Añadido
- **`Calidad_Auto`** en `DA_Tuning` (activo por defecto): con el interruptor activo, la sombra de la linterna depende solo de la calidad (`Calidad >= Linterna_SombraMinCalidad`) y ya no del mapa `L_Parque`. Apagado, se comporta como antes (v0.3.81).
- `BP_AfinacionDatos.EsCalidadAuto`: función auxiliar que expone `Calidad_Auto` para `BP_ThirdPersonCharacter.CalidadExtra`.

### Cambiado
- `CalidadExtra`: la condición `NO estamos en L_Parque` pasa a `(NO L_Parque) O Calidad_Auto`. Sin Event Tick nuevo.

### Pendiente
- **A6 parte 2 (detección por GPU): no viable en Blueprint puro.** Revisado el 20-11: no hay nodo que lea la marca/modelo de la GPU, y `RunHardwareBenchmark` solo entrega el resultado aplicándolo (`ApplyHardwareBenchmarkResults` cambia la escalabilidad del renderer = Perfil A, requiere OK del CTO). Hoy, sin opciones guardadas, `CargarOpciones` no toca `Calidad` y queda en su valor por defecto 0 (Baja), que es lo correcto para la iGPU; el jugador sube la calidad en Opciones. **Decisión del CTO (20-11): no usar Perfil A por ahora; A6 se cierra con Baja por defecto.**
- Sin probar en build: verificar sombras de linterna en Hospital y Parque con calidad Baja/Media.

## [0.3.81] — 2026-11-19 · Recorrido circular y regreso al hospital (sin build)

Ver `docs/RECORRIDO_JUEGO.md` (hospital → parque → castillo → casona → parque → hospital).

### Cambiado
- **Acto 3 / regreso al hospital:** aparece al fondo del Ala de Internación (0,-11800) mirando al norte y el payaso a 10 m detrás (0,-12900). Hay que correr todo el hospital hasta el Pabellón 4. Antes aparecía en (0,-3300), que quedó a mitad de camino tras alargar el hospital.
- Mensaje: "¡CORRE! Vuelve al Pabellón 4, al fondo del pasillo."

### Añadido
- `docs/RECORRIDO_JUEGO.md`: diagrama, tabla de tramos con su disparador y estado, flags y decisiones abiertas.

## [0.3.80] — 2026-11-19 · El castillo lleva a la Casona (sin build)

Idea del CTO: la Casona es el interior imposible del castillo del parque; se entra con un glitch, sin replicar el castillo.

### Añadido
- **`BP_PuertaCastillo`** en `L_Parque`, frente al portón del castillo (0,-12140): al acercarte (< 1,3 m) guarda y lanza el glitch + TV hacia `L_Casona`.
- **Regreso:** `SALIDA_Parque` (ático de la Casona) marca el flag `VuelveDeCasona` y lanza el glitch hacia `L_Parque`; `BP_GestorNivel` te deja frente al castillo (0,-12000) mirando hacia afuera, de espaldas al castillo.
- **`WBP_Transicion.IniciarTV`** acepta un destino ya puesto (`Destino`); si viene vacío elige como antes (hospital/parque). Sin cambios en el flujo de la demo.
- **`BP_GestorNivel`** tiene rama para `L_Casona` (VHS + `Comenzar`, así se carga el inventario). `BP_GestorNivel` y `BP_KitPrueba` puestos en `L_Casona`.
- `-ir=casona` y `-ir=castillo`; `Probar.bat` → **P → 6** (castillo) y **P → 7** (casona).

### Pendiente
- Verificar que el portón no esté bloqueado por otra malla y que el trigger se active; el castillo seguirá vacío por dentro.
- La Casona no pasa por la persecución ni el Acto 3; solo el regreso al parque.

## [0.3.79] — 2026-11-19 · La Casona construida y `L_Pabellon4` retirado (sin build)

Quedan 3 niveles: `L_Hospital` (con el pabellón), `L_Casona` y `L_Parque`. Validación en `auditoria/mapas_2026-11-19.md`; diseño en `docs/DISENO_CASONA.md`.

### Añadido: `L_Casona` (blockout vestido, ~700 actores)
- **Estructura** (cubos con materiales de `DetectiveOffice` y `SICKA`): planta baja (vestíbulo, comedor, biblioteca, cocina, estudio), **escalera central** de 19 peldaños, planta alta (pasillo alfombrado, dormitorio principal, baño, dormitorio B) y **ático** con escalera propia. Friso de madera, papel mural, piso de madera, techo de yeso.
- **Muebles de los packs:** `UnrealBucket` (araña, consola, espejo, reloj de pie, piano, aparador, libreros, sofá y sillón victorianos, camas góticas, roperos, veladores), `DetectiveOffice` (marcos, escritorios, tablero de evidencias, alfombras), `AnimatedWoodenCrate` (ático), `Horror_Hospital` (lavabos y decals).
- **Luz:** 16 puntuales ámbar sin sombras. 16 decals de desgaste.
- **Juego:** `PlayerStart` en la entrada, linterna sobre la consola (para pruebas), 2 escondites (`BP_HideSpot`) en los roperos, y **`SALIDA_Parque`** en el ático, que abre `L_Parque`.
- **Marcadores** con etiqueta: `Entrada_Hospital`, `Salida_Parque`, `Album_1..4` (con 4 marcos), `Escondite_1/2`, `Evento_Apagon`, `Sombra_Anciana_Inicio`.
- `BP_PuertaNivel` ya no depende de `DA_Tuning`; abre `L_Parque`. Lo dejó de usar `L_Hospital`.

### Quitado
- `L_Pabellon4.umap` salió del proyecto (copia en `Documents/Unreal Projects/Respaldos_Config`; también en git por el tag `v0.3.41-monolito`).

### Pendiente
- Puzle de álbumes, apagón, Sombra anciana, páginas 16–40, conexión Hospital → Casona (glitch + TV) y Casona → Parque con VHS, horneado de luz, ajuste fino de posiciones y rotaciones de los muebles (se colocaron sin verlos).
- No es alcanzable desde el menú: solo con `-ir=casona` (falta agregar la zona a `BP_KitPrueba`) o abriendo el mapa directo.

## [0.3.78] — 2026-11-19 · Ala de Internación con la atmósfera de referencia (sin build)

Referencia del CTO: pasillo largo y oscuro, azulejo blanco hasta media pared, muros verdosos descascarados, tubos fluorescentes, escombros, silla de ruedas y luz cálida al fondo.

### Añadido
- **Materiales del hospital** en el ala nueva: `MI_Muro_VerdeAzulado` (muros), `MI_Piso_Azulejo` (piso), `MI_Techo`.
- **Friso de azulejos** (`MI_Azulejo`, 0–150 cm) en las dos paredes del pasillo, con huecos en las puertas, y **marcos de puerta azules** (`MI_Marco_Azul`) en los 12 cuartos.
- **6 tubos fluorescentes** en el techo (uno de cada tres apagado) y una **ventana cálida con luz ámbar** al fondo de la sala de espera.
- **Utilería del pack Horror_Hospital:** 2 sillas de ruedas, 2 camillas, cubos de residuos, cajas, atril de suero, banca, barrera médica, silla.
- **8 montones de escombro** con papeles arrugados y **16 decals** (suciedad de piso, grietas y daño en muros).
- Todo en la carpeta `Ala_Internacion` del outliner.

### Pendiente
- Ver en juego el equilibrio de luz y presupuesto de FPS; el resto del hospital no se tocó.

## [0.3.77] — 2026-11-19 · Hospital más largo: Ala de Internación (sin build)

Decisión del CTO: el Pabellón 4 es el hospital. Quedan 3 niveles: `L_Hospital`, `L_Parque` y `L_Casona`.

### Añadido
- **Ala de Internación** en `L_Hospital` (bloqueo con cubos), después de la plataforma del glitch: pasillo de 66 m, 12 cuartos (6 por lado) y una sala de espera final de 16×16 m, entre Y -4900 y -13100. 6 luces suaves sin sombras. Carpeta `Ala_Internacion`.
- `DA_Tuning.Salida_Y` (-12600): `BP_GestorNivel.VigilarSalida` ya no usa -4000 escrito a mano. Al pasar esa Y (con `SalidaHecha`) arranca la transición TV al parque, igual que antes.

### Quitado
- `L_HospitalGrande` (mapa y puerta `PUERTA_HospitalGrande`). Se retira del cocinado. Se mantiene `L_Casona`.

### Por probar
- Que tras el glitch se pueda caminar por el ala nueva y que la salida al parque se dispare al final. El texto de objetivo y el navmesh (volumen existente hasta Y -19500) no se han verificado.

## [0.3.76] — 2026-11-19 · Mapas nuevos: hospital grande y casona (bloqueo, sin build)

Ordenar los 3 mapas del plan. Auditoría en `auditoria/mapas_2026-11-19.md`.

### Añadido
- **`L_HospitalGrande`** (bloqueo con cubos, sin arte): pasillo principal de 72 m con 12 cuartos, recepción de 24×24 m y un ala norte de 42 m con 6 cuartos más. 10 luces suaves sin sombras. Entrada en (250,0) con marcador `Entrada_Pabellon`.
- **`L_Casona`** (bloqueo mínimo): recibidor, pasillo de 28 m y 6 cuartos. Marcadores `Entrada_Hospital` y `Salida_Parque`.
- **`BP_PuertaNivel`** (`/Game/MundoMagico/Mundo`): vigila con un temporizador de 0,3 s (sin Tick) y abre `L_HospitalGrande` al acercarse a menos de 1,2 m. Solo funciona si `HospitalGrande_Activo` es true.
- **`PUERTA_HospitalGrande`** puesta en `L_Hospital` en (-330,-2700), junto al cartel ARCHIVO. **Posición provisional**: el CTO decide la puerta real.
- `DA_Tuning`: `HospitalGrande_Activo` (false). La demo no cambia.
- `Construir_Version.ps1` cocina también los mapas nuevos.

### Corregido
- `Probar.bat` abre los mapas con la ruta completa; en Shipping el nombre corto caía a `L_Hospital` y el salto al parque no funcionaba.

### Pendiente
- Falta: arte, navmesh real, `BP_GestorNivel`, volver al pabellón y conectar la casona con el hospital y el parque.

## [0.3.75] — 2026-11-18 · Acto 3: la persecución final (sin build)

Tras elegir "sala" en la cabina, el juego vuelve al hospital y el payaso te persigue hasta el expediente.

### Añadido
- **`BP_PayasoPersecucion`** (copia de la sombra): persigue al jugador a `Payaso_VelFinal` (520 cm/s ≈ 5,2 m/s); la linterna no lo detiene. Al tocarte hace 1 de daño cada 1,5 s. Se destruye cuando termina el juego. Sin Event Tick (temporizador de 0,2 s).
- **`EleccionSala`**: marca `PersecucionFinal` en los flags, guarda, y lanza la transición TV hacia `L_Hospital`.
- **`WBP_Transicion.IniciarTV`**: el destino es `L_Hospital` si `Demo_SinParque` o existe `PersecucionFinal`.
- **`BP_GestorNivel`**: en `L_Hospital` con `PersecucionFinal`: carga la partida, teletransporta a (0,-3300,120), aparece el payaso en (0,-4300,100) y el mensaje "¡CORRE!". El expediente y los 3 finales ya existían.
- `DA_Tuning`: `Acto3_Persecucion` (true), `Payaso_VelFinal` (520).

### Pendiente de probar
- Que la rampa de la sala 4 y el navmesh dejen llegar al payaso; que el flag no afecte una partida nueva (revisar `LimpiarFlags`).

## [0.3.74] — 2026-11-18 · A4: pistas según tu progreso (sin build)

Plan de la reunión "Replicar el HTML": el HTML tiene una pista bajo demanda y una automática si te atascas.

### Añadido
- **`MostrarPista`** (jugador): un mensaje de 7 s con la siguiente cosa que hacer, según lo que ya tienes. Orden:
  1. sin linterna → "la linterna está sobre el mostrador de recepción";
  2. sin energía → "sal por Urgencias y enciende los 4 interruptores";
  3. menos de 3 cintas → "busca las cintas por el parque";
  4. sin llave → "la llave está en los disfraces de la bodega";
  5. casa cerrada → "abre la casa de Alfredo con la llave";
  6. menos de 4 cintas → "la última cinta está dentro de la casa";
  7. sin fusible → "el fusible está en el compartimento del reloj; escucha el fonógrafo";
  8. cintas sin ver → "ve a la cabina VIDEO";
  9. al final → "vuelve a la sala psiquiátrica del Pabellón 4 y lee el expediente".
- **Tecla Y** (teclado) para pedir la pista cuando quieras.
- **Pista automática**: `RevisarPista` corre cada 30 s en un timer (sin Tick). Si tu progreso (cintas, páginas y los 6 indicadores) no cambia durante `Pista_AutoSeg` (180 s), muestra la pista sola.
- **`Pistas_Activas`** (activo) y **`Pista_AutoSeg`** (180) en `DA_Tuning`. Los valores y el timer se cargan desde `CalidadExtra`.

### Nota técnica
- `CallFunction|MostrarMensaje` en DSL exige parámetros por nombre (`:Texto`, `:Duracion`); con posicionales el texto se corría a `self`. Anotado para futuros cambios.
- Con mando no hay botón libre, así que se usa la pista automática.

## [0.3.73] — 2026-11-18 · El parque siempre arranca + se recoge lo que miras

Playtest del CTO (v0.3.72): *"llego al parque y el juego llega a la pantalla de inicio"*; y pendiente de antes: *"si hay varios objetos juntos a la linterna no pude interactuar bien"*.

### Corregido
- **El parque ya no puede devolverte al hospital ni dejarte en el menú.** `BP_GestorNivel` dependía de una marca "Transicion" en disco: si faltaba, mandaba a `L_Hospital` o dejaba la pantalla de inicio sin llamar a `Comenzar`. Ahora, en `L_Parque`, **siempre** se hace la transición VHS, se llama a `Comenzar` y se coloca al jugador en la entrada. La marca solo se borra si existe. El rebote a `L_Hospital` se eliminó (nunca debía ocurrir en el juego real). Con v0.3.72 la marca ya se guardaba con `BP_Partida`; esto cubre el caso de que algo la pierda.

### Añadido
- **Recoger el objeto que miras** (`Interaccion_MiradaCm` en `DA_Tuning`, 150; 0 = casi como antes):
  - `RecogerObjeto` busca el objeto más cercano a **un punto 1,5 m delante de la cámara** (cámara + dirección de mira × `MiradaCm`) y no al más cercano a tus pies. Con varios objetos juntos se toma el que estás mirando.
  - El radio de recogida pasa de 250 a **200 cm** medido desde ese punto.
  - Variable nueva `MiradaCm` en el jugador, cargada desde `CalidadExtra`.

### Para probar
- Ruta normal hasta el parque: debe cargar con la transición VHS y quedar en la entrada, sin menú.
- Con varios objetos juntos (por ejemplo, el mostrador de la recepción), mira uno y presiona E o □: debe tomar ese.

## [0.3.72] — 2026-11-18 · Al cruzar el glitch ya no vuelve al menú: la marca de transición se guarda

Playtest del CTO (v0.3.71, build normal): *"va fluido en el hospital, pero al salir al parque llego a la pantalla de iniciar juego"*.

### Corregido
- **Causa:** `BP_GestorNivel.VigilarSalida` guardaba la marca "Transicion" con `CreateSaveGameObject("/Script/Engine.SaveGame")`, que es una **clase abstracta**: Unreal no puede crear la instancia, `SaveGameToSlot` guardaba nada y el parque no encontraba la marca. Sin ella, el parque cree que se abrió directo y manda de vuelta al hospital (`OpenLevel L_Hospital`), que abre en el menú.
- **Arreglo:** la clase es ahora `BP_Partida` (concreta). Al cruzar el glitch se guarda la marca, el parque la encuentra, la borra, hace la transición VHS y te deja en la entrada.
- Revisé el resto del proyecto: es el único uso de la clase abstracta.

### Aprendido (mediciones)
- **Las builds Development distorsionan el rendimiento en esta iGPU** (3 FPS en el hospital contra fluido en la build normal). Las mediciones de la v0.3.69–0.3.71_dev no valen para FPS. Se usan solo para buscar cierres.

## [0.3.71] — 2026-11-18 · Sin teclados de código en el parque (cofres directos)

Playtest del CTO: *"al entrar al parque aparece como un cofre de código; solo hay un código y es en el teatro, el resto se puede desactivar"*.

### Cambiado
- **Los teclados de código del parque se desactivan.** Eran 2: el **candado del cofre de la bodega** (`BOD_Candado`, código 317) y la **caja fuerte de la casa** (`CASA_CajaFuerte`, código 842). Ambos dan +2 pilas y +1 botiquín y no bloquean la historia.
- **`AbrirCandado` ahora decide según `Codigos_Teclado`** (`DA_Tuning`, apagado): con el interruptor apagado no sale el panel "CANDADO [0] 0 0"; **E / □ abre el cofre o la caja directo** y entrega su recompensa, como si hubieras puesto el código bueno. Con el interruptor encendido vuelve el teclado.
- **Se deja el reloj de la casa** (`CASA_Reloj`): no es un código numérico y su compartimento es el camino al fusible. El puzle del teatro no cambia.

### Medición de rendimiento (v0.3.70_dev, `herramientas/barrido_zonas.ps1`)
- El FPS depende de la zona: la entrada del parque iba a unos 30 FPS y los autos a unos 3 antes del perfil liviano; con `Parque_PerfilBajo`, teatro ~9 y autos ~13 FPS (muestras poco precisas). **La noria se cerró sola** sin reporte de cierre. Pendiente: aislar qué translucidez pesa en esas zonas (el perfil GPU mostró el pase *Translucency (AfterDOF)*).

## [0.3.70] — 2026-11-18 · Perfil liviano forzado en el parque (diagnóstico de rendimiento)

### Cambiado
- **El parque corría a 3 FPS** (medición D11 del CTO: Frame 347–373 ms, CPU 13 ms, **GPU 348–373 ms**, 120–132 draws, 474K prims). La GPU se va en pases de pantalla completa. El log mostró que `CalidadExtra` aplicaba la rama pesada (TSR, `DistanceFieldAO`, `t.MaxFPS 0`, resolución 80 %) porque la calidad guardada no es Baja.
- **`CalidadExtra` fuerza el perfil liviano en `L_Parque`**, sin importar la calidad elegida: `r.AntiAliasingMethod 2` (TAA), `r.MegaLights.Allow 0`, `t.MaxFPS 30` y `r.DistanceFieldAO 0`. Interruptor nuevo **`Parque_PerfilBajo`** en `DA_Tuning` (activo). El hospital no cambia.
- `CalidadExtra` se reescribió completa; mantiene las sombras de la linterna por calidad, los avisos por mando, las páginas con texto y el gasto de batería.

### Para verificar
- En la build Development, `ProfileGPU` en el log muestra el pase más caro. Esperado: 25–30 FPS en el parque. Si sigue lento, el perfil dirá qué pase es.

## [0.3.69] — 2026-11-18 · Arreglo del cierre al entrar al parque con partida guardada

### Corregido
- **El parque cerraba el juego y "partías en la sala del hospital"** (causa H2 de la reunión de avance). Evidencia: con `Partida.sav` borrado, `L_Parque -ir=parque` corrió más de 5 min sin cerrarse en la build Development; todos los cierres tenían partida guardada.
  - `Cargar` hacía `SetActorLocation(PosControl)` sin comprobar el mapa. La posición guardada era del hospital y `L_Parque` conserva una copia de esa sala: el jugador aparecía dentro y se caía.
  - **`BP_Partida` guarda ahora `Mapa`** (el nombre del mapa donde se guardó) y `Guardar` lo escribe.
  - **`Cargar` restaura la posición solo si `Mapa` es igual al mapa actual.** Si la partida viene de otro mapa, o es antigua y no tiene `Mapa`, se carga todo lo demás (inventario, flags, cintas, batería) y el jugador queda en el punto de inicio del mapa.
  - Con esto, cruzar el glitch del hospital deja al jugador en la **entrada del parque**, no en la sala del hospital.
- Las partidas antiguas siguen cargando; en el primer uso no restauran la posición.

### Pendiente
- Si el parque se cae igual, la causa es otra. Reproducir con la build Development `v0.3.69_dev` y mirar `Saved\Logs`. Mover o borrar la copia de la sala del hospital que sigue dentro de `L_Parque` (`NavMesh_Hospital`, `BP_IsidoraAparicion`).

## [0.3.68] — 2026-11-18 · Parque sin sombras dinámicas (NO resolvió el cierre)

> **Corrección (18-11, reunión de avance):** los reportes de cierre muestran que **D9 y D10 también se cayeron** (11:34 y 11:35), y que la v0.3.68 se cae igual (`-ir=parque` 11:54, `-ir=teatro` 11:57). Las sombras **no** eran la causa, o no la única. Se mantiene el cambio, que es más liviano, y la causa raíz sigue abierta: ver build Development `v0.3.68_dev` con logs y `auditoria/html_completo/estado_parque_2026-11-18.md`.

### Corregido
- **Causa del GPUCrash al entrar al parque (S1)**, encontrada con el diagnóstico P0 del CTO en `Probar.bat` (opciones D1–D10):
  - D2–D8 se cayeron todas (configuración liviana, resolución al 50 %, sin RayTracing, menos memoria de texturas, sin nubes ni partículas). **No era memoria ni resolución:** quedaban unos 800 MB libres.
  - **D10, solo "sin sombras dinámicas", no se cae**, y D9 tampoco.
  - Culpable: en `L_Parque`, **14 luces puntuales proyectaban sombras** (6 pasadas por cuadro cada una sobre unos 1.400 actores) más **el sol, con 4 cascadas dinámicas**. La iGPU Iris Plus no termina el cuadro a tiempo y Windows la resetea (TDR). En el hospital las luces nunca tuvieron sombra; por eso aguantaba.
- **Arreglo en el mapa, sin tocar el proyecto:** `CastShadows = false` en las 14 luces, el sol y el `SkyLight` de `L_Parque`. El parque queda con la misma regla que el hospital: **solo la linterna puede proyectar sombras**, y en el parque está apagada (`Linterna_SombrasParque`).

### Diagnóstico agregado a `Probar.bat` (submenú P)
- D1–D10 para aislar cierres por tipo de carga: memoria, resolución, RayTracing, texturas, nubes, partículas y sombras.
- D5 ahora es la ruta normal **sin `-ir`**, porque el salto de zona devolvía al hospital al llegar al parque.

## [0.3.67] — 2026-11-18 · Vuelve el parque; sin pantalla de fin (sin build)

### Cambiado
- **El CTO pide sacar la pantalla "FIN DE LA DEMO" y volver a conectar el parque.** La demo del HTML no termina en el hospital: sigue en el parque (interruptores, cintas VHS, casa de Alfredo, cabina VIDEO y finales). Por eso se revierte la decisión "demo sin parque" del 18-11.
  - `Demo_SinParque` = **falso** en `DA_Tuning`, cambiado con el script nuevo `herramientas/ajustar_tuning.py`, que no necesita abrir el editor.
  - Tras el glitch y la TV se carga **`L_Parque`** como antes. La pantalla de fin no aparece, porque solo se muestra con `Demo_SinParque` activo.
- **Pendiente crítico:** el parque **cierra el juego en la iGPU** (GPUCrash, falta de recursos). La solución sale de la reunión "Replicar el HTML" (`auditoria/html_completo/`).

## [0.3.66] — 2026-11-18 · Tubos fluorescentes en falla + pantalla de FIN DE LA DEMO

Playtest del CTO (v0.3.65): la botella y recoger objetos funcionan bien.

### Añadido
- **`BP_TuboFalla`** (`/Game/MundoMagico/Mundo/`): un tubo fluorescente que **parpadea al azar** en vez de quedar fijo.
  - La luz `Tubo` (PointLight) es blanco frío verdoso, de 8 cd, con forma de tubo de 1,2 m, 6,5 m de alcance y **sin sombras** (barato para la iGPU).
  - La función `Parpadeo` se llama con timers que se reprograman al azar (sin Tick):
    - El 75 % de las veces es un **corte corto** (0,04–0,15 s al 8 %).
    - El 25 % es un **apagón** de 1–3,5 s, con chasquido eléctrico (`LightFlicker02`).
    - Encendido dura 0,3–4 s.
  - **7 tubos en `L_Hospital`:** 2 en Reanimación, 1 en Farmacia, 2 en la sala del despertar y 2 en el pasillo de Urgencias. Las habitaciones se ubicaron con rayos (techo a 3,2 m).
  - Interruptor nuevo **`Tubos_Falla`** en `DA_Tuning` (activo). Si lo apagas, quedan encendidos fijos.
- **Pantalla "FIN DE LA DEMO"** (playtest: *"al salir del pasillo se cae el juego y vuelve al inicio"*). No era un cierre: con `Demo_SinParque` se volvía al menú sin aviso.
  - Ahora, tras la TV, se ve **fondo negro y el texto** "FIN DE LA DEMO · Capítulo 1 · Hospital · La historia de Isidora continúa en la Casona… · Gracias por jugar · Ciclo Studio Game" durante **6,5 s**. Después vuelve al menú.
  - Componentes en `WBP_Transicion`: `FinFondo`, `FinTexto` y `TFin`.

### Hallazgo
- Las luces originales de las habitaciones tienen **LightColor negro**, así que no alumbran y por eso solo se veía la luna. Se dejaron así: ahora los tubos son la luz de cada sala.

### Pendiente (del playtest)
- **Recoger con varios objetos juntos:** hoy se toma siempre el **más cercano**, no el que miras. Cambiar a "el más centrado en la vista" (va con A4, pistas e interacción).

## [0.3.65] — 2026-11-18 · Sprint A5 + A7: la botella distrae y la Sombra es más justa (sin build)

### Corregido
- **A5 · La botella atraía a la Sombra hacia ti.** `Lanzar` registraba el ruido en **tu posición**, porque la distancia se multiplicaba por 0. Ahora el ruido y el sonido de vidrio quedan **12 m adelante**, donde cae la botella, y la Sombra va a investigar ahí. Así puedes usarla para alejarla y pasar.

### Añadido
- **A7 · Sombra más justa** (@gameplay-engineer y ai-programmer):
  - **Pausa después del golpe:** tras golpearte, la Sombra **vuelve a su zona** y no te caza durante **`Sombra_PausaGolpe` = 3 s**. Se acabó encadenar golpes sin poder escapar.
  - **Anti-atasco:** si persigue o investiga y queda trabada (velocidad < 20 durante 2,5 s), vuelve a patrullar con un punto nuevo. Interruptor **`Sombra_AntiAtasco`** (activo).
  - Las dos van dentro de `RevisarLinterna`, que ya corre cada 0,2 s; no se agregó Tick.

### Para probar en la próxima build
- Lanzar una botella (T / R2) lejos de la Sombra y ver que va hacia donde sonó el vidrio.
- Dejarse golpear una vez: la Sombra debería retirarse unos segundos.

## [0.3.64] — 2026-11-18 · Sprint A3: la linterna te delata y ahuyenta a la Sombra (sin build)

Pilar 1 de la reingeniería, "la luz te salva y te delata" (@gameplay-engineer, ai-programmer y systems-designer). Se agregó como un sentido aparte de la Sombra para no reescribir su IA.

### Añadido
- **`BP_AI_Sombra.RevisarLinterna`**: corre en un timer de 0,2 s que arranca en BeginPlay. No usa Tick.
  - **Te delata:** con la linterna **encendida** la Sombra te ve a `Sombra_VistaDist` (22 m). **Apagada**, solo a `Sombra_VistaApagada` (**7 m**). Ver de cerca (2,5 m) sigue igual.
  - **La ahuyenta:** si le apuntas la linterna (cono de unos 16°, a menos de 18 m) durante **`Sombra_RepeleSeg` = 2,5 s**, la Sombra chilla, **huye 15 m** a velocidad de persecución y deja de cazarte durante **`Sombra_HuidaSeg` = 5 s**. El tiempo apuntado baja si dejas de apuntarle.
  - `Pensar` respeta la huida: se salta su turno mientras `HuyeHasta` sea mayor que el tiempo de juego.
- **Batería en `DA_Tuning`:** `Bateria_GastoSeg` = **0,35 %/s** (antes 0,22, fijo en el BP). La lee `CalidadExtra` y la usa `ActualizarBateria`. La linterna ahora es un recurso que se gasta y hay que administrar.
- Interruptores y valores nuevos en `DA_Tuning`: **`Sombra_RepeleLinterna`** (activo), `Sombra_RepeleSeg`, `Sombra_VistaApagada`, `Sombra_HuidaSeg` y `Bateria_GastoSeg`.

### Para probar en la próxima build
- Con la linterna apagada, que puedas acercarte más a la Sombra sin que te vea.
- Que al apuntarle unos 2–3 s se escuche el chillido, retroceda y no te persiga durante unos 5 s.
- Que la batería dure lo razonable (100 % ≈ 4,8 min encendida).

## [0.3.63] — 2026-11-18 · Demo sin parque + las 15 páginas de la Novela (sin build)

### Corregido
- **El juego se cerraba al cargar el parque**: el iGPU se queda sin recursos (playtest de la v0.3.62). Ya estaba decidido que el parque no entra en la demo pública.
  - Interruptor nuevo **`Demo_SinParque`** en `DA_Tuning` (activo).
  - Con el interruptor activo, `WBP_Transicion.IniciarTV` fija `Destino = L_Hospital` y `Paso` abre ese nivel: después del glitch y la TV, la demo **vuelve al menú** en vez de cargar `L_Parque`.
  - Para pruebas internas del parque, desmárcalo o usa el submenú P de `Probar.bat`.

### Añadido
- **Las 15 páginas de la Novela con texto** (B1, adelantado; textos de @world-builder, writer y narrative-director). Canon aprobado: leucemia en recaída + miocardiopatía, Isidora muere a las 03:33 del 13/03/1998.
  - **`BP_NovelaDatos`** (PrimaryDataAsset con `Titulos[]` y `Textos[]`) + **`DA_NovelaData`** con las 15 entradas: ficha de ingreso, hoja de enfermería, orden de la dirección, registro de Urgencias, inventario, ECG, portería 23:41, mantención, solicitud de radiografía, archivo, lavandería, hoja de Alfredo, radiografía 03:33, libro de la morgue y certificado sin firmar.
  - **`WBP_Pagina`**: una hoja de papel nueva (`PapelMarco`) con letra Special Elite, título, texto y "PÁGINA n / 15". La función nueva `Llenar(Indice)` lee `DA_NovelaData`.
  - **`MostrarPagina`**: con el modo Novela activo y página ≤ 15, muestra el texto en vez de la imagen. La página queda **7 s** en pantalla (antes 4,5).
  - Interruptor nuevo **`Novela_Texto`** en `DA_Tuning` (activo). Apagado, vuelven las imágenes `T_Pag_*`.
- **Contador del HUD**: "PÁGINAS x/**15**" en la demo (antes /62). Vuelve a /62 si se apaga `Novela_Texto`.

### Pendiente
- Las páginas aparecen en el **orden en que se recogen** (la 1.ª recogida muestra la página 1). Cuando se construya el sótano (Sprint B), cada `REC_Pagina` tendrá su número fijo.
- Si usas **Continuar** con una partida guardada en la salida, la demo puede terminar de inmediato. Se resuelve con la pantalla final (A8).

## [0.3.62] — 2026-11-18 · Sprint A: A6 (sombras según la calidad) + A2 (avisos según el mando) + herramientas

### Cambiado
- **A6 · Las sombras de la linterna dependen de la calidad y no solo del mapa** (reingeniería de @tech-lead).
  - `CalidadExtra` aplica: sombras = (no es el parque **y** calidad ≥ `Linterna_SombraMinCalidad`) **o** `Linterna_SombrasParque`.
- **Nuevo en `DA_Tuning`: `Linterna_SombraMinCalidad`** = 1 (Media). En calidad Baja la linterna ya no proyecta sombras y se gana rendimiento en la iGPU. Pon **0** para tener sombras también en Baja, como en la v0.3.61.

### Añadido
- **A2 · Avisos según el dispositivo** (@ux-toolsmith):
  - Al tocar un botón del mando, los avisos del HUD cambian a **[□] Recoger / [□] Usar**. Al tocar una tecla vuelven a **[E]**.
  - Usa el evento `AnyKey`, sin consumir la entrada, más `IsGamepadKey` → `UsandoMando`.
  - Interruptor nuevo **`UI_AvisosPorDispositivo`** en `DA_Tuning` (activo). Si lo apagas, siempre muestra [E].
- **`Probar.bat` nuevo** (@ux-toolsmith y @qa-publisher):
  - El hospital va primero.
  - Juego **sin sonido**.
  - **Borrar partida**, que la mueve a `SaveGames\respaldo_<fecha>` sin eliminarla.
  - **Abrir logs y cierres**.
  - **Recoger logs en un ZIP** al Escritorio, con `herramientas/Recoger_Logs.ps1`.
  - El parque pasa a un submenú interno, porque queda fuera de la demo pública.

## [0.3.61] — 2026-11-18 · Sprint A de Ciclo Studio Game: A1 (look del HTML en el hospital) + A10

### Añadido
- **A1 · Look del juego HTML en `L_Hospital`** (propuesta de @world-builder, technical-artist y art-director):
  - `PPV_PostHTML`: PostProcessVolume sin límites, con prioridad 1 y etiqueta `PostHTML`.
  - LUT **`T_LUT_HospitalHTML`**, generada con `herramientas/componer_lut_hospital.py`. Replica el post del HTML (desaturación 22 %, curva S 35 %) con tinte verdoso de hospital y sombras verde-azuladas.
  - Viñeta 0,65, grano 0,3 y aberración cromática 1,2.
  - **Niebla del hospital** con el color del HTML (#070b14) y **sin niebla volumétrica**, más liviana para la iGPU.
- **Interruptor nuevo `PostHTML_Activo` en `DA_Tuning`** (activo). Si lo apagas, `BP_PostHTML` (`DEV_PostHTML` en el mapa) desactiva `PPV_PostHTML` al empezar y vuelve el look anterior.

### Corregido
- **A10 · El menú ya no usa el Tick para la luz de la portada** (hallazgo de @tech-lead). `MoverLuz` se llama con un timer de 0,033 s creado en el Construct de `WBP_Menu`.

## [0.3.60] — 2026-11-18 · Linterna como la del juego HTML, sin linterna flotante, sonidos de puertas y páginas

### Cambiado
- **Haz de la linterna con los parámetros del juego HTML** (`SpotLight(0xffeedd, 55 m, 0.4 rad, penumbra 0.5)`):
  - Color cálido (255, 238, 221).
  - Cono exterior de 23° (antes 30°) e interior de 11,5°, lo que da el mismo borde suave.
  - Alcance de 55 m (antes 30 m).
- **Perfil IES nuevo, `IES_Linterna`** (`herramientas/componer_linterna_y_pagina.py`). Imita la máscara de la linterna HTML: centro caliente, un anillo del reflector a unos 9°, caída suave hasta 23° y un halo tenue hasta 30°. Así el círculo deja de ser plano y "falso".
- **L2 (enfocar):** el haz se cierra a 14° (antes 18°). Al soltar vuelve a 23° (antes saltaba a 34° y pisaba el ajuste).
- **Hojas de la Novela con sonido propio:** `SFX_Pagina` (roce de papel y una nota grave) reemplaza a `SW_Note_Pickup`.

### Corregido
- **Linterna flotante en 1ª persona.** El modelo `SM_Flashlight_V2` colgado de la cámara se veía suelto, sin mano. `ColocarLinterna` ahora marca `OwnerNoSee = true` en 1ª persona (antes el valor iba en falso) y lo devuelve a falso en 3ª.
- **Sonido de puerta al apretar Cuadrado o E.** Interactuar cerca de una puerta cerrada hacía sonar el forcejeo (`SW_TryOpen_Door`), y también el sonido propio de `BP_Locked_Door`. Se quitaron los dos. Las puertas solo suenan cuando se abren o se cierran (también la de llave). El mensaje "Está cerrada con llave" se mantiene.

## [0.3.59] — 2026-11-17 · Portada: luz sin borde y más clara

### Corregido
- **Al mover la luz de la portada se veía su borde rectangular** (playtest de la v0.3.58). `LuzMouse` medía lo mismo que la pantalla y al desplazarse dejaba franjas sin oscurecer.
  - Ahora ocupa toda la pantalla (Fill) con `RenderScale` 2,2 × 3,91: el círculo cubre la pantalla entera aunque el mouse esté en una esquina.

### Cambiado
- **Más luz en la portada:**
  - Centro cálido al 28 % (antes 20 %) y más ancho.
  - Bordes oscuros al 45 % (antes 60 %), con transición suave.

## [0.3.58] — 2026-11-17 · Arreglo del cierre al llegar al parque + portada web (sin build)

### Corregido
- **El juego se cerraba solo al pasar al parque** (playtest de la v0.3.57).
  - El reporte dice `GPUCrash`: la iGPU se colgaba.
  - Causa: desde la v0.3.55 la linterna proyecta sombras siempre, y en `L_Parque` (unos 1.400 actores) eso supera lo que aguanta la Iris Plus.
  - Arreglo: `CalidadExtra` apaga las sombras de la linterna en `L_Parque`. En el hospital siguen activas.
- **Nuevo interruptor `Linterna_SombrasParque` en `DA_Tuning`** (apagado). Si lo marcas, vuelven las sombras en el parque. Úsalo solo en un PC con GPU dedicada.

### Cambiado
- **Portada del menú igual a la versión web:**
  - Imagen en 1ª persona de la entrada del parque con la linterna en la mano (`T_Portada_FP`, 1920×1080).
  - **Luz de linterna que sigue al mouse**: `LuzMouse` con `T_LuzPortada`, centro cálido y bordes oscuros, con suavizado. La mueve la función `MoverLuz`, que se llama desde el Tick del menú.
  - Los botones bajan a 740, 806 y 872 (marco de 1920×1080).
  - Se mantienen el zoom lento, la niebla, la lluvia, el grano y el parpadeo.
- Scripts: `herramientas/componer_luz_portada.py` e `importar_portada_fp.py`.

## [0.3.57] — 2026-11-17 · Escondites que no atascan (etapa E2, sin build)

### Añadido
- **`/Game/MundoMagico/Mundo/BP_HideSpot`**: escondite nuevo, duplicado de `BP_Hiding_Door` del Horror Template. Se entra y se sale con **E**.
  - Al entrar:
    1. Guarda dónde estabas (`PosFuera`).
    2. Apaga la colisión del **jugador** (no la del mueble, para que el rayo de E lo siga tocando).
    3. Te lleva al marcador de adentro y congela el movimiento.
    4. Te pone la etiqueta `Escondido`.
  - Al salir te devuelve al mismo punto donde estabas, que ya se sabe libre. Así se corrige el atasco de los clósets viejos.
  - Con la etiqueta `Escondido`, **la Sombra no te ve**, y sigue funcionando contener la respiración (Shift).
- **`BP_ThirdPersonCharacter.PonerEscondido(Si)`**: agrega o quita la etiqueta `Escondido`.

- **2 escondites colocados en `L_Hospital`, en el pasillo de la Sombra** (paredes en x = ±195):
  - `ESC_HideSpot_1`: (110, −3000), pared derecha.
  - `ESC_HideSpot_2`: (−110, −3350), pared izquierda.
  - Los dos miran al centro y el frente queda a 90 cm del eje del pasillo. Se verificó con rayos.
- Al entrar, el jugador y la **cámara** miran hacia la puerta (se usa la flecha del clóset y `SetControlRotation`).

### Pendiente
- Probarlos en la próxima build (opción 2 de `Probar.bat`, pasillo).

## [0.3.56] — 2026-11-16 · Build más liviana (sin build)

Medición de la v0.3.54: el paquete pesa 3,0 GB y 2,4 GB son texturas, la mayoría en 4K.

### Cambiado
- **`Config/DefaultDeviceProfiles.ini` nuevo.** Limita las texturas de la build de Windows: color hasta 2048 y normales, especular y efectos hasta 1024. No recompila shaders. Para volver al original basta borrar el archivo.
- **Plugin `NNEDenoiser` desactivado** en el `.uproject` (unos 95 MB de modelos para ray tracing que el juego no usa).
- **`Empaquetar_Demo.ps1` excluye el `.pdb`** (símbolos de depuración, unos 175 MB) del ZIP.

## [0.3.55] — 2026-11-16 · Linterna realista en 1ª persona (sin build)

### Cambiado
- **`ColocarLinterna` en el jugador**, llamada al final de `CambiarCamara`. En 1ª persona el foco y el modelo de la linterna cuelgan de la cámara (abajo a la derecha), así el haz sigue la mirada y nace en la linterna. El modelo pasa a ser `SM_Flashlight_V2`, el mismo que se recoge del mostrador. En 3ª persona vuelve a la mano y al cuerpo como antes.
- **Sombras de la linterna siempre activas**, también en calidad Baja (se quita el apagado de v0.3.54). Sombras de contacto en el foco (`ContactShadowLength` 0,04) y `SourceRadius` 1,2. En Baja la resolución de sombras sube de 512 a 1024.

## [0.3.54] — 2026-11-16 · Calidad Baja más liviana (sin build)

### Cambiado
- **`CalidadExtra` en el jugador**, llamada desde `AplicarOpciones`. Solo en calidad Baja: la linterna deja de proyectar sombras, suavizado TAA en vez de TSR, tope de 30 fps, sin sombras de contacto, sin oclusión por campos de distancia y sin MegaLights. En Media o superior se restaura todo.

## [0.3.53] — 2026-11-16 · Arreglos del primer playtest (sin build)

### Corregido
- **La tecla E abría el candado en el hospital.** `ActualizarCandado` medía la distancia a un candado que en `L_Hospital` no existe y la daba por 0 ("cerca"). Ahora exige que el objeto exista. Mismo arreglo en `ActualizarCerca` para fonógrafo, reloj y caja fuerte.
- **Isidora partía con la linterna en la mano.** `LinternaMano` nunca se ocultaba. Ahora parte oculta y `RefrescarLinternaMano` la muestra solo cuando `TieneLinterna` es verdadero.

## [0.3.52] — 2026-11-16 · Volumen general en el menú (etapa E2, sin build)

### Añadido
- **Fila "General" en Configuración y en la pausa.** Baja o sube todo junto (efectos, ambiente y música). Queda entre "Dificultad" y "Volver al menú principal", que pasa a ser la fila 8.
  - Se guarda con las demás opciones (slot "Opciones"). Las partidas con opciones antiguas cargan con General al 100 %.
  - Funciona con teclado, mando y mouse (clic y hover), igual que las otras filas.
- Funciones nuevas en el jugador: `LineaGeneral`, `AjustarGeneral`, `CargarGeneral`, `RefrescarFila7` y `CfgSel7`. En `WBP_Menu`: `CfgBtn7` y `CfgRow7`.

## [0.3.51] — 2026-11-15 · Efectos y música por separado (etapa E2, sin build)

### Cambiado
- **La opción "Efectos" del menú ya no baja la música.** Antes movía el volumen general (clase `Master` con sus hijas), así que "Efectos 0 %" también silenciaba la música. Ahora controla solo los efectos y el ambiente (`Master` sin hijas, `SC_SFX` y `SC_Ambiente`); "Música" sigue controlando la música. No cambian las filas del menú ni el guardado de opciones.
- **Nuevo valor `Audio_MezclaAmbiente` en `DA_Tuning` (1,0):** multiplica el volumen del ambiente (lluvia, zumbidos) respecto a los efectos.
- **Nuevo interruptor `Latido_Sombra` en `DA_Tuning` (activo):** apaga o enciende la reacción del latido a la Sombra.

## [0.3.50] — 2026-11-15 · El latido siente a la Sombra (etapa E3, sin build)

### Cambiado
- **`BP_LatidoMiedo` ahora reacciona a `BP_AI_Sombra`.** Además de `BP_Enemigo`, busca todas las Sombras y se queda con la distancia más corta: el corazón se acelera cuando la Sombra se acerca, igual que con los otros enemigos.
  - Usa los mismos valores de `DA_Tuning`: `Latido_Alcance` y `Latido_VolMax` (con `Latido_VolMax` en 0 se apaga).
  - No se tocaron los nodos de audio (`AdjustVolume`, `SetPitchMultiplier`).

## [0.3.49] — 2026-11-15 · El payaso como amenaza real (etapa E3, sin build)

### Cambiado
- **`BP_PayasoAcecho` ya no solo mira: se acerca.** Mientras está a la vista:
  - **Si no lo miras** (producto punto < 0,5), da un paso hacia ti cada 0,25 s, de `Payaso_PasoCm` (110 cm) por paso. Se queda pegado al suelo y se oye un paso suave.
  - **Si lo miras, se congela.** Si lo miras fijo durante `Payaso_MiradaSeg`, desaparece y se oye una respiración detrás de ti.
  - **Si llega a menos de `Payaso_DistGolpe` (2,3 m), te golpea:** chillido, vibración y 1 de daño. Después desaparece hasta su próxima aparición.
  - Con el avance activo ya no huye cuando te acercas (`Payaso_DistHuye` solo cuenta si `Payaso_Avanza` está apagado). Se va al cumplir `Payaso_DuracionSeg`, que **sube de 9 a 20 s**.
- **Nuevos valores en `DA_Tuning`:** `Payaso_Avanza` (activo), `Payaso_PasoCm` (110) y `Payaso_DistGolpe` (230).

## [0.3.48] — 2026-11-15 · Las llaves y los flags se guardan en la partida (cierre de la etapa E2, sin build)

### Agregado
- **Los flags del GameInstance (llaves y eventos) ahora se guardan con la partida**, así que sobreviven a cerrar el juego y volver con **Continuar**.
  - **`BP_GI_MundoMagico`** tiene 3 funciones nuevas:
    - `GuardarFlags`: guarda los flags en el slot `Flags`, usando el SaveGame `BP_Partida`, que ahora tiene el arreglo `Flags`.
    - `CargarFlags`: los recupera.
    - `LimpiarFlags`: vacía los flags y borra ese slot.
  - **El jugador:**
    - `Guardar` llama a `GuardarFlags` justo después de guardar la partida.
    - `Cargar` llama a `CargarFlags` apenas lee la partida.
    - **Iniciar juego** (`MenuConfirmar`, opción 0) llama a `LimpiarFlags`, para que no queden llaves de una partida anterior.

## [0.3.47] — 2026-11-15 · La Sombra con linterna (etapa E3)

### Agregado
- **`BP_AI_Sombra`** (`/Game/MundoMagico/IA`): un enemigo nuevo hecho con el modelo y las animaciones de la IA del Horror Template, con lógica propia sin Behavior Tree y todos sus valores en `DA_Tuning`.
  - **Linterna en la mano** (`Haz`): un foco cálido de 22°, con polvo en el haz y sin sombras, para cuidar la iGPU. **El haz es su cono de visión.**
  - **Vista:** te ve si estás dentro del cono (22°) y a menos de 22 m, sin nada en medio. A menos de 2,5 m te ve igual, aunque estés detrás. **Escondido no te ve.**
  - **Oído:** escucha los pasos fuertes de `AC_Ruido` (correr, metal, madera) a menos de 18 m y **va a investigar** ese punto.
  - **Estados:**
    1. **Patrulla:** camina a puntos al azar a menos de 12 m de su posición inicial.
    2. **Investigar:** va al último ruido o al último lugar donde te vio, y a los 6 s vuelve a patrullar.
    3. **Persecución:** corre hacia ti. Si te pierde de vista más de 4 s, vuelve a investigar.
  - **Ataque:** a menos de 1,5 m golpea, con un intervalo de 1,5 s entre golpes, usando el mismo daño que el enemigo actual.
  - **Al verte por primera vez** suena un chillido y se registra en el Director de miedo como tipo enemigo con intensidad 30.
- **Primera Sombra en el mapa:** `ENE_Sombra_Pasillo`, en el tramo final del Pasillo de Urgencias (0, −3200), después del susto. Está en la carpeta `Hospital/IA`.
- **Nuevos valores en `DA_Tuning`:** `Sombra_Activa`, `Sombra_VistaDist`, `Sombra_VistaAngulo`, `Sombra_OidoDist`, `Sombra_PierdeSeg`, `Sombra_VelPatrulla` (140), `Sombra_VelAlerta` (240), `Sombra_VelPersecucion` (420), `Sombra_DistAtaque` y `Sombra_RadioPatrulla`.

## [0.3.46] — 2026-11-12 · Llaves con nombre, puertas cerradas y GameInstance (etapa E2, sin build)

### Agregado
- **`BP_GI_MundoMagico`** (`/Game/MundoMagico/Core`): ahora es el **GameInstance del proyecto** (`GameInstanceClass` en `DefaultEngine.ini`).
  - Guarda los **flags** (llaves y eventos) y se conservan aunque cambies de mapa (hospital → parque).
  - Tiene `AgregarFlag(Flag)`. Las puertas leen `Flags` directamente.
- **`BP_DoorLocked`** (`/Game/MundoMagico/Mundo`): una puerta del Horror Template con ícono y animación de abrir.
  - **Se abre solo si existe el flag que lleva como primera etiqueta** (por ejemplo `LlaveMorgue`).
  - Si no está, suena el picaporte y aparece "Está cerrada. Algo tiene que abrirla."
- **`BP_LlaveFlag`**: una llave que se recoge con □/E.
  - **Agrega su primera etiqueta como flag** y muestra "Encontraste: <nombre>".
  - Una llave con la etiqueta `LlaveMorgue` abre la puerta con la misma etiqueta.
- Las dos usan la interfaz de interacción que ya existía (`BPI_Interaction`), así que funcionan con el mismo botón sin tocar `UsarPuerta`.

### Pendiente
- **Los flags todavía no se guardan en la partida:** se pierden al salir del juego y volver con Continuar. Entra con `SG_Partida`.
- **Las puertas con código numérico** van con el puzle del negatoscopio (E4).

## [0.3.45] — 2026-11-12 · Ruido según el suelo (etapa E2, sin build)

### Agregado
- **`AC_Ruido`** (componente en `/Game/MundoMagico/Componentes`, agregado al jugador). En cada paso calcula **qué tan fuerte suena** según el suelo y cómo te mueves:
  - **Superficie:**

    | Superficie | Multiplicador |
    |---|---|
    | Metal, reja, hierro y la zona de los autos | 1,8 |
    | Madera, escenario y las zonas del teatro y la casa | 1,6 |
    | Charco | 1,4 |
    | Baldosa | 1,0 |
    | Grava (camino) | 0,9 |
    | Tierra, pasto y la zona de la bodega | 0,6 |
    | Alfombra | 0,3 |

  - **Forma de moverte:** caminar ×1,0, correr ×2,5 y agachado ×0,3.
  - **Ruido = movimiento × superficie.** El radio en que se oye es `Ruido_RadioBase` × ruido (1500 cm caminando sobre baldosa).
  - El componente emite un **evento de ruido de IA** (`ReportNoiseEvent`, etiqueta "Paso"). Lo va a escuchar la Sombra con linterna en la etapa E3.
  - Los pasos fuertes (ruido de 1,2 o más, como correr o pisar metal o madera) **también alertan al enemigo actual**: actualizan `RuidoPos` y `RuidoT`.
- **Nuevos valores en `DA_Tuning`:**
  - `Ruido_Activo`;
  - un multiplicador por superficie (`Ruido_Metal`, `Ruido_Madera`, `Ruido_Charco`, `Ruido_Baldosa`, `Ruido_Grava`, `Ruido_Tierra`, `Ruido_Alfombra`);
  - `Ruido_Caminar`, `Ruido_Correr` y `Ruido_Agachado`;
  - `Ruido_RadioBase`.
- **El panel `-dev` muestra además** el **ruido** del último paso (×10) y la **tensión** del Director.

## [0.3.44] — 2026-11-12 · Director de miedo (etapa E2, sin build)

### Agregado
- **`BP_DirectorMiedo`** (actor `DEV_DirectorMiedo` en los dos mapas): un solo reloj para todos los sustos.
  - Lleva una **tensión** de 0 a 100 que baja con el tiempo, a `Director_Decae` puntos por segundo.
  - Fija un **enfriamiento global** de `Director_Enfriamiento` segundos (40 por defecto) entre sustos.
  - **No repite el mismo tipo** dos veces seguidas, salvo que haya pasado el doble del enfriamiento. Los tipos son: 0 audio, 1 visual, 2 físico, 3 enemigo y 4 guion.
  - `PedirTurno(Tipo, Intensidad)` lo usan los sustos que pueden esperar. `Registrar(Tipo, Intensidad)` lo usan los sustos guionados, que siempre ocurren pero cuentan para el ritmo.
- **Qué sustos usa cada uno:**
  - **El payaso acosador** pide turno antes de aparecer, como tipo enemigo con intensidad 25. Si el Director lo niega, reintenta a los 8 s.
  - **Isidora** pide turno antes de aparecer, como tipo visual con intensidad 15.
  - **El teatro y la noria** registran su susto como tipo guion con intensidad 40. Así, después de uno de ellos, el payaso o Isidora esperan su turno.
- **Nuevos valores en `DA_Tuning`:** `Director_Activo`, `Director_Enfriamiento` (40 s) y `Director_Decae` (1,5 por segundo).

## [0.3.43] — 2026-11-12 · Cámara en 1ª persona como modo principal (etapa E2, sin build)

### Cambiado
- **El juego arranca en 1ª persona** (decisión del CTO). La 3ª persona sigue como opción con el botón de cambiar vista que ya existía (`CambiarCamara`).
  - `BP_GestorNivel.AplicarCamara` se ejecuta 0,4 s después de cargar cada mapa. Si `DA_Tuning.Camara_PrimeraPorDefecto` está activo, cambia a 1ª persona y quita el retraso de cámara para que no se sienta flotante.
  - **Altura de los ojos:** en 1ª persona la cámara sube de 25 cm sobre el centro de la cápsula (la altura del pecho) a 57 cm, con 10 cm hacia adelante (`SocketOffset` = 10, 0, 32).
  - **Nuevo valor en `DA_Tuning`:** `Camara_PrimeraPorDefecto` (activo). Si lo desmarcas, el juego vuelve a arrancar en 3ª persona.

## [0.3.42] — 2026-11-12 · Hospital y parque en dos mapas (etapa E1 de la arquitectura en 3 niveles). La build del 12-11 no trae la transición TV/VHS

### Cambiado
- **El mapa único `L_Pabellon4` se separó en `L_Hospital` y `L_Parque`.** `L_Pabellon4` queda congelado como respaldo.
  - **`L_Hospital`:** sala, pasillo de urgencias y patio de salida. Tiene 432 actores; se le quitaron 1 404 del parque, junto con el terreno, el bosque y las rejas.
  - **`L_Parque`:** entrada, avenida y atracciones. Tiene 1 427 actores; se le quitaron 409 del hospital. El `PlayerStart` quedó en la reja de entrada (0, −4300).
  - Las coordenadas no cambian, así que todas las funciones del jugador siguen funcionando igual.
- **Al parque solo se llega con el glitch** (`BP_GestorNivel`, actor `DEV_GestorNivel` en los dos mapas):
  - En el hospital, cuando el glitch de salida te lleva al otro lado, se guarda la partida, se crea el aviso `Transicion` y se abre `L_Parque`.
  - En el parque, se lee ese aviso, la partida arranca sola con la pantalla "MUNDO MÁGICO" y te deja en la reja de entrada.
  - Si abres el parque sin venir del glitch (y sin `-ir=`), te devuelve al hospital, que es donde está el menú.
  - **Continuar** con una partida que ya salió del hospital te lleva directo al parque.
- **"Volver al menú"** abre `L_Hospital`.
- **Configuración y herramientas:**
  - `GameDefaultMap` y `EditorStartupMap` ahora son `L_Hospital`.
  - La build cocina los dos mapas.
  - `Abrir_MundoMagico*.ps1` abre `L_Hospital`.
  - `Probar.bat` abre el mapa que corresponde a cada zona.

### Agregado (12-11, después de la build)
- **Transición TV → VHS entre los dos mapas** (`WBP_Transicion`, a cargo de `BP_GestorNivel`):
  - **Salida del hospital, 1,6 s:** la imagen se encoge a una línea blanca y luego a un punto que se apaga, con un "tuk" del tubo y un pitido agudo que se va (`SFX_TV_Apagar`). Después viene el negro y se abre `L_Parque`.
  - **Llegada al parque, 2,4 s:** en negro aparece "▶ PLAY 03:17" parpadeando y un grano VHS que tiembla, con el clac del mecanismo, el motor y el soplido de la cinta (`SFX_VHS_Play`, sin estática de TV). Después el parque aparece con un fundido.
  - Los sonidos se componen con `herramientas/componer_transicion_tv.py` y se importan con `importar_transicion_tv.py`.
- **`DEV_PisoGlitch`:** un piso invisible en el punto de llegada del glitch dentro de `L_Hospital`, para que el jugador no caiga mientras la TV se apaga.

## [0.3.41] — 2026-11-11 · Kit de afinación del CTO (fase 1 del plan de mejora rápida, sin build)

### Agregado
- **`DA_Tuning`** (`/Game/MundoMagico/Datos`, clase `BP_AfinacionDatos`): 18 valores e interruptores que el CTO cambia con doble clic, sin tocar Blueprints.
  - Payaso acosador: espera, frecuencia, veces, distancias, mirada y duración.
  - Isidora: distancias.
  - Latido: alcance y volumen.
  - Interruptores de los sustos del teatro y la noria.
  - Los leen en `BeginPlay` `BP_PayasoAcecho`, `BP_IsidoraAparicion`, `BP_LatidoMiedo`, `BP_SustoTeatro` y `BP_SustoNoria`. El cook incluye siempre `/Game/MundoMagico/Datos` (`DirectoriesToAlwaysCook`).
- **Saltos de prueba `-ir=<zona>`** (`BP_KitPrueba`, actor `DEV_KitPrueba` en `Juego/Dev`):
  - Al abrir el `.exe` con `-ir=teatro`, el juego arranca solo y te deja en la zona.
  - Zonas: hospital, pasillo, parque, teatro, noria, bodega y autos. Usan los `RSP_*` con la etiqueta `Ir_<zona>`.
- **Panel `-dev`:** un texto arriba a la izquierda (`DevTexto` en `WBP_HUD`) con fps, posición X/Y y batería. Se actualiza cada 0,5 s.
- **`Probar.bat`** en la raíz del repo: un menú del 1 al 8 que abre la build más reciente en la zona elegida, con el panel encendido.
- **`GUIA_TUNING.md`**: qué valor hace qué, su rango seguro, cómo probar y qué se ajusta a mano en el editor.

## [0.3.40] — 2026-11-11 · Hospital AAA, parte 1 (sin build: entra en la build del 12-11)

### Agregado
- **Balizas de emergencia giratorias** (`LampsPack`, `BP_Lamp_Flat_Crazy`), colgadas del techo y teñidas de rojo (luz roja de 750 de radio, sin sombras para cuidar el rendimiento, con dispersión volumétrica). Materiales nuevos: `MI_Baliza_Vidrio_Rojo` y `MI_Baliza_Brillo_Rojo` en `/Game/MundoMagico/Luces`.
  - Hay una en el pasillo de salida de la sala, encendida desde el inicio.
  - Hay tres en el Pasillo de Urgencias, **apagadas hasta el susto**: aparecen con la etiqueta `SangrePasillo` en el destello blanco, como una alarma que se dispara después de que pasa la criatura.
- **Utilería macabra** (`TA_CorpsesPackSample`): hay un cuerpo en la sala, otro en el pasillo de salida y otro en el Pasillo de Urgencias. El de Urgencias aparece solo después del susto.
- **Papeles físicos** (`SM_Notes`):
  - Hay notas con sangre en las paredes de la sala y de los dos pasillos; la de Urgencias aparece después del susto.
  - Hay seis papeles arrugados en el suelo.
- **Alarma de emergencia** (`SFX_Alarma`, compuesta con `herramientas/componer_alarma.py` e importada con `importar_alarma.py`): es una sirena lenta de dos tonos que suena por un parlante roto. Al final se corta la corriente, el tono cae y se apaga. Suena en `BP_SustoPasillo` justo cuando se encienden las balizas, después del destello.
- **Las 62 páginas recogibles cambian a papeles de terror** (`SM_Notes`): ahora usan papel envejecido, viejo, quemado, con esquina rota y con sangre, en lugar del papel limpio de oficina. Cada una tiene un giro distinto en el suelo.

- **Sustos reales en el teatro y la noria** (`BP_SustoTeatro`, `BP_SustoNoria`, en la carpeta `Parque/Sustos`). Cada uno se activa una sola vez, cuando el jugador entra a la zona:
  1. Baja el sonido y se escucha una respiración.
  2. Se enciende un foco que deja ver a la criatura: de pie en el escenario, o arriba de la noria, en la cabina más alta.
  3. El foco se apaga y la criatura desaparece.
  4. A los 1,1 s aparece **frente al jugador** con el ataque, el chillido y la vibración del control. Después hay un corte a negro.
  5. Sale un mensaje en pantalla.

- **Payaso acosador** (`BP_PayasoAcecho`, actor `ENE_PayasoAcecho` en `Parque/Sustos`): aparece a lo lejos en el parque, fuera de la persecución.
  - **Cuándo aparece:**
    - La primera vez, a los 45 s de juego como mínimo. Después, cada 40–80 s.
    - Como máximo 6 veces por partida, y solo en el parque (y < −4700).
    - Nunca durante la persecución ni después del final.
  - **Dónde aparece:** a 22–32 m, dentro de ±30° de donde mira la cámara, y solo si la línea de visión está despejada. Se para sobre el suelo real, te mira y queda quieto (`anim_idle_1`).
  - **Cuándo desaparece:**
    - Si te acercas a menos de 15 m.
    - Si lo miras fijo durante 1 s.
    - Si pasan 9 s.
  - Al desaparecer, suena una **respiración detrás del jugador**.

- La utilería nueva no tiene colisión, así que no estorba al caminar ni cambia el sonido de los pasos. Todo está en la carpeta `Hospital/AAA` del Outliner.

### Cambiado (playtest del CTO: "no noté el latido ni a Isidora")
- **Isidora** (`BP_IsidoraAparicion`, en el teatro, la bodega y la noria):
  - Ahora aparece entre **10 y 35 m**; antes, entre 11 y 23 m.
  - Al aparecer, suena un **glitch de radio** desde donde está y el control da una vibración suave.
  - Desaparece cuando te acercas a menos de 9 m, con una respiración.
- **Latido del miedo** (`BP_LatidoMiedo`):
  - Ahora también se acelera cuando **el payaso acosador está en pantalla**; antes solo reaccionaba al enemigo de la persecución.
  - Volumen máximo de 1,0 (antes 0,85) y alcance de 28 m (antes 25 m).
- **Descompresión tras el glitch:** el payaso acosador no aparece hasta **60 s después de llegar al parque**. Antes podía aparecer apenas llegabas, porque contaba desde el inicio de la partida.

## [0.3.39] — 2026-11-11 · Reverb por zona

### Agregado
- **Efectos de reverb** (`/Game/MundoMagico/Audio/Mezcla`), creados con `herramientas/crear_reverbs.py`:
  - `RV_Sala`: cuarto chico, seco, con decay de 0,75 s.
  - `RV_Pasillo`: pasillo largo de baldosa, con decay de 2,6 s y reflexiones marcadas.
  - `RV_Exterior`: parque abierto, con cola leve.
- **`AplicarReverb`** (BP del jugador): se llama al cambiar de zona, en `ActualizarZona` después de `Guardar`. Aplica el reverb que corresponde con un fundido de 1,5 s:
  - Pasillo de Urgencias: `RV_Pasillo`.
  - Pabellón 4, Teatro, Casa y Bodega: `RV_Sala`.
  - Parque: `RV_Exterior`.
- El reverb afecta los pasos, la criatura, las puertas y todo el sonido del juego, así que cada lugar suena distinto.

## [0.3.38] — 2026-11-11 · Puertas con pantalla de carga (build del día, junto con la 0.3.37)

### Corregido
- **El sonido de "abrir puerta" sonaba al recoger cosas.** `UsarPuerta` toma el interactuable más cercano a menos de 2,3 m, y cualquiera que no fuera clóset, llave o puerta con llave (por ejemplo los 85 `BP_Recogible`) reproducía `SW_Open_Door`. Ahora solo suena si el objeto es una puerta real (`CastToBP_Creepy_Door`).

### Agregado
- **Pantalla de carga al estilo PlayStation** (`WBP_HUD`):
  - `CargaFondo`: negro a pantalla completa (ZOrder 30).
  - `CargaTitulo`: el nombre del lugar en Special Elite 46.
  - `CargaSub`: "CARGANDO" abajo a la derecha.
- **`MostrarCarga(Texto)` y `OcultarCarga`** (BP del jugador): muestran la pantalla 2,4 s con los controles bloqueados. Se usan:
  - al abrir la puerta de la sala al pasillo (`BP_Creepy_Door`): "PASILLO DE URGENCIAS";
  - al llegar al parque después del glitch (`GlitchLlegar`): "MUNDO MÁGICO".
- La silueta del sombrero ahora espera 3,2 s (antes 1,8), para aparecer después de la pantalla de carga del parque y no detrás de ella.

## [0.3.37] — 2026-11-11 · Pasos según el suelo real

### Agregado
- **Packs de Fab** agregados por el CTO: **Essential Footsteps** (botas en 10 superficies), **AutoFootsteps** (agua y madera) y **HeartBeats** (latidos de terror, médicos y de acción, para usar después).
- **`PasoSuelo2` y `TocarPaso`** (BP del jugador): cada paso mira **la malla que pisa Isidora** (`GetMovementBaseActor` → `StaticMesh`) y elige el sonido de bota, caminando o corriendo:

  | Pisa… | Suena |
  |---|---|
  | charco | barro mojado |
  | rejas, hierro, metal, o la zona de autos chocones | metal |
  | madera o escenario, o la zona del teatro o la casa | madera |
  | el camino del parque | grava |
  | terreno o pasto, o la zona de la bodega | tierra |
  | cualquier otra cosa (hospital) | concreto |

- `Paso` ahora llama a `PasoSuelo2`. El `PasoSuelo` antiguo, que elegía solo según la zona, queda sin uso.

### Nota
- Los packs de Fab no entran en `Versionar_Unreal.ps1`, así que viven solo en este PC. Para reconstruir en otro equipo hay que agregarlos desde Fab.

## [0.3.36] — 2026-11-11 · Slice AAA, Fase 1: linterna y cámara (build del día)

### Agregado
- **`IES_Linterna`** (`/Game/MundoMagico/Luces`): perfil de luz IES generado por script (`herramientas/crear_ies_linterna.py`), con centro caliente, anillo del reflector y caída suave. Se importa con `importar_ies.py`, que también lo asigna a la linterna.
- **`ParpadeoLinterna`** (BP del jugador): con la batería bajo 20 %, la linterna baja de intensidad y parpadea. Mientras menos batería, más seguido. Se llama desde `ActualizarBateria`.
- **Recursos de Fab**: revisión de los 114 de la biblioteca, con un plan de uso por lotes en `auditoria/fab_recursos_slice.md`.

### Cambiado
- **Linterna**: 4700 K, cálida como una incandescente de 1998 (antes 6300 K). Conos interior y exterior de 12° y 30° (antes 9° y 34°). Dispersión volumétrica de 3,5 (antes 2), para que el haz se vea en la niebla.
- **Cámara al hombro**, estilo RE2 remake: brazo de 190 cm (antes 270) y desplazamiento lateral de 60 y vertical de 55 (antes 50 y 65). Cambia en el arranque, en `CambiarCamara` y en `CamaraDinamica`, que mantiene el balanceo al caminar.

## [0.3.35] — 2026-11-11 · Correcciones de la prueba del CTO (build única del día)

Responde a lo que el CTO vio jugando la v0.3.34 con `auditoria/checklist_v0.3.34.md`.

### Cambiado
- **Visibilidad**: era el problema principal, porque no dejaba ver casi nada de lo nuevo.
  - Exposición: `autoExposureMin/MaxBrightness` baja de 1,1 a 0,4 (la imagen se ve más clara).
  - Luna: de 0,5 a 1,2 lux.
  - SkyLight: de 0,16 a 0,3.
- **Faroles con halo**:
  - 49 luces del parque con `VolumetricScatteringIntensity` 3: el haz se ve en la niebla.
  - `BP_FarolMuere`: de 7 a 18 cd, radio 10 m y dispersión volumétrica 5, para que se note cuando se apagan.
  - Faroles con parpadeo: `LightBrightness` de 1000 a 2500.
- **Rocas**: seguían claras porque la textura del pack es casi blanca. BaseColor ×0,22 (antes 0,55).
- **Linterna**: gasta 0,22 % por segundo en vez de 0,4. Dura unos 7,5 min en vez de 4.
- **Reaparecer**: nueva función `PuntoRespawn`. `Revivir` te lleva al punto fijo **más cercano** de los 10 `TargetPoint` con tag `Respawn` (carpeta `Juego/Respawn`): sala, pasillo, entrada, avenida, avenida al fondo, noria, teatro, bodega, casa y autos chocones. Antes el punto de control se actualizaba también al cambiar de objetivo, y te dejaba en lugares raros.
- **Silueta del sombrero más terrorífica**:
  - más alta y delgada (cuerpo con escala 2,3 × 3,2);
  - sombrero y ala más arriba;
  - **dos ojos rojos encendidos** (`OjoI`/`OjoD` con `MI_LedRojo`).
  - `ENT_SustoEnergia` se volvió a colocar para que tome los cambios.

### Por revisar
- El glitch de salida del hospital no se activó en la prueba. Es probable que la partida viniera de **Continuar**, porque `SalidaHecha` se guarda. Hay que probar con **Iniciar juego**.

## [0.3.34] — 2026-11-11 · Teaser "El último turno" (del atardecer a la noche)

### Agregado
- **`BP_TeaserDirector`**: en modo `-teaser` ahora hace un ciclo de atardecer a noche en los primeros 18 s.
  - Luz direccional: el sol baja de −35° a +5° y pasa de naranja a rojo, mientras su intensidad cae de 8 a 0. Después vuelve la luna (0,5 lux, azul).
  - Luz de cielo: baja de 1,6 a 0,16.
  - Recién de noche aparecen las estrellas (tag `Estrellas`) y la cúpula del cielo (tag `CieloDomo`).
  - La silueta del sombrero aparece solo cuando ya es de noche.
  - Las 7 tomas parten a través de la reja, mirando hacia el atardecer.
- **Banda sonora compuesta por script** (`herramientas/componer_teaser_vals.py` y la versión corta `_corto.py`):
  - un vals de feria (cajita y organillo) que se desafina, se ralentiza y se apaga como cinta estirada;
  - un dron grave y un latido que se acelera;
  - un segundo de silencio total y un golpe con chirrido metálico;
  - notas finales de cajita desafinada sobre el título.
- **`herramientas/montar_teaser_final.ps1`**: arma el corte final de 39 s con dos textos, "EL PARQUE CERRÓ HACE AÑOS." y "PERO NADIE LE AVISÓ A ELLA.".
- **Video**: `Teaser/Teaser_ElUltimoTurno.mp4`, más una copia liviana `_movil.mp4`. No se versionan.

### Pendiente
- El cielo sigue negro durante el atardecer: solo el suelo y los árboles toman la luz dorada.
- En la grabación el juego se cerró una toma antes, así que la toma de la silueta se tomó del primer teaser.

## [0.3.31] — 2026-11-10 · Luz de luna (build de prueba)

### Corregido
- **La luna no iluminaba.** `DirectionalLight_0` tenía `LightColor` en (0,0,0), así que aportaba cero luz y lejos de los faroles todo quedaba negro. Probablemente quedó así en una pasada de colores anterior.
- Ahora tiene el azul frío original de la especificación del parque (0x5577BB), con intensidad 0,5 lux y sin temperatura. Las sombras siguen activas, así que no entra al hospital.

### Notas
- Falta verificarla jugando: la captura automática no pudo grabar la ventana del juego.
- Si queda muy clara o muy oscura, se ajusta con un solo número (`Intensity`).

## [0.3.30] — 2026-11-10 · Audio espacial (build de prueba)

### Agregado
- **`ATT_Puntual`** (SoundAttenuation), creado con `herramientas/audio_espacial.py`.
  - Esfera de 2,5 m más caída natural de 35 m.
  - Filtro de agudos por distancia (a 35 m solo pasan hasta 2,5 kHz), para que lo lejano suene apagado.
  - Dentro de 1,2 m deja de ser espacial, así un sonido pegado al jugador no salta de oído.
- **Asignado a 34 sonidos que salen de un lugar concreto**:
  - metales que crujen y lámparas que se sacuden;
  - chispas y vapor;
  - golpes, chillido y carrera de la criatura, vidrio y pedestal;
  - puertas, llave y cajas de madera.
- No se tocan los ambientes generales (viento, lluvia, música) ni los pasos del jugador, que siguen sonando en todas partes. Los sonidos 2D ignoran la atenuación.

## [0.3.29] — 2026-11-10 · Vibración del control (build de prueba)

### Agregado
- **Efectos de vibración** en `/Game/MundoMagico/FX`, creados con `herramientas/crear_vibraciones.py`. Las curvas se pusieron por MCP.
  - `FFE_Golpe`: 0,35 s, fuerte, con todos los motores.
  - `FFE_Susto`: 0,8 s, sube de golpe y se va apagando, con todos los motores.
  - `FFE_Trueno`: 1,6 s, retumbo grave, solo con los motores grandes.
- **Dónde vibra** (`ClientPlayForceFeedback` sobre el PlayerController):
  - Al recibir un golpe (`RecibirGolpe`).
  - En cada trueno (`Trueno`).
  - En `MostrarSusto`.
  - Cuando aparece la silueta del sombrero (`BP_SustoEnergia`).
  - Cuando aparece la criatura del pasillo (`BP_SustoPasillo`).

## [0.3.28] — 2026-11-10 · Guardado al entrar a cada zona (build de prueba)

### Agregado
- **Punto de control por zona.** Al entrar a una zona nueva (las mismas que muestran su nombre en pantalla: Pabellón 4, Pasillo de Urgencias, Parque, Bodega, Teatro, Autos Chocones y Casa del Parque), `ActualizarZona` guarda tu posición en `PosControl` y llama a `Guardar`.
- Si mueres, `Revivir` te devuelve a la entrada de la última zona. **Continuar** en el menú también parte desde ahí.
- El guardado automático cada 30 s y al pausar sigue igual.

## [0.3.27] — 2026-11-10 · Silencio antes del susto (build de prueba)

### Agregado
- **`SM_Silencio`** (SoundMix): baja SC_Ambiente y Music a 0,03. Entra en 0,35 s y sale en 2,5 s. Los efectos (SC_SFX) no se tocan. Se crea con `herramientas/crear_mezcla_silencio.py`.

### Cambiado
- **`BP_SustoEnergia`** (silueta del sombrero): al dispararse, el ambiente y la música se apagan. La espera antes de mostrar la silueta sube de 1,2 a 1,8 s, para que se note el vacío. El sonido vuelve poco a poco cuando la silueta desaparece.
- **`BP_SustoPasillo`**: el silencio entra con el mensaje "Las luces empiezan a apagarse" y dura todo el susto. El ambiente vuelve con el último sonido.

## [0.3.26] — 2026-11-10 · Flashbacks sobre negro (build de prueba)

### Cambiado
- **Flashbacks de páginas** (las páginas 1, 5, 8, 12, 14 y 17 muestran burbujas de diálogo).
  - Antes aparecían sobre el juego en movimiento, con el fondo y la lluvia. Ahora la pantalla queda en negro durante todo el flashback y el juego vuelve cuando termina.
  - `WBP_HUD`: imagen nueva `FBNegro`, negra y a pantalla completa, con ZOrder 5. Las burbujas `FBBurbuja` y `FBOnoma` pasan a ZOrder 10, para quedar encima.
  - `BP_ThirdPersonCharacter`: `AntesDePagina` sube la opacidad de `FBNegro` a 1 y `SiguienteLineaFB` la baja a 0 al terminar.

## [0.3.25] — 2026-11-10 · Rocas que brillaban en la noche (build de prueba)

### Corregido
- **`M_RocksStylized`** (pack FreePack-RocksStylized, usado por las 34 rocas del parque).
  - Antes era un material sin iluminación (Unlit) con la textura en Emissive, así que las rocas brillaban blancas en plena noche. Se vio en la grabación del teaser.
  - Ahora es un material iluminado normal (DefaultLit): la textura × 0,55 va a BaseColor y la rugosidad es 0,92.
  - Ese pack no está en la lista de `Versionar_Unreal.ps1`, así que este cambio vive solo en el proyecto local.

### Notas
- No se pudo verificar con captura, porque Windows negó el acceso a la duplicación de escritorio en ese momento. Hay que confirmarlo jugando.
- La iluminación general del juego empaquetado (DX11) se ve muy oscura lejos de las luces. Se dejó así a propósito; queda pendiente de decidir.

## [0.3.24] — 2026-11-10 · Modo teaser y primer corte del teaser

### Agregado
- **`BP_TeaserDirector`** (`TEASER_Director` en el mapa)
  - Solo se activa al abrir el juego con `-teaser`. Entonces quita los widgets, despausa, toma la cámara (CineCamera de 24 mm, sin desenfoque) y muestra la silueta y las apariciones de Isidora.
  - Recorre 7 tomas definidas en los arreglos `IniPos`, `FinPos`, `Mira` y `Dur`, con un movimiento suave. Al terminar, cierra el juego.
- **`herramientas/montar_teaser.ps1`**: arma el teaser con FFmpeg a partir de la grabación.
  - Elige los mejores tramos y les aplica etalonaje, franjas de cine, fundidos y textos en Special Elite.
  - Agrega la tarjeta final "MUNDO MÁGICO / TURNO NOCTURNO / PRÓXIMAMENTE", la música de terror y el latido acelerado sobre la silueta.
- **Primer corte**: `Teaser/Teaser_MundoMagico.mp4`, de 25 s y 1280×720. No se versiona.

### Cómo grabar
- Ejecuta `MundoMagico.exe -teaser -windowed -ResX=1280 -ResY=720 -WinX=60 -WinY=60 -nosound`.
- Graba con FFmpeg `ddagrab` en la zona de la ventana. `gdigrab` deja la imagen negra porque no captura DirectX.

## [0.3.23] — 2026-11-10 · Isidora fantasma (build de prueba)

### Agregado
- **`M_Isi_Fantasma`**: material translúcido sin iluminación, de un blanco frío que brilla más en los bordes del cuerpo (Fresnel), con opacidad de 0,12 a 0,65.
- **`BP_IsidoraAparicion`**
  - Usa SKM_Isidora3 con las 11 ranuras de material en `M_Isi_Fantasma`, sin sombra ni colisión, en reposo lento (MM_Idle a 0,35).
  - Empieza invisible.
  - Aparece cuando el jugador está a entre 11 y 23 m, y se gira para mirarlo de frente.
  - Si el jugador se acerca a menos de 9,5 m, desaparece con una respiración y no vuelve.
- **3 apariciones** (`Parque/Vivo/Isidora`): `APA_IsidoraTeatro` (−5000, −11000), `APA_IsidoraBodega` (−6500, −5900) y `APA_IsidoraNoria` (2500, −9800).

## [0.3.22] — 2026-11-10 · Latido del miedo (build de prueba)

### Agregado
- **`SFX_Latido`**: latido grave (lub-dub) de 1 s, sin ruido y en loop exacto, con clase SC_SFX. Se compone con `herramientas/componer_latido.py` y se importa con `importar_latido.py`.
- **`BP_LatidoMiedo`** (`SND_LatidoMiedo`, en `Parque/Vivo`)
  - Cada 0,15 s mide la distancia a la amenaza más cercana: payaso, oso Willy o león.
  - A más de 25 m está en silencio. A menos de 3 m suena al máximo.
  - El volumen sube en curva (t²) y el ritmo se acelera de unos 51 a 105 latidos por minuto (pitch de 0,85 a 1,75).
  - No es espacial: se siente dentro de la cabeza.

## [0.3.21] — 2026-11-10 · Niebla en la avenida y faroles que mueren (build de prueba)

### Agregado
- **`BP_FarolMuere`** (Blueprint nuevo)
  - Es una luz cálida de 7 cd, 3100 K y sin sombras.
  - Cuando el jugador se acerca a menos de 7 m, parpadea, se apaga y queda a oscuras unos 9 s. Después vuelve con un parpadeo y se rearma 8 s más tarde.
  - Revisa la distancia cada 0,2 s.
- **8 faroles que se apagan al pasar** (`FAR_Muere*`) y **4 faroles con parpadeo verdoso** (`FAR_Parpadeo*`, `BP_FlickeringLight`), puestos en la cabeza de 12 de los 20 faroles que antes no tenían luz (carpeta `Parque/Faroles`). Los otros 8 siguen apagados.
- **7 bancos de niebla baja en la avenida y sus costados** (`NIE_Avenida*`, `BP_FogArea`). Usan el mismo color frío que `NIE_Jardin`, con densidad 0,45.

### Notas técnicas
- La lógica se escribió con `write_graph_dsl`, que solo funciona con el editor en inglés. Para usarla hay que abrir Unreal con `-culture=en`.

## [0.3.20] — 2026-11-10 · Portada estilo afiche de cine (build de prueba)

### Cambiado
- **Portada del menú** (`T_PortadaHTML`, con el mismo asset y otra imagen).
  - **Fondo:** el arte de la novela gráfica (`portadamuestra.jpg`), desaturado, oscurecido y con tinte frío.
  - **Encuadre:** degradado negro a la izquierda, viñeta y franjas negras de cine arriba y abajo.
  - **Título:** "MUNDO MÁGICO" en Special Elite, con una línea roja, "TURNO NOCTURNO" espaciado y la frase "El parque cerró hace años. Alguien sigue de turno."
  - **Créditos:** un bloque al pie, al estilo de un afiche.
  - **Herramientas:** se genera con `herramientas/componer_portada_cine.py` (Pillow) y se importa sin abrir Unreal con `importar_portada_cine.py`. La imagen anterior sigue en `Fuentes/UI/T_PortadaHTML.png`.
- **`WBP_Menu`**
  - Los 3 botones pasan del recuadro central a la izquierda, bajo el título, alineados a la izquierda.
  - La lluvia del menú baja a 0,12 de opacidad.

## [0.3.19] — 2026-11-10 · Manchas en muros del parque y telas reales (build de prueba)

### Agregado
- **32 decals en paredes del parque** (`DEC_Muro_Parque*`, carpeta `Parque/Decals`).
  - Hay grafitis, óxido chorreado, filtraciones, grunge y concreto dañado.
  - Se colocaron con trazados laterales desde la avenida (y −4.700 a −12.700), con alturas, tamaños y giros variados.

### Cambiado
- **`MI_Bata` y `MI_Colchon`** pasaron de `BasicShapeMaterial` a `M_Triplanar`, con tela de Horror_Hospital (funda y colcha sucias). Se conservó el color original.
- `MI_Charco` (agua brillante) y `MI_Hoja` (hoja verde) se dejaron planos a propósito.

## [0.3.18] — 2026-11-10 · Decals en el parque y silueta con cuerpo real (build de prueba)

### Agregado
- **47 decals en el suelo del parque** (carpeta `Parque/Decals`, `DEC_Parque*`).
  - Usan suciedad, grietas, daño de concreto, filtraciones, hundimientos y grunge de Horror_Hospital.
  - Están repartidos en una grilla irregular, desde la entrada hasta y −12.700.
  - Se colocaron con trazado al suelo, saltándose los techos y las atracciones.
- **`MI_Silueta`**: material negro propio para la silueta, copiado de `MI_Oscuro`.

### Cambiado
- **`BP_SustoEnergia`** (la silueta del sombrero en la entrada) tiene un cuerpo real.
  - Nuevo componente `Cuerpo`: SKM_Manny en negro, escala 2,75, con respiración lenta (MM_Idle a 0,6).
  - Las esferas del torso, los brazos, las piernas y la cabeza quedan ocultas en el juego. El sombrero, el resplandor y la luz roja siguen igual.
- **`ENT_SustoEnergia`** se volvió a colocar en la misma posición, para que tome el componente nuevo.

## [0.3.17] — 2026-11-10 · Materiales con suciedad (build de prueba)

### Cambiado
- **`M_Triplanar`**, base de 17 materiales del parque: se le agregó una capa de suciedad.
  - Usa `WorldAlignedTexture` con `T_Dirt_World_Align_01_A_D` (Horror_Hospital) en dos escalas, 300 y 1700, para que no se note la repetición.
  - Se mezcla con el color base mediante el parámetro `Suciedad` (0,65).
  - La rugosidad sube hacia 0,92, para quitar el brillo plástico.
  - Parámetro nuevo: `EscalaSuciedad`.
- **5 materiales que eran color plano** (`BasicShapeMaterial`) pasan a `M_Triplanar` con texturas reales:
  - `MI_Concreto` → `T_Soul_Concrete_Generic01`
  - `MI_Escombro` → `T_Cave_Floor`
  - `MI_Azulejo` y `MI_Marco_Azul` → `T_PaintPeelOff`
  - `MI_Oscuro` → `T_Slums_Wall`

## [0.3.16] — 2026-11-10 · Volumen general funcional (build de prueba)

### Corregido
- Ningún sonido queda sin Sound Class: 267 assets (SoundWave y SoundCue) recibieron una.
  - Loops, viento, lluvia, truenos y metal → `SC_Ambiente`.
  - El resto → `SC_SFX`.
- `DefaultEngine.ini`: `[/Script/Engine.AudioSettings] DefaultSoundClassName=SC_SFX`, por si aparece un sonido nuevo sin clase.
- Ahora el deslizador de Volumen general (override de Master con hijos) controla todo el audio.
- Nota: los pasos pasan a sonar con `SC_SFX` (1,25) y los truenos con `SC_Ambiente` (0,6).

## [0.3.15] — 2026-11-10 · Encontrado el ruido de "TV sin señal" (build de prueba)

### Corregido
- **Causa del ruido:** los sonidos de lluvia `Rain_Heavy_Ext01` (SoulCity, que usa `Rain_Heavy_Ext_Cue` en el personaje) y `SFX_Lluvia`. Se encontró por descarte:
  - 0.3.13: todo mudo menos pasos y truenos → sin ruido.
  - 0.3.14: mudos solo 17 ruidos blancos → sin ruido.
  - 0.3.15: mudas solo las 2 lluvias → sin ruido.
- Las dos lluvias quedan con volumen de asset 0. La lluvia se sigue viendo (`NS_Lluvia`) y los truenos se mantienen.
- Los demás sonidos volvieron a su volumen original (respaldo en `herramientas/volumenes_originales.json`).

### Cambiado (0.3.12)
- `t.MaxFPS=30` en `DefaultEngine.ini`: es la mejora aprobada "30 fps iGPU".
- Se devolvieron las respiraciones, `SFX_VHS`, `SFX_T_Glitch`, `MUS_Persecucion` y la lluvia exterior a 0,25, porque no eran la causa.

### Pendiente
- El deslizador de Volumen general no afecta a los sonidos sin Sound Class (pasos, truenos, lluvia y SoulCity). Hay que asignarles `SC_SFX` o `SC_Ambiente`.
- Si se quiere volver a escuchar lluvia, buscar una lluvia más suave y limpia.

## [0.3.11] — 2026-11-10 · Música aislada (build de diagnóstico)

### Cambiado
- `MUS_Menu` y `MUS_Persecucion` con volumen de asset 0. Así queda igual que con el deslizador de Música en 0, que es cuando desaparece la estática.
- Nueva `MUS_Ambiente_Terror` (copia de la pista compuesta, volumen 0,5, loop), que suena como **una sola instancia 2D** en `AmbientSound_2` (sin espacialización, volumen 0,4).
- Diagnóstico: si la estática vuelve, el problema está en la pista; si no vuelve, estaba en cómo se disparaban las músicas viejas.

## [0.3.10] — 2026-11-10 · Música de terror nueva (build de prueba)

### Cambiado
- **`MUS_Menu`:** ahora usa `Musica_Terror_Menu.wav` (92 s en loop, volumen 0,6, sin ruido), generada con `herramientas/componer_musica_terror.py`. Tiene un dron grave disonante, un sub-grave, un acorde frío que respira, una cajita musical de feria desafinada con eco y un latido lejano. El CTO identificó la música anterior como el ruido de "TV sin señal" (al bajarla a 0, desapareció).
- **`AmbientSound_2`** (la música en el parque): volumen de 0,55 a 0,3.

## [0.3.9] — 2026-11-09 · Cortes de audio (build de prueba)

### Corregido
- **Cortes de audio que sonaban a estática** en la portada y el parque. En `DefaultEngine.ini` se cambió `AudioNumBuffersToEnqueue` de 1 a 4 y `AudioCallbackBufferFrameSize` de 1024 a 2048. También quedan `AudioMaxChannels=32` y `AudioNumSourceWorkers=2`, para que el CPU tenga margen.

## [0.3.8] — 2026-11-09 · Respiraciones silenciadas (build de prueba)

### Quitado
- **`SFX_Respiracion` y `SFX_T_Respiracion`:** volumen del asset en 0. Según el CTO, sonaban a estática de TV. La mecánica de contener la respiración sigue activa, pero sin sonido; está pendiente reemplazarla por una respiración grabada.

## [0.3.7] — 2026-11-09 · Sin estática a ratos (build de prueba)

### Quitado
- **`SFX_T_Glitch`** (estática de cinta dañada, 1,5 s, que sonaba a ratos en la portada y con el glitch): el volumen del asset pasa a 0. Por eso el susto del glitch queda sin sonido hasta reemplazarlo.

## [0.3.6] — 2026-11-09 · Sin estática de TV (build de prueba)

### Quitado
- **`SFX_VHS`** (estática VHS de 1,5 s que se repetía en la portada y en el juego): el volumen del asset pasa a 0. Lo confirmó el CTO; era el ruido de "TV sin señal".

## [0.3.5] — 2026-11-09 · Thriller y sin ruido de estática (build de prueba)

### Cambiado
- **Color estilo thriller** (antes era un look de película "bonito"):
  - Saturación 0,55; sombras verde-azuladas y altas luces amarillo pálido.
  - Tinte verdoso, FilmToe 0,68, viñeta 0,75 y bloom 0,22.
- **Luces:** hospital en fluorescente frío (4300 K) y parque en sodio pálido (3100 K).

### Quitado
- **Estática del hospital:** los 3 `AmbientSound` con `AMB_T_Hospital` (ruido generado) quedan apagados (`bAutoActivate` false, volumen 0).
- **Siseo de lluvia en el parque:** en `ActualizarLluvia`, el volumen exterior pasa de 0,25 a 0. La lluvia se sigue viendo y los truenos se mantienen.

## [0.3.3] — 2026-11-09 · Look de película (post-proceso + luces, build de prueba)

### Cambiado
- **Post-proceso (`PostProcessVolume_0`):**
  - Saturación 0,72.
  - Sombras frías y desaturadas, y altas luces ámbar.
  - Contraste 1,14; FilmToe 0,62 (negros más profundos).
  - Grano 0,26, viñeta 0,62 y aberración de color 0,35.
  - Bloom 0,6, nitidez 0,35 y AO 0,7.
  - Exposición fija sin cambios.
- **Luces:** 45 luces blancas pasan a temperatura de ampolleta. Las `LuzParque` quedan en 2600 K, los focos del teatro en 3200 K y el resto en 2900 K. No se tocaron las luces que colorea `BP_Interruptor` ni los rellenos altos.
- **SkyLight:** 0,22 → 0,16, para tener negros reales.

## [0.3.2] — 2026-11-09 · Sprint 2: escondites y respiración (build de prueba)

### Corregido
- **Escondites (`BP_Hiding_Door`):** al entrar o salir, el jugador se teletransportaba con el centro de la cápsula a la altura de los pies del marcador (z = 4 cm), y quedaba medio enterrado y atascado. Ahora se suma +100 cm en Z y `bTeleport = true`. Los 3 clósets (`ESC_Armario_*`) volvieron a `Hospital/Escondites`.

### Añadido
- **Contener la respiración** (función `Respiracion`, se llama cada tick después de `EventoCasa`; variable `Jadeo`):
  - Escondida + mantener **Correr (L3 / Shift)**: gasta aliento a 25/s.
  - Si el aliento llega a 0: jadeo con `SFX_Respiracion`, mensaje y **ruido de 15 m durante 3 s** (`RuidoPos`/`RuidoT`/`RadioRuido`), al que los enemigos acuden.
  - Escondida respirando normal con un `BP_Enemigo` a menos de 7 m: ruido suave de 7 m (te escucha).
  - El mensaje al esconderse explica el control.
- **Versionado del proyecto Unreal:** rama `unreal` con Git LFS (`herramientas/Versionar_Unreal.bat`).

## [0.3.1] — 2026-11-09 · Demo corta para publicar el 13 de noviembre (candidata)

- Es la demo corta (final frente al castillo) con todo lo corregido después de la v0.3.0:
  - entrada sin torniquetes duplicados;
  - modo Calidad Baja más liviano (v0.4.1);
  - calidad Baja por defecto.
- **Corregido el bloqueo real de la entrada:** las mallas `SM_Ent_Reja` (barrotes de 15 m) y `SM_Ent_Hierro` (arco) tenían una colisión simple en forma de caja que tapaba el hueco central. El rayo de visión pasaba, pero el personaje no. Se quitó su colisión, como en el diseño original (`sin_colision.py`). Los costados siguen cerrados por `ENT_BloqueoI/D`.
- **Interruptores de la entrada, corregidos:**
  - `UsarInterruptor` ahora se llama **primero** al presionar E (`EventGraph`, antes de `Interactuar`). Antes estaba al final de la cadena de más de 200 nodos de `InteractuarZonas`, donde se perdía.
  - Se quitó el cortocircuito del 4.º interruptor. Los 4 se encienden **en cualquier orden** y con un solo intento cada uno.
  - El radio pasó de 2,3 m a 3 m.
  - **Sombra bajo el letrero (`BP_SustoEnergia`):** ahora aparece **al llegar a la entrada desde el pasillo** (jugador con Y < -4200, después del glitch) como primer susto del parque. Antes requería energía y además cortaba la luz con un segundo cortocircuito. Ya no corta la energía: los interruptores encienden las luces de forma definitiva.
  - **Audio de terror priorizado:**
    - Lluvia del jugador (`SonidoLluvia`, Rain_Heavy de SoulCity): filtro pasa-bajos a 2,8 kHz (quita el siseo de "radio") y volumen en el parque de 0,55 a 0,25.
    - `SC_Ambiente` al 60 %, `SC_SFX` al 125 %.
    - `SND_Ambiente_Parque` y `SND_VientoParque` a la mitad.
    - Pasos (`PasoSuelo`) de 0,5 a 0,8.
  - **Lluvia visual** (`NS_Lluvia`): 2.600 → 1.400 gotas/s; color más oscuro y transparente (0,30 / 0,36 / 0,45, alfa 0,35).
  - **Salto con ✕ (mando), blindado:** antes de `Jump` ahora llama a `UnCrouch` y deja `Agachada = false`, así que si el personaje quedó agachado, se levanta y salta. Además, ✕ solo confirma en el menú principal si `WidgetMenu` está en pantalla **y visible** (evita un menú "fantasma").
  - **Mando:** el botón de interactuar (`Gamepad_FaceButton_Left`) solo llamaba a `Interactuar`, así que nunca revisaba los interruptores ni llamaba a `RecogerObjeto` / `InteractuarZonas`. Ahora usa la misma cadena que la E.
- **Cinemática del parque:** ahora se activa **al volver la energía** (4 interruptores, `Energia`) y no al cruzar el portón. Dura 12 s (antes 5). `CAM_CineParque` va enganchada a `CINE_Pivote` (un `BP_Giratorio` invisible en el centro del parque, a 22°/s), así que la cámara **orbita el parque** a 75 m de radio y 31 m de altura mostrando las atracciones iluminadas.
- Nueva variable `DemoCorta` (bool) en el jugador: si está activa, `RevisarFinDemo` usa el final del castillo (Y < -11450); si no, el final es la huida de la Casa.
- **Estado del proyecto:** `DemoCorta = true` en el CDO. Para volver a generar la demo completa hay que ponerla en `false` y `ProjectVersion=0.4.x`.

## [0.4.1] — 2026-11-09 · Rendimiento

### Cambiado
- **El modo Calidad Baja ahora es más agresivo** (`AplicarOpciones`, solo si `Calidad == 0`):
  - resolución interna al 55 %;
  - `r.ViewDistanceScale 0.6`;
  - sin niebla volumétrica ni desenfoque de movimiento;
  - follaje al 50 %;
  - sombras a 512;
  - sin VSM ni Lumen (difusa y reflejos).

  Al pasar a Media o Alta se restauran los valores (distancia, niebla, blur, follaje y sombras).
- Bosques: distancia de corte de 70–95 m en las plantillas de los 7 `BP_Bosque_*`. Las instancias del mapa conservan su valor guardado de 120 m.
- Se corrigió el clímax de la Casa (radio de 16 m).

## [0.4.0] — 2026-11-09 · Demo completa (build de prueba, sin validar por el CTO)

### Añadido
- **Evento del Carrusel** (`EventoCarrusel` en el jugador, se llama cada tick después de `RevisarFinDemo`; variable `FaseCarrusel`):
  1. A menos de 13 m del carrusel, la música de feria (tag `CarruselSonido`) se detiene, el carrusel y sus luces (tag `CarruselGira`) empiezan a girar y aparece el mensaje "La música del carrusel se apagó... y empezó a girar solo."
  2. A menos de 4 m del centro, el carrusel se detiene en seco, la muñeca (tag `Muneca`) aparece entre el carrusel y la avenida (-1400, -7600) y se ejecuta `MostrarSusto`.
- **Clímax de la Casa de Alfredo** (`EventoCasa`, variable `FaseCasa`, se llama después de `EventoCarrusel`): al obtener el fusible de la caja fuerte (`TieneFusible`), se corta la linterna (`ForzarOscuridad`), aparece el mensaje "Se cortó la luz. Afuera, algo se ríe… ¡Vuelve a la entrada del parque!", el payaso (tag `Payaso`) aparece frente a la casa (8200, -14300) con un chillido posicional, y empieza la huida.
- **Nuevo final de la demo completa:** después del clímax, llegar a la entrada del parque (Y > -5300, |X| < 900) muestra la pantalla final.

### Cambiado
- El final frente al castillo (`RevisarFinDemo`) queda desactivado para v0.4.0 (umbral -99999). La demo corta v0.3.0 ya publicada no cambia.
- Se abrió la bodega (`BLQ_Bodega` a `Parque/Demo/Sobrantes`), porque la llave de la casa está ahí.

### Corregido (después del build 0.4.0, entra en la próxima versión)
- El clímax de la Casa solo se dispara **dentro de un radio de 16 m de la casa**. Así no se repite al cargar una partida guardada en otro lugar.

### Añadido (cont.)
- Mezcla propia: `SC_SFX` y `SC_Ambiente` (`MundoMagico/Audio/Mezcla`). El volumen general ahora aplica a las clases hijas.
- Diseño del tramo de la demo completa: `auditoria/diseno_sprint3_demo.md`.

## [0.3.0] — 2026-11-09 · Demo corta (objetivo de publicación: 13 de noviembre)

### Añadido
- **Fin de la demo:** la nueva función `RevisarFinDemo` del jugador, que corre cada tick después de `RevisarSalida`, marca `Terminado` al llegar al final de la avenida frente al castillo (|X| < 700, Y < -11450). Así se activa la pantalla final existente: "CAPÍTULO 1 · COMPLETADO… Continúa en el Capítulo 2".
- **Bloqueos de demo** (`Parque/Demo/Bloqueos`): una reja y un cono en el camino al tren, la torre y la montaña, y en las puertas del castillo, el teatro y la bodega.
- Checklist de prueba de humo: `auditoria/checklist_humo.md`.

### Corregido
- **No se podía entrar al parque:** 4 torniquetes duplicados (`ENT_TorniqueteD3` a `D6`) tapaban el centro del portón. Se movieron bajo el suelo a `Parque/Demo/Sobrantes`. Ahora hay 5,2 m de paso libre, verificado con trazos de colisión.

### Cambiado
- **Calidad Baja por defecto** en el primer arranque (valor por defecto de `Calidad` en el CDO = 0). El jugador puede subirla en Configuración.
- Plan acelerado a sprints de 1 semana (`PLAN_DE_TRABAJO.md`).

## [0.2.0] — 2026-11-09 (Sprint 1: Cimientos de Tensión e Interacción)

### Corregido
- La camilla `HX_Camilla3` encajonaba al jugador junto a la cama inicial al ir por la linterna. Se movió a la esquina sureste de la sala (330, 2620). Lo detectó la prueba de humo del build.
- `BP_ThirdPersonCharacter.AngelPayaso`: acceso a `Payasos[4]` con un array de 1 elemento. Ahora usa el índice 0.
- `BP_ThirdPersonCharacter.OcultarPagina`: `Accessed None` sobre `WidgetPagina`. Ahora valida con `IsValid` antes de `RemoveFromParent`.
- `Vigilar` y `RevisarFinal`: `SetTimer` con tiempo 0 se usaba para cancelar los timers de `MostrarSusto` y `MostrarFinal`. Ahora un `Branch` decide entre `SetTimer` y `ClearTimerbyFunctionName`.
- `MostrarFinal`: timer de `AbrirPanel` de 0,01 a 0,05 s.

### Añadido
- Parque: 69 props de Fab en `Parque/Fab2`:
  - 34 rocas en senderos y cerros.
  - 20 faroles apagados en senderos secundarios.
  - 4 columnas en el teatro y el castillo.
  - Señalética de metro en la avenida.
  - Herramientas afuera de la bodega.
- Parque vivo:
  - Lluvia, viento y ruidos metálicos al azar (`BP_RuidoAzar`).
  - Nieblas locales, luces que parpadean y partículas.
  - Torniquetes, boleterías y máquinas expendedoras.
- Herramientas:
  - `Abrir_MundoMagico_Liviano.bat`: abre Unreal sin RayTracing, VSM ni Lumen, solo mientras está abierto.
  - `Respaldar_Proyecto.bat`: zip con fecha de lo propio del proyecto. Conserva los últimos 10.
- Documentación del estudio: `BOARD.md`, `docs/historico/AUDIT_REPORT.md`, `auditoria/*.md`.

### Cambiado
- `DefaultGame.ini`: `ProjectName`, `CompanyName` y `ProjectVersion=0.2.0`.
- Render para iGPU:
  - SkyAtmosphere y VolumetricCloud ocultos.
  - 29 luces decorativas sin sombras.
  - Niebla volumétrica con rejilla 16/64.
- Audio 3D:
  - Atenuaciones `ATT_Corto`, `ATT_Medio` y `ATT_Largo` en los 12 `SND_*` y 7 `SFX`.
  - Sustos del pasillo posicionales (`PlaySoundAtLocation`).
- Fuentes: respaldo `DroidSansFallback` para ▲ ✕ ↑ ↓ ← →.
- Iluminación por zonas:
  - Avenida en penumbra.
  - Carrusel y Noria menos intensos.
- Oso Willy y León movidos a los laterales del parque.
- La botella emite `MakeNoise` al romperse.
- La noria gira sin colisión física; corrige el aviso de escala 0.
- Renombres:
  - `SM_SM_*` → `SM_*`.
  - `Ambiente_*` → `AMB_*`.
  - `Musica_*` → `MUS_*`.
- 7 packs de Fab sin uso (6 GB) archivados fuera de `Content/`.
- Nuevo `Construir_Version.bat`: sincroniza a `C:\MM_Build` y genera `Build/v<versión>`.
- Build Shipping v0.2.0: cook con 0 errores y 0 warnings.

### Mantenimiento
- 20 archivos `.tmp` de guardados interrumpidos en `Saved/` enviados a la Papelera.

### Problemas conocidos
- En el PC de desarrollo (Intel Iris Plus, 8 GB) el editor y PIE se cuelgan por GPU (`DXGI_ERROR_DEVICE_HUNG`). Ver `docs/historico/AUDIT_REPORT.md`.

## [0.1.0] — 2026-11-02
- Primer build Shipping Win64, 2,2 GB, 0 errores, generado desde `C:\MM_Build`.
- Incluye:
  - Hospital y pasillo.
  - Parque con atracciones.
  - Enemigo payaso.
  - Guardado y continuación (`BP_Partida`).
  - Mochila y pilas manuales.
  - Cinemáticas.
  - Menú con opciones.
