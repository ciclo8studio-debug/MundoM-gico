# 14 - UMG / CommonUI (BLACKLINE: Night Haul, UE 5.8.4)

Estado: propuesta. APIs no confirmadas en `docs/engine-reference/unreal/` van con `// VERIFY-5.8`. Textos: `FText` con tabla `ST_Blackline` (ver `06_textos.md`), nunca `FString`.

## 0. Decisiones y dudas abiertas

- **Dependencia nueva:** el CONTRATO lista `UMG` pero no `CommonUI`. Se necesitan `CommonUI`, `CommonInput`, `Slate`, `SlateCore` en `Blackline.Build.cs` y el plugin `CommonUI` en el `.uproject`. Hay que aprobarlo (cambio de contrato).
- **CommonUI en un juego de conduccion:** la guia del motor lo desaconseja para UI solo-mouse sin navegacion. Aqui se justifica por (a) prompts de tecla/gamepad dinamicos, (b) la pantalla de manifiesto navegable con gamepad. El HUD NO es activable: es `UCommonUserWidget` (sin stack ni foco).
- **Acciones CommonUI:** se usan filas ya existentes del `CommonInputActionDataBase` del proyecto. Filas propuestas (nuevas, requieren OK): `BL_Confirm`, `BL_Back`, `BL_NextLine`, `BL_PrevLine`. En 5.8 Enhanced Input y Common UI estan unificados (sin assets duplicados); formato exacto del vinculo IA -> prompt: `// VERIFY-5.8`.
- **Flujo de pantallas (volver, transiciones, modal vs. pantalla completa):** lo define ux-designer. Este documento solo fija la mecanica de stack; el comportamiento por defecto abajo es provisional.
- La UI nunca modifica estado de juego: lee del Pawn/Checkpoint/Pursuit por delegados y emite eventos.

## 1. Capas (un `UCommonActivatableWidgetContainerBase` por capa)

`UBlacklineRootLayout : UCommonUserWidget` (creado por `ABlacklinePlayerController`, `AddToViewport`):

| Widget (BindWidget) | Tipo | Z | Uso |
|---|---|---|---|
| `HUDLayer` | `UCommonActivatableWidgetStack` | 0 | `WBP_HUD`, `WBP_InteractPrompt` (siempre visibles) |
| `MenuLayer` | `UCommonActivatableWidgetStack` | 10 | `WBP_Manifest`, pausa |
| `PopupLayer` | `UCommonActivatableWidgetQueue` | 20 | avisos de checkpoint, confirmaciones |
| `OverlayLayer` | `UCommonActivatableWidgetStack` | 30 | fundidos, carga, debug |

Nota: `HUDLayer` se llena en `BeginPlay` con `AddWidget` y nunca se desapila durante la conduccion.

## 2. WBP_HUD (clase base C++ `UBlacklineHUDWidget : UCommonUserWidget`)

Jerarquia (flat; sin Canvas Panel salvo la raiz; cada grupo es un Overlay/HorizontalBox con anclas):

