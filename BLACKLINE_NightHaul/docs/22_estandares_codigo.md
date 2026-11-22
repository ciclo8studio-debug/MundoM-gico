# 22 - Estandares de codigo UE 5.8 (modulo Blackline)

Base: `CONTRATO.md`. UE 5.8.4 es posterior al conocimiento del equipo: toda API dudosa se marca `// VERIFY-5.8` y se confirma contra `docs/engine-reference/unreal/` y los headers del engine instalado. Chaos Vehicles es plugin **Experimental** en 5.8 (ver `PLUGINS.md`): sus firmas pueden cambiar.

## 1. Naming
- Prefijos UE: `U` (UObject/componente/subsistema), `A` (Actor), `F` (struct), `E` (enum), `I` (interfaz), `T` (plantilla). Enums `enum class ... : uint8`.
- Clases con prefijo de modulo solo donde el contrato lo fija (`ABlacklineVanPawn`, `UBlacklineWheelFront`); el resto sin prefijo (`ACheckpointController`).
- Archivos = nombre de clase sin prefijo (`CargoSimulationComponent.h`). Ruta: `Public|Private/<Subcarpeta>/` igual que el contrato (Core, Data, Cargo, Checkpoint, Pursuit, Terrain, Vehicle).
- Booleanos `bNombre`. Funciones en PascalCase con verbo (`GetSpeedKmh`, `PushSurfaceModifier`). Handlers privados `HandleXxx`; callbacks de delegado `OnXxx` solo para delegados publicos.
- Unidades en el nombre cuando no son del engine: `Kmh`, `Ms`, `Kg`, `Cm`, `01` (normalizado 0..1). UE trabaja en cm y kg: no mezclar m.
- Delegados: `FOnXxx` (contrato). Log: `LogBlackline` con `UE_LOG(LogBlackline, ...)`.
- Cabecera de archivo: comentario de 1 linea con el proposito. Doc comment `/** */` en toda clase y metodo publico.

## 2. Includes
- `.h`: primero `CoreMinimal.h`, luego la base (`GameFramework/Actor.h`), luego `Core/BlacklineTypes.h`, y `X.generated.h` **siempre ultimo**.
- Forward-declare (`class UBoxComponent;`) todo lo que solo se use por puntero/referencia en el `.h`; incluir en el `.cpp`.
- `.cpp`: su propio header primero, luego engine, luego proyecto. Rutas relativas a `Public/` (`"Vehicle/BlacklineVanPawn.h"`), nunca `../`.
- Evitar ciclos Pawn <-> Terrain <-> Cargo: cada par se conoce solo por forward declaration en el `.h`.
- No incluir `ChaosVehicleMovementComponent.h` en headers publicos si basta `ChaosWheeledVehicleMovementComponent` por forward declaration.
- `Blackline.h` solo para `LogBlackline`; no es PCH de includes pesados.

## 3. UPROPERTY / UFUNCTION
- **Todo puntero UObject lleva `UPROPERTY`** (regla del contrato) o `TWeakObjectPtr`; usar `TObjectPtr<T>` en miembros.
- Componentes creados en constructor: `VisibleAnywhere, BlueprintReadOnly` + `CreateDefaultSubobject`. Referencias a assets: `EditAnywhere` (+ `TSoftObjectPtr`/`TSoftClassPtr` si pesan).
- Datos de diseno: `EditAnywhere` con default razonable y `meta=(ClampMin, ClampMax, Units="cm")`. Sin coordenadas magicas en codigo.
- `BlueprintCallable/ReadWrite` solo donde diseno itere; el resto C++ puro. Estado interno: `Transient`, sin `Blueprint*`.
- Contenedores de punteros UObject: `UPROPERTY(Transient) TArray<TObjectPtr<T>>` (visible al GC). Los `TArray<TWeakObjectPtr>` no requieren UPROPERTY.
- `UFUNCTION()` obligatorio en callbacks enlazados con `AddDynamic`.
- `UCLASS(Abstract)` para bases; `meta=(AllowPrivateAccess="true")` si un campo privado se expone a BP.
- Datos: `UPrimaryDataAsset` + `GetPrimaryAssetId`; no leer `UCargoItemDataAsset` con `LoadSynchronous` en gameplay (usar StreamableManager en carga de nivel).
- Sin Tick en actores (`PrimaryActorTick.bCanEverTick = false`); excepcion: `UCargoSimulationComponent` a 20 Hz (`TickInterval = 0.05f`). Preferir `FTimerManager`.

