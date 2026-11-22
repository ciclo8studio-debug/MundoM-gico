# 18 — Localización (BLACKLINE: NIGHT HAUL)

Idiomas: `es` (fuente, base) y `en`. Cadena de fallback: `es-CL -> es -> en`. Nunca se muestra una clave cruda al jugador.
Motor: sistema nativo de UE (FText, String Tables, Localization Dashboard). Marcar APIs dudosas con `// VERIFY-5.8`.

## 1. Conflicto con docs/06_textos.md (decisión pendiente)

`06_textos.md` define UNA tabla `ST_Blackline` con claves `BL_<AREA>_<ID>`. Este documento pide tres tablas: `ST_Dialogue`, `ST_UI`, `ST_Cargo`.
Propuesta: adoptar las tres tablas y conservar el sufijo de `06` como clave (`BL_GUARD_GREET_01` sigue igual), así `06_textos.md` solo cambia de tabla, no de clave. Manifiestos (`BL_MAN_*`) van a `ST_Cargo`. Confirmar con el CTO/writer antes de importar.

## 2. FText y LOCTEXT en las clases del contrato

Regla: todo texto visible es `FText`; nunca `FString`/`FName` para mostrar. `FName` solo para IDs (`ItemId`, `ManifestId`).

| Clase | Campo / uso | Mecanismo |
|---|---|---|
| `UCargoItemDataAsset` | `DisplayName` (FText) | `EditAnywhere`; el data asset referencia la clave de `ST_Cargo` (campo "String Table + Key"). Añadir `FText Description` (`BL_CARGO_*_DESC`) |
| `UManifestDataAsset` | `CompanyName`, `RouteCode`, `Notes` (FText) | Referencia a `ST_Cargo` (`BL_MAN_*`). `RouteCode` es dato, no se traduce: usar `FText::AsCultureInvariant` |
| `ACheckpointController` | Líneas del guardia por veredicto | Se pide `FText` a `ST_Dialogue` con `FText::FromStringTable(ST_ID, Key)` y se formatea con `FText::Format` + `FFormatNamedArguments` (`{company}`, `{route}`, `{plate}`) |
| `ABlacklineVanPawn` | Etiquetas de `EDashControl` y prompts de `IA_Interact` | `LOCTEXT` en `Private/Vehicle/` con namespace `Blackline.Dash`; enums vía `UEnum::GetDisplayValueAsText` con `UMETA(DisplayName=NSLOCTEXT(...))` |
| `UPursuitDirectorSubsystem` | Avisos de estado (`EPursuitState`) | Sin texto propio: expone el estado; el HUD (UMG) resuelve la clave de `ST_UI` |
| `UCargoSimulationComponent` | Avisos de integridad / carga movida | Igual: delegados con valores; el texto lo pone UMG |
| `ATerrainModifierVolume` | Aviso "requiere 4x4" (`bRequires4WD`) | Clave `ST_UI`, mostrada por UMG |
| `UBlacklineWheel*` | Ninguno | — |

Convenciones C++:
- `#define LOCTEXT_NAMESPACE "Blackline.<Area>"` al inicio del .cpp y `#undef LOCTEXT_NAMESPACE` al final. Nunca en un .h.
- Los textos del guardia, cargas y UI viven en String Tables, no en LOCTEXT de C++. LOCTEXT queda solo para fallbacks de desarrollo y etiquetas de enums.
- Números con `FText::AsNumber`, porcentajes con `FText::AsPercent`, tiempo con `FText::AsTime`. Nunca concatenar con `FString::Printf` para texto visible.
- Los `UENUM` no se muestran por nombre: se mapean a clave `ST_UI` (`BL_UI_ENUM_<ENUM>_<VALOR>`).

## 3. String Tables

Archivos: `Content/Blackline/Localization/ST_Dialogue`, `ST_UI`, `ST_Cargo` (CSV importables, columna Key/SourceString; comentario de contexto en metadata). Convención de claves: `BL_<AREA>_<ID>`, en mayúsculas, números con dos dígitos.

### ST_Dialogue (guardia del retén)
Variables: `{company}`, `{route}`, `{plate}`. Tope: 120 caracteres.
- `BL_GUARD_GREET_01..05` (estado Approaching)
- `BL_GUARD_SUSP_01..05` (SecondaryInspection)
- `BL_GUARD_PASS_01..05` (CleanPass)
- `BL_GUARD_ALERT_01..05` (PursuitAlert)
- Futuro: `BL_RADIO_*` (radio de la policía durante la persecución), `BL_NPC_*`.

### ST_Cargo
- `BL_CARGO_CRYO_NAME` / `_DESC`, `BL_CARGO_RELIC_NAME` / `_DESC`, `BL_CARGO_AI_NAME` / `_DESC`, `BL_CARGO_VOL_NAME` / `_DESC`
- `BL_CARGO_STD_NAME` / `_DESC`, `BL_CARGO_FRAGILE_NAME` / `_DESC` (por definir)
- `BL_CARGO_CLASS_<STANDARD|FRAGILE|VOLATILE|CRYOGENIC|RELIC|AISERVER>` (etiqueta de `ECargoClass`)
- Manifiestos: `BL_MAN_<AGRO|MED|MIN|MUS>_COMPANY`, `_ROUTE`, `_NOTES`

