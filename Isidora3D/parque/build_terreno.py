# Terreno organico del parque: suelo, cerros, montes, caminos, reja, arboles y pasto.
# Uso: blender -b --python build_terreno.py   (coordenadas de Unreal en cm; se exporta en metros con Y invertida)
import bpy, json, math, random
import numpy as np

DIR = r"C:/Users/pansh/Desktop/MundoMagico_Project/Isidora3D/parque/"
random.seed(11)
rs = np.random.RandomState(7)
bpy.ops.wm.read_factory_settings(use_empty=True)
col = bpy.context.collection


def catmull(p, n=8, closed=False):
    p = [np.array(q, float) for q in p]
    q = [p[0]] + p + [p[-1]]
    o = []
    for i in range(1, len(q) - 2):
        a, b, c, d = q[i - 1], q[i], q[i + 1], q[i + 2]
        for k in range(n):
            t = k / n
            o.append(.5 * (2 * b + (c - a) * t + (2 * a - 5 * b + 4 * c - d) * t * t + (3 * b - a - 3 * c + d) * t ** 3))
    o.append(p[-1])
    return np.array(o)


# Perimetro: va desde el porton este, da la vuelta al parque y vuelve al porton oeste
PER = catmull([(900, -3950), (2500, -3900), (5500, -3700), (8400, -4700), (8600, -7500), (8300, -10000), (9400, -12300), (11000, -13800),
               (10900, -16600), (8000, -17200), (6500, -18900), (3500, -19200), (200, -18300), (-3500, -19000), (-7000, -18300), (-7800, -16200),
               (-9000, -14000), (-9400, -11500), (-9000, -9000), (-9800, -6500), (-9200, -4300), (-6500, -3900), (-3500, -3700), (-900, -3950)], 10)
EMBUDO = [np.array([(900, -3950), (240, -3535)], float), np.array([(-900, -3950), (-240, -3535)], float)]

CAM = [  # (puntos, ancho)
    ([(0, -3960), (0, -6000), (0, -9000), (0, -11900)], 400),
    ([(150, -9400), (2000, -9700), (3900, -10250)], 320),
    ([(3900, -10250), (4800, -11800), (6600, -11300), (8000, -11600), (8500, -13300), (8900, -14700)], 280),
    ([(150, -6100), (2600, -5900), (5100, -6050)], 320),
    ([(-150, -8000), (-2200, -8300), (-4500, -8350)], 320),
    ([(-150, -4950), (-2600, -4950), (-5200, -5200), (-7250, -5150)], 300),
    ([(-150, -10500), (-2500, -10900), (-4300, -11800), (-5300, -12400)], 320),
    ([(300, -11800), (1500, -12000), (1900, -13400), (1650, -15700)], 300),
    ([(1650, -15700), (2900, -16500), (3700, -17300)], 280),
    ([(-300, -11800), (-1800, -12400), (-2900, -13400), (-3100, -15300), (-2700, -16650)], 300),
    ([(-5300, -12400), (-4900, -10800), (-4600, -9750)], 260),
]
CAM = [(catmull(p, 12), w) for p, w in CAM]

PADS = [(-7650, -13250, -5300, -11750), (8700, -15500, 9700, -14700), (5100, -6700, 7300, -5500), (3900, -11700, 7000, -8900), (3700, -18050, 5150, -16800),
        (-8300, -5600, -7250, -4850), (-1000, -13500, 1000, -11900), (800, -7750, 2250, -6650), (-7200, -9750, -4500, -7500), (1200, -16600, 2000, -15800),
        (-4400, -14200, -2900, -12800), (-6800, -17800, 0, -16700), (-2700, -7700, -1300, -6300), (-3600, -6400, -2400, -5400), (-3200, -4700, -1800, -4050),
        (-1500, -5000, 1500, -4100)]
HOSP = (-1300, -3950, 1300, 3700)
CERROS = [(6500, -13300, 1700, 1250), (-5300, -15200, 1500, 900), (-8200, -10300, 900, 480), (3200, -13000, 850, 340), (7300, -8100, 800, 420), (-1600, -18000, 800, 300), (-3000, -9700, 650, 180), (2600, -8000, 500, 110)]


def seg_dist(X, Y, P):
    d = np.full(X.shape, 1e12)
    for a, b in zip(P[:-1], P[1:]):
        v = b - a
        t = np.clip(((X - a[0]) * v[0] + (Y - a[1]) * v[1]) / max(v @ v, 1e-6), 0, 1)
        d = np.minimum(d, (X - a[0] - t * v[0]) ** 2 + (Y - a[1] - t * v[1]) ** 2)
    return np.sqrt(d)