## 4. Delegados
- Contrato: `DECLARE_DYNAMIC_MULTICAST_DELEGATE_*` con `UPROPERTY(BlueprintAssignable)`. Los nombres de parametros llevan sufijo de unidad (`Integrity01`).
- Enlazar con `AddDynamic(this, &UClase::HandleXxx)` (el handler es `UFUNCTION`); en `EndPlay` o `Deinitialize` hacer `RemoveDynamic`. Evitar enlazar dos veces (`AddUniqueDynamic`).
- Broadcast: solo si hay cambio real (comparar valor anterior) para no saturar BP. Nunca broadcast desde el constructor ni desde el hilo de fisica.
- Delegados nativos (`DECLARE_MULTICAST_DELEGATE`) solo para uso interno C++ sin BP; con `FDelegateHandle` guardado y `Remove` en limpieza.
- `UPursuitDirectorSubsystem::Get(const UObject*)` usa `GEngine->GetWorldFromContextObject(..., LogAndReturnNull)` y comprueba nullptr.

## 5. Threading de fisica (Chaos)
- Todo codigo del juego corre en el **hilo de juego**. No tocar `FBodyInstance`/Chaos particles desde otros hilos.
- Chaos Vehicles puede simular en el hilo de fisica (Physics Thread/async). Si `p.Chaos.Simulation.Async`/`bUseAsyncPhysics`... esta activo, la lectura de velocidad/transform en juego es el **ultimo estado interpolado**, no el de este paso: no usarlo para logica exacta. // VERIFY-5.8 (nombres de CVar/setting).
- Escribir al cuerpo (`SetCenterOfMass`, `AddForce`, masa) solo desde juego y de forma esporadica (cambio de carga), no cada frame. Para fuerzas por paso de fisica usar el callback de fisica soportado en 5.8 (`FAsyncPhysicsTickCallback`/`AsyncPhysicsTickComponent`). // VERIFY-5.8
- Modificar parametros de rueda en runtime (friccion por terreno): hacerlo por `UChaosWheeledVehicleMovementComponent` (setters del componente), nunca editar el `UChaosVehicleWheel` CDO compartido. Aplicar solo en `Push/PopSurfaceModifier`, no por frame.
- Overlaps y `BeginOverlap`/`EndOverlap` se despachan en juego; el volumen no guarda punteros crudos al pawn (usar `TArray<TObjectPtr>` + limpieza en `EndPlay`).
- Cargo a 20 Hz lee `GetLateralG()` derivado de velocidad del pawn; suavizar (low-pass) para tolerar el desfase de interpolacion.
- Estado compartido sin locks: prohibido. Si algun dia hay trabajo paralelo, usar `FCriticalSection`/`TAtomic` y documentarlo en el header.

