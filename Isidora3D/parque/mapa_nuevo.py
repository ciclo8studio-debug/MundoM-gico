# Dibuja T_Mapa3 con el trazado nuevo. Limites: x -12000..12000, y -19500..4500
import json
from PIL import Image, ImageDraw, ImageFont, ImageFilter

D = r"C:/Users/pansh/Desktop/MundoMagico_Project/Isidora3D/parque/"
OUT = r"C:/Users/pansh/Documents/Unreal Projects/MUNDO_MÁGICO/Fuentes/UI/T_Mapa3.png"
S = 2048
K = S / 24000.0
t = json.load(open(D + "trazado.json"))


def P(x, y):
    return ((x + 12000) * K, (y + 19500) * K)


im = Image.new("RGB", (S, S), (12, 13, 14))
g = ImageDraw.Draw(im)
g.polygon([P(*p) for p in t["per"]], fill=(34, 36, 32))
g.line([P(*p) for p in t["per"]] + [P(*t["per"][0])], fill=(70, 62, 52), width=5)
for cx, cy, r in ((6500, -13300, 1200), (-5300, -15200, 1050), (-8200, -10300, 620), (3200, -13000, 560), (7300, -8100, 520)):
    a, b = P(cx - r, cy - r), P(cx + r, cy + r)
    g.ellipse([a, b], fill=(45, 58, 42))
    a, b = P(cx - r * .55, cy - r * .55), P(cx + r * .55, cy + r * .55)
    g.ellipse([a, b], fill=(54, 70, 50))
for pts, w in t["cam"]:
    g.line([P(*p) for p in pts], fill=(92, 88, 80), width=max(int(w * K * 1.15), 8), joint="curve")


def R(x0, y0, x1, y1, c=(150, 145, 135)):
    g.rectangle([P(x0, y0), P(x1, y1)], fill=c)


def C(x, y, r, c=(150, 145, 135)):
    g.ellipse([P(x - r, y - r), P(x + r, y + r)], fill=c)


H = (120, 140, 140)
R(-820, 2480, 820, 3520, H)
R(-210, -3530, 210, 2490, H)
R(-1010, 200, -200, 1000, H)
R(200, 1300, 1010, 2100, H)
R(-400, -3950, 400, -3530, (34, 36, 32))
R(750, -5350, 1050, -5050)
C(-2000, -7000, 600, (140, 50, 45))
R(4300, -10500, 6700, -10100)
R(-700, -13200, 700, -12200)
for dx, dy in ((-750, -550), (750, -550), (-750, 550), (750, 550)):
    C(dx, -12700 + dy, 160)
R(-7600, -13200, -5800, -11800)
R(5800, -6600, 7200, -5600)
for dx, dy in ((0, 0), (700, 400), (-600, 500), (400, -700), (-700, -500), (900, -200)):
    C(-6000 + dx, -8500 + dy, 380, (50, 80, 52))
R(8750, -15450, 9650, -14750)
R(-8200, -5500, -7400, -4900)
R(800, -7700, 2200, -6700)
for x, y in ((-700, -6000), (700, -8200), (-700, -10200)):
    R(x - 125, y - 125, x + 125, y + 125)
R(-700, -4680, 700, -4520, (140, 50, 45))
R(4100, -17800, 4900, -17000)
C(1600, -16200, 260)
C(-3650, -13500, 560, (82, 78, 70))
g.line([P(-6200, -17160), P(-400, -17160)], fill=(120, 110, 95), width=10)
f = ImageFont.truetype("C:/Windows/Fonts/segoeuib.ttf", 34)
for s, x, y in (("PABELLON 4", -760, 2680), ("URGENCIAS", 260, -2200), ("REANIMACION", -1500, 40), ("FARMACIA", 1060, 1600), ("ENTRADA", -500, -4300), ("VIDEO", 700, -5550),
                ("CARRUSEL", -2600, -7000), ("NORIA", 5000, -9850), ("CASTILLO", -560, -13500), ("TEATRO", -7200, -12650), ("TREN", 6200, -6250), ("JARDIN", -6500, -8600),
                ("CASA", 8850, -15750), ("BODEGA", -8300, -5750), ("AUTOS", 1100, -7300), ("CHOCONES", 3950, -18150), ("JUEGOS", -3400, -5750), ("TORRE", 1250, -16650),
                ("MONTANA RUSA", -4200, -17600), ("TRENCITO", -4100, -14300)):
    g.text(P(x, y), s, font=f, fill=(235, 225, 200))
im.save(OUT)
im.resize((900, 900)).save(D + "mapa_prev.jpg", quality=80)
print("MAPA_OK")
