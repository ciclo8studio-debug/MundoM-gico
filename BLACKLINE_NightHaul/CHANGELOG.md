# Changelog — BLACKLINE: NIGHT HAUL

## 0.1.0-dev (en curso)
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
