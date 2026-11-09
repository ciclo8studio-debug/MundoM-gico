# Diseño: Behavior Tree del enemigo (Sprint 1 · tarea 1.14)

**Autor:** `@gameplay-engineer` · **Estado:** diseño, la migración todavía no se hace. Se implementa cuando exista un equipo o un build que permita probarla.

## Situación actual (`BP_Enemigo`)
- Máquina de estados sobre `Event Tick`: `Pensar`, `Fase` (int), `VolverACasa`, `Enfriamiento`, `GolpeAhora`, `InicioBusqueda`, `UltimaRabia`.
- Variantes por flag: `EsPayaso` y `EsLeon`. `ENE_OsoWilly` es la variante base. `ENE_Muneca` usa su propio `BP_Muneca`.
- Percepción: lee directamente del jugador `RuidoPos`, `RuidoT`, `LinternaEncendida`, `Agachada` y `Escondido`. Movimiento con `SimpleMoveToLocation` y visión por `LineTrace`.
- `Casa` se toma de la posición inicial. Desde el Sprint 1, el oso queda en el lateral oeste (-4700, -10300) y el león en el lateral este (7300, -11300).
- El jugador ya emite **`MakeNoise`** al romperse la botella (`SonidoBotella`, alcance 25 m, tag `Botella`), así que hay un estímulo de oído listo para AIPerception.

## Diseño propuesto

### Componentes
- `AIC_Enemigo` (AIController) con **AIPerception**:
  - **Vista:** radio 1500, pérdida a 1800, ángulo 70°. El jugador **agachado** reduce el radio efectivo un 50 % y la **linterna encendida** lo aumenta un 50 % (se aplica con un servicio que ajusta el umbral).
  - **Oído:** radio 2500. Recibe `MakeNoise` (botella, carrera, puertas, generador).
- `BB_Enemigo` (Blackboard): `Objetivo` (Actor), `UltimaPosConocida` (Vector), `PosRuido` (Vector), `Casa` (Vector), `Estado` (enum `E_EstadoEnemigo`: Patrulla, Investigar, Perseguir, Atacar, Volver), `Rabia` (float).

### Árbol (`BT_Enemigo`)
```
Root
└─ Selector
   ├─ [Deco: Objetivo visible] Secuencia PERSEGUIR
   │    ├─ Servicio: actualizar UltimaPosConocida (0.2 s)
   │    ├─ MoveTo Objetivo (radio aceptación 120)
   │    └─ [Deco: distancia < 150 y Enfriamiento listo] Tarea ATACAR (reutiliza BT_Task_Attack_Player)
   ├─ [Deco: PosRuido válido, últimos 8 s] Secuencia INVESTIGAR
   │    ├─ MoveTo PosRuido
   │    ├─ Esperar 2-4 s (mirar alrededor, anim AnimEstado=Buscar)
   │    └─ Limpiar PosRuido
   ├─ [Deco: UltimaPosConocida válida] Secuencia BUSCAR
   │    ├─ MoveTo UltimaPosConocida
   │    └─ EQS: 3 puntos aleatorios a < 600 de distancia → MoveTo cada uno
   └─ Secuencia PATRULLA / VOLVER
        ├─ [Deco: lejos de Casa > 2500] MoveTo Casa
        └─ BT_Task_Wonder (radio 800 alrededor de Casa)
```

### Variantes sin duplicar lógica
- **Payaso:** más velocidad en Perseguir y un sonido de risa al cambiar a Investigar.
- **León:** ignora el oído si el jugador está `Escondido`. Rugido al detectar.
- **Muñeca:** sigue con su BP propio (mecánica de "no mirar"), fuera de este árbol.

## Plan de migración (Sprint 2 o posterior)
1. Crear `BB_Enemigo`, `BT_Enemigo` y `AIC_Enemigo` en `MundoMagico/Blueprints/AI/`, partiendo de `Horror_Template/.../BT_Ai_Enemy` y sus tareas.
2. Agregar a `BP_Enemigo` un bool `UsarBT`, en falso por defecto. Si está activo, `Event Tick` no ejecuta `Pensar` y el AIController es `AIC_Enemigo`.
3. Probar solo con `ENE_OsoWilly` (`UsarBT=true`) en un build empaquetado. Comparar con la FSM actual.
4. Cuando se valide, pasar el resto y borrar la FSM sobre Tick (`Pensar`, `Fase`).
5. Dejar de leer variables del jugador. Todo estímulo llega por percepción (`MakeNoise` y vista).

**Requisito:** NavMesh dinámico (ya existe: `NavMesh_Parque`, `NavMesh_Hospital`). El aviso de `CrowdManager` aparece porque el NavMesh dinámico todavía no se ha construido al iniciar. Es inofensivo con `SimpleMoveToLocation`, pero conviene revisarlo al pasar a `MoveTo` de BT.
