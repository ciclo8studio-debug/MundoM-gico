# Importa SFX_Latido.wav como SoundWave en loop, con la clase SC_SFX. Unreal cerrado (UnrealEditor-Cmd -run=pythonscript).
import unreal

t = unreal.AssetImportTask()
t.filename = r"C:/Users/pansh/Desktop/MundoMagico_Project/MUSICA_PARA_UNREAL/SFX_Latido.wav"
t.destination_path = "/Game/MundoMagico/Audio/Terror"
t.destination_name = "SFX_Latido"
t.automated = True
t.replace_existing = True
t.save = False
unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([t])
a = unreal.load_asset("/Game/MundoMagico/Audio/Terror/SFX_Latido")
a.set_editor_property("looping", True)
a.set_editor_property("volume", 1.0)
a.set_editor_property("sound_class_object", unreal.load_asset("/Game/MundoMagico/Audio/Mezcla/SC_SFX"))
unreal.EditorAssetLibrary.save_loaded_asset(a, False)
unreal.log("LATIDO_OK %s" % a.get_path_name())
