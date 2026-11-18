# Genera T_LuzPortada.png (1024x1024 RGBA): luz de linterna de la portada web (#mlight) para WBP_Menu.
# El widget mide 2x el ancho de pantalla (cuadrado) y se centra en el mouse; perfil en fraccion del medio lado:
#   v0.3.59: 0 -> calido a .28 | .11 -> calido a .10 | .18 -> transparente | .40 en adelante -> negro a .45
#   (WBP_Menu: LuzMouse a pantalla completa con RenderScale 2.2 x 3.91 para que sea un circulo sin bordes)
import os, struct, zlib, math

N = 1024
DST = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'MUSICA_PARA_UNREAL', 'T_LuzPortada.png')
CAL = (255, 236, 200)

def px(r):
    # v0.3.59: mas luz y borde invisible (el widget cubre toda la pantalla con RenderScale)
    if r < 0.11:
        a = 0.28 + (0.10 - 0.28) * (r / 0.11); return CAL, a
    if r < 0.18:
        a = 0.10 * (1 - (r - 0.11) / 0.07); return CAL, a
    if r < 0.40:
        t = (r - 0.18) / 0.22; return (0, 0, 0), 0.45 * (t * t * (3 - 2 * t))
    return (0, 0, 0), 0.45

raw = bytearray()
for y in range(N):
    raw.append(0)
    for x in range(N):
        r = math.hypot(x - N / 2 + 0.5, y - N / 2 + 0.5) / (N / 2)
        c, a = px(r)
        raw += bytes((c[0], c[1], c[2], int(max(0, min(1, a)) * 255)))

def chunk(t, d):
    return struct.pack('>I', len(d)) + t + d + struct.pack('>I', zlib.crc32(t + d) & 0xffffffff)

png = b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', struct.pack('>IIBBBBB', N, N, 8, 6, 0, 0, 0)) \
    + chunk(b'IDAT', zlib.compress(bytes(raw), 9)) + chunk(b'IEND', b'')
open(os.path.abspath(DST), 'wb').write(png)
print('OK', os.path.abspath(DST))

