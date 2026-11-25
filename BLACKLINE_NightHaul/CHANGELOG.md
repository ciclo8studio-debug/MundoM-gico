# Changelog — BLACKLINE: NIGHT HAUL

## 0.1.0-dev (en curso)
### 2026-11-25 — v0.1.25 (corrección del volcado)
- **Causa del furgón atascado tras volcar (confirmada en el log):** el teletransporte del actor no movía el cuerpo físico simulado (z se quedaba en -79 mientras el log decía «repuesto en Z=3350»). Ahora `TeleportVan` mueve el cuerpo físico (`SetAllPhysicsPosition/Rotation`) y lo deja quieto. También lo usa el arresto. Prueba automática: `-teleporttest` manda el furgón al 30% de la ruta a los 8 s y el log muestra la posición final.
- **Caída bajo el mundo:** cada 1 s se revisa si el furgón está 30 m bajo la ruta; si es así, se devuelve a la carretera.
- **Lanzamiento a 300 km/h:** lo causaba el choque con los autos en contra (cajas físicas movidas a saltos por timer). Ahora esos autos no tienen colisión física; el choque lo detecta la ruta: frena el furgón al 25%, -5% de paciencia y mensaje CHOQUE.

### 2026-11-25 — v0.1.23 y v0.1.24
- **Campaña lineal** (v0.1.23, compilada): C01, C09, C15, C18, C21; tras la prueba de vida se viaja al siguiente contrato y su zona (`-zone` / `-contract` la desactivan).
- **Menú de contratos mínimo** (v0.1.24): F5 / F6 elige el contrato anterior / siguiente y recarga la zona. `ShiftCampaign` en el subsistema.
- **Nueva partida:** tras cualquier final (rescate, con costo o malo) y en campaña, a los 25 s se reinicia desde C01 (`ResetCampaign`). Un encargo fallido se repite a los 10 s.
- **Lluvia visible** (`UBlacklineRainComponent`): rayas que acompañan al furgón, solo con clima de lluvia; timer, sin Tick. **Asfalto mojado:** la calzada y la berma se oscurecen según la humedad del clima del contrato.
- **Cabina propia** (`UBlacklineCabinComponent`): tablero, volante, collar rojo colgado del espejo, pelota en el copiloto y foto de Pancito. Formas básicas; se ven con la cámara de cabina (V). Posiciones a ajustar tras verlas.
- **Guardia y barrera con modelo** (`ABlacklineGuardPost`): caseta, guardia con chaleco y barrera roja y blanca; sube con el veredicto de paso limpio.
- **Decisión final:** al llegar al último contrato, 15 s para avisar a la policía (P / Círculo) o entregar a La Cofradía. Nuevo **final de la ley** (dos variantes según la paciencia).
- **Menú de contratos en el HUD:** línea CAMPAÑA con el contrato actual resaltado (F5 / F6).
- **Release:** aviso de ficción y contenido, `docs/25_release.md`, versión del proyecto 0.1.24.
- **Modo ayuda:** H oculta o muestra el recordatorio de controles (decisión 6 del acta).
- El arco 3 usa **C15 Fósil del Desierto** (antes el BOARD decía C12).
### 2026-11-25 — v0.1.21 y v0.1.22
- Barras en el panel: **FURGON** (daño) y **LA COFRADIA - paciencia** (antes la barra del perro). Pancito aparece solo en los mensajes.
- **Audio procedural** (`UBlacklineProceduralAudio`, sin assets): motor segun RPM, lluvia segun el clima, viento segun la velocidad y sirena en persecucion.
- **Arresto:** < 10 km/h con 2+ patrullas a menos de 20 m durante 4 s: pierde la carga, 25% de los viaticos y paciencia; vuelve al inicio; al 3er antecedente, final malo.
- Reunion de los 5 superagentes: `docs/reunion_2026-11-25/ACTA.md`.

### 2026-11-25 — nombres
- El protagonista pasa a llamarse **Enzo** (antes Mateo) y su perro **Pancito** (antes Chuño), por decision del CTO. Las entradas anteriores de este changelog usan los nombres nuevos por consistencia.

### 2026-11-25 (noche) — v0.1.15 a v0.1.20
- Historia nueva (docs/24): Enzo Salinas, su perro Pancito y La Cofradia. Briefings de los 25 contratos, bienestar de Pancito (+10% a tiempo, -15% plazo vencido, -25% carga perdida, -10% al ser descubierto), plazo por encargo, pruebas de vida y finales.
- Suelo: sin triangulos duplicados (manchas negras), desnivel terreno-calzada de 5 cm, faros sin sombras.
- Furgon: curva de torque en RPM reales (Chaos no la normaliza), 800 Nm, llega a ~165 km/h; combustible; recuperacion al volcar (3 s, vuelve al inicio de la ruta; `bRespawnAtRouteStart`).
- Ruta: bencinera y restaurante (E / Circulo), 6 autos en contra, minimapa, reten con manifiesto automatico y veredictos, contrato automatico por zona (`-contract=C09` para forzar uno).
- Patrullas con persecucion propia (sin NavMesh). Teclas: L luces, I motor, J inhibidor, R volver a la carretera (mando: cruceta y Select).