```
WBP_HUD
└─ RootOverlay (Overlay)                       [no BindWidget]
   ├─ StaticInvalidationBox (InvalidationBox)  [BindWidget]   <- parte casi estatica
   │  └─ SpeedCluster (HorizontalBox)
   │     ├─ SpeedText (CommonTextBlock)        [BindWidget]   "87"
   │     ├─ SpeedUnitText (CommonTextBlock)    [BindWidget]   "km/h"
   │     ├─ DriveModeText (CommonTextBlock)    [BindWidget]   "RWD" / "4x4"
   │     └─ DriveModeIcon (Image)              [BindWidget]   forma ademas de color
   ├─ CargoCluster (VerticalBox)
   │  ├─ CargoIntegrityBar (ProgressBar)       [BindWidget]
   │  ├─ CargoIntegrityText (CommonTextBlock)  [BindWidget]
   │  └─ CargoShiftWarning (Image)             [BindWidget, opcional] Collapsed por defecto
   ├─ SuspicionCluster (VerticalBox)
   │  ├─ SuspicionBar (ProgressBar)            [BindWidget]
   │  ├─ SuspicionIcon (Image)                 [BindWidget]
   │  └─ CheckpointStateText (CommonTextBlock) [BindWidgetOptional]
   ├─ PursuitCluster (VerticalBox)
   │  ├─ PursuitStateText (CommonTextBlock)    [BindWidget]   None/Hunting/Searching/Lost
   │  ├─ PursuitHeatBar (ProgressBar)          [BindWidget]
   │  └─ PursuitIcon (Image)                   [BindWidget]
   ├─ DashStatusRow (HorizontalBox)            [BindWidget]   icono por EDashControl
   │  ├─ DashHeadlightsIcon (Image)            [BindWidget]
   │  ├─ DashWipersIcon (Image)                [BindWidget]
   │  ├─ DashWindowIcon (Image)                [BindWidget]
   │  ├─ DashJammerIcon (Image)                [BindWidget]
   │  ├─ DashDiffLockIcon (Image)              [BindWidget]
   │  └─ DashIgnitionIcon (Image)              [BindWidget]
   └─ SurfaceText (CommonTextBlock)            [BindWidgetOptional] Asphalt/Mud...
```

Nombres exactos de `BindWidget` (deben coincidir 1:1 con el Designer; si falta alguno, falla la compilacion del BP):

| Nombre | Clase | Requerido |
|---|---|---|
| `StaticInvalidationBox` | `UInvalidationBox` | si |
| `SpeedText`, `SpeedUnitText`, `DriveModeText` | `UCommonTextBlock` | si |
| `DriveModeIcon` | `UImage` | si |
| `CargoIntegrityBar` | `UProgressBar` | si |
| `CargoIntegrityText` | `UCommonTextBlock` | si |
| `CargoShiftWarning` | `UImage` | opcional |
| `SuspicionBar` | `UProgressBar` | si |
| `SuspicionIcon` | `UImage` | si |
| `CheckpointStateText` | `UCommonTextBlock` | opcional |
| `PursuitStateText` | `UCommonTextBlock` | si |
| `PursuitHeatBar` | `UProgressBar` | si |
| `PursuitIcon` | `UImage` | si |
| `DashStatusRow` | `UHorizontalBox` | si |
| `DashHeadlightsIcon`, `DashWipersIcon`, `DashWindowIcon`, `DashJammerIcon`, `DashDiffLockIcon`, `DashIgnitionIcon` | `UImage` | si |
| `SurfaceText` | `UCommonTextBlock` | opcional |

### 2.1 Clase C++ (esqueleto completo de la parte de enlace)

`Source/Blackline/Public/UI/BlacklineHUDWidget.h`