### ST_UI
| Grupo | Claves |
|---|---|
| Tablero (`EDashControl`) | `BL_UI_DASH_HEADLIGHTS`, `_WIPERS`, `_WINDOW`, `_SIGNALJAMMER`, `_DIFFLOCK`, `_IGNITION`; estados `BL_UI_STATE_ON` / `_OFF` |
| Interacción | `BL_UI_PROMPT_INTERACT`, `BL_UI_PROMPT_TOGGLECAM`, `BL_UI_PROMPT_DRIVEMODE` |
| Modo de tracción | `BL_UI_DRIVE_RWD`, `BL_UI_DRIVE_4WD` |
| Retén (`ECheckpointState`) | `BL_UI_CHECKPOINT_APPROACHING`, `_INSPECTING`, `_CLEARED`, `_PURSUIT` |
| Veredicto | `BL_UI_VERDICT_CLEAN`, `_SECONDARY`, `_ALERT` |
| Persecución (`EPursuitState`) | `BL_UI_PURSUIT_HUNTING`, `_SEARCHING`, `_LOST` |
| Superficie / 4x4 | `BL_UI_SURFACE_<ASPHALT|WET|GRAVEL|MUD|FOREST>`, `BL_UI_WARN_NEEDS_4WD` |
| Carga | `BL_UI_CARGO_INTEGRITY` ("Integridad: {value}"), `BL_UI_CARGO_SHIFTED`, `BL_UI_CARGO_SIGNATURE` |
| Menús | `BL_UI_MENU_PLAY`, `_OPTIONS`, `_QUIT`, `_LANGUAGE`, `_BACK` |

Cada clave lleva comentario de contexto: pantalla, límite de caracteres y variables.

## 4. Plan es / en

| Fase | Contenido | Responsable |
|---|---|---|
| 1 | Texto fuente en `es` (ya en `06_textos.md`); cargar CSV a las 3 tablas | writer + tools |
| 2 | Traducción `en` con glosario (Retén = Checkpoint, Furgoneta = Van, Manifiesto = Manifest, Hermandad Gris = Grey Brotherhood, nombres propios sin traducir: "Escarcha", "Mnemosina") | traductor externo / writer |
| 3 | Import vía Localization Dashboard (`Content/Localization/Blackline`, cultura nativa `es`, objetivo `en`) | tools |
| 4 | Prueba: cambiar cultura en runtime (`FInternationalization::Get().SetCurrentCulture`) en una build empaquetada | QA |

- Cambio de idioma desde el menú sin reiniciar; widgets se refrescan vía evento de cambio de cultura.
- Sin voces dobladas por ahora: solo texto y subtítulos del guardia. Si se dobla, el audio sigue la misma clave.
- Fuentes: el set Latin con diacríticos (ñ, á, é, í, ó, ú, ¿, ¡) debe verificarse en todas las fuentes de UI. No se necesita CJK/RTL.
- Fecha, número y moneda (`ValueCredits`): formato por cultura con `FText::AsNumber`/`AsCurrency`; `es-CL` usa punto de millares y coma decimal.

## 5. Reglas de longitud para UI

Referencia: el inglés suele ser más corto que el español (en torno al 15-25 %); el español base fija el máximo.

| Elemento | Máx. caracteres |
|---|---|
| Línea de diálogo del guardia (subtítulo) | 120 (máx. 2 líneas) |
| Etiqueta de botón del tablero | 14 |
| Prompt de interacción | 28 |
| Estado del retén / persecución (HUD) | 24 |
| Nombre de carga | 36 |
| Descripción de carga | 160 |
| Campo `Notes` del manifiesto | 90 |
| Nombre de empresa en manifiesto | 40 |
| Menú (botón) | 20 |

Reglas:
1. Se presupuesta +35 % sobre el texto fuente en el layout, por si se añaden idiomas largos (alemán, finés).
2. Contenedores con auto-ajuste (Size Box + Scale Box o `Auto Wrap Text`); nunca recortar sin elipsis.
3. Los límites van en el comentario de contexto de cada clave y se validan con un script del importador (falla si se pasa).
4. Variables con nombre (`{company}`), nunca `{0}`; el traductor puede reordenarlas. Sin concatenar fragmentos de frase.
5. Plurales con formato ICU: `{count}|plural(one=caja,other=cajas)` en UE; incluir `other` siempre. Es y en solo necesitan `one`/`other`.
6. Pseudolocalización: build de prueba que alarga cada texto +40 % y añade acentos para detectar desbordes antes de traducir.

## 6. Pendientes
- Confirmar las tres tablas vs `ST_Blackline` (sección 1).
- Lista final de cargas (`06_textos.md` aún sin cierre) y claves STD/FRAGILE.
- Revisar con ux-designer los máximos de la sección 5 contra `13_ux_cabina.md`.
- Glosario `es`/`en` definitivo (writer).
