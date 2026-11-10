# Reemplaza la imagen de T_PortadaHTML (portada del menu) por T_PortadaCine.png, sin cambiar el nombre del asset.
# Unreal cerrado: UnrealEditor-Cmd <uproject> -run=pythonscript -script=<este archivo> -unattended -nosplash -nullrhi
import unreal

SRC = r"C:/Users/pansh/Documents/Unreal Projects/MUNDO_MÁGICO/Fuentes/UI/T_PortadaCine.png"
t = unreal.AssetImportTask()
t.filename = SRC
t.destination_path = "/Game/MundoMagico/UI"
t.destination_name = "T_PortadaHTML"
t.automated = True
t.replace_existing = True
t.save = True
unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([t])
a = unreal.load_asset("/Game/MundoMagico/UI/T_PortadaHTML")
a.set_editor_property("lod_group", unreal.TextureGroup.TEXTUREGROUP_UI)
a.set_editor_property("mip_gen_settings", unreal.TextureMipGenSettings.TMGS_NO_MIPMAPS)
unreal.EditorAssetLibrary.save_loaded_asset(a, False)
unreal.log("PORTADA_OK %s %sx%s" % (a.get_path_name(), a.blueprint_get_size_x(), a.blueprint_get_size_y()))