```cpp
#pragma once
#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Core/BlacklineTypes.h"
#include "BlacklineHUDWidget.generated.h"

class UCommonTextBlock; class UProgressBar; class UImage; class UInvalidationBox; class UHorizontalBox;
class ABlacklineVanPawn; class UCargoSimulationComponent; class ACheckpointController;

UCLASS(Abstract)
class BLACKLINE_API UBlacklineHUDWidget : public UCommonUserWidget
{
    GENERATED_BODY()
public:
    /** Enlaza el HUD a un furgon. Llamado por el PlayerController en OnPossess. */
    void BindToVan(ABlacklineVanPawn* Van);
    void BindToCheckpoint(ACheckpointController* Checkpoint);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UPROPERTY(meta=(BindWidget)) TObjectPtr<UInvalidationBox> StaticInvalidationBox;
    UPROPERTY(meta=(BindWidget)) TObjectPtr<UCommonTextBlock> SpeedText;
    UPROPERTY(meta=(BindWidget)) TObjectPtr<UCommonTextBlock> SpeedUnitText;
    UPROPERTY(meta=(BindWidget)) TObjectPtr<UCommonTextBlock> DriveModeText;
    UPROPERTY(meta=(BindWidget)) TObjectPtr<UImage> DriveModeIcon;
    UPROPERTY(meta=(BindWidget)) TObjectPtr<UProgressBar> CargoIntegrityBar;
    UPROPERTY(meta=(BindWidget)) TObjectPtr<UCommonTextBlock> CargoIntegrityText;
    UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UImage> CargoShiftWarning;
    UPROPERTY(meta=(BindWidget)) TObjectPtr<UProgressBar> SuspicionBar;
    UPROPERTY(meta=(BindWidget)) TObjectPtr<UImage> SuspicionIcon;
    UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UCommonTextBlock> CheckpointStateText;
    UPROPERTY(meta=(BindWidget)) TObjectPtr<UCommonTextBlock> PursuitStateText;
    UPROPERTY(meta=(BindWidget)) TObjectPtr<UProgressBar> PursuitHeatBar;
    UPROPERTY(meta=(BindWidget)) TObjectPtr<UImage> PursuitIcon;
    UPROPERTY(meta=(BindWidget)) TObjectPtr<UHorizontalBox> DashStatusRow;
    UPROPERTY(meta=(BindWidget)) TObjectPtr<UImage> DashHeadlightsIcon;
    UPROPERTY(meta=(BindWidget)) TObjectPtr<UImage> DashWipersIcon;
    UPROPERTY(meta=(BindWidget)) TObjectPtr<UImage> DashWindowIcon;
    UPROPERTY(meta=(BindWidget)) TObjectPtr<UImage> DashJammerIcon;
    UPROPERTY(meta=(BindWidget)) TObjectPtr<UImage> DashDiffLockIcon;
    UPROPERTY(meta=(BindWidget)) TObjectPtr<UImage> DashIgnitionIcon;
    UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UCommonTextBlock> SurfaceText;

    /** Estilo central (ver seccion 5). */
    UPROPERTY(EditDefaultsOnly, Category="Blackline|UI") TObjectPtr<class UBlacklineUIStyleDataAsset> Style;

    /** Intervalo de refresco de la velocidad (s). Sin NativeTick: timer. */
    UPROPERTY(EditAnywhere, Category="Blackline|UI", meta=(ClampMin="0.05")) float SpeedRefreshInterval = 0.1f;
    /** La velocidad solo se reescribe si cambia al menos este valor (km/h). */
    UPROPERTY(EditAnywhere, Category="Blackline|UI") float SpeedChangeThresholdKmh = 1.f;

private:
    UFUNCTION() void HandleCargoIntegrityChanged(float Integrity01, float Delta);
    UFUNCTION() void HandleCargoShifted(float Severity01);
    UFUNCTION() void HandleDashControlChanged(EDashControl Control, bool bOn);
    UFUNCTION() void HandleCheckpointStateChanged(ECheckpointState Old, ECheckpointState New);
    UFUNCTION() void HandleVerdict(EInspectionVerdict Verdict, float Suspicion01);
    UFUNCTION() void HandlePursuitStateChanged(EPursuitState New);
    void RefreshSpeed();
    void RefreshPursuitHeat();

    TWeakObjectPtr<ABlacklineVanPawn> BoundVan;           // los widgets sobreviven a los actores: siempre weak
    TWeakObjectPtr<ACheckpointController> BoundCheckpoint;
    FTimerHandle SpeedTimer;
    int32 LastShownSpeedKmh = -1;
};
```

