# 15 - Herramientas de editor

## Requisitos
- Modulo `Blackline` compilado (clases `UCargoItemDataAsset`, `UManifestDataAsset`).
- Plugin **Python Editor Script Plugin** activo (Edit > Plugins). Reiniciar el editor.
- `Content/Python` esta en el path de Python del editor (se agrega solo al estar bajo `Content/Python`). // VERIFY-5.8

Los nombres Python de clases/propiedades siguen la convencion de UE (`CargoItemDataAsset`, `mass_kg`, `fragility01`). Si algo falla con AttributeError, revisar con `dir(unreal.CargoItemDataAsset)`.

## 1. Crear assets de ejemplo
`Content/Python/blackline_make_assets.py`. En Output Log, cambiar a **Python** y ejecutar:

```python
import blackline_make_assets as b
b.run()                 # omite los que ya existen
b.run(overwrite=True)   # borra y regenera
```

Crea en `/Game/Blackline/Data/Cargo`: `DA_Cargo_Standard, Fragile, Volatile, Cryogenic, Relic, AIServer`, y en `/Game/Blackline/Data/Manifests`: `DA_Manifest_Clean, Forged, Heavy`. Edita las tablas `CARGOS` y `MANIFESTS` al inicio del script para cambiar valores. Los valores son de ejemplo (el contrato define campos, no cifras): ajustar con diseno.

Tambien por linea de comandos: `UnrealEditor-Cmd.exe Blackline.uproject -run=pythonscript -script="Content/Python/blackline_make_assets.py"`.

## 2. Validacion de data assets
`Content/Python/blackline_validate_assets.py` revisa:
- Cargo: ItemId unico y no vacio, MassKg > 0, Fragility01 y ContrabandSignature01 en [0,1], BoxExtentCm > 0, ValueCredits >= 0, DisplayName no vacio.
- Manifest: ManifestId unico, ForgeryQuality01 en [0,1], DeclaredCargo no vacio, cada linea con Item y Quantity >= 1.

```python
import blackline_validate_assets as v
errores = v.run()
```

### Editor Utility (un clic)
1. Content Browser > clic derecho > Editor Utilities > **Editor Utility Widget**, nombre `EUW_ValidateData`.
2. En el Designer agregar un `Button` ("Validar") y un `MultiLineEditableText` (solo lectura, "Resultado").
3. En el Graph: `OnClicked (Button)` > **Execute Python Script** con `import blackline_validate_assets as v; v.run()`. Para mostrar resultados, leer el Output Log (filtro `[Blackline]`) o usar `Execute Python Script` con `unreal.log` y copiar el texto al widget.
4. Abrir con clic derecho > Run Editor Utility Widget. Opcional: registrarlo en el menu Tools con un Editor Utility Action o Tool Menus.

Alternativa sin Blueprint: ejecutar `v.run()` en la consola Python; errores salen en rojo en el Output Log y quedan en el valor de retorno.

## Convenciones
- Nombres: `DA_Cargo_<Nombre>`, `DA_Manifest_<Nombre>`.
- Correr la validacion antes de cada build/commit de contenido.
