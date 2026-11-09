# Arboles secos del diseno antiguo (grupos ARB_) como una malla cada uno. Uso: blender -b --python arboles_secos.py
import bpy, json, math, re
import numpy as np

DIR = r"C:/Users/pansh/Desktop/MundoMagico_Project/Isidora3D/parque/"
bpy.ops.wm.read_factory_settings(use_empty=True)
A = json.load(open(DIR + "arboles_viejos.json"))
for n, k in (("SM_Terr_Seco1", "1"), ("SM_Terr_Seco2", "6")):
    P = [a for a in A if re.match(r"ARB_%s_\d+" % k, a[0])]
    o = next(a for a in P if a[0].endswith("_00"))[3]
    V, F = [], []
    for a in P:
        c = np.array(a[3], float) - [o[0], o[1], 0]
        p, y = math.radians(a[4][0]), math.radians(a[4][1])
        u = np.array([-math.sin(p) * math.cos(y), -math.sin(p) * math.sin(y), math.cos(p)])
        e = np.cross(u, (1, 0, 0) if abs(u[0]) < .9 else (0, 1, 0))
        e /= np.linalg.norm(e)
        f = np.cross(u, e)
        r, h, seg = a[5][0] * 50, a[5][2] * 50, 8
        b = len(V)
        for q, rr in ((c - u * h, r), (c + u * h, r * .62)):
            for i in range(seg):
                t = 6.283185 * i / seg
                V.append(q + (e * math.cos(t) + f * math.sin(t)) * rr)
        for i in range(seg):
            j = (i + 1) % seg
            F.append((b + i, b + j, b + seg + j, b + seg + i))
    V = np.array(V)
    W = np.column_stack([V[:, 0] / 100, -V[:, 1] / 100, V[:, 2] / 100])
    me = bpy.data.meshes.new(n)
    me.from_pydata(W.tolist(), [], [tuple(reversed(x)) for x in F])
    me.update()
    l = me.uv_layers.new(name="UV")
    idx = np.zeros(len(me.loops), np.int32)
    me.loops.foreach_get("vertex_index", idx)
    uv = np.column_stack([np.arctan2(V[:, 1], V[:, 0]) / 3.1416, V[:, 2] / 200]).astype(np.float32)
    l.data.foreach_set("uv", uv[idx].ravel())
    me.polygons.foreach_set("use_smooth", [True] * len(me.polygons))
    me.update()
    ob = bpy.data.objects.new(n, me)
    bpy.context.collection.objects.link(ob)
    bpy.ops.object.select_all(action='DESELECT')
    ob.select_set(True)
    bpy.context.view_layer.objects.active = ob
    bpy.ops.export_scene.fbx(filepath=DIR + n + ".fbx", use_selection=True, object_types={'MESH'}, mesh_smooth_type='FACE', add_leaf_bones=False, bake_anim=False)
    print("OK", n, len(P), round(float(V[:, 2].max())))
    bpy.data.objects.remove(ob, do_unlink=True)
print("SECOS_FIN")