Reglas del HUD:
- Sin `NativeTick`. Velocidad y calor de persecucion por `FTimerManager` (0.1 s y 0.25 s); el resto por delegados del contrato (`FOnCargoIntegrityChanged`, `FOnCargoShifted`, `FOnDashControlChanged`, `FOnCheckpointStateChanged`, `FOnInspectionVerdict`, `FOnPursuitStateChanged`).
- Delegados dinamicos: `AddDynamic` en `BindToVan`, `RemoveDynamic` en `NativeDestruct` y al cambiar de Pawn. Comprobar `IsValid()` antes de cada lectura.
- Textos con `FText::AsNumber`/`FText::Format` sobre claves de `ST_Blackline` (`BL_HUD_*`, por crear con localization-lead).
- `ESlateVisibility::Collapsed` (no `Hidden`) para `CargoShiftWarning` y para clusters sin uso.
- Nada de `Canvas Panel` anidado: raiz Overlay + anclas por cluster.

## 3. WBP_InteractPrompt

Prompt contextual al apuntar a un boton del tablero (trazas `ECC_GameTraceChannel1`, tags `Dash.*`). El Pawn hace la traza solo al pulsar `IA_Interact` (ver `09_rendimiento.md`); para el prompt de "mirando" se usa un **timer de baja frecuencia** (0.15 s, 1 traza) en el Pawn que emite un delegado nuevo `FOnInteractTargetChanged(EDashControl Control, bool bValid)` (anadido al Pawn; el contrato permite anadir). Esto es una peticion al equipo de vehiculo: **duda abierta**.

Jerarquia: `UBlacklineInteractPromptWidget : UCommonUserWidget`

```
WBP_InteractPrompt
└─ PromptRoot (Overlay)                        Visibility = Collapsed por defecto
   └─ PromptBox (HorizontalBox)
      ├─ InteractAction (CommonActionWidget)   [BindWidget]  icono por plataforma
      ├─ PromptLabel (CommonTextBlock)         [BindWidget]  "Luces", "Limpiaparabrisas"...
      └─ StateText (CommonTextBlock)           [BindWidgetOptional] "ON"/"OFF"
```

| Nombre BindWidget | Clase | Requerido |
|---|---|---|
| `PromptRoot` | `UOverlay` | si |
| `InteractAction` | `UCommonActionWidget` | si |
| `PromptLabel` | `UCommonTextBlock` | si |
| `StateText` | `UCommonTextBlock` | opcional |

- `InteractAction->SetInputAction(InteractActionRow)` con `FDataTableRowHandle InteractActionRow` (`EditDefaultsOnly`) apuntando a una fila del `CommonInputActionDataBase`; el icono cambia solo (teclado/gamepad) via `UCommonInputSubsystem`. `// VERIFY-5.8` por la unificacion con Enhanced Input.
- Etiqueta por `EDashControl`: `TMap<EDashControl, FText> ControlLabels` (`EditAnywhere`, claves `BL_DASH_*`).
- `ShowPrompt(EDashControl, bool bOn)` y `HidePrompt()`: solo cambian visibilidad y texto; sin allocs por frame.
- Si `bRequires` de contexto (motor apagado, inspeccion en curso): el widget solo refleja lo que el Pawn dice; no decide.

## 4. CommonUI: routing de entrada

- **Conduccion:** entrada por Enhanced Input (`DefaultIMC` del Pawn). Ningun widget del HUD es focusable (`SetIsFocusable(false)`, visibilidad `SelfHitTestInvisible`) y no consume entrada. La causa de "no responde la tecla" en juego es del IMC (Enhanced Input), no de la UI.
- **Manifiesto:** `WBP_Manifest` es `UCommonActivatableWidget` apilado en `MenuLayer`. Al activarse, `GetDesiredInputConfig()` devuelve `ECommonInputMode::Menu` (o `All` para seguir viendo el parabrisas), `EMouseCaptureMode::NoCapture`, sin ocultar HUD por defecto; el juego no se pausa salvo que ux-designer lo pida (la inspeccion es en tiempo real: `ACheckpointController` esta en `Inspecting`). `// VERIFY-5.8` firma exacta de `GetDesiredInputConfig`.
- **Foco:** `NativeGetDesiredFocusTarget()` devuelve `ConfirmButton`. Navegacion de lineas con D-pad/stick y teclas; rueda del mouse permitida.
- **Acciones enlazadas** en `NativeOnActivated` con `RegisterUIActionBinding(FBindUIActionArgs)`: Confirm (presentar) y Back (cancelar), con `bDisplayInActionBar = true`.
- **Cambio de dispositivo:** `UCommonInputSubsystem::OnInputMethodChanged` refresca los `UCommonActionWidget`; no se hardcodean iconos.
- **Debug:** `CommonUI.DumpActivatableTree`, `CommonUI.DumpActionBindings`, Input Debugger de 5.8 (`// VERIFY-5.8`).
- **Plantilla de comportamiento provisional (a validar por ux-designer):** Back cierra el manifiesto sin presentar; Confirm presenta y cierra; abrir el manifiesto lo hace `IA_Interact`/evento de `ACheckpointController` en estado `Approaching`.

