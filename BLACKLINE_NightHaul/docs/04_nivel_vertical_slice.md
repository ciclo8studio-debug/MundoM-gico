# 04 — Nivel Vertical Slice: "Ruta 9 / Muelle Siete" (greybox)

**Tema:** noche, lluvia, furgón con carga ilegal. Tensión por inspección, luego huida.
**Duración estimada:** 12 min (ruta limpia 4-5 min + retén 1-2 min + escape 5-6 min).
**Unidades:** metros (1 m = 100 uu). Todo valor es `EditAnywhere` (sin coordenadas mágicas); las cifras de abajo son defaults de diseño.
**Contrato:** `CONTRATO.md` (`ATerrainModifierVolume`, `ACheckpointController`, `UPursuitDirectorSubsystem`, `ESurfaceKind`).

## 1. Esquema ASCII (norte arriba, ~2400 m x 1400 m)

```
 N
 ^                                                  [F] Bosque (Forest)
 |   (0,0)=SPAWN                                   .-~~~~~~~~~~~~~~~~.
 |                                                /  T3  Forest      \
 |   S-----A1-----A2--+                         /   950 m, 4WD opc.   \
 |   | Asfalto lluvioso 1100 m                 /                       \
 |   |  (WetAsphalt)  curva-S                 /                         \
 |   +--------A3--------+                    /                           \
 |                     |                   /                             \
 |            [CP] RETEN PORTUARIO ========X1 (bifurcacion 1, +60 m)       \
 |            Barrera+InspectionZone        |                               \
 |            (km 1.1, 0.6 km a muelle)     +---T2 Barro (Mud) 700 m, 4WD ---> [E2]
 |                     |                    |        (StuckSinkDepth)
 |                     |                    |
 |                     +---------X2---------+---T1 Industrial (Gravel) 800 m ---> [E1]
 |                                 (patio contenedores, patrulla fija)
 |
 +--> E (Este)
 Leyenda: S spawn | A* tramos de lluvia | CP reten | X* bifurcaciones | E* salida (meta) | P* patrulla
```

Vista del reten (zoom, 120 m x 60 m):

```
   aprox. 150 m (cartel luminoso, baja velocidad)
        |
   [conos]--[conos]       <- carril unico, 6 m ancho
   ==[INSPECTION ZONE 14x6x4 m]==[BARRERA]==> salida escape
        ^ luz spot (InspectionLight)   garita (Cover) a la izq.
        P1 (guardia) + P2 (vehiculo estacionado, se activa en PursuitAlert)
```

## 2. Ruta critica

| Tramo | Superficie | Largo | Notas |
|---|---|---|---|
| S -> A3 (asfalto lluvioso) | WetAsphalt | 1100 m | 3 rectas + curva-S; aprendizaje de carga/lateral G |
| A3 -> CP (aproximacion) | WetAsphalt | 150 m | Estado `Approaching`; frenar, apagar luces opcional |
| CP (inspeccion) | Asfalto | 30 m | `Inspecting`, ventana abierta; veredicto |
| CP -> X1 | Asfalto | 60 m | Si `CleanPass`: fin del slice (rama limpia, ~+300 m a muelle) |
| X1 -> E (3 rutas de escape) | ver 3 | 700-950 m | Solo con `PursuitAlert` / `SecondaryInspection` fallida |

Total ruta limpia ~1.3 km; ruta con persecucion ~2.0-2.2 km.

## 3. Rutas opcionales (escape)

| Ruta | Superficie | Largo | Tiempo (a 40-60 km/h) | Riesgo/recompensa |
|---|---|---|---|---|
| T1 Industrial | Gravel | 800 m | ~55 s | Rapida, patrulla fija, esquinas ciegas |
| T2 Barro | Mud | 700 m | ~70 s (4WD) / atasco (RWD) | Requiere `FourWheel`; sin patrulla; riesgo de atasco |
| T3 Bosque | Forest | 950 m | ~90 s | Mejor para perder al perseguidor (`Searching`), danos a carga fragil |

## 4. ATerrainModifierVolume (ubicacion y parametros)

Origen (0,0) = spawn; eje +X = este. Offsets relativos al nivel.

| ID | Surface | Centro (m) | Extension (m, X x Y x Z) | Friccion | bRequires4WD | StuckSinkDepthCm |
|---|---|---|---|---|---|---|
| TM_Asf_01 | WetAsphalt | (550, 0) | 1100 x 12 x 4 | 0.70 | no | 0 |
| TM_Asf_Curva | WetAsphalt | (900, 40) | 120 x 14 x 4 | 0.60 | no | 0 |
| TM_Reten | Asphalt | (1175, 0) | 150 x 12 x 4 | 0.95 | no | 0 |
| TM_Indus_T1 | Gravel | (1700, -300) | 800 x 10 x 4 | 0.75 | no | 0 |
| TM_Barro_T2 | Mud | (1650, 0) | 700 x 16 x 4 | 0.40 | si | 25 |
| TM_Bosque_T3 | Forest | (1700, 380) | 950 x 18 x 4 | 0.55 | no (opcional) | 8 |

