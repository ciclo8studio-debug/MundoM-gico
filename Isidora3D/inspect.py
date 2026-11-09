import bpy
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=r"C:/Users/pansh/Desktop/MundoMagico_Project/Isidora3D/SKM_Quinn_Simple.fbx")
for o in bpy.data.objects:
    print("OBJ", o.type, o.name, tuple(round(v, 3) for v in o.dimensions), tuple(round(v, 3) for v in o.scale))
    if o.type == 'MESH':
        print("  verts", len(o.data.vertices), "groups", len(o.vertex_groups), "mats", [m.name for m in o.data.materials])
    if o.type == 'ARMATURE':
        for b in o.data.bones:
            if not any(s in b.name for s in ('index', 'middle', 'pinky', 'ring', 'thumb', 'ik_', 'twist', 'corrective')):
                h = o.matrix_world @ b.head_local
                print("  BONE", b.name, b.parent.name if b.parent else '-', tuple(round(v, 3) for v in h))
