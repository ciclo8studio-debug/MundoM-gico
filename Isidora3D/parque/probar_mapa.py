import unreal

unreal.EditorLoadingAndSavingUtils.load_map("/Game/MundoMagico/Maps/L_Pabellon4")
w = unreal.EditorLevelLibrary.get_editor_world()
PT = {'cerro': (6500, -13300), 'avenida': (0, -9000), 'pasillo_viejo': (0, -2000), 'pasillo_nuevo': (0, 1000), 'reanimacion': (-600, 500), 'farmacia': (600, 1950), 'sala': (0, 3100),
      'patio_puerta': (0, -3800), 'noria': (5500, -9000), 'casa_puerta': (8745, -15100), 'teatro': (-5650, -12500), 'bodega': (-7300, -5200), 'chocones': (4400, -17400)}
for n, (x, y) in PT.items():
    r = unreal.SystemLibrary.line_trace_single(w, unreal.Vector(x, y, 250 if abs(x) < 1100 and y > -3500 else 2500), unreal.Vector(x, y, -600), unreal.TraceTypeQuery.TRACE_TYPE_QUERY1, False, [], unreal.DrawDebugTrace.NONE, True)
    if r:
        t = r.to_tuple()
        unreal.log("TRAZO %s z=%d %s" % (n, t[4].z, t[9].get_actor_label() if t[9] else None))
    else:
        unreal.log("TRAZO %s SIN_SUELO" % n)
# Muro invisible: desde dentro hacia fuera por el oeste y por el embudo
for n, a, b in (('limite_oeste', (-8800, -7000, 120), (-11000, -7000, 120)), ('limite_embudo', (300, -3700, 120), (1500, -3700, 120))):
    r = unreal.SystemLibrary.line_trace_single(w, unreal.Vector(*a), unreal.Vector(*b), unreal.TraceTypeQuery.TRACE_TYPE_QUERY1, False, [], unreal.DrawDebugTrace.NONE, True)
    unreal.log("TRAZO %s %s" % (n, (r.to_tuple()[9].get_actor_label() if r else 'NADA')))
nav = unreal.NavigationSystemV1.get_navigation_system(w)
unreal.log("TRAZO navsys %s" % nav)
for n, a, b in (('nav_casa', (0, -5000, 10), (8745, -15000, 10)), ('nav_hosp', (0, -2300, 10), (0, 1200, 10)), ('nav_entrada', (0, -3000, 10), (0, -5000, 10))):
    try:
        p = unreal.NavigationSystemV1.find_path_to_location_synchronously(w, unreal.Vector(*a), unreal.Vector(*b))
        pts = p.path_points if p else []
        unreal.log("TRAZO %s puntos=%d fin=%s" % (n, len(pts), pts[-1] if pts else None))
    except Exception as e:
        unreal.log("TRAZO %s ERR %s" % (n, e))
unreal.log("TRAZO_FIN")
