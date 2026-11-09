# Reparte los arboles: modelos reales GV dentro del parque y borde de cerros; el resto queda como silueta lejana.
import json, math, random

D = r"C:/Users/pansh/Desktop/MundoMagico_Project/Isidora3D/parque/"
random.seed(23)
POS = json.load(open(D + "arboles_pos.json"))
ACT = json.load(open(D + "actores.json"))
TR = json.load(open(D + "trazado.json"))
PER = TR["per"][::4]
CAM = [p for c, w in TR["cam"] for p in c[::2]]
CELDA = 600
G = {}
for a in ACT:
    G.setdefault((int(a[0] // CELDA), int(a[1] // CELDA)), []).append((a[0], a[1]))
for p in CAM:
    G.setdefault((int(p[0] // CELDA), int(p[1] // CELDA)), []).append((p[0], p[1]))


def holgura(x, y):
    i, j = int(x // CELDA), int(y // CELDA)
    m = 1800.0
    for u in range(i - 3, i + 4):
        for v in range(j - 3, j + 4):
            for q in G.get((u, v), ()):
                m = min(m, math.hypot(q[0] - x, q[1] - y))
    return m


def borde(x, y):
    return min(math.hypot(p[0] - x, p[1] - y) for p in PER)


lejos = sorted([p for p in POS if not p[3]], key=lambda p: borde(p[0], p[1]))
reales = [p for p in POS if p[3]] + lejos[:520]
SEL = [[p[0], p[1]] for p in reales]
OUT = {k: [] for k in ("A", "B", "C", "I", "Seco1", "Seco2")}
ZMIN = {"A": 0, "B": 0, "C": 213, "I": 3, "Seco1": 0, "Seco2": 0}
for p in reales:
    h = holgura(p[0], p[1]) if p[3] else 1800
    if h > 850:
        k = random.choices(("I", "C", "B", "Seco1", "Seco2"), (34, 30, 16, 10, 10))[0]
    elif h > 520:
        k = random.choices(("C", "B", "A", "Seco1", "Seco2"), (24, 30, 20, 13, 13))[0]
    else:
        k = random.choices(("B", "A", "Seco1", "Seco2"), (32, 32, 18, 18))[0]
    s = round(random.uniform(.8, 1.2), 2)
    OUT[k].append("%d,%d,%d,%d,%s" % (p[0], p[1], p[2] - 4 + ZMIN[k] * s, random.randint(-180, 180), s))
json.dump(SEL, open(D + "arboles_sel.json", "w"))
json.dump({k: ";".join(v) for k, v in OUT.items()}, open(D + "arboles_datos.json", "w"))
print({k: len(v) for k, v in OUT.items()}, "reales", len(reales), "lejanos", len(POS) - len(reales))