## 5. WBP_Manifest (pantalla de manifiesto)

Muestra un `UManifestDataAsset` (`ManifestId, CompanyName, DeclaredCargo[], ForgeryQuality01, RouteCode, Notes`, y `ComputeDeclaredMassKg()` / `ComputeDeclaredSignature01()`). Es de solo lectura; "Presentar" envia el evento al Checkpoint (`ACheckpointController::PresentManifest`) a traves del PlayerController, no lo llama el widget directamente sobre estado propio.

### 5.1 Jerarquia

```
WBP_Manifest  (UBlacklineManifestScreen : UCommonActivatableWidget)
└─ ScreenRoot (Overlay)
   └─ PaperFrame (Border / Image)             fondo "papel"
      └─ ContentBox (VerticalBox)
         ├─ HeaderBox (HorizontalBox)
         │  ├─ CompanyText (CommonTextBlock)  [BindWidget]  CompanyName
         │  └─ RouteText (CommonTextBlock)    [BindWidget]  RouteCode
         ├─ LinesList (CommonListView)         [BindWidget]  una fila por FManifestLine
         ├─ TotalsBox (HorizontalBox)
         │  ├─ DeclaredMassText (CommonTextBlock)      [BindWidget]  ComputeDeclaredMassKg()
         │  └─ DeclaredSignatureBar (ProgressBar)       [BindWidget]  ComputeDeclaredSignature01()
         ├─ NotesText (CommonTextBlock)        [BindWidget]  Notes (autowrap)
         ├─ ForgeryBox (HorizontalBox)         [BindWidgetOptional] ayuda visual al jugador
         │  ├─ ForgeryBar (ProgressBar)        [BindWidget] ForgeryQuality01
         │  └─ ForgeryText (CommonTextBlock)   [BindWidget]
         └─ ButtonRow (HorizontalBox)
            ├─ PresentButton (CommonButtonBase) [BindWidget]  confirmar/presentar
            └─ CancelButton (CommonButtonBase)  [BindWidget]  volver
```

Fila (`WBP_ManifestLineEntry : UCommonUserWidget`, implementa `IUserObjectListEntry`):
`ItemNameText` (`CommonTextBlock`), `QuantityText` (`CommonTextBlock`), `ClassIcon` (`Image`, forma por `ECargoClass`). Todos `BindWidget`.

| Nombre BindWidget (pantalla) | Clase | Requerido |
|---|---|---|
| `CompanyText`, `RouteText`, `NotesText` | `UCommonTextBlock` | si |
| `LinesList` | `UCommonListView` | si |
| `DeclaredMassText` | `UCommonTextBlock` | si |
| `DeclaredSignatureBar` | `UProgressBar` | si |
| `ForgeryBar` | `UProgressBar` | opcional (ver abajo) |
| `ForgeryText` | `UCommonTextBlock` | opcional |
| `PresentButton`, `CancelButton` | `UCommonButtonBase` | si |

