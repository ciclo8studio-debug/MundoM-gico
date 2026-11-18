# Importa T_LUT_HospitalHTML como LUT (grupo ColorLookupTable, sin mips, sin sRGB). Unreal cerrado (-run=pythonscript).
import unreal

t = unreal.AssetImportTask()
t.filename = "C:/Users/pansh/Desktop/MundoMagico_Project/MUSICA_PARA_UNREAL/T_LUT_HospitalHTML.png"
t.destination_path = "/Game/MundoMagico/Luces"
t.destination_name = "T_LUT_HospitalHTML"
t.automated = True
t.replace_existing = True
t.save = False
unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([t])
a = unreal.load_asset("/Game/MundoMagico/Luces/T_LUT_HospitalHTML")
a.set_editor_property("lod_group", unreal.TextureGroup.TEXTUREGROUP_COLOR_LOOKUP_TABLE)
a.set_editor_property("mip_gen_settings", unreal.TextureMipGenSettings.TMGS_NO_MIPMAPS)
a.set_editor_property("srgb", False)
a.set_editor_property("compression_settings", unreal.TextureCompressionSettings.TC_VECTOR_DISPLACEMENTMAP)
a.set_editor_property("filter", unreal.TextureFilter.TF_BILINEAR)
a.set_editor_property("address_x", unreal.TextureAddress.TA_CLAMP)
a.set_editor_property("address_y", unreal.TextureAddress.TA_CLAMP)
unreal.EditorAssetLibrary.save_loaded_asset(a, False)
unreal.log("LUT_OK %s" % a.get_path_name())
