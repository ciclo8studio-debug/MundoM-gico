# 21 — Prototipo de 1 día: validar la sensación de manejo

**Hipótesis:** el furgón 4x4 con carga desplazable se siente pesado y ágil a la vez, y el retén genera tensión. Si no, el resto del slice no vale la pena.
**Contenido mínimo:** mapa plano grande (asfalto + parche de barro), `BP_BlacklineVan`, 1 caja (`DA_Cargo` frágil, 120 kg), 1 `ACheckpointController`, 1 `ATerrainModifierVolume` (Mud). Sin Lumen High, sin audio final, sin patrullas (solo `StartPursuit` con log).
**Objetivo de rendimiento:** 60 FPS desde el día 1 (si baja de 50 en mapa vacío, parar y revisar).

## Orden (≈8 h)
| # | Prueba | Tiempo | Medir | Pasa si |
|---|---|---|---|---|
| 1 | Compila el módulo y el furgón maneja en plano | 1.5 h | Compila; ruedas giran | Sin crash, dirección responde |
| 2 | Ajuste de feel: torque, suspensión, masa | 1.5 h | Aceleración 0-60 km/h, frenado, vuelco a 60 km/h en giro | 0-60 en 6-9 s, no vuelca sin intención |
| 3 | Carga: 1 caja a 120 kg, luego 600 kg | 1 h | Desplazamiento del centro de masa, G lateral, integridad | Se nota la inercia; integridad baja al girar brusco |
| 4 | Cámara 1ª/3ª y tablero por LineTrace | 1 h | Toggle sin salto; 6 botones responden | Todos los `Dash.*` activan |
| 5 | Barro 2WD vs 4WD | 1 h | Tiempo para cruzar 40 m | 2WD se atasca o tarda ≥2x; 4WD cruza |
| 6 | Retén: parar, ventanilla, veredicto | 1 h | Tres veredictos alcanzables con tuning | Se puede forzar CleanPass, Secundaria y Alerta |
| 7 | FPS y memoria | 0.5 h | `stat unit`, `stat fps` | ≥58 FPS medios, sin picos >25 ms |

## Criterios de pivote
- **Pivotar el manejo** si el furgón no es divertido tras 2 h de ajuste (prueba 2): simplificar a modelo arcade y mantener solo el desplazamiento de carga como efecto visual.
- **Recortar la carga dinámica** si afecta la estabilidad de la simulación (jitter, explosiones físicas).
- **Simplificar el retén** a umbral único si los tres veredictos no se distinguen para un jugador nuevo.
- **Parar y bajar calidad** si el mapa vacío ya está bajo 50 FPS.

Umbrales numéricos son estimados; calibrar con la primera ronda de pruebas.
