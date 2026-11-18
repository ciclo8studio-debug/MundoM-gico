# Importa IES_Linterna (TextureLightProfile) y SFX_Pagina (SoundWave, clase SC_SFX). Unreal cerrado (-run=pythonscript).
import unreal

herr = unreal.AssetToolsHelpers.get_asset_tools()
src = "C:/Users/pansh/Desktop/MundoMagico_Project/MUSICA_PARA_UNREAL/"
for archivo, carpeta, nombre in (("IES_Linterna.ies", "/Game/MundoMagico/Luces", "IES_Linterna"),
                                 ("SFX_Pagina.wav", "/Game/MundoMagico/Audio/Terror", "SFX_Pagina")):
    t = unreal.AssetImportTask()
    t.filename = src + archivo
    t.destination_path = carpeta
    t.destination_name = nombre
    t.automated = True
    t.replace_existing = True
    t.save = False
    herr.import_asset_tasks([t])
    a = unreal.load_asset(carpeta + "/" + nombre)
    if nombre == "SFX_Pagina":
        a.set_editor_property("looping", False)
        a.set_editor_property("volume", 0.8)
        a.set_editor_property("sound_class_object", unreal.load_asset("/Game/MundoMagico/Audio/Mezcla/SC_SFX"))
    unreal.EditorAssetLibrary.save_loaded_asset(a, False)
    unreal.log("LINTERNA_OK %s %s" % (a.get_class().get_name(), a.get_path_name()))
