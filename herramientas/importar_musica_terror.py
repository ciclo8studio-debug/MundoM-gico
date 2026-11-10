# Reemplaza el audio de MUS_Menu por Musica_Terror_Menu.wav, conservando la Sound Class y el loop.
# Se ejecuta con UnrealEditor-Cmd -ExecutePythonScript (Unreal cerrado).
import unreal

SRC = r"C:/Users/pansh/Desktop/MundoMagico_Project/MUSICA_PARA_UNREAL/Musica_Terror_Menu.wav"
DST = "/Game/MundoMagico/Audio"
viejo = unreal.load_asset(DST + "/MUS_Menu")
clase = viejo.get_editor_property('sound_class_object') if viejo else None
unreal.log("MUSICA_CLASE %s" % clase)

t = unreal.AssetImportTask()
t.filename = SRC
t.destination_path = DST
t.destination_name = "MUS_Menu"
t.automated = True
t.replace_existing = True
t.save = False
unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([t])

a = unreal.load_asset(DST + "/MUS_Menu")
a.set_editor_property('looping', True)
a.set_editor_property('volume', 0.6)
if clase:
    a.set_editor_property('sound_class_object', clase)
unreal.EditorAssetLibrary.save_loaded_asset(a, False)
unreal.log("MUSICA_OK %s dur=%s" % (a.get_path_name(), a.get_editor_property('duration')))
