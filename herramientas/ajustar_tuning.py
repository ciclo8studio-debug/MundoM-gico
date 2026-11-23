# Cambia valores de DA_Tuning sin abrir el editor (Unreal cerrado):
#   UnrealEditor-Cmd.exe <uproject> -run=pythonscript -script="<este archivo>"
# Editar el diccionario VALORES antes de correrlo.
import unreal

VALORES = {
    "Demo_SinParque": False,   # 23-11 (tarde): el CTO reconecta el parque; Plan B no evita el cuelgue de GPU tras cambiar de mapa
    "Salida_Y": -4600.0,        # 23-11: el viaje al parque ocurre justo despues de la puerta de Urgencias (antes -12600)
    "Sombra_RepeleSeg": 1.5,    # 23-11: antes 2.5; la linterna no la ahuyentaba en las pruebas del CTO
}

da = unreal.load_asset("/Game/MundoMagico/Datos/DA_Tuning.DA_Tuning")
for k, v in VALORES.items():
    da.set_editor_property(k, v)
    unreal.log("TUNING_OK %s = %s" % (k, da.get_editor_property(k)))
unreal.EditorAssetLibrary.save_loaded_asset(da, False)
