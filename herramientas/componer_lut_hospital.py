# Genera T_LUT_HospitalHTML.png (LUT de Unreal 256x16: x = r + 16*b, y = g, 16 niveles por canal).
# Replica el post del juego HTML (desaturacion 22 %, curva S 35 %, tinte) con el giro verdoso pedido para el hospital:
#   tinte (0.92, 1.02, 0.97) y sombras levantadas hacia verde-azulado. Sin vineta/grano: eso va en el PostProcessVolume.
import os, struct, zlib

DST = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'MUSICA_PARA_UNREAL', 'T_LUT_HospitalHTML.png')
TINTE = (0.92, 1.02, 0.97)
SOMBRA = (0.000, 0.018, 0.012)

def curva_s(v, k=0.35):
    s = v * v * (3 - 2 * v)
    return v + (s - v) * k

def grade(r, g, b):
    l = 0.2126 * r + 0.7152 * g + 0.0722 * b
    r, g, b = (c + (l - c) * 0.22 for c in (r, g, b))           # desaturar 22 %
    r, g, b = (curva_s(c) for c in (r, g, b))                    # contraste en S
    out = []
    for c, t, s in zip((r, g, b), TINTE, SOMBRA):
        c = c * t + s * (1 - c)                                   # tinte + sombras verdosas
        out.append(max(0.0, min(1.0, c)))
    return out

W, H = 256, 16
raw = bytearray()
for y in range(H):
    raw.append(0)
    for x in range(W):
        r, b, g = (x % 16) / 15, (x // 16) / 15, y / 15
        raw += bytes(int(round(c * 255)) for c in grade(r, g, b))

def chunk(t, d):
    return struct.pack('>I', len(d)) + t + d + struct.pack('>I', zlib.crc32(t + d) & 0xffffffff)

png = b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', struct.pack('>IIBBBBB', W, H, 8, 2, 0, 0, 0)) \
    + chunk(b'IDAT', zlib.compress(bytes(raw), 9)) + chunk(b'IEND', b'')
open(os.path.abspath(DST), 'wb').write(png)
print('OK', os.path.abspath(DST))
