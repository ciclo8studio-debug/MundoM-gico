# Banda sonora del teaser "El ultimo turno", version de 39 s (cuerpo de 32 s + titulo de 7 s), solo libreria estandar.
#  0-10 s  vals de feria alegre (cajita musical + organillo, Do mayor, 3/4)
# 10-22 s  el vals se desafina, se ralentiza y se apaga (cinta estirada + filtro)
# 16-30 s  dron grave que crece; latido que se acelera desde 22 s
# 30-31.2  silencio total; 31.2 golpe grave con chirrido metalico
# 36-43 s  sobre el titulo: ultimas notas de cajita, muy desafinadas
import math, random, struct, wave, os

SR = 44100
DUR = 39.0
N = int(SR * DUR)
TAU = 2 * math.pi
random.seed(7)
out = [0.0] * N

def nota(f):  # semitonos desde Do4
    return 261.63 * 2 ** (f / 12)

def add(i, v):
    if 0 <= i < N:
        out[i] += v

# ---------- vals (con deformacion progresiva) ----------
def pitch_factor(t):          # afinacion: 1 hasta 9 s, cae a 0.80 a los 22 s
    if t < 9: return 1.0
    a = min(1.0, (t - 9) / 13)
    return 1.0 - 0.20 * a * a

def wow(t):                   # vaiven de cinta que crece
    a = 0.0 if t < 8 else min(1.0, (t - 8) / 12)
    return 1 + a * 0.012 * math.sin(TAU * 0.7 * t) + a * 0.004 * math.sin(TAU * 5.3 * t)

def gain_vals(t):             # se apaga entre 21 y 27 s
    if t < 18: return 1.0
    return max(0.0, 1 - (t - 18) / 6)

# Melodia original (Do mayor), 3/4: (grado en semitonos, duracion en tiempos)
mel = [(4,1),(7,1),(12,1), (11,2),(7,1), (9,1),(5,1),(9,1), (7,3),
       (4,1),(7,1),(12,1), (14,2),(12,1), (11,1),(9,1),(7,1), (12,3),
       (16,1),(14,1),(12,1), (11,2),(9,1), (7,1),(9,1),(11,1), (12,3)]
bajos = [0, 7, 5, 7, 0, 7, 5, 0, 9, 4, 5, 0]     # raiz de cada compas
acordes = {0:(4,7), 7:(11,14), 5:(9,12), 9:(12,16), 4:(7,11)}

def cajita(t0, f, g):
    s0 = int(t0 * SR)
    for j in range(int(SR * 1.2)):
        u = j / SR
        t = t0 + u
        if t >= DUR: break
        ff = f * pitch_factor(t) * wow(t)
        e = math.exp(-u * 4.0)
        v = math.sin(TAU * ff * u) + 0.35 * math.sin(TAU * 4.02 * ff * u) * math.exp(-u * 12)
        add(s0 + j, v * e * g * 0.16 * gain_vals(t))

def organillo(t0, f, dur, g):
    s0 = int(t0 * SR)
    for j in range(int(SR * dur)):
        u = j / SR
        t = t0 + u
        if t >= DUR: break
        ff = f * pitch_factor(t) * wow(t)
        env = min(1.0, u / 0.02) * min(1.0, (dur - u) / 0.05)
        v = math.sin(TAU * ff * u) + 0.33 * math.sin(TAU * 3 * ff * u) + 0.2 * math.sin(TAU * 5 * ff * u)
        add(s0 + j, v * env * g * 0.05 * gain_vals(t))

t = 0.3
beat = 0.43
i_mel = 0; comp = 0
while t < 24:
    # tempo que se estira desde 10 s
    b = beat * (1 + (0 if t < 10 else min(0.45, (t - 10) * 0.04)))
    raiz = bajos[comp % len(bajos)]
    organillo(t, nota(raiz - 24), b * 0.9, 1.0)
    for k in (1, 2):
        for st in acordes[raiz]:
            organillo(t + k * b, nota(st - 12), b * 0.7, 0.6)
    # melodia del compas
    resto = 3
    tt = t
    while resto > 0:
        g, d = mel[i_mel % len(mel)]
        cajita(tt, nota(g), 1.0)
        tt += d * b; resto -= d; i_mel += 1
    t += 3 * b; comp += 1