CIERRE = np.vstack([PER, [(-900, 8000), (900, 8000)]])  # el poligono interior no incluye el sur del hospital


def dentro(X, Y):
    r = np.zeros(X.shape, bool)
    P = PER
    for a, b in zip(P, np.roll(P, -1, 0)):
        c = ((a[1] > Y) != (b[1] > Y)) & (X < (b[0] - a[0]) * (Y - a[1]) / (b[1] - a[1] + 1e-9) + a[0])
        r ^= c
    return r


def sdist(X, Y):
    d = seg_dist(X, Y, PER)
    return np.where(dentro(X, Y), -d, d)


K = [(rs.uniform(0, 6.283), 2 * math.pi / w, rs.uniform(0, 6.283), a) for w, a in ((9000, 1), (6100, .8), (4300, .7), (2700, .5), (1900, .35), (1200, .22), (800, .14))]


def ruido(X, Y, s=1.0, f=0.0):
    r = 0
    for th, w, ph, a in K:
        r = r + a * np.sin((X * math.cos(th + f) + Y * math.sin(th + f)) * w * s + ph)
    return r / 2.2


def smooth(a, b, x):
    t = np.clip((x - a) / (b - a), 0, 1)
    return t * t * (3 - 2 * t)


def exterior(X, Y, sd):
    n = ruido(X, Y)
    m = ruido(X, Y, .22, 1.3)
    return smooth(0, 3200, sd) * (1050 + 480 * n) + smooth(2500, 14000, sd) * (3800 + 2400 * m) + smooth(12000, 45000, sd) * (5200 + 3400 * ruido(X, Y, .09, 2.1))


def rect_d(X, Y, r):
    dx = np.maximum(np.maximum(r[0] - X, X - r[2]), 0)
    dy = np.maximum(np.maximum(r[1] - Y, Y - r[3]), 0)
    return np.hypot(dx, dy)


ACT = json.load(open(DIR + "actores.json"))
C = 150
xs = np.arange(-16050, 16051, C, dtype=float)
ys = np.arange(-24000, 9001, C, dtype=float)
X, Y = np.meshgrid(xs, ys)

SD = sdist(X, Y)
dpad = np.full(X.shape, 1e9)
for r in PADS:
    dpad = np.minimum(dpad, rect_d(X, Y, r))
dcam = np.full(X.shape, 1e9)
for P, w in CAM:
    dcam = np.minimum(dcam, seg_dist(X, Y, P) - w / 2)
dact = np.full(X.shape, 1e9)
for a in ACT:
    i, j = int((a[0] - xs[0]) / C), int((a[1] - ys[0]) / C)
    i0, i1, j0, j1 = max(i - 7, 0), i + 8, max(j - 7, 0), j + 8
    dact[j0:j1, i0:i1] = np.minimum(dact[j0:j1, i0:i1], np.hypot(X[j0:j1, i0:i1] - a[0], Y[j0:j1, i0:i1] - a[1]))
dlibre = np.minimum(np.minimum(dpad, dcam), dact - 80)
F = 1 - smooth(120, 820, dlibre)

dh = rect_d(X, Y, HOSP)
FH = 1 - smooth(0, 2200, dh)
und = 70 * ruido(X, Y, 1.6) + 45 * ruido(X, Y, 3.1, .7)
cer = 0
for cx, cy, r, h in CERROS:
    cer = cer + h * np.exp(-((X - cx) ** 2 + (Y - cy) ** 2) / (2 * (r * .58) ** 2))
Z = -1 + (1 - F) * (und + cer * (.9 + .1 * ruido(X, Y, 2.0))) * (1 - FH)
Z = Z + exterior(X, Y, SD) * (1 - FH) * (1 - F)
huella = ((np.abs(X) < 240) & (Y > -3525) & (Y < 2500)) | ((np.abs(X) < 1050) & (Y > 2480) & (Y < 3540)) | ((np.abs(X) < 1050) & (Y > 180) & (Y < 2120))
Z = np.where(huella, -30, Z)

print("Z", Z.min(), Z.max(), "dentro max", Z[SD < 0].max())


