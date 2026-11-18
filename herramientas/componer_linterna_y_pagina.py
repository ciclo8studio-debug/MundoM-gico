# Genera (solo libreria estandar):
#  IES_Linterna.ies : perfil de linterna real, basado en la linterna del juego HTML (cono 23 deg, borde suave 50 %).
#                     Centro caliente, anillo brillante del reflector a ~9 deg, caida suave y halo debil hasta 30 deg.
#  SFX_Pagina.wav   : roce de papel + nota grave suave, para recoger hojas de la Novela (distinto a la puerta).
import math, os, random, struct, wave

DST = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'MUSICA_PARA_UNREAL')

# ---------- IES (LM-63-2002, tipo C, simetria rotacional) ----------
ang = [i * 0.5 for i in range(0, 181)]          # 0..90 deg verticales
def cand(a):
    if a <= 23:
        base = 1.0 - 0.35 * (a / 23) ** 2                     # centro -> borde del cono
        ring = 0.18 * math.exp(-((a - 9.0) / 1.6) ** 2)       # anillo del reflector
        dip = -0.10 * math.exp(-((a - 5.0) / 1.2) ** 2)       # zona algo mas oscura dentro del anillo
        v = base + ring + dip
        if a > 11.5:                                          # penumbra del HTML (0,5): baja suave hasta 23 deg
            t = (a - 11.5) / 11.5
            v *= 1 - t * t * (3 - 2 * t) * 0.92
        return v
    if a <= 30:                                               # halo tenue fuera del cono
        return 0.08 * (1 - (a - 23) / 7)
    return 0.0
vals = [max(0.0, cand(a)) * 1000 for a in ang]
lines = ['IESNA:LM-63-2002', '[TEST] MundoMagico', '[MANUFAC] Linterna HTML', 'TILT=NONE',
         '1 -1 1 %d 1 1 2 0 0 0' % len(ang), '1 1 0']
lines.append(' '.join('%.1f' % a for a in ang))
lines.append('0')
lines.append(' '.join('%.2f' % v for v in vals))
open(os.path.join(DST, 'IES_Linterna.ies'), 'w').write('\n'.join(lines) + '\n')
print('OK IES_Linterna.ies')

# ---------- SFX_Pagina.wav (1,1 s) ----------
SR = 44100
random.seed(7)
N = int(SR * 1.1)
out = [0.0] * N
lp = 0.0; hp_prev = 0.0; hp = 0.0
for i in range(N):
    t = i / SR
    # dos roces de papel (ruido filtrado pasa-banda con envolvente)
    env = 0.0
    for t0, d, a in ((0.0, 0.22, 1.0), (0.18, 0.30, 0.7)):
        if t0 <= t < t0 + d:
            u = (t - t0) / d
            env += a * math.sin(math.pi * u) ** 1.5
    n = random.uniform(-1, 1)
    lp += 0.35 * (n - lp)
    hp = lp - hp_prev; hp_prev = lp
    out[i] += hp * env * 1.4
    # nota grave misteriosa (la novela "responde"), entra suave
    if t > 0.12:
        u = t - 0.12
        e2 = min(1.0, u / 0.08) * math.exp(-u * 2.6)
        out[i] += 0.22 * e2 * (math.sin(2 * math.pi * 196 * u) + 0.5 * math.sin(2 * math.pi * 293.7 * u)
                               + 0.25 * math.sin(2 * math.pi * 392 * u + 0.4))
pk = max(abs(v) for v in out) or 1
with wave.open(os.path.join(DST, 'SFX_Pagina.wav'), 'wb') as w:
    w.setnchannels(1); w.setsampwidth(2); w.setframerate(SR)
    w.writeframes(b''.join(struct.pack('<h', int(v / pk * 0.8 * 32767)) for v in out))
print('OK SFX_Pagina.wav')
