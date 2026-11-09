import unreal

EA = unreal.EditorAssetLibrary
lib = unreal.EditorStaticMeshLibrary
D = "/Game/MundoMagico/Arboles/"
S = "/Game/GV_FreeShrubsPack/Meshes/Shrubs/Wind/Shrub_%s/GV_Vol7_Shrub_%s_full_type1"
for k in "ABCI":
    p = D + "SM_Arbol_" + k
    if not EA.does_asset_exist(p):
        EA.duplicate_asset(S % (k, k), p)
    m = unreal.load_asset(p)
    ns = m.get_editor_property('nanite_settings')
    ns.enabled = False
    m.set_editor_property('nanite_settings', ns)
    o = unreal.EditorScriptingMeshReductionOptions()
    o.auto_compute_lod_screen_size = False
    R = unreal.EditorScriptingMeshReductionSettings
    o.reduction_settings = [R(1.0, 1.0), R(.4, .3), R(.12, .12), R(.03, .04)]
    n = lib.set_lods(m, o)
    b = lib.get_lod_build_settings(m, 0)
    b.distance_field_resolution_scale = 0.0
    lib.set_lod_build_settings(m, 0, b)
    lib.remove_collisions(m)
    EA.save_loaded_asset(m, False)
    unreal.log("ARB_OK %s lods=%s verts=%s" % (k, n, [lib.get_number_verts(m, i) for i in range(lib.get_lod_count(m))]))
tools = unreal.AssetToolsHelpers.get_asset_tools()
T = []
for n in ("SM_Terr_Seco1", "SM_Terr_Seco2"):
    t = unreal.AssetImportTask()
    t.filename = r"C:/Users/pansh/Desktop/MundoMagico_Project/Isidora3D/parque/" + n + ".fbx"
    t.destination_path = "/Game/MundoMagico/Arboles"
    t.destination_name = n
    t.automated = True
    t.replace_existing = True
    t.save = False
    u = unreal.FbxImportUI()
    u.import_mesh = True
    u.import_materials = False
    u.import_textures = False
    u.import_animations = False
    u.mesh_type_to_import = unreal.FBXImportType.FBXIT_STATIC_MESH
    u.static_mesh_import_data.combine_meshes = True
    u.static_mesh_import_data.auto_generate_collision = False
    u.static_mesh_import_data.generate_lightmap_u_vs = False
    t.options = u
    T.append(t)
tools.import_asset_tasks(T)
for n in ("SM_Terr_Seco1", "SM_Terr_Seco2"):
    m = unreal.load_asset(D + n)
    lib.remove_collisions(m)
    EA.save_loaded_asset(m, False)
unreal.log("ARB_FIN")