Duda abierta (diseno): si `ForgeryQuality01` se muestra al jugador o es dato oculto de la IA del retén. Por defecto `ForgeryBar/ForgeryText` son `BindWidgetOptional` y quedan `Collapsed`.

### 5.2 Datos: UObject de fila (nada de structs crudos en `ListView`)

`FManifestLine` es struct y su `Item` es `TSoftObjectPtr`; se envuelve en un UObject y la carga es asincrona (no sincrona en juego, ver `09_rendimiento.md`).

```cpp
UCLASS(BlueprintType)
class BLACKLINE_API UManifestLineObject : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly) FText ItemName;
    UPROPERTY(BlueprintReadOnly) ECargoClass CargoClass = ECargoClass::Standard;
    UPROPERTY(BlueprintReadOnly) int32 Quantity = 1;
};
```

`UBlacklineManifestScreen` (cabecera esencial):

```cpp
UCLASS(Abstract)
class BLACKLINE_API UBlacklineManifestScreen : public UCommonActivatableWidget
{
    GENERATED_BODY()
public:
    /** Entrada de datos: antes de activar. Mantiene referencia debil. */
    void SetManifest(UManifestDataAsset* InManifest);

    /** Evento hacia el sistema de juego (el widget no muta estado). */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnManifestPresented, UManifestDataAsset*);
    FOnManifestPresented OnManifestPresented;

protected:
    virtual void NativeOnActivated() override;
    virtual void NativeOnDeactivated() override;
    virtual UWidget* NativeGetDesiredFocusTarget() const override { return PresentButton; }

    UPROPERTY(meta=(BindWidget)) TObjectPtr<UCommonTextBlock> CompanyText;
    UPROPERTY(meta=(BindWidget)) TObjectPtr<UCommonTextBlock> RouteText;
    UPROPERTY(meta=(BindWidget)) TObjectPtr<UCommonTextBlock> NotesText;
    UPROPERTY(meta=(BindWidget)) TObjectPtr<UCommonListView> LinesList;
    UPROPERTY(meta=(BindWidget)) TObjectPtr<UCommonTextBlock> DeclaredMassText;
    UPROPERTY(meta=(BindWidget)) TObjectPtr<UProgressBar> DeclaredSignatureBar;
    UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UProgressBar> ForgeryBar;
    UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UCommonTextBlock> ForgeryText;
    UPROPERTY(meta=(BindWidget)) TObjectPtr<UCommonButtonBase> PresentButton;
    UPROPERTY(meta=(BindWidget)) TObjectPtr<UCommonButtonBase> CancelButton;

    UPROPERTY(EditDefaultsOnly, Category="Blackline|UI") FDataTableRowHandle ConfirmAction;
    UPROPERTY(EditDefaultsOnly, Category="Blackline|UI") FDataTableRowHandle BackAction;

private:
    void Populate();                       // una sola vez por apertura: SetListItems(...) en bloque
    void OnItemsLoaded();                  // callback del StreamableHandle
    void HandlePresentClicked();
    void HandleCancelClicked();

    TWeakObjectPtr<UManifestDataAsset> Manifest;
    TSharedPtr<FStreamableHandle> ItemLoadHandle;   // carga asincrona de UCargoItemDataAsset
};
```

Mapeo de datos a widgets (`Populate`):

| Campo de `UManifestDataAsset` | Widget | Forma |
|---|---|---|
| `CompanyName` (`FText`) | `CompanyText` | `SetText` directo |
| `RouteCode` (`FText`) | `RouteText` | `SetText` directo |
| `Notes` (`FText`) | `NotesText` | `SetText`, autowrap |
| `DeclaredCargo` (`TArray<FManifestLine>`) | `LinesList` | una `UManifestLineObject` por linea; `Item` resuelto con `UAssetManager::GetStreamableManager().RequestAsyncLoad` (recolectar `ToSoftObjectPath()`), `ItemName = Item->DisplayName` (`FText`), `CargoClass`, `Quantity`; `LinesList->SetListItems(Objects)` una sola vez |
| `ComputeDeclaredMassKg()` | `DeclaredMassText` | `FText::Format(NSLOCTEXT/ST_Blackline "{0} kg", FText::AsNumber(Mass))` |
| `ComputeDeclaredSignature01()` | `DeclaredSignatureBar` | `SetPercent` |
| `ForgeryQuality01` | `ForgeryBar`/`ForgeryText` | solo si el diseno lo expone |
| `ManifestId` | ninguno | solo log/`LogBlackline` |

