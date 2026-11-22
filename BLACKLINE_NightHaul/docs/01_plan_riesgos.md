# Plan y riesgos — BLACKLINE: Night Haul (vertical slice)

## Hitos del vertical slice
| Hito | Entregable | Criterio |
|---|---|---|
| M0 Esqueleto | Módulo `Blackline`, Build.cs, `BlacklineTypes.h`, LogBlackline, compila | Compila limpio en UE 5.8.4 |
| M1 Conducción | `ABlacklineVanPawn` + ruedas Chaos + Enhanced Input + 2 cámaras | Una van maneja en un circuito de prueba |
| M2 Terreno | `ATerrainModifierVolume` (asfalto mojado, barro, grava) + 4WD/DiffLock | El barro atasca en RWD y pasa en 4WD |
| M3 Carga | Data assets + `UCargoSimulationComponent` (CoM, integridad, G lateral) | La carga altera el manejo y se daña con impactos |
| M4 Control | `ACheckpointController` + manifiestos + `FSuspicionSnapshot` | Los 3 veredictos se disparan según la carga y el estado de la van |
| M5 Persecución | `UPursuitDirectorSubsystem` (Hunting/Searching/Lost) | Un fallo en el control inicia la persecución y se puede escapar |
| M6 Tablero | Botones con canal Interact y tags `Dash.*` | Los 6 controles funcionan en cabina |
| M7 Cierre | Ruta jugable de un contrato, perfilado, build empaquetada | Cumple la definición de terminado |

## Orden de implementación de clases
1. `BlacklineTypes.h` (enums y structs); `Blackline.h/.cpp` (log).
2. `UCargoItemDataAsset`, `UManifestDataAsset` (solo datos).
3. `UBlacklineWheelFront/Rear`.
4. `ABlacklineVanPawn` (movimiento, input, cámaras, modo de tracción), con stubs para Cargo y suspicion.
5. `ATerrainModifierVolume` + `Push/PopSurfaceModifier`.
6. `UCargoSimulationComponent` (se integra a la van; `SetCenterOfMass`).
7. `UPursuitDirectorSubsystem` (basado en timers).
8. `ACheckpointController` (depende de van y persecución).
9. Tablero: `ActivateDashControl`, traza Interact, `OpenWindowForInspection`.
10. Integración: `GetSuspicionSnapshot` completo, mapa de prueba, tuning.

## Riesgos principales
| # | Riesgo | Prob. | Impacto | Mitigación |
|---|---|---|---|---|
| 1 | Cambios de API de ChaosVehicles en 5.8.4 (`UChaosVehicleWheel`, `AWheeledVehiclePawn`) | Alta | Alto | Consultar `docs/engine-reference/unreal/`; marcar `// VERIFY-5.8`; compilar en M0 |
| 2 | Inestabilidad física del vehículo (vuelcos, jitter, suspensión) | Alta | Alto | Valores por defecto conservadores en `EditAnywhere`; CoM bajo; probar en circuito antes de agregar carga |
| 3 | `SetCenterOfMass` dinámico se ve desfasado con la carga | Media | Alto | Limitar la actualización a 20 Hz; suavizar el cambio; clamp de offsets |
| 4 | Fricción por superficie (barro, mojado) no se aplica por rueda en Chaos | Media | Alto | Probar `FrictionForceMultiplier` por rueda; plan B: escalar fuerzas o el freno de mano |
| 5 | Rendimiento en física + Niagara + IA con la persecución | Media | Alto | Sin Tick en actores; timers; presupuestos con `stat unit`; LOD de Niagara |
| 6 | Enhanced Input: asset IMC/IA mal enlazados o nulos | Media | Medio | Validación con `ensure` y log al `BeginPlay`; fallback con bindings mínimos |
| 7 | Canal `ECC_GameTraceChannel1` no configurado en `DefaultEngine.ini` | Media | Medio | Definir el canal "Interact" en config desde M0; test de la traza |
| 8 | El balance de sospecha (veredictos) se siente injusto o ilegible | Media | Medio | Fórmula en `EditAnywhere`; log de factores; iterar con el snapshot |
| 9 | Persecución: la IA se pega al jugador o lo pierde sin motivo | Media | Medio | Máquina de estados con timers; tuning de visibilidad y calor; modo debug |
| 10 | Dependencias circulares entre Van, Cargo, Checkpoint y Pursuit | Media | Medio | Respetar el contrato; declaraciones adelantadas; orden de implementación; includes mínimos |

## Definición de terminado
- Compila sin errores ni warnings nuevos en UE 5.8.4 y se empaqueta.
- Todas las clases del `CONTRATO.md` existen con sus firmas públicas intactas.
- Un contrato recorrible de punta a punta: carga, ruta, control, veredicto y persecución.
- Sin Tick en actores (salvo Cargo a 20 Hz); sin coordenadas escritas a mano.
- Los `VERIFY-5.8` están resueltos o documentados.
- Rendimiento: objetivo de fotogramas medido en la máquina de destino y registrado.
- Tuning expuesto vía `EditAnywhere`; sin crashes en 30 min de juego.
