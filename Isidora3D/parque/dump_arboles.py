import json
import unreal

unreal.EditorLoadingAndSavingUtils.load_map("/Game/MundoMagico/Maps/L_RespaldoArboles")
w = unreal.EditorLevelLibrary.get_editor_world()
out = []
for a in unreal.GameplayStatics.get_all_actors_of_class(w, unreal.StaticMeshActor):
    l = a.get_actor_label()
    if not (l.startswith('ARB_') or l.startswith('PQ_Arbol')):
        continue
    c = a.static_mesh_component
    p, r, s = a.get_actor_location(), a.get_actor_rotation(), a.get_actor_scale3d()
    m = c.get_material(0)
    out.append([l, c.static_mesh.get_name() if c.static_mesh else None, m.get_name() if m else None, [round(p.x, 1), round(p.y, 1), round(p.z, 1)], [round(r.pitch, 1), round(r.yaw, 1), round(r.roll, 1)], [round(s.x, 3), round(s.y, 3), round(s.z, 3)]])
with open(r"C:/Users/pansh/Desktop/MundoMagico_Project/Isidora3D/parque/arboles_viejos.json", "w") as f:
    json.dump(out, f)
unreal.log("ARB_FIN %d" % len(out))