Reglas:
- `Populate()` se llama en `NativeOnActivated`, no por frame; si el asset cambia, se vuelve a llamar. Mientras carga la carga asincrona, `LinesList` queda vacia y `PresentButton->SetIsEnabled(false)`.
- `NativeOnDeactivated`: `ItemLoadHandle.Reset()`, `LinesList->ClearListItems()`, `OnManifestPresented` no se limpia (lo posee quien lo creo).
- `HandlePresentClicked`: `OnManifestPresented.Broadcast(Manifest.Get())` + `DeactivateWidget()`. El PlayerController, suscrito, llama `ACheckpointController::PresentManifest`. Guardas `IsValid(Manifest)`.
- Clic con teclado/gamepad/mouse: `UCommonButtonBase::OnClicked()`.
- Fuente y estilo de los textos: por `UCommonTextStyle` del `UBlacklineUIStyleDataAsset` (seccion 6).

## 6. Estilos (tema central)

`UBlacklineUIStyleDataAsset : UPrimaryDataAsset` (`Source/Blackline/Public/UI/`), referenciado por todos los widgets (nada de colores/fuentes en el BP):

```cpp
UCLASS(BlueprintType)
class BLACKLINE_API UBlacklineUIStyleDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere) FLinearColor ColorNormal   = FLinearColor(0.85f, 0.85f, 0.80f);
    UPROPERTY(EditAnywhere) FLinearColor ColorWarning  = FLinearColor(0.95f, 0.70f, 0.10f);
    UPROPERTY(EditAnywhere) FLinearColor ColorDanger   = FLinearColor(0.90f, 0.15f, 0.15f);
    UPROPERTY(EditAnywhere) FLinearColor ColorSafe     = FLinearColor(0.20f, 0.75f, 0.40f);
    UPROPERTY(EditAnywhere) TSubclassOf<UCommonTextStyle> TextHeader;
    UPROPERTY(EditAnywhere) TSubclassOf<UCommonTextStyle> TextBody;
    UPROPERTY(EditAnywhere) TSubclassOf<UCommonTextStyle> TextHUDNumber;
    UPROPERTY(EditAnywhere) float PaddingSmall = 4.f;
    UPROPERTY(EditAnywhere) float PaddingMedium = 8.f;
    UPROPERTY(EditAnywhere) float TextScale = 1.f;      // 0.85 / 1.0 / 1.25 (pequeno/normal/grande)
    UPROPERTY(EditAnywhere) bool bUseShapeIcons = true; // forma ademas de color (daltonismo)
};
```

Assets: `DA_UIStyle_Default`, `DA_UIStyle_HighContrast`, `DA_UIStyle_Colorblind`; cambio por ajuste del jugador, aplicado a los widgets vivos por un evento (no tick). `CommonButtonStyle` por `UCommonButtonStyle` BP (hover/pressed/disabled/focused definidos para teclado y gamepad).

Accesibilidad:
- 3 tamanos de texto minimo (`TextScale`).
- Ningun estado solo por color: `SuspicionIcon`, `PursuitIcon`, `DriveModeIcon`, `ClassIcon` cambian de forma y de texto.
- Subtitulos (lineas del guardia `BL_GUARD_*`): widget aparte con tamano, fondo y etiqueta de orador configurables (pendiente, no cubierto aqui).
- Opcion de reducir/omitir animaciones de UI.

## 7. Optimizacion e invalidacion

