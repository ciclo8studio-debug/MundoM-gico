# Crea SM_Silencio: SoundMix que baja Ambiente y Musica casi a cero (silencio antes del susto).
# Unreal cerrado: UnrealEditor-Cmd <uproject> -run=pythonscript -script=<este archivo> -unattended -nosplash -nullrhi
import unreal

ruta = "/Game/MundoMagico/Audio/Mezcla"
nombre = "SM_Silencio"
if unreal.EditorAssetLibrary.does_asset_exist(ruta + "/" + nombre):
    mix = unreal.load_asset(ruta + "/" + nombre)
else:
    mix = unreal.AssetToolsHelpers.get_asset_tools().create_asset(nombre, ruta, unreal.SoundMix, unreal.SoundMixFactory())

def ajuste(clase, vol):
    a = unreal.SoundClassAdjuster()
    a.set_editor_property("sound_class_object", unreal.load_asset(clase))
    a.set_editor_property("volume_adjuster", vol)
    a.set_editor_property("pitch_adjuster", 1.0)
    a.set_editor_property("apply_to_children", True)
    return a

mix.set_editor_property("sound_class_effects", [
    ajuste("/Game/MundoMagico/Audio/Mezcla/SC_Ambiente", 0.03),
    ajuste("/Engine/EngineSounds/Music", 0.03),
])
mix.set_editor_property("fade_in_time", 0.35)
mix.set_editor_property("fade_out_time", 2.5)
mix.set_editor_property("duration", -1.0)
unreal.EditorAssetLibrary.save_loaded_asset(mix, False)
unreal.log("SILENCIO_OK %s" % mix.get_path_name())