### 2026-11-25
- **Chile comprimido 1:100:** `FChileWaypointRow`, `DT_ChileWaypoints` (26 waypoints de ciudades reales en 4 zonas: Norte, Centro, Sur, Cordillera; ~29 km), `AChileRouteActor` (ruta en spline, calzada con lineas, terreno procedural por bioma) y mapa `L_ChileRoute` (mapa de inicio). Zona con `ZoneId` o `-zone=N`.
- Perfil grafico liviano (DX11, sin Lumen/VSM/RT, sg.*=0, resolucion 50%) aplicado a Blackline.
- Furgon: simulacion de fisica forzada, curva de torque y caja por defecto, motor encendido, camara de tercera persona sin colision del brazo, carroceria y neumaticos visuales, telemetria en el log.
- Input: IMC con modificadores correctos y mapeo de mando PS4 (R2/L2, stick, Cuadrado, Triangulo, Circulo, L1).
- Fix: el furgon caia a traves del suelo por teletransportarlo despues de aparecer; ahora el `PlayerStart` se mueve al inicio de la ruta antes de que aparezca el jugador. Losas de colision de cubos bajo la calzada (1744 en la zona 2).
- **Extras de la ruta:** bencinera y restaurante (`ARoadsidePOI`, E/Circulo), 6 autos en sentido contrario (`ATrafficCar`), minimapa con velocidad, combustible y mensajes (`UBlacklineMinimapWidget`), combustible en el furgon.
- Velocidad: par 800 Nm, marcha final 3.5, cambio 0.2 s (llega a ~165 km/h). Faros sin sombras (sin manchas negras). Material del terreno con brillo minimo propio.
- Scripts: `Rebuild_And_Package.ps1`, `Package_And_Probe.ps1`, `blackline_build_chile_map.py`, `blackline_chile_route.py`.

### 2026-11-22 (noche)
- `UBlacklineQualityGovernor` (subsistema de mundo, sin Tick, timer de 1 s): resolucion adaptativa por FPS medidos, ajusta `r.ScreenPercentage` 60-90 % en pasos de 5 con histeresis (baja al tiro, sube tras 5 mediciones con holgura). Interruptor y limites en Project Settings > Game > Blackline Quality. Compila (Development); sin probar en juego.
### 2026-11-22 (tarde)
- `ABlacklinePatrolCar` (patrulla placeholder con sirena, AIController), `ADeliveryZone` (cierra el contrato al detenerse; bloquea con persecución activa), `AWeatherDirector` (clima por contrato: lluvia, mojado, nieve, niebla), `UContractTabletWidget` (base C++ de la tablet).
- Tests de Automation `Blackline.*` (tabla de 25 contratos, nivel de manifiesto, carga de contrato). Ejecutados: 3/3 pasan (`Automation RunTests Blackline.`).
- `Config/DefaultGame.ini`: 3 niveles de persecución con la patrulla en C++ y ruta de la tabla de contratos.
- Script del mapa: NavMesh, zona de entrega a 1.5 km y director de clima; ahora regenera el mapa aunque el editor lo tenga abierto.

### 2026-11-22
- Contratos: 25 encargos en `DT_BlacklineContracts.csv`, `FBlacklineContractRow`, `UBlacklineContractSubsystem` y ajustes de proyecto.
- Carga: `LoadContract`/`LoadContractFromTable`, evento `OnCargoDestroyed`.
- Retén: nivel de documento exigido por contrato (`ApplyContract`).
- Furgón: `GetEngineRpm`, `GetEngineRpm01`, `GetStealth01`, estado de luces e inhibidor.
- Mapa plano `L_FlatRoad_Chile` (ruta estilo chileno), `BP_BlacklineVan`, inputs y `GM_Blackline` generados por `blackline_build_map.py`.
- Fix: `UpdateDifferentialSetup` no existe en 5.8; ahora `SetDifferentialFrontRearSplit`.
- Fix: `.gitignore` raíz ignoraba todo el C++; excepción añadida en `BLACKLINE_NightHaul/.gitignore`.
- Compila el target `Blackline Win64 Development` y `BlacklineEditor` (hasta el commit del 22-11 antes de los contratos).

### 2026-11-21
- Primera versión del proyecto: contrato de interfaces, módulo C++ (tipos, datos, carga, retén, persecución, terreno, ruedas, furgón, HUD) y 25 documentos de diseño, arte, QA y release por los 31 agentes del estudio.