def alt(x, y):
    fx = min(max((x - xs[0]) / C, 0), len(xs) - 1.001)
    fy = min(max((y - ys[0]) / C, 0), len(ys) - 1.001)
    i, j = int(fx), int(fy)
    u, v = fx - i, fy - j
    return (Z[j, i] * (1 - u) + Z[j, i + 1] * u) * (1 - v) + (Z[j + 1, i] * (1 - u) + Z[j + 1, i + 1] * u) * v


def malla(name, V, Fc, uv=None, smooth_=True):
    V = np.asarray(V, float)
    me = bpy.data.meshes.new(name)
    W = np.column_stack([V[:, 0] / 100, -V[:, 1] / 100, V[:, 2] / 100])
    # invertir Y cambia la orientacion: se invierten las caras para que miren hacia afuera
    me.from_pydata(W.tolist(), [], [tuple(reversed(f)) for f in Fc])
    me.update()
    if uv is not None:
        l = me.uv_layers.new(name="UV")
        idx = np.zeros(len(me.loops), np.int32)
        me.loops.foreach_get("vertex_index", idx)
        l.data.foreach_set("uv", np.asarray(uv, np.float32)[idx].ravel())
    me.polygons.foreach_set("use_smooth", [smooth_] * len(me.polygons))
    me.update()
    o = bpy.data.objects.new(name, me)
    col.objects.link(o)
    bpy.ops.object.select_all(action='DESELECT')
    o.select_set(True)
    bpy.context.view_layer.objects.active = o
    bpy.ops.export_scene.fbx(filepath=DIR + name + ".fbx", use_selection=True, object_types={'MESH'}, mesh_smooth_type='FACE', add_leaf_bones=False, bake_anim=False)
    print("OK", name, len(me.vertices), len(me.polygons))
    bpy.data.objects.remove(o, do_unlink=True)


def rejilla(name, Xg, Yg, Zg, sel, escala_uv=800.0):
    h, w = Xg.shape
    ii = np.arange(h * w).reshape(h, w)
    q = np.stack([ii[:-1, :-1], ii[:-1, 1:], ii[1:, 1:], ii[1:, :-1]], -1)[sel]
    used, inv = np.unique(q, return_inverse=True)
    inv = inv.reshape(q.shape)
    V = np.column_stack([Xg.ravel()[used], Yg.ravel()[used], Zg.ravel()[used]])
    uv = V[:, :2] / escala_uv
    malla(name, V, [tuple(int(k) for k in f) for f in inv], uv)


SDc = (SD[:-1, :-1] + SD[1:, 1:]) / 2
rejilla("SM_Terr_Parque", X, Y, Z, SDc < 500)
rejilla("SM_Terr_Cerros", X, Y, Z, SDc >= 500)

# Montes lejanos: silueta en el horizonte, sin colision
mx = np.arange(-72000, 72001, 1500.0)
my = np.arange(-84000, 60001, 1500.0)
MX, MY = np.meshgrid(mx, my)
MZ = exterior(MX, MY, sdist(MX, MY))
cx = (MX[:-1, :-1] + MX[1:, 1:]) / 2
cy = (MY[:-1, :-1] + MY[1:, 1:]) / 2
rejilla("SM_Terr_Montes", MX, MY, MZ - 40, (np.abs(cx) > 15000) | (cy < -23000) | (cy > 8000), 3000.0)


# Caminos
V, Fc, U = [], [], []
for P, w in CAM:
    b = len(V)
    s = 0.0
    for i, p in enumerate(P):
        t = P[min(i + 1, len(P) - 1)] - P[max(i - 1, 0)]
        t = t / (np.linalg.norm(t) + 1e-6)
        n = np.array([-t[1], t[0]])
        if i:
            s += np.linalg.norm(p - P[i - 1])
        for k, sg in enumerate((-1, 1)):
            q = p + n * sg * w / 2
            V.append((q[0], q[1], alt(q[0], q[1]) + 2.5))
            U.append((k, s / 400))
    for i in range(len(P) - 1):
        a = b + i * 2
        Fc.append((a, a + 1, a + 3, a + 2))
malla("SM_Terr_Camino", V, Fc, U)


def prisma(V, Fc, a, b, r, seg=4, r2=None):
    """Tubo simple entre dos puntos, sin tapas."""
    a, b = np.array(a, float), np.array(b, float)
    d = b - a
    d = d / (np.linalg.norm(d) + 1e-6)
    e = np.cross(d, (0, 0, 1) if abs(d[2]) < .9 else (1, 0, 0))
    e = e / np.linalg.norm(e)
    f = np.cross(d, e)
    n = len(V)
    for c, rr in ((a, r), (b, r if r2 is None else r2)):
        for k in range(seg):
            t = 6.283 * k / seg
            V.append(tuple(c + (e * math.cos(t) + f * math.sin(t)) * rr))
    for k in range(seg):
        m = (k + 1) % seg
        Fc.append((n + k, n + m, n + seg + m, n + seg + k))


