# Genera T_LuzPortada.png (1024x1024 RGBA): luz de linterna de la portada web (#mlight) para WBP_Menu.
# El widget mide 2x el ancho de pantalla (cuadrado) y se centra en el mouse; perfil en fraccion del medio lado:
#   0 -> calido a .20 · .09 -> calido a .07 · .15 -> transparente · .36 en adelante -> negro a .60
import os, struct, zlib, math

N = 1024
DST = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'MUSICA_PARA_UNREAL', 'T_LuzPortada.png')
CAL = (255, 236, 200)

def px(r):
    if r < 0.09:
        a = 0.20 + (0.07 - 0.20) * (r / 0.09); return CAL, a
    if r < 0.15:
        a = 0.07 * (1 - (r - 0.09) / 0.06); return CAL, a
    if r < 0.36:
        a = 0.60 * ((r - 0.15) / 0.21) ** 1.2; return (0, 0, 0), a
    return (0, 0, 0), 0.60

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
