# Crea los efectos de vibracion del control (ForceFeedbackEffect) en /Game/MundoMagico/FX.
# Unreal cerrado: UnrealEditor-Cmd <uproject> -run=pythonscript -script=<este archivo> -unattended -nosplash -nullrhi
import unreal

RUTA = "/Game/MundoMagico/FX"
# nombre: [(tiempo, intensidad), ...], motores grandes, motores chicos
EFECTOS = {
    "FFE_Golpe":  ([(0.0, 1.0), (0.12, 0.8), (0.35, 0.0)], True, True),
    "FFE_Susto":  ([(0.0, 0.0), (0.05, 1.0), (0.25, 0.6), (0.8, 0.0)], True, True),
    "FFE_Trueno": ([(0.0, 0.0), (0.2, 0.7), (0.6, 0.45), (1.6, 0.0)], True, False),
}
herr = unreal.AssetToolsHelpers.get_asset_tools()
for nombre, (claves, grandes, chicos) in EFECTOS.items():
    ruta = RUTA + "/" + nombre
    if unreal.EditorAssetLibrary.does_asset_exist(ruta):
        efecto = unreal.load_asset(ruta)
    else:
        efecto = herr.create_asset(nombre, RUTA, unreal.ForceFeedbackEffect, None)
    canal = unreal.ForceFeedbackChannelDetails()
    canal.set_editor_property("affects_left_large", grandes)
    canal.set_editor_property("affects_right_large", grandes)
    canal.set_editor_property("affects_left_small", chicos)
    canal.set_editor_property("affects_right_small", chicos)
    # La curva se pone despues por MCP (ObjectTools.set_properties): RuntimeFloatCurve no se edita desde Python.
    efecto.set_editor_property("channel_details", [canal])
    unreal.EditorAssetLibrary.save_loaded_asset(efecto, False)
    unreal.log("VIBRA_OK %s" % ruta)
