# Crea ATT_Puntual (atenuacion espacial) y la asigna a los sonidos que salen de un lugar concreto.
# Los ambientes generales (viento, lluvia, musica) y los pasos del jugador no se tocan.
# Unreal cerrado: UnrealEditor-Cmd <uproject> -run=pythonscript -script=<este archivo> -unattended -nosplash -nullrhi
import unreal, re, json, os

RUTA, NOMBRE = "/Game/MundoMagico/Audio/Mezcla", "ATT_Puntual"
if unreal.EditorAssetLibrary.does_asset_exist(RUTA + "/" + NOMBRE):
    att = unreal.load_asset(RUTA + "/" + NOMBRE)
else:
    att = unreal.AssetToolsHelpers.get_asset_tools().create_asset(NOMBRE, RUTA, unreal.SoundAttenuation, unreal.SoundAttenuationFactory())

s = att.get_editor_property("attenuation")
s.set_editor_property("attenuate", True)
s.set_editor_property("spatialize", True)
s.set_editor_property("distance_algorithm", unreal.AttenuationDistanceModel.NATURAL_SOUND)
s.set_editor_property("attenuation_shape", unreal.AttenuationShape.SPHERE)
s.set_editor_property("attenuation_shape_extents", unreal.Vector(250, 0, 0))
s.set_editor_property("falloff_distance", 3500.0)
s.set_editor_property("non_spatialized_radius_start", 120.0)
s.set_editor_property("non_spatialized_radius_end", 40.0)
s.set_editor_property("attenuate_with_lpf", True)
s.set_editor_property("lpf_radius_min", 600.0)
s.set_editor_property("lpf_radius_max", 3500.0)
s.set_editor_property("lpf_frequency_at_min", 20000.0)
s.set_editor_property("lpf_frequency_at_max", 2500.0)
att.set_editor_property("attenuation", s)
unreal.EditorAssetLibrary.save_loaded_asset(att, False)

PUNTUAL = re.compile(r"Shake_Metal|ShakingLamp|SparkLoop|SteamVent|MetalPanel_|SFX_T_(Carrera|Chillido|Golpe)|SFX_(Golpe|Vidrio|Pedestal)|WoodenCrate|_Door|Key_Insertion", re.I)
lista = json.load(open(os.path.join(unreal.Paths.project_saved_dir(), "sonidos_todos.json"), encoding="utf8"))
n = 0
for ruta in lista:
    if not PUNTUAL.search(ruta):
        continue
    a = unreal.load_asset(ruta)
    if a is None:
        continue
    a.set_editor_property("attenuation_settings", att)
    unreal.EditorAssetLibrary.save_loaded_asset(a, False)
    n += 1
unreal.log("ESPACIAL_OK %d sonidos con %s" % (n, att.get_path_name()))