## 6. Errores comunes de compilacion (ChaosVehicles 5.8)
| Sintoma | Causa | Arreglo |
|---|---|---|
| `LNK2019`/`unresolved external` en `UChaosVehicleWheel` | Falta `ChaosVehicles` en `PublicDependencyModuleNames` | Verificar `Blackline.Build.cs` (contrato: ChaosVehicles, PhysicsCore) |
| `cannot open include file 'ChaosVehicleWheel.h'` / `WheeledVehiclePawn.h` | Include mal ubicado | `#include "ChaosVehicleWheel.h"` y `#include "WheeledVehiclePawn.h"` (modulo ChaosVehicles); `ChaosWheeledVehicleMovementComponent.h` para el movement. // VERIFY-5.8 |
| `'X' is not a member of 'UChaosVehicleWheel'` | Propiedad renombrada/retirada entre versiones (`MaxSteerAngle`, `CorneringStiffness`, `WheelMass`, `SuspensionSmoothing`, `WheelLoadRatio`) | Abrir el header del engine y ajustar; mantener el `// VERIFY-5.8` hasta confirmar |
| `EAxleType` no resuelve | Enum con namespace/alcance distinto | Cualificar `EAxleType::Front` y comprobar el header |
| Error en `GENERATED_BODY`/UHT con struct del contrato (`TSoftObjectPtr<UCargoItemDataAsset>` en `BlacklineTypes.h`) | Falta forward declaration `class UCargoItemDataAsset;` antes de la struct | Declararla en `BlacklineTypes.h`; no incluir el header del asset (ciclo) |
| UHT: "Unrecognized type" en `UPROPERTY` | Include/forward faltante o `generated.h` no ultimo | Revisar seccion 2 |
| Constructor de pawn no usa `UBlacklineWheelFront` | Usar `ObjectInitializer.SetDefaultSubobjectClass` en el constructor del pawn | Asignar `WheelSetups[i].WheelClass` en el movement component (`GetVehicleMovementComponent()->WheelSetups`) |
| `GetVehicleMovementComponent()` devuelve base | Hay que castear a `UChaosWheeledVehicleMovementComponent` | `CastChecked<UChaosWheeledVehicleMovementComponent>` en el constructor |
| `ECC_GameTraceChannel1` sin efecto | Canal no definido en `DefaultEngine.ini` | Registrar "Interact" en `[/Script/Engine.CollisionProfile]`; usar `#define` con nombre en un header |
| Warning de deprecacion en API de fisica antigua | PhysX/APIs previas a 5.8 removidas | Consultar `deprecated-apis.md`; no usar PhysX Vehicles |
| `UE_LOG` sin categoria | Falta `DECLARE_LOG_CATEGORY_EXTERN` y `DEFINE_LOG_CATEGORY` | Declarar en `Blackline.h`, definir en `Blackline.cpp` |

## 7. Checklist de revision (.h/.cpp en `Source/Blackline/`)
**Estructura**
- [ ] Ruta `Public|Private/<Subcarpeta>/` correcta; un tipo principal por archivo.
- [ ] `#pragma once`, `.generated.h` ultimo, `BLACKLINE_API` en clases UCLASS.
- [ ] Forward declarations en `.h`; includes pesados en `.cpp`; sin ciclos.
- [ ] Firmas publicas identicas al `CONTRATO.md` (solo se anaden).

**Reflexion y memoria**
- [ ] Todo puntero UObject: `UPROPERTY` o `TWeakObjectPtr`; miembros `TObjectPtr`.
- [ ] Defaults razonables, `ClampMin/Max` y `Units`; sin numeros magicos ni coordenadas fijas.
- [ ] `BlueprintCallable/ReadWrite` justificado.
- [ ] Callbacks con `UFUNCTION()`; `AddDynamic` con su `RemoveDynamic` en `EndPlay/Deinitialize`.

**Rendimiento / hilos**
- [ ] Sin Tick en actores; componente de carga a 20 Hz; timers en vez de tick.
- [ ] Sin acceso a fisica fuera del hilo de juego; sin escrituras a cuerpo por frame.
- [ ] Sin `LoadSynchronous`, `GetAllActorsOfClass` ni `TActorIterator` en rutas calientes.
- [ ] Broadcast solo ante cambio real.

**Robustez**
- [ ] Comprobar nullptr/`IsValid` en punteros de overlap, subsistemas y componentes.
- [ ] `Super::` llamado en overrides (`BeginPlay`, `EndPlay`, `PostInitializeComponents`).
- [ ] Limpieza al destruir (`Pop` de modificadores, desenlazar delegados).
- [ ] `UE_LOG(LogBlackline, ...)` en vez de `GLog`/`printf`; sin logs por frame.

**Estilo**
- [ ] Naming de la seccion 1; doc comment en toda clase y metodo publico.
- [ ] Metodo <= 40 lineas, complejidad ciclomatica <= 10.
- [ ] APIs de 5.8 dudosas marcadas `// VERIFY-5.8`.
- [ ] Compila limpio sin warnings nuevos con `-WarningsAsErrors` en CI local.
