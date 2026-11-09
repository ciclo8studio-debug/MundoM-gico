import glob, os
import unreal

D = r"C:/Users/pansh/Desktop/MundoMagico_Project/Isidora3D/parque/"
F = "/Game/MundoMagico/Parque"
EM = {'Neon_Rosa': (6, .35, 2.2), 'Neon_Cian': (.4, 3.2, 6), 'Neon_Turquesa': (.5, 5, 3.6), 'Neon_Rojo': (7, .3, .8), 'Neon_Dorado': (6, 3.2, .7), 'Neon_Verde': (.6, 5, 1.2), 'Bombilla_Parque': (5, 3, 1.3), 'Ventana_Calida': (2.2, 1.2, .45)}
SO = {'Rot_Oxido': (.13, .06, .035), 'Rot_Letra': (.32, .27, .2), 'Rot_Madera': (.1, .065, .04), 'Rot_Rojo': (.3, .035, .03), 'Rot_Crema': (.55, .5, .4), 'Rot_Negro': (.012, .012, .014), 'Rot_Metal': (.07, .07, .08), 'Rot_Asiento': (.05, .02, .02), 'Auto_Rojo': (.45, .03, .03), 'Auto_Amarillo': (.55, .38, .03), 'Auto_Turquesa': (.04, .3, .3), 'Auto_Morado': (.2, .05, .3), 'Auto_Naranja': (.6, .17, .03), 'Auto_Crema': (.6, .58, .5), 'Tren_Verde': (.04, .2, .08)}
tools = unreal.AssetToolsHelpers.get_asset_tools()
tasks = []
for f in sorted(glob.glob(D + "SM_*.fbx")):
    if unreal.EditorAssetLibrary.does_asset_exist(F + "/" + os.path.basename(f)[:-4]):
        continue
    t = unreal.AssetImportTask()
    t.filename = f
    t.destination_path = F
    t.destination_name = os.path.basename(f)[:-4]
    t.automated = True
    t.replace_existing = True
    t.save = True
    o = unreal.FbxImportUI()
    o.import_mesh = True
    o.import_materials = False
    o.import_textures = False
    o.import_animations = False
    o.mesh_type_to_import = unreal.FBXImportType.FBXIT_STATIC_MESH
    o.static_mesh_import_data.combine_meshes = True
    t.options = o
    tasks.append(t)
tools.import_asset_tasks(tasks)
for t in tasks:
    unreal.log("IMPORTADO %s" % list(t.imported_object_paths))

fac = unreal.MaterialInstanceConstantFactoryNew()
lib = unreal.MaterialEditingLibrary
for group, parent in ((EM, "/Game/MundoMagico/Materials/M_Emisivo"), (SO, "/Engine/BasicShapes/BasicShapeMaterial")):
    pm = unreal.load_asset(parent)
    for n, c in group.items():
        p = F + "/MI_" + n
        a = unreal.load_asset(p) if unreal.EditorAssetLibrary.does_asset_exist(p) else tools.create_asset("MI_" + n, F, unreal.MaterialInstanceConstant, fac)
        lib.set_material_instance_parent(a, pm)
        lib.set_material_instance_vector_parameter_value(a, "Color", unreal.LinearColor(c[0], c[1], c[2], 1))
        unreal.EditorAssetLibrary.save_loaded_asset(a)
unreal.log("IMPORTAR_FIN")
