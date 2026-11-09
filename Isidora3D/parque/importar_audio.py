import glob, os
import unreal

D = r"C:/Users/pansh/Desktop/MundoMagico_Project/Isidora3D/parque/audio/"
F = "/Game/MundoMagico/Audio/Terror"
tools = unreal.AssetToolsHelpers.get_asset_tools()
tasks = []
for f in sorted(glob.glob(D + "*.wav")):
    t = unreal.AssetImportTask()
    t.filename = f
    t.destination_path = F
    t.destination_name = os.path.basename(f)[:-4]
    t.automated = True
    t.replace_existing = True
    t.save = True
    tasks.append(t)
tools.import_asset_tasks(tasks)
a = unreal.load_asset(F + "/Ambiente_T_Hospital")
if a:
    a.set_editor_property('looping', True)
    unreal.EditorAssetLibrary.save_loaded_asset(a, False)
for t in tasks:
    unreal.log("AUDIO_OK %s" % list(t.imported_object_paths))
unreal.log("AUDIO_FIN")