# Reja siguiendo el terreno y muro invisible
RV, RF, LV, LF = [], [], [], []
for P in [PER] + EMBUDO:
    L = np.r_[0, np.cumsum(np.linalg.norm(np.diff(P, axis=0), axis=1))]
    pts = []
    for s in np.arange(0, L[-1] + 1, 300.0):
        i = min(np.searchsorted(L, s, 'right') - 1, len(P) - 2)
        t = (s - L[i]) / max(L[i + 1] - L[i], 1e-6)
        q = P[i] + (P[i + 1] - P[i]) * t
        pts.append((q[0], q[1], alt(q[0], q[1])))
    pts.append((P[-1][0], P[-1][1], alt(P[-1][0], P[-1][1])))
    for i, p in enumerate(pts):
        lean = random.uniform(-14, 14)
        prisma(RV, RF, p, (p[0] + lean, p[1] + lean * .6, p[2] + 245), 6)
        if i:
            o = pts[i - 1]
            for hz in (60, 140, 225):
                if random.random() > .05:
                    prisma(RV, RF, (o[0], o[1], o[2] + hz), (p[0], p[1], p[2] + hz), 3, 3)
            n = len(LV)  # muro con grosor para que la capsula no lo atraviese
            ax, ay = p[0] - o[0], p[1] - o[1]
            ln = math.hypot(ax, ay) + 1e-6
            nx, ny = -ay / ln * 20, ax / ln * 20
            for c in (o, p):
                for sx in (-1, 1):
                    LV.append((c[0] + nx * sx, c[1] + ny * sx, c[2] - 120))
                    LV.append((c[0] + nx * sx, c[1] + ny * sx, c[2] + 900))
            LF += [(n, n + 1, n + 5, n + 4), (n + 2, n + 6, n + 7, n + 3), (n + 1, n + 3, n + 7, n + 5), (n, n + 4, n + 6, n + 2), (n, n + 2, n + 3, n + 1), (n + 4, n + 5, n + 7, n + 6)]
malla("SM_Terr_Reja", RV, RF, None, False)
malla("SM_Terr_Limite", LV, LF, None, False)


# Arboles
def copa(V, Fc, c, r):
    """Copa irregular: octaedro subdividido a mano con ruido."""
    n = len(V)
    pts = [(0, 0, 1), (1, 0, .2), (.5, .87, .2), (-.5, .87, .2), (-1, 0, .2), (-.5, -.87, .2), (.5, -.87, .2), (.8, .45, -.5), (-.1, .9, -.5), (-.85, .4, -.5), (-.75, -.5, -.5), (.1, -.9, -.5), (.85, -.4, -.5), (0, 0, -.9)]
    for p in pts:
        k = random.uniform(.78, 1.18)
        V.append((c[0] + p[0] * r * k, c[1] + p[1] * r * k, c[2] + p[2] * r * k * .8))
    for i in range(6):
        Fc.append((n, n + 1 + i, n + 1 + (i + 1) % 6))
        Fc.append((n + 1 + i, n + 7 + i, n + 1 + (i + 1) % 6))
        Fc.append((n + 1 + (i + 1) % 6, n + 7 + i, n + 7 + (i + 1) % 6))
        Fc.append((n + 13, n + 7 + (i + 1) % 6, n + 7 + i))


TV, TF, BV, BF, CV, CF = [], [], [], [], [], []


POS = []


