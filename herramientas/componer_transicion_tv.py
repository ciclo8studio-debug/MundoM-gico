# Compone los 2 sonidos de la transición hospital -> parque (solo librería estándar):
#  SFX_TV_Apagar.wav (1,4 s): "tuk" grave del tubo + pitido agudo de alta tensión que se va + chispa.
#  SFX_VHS_Play.wav  (2,4 s): clac del mecanismo + motor con "wow" + soplido de cinta filtrado (sin estática de TV).
import math, struct, wave, os, random

SR = 44100
TAU = 2 * math.pi
random.seed(11)
DST = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'MUSICA_PARA_UNREAL')

def guardar(nombre, out, pico=0.85):
    pk = max(abs(v) for v in out) or 1
    g = pico / pk
    ruta = os.path.abspath(os.path.join(DST, nombre))
    with wave.open(ruta, 'wb') as w:
        w.setnchannels(1); w.setsampwidth(2); w.setframerate(SR)
        w.writeframes(b''.join(struct.pack('<h', int(max(-1, min(1, v * g)) * 32767)) for v in out))
    print('OK', ruta)

# --- TV que se apaga ---
N = int(SR * 1.4)
tv = [0.0] * N
for i in range(N):
    t = i / SR
    # tuk: golpe grave descendente
    if t < 0.25:
        f = 90 - 50 * (t / 0.25)
        tv[i] += math.sin(TAU * f * t) * math.exp(-t * 18) * 1.0
    # pitido agudo (flyback ~8 kHz) que baja y se apaga
    f2 = 8200 - 2600 * t
    tv[i] += 0.18 * math.sin(TAU * f2 * t) * math.exp(-t * 2.6)
    # chispa breve al inicio
    if t < 0.04:
        tv[i] += random.uniform(-1, 1) * 0.5 * (1 - t / 0.04)
lp = 0.0
for i in range(N):
    lp += 0.55 * (tv[i] - lp)
    tv[i] = lp
guardar('SFX_TV_Apagar.wav', tv)

# --- VHS: play ---
N = int(SR * 2.4)
vhs = [0.0] * N
fase = 0.0
lp = 0.0
for i in range(N):
    t = i / SR
    # clac mecánico (dos golpes secos)
    for t0, a in ((0.02, 1.0), (0.16, 0.6)):
        if t0 <= t < t0 + 0.05:
            u = t - t0
            vhs[i] += (random.uniform(-1, 1) * 0.7 + math.sin(TAU * 1400 * u) * 0.5) * math.exp(-u * 90) * a
    # motor con wow (60 Hz que oscila)
    if t > 0.2:
        fase += TAU * (60 + 3 * math.sin(TAU * 0.7 * t)) / SR
        env = min(1.0, (t - 0.2) / 0.3) * (1 - max(0.0, (t - 1.9) / 0.5))
        vhs[i] += 0.22 * math.sin(fase) * env
        # soplido de cinta: ruido filtrado suave (no estática)
        n = random.uniform(-1, 1)
        lp += 0.06 * (n - lp)
        vhs[i] += lp * 1.6 * env
guardar('SFX_VHS_Play.wav', vhs, 0.7)
