# Cambia valores de DA_Tuning sin abrir el editor (Unreal cerrado):
#   UnrealEditor-Cmd.exe <uproject> -run=pythonscript -script="<este archivo>"
# Editar el diccionario VALORES antes de correrlo.
import unreal

VALORES = {
    "Demo_SinParque": False,   # 18-11: el CTO pide volver a conectar el parque (sin pantalla de fin de demo)
}

da = unreal.load_asset("/Game/MundoMagico/Datos/DA_Tuning.DA_Tuning")
for k, v in VALORES.items():
    da.set_editor_property(k, v)
    unreal.log("TUNING_OK %s = %s" % (k, da.get_editor_property(k)))
unreal.EditorAssetLibrary.save_loaded_asset(da, False)
