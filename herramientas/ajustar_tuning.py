# Cambia valores de DA_Tuning sin abrir el editor (Unreal cerrado):
#   UnrealEditor-Cmd.exe <uproject> -run=pythonscript -script="<este archivo>"
# Editar el diccionario VALORES antes de correrlo.
import unreal

VALORES = {
    "Demo_SinParque": True,    # 23-11: G0 del parque no pasa (cuelgue de GPU al pasar parque -> Casona); el CTO activa el Plan B
}

da = unreal.load_asset("/Game/MundoMagico/Datos/DA_Tuning.DA_Tuning")
for k, v in VALORES.items():
    da.set_editor_property(k, v)
    unreal.log("TUNING_OK %s = %s" % (k, da.get_editor_property(k)))
unreal.EditorAssetLibrary.save_loaded_asset(da, False)