Reglas: volumenes no se solapan salvo 10 m de transicion en bifurcaciones (X1, X2); un pop siempre sigue a un push (`Push/PopSurfaceModifier`). Barro con RWD: hundimiento 25 cm y friccion 0.40 = atasco probable (cebo de decision).

## 5. Patrullas y retén (rutas, sightlines, zonas de presion; el comportamiento lo implementa ai-programmer)

| ID | Tipo | Posicion / ruta | Rango vision | Activacion |
|---|---|---|---|---|
| CP_Main | `ACheckpointController` | (1150, 0), InspectionZone 14x6x4 m | - | Aproximacion a 150 m |
| P1 Guardia | Fija | Garita en (1150, 8), mira al carril | 40 m | `Inspecting` |
| P2 Patrulla reten | Vehiculo | Estacionado en (1180, -10) | 80 m (faros) | `PursuitAlert`; persigue hacia X1 |
| P3 Patrulla industrial | Patrulla en bucle | Patio (1650, -300), bucle de 200 m | 60 m | Siempre activa en T1; cambia a `Hunting` al verte |
| P4 Helicoptero/dron (opcional) | Barrido | Sobre T3, radio 150 m | 120 m (haz) | Solo si calor `GetHeat01() > 0.7` |

Nota: T2 sin patrulla (castiga con atasco, no con IA). Jammer (`SignalJammer`) reduce deteccion de P1; luces apagadas reducen vision de P3/P4.

## 6. Ritmo (minutos)

| Min | Intensidad (0-10) | Segmento | Beat |
|---|---|---|---|
| 0:00-0:45 | 2 | Spawn, tutorial de tablero | Encender motor, luces, limpiaparabrisas |
| 0:45-3:00 | 3-4 | Asfalto lluvioso | Aprender frenada con carga; sube lateral G en curva-S |
| 3:00-4:30 | 5 | Aproximacion al reten | Luces del puerto, cola; decision: jammer/luces |
| 4:30-6:00 | 8 | Inspeccion | Pico de tension, sin accion de conduccion |
| 6:00-6:20 | 9 (si falla) | PursuitAlert | Sirenas, X1 en 60 m: elegir ruta |
| 6:20-9:00 | 8 -> 6 | Escape T1/T2/T3 | Persecucion, 90 s por ruta |
| 9:00-10:30 | 4 | `Searching` / `Lost` | Respiro, carga danada |
| 10:30-12:00 | 2 | Llegada / resultado | Resumen de integridad y creditos |

Clave: un unico pico alto en el reten, valle largo antes, valle corto despues.

## 7. Elementos de guia (leading)

- Luz: halo del puerto en el horizonte; spot del reten; bengalas amarillas en bifurcaciones.
- Geometria: guardarrailes convergentes hacia el reten; carteles de ruta T1/T2/T3 en X1.
- Audio: sirena distante antes del CP; radio del guardia en el reten.

## 8. Narrativa ambiental (a coordinar con narrative-director)

- Camion volcado antes del reten (otro transportista cazado).
- Manifiesto pegado en la garita con el codigo de ruta (`FText RouteCode`).
- Contenedores marcados con el logo de la empresa del manifiesto.

## 9. Audio por tramo

| Momento | Cambio |
|---|---|
| Spawn | Lluvia + motor al ralenti |
| A2 | Lluvia mas fuerte, bajo grave |
| Aproximacion | Se apaga el ambiente; sube el latido |
| `Inspecting` | Silencio relativo; radio del guardia |
| `PursuitAlert` | Sirena + capa de percusion |
| `Searching` | Percusion fuera; ambiente bajo |
| `Lost` | Lluvia limpia y motor |

## 10. Lista de encuentros

| Encuentro | Tipo | Dificultad | Posicion |
|---|---|---|---|
| Curva-S | Conduccion | Baja | (900, 40) |
| Reten | Social/stealth | Alta | (1150, 0) |
| Persecucion X1 | Persecucion | Alta | (1235, 0) |
| Patio T1 | Patrulla | Media | (1650, -300) |
| Barro T2 | Terreno | Media-alta | (1650, 0) |
| Bosque T3 | Sigilo | Media | (1700, 380) |

## 11. Dudas abiertas

- Confirmar con game-designer: duracion objetivo (12 min) y si la rama `CleanPass` cierra el slice.
- La ruta T3 depende de `ESurfaceKind::Forest` y de la regla de daño a carga fragil (sistema de carga).
- Cifras de friccion son punto de partida; afinar en playtest.
