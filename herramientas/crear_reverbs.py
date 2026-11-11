# Crea efectos de reverb para las salas del hospital (/Game/MundoMagico/Audio/Mezcla).
# Unreal cerrado: UnrealEditor-Cmd <uproject> -run=pythonscript -script=<este archivo> -unattended -nosplash -nullrhi
import unreal

RUTA = "/Game/MundoMagico/Audio/Mezcla"
# nombre: (density, diffusion, gain, gain_hf, decay_time, decay_hf_ratio, reflections_gain, reflections_delay, late_gain, late_delay, air_absorption_gain_hf)
REVERBS = {
    "RV_Sala":    (0.6, 0.7, 0.32, 0.55, 0.75, 0.6, 0.35, 0.008, 0.9, 0.012, 0.99),   # cuarto chico, seco
    "RV_Pasillo": (1.0, 0.85, 0.42, 0.7, 2.6, 0.75, 0.55, 0.015, 1.4, 0.03, 0.99),    # pasillo largo de baldosa
    "RV_Exterior":(0.2, 0.3, 0.18, 0.4, 1.4, 0.5, 0.05, 0.03, 0.4, 0.05, 0.97),      # parque abierto, cola leve
}
herr = unreal.AssetToolsHelpers.get_asset_tools()
for nombre, v in REVERBS.items():
    ruta = RUTA + "/" + nombre
    if unreal.EditorAssetLibrary.does_asset_exist(ruta):
        r = unreal.load_asset(ruta)
    else:
        r = herr.create_asset(nombre, RUTA, unreal.ReverbEffect, unreal.ReverbEffectFactory())
    campos = ["density", "diffusion", "gain", "gain_hf", "decay_time", "decay_hf_ratio",
              "reflections_gain", "reflections_delay", "late_gain", "late_delay", "air_absorption_gain_hf"]
    for c, val in zip(campos, v):
        try:
            r.set_editor_property(c, val)
        except Exception as e:
            unreal.log_warning("REVERB_CAMPO %s %s %s" % (nombre, c, e))
    unreal.EditorAssetLibrary.save_loaded_asset(r, False)
    unreal.log("REVERB_OK %s" % ruta)