def arbol(x, y, cerca):
    POS.append([round(x), round(y), round(alt(x, y)), 1 if cerca else 0])
    z = alt(x, y) - 8
    h = random.uniform(520, 980) * (1 if cerca else 1.25)
    r = h * random.uniform(.028, .042)
    tx, ty = random.uniform(-.12, .12) * h, random.uniform(-.12, .12) * h
    top = (x + tx, y + ty, z + h)
    mid = (x + tx * .4, y + ty * .4, z + h * .55)
    if cerca:
        prisma(TV, TF, (x, y, z), mid, r, 6, r * .72)
        prisma(BV, BF, mid, top, r * .72, 5, r * .22)
    else:
        prisma(BV, BF, (x, y, z), top, r, 4, r * .3)
    muerto = random.random() < .38
    for k in range(random.randint(3, 5) if cerca else 2):
        t = random.uniform(.45, .85)
        a = random.uniform(0, 6.283)
        L = h * random.uniform(.22, .42)
        o = (x + tx * t, y + ty * t, z + h * t)
        e = (o[0] + math.cos(a) * L, o[1] + math.sin(a) * L, o[2] + L * random.uniform(.25, .8))
        prisma(BV, BF, o, e, r * .32, 3, r * .08)
        if not muerto and cerca:
            copa(CV, CF, e, h * random.uniform(.16, .25))
    if not muerto:
        copa(CV, CF, top, h * random.uniform(.24, .34))
        if not cerca:
            copa(CV, CF, (top[0] + h * .15, top[1] - h * .1, top[2] - h * .22), h * .26)


SEL = set(tuple(p) for p in json.load(open(DIR + "arboles_sel.json")))
_arbol = arbol


def arbol(x, y, cerca):
    global TV, TF, BV, BF, CV, CF
    if (round(x), round(y)) in SEL:
        g = TV, TF, BV, BF, CV, CF
        TV, TF, BV, BF, CV, CF = [], [], [], [], [], []
        _arbol(x, y, cerca)
        TV, TF, BV, BF, CV, CF = g
    else:
        _arbol(x, y, cerca)


cnt = [0, 0]
intent = 0
while intent < 26000:
    intent += 1
    x, y = random.uniform(-15000, 15500), random.uniform(-23000, 4800)
    j, i = int((y - ys[0]) / C), int((x - xs[0]) / C)
    sd = SD[j, i]
    if huella[j, i] or sd > 6200 or dh[j, i] < 200:
        continue
    den = float(ruido(np.array(x), np.array(y), 2.4, .5))
    if sd < -60:
        if dlibre[j, i] < 260 or den < -.25 or cnt[0] >= 620:
            continue
        arbol(x, y, True)
        cnt[0] += 1
    elif sd > 60:
        if den < -.35 or cnt[1] >= 1500:
            continue
        arbol(x, y, False)
        cnt[1] += 1
print("arboles", cnt)
print("pos", len(POS))
print("cerca sin hornear", len(TV))
malla("SM_Terr_LejosRamas", BV, BF, None, True)
malla("SM_Terr_LejosCopas", CV, CF, None, False)

# Pasto seco en matas
PV, PF = [], []
n = 0
intent = 0
while n < 3200 and intent < 60000:
    intent += 1
    x, y = random.uniform(-10500, 11500), random.uniform(-19500, -3700)
    j, i = int((y - ys[0]) / C), int((x - xs[0]) / C)
    if SD[j, i] > 900 or dcam[j, i] < 40 or dpad[j, i] < 0 or float(ruido(np.array(x), np.array(y), 3.3, 1.1)) < -.1:
        continue
    z = alt(x, y) - 3
    for k in range(random.randint(3, 6)):
        a, b = x + random.uniform(-45, 45), y + random.uniform(-45, 45)
        h = random.uniform(28, 72)
        m = len(PV)
        s = random.uniform(5, 9)
        PV += [(a - s, b - s, z), (a + s, b - s, z), (a, b + s, z), (a + random.uniform(-16, 16), b + random.uniform(-16, 16), z + h)]
        PF += [(m, m + 1, m + 3), (m + 1, m + 2, m + 3), (m + 2, m, m + 3)]
    n += 1
print("matas", n)
malla("SM_Terr_Pasto", PV, PF, None, False)

# Revision: nada debe quedar fuera de la reja ni enterrado
A = np.array([a[:2] for a in ACT], float)
sa = sdist(A[:, 0], A[:, 1])
mal = [ACT[i][3] + " %d,%d" % (ACT[i][0], ACT[i][1]) for i in range(len(ACT)) if sa[i] > -80 and ACT[i][1] < -3900]
print("FUERA", len(mal), mal[:30])
alto = [ACT[i][3] for i in range(len(ACT)) if abs(alt(ACT[i][0], ACT[i][1]) + 1) > 12 and ACT[i][1] < -3900]
print("DESNIVEL", len(alto), alto[:30])
json.dump({"per": PER.tolist(), "cam": [[P.tolist(), w] for P, w in CAM]}, open(DIR + "trazado.json", "w"))
print("TERRENO_FIN")
