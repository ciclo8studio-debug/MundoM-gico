# Changelog — BLACKLINE: NIGHT HAUL

## 0.1.0-dev (en curso)
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