# filtro paso bajo progresivo sobre el vals (12 s -> 24 s)
y = 0.0
for i in range(N):
    tt = i / SR
    fc = 9000 if tt < 12 else max(700, 9000 - (tt - 12) * 700)
    a = 1 - math.exp(-TAU * fc / SR)
    y += a * (out[i] - y)
    out[i] = y

# ---------- dron grave (16 -> 30 s) ----------
for i in range(int(14 * SR), int(26.5 * SR)):
    tt = i / SR
    g = min(1.0, (tt - 14) / 7) ** 1.5
    d = (math.sin(TAU * 55.0 * tt) + 0.7 * math.sin(TAU * 58.27 * tt + 1.1)
         + 0.35 * math.sin(TAU * 77.78 * tt + 0.4) + 0.25 * math.sin(TAU * 110.0 * tt * (1 + 0.002 * math.sin(TAU * 0.3 * tt))))
    sub = math.sin(TAU * 36.0 * tt)
    out[i] += (math.tanh(d * 0.9) * 0.22 + sub * 0.12) * g

# ---------- latido que se acelera (22 -> 30 s) ----------
def golpe(t0, amp):
    s0 = int(t0 * SR)
    for off, gg, f0 in ((0.0, 1.0, 62), (0.26, 0.7, 70)):
        for j in range(int(SR * 0.3)):
            u = j / SR
            add(s0 + int(off * SR) + j, math.sin(TAU * (f0 - 24 * u) * u) * math.exp(-u * 14) * (1 - math.exp(-u * 300)) * amp * gg)

tl = 18.5
while tl < 26.2:
    a = (tl - 18.5) / 8
    golpe(tl, 0.35 + 0.35 * a)
    tl += 1.0 - 0.45 * a      # de 60 a ~110 lpm

# ---------- silencio total 30.0 -> 31.2 ----------
for i in range(int(26.5 * SR), int(27.8 * SR)):
    out[i] = 0.0

# ---------- golpe final 31.2 s ----------
s0 = int(27.8 * SR)
for j in range(int(SR * 3.5)):
    u = j / SR
    boom = math.sin(TAU * (48 - 18 * min(u, 1)) * u) * math.exp(-u * 1.6)
    metal = (math.sin(TAU * 1830 * u) + 0.6 * math.sin(TAU * 2417 * u) + 0.4 * math.sin(TAU * 3301 * u)) * math.exp(-u * 3.2) * 0.12
    ruido = (random.random() * 2 - 1) * math.exp(-u * 22) * 0.25
    add(s0 + j, (boom * 0.9 + metal + ruido) * min(1.0, u / 0.004))

# ---------- cajita final desafinada sobre el titulo ----------
for k, (tt, st) in enumerate(((34.6, 12), (35.5, 11), (36.4, 7), (37.4, 6))):
    s0 = int(tt * SR); f = nota(st) * 0.79
    for j in range(int(SR * 1.4)):
        u = j / SR
        ff = f * (1 + 0.015 * math.sin(TAU * 0.9 * (tt + u)))
        add(s0 + j, (math.sin(TAU * ff * u) + 0.3 * math.sin(TAU * 4.02 * ff * u) * math.exp(-u * 10)) * math.exp(-u * 3.0) * 0.14)

# fade final y normalizado
for i in range(int(37.5 * SR), N):
    out[i] *= max(0.0, (DUR - i / SR) / 1.5)
pk = max(abs(v) for v in out) or 1
g = 0.85 / pk
dst = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'MUSICA_PARA_UNREAL', 'Teaser_Vals_corto.wav')
with wave.open(os.path.abspath(dst), 'wb') as w:
    w.setnchannels(1); w.setsampwidth(2); w.setframerate(SR)
    w.writeframes(b''.join(struct.pack('<h', int(max(-1, min(1, v * g)) * 32767)) for v in out))
print('OK', os.path.abspath(dst))
