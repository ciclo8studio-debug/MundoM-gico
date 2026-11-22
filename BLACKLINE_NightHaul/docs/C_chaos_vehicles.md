# C. Chaos Vehicles: furgón 4x4 (Blackline)

Valores de partida para un furgón de ~3200 kg cargado vacío 2600 kg. Marcados `VERIFY-5.8` los nombres de propiedad que debes confirmar en el editor (ChaosVehicles es Experimental en 5.8, ver `docs/engine-reference/unreal/PLUGINS.md`). Unidades UE: cm, kg, Nm, grados.

## 1. Assets previos
1. Plugin **Chaos Vehicles** activo (ya en Build.cs).
2. Malla esquelética del furgón con huesos: `root`, `body`, `wheel_FL`, `wheel_FR`, `wheel_RL`, `wheel_RR`. Eje X adelante, Z arriba, Y derecha. Ruedas en pivote del centro del eje.
3. Crear `BP_BlacklineVan` (hijo de `ABlacklineVanPawn`), `ABP_Van` (hijo de `UVehicleAnimationInstance`) y `BP_WheelFront`/`BP_WheelRear` (hijos de `UBlacklineWheelFront/Rear`).

## 2. PhysicsAsset (PA_Van)
1. Click derecho en la malla > Create > Physics Asset. Generar con **Primitive Type = Box**, Min Bone Size 20, **Create Body for All Bones: apagado**.
2. Cuerpos:
   - `body`: 1 caja (aprox. 480 x 210 x 190 cm, ajustada al chasis), **Mass 2600 kg** (Override Mass), Linear Damping 0.01, Angular Damping 0.0, Collision Response: Block, Simulation: Simulated.
   - Segunda caja baja opcional para la caja de carga (el centro de masa lo mueve `UCargoSimulationComponent`).
   - Ruedas `wheel_*`: **borrar sus cuerpos** (Chaos las simula con raycast/sweep). Si quedan, ponerlas con Physics Type = Kinematic y colisión Query Only/No Collision.
3. Centro de masa: en el BP, `Center of Mass Override` en el movement component = (0, 0, -25) cm respecto al origen (bajo y algo adelantado: X +10). Evita vuelcos; la carga lo desplaza después.
4. En la malla: Physics Asset = PA_Van, Anim Class = ABP_Van, Collision Preset = Vehicle (Object Type Vehicle, Simulate Physics activado lo hace el pawn).

## 3. Movement component (Chaos Wheeled Vehicle Movement)
### Ruedas (Wheel Setups, 4 entradas)
| Slot | Clase | Bone | Offset adicional |
|---|---|---|---|
| 0 | BP_WheelFront | wheel_FL | 0 |
| 1 | BP_WheelFront | wheel_FR | 0 |
| 2 | BP_WheelRear | wheel_RL | 0 |
| 3 | BP_WheelRear | wheel_RR | 0 |

### UChaosVehicleWheel (defaults ya codificados en C++)
| Propiedad | Delantera | Trasera |
|---|---|---|
| AxleType | Front | Rear |
| WheelRadius | 38 | 38 |
| WheelWidth | 26.5 | 28 |
| WheelMass | 25 | 27 |
| MaxSteerAngle (bAffectedBySteering) | 32 (sí) | 0 (no) |
| bAffectedByEngine | sí | sí |
| bAffectedByBrake | sí | sí |
| bAffectedByHandbrake | no | sí |
| MaxBrakeTorque (Nm) | 4200 | 3000 |
| MaxHandBrakeTorque (Nm) | 0 | 6000 |
| FrictionForceMultiplier | 3.0 | 3.2 |
| CorneringStiffness | 1000 | 1000 |
| SlipThreshold / SkidThreshold | 400 / 400 | 400 / 400 |

Delanteras directrices: ángulo 32 grados ya que radio de giro ~6.5 m con batalla 3.2 m. Si la dirección es nerviosa a alta velocidad, ver Steering Setup (abajo).

### Suspensión
| Propiedad | Delantera | Trasera |
|---|---|---|
| SuspensionAxis | (0,0,-1) | (0,0,-1) |
| SuspensionMaxRaise (cm) | 12 | 12 |
| SuspensionMaxDrop (cm) | 16 | 18 |
| SpringRate | 90 | 110 |
| SpringPreload | 50 | 60 |
| SuspensionDampingRatio | 0.55 | 0.60 |
| SuspensionSmoothing | 8 | 8 |
| WheelLoadRatio | 0.5 | 0.5 |
| RollbarScaling | 0.35 | 0.30 |
| SweepShape | Raycast (Spherecast si hay baches pequeños) | idem |

Ajuste: si rebota (pogo), DampingRatio +0.1. Si toca topes, SpringRate +15%. Altura de reposo objetivo ~ 28 cm libre al suelo.

### Motor (Engine Setup)
- MaxTorque (escala): 420 Nm. MaxRPM: 4800. EngineIdleRPM: 800. EngineBrakeEffect: 0.08. EngineRevUpMOI: 0.6. EngineRevDownRate: 450.
- **Torque Curve** (RPM -> Nm; el editor de curva normaliza en eje X por MaxRPM, aquí valores reales; tipo Diesel turbo, par plano):

| RPM | Nm |
|---|---|
| 0 | 180 |
| 800 | 260 |
| 1200 | 340 |
| 1800 | 420 |
| 2500 | 420 |
| 3200 | 390 |
| 4000 | 310 |
| 4800 | 200 |

En la curva de UE: X = RPM/MaxRPM (0, 0.167, 0.25, 0.375, 0.52, 0.667, 0.833, 1.0), Y = Nm/420 (0.43, 0.62, 0.81, 1.0, 1.0, 0.93, 0.74, 0.48). Interpolación: Cubic.

