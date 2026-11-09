# Genera las piezas tematicas del parque (una malla por material). Uso: blender -b --python build_parque.py
import bpy, bmesh, math, os, random
from mathutils import Vector

DIR = r"C:/Users/pansh/Desktop/MundoMagico_Project/Isidora3D/parque/"
FONT = r"C:/Users/pansh/Desktop/MundoMagico_Project/Fuentes_descarga/IMFeENrm28P.ttf"
random.seed(4)
bpy.ops.wm.read_factory_settings(use_empty=True)
col = bpy.context.collection


def clear():
    for o in list(bpy.data.objects):
        bpy.data.objects.remove(o, do_unlink=True)


def new(name, bm, smooth=True):
    me = bpy.data.meshes.new(name)
    bm.to_mesh(me)
    bm.free()
    o = bpy.data.objects.new(name, me)
    col.objects.link(o)
    for p in me.polygons:
        p.use_smooth = smooth
    return o


def export(name, objs):
    """Une objs en una malla y la exporta a FBX."""
    bpy.ops.object.select_all(action='DESELECT')
    for o in objs:
        o.select_set(True)
    bpy.context.view_layer.objects.active = objs[0]
    if len(objs) > 1:
        bpy.ops.object.join()
    o = bpy.context.view_layer.objects.active
    o.name = name
    o.data.name = name
    bpy.ops.object.select_all(action='DESELECT')
    o.select_set(True)
    bpy.ops.export_scene.fbx(filepath=DIR + name + ".fbx", use_selection=True, object_types={'MESH'},
                             mesh_smooth_type='FACE', add_leaf_bones=False, bake_anim=False)
    print("OK", name, len(o.data.vertices))
    bpy.data.objects.remove(o, do_unlink=True)


def caja(c, s, rot=0.0, bevel=0.0):
    bm = bmesh.new()
    bmesh.ops.create_cube(bm, size=1)
    if bevel:
        bmesh.ops.bevel(bm, geom=list(bm.edges), offset=bevel / max(s), segments=2, affect='EDGES')
    cr, sr = math.cos(rot), math.sin(rot)
    for v in bm.verts:
        x, y, z = v.co.x * s[0], v.co.y * s[1], v.co.z * s[2]
        v.co = Vector((x * cr - y * sr + c[0], x * sr + y * cr + c[1], z + c[2]))
    return new("c", bm, False)


def tubo(pts, r, seg=8, smooth=True, r2=None):
    bm = bmesh.new()
    rows = []
    pts = [Vector(p) for p in pts]
    for i, p in enumerate(pts):
        d = (pts[min(i + 1, len(pts) - 1)] - pts[max(i - 1, 0)]).normalized()
        a = d.orthogonal().normalized()
        b = d.cross(a)
        rr = r if r2 is None else r + (r2 - r) * i / (len(pts) - 1)
        rows.append([bm.verts.new(p + (a * math.cos(2 * math.pi * j / seg) + b * math.sin(2 * math.pi * j / seg)) * rr) for j in range(seg)])
    for i in range(len(rows) - 1):
        for j in range(seg):
            bm.faces.new((rows[i][j], rows[i][(j + 1) % seg], rows[i + 1][(j + 1) % seg], rows[i + 1][j]))
    bm.faces.new(rows[0][::-1])
    bm.faces.new(rows[-1])
    return new("t", bm, smooth)


def esferas(pts, r, sub=1):
    bm = bmesh.new()
    for p in pts:
        ret = bmesh.ops.create_icosphere(bm, subdivisions=sub, radius=r)
        for v in ret['verts']:
            v.co += Vector(p)
    return new("b", bm, True)


def anillo(R, r, center, plano='XY', seg=96, tseg=8):
    pts = []
    for i in range(seg + 1):
        a = 2 * math.pi * i / seg
        if plano == 'XY':
            pts.append((center[0] + R * math.cos(a), center[1] + R * math.sin(a), center[2]))
        else:
            pts.append((center[0] + R * math.cos(a), center[1], center[2] + R * math.sin(a)))
    return tubo(pts, r, tseg)


# ------------------------- ENTRADA -------------------------
def arc(x, W=13.0, base=11.6, h=1.5):
    return base + h * (1 - (x / W) ** 2)


