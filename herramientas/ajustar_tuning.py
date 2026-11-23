# Cambia valores de DA_Tuning sin abrir el editor (Unreal cerrado):
#   UnrealEditor-Cmd.exe <uproject> -run=pythonscript -script="<este archivo>"
# Editar el diccionario VALORES antes de correrlo.
import unreal

VALORES = {
    "Salida_Y": -4250.0,        # 23-11 (noche): el glitch te deja en y=-4300; el viaje al parque ocurre al llegar (antes -4600, unos pasos mas adelante)
}

unreal.AssetRegistryHelpers.get_asset_registry().scan_paths_synchronous(["/Game/MundoMagico/Datos"], True)  # sin esto, en modo Cmd a veces no encuentra el asset
da = unreal.load_asset("/Game/MundoMagico/Datos/DA_Tuning.DA_Tuning")
for k, v in VALORES.items():
    da.set_editor_property(k, v)
    unreal.log("TUNING_OK %s = %s" % (k, da.get_editor_property(k)))
unreal.EditorAssetLibrary.save_loaded_asset(da, False)
