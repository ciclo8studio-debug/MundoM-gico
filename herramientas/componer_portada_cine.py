# Compone la portada estilo afiche de cine (1920x1280) a partir del arte de la novela grafica.
# Uso: python3.13 herramientas/componer_portada_cine.py   (requiere Pillow)
# Salida: <Unreal>/Fuentes/UI/T_PortadaCine.png  (se importa sobre T_PortadaHTML con importar_portada_cine.py)
import os
from PIL import Image, ImageDraw, ImageFont, ImageEnhance, ImageFilter

RAIZ = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ARTE = os.path.join(RAIZ, "VIDEO GAME MUNDO", "NOVELA GRÁFICA", "portadamuestra.jpg")
FUENTES = os.path.join(RAIZ, "Fuentes_descarga")
SALIDA = r"C:/Users/pansh/Documents/Unreal Projects/MUNDO_MÁGICO/Fuentes/UI/T_PortadaCine.png"
W, H = 1920, 1280
BARRA = 185   # franjas negras (en 16:9 se ven ~85 px)

# 1) Fondo: cubrir 1920x1280 dejando a los personajes a la derecha
arte = Image.open(ARTE).convert("RGB")
s = H / arte.height
arte = arte.resize((round(arte.width * s), H), Image.LANCZOS)
x0 = arte.width - W
img = arte.crop((x0, 0, x0 + W, H))

# 2) Etalonaje: menos saturacion, mas oscuro, mas contraste, tinte frio
img = ImageEnhance.Color(img).enhance(0.6)
img = ImageEnhance.Brightness(img).enhance(0.72)
img = ImageEnhance.Contrast(img).enhance(1.15)
tinte = Image.new("RGB", (W, H), (8, 22, 28))
img = Image.blend(img, tinte, 0.18)

# 3) Degradado negro desde la izquierda (zona de texto) + viñeta
mask = Image.new("L", (W, H), 0)
d = ImageDraw.Draw(mask)
for x in range(W):
    a = max(0.0, 1 - x / 1150) ** 1.4 * 235
    d.line([(x, 0), (x, H)], fill=int(a))
img = Image.composite(Image.new("RGB", (W, H), (0, 0, 0)), img, mask)
vin = Image.new("L", (W, H), 0)
ImageDraw.Draw(vin).ellipse((-320, -260, W + 320, H + 260), fill=255)
vin = vin.filter(ImageFilter.GaussianBlur(220))
img = Image.composite(img, Image.new("RGB", (W, H), (0, 0, 0)), vin)

# 4) Franjas de cine
d = ImageDraw.Draw(img)
d.rectangle((0, 0, W, BARRA), fill=(0, 0, 0))
d.rectangle((0, H - BARRA, W, H), fill=(0, 0, 0))

# 5) Titulo, subtitulo y frase
f_tit = ImageFont.truetype(os.path.join(FUENTES, "SpecialElite-Regular.ttf"), 128)
f_sub = ImageFont.truetype(os.path.join(FUENTES, "BarlowCondensed-SemiBold.ttf"), 36)
f_fra = ImageFont.truetype(os.path.join(FUENTES, "BarlowCondensed-Regular.ttf"), 32)
f_cre = ImageFont.truetype(os.path.join(FUENTES, "BarlowCondensed-Regular.ttf"), 22)
HUESO, ROJO, GRIS = (232, 226, 210), (178, 28, 24), (140, 140, 135)

def espaciado(xy, texto, fuente, color, sep):
    x, y = xy
    for ch in texto:
        d.text((x, y), ch, font=fuente, fill=color)
        x += d.textlength(ch, font=fuente) + sep
    return x

X = 140
d.text((X, 300), "MUNDO MÁGICO", font=f_tit, fill=HUESO)
ancho = d.textlength("MUNDO MÁGICO", font=f_tit)
d.line((X + 4, 452, X + ancho, 452), fill=ROJO, width=3)
espaciado((X + 6, 470), "TURNO NOCTURNO", f_sub, ROJO, 14)
d.text((X + 6, 560), "El parque cerró hace años. Alguien sigue de turno.", font=f_fra, fill=GRIS)

# 6) Bloque de creditos estilo afiche (dentro de la franja inferior visible)
cred = "UN JUEGO DE TERROR   ·   BASADO EN LA NOVELA GRÁFICA «MUNDO MÁGICO»   ·   DEMO"
xs = (W - (d.textlength(cred, font=f_cre) + 4 * len(cred))) / 2
espaciado((xs, H - BARRA + 12), cred, f_cre, (110, 110, 105), 4)

img.save(SALIDA)
img.convert("RGB").resize((960, 640)).save(os.path.join(os.environ.get("TEMP", "."), "portada_cine_prev.jpg"), quality=85)
print("OK", SALIDA)