### Caja de cambios (Transmission Setup)
- bUseAutomaticGears: true. bUseAutoReverse: true. ChangeUpRPM 3800, ChangeDownRPM 1600. GearChangeTime 0.35 s. TransmissionEfficiency 0.9.
- FinalRatio: 4.10.
- Relaciones adelante: 1ª 4.2, 2ª 2.6, 3ª 1.7, 4ª 1.2, 5ª 0.9 (velocidad máx. ~125 km/h con 38 cm de radio).
- Reversa: -3.8.
- Sin modo 4 LO separado: ver bloqueo abajo.

### Diferenciales (Differential Setup, modo AllWheelDrive)
- DifferentialType: **AllWheelDrive** (EVehicleDifferential::AllWheelDrive).
- FrontRearSplit: **0.45** (45% delante, 55% atrás en asfalto).
- Esto es el modo `EDriveMode::FourWheel`. Para `EDriveMode::RWD` en C++: `FrontRearSplit = 0.0` y `DifferentialType = RearWheelDrive` (VERIFY-5.8: `SetDifferentialSetup` runtime; si no existe, cambiar `DifferentialSetup` y recrear la sim con `RecreatePhysicsState()`).
- Diferencial bloqueado (IA/dash `DiffLock`): al activar, `FrontRearSplit = 0.5` y subir `FrictionForceMultiplier` de ruedas +10% (efecto de reparto igual en eje). Rueda a rueda: `Chaos` usa `bLimitedSlip`; si está disponible (`VERIFY-5.8`), `LSD = true`, `LimitedSlip` 0.9 con DiffLock, 0.25 sin él.
- Resultado práctico: DiffLock ON en barro evita que una rueda patine y el furgón se queda sin tracción.

### Dirección (Steering Setup)
- SteeringType: AngleRatio. SteeringCurve (velocidad km/h -> factor del ángulo): (0, 1.0), (30, 0.85), (60, 0.55), (100, 0.35), (130, 0.25).
- Input steer smoothing: rise 4.0, fall 6.0.

### Resto
- Mass del chasis: 2600 kg (el PA gana). DragCoefficient 0.35. DownforceCoefficient 0.2. InertiaTensorScale (1.0, 1.0, 0.9).
- Aerodinámica: Area (frontal) 4.0 m2 si existe.
- Asignar `bSuspensionEnabled = true`, `bWheelFrictionEnabled = true`.
- Anti-vuelco: `Sleep Threshold` normal; `bDeprecateTrailers` no aplica.

## 4. Fricción en tiempo real al entrar a barro (sin remolques)
El volumen `ATerrainModifierVolume` ya llama `PushSurfaceModifier/PopSurfaceModifier`. Implementar en `ABlacklineVanPawn` (no hay remolques, así que se aplica siempre a las 4 ruedas):

1. Mantener `TArray<ATerrainModifierVolume*> SurfaceStack`. Push: `AddUnique`; Pop: `Remove`. Recalcular tras cada cambio.
2. Cálculo: multiplicador efectivo = **mínimo** de `FrictionMultiplier` de los volúmenes activos (el peor manda), 1.0 si la pila está vacía.
   - Si hay un volumen con `bRequires4WD` y `GetDriveMode() == RWD` y el DiffLock está apagado: multiplicador *= 0.6 (se patina) y activar sink.
3. Aplicar a las ruedas (hilo de juego, solo cuando cambia, sin Tick):
```cpp
void ABlacklineVanPawn::ApplySurfaceFriction(float Mult)
{
    UChaosWheeledVehicleMovementComponent* Move = GetVehicleMovementComponent(); // VERIFY-5.8
    for (UChaosVehicleWheel* W : Move->Wheels)
    {
        const float Base = (W->AxleType == EAxleType::Front) ? 3.0f : 3.2f; // defaults de la clase
        W->FrictionForceMultiplier = Base * Mult;   // VERIFY-5.8: puede requerir W->SetFrictionMultiplier? y UpdateWheelSetup
    }
    Move->RecreatePhysicsState(); // solo si el multiplicador no se propaga solo: VERIFY-5.8
}
```
   Alternativa recomendada si la propiedad no se refleja en la sim: cambiar el **Physical Material** del volumen (fricción 0.4 barro / 0.9 asfalto) y mantener `FrictionForceMultiplier` fijo, porque Chaos multiplica por la fricción del material de superficie.
4. Interpolar para no dar tirón: llamar al cambio con un `FTimerManager` a 0.05 s, 6 pasos, de 1.0 hacia Mult (o al revés al salir). Evita Tick.
5. Tablas de superficie (Multiplicador):

| ESurfaceKind | FrictionMultiplier | StuckSinkDepthCm | bRequires4WD |
|---|---|---|---|
| Asphalt | 1.00 | 0 | no |
| WetAsphalt | 0.75 | 0 | no |
| Gravel | 0.65 | 3 | no |
| Mud | 0.45 | 15 | sí |
| Forest | 0.55 | 8 | sí |

6. Atasco: si Mud, 4WD apagado y velocidad < 8 km/h durante 1.5 s con gas > 0.5: `ApplyImpulse` nulo, aplicar `LinearDamping` +1.5 y `AngularDamping` +0.5 al cuerpo hasta marcha atrás o DiffLock. Se quita al salir del volumen.

## 5. Checklist de prueba
- [ ] Furgón parado, 0 deslizamiento lateral.
- [ ] 0-100 km/h: 11-14 s.
- [ ] Frenado 100-0: 55-65 m.
- [ ] Barro con 4WD + DiffLock: sale; con RWD sin bloqueo: atasca.
- [ ] Carga desplaza el centro de masa sin que el pawn vuelque a 60 km/h en curva de 35 m.
