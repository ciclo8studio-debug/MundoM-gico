# Importa SFX_TV_Apagar y SFX_VHS_Play (sin loop, clase SC_SFX). Unreal cerrado (UnrealEditor-Cmd -run=pythonscript).
import unreal

herr = unreal.AssetToolsHelpers.get_asset_tools()
for nombre, vol in (("SFX_TV_Apagar", 0.9), ("SFX_VHS_Play", 0.6)):
    t = unreal.AssetImportTask()
    t.filename = "C:/Users/pansh/Desktop/MundoMagico_Project/MUSICA_PARA_UNREAL/%s.wav" % nombre
    t.destination_path = "/Game/MundoMagico/Audio/Terror"
    t.destination_name = nombre
    t.automated = True
    t.replace_existing = True
    t.save = False
    herr.import_asset_tasks([t])
    a = unreal.load_asset("/Game/MundoMagico/Audio/Terror/" + nombre)
    a.set_editor_property("looping", False)
    a.set_editor_property("volume", vol)
    a.set_editor_property("sound_class_object", unreal.load_asset("/Game/MundoMagico/Audio/Mezcla/SC_SFX"))
    unreal.EditorAssetLibrary.save_loaded_asset(a, False)
    unreal.log("TRANSICION_OK %s" % a.get_path_name())
