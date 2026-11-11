# Genera IES_Linterna.ies: perfil de linterna incandescente (centro caliente, anillo del reflector, caida suave).
# Formato IESNA LM-63-2002. Uso: python herramientas/crear_ies_linterna.py
import os, math
ang = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 16, 18, 20, 23, 26, 30, 35, 40, 50, 60, 90]
def rel(a):
    centro = math.exp(-(a / 3.2) ** 2) * 0.45          # punto caliente
    anillo = math.exp(-((a - 9.5) / 1.8) ** 2) * 0.25   # anillo del reflector
    base = 1 / (1 + (a / 13.0) ** 4)                    # cono principal
    halo = math.exp(-a / 22.0) * 0.06                   # derrame
    return max(0.0, 0.55 * base + centro + anillo + halo) if a < 60 else 0.0
cd = [rel(a) for a in ang]
m = max(cd)
cd = [round(1000 * v / m, 1) for v in cd]
out = ["IESNA:LM-63-2002", "[TEST] MundoMagico", "[MANUFAC] Claude", "[LUMCAT] Linterna 1998", "[LUMINAIRE] Linterna incandescente",
       "TILT=NONE",
       "1 -1 1 %d 1 1 2 0.05 0.05 0.05" % len(ang),
       "1.0 1.0 10",
       " ".join(str(a) for a in ang),
       "0",
       " ".join(str(v) for v in cd)]
dst = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'Fuentes_descarga', 'IES_Linterna.ies'))
open(dst, 'w').write("\n".join(out) + "\n")
print('OK', dst, cd[:12])