def entrada():
    # postes oxidados, levemente torcidos, y celosia
    objs = []
    for x in (-12, -8, -3, 3, 8, 12):
        lean = random.uniform(-.12, .12)
        objs.append(tubo([(x, 0, 0), (x + lean * .5, 0, 7.5), (x + lean, .05, 15.6)], .13, 10, True, .09))
        objs.append(tubo([(x + lean, .05, 15.6), (x + lean, .05, 16.0)], .05, 6))
    for z0 in (0.0, 2.95):
        pts = [(-13.2 + i * .66, .12, arc(-13.2 + i * .66) - 0.25 + z0) for i in range(41)]
        objs.append(tubo(pts, .06, 6))
    for i in range(40):
        x0, x1 = -13.2 + i * .66, -13.2 + (i + 1) * .66
        a, b = (0.0, 2.95) if i % 2 == 0 else (2.95, 0.0)
        objs.append(tubo([(x0, .12, arc(x0) - .25 + a), (x1, .12, arc(x1) - .25 + b)], .028, 5))
    export("SM_Ent_Hierro", objs)

    # letras
    def texto(fill):
        cu = bpy.data.curves.new("txt", 'FONT')
        cu.body = "MUNDO MÁGICO"
        cu.font = bpy.data.fonts.load(FONT)
        cu.size = 3.5
        cu.align_x = 'CENTER'
        cu.space_character = 1.04
        cu.resolution_u = 2
        if fill:
            cu.extrude = .16
        else:
            cu.fill_mode = 'NONE'
            cu.bevel_depth = .035
            cu.bevel_resolution = 0
        o = bpy.data.objects.new("txt", cu)
        col.objects.link(o)
        bpy.context.view_layer.objects.active = o
        o.select_set(True)
        bpy.ops.object.convert(target='MESH')
        o = bpy.context.view_layer.objects.active
        bpy.ops.object.select_all(action='DESELECT')
        return o

    def curvar(o, y):
        xs = [v.co.x for v in o.data.vertices]
        k = 24.6 / (max(xs) - min(xs))
        zs = [v.co.y for v in o.data.vertices]
        zc = (max(zs) + min(zs)) / 2
        for v in o.data.vertices:
            x = v.co.x * k
            z = (v.co.y - zc) * k * .92
            d = v.co.z
            v.co = Vector((x, y - d, arc(x) + 1.2 + z))

    o = texto(True)
    curvar(o, -.08)
    export("SM_Ent_Letras", [o])
    o = texto(False)
    curvar(o, -.33)
    export("SM_Ent_NeonLetras", [o])

    # barra de neon y bombillas en arco
    pts = [(-12.8 + i * .4, -.2, arc(-12.8 + i * .4) - .42) for i in range(65)]
    export("SM_Ent_NeonBarra", [tubo(pts, .05, 6)])
    b = [(-12.9 + i * .43, -.24, arc(-12.9 + i * .43) + 2.95) for i in range(61)]
    b += [(-12.9 + i * .43, -.24, arc(-12.9 + i * .43) - .62) for i in range(61)]
    export("SM_Ent_Bombillas", [esferas(b, .075)])

    # rejas con puntas entre poste central y boleteria
    objs = []
    for s in (-1, 1):
        for i in range(29):
            x = s * (3.1 + i * .15)
            objs.append(tubo([(x, 0, 0), (x, 0, 2.7)], .018, 5, False))
            objs.append(tubo([(x, 0, 2.7), (x, 0, 2.95)], .03, 5, False, .001))
        for z in (.35, 1.5, 2.55):
            objs.append(caja((s * 5.25, 0, z), (4.45, .03, .05)))
    export("SM_Ent_Reja", objs)

    # porton abierto (dos hojas, se instancia)
    objs = []
    for i in range(16):
        x = .08 + i * .17
        objs.append(tubo([(x, 0, .05), (x, 0, 2.6 + .25 * math.sin(i / 15 * math.pi))], .02, 5, False))
    for z in (.2, 1.3, 2.45):
        objs.append(caja((1.4, 0, z), (2.8, .04, .06)))
    export("SM_Ent_Porton", objs)


