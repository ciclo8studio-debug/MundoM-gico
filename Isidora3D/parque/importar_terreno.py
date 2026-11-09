import unreal

D = r"C:/Users/pansh/Desktop/MundoMagico_Project/Isidora3D/parque/"
F = "/Game/MundoMagico/Parque"
N = ['Parque', 'Cerros', 'Montes', 'Camino', 'Reja', 'Limite', 'Tronco', 'Ramas', 'Copas', 'Pasto']
COL = ('Parque', 'Limite', 'Tronco')  # colision compleja; el resto sin colision
tools = unreal.AssetToolsHelpers.get_asset_tools()
tasks = []
for n in N:
    t = unreal.AssetImportTask()
    t.filename = D + "SM_Terr_" + n + ".fbx"
    t.destination_path = F
    t.destination_name = "SM_Terr_" + n
    t.automated = True
    t.replace_existing = True
    t.save = False
    o = unreal.FbxImportUI()
    o.import_mesh = True
    o.import_materials = False
    o.import_textures = False
    o.import_animations = False
    o.mesh_type_to_import = unreal.FBXImportType.FBXIT_STATIC_MESH
    d = o.static_mesh_import_data
    d.combine_meshes = True
    d.auto_generate_collision = False
    d.generate_lightmap_u_vs = False
    t.options = o
    tasks.append(t)
tools.import_asset_tasks(tasks)
sub = unreal.EditorStaticMeshLibrary
lib = unreal.EditorStaticMeshLibrary
for n in N:
    p = F + "/SM_Terr_" + n
    m = unreal.load_asset(p)
    if not m:
        unreal.log_error("NO_IMPORTO " + n)
        continue
    b = sub.get_lod_build_settings(m, 0)
    b.distance_field_resolution_scale = 0.0
    sub.set_lod_build_settings(m, 0, b)
    bs = m.get_editor_property('body_setup')
    if n in COL:
        if bs:
            bs.set_editor_property('collision_trace_flag', unreal.CollisionTraceFlag.CTF_USE_COMPLEX_AS_SIMPLE)
            m.post_edit_change() if hasattr(m, 'post_edit_change') else None
    else:
        lib.remove_collisions(m)
        if bs:
            bs.set_editor_property('collision_trace_flag', unreal.CollisionTraceFlag.CTF_USE_SIMPLE_AS_COMPLEX)
    unreal.EditorAssetLibrary.save_loaded_asset(m, False)
    unreal.log("TERR_OK %s bs=%s flag=%s" % (n, bool(bs), bs.get_editor_property("collision_trace_flag") if bs else None))
# Material de follaje
fac = unreal.MaterialInstanceConstantFactoryNew()
mlib = unreal.MaterialEditingLibrary
pm = unreal.load_asset("/Engine/BasicShapes/BasicShapeMaterial")
for n, c in {'Follaje': (.018, .038, .022), 'Cerro': (.02, .03, .018)}.items():
    p = F + "/MI_" + n
    a = unreal.load_asset(p) if unreal.EditorAssetLibrary.does_asset_exist(p) else tools.create_asset("MI_" + n, F, unreal.MaterialInstanceConstant, fac)
    mlib.set_material_instance_parent(a, pm)
    mlib.set_material_instance_vector_parameter_value(a, "Color", unreal.LinearColor(c[0], c[1], c[2], 1))
    unreal.EditorAssetLibrary.save_loaded_asset(a)
unreal.log("TERR_FIN")
