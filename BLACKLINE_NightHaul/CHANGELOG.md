# Changelog — BLACKLINE: NIGHT HAUL

## 0.1.0-dev (en curso)
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