def boleteria():
    W, D, H = 4.2, 2.6, 3.1
    objs = []
    n = 26
    for i in range(n):  # tablas verticales al frente y atras
        x = -W / 2 + (i + .5) * W / n
        if abs(x) > .75:
            objs.append(caja((x, -D / 2, H / 2 - random.uniform(0, .04)), (W / n * .93, .05, H)))
        elif True:
            objs.append(caja((x, -D / 2, .55), (W / n * .93, .05, 1.1)))
            objs.append(caja((x, -D / 2, H - .3), (W / n * .93, .05, .6)))
        objs.append(caja((x, D / 2, H / 2), (W / n * .93, .05, H)))
    for s in (-1, 1):
        for i in range(14):
            y = -D / 2 + (i + .5) * D / 14
            objs.append(caja((s * W / 2, y, H / 2), (.05, D / 14 * .93, H)))
    objs.append(caja((0, -D / 2 - .18, 1.12), (1.8, .4, .07)))  # mostrador
    objs.append(caja((0, 0, .03), (W, D, .06)))
    export("SM_Bol_Madera", objs)

    objs = [caja((0, 0, H + .22), (W + .9, D + .9, .12), 0, .04)]
    for i in range(9):
        objs.append(tubo([(-.75 + i * .1875, -D / 2 - .02, 1.1), (-.75 + i * .1875, -D / 2 - .02, H - .6)], .012, 5, False))
    for s in (-1, 1):
        objs.append(tubo([(s * (W / 2 + .3), -D / 2 - .35, 0), (s * (W / 2 + .3), -D / 2 - .35, H + .2)], .06, 8))
    export("SM_Bol_Metal", objs)

    objs = [caja((0, -D / 2 - .05, H - .02), (W + .3, .08, .75), 0, .02)]
    for s in (-1, 1):
        objs.append(caja((s * 1.45, -D / 2 - .04, 1.3), (.75, .03, 1.2)))
    export("SM_Bol_Panel", objs)
    pts = [(-W / 2 + i * .3, -D / 2 - .13, H + .42) for i in range(15)]
    export("SM_Bol_Bombillas", [esferas(pts, .06)])


# ------------------------- AVENIDA -------------------------
def guirnalda():
    export("SM_Gui_Poste", [tubo([(0, 0, 0), (0, 0, 6.6)], .075, 8, True, .05), tubo([(0, 0, 6.3), (.5, 0, 6.3)], .03, 6),
                            caja((0, 0, .25), (.3, .3, .5), 0, .03)])
    L = 10.4
    pts = [(-L / 2 + L * i / 40, 0, 6.25 - 1.15 * math.sin(math.pi * i / 40)) for i in range(41)]
    export("SM_Gui_Cable", [tubo(pts, .012, 5)])
    b = [(p[0], p[1], p[2] - .1) for p in pts[1:-1]]
    export("SM_Gui_Bombillas", [esferas(b, .075)])


def kiosco():
    W, D = 2.5, 2.1
    objs = [caja((0, .3, .55), (W, D - .7, 1.1), 0, .02), caja((0, D / 2 - .05, 1.5), (W, .1, 3.0)),
            caja((-W / 2 + .04, .3, 1.5), (.08, D - .7, 3.0)), caja((W / 2 - .04, .3, 1.5), (.08, D - .7, 3.0)),
            caja((0, -.42, 1.13), (W + .2, .34, .07), 0, .02)]
    for i in range(12):
        objs.append(caja((-W / 2 + (i + .5) * W / 12, -.4 + .02, .55), (W / 12 * .9, .03, 1.05)))
    export("SM_Kio_Madera", objs)
    rojo, crema = [], []
    n = 12
    for i in range(n):
        x0 = -W / 2 - .15 + i * (W + .3) / n
        x1 = x0 + (W + .3) / n
        bm = bmesh.new()
        v = [bm.verts.new(p) for p in ((x0, .95, 3.05), (x1, .95, 3.05), (x1, -1.4, 2.5), (x0, -1.4, 2.5))]
        bm.faces.new(v)
        v2 = [bm.verts.new(p) for p in ((x0, -1.4, 2.5), (x1, -1.4, 2.5), ((x0 + x1) / 2, -1.42, 2.22))]
        bm.faces.new(v2)
        ret = bmesh.ops.solidify(bm, geom=list(bm.faces), thickness=.02)
        (rojo if i % 2 == 0 else crema).append(new("a", bm, False))
    export("SM_Kio_ToldoRojo", rojo)
    export("SM_Kio_ToldoCrema", crema)
    export("SM_Kio_Letrero", [caja((0, .5, 3.45), (W + .1, .1, .7), 0, .02), caja((-1.0, -1.3, 1.3), (.06, .06, 2.6)), caja((1.0, -1.3, 1.3), (.06, .06, 2.6))])
    pts = [(-W / 2 + i * .25, -1.34, 2.4) for i in range(11)]
    export("SM_Kio_Bombillas", [esferas(pts, .05)])


