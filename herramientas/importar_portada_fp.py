# Importa la portada web en 1a persona (T_Portada_FP) y la luz del mouse (T_LuzPortada) a /Game/MundoMagico/UI.
# Unreal cerrado: UnrealEditor-Cmd.exe <uproject> -run=pythonscript -script=<este archivo>
import unreal

herr = unreal.AssetToolsHelpers.get_asset_tools()
src = "C:/Users/pansh/Desktop/MundoMagico_Project/MUSICA_PARA_UNREAL/"
for archivo, nombre in (("T_Portada_FP.jpg", "T_Portada_FP"), ("T_LuzPortada.png", "T_LuzPortada")):
    t = unreal.AssetImportTask()
    t.filename = src + archivo
    t.destination_path = "/Game/MundoMagico/UI"
    t.destination_name = nombre
    t.automated = True
    t.replace_existing = True
    t.save = False
    herr.import_asset_tasks([t])
    a = unreal.load_asset("/Game/MundoMagico/UI/" + nombre)
    a.set_editor_property("lod_group", unreal.TextureGroup.TEXTUREGROUP_UI)
    a.set_editor_property("mip_gen_settings", unreal.TextureMipGenSettings.TMGS_NO_MIPMAPS)
    a.set_editor_property("never_stream", True)
    unreal.EditorAssetLibrary.save_loaded_asset(a, False)
    unreal.log("PORTADA_OK %s" % a.get_path_name())