- `StaticInvalidationBox` envuelve `SpeedCluster`, `CargoCluster`, `DashStatusRow` (cambian poco). Se invalida solo en `Handle*` (integridad, dash, modo de traccion, velocidad con umbral) via `InvalidateCache`/cambio de propiedad; no se cachea lo que anima cada frame (barras con interpolacion, parpadeos). `// VERIFY-5.8` nombres de API de invalidacion, y confirmar con `Slate.InvalidationDebugging` / `stat slate`.
- Velocidad: timer 0.1 s, y solo `SetText` si `|km/h - ultimo| >= SpeedChangeThresholdKmh` (entero distinto). Una `FText` por cambio, no por frame.
- Cargo/Persecucion: por delegado; el calor de persecucion por timer 0.25 s (coincide con la frecuencia de `UPursuitDirectorSubsystem`).
- Lista del manifiesto: 4-8 lineas tipicas; `UCommonListView` con pool de entradas y `SetListItems` en bloque; sin crear widgets en tick.
- Sin `NativeTick` en ningun widget del HUD ni del manifiesto. Sin bindings por funcion (`PropertyBinding`) de UMG (evaluan cada frame): todo por `Set*`.
- `Collapsed` para lo oculto; jerarquia plana; `Retainer Box` NO (coste de memoria y tests de render; evaluar solo si `stat slate` lo pide).
- Presupuesto: UMG dentro del 1.0 ms compartido con Checkpoint y Terrain (`09_rendimiento.md` seccion 1); objetivo propio <= 0.4 ms de game thread y 0 ms en reposo (solo timers).
- Medicion: build Development/Test (nunca PIE): `stat slate`, `stat ui`, `Slate.ShowDebugOutlines 1`, Widget Reflector, y Insights con `-trace=cpu,frame`. Escenarios de `09_rendimiento.md` (a) y (f).
- Pooling: los avisos de `PopupLayer` (cola) reutilizan una sola instancia por tipo; limpiar texto y visibilidad al liberar.

## 8. Archivos a crear (en `Source/Blackline/`)

| Archivo | Contenido |
|---|---|
| `Public/UI/BlacklineRootLayout.h`, `Private/UI/BlacklineRootLayout.cpp` | capas y stacks |
| `Public/UI/BlacklineHUDWidget.h`, `Private/UI/BlacklineHUDWidget.cpp` | HUD |
| `Public/UI/BlacklineInteractPromptWidget.h`, `.cpp` | prompt |
| `Public/UI/BlacklineManifestScreen.h`, `.cpp` | pantalla |
| `Public/UI/ManifestLineObject.h`, `ManifestLineEntryWidget.h/.cpp` | fila de lista |
| `Public/UI/BlacklineUIStyleDataAsset.h` | tema |

Content: `Content/UI/WBP_HUD`, `WBP_InteractPrompt`, `WBP_Manifest`, `WBP_ManifestLineEntry`, `DA_UIStyle_*`, DataTable `DT_CommonInputActions`.

## 9. Dudas abiertas

1. Aprobar `CommonUI`/`CommonInput`/`Slate`/`SlateCore` en Build.cs y el plugin.
2. Filas nuevas `BL_Confirm`, `BL_Back`, `BL_NextLine`, `BL_PrevLine` en `CommonInputActionDataBase`.
3. Delegado nuevo `FOnInteractTargetChanged` en `ABlacklineVanPawn` (traza a baja frecuencia).
4. Flujo UX (volver, pausar o no durante el manifiesto, modal o pantalla completa): ux-designer.
5. Exponer o no `ForgeryQuality01` al jugador: diseno.
6. Claves `BL_HUD_*`, `BL_DASH_*`, `BL_UI_*` en `ST_Blackline`: localization-lead.
7. Firmas exactas 5.8 (`GetDesiredInputConfig`, vinculo Enhanced Input a CommonUI, API de invalidacion): `// VERIFY-5.8`.