# ------------------------- AUTOS CHOCONES -------------------------
def auto():
    bm = bmesh.new()
    bmesh.ops.create_cube(bm, size=1)
    bmesh.ops.subdivide_edges(bm, edges=list(bm.edges), cuts=3, use_grid_fill=True)
    for v in bm.verts:
        x, y, z = v.co
        # forma de bote: frente redondo, respaldo alto
        w = 1.0 - .28 * max(0, -y * 2) ** 2
        h = .55 + .55 * max(0, y * 2)
        v.co = Vector((x * 1.25 * w, y * 2.0, .3 + (z + .5) * h * .62))
    bmesh.ops.bevel(bm, geom=list(bm.edges), offset=.05, segments=2, affect='EDGES')
    cuerpo = new("cu", bm, True)
    export("SM_Auto_Cuerpo", [cuerpo])
    pts = []
    for i in range(49):
        a = 2 * math.pi * i / 48
        pts.append((math.cos(a) * .8, math.sin(a) * 1.16, .25))
    export("SM_Auto_Goma", [tubo(pts, .12, 8)])
    export("SM_Auto_Asiento", [caja((0, .3, .72), (.85, .55, .16), 0, .05), caja((0, .62, 1.0), (.85, .16, .65), 0, .05), caja((0, -.3, .62), (.8, .5, .2), 0, .04)])
    export("SM_Auto_Antena", [tubo([(0, .78, 1.0), (0, .78, 3.9)], .02, 6), esferas([(0, .78, 3.95)], .07), tubo([(0, -.25, .75), (0, -.25, .98)], .02, 6), anillo(.16, .018, (0, -.25, 1.0))])
    export("SM_Auto_Foco", [esferas([(-.32, -1.14, .55), (.32, -1.14, .55)], .08)])


def pista(tag, w, d, z):
    """Marco de neon y bombillas para el techo de una pista (w x d en metros)."""
    rosa = [caja((0, -d / 2, z), (w, .1, .1)), caja((-w / 2, 0, z), (.1, d, .1))]
    cian = [caja((0, d / 2, z), (w, .1, .1)), caja((w / 2, 0, z), (.1, d, .1))]
    export("SM_Pista%s_NeonRosa" % tag, rosa)
    export("SM_Pista%s_NeonCian" % tag, cian)
    b = []
    n, m = int(w / .55), int(d / .55)
    for i in range(n + 1):
        for s in (-1, 1):
            b.append((-w / 2 + i * w / n, s * (d / 2 + .12), z - .18))
    for i in range(m + 1):
        for s in (-1, 1):
            b.append((s * (w / 2 + .12), -d / 2 + i * d / m, z - .18))
    export("SM_Pista%s_Bombillas" % tag, [esferas(b, .07)])


# ------------------------- NEONES ATRACCIONES -------------------------
def neones():
    export("SM_Carr_Neon", [anillo(6.62, .05, (0, 0, 6.15)), anillo(5.0, .04, (0, 0, 8.3))])
    b = [(6.75 * math.cos(2 * math.pi * i / 64), 6.75 * math.sin(2 * math.pi * i / 64), 5.95) for i in range(64)]
    export("SM_Carr_Bombillas", [esferas(b, .08)])
    export("SM_Noria_Neon", [anillo(12.75, .07, (0, -.75, 0), 'XZ', 128), anillo(12.75, .07, (0, .75, 0), 'XZ', 128), anillo(5.2, .05, (0, -.75, 0), 'XZ', 64)])
    # castillo: tira y ventanas
    export("SM_Cast_Neon", [caja((0, 0, 8.4), (14.2, .14, .14)), caja((-7.1, 0, 4.2), (.1, .1, 8.4)), caja((7.1, 0, 4.2), (.1, .1, 8.4))])
    w = []
    for i in (-3, -2, -1, 1, 2, 3):
        x = i * 1.95
        w.append(caja((x, 0, 4.8), (.85, .04, 1.5)))
        bm = bmesh.new()
        v = [bm.verts.new((x + .425 * math.cos(a * math.pi / 8), 0, 5.55 + .425 * math.sin(a * math.pi / 8))) for a in range(9)]
        bm.faces.new(v)
        w.append(new("arco", bm, False))
    export("SM_Cast_Ventanas", w)
    # marco de bombillas del teatro (plano YZ, 6.2 x 2.2)
    b = []
    for i in range(23):
        y = -3.1 + i * 6.2 / 22
        b += [(0, y, 1.1), (0, y, -1.1)]
    for i in range(1, 8):
        z = -1.1 + i * 2.2 / 8
        b += [(0, -3.1, z), (0, 3.1, z)]
    export("SM_Tea_Bombillas", [esferas(b, .08)])
    export("SM_Tea_Neon", [caja((0, 0, -1.45), (.1, 6.6, .1)), caja((0, 0, 1.45), (.1, 6.6, .1))])


