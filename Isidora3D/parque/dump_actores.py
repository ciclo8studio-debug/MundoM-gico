import json
import unreal

SK = ('ARB_', 'PQ_Arbol', 'PASTO', 'NUBE', 'LUNA', 'Luna', 'CIELO', 'NavMesh', 'PQ_Suelo', 'PQ_Reja', 'PQ_Camino', 'Floor', 'WorldSettings',
      'SkyAtmosphere', 'SkyLight', 'VolumetricCloud', 'SM_SkySphere', 'Exposicion', 'RecastNavMesh', 'Buoyancy', 'DefaultPhysics',
      'GameplayDebugger', 'AbstractNavData', 'Brush', 'ExponentialHeightFog', 'SND_')
unreal.EditorLoadingAndSavingUtils.load_map("/Game/MundoMagico/Maps/L_Pabellon4")
w = unreal.EditorLevelLibrary.get_editor_world()
out = []
for a in unreal.GameplayStatics.get_all_actors_of_class(w, unreal.Actor):
    l = a.get_actor_label()
    if l.startswith(SK):
        continue
    p = a.get_actor_location()
    if p.x == 0 and p.y == 0:
        continue
    out.append([round(p.x), round(p.y), round(p.z), l])
with open(r"C:/Users/pansh/Desktop/MundoMagico_Project/Isidora3D/parque/actores.json", "w") as f:
    json.dump(out, f)
unreal.log("DUMP_FIN %d" % len(out))