# ------------------------- MONTANA RUSA -------------------------
def montana():
    from mathutils import geometry
    P = [(-50, 0, 1.5), (-42, .6, 9), (-32, 0, 17), (-22, -.6, 5), (-12, .6, 12), (-2, 0, 22), (8, -.6, 8), (18, .6, 16), (28, 0, 4), (33, .4, 9)]
    P = [Vector(p) for p in P]

    def cr(p0, p1, p2, p3, t):
        return .5 * ((2 * p1) + (-p0 + p2) * t + (2 * p0 - 5 * p1 + 4 * p2 - p3) * t * t + (-p0 + 3 * p1 - 3 * p2 + p3) * t ** 3)
    pts = []
    Q = [P[0]] + P + [P[-1]]
    for i in range(1, len(Q) - 2):
        for k in range(14):
            pts.append(cr(Q[i - 1], Q[i], Q[i + 1], Q[i + 2], k / 14))
    pts.append(P[-1])
    side = Vector((0, .55, 0))
    objs = [tubo([p + side for p in pts], .06, 6), tubo([p - side for p in pts], .06, 6), tubo([p + Vector((0, 0, -.45)) for p in pts], .09, 6)]
    for i in range(0, len(pts), 2):
        objs.append(tubo([pts[i] + side, pts[i] - side], .025, 4, False))
    for i in range(0, len(pts), 5):
        p = pts[i]
        if p.z > 1.2:
            for s in (-1, 1):
                objs.append(tubo([(p.x, p.y + s * 1.2, 0), (p.x, p.y + s * .3, p.z - .45)], .07, 5, False))
            for h in range(1, int(p.z // 3) + 1):
                z = h * 3.0
                objs.append(tubo([(p.x, p.y - 1.1, z - 1), (p.x, p.y + 1.1, z)], .03, 4, False))
    export("SM_Montana", objs)
    export("SM_Montana_Carro", [caja((0, 0, .55), (2.4, 1.15, .6), 0, .1), caja((1.05, 0, .85), (.5, 1.1, .7), 0, .1)])


def tren():
    export("SM_Tren_Riel", [anillo(5.9, .035, (0, 0, .06)), anillo(6.3, .035, (0, 0, .06))])
    d = []
    for i in range(40):
        a = 2 * math.pi * i / 40
        d.append(caja((6.1 * math.cos(a), 6.1 * math.sin(a), .03), (.9, .16, .06), a))
    export("SM_Tren_Durmientes", d)
    export("SM_Tren_Vagon", [caja((0, 0, .75), (1.0, 1.7, .75), 0, .06), caja((0, 0, 1.75), (1.0, 1.7, .08), 0, .03)] + [tubo([(sx * .45, sy * .75, 1.1), (sx * .45, sy * .75, 1.75)], .03, 5) for sx in (-1, 1) for sy in (-1, 1)])
    r = [tubo([(sx * .56, sy * .55, .22), (sx * .46, sy * .55, .22)], .2, 12, True) for sx in (-1, 1) for sy in (-1, 1)]
    export("SM_Tren_Ruedas", r)
    export("SM_Tren_Locomotora", [tubo([(0, -1.2, .95), (0, .7, .95)], .46, 14), caja((0, 1.05, 1.1), (1.0, .9, 1.4), 0, .05), tubo([(0, -.8, 1.3), (0, -.8, 1.9)], .13, 10, True, .2), caja((0, 1.05, 1.85), (1.15, 1.05, .1), 0, .03)])


clear()
entrada()
boleteria()
guirnalda()
kiosco()
auto()
pista("A", 15.0, 11.0, 5.05)
pista("B", 9.8, 9.8, 4.1)
neones()
montana()
tren()
print("BUILD_OK", len([f for f in os.listdir(DIR) if f.endswith('.fbx')]))
