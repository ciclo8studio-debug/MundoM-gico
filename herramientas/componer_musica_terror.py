# Compone "Musica_Terror_Menu.wav": pista de terror suave, sin ruido (nada de estatica).
# Capas: dron grave con batido disonante, sub-grave que se siente, acorde disonante que respira,
# cajita musical de feria desafinada y lejana, y un latido muy suave de vez en cuando.
# Uso: python herramientas/componer_musica_terror.py   (solo libreria estandar)
import math, random, struct, wave, os

SR = 44100
DUR = 96.0                      # segundos (loop)
N = int(SR * DUR)
TAU = 2 * math.pi
random.seed(13)
out = [0.0] * N

def add(i, v):
    if 0 <= i < N:
        out[i] += v

# 1) Dron grave: La1 + Sib1 (segunda menor -> batido inquietante) + Mib2 (triton)
for i in range(N):
    t = i / SR
    lfo = 0.6 + 0.4 * math.sin(TAU * t / 23.0)
    d = (math.sin(TAU * 55.0 * t) + 0.7 * math.sin(TAU * 58.27 * t + 1.1)
         + 0.35 * math.sin(TAU * 77.78 * t + 0.4))
    d = math.tanh(d * 0.9) * 0.16 * lfo
    sub = math.sin(TAU * 36.0 * t) * 0.10 * (0.5 + 0.5 * math.sin(TAU * t / 31.0 + 2.0))
    out[i] += d + sub

# 2) Acorde disonante que aparece y desaparece (cuerdas frias)
pad = [261.63, 277.18, 369.99, 392.0]
for i in range(N):
    t = i / SR
    env = max(0.0, math.sin(TAU * t / 48.0 - 0.6)) ** 2
    if env < 1e-4:
        continue
    v = 0.0
    for k, f in enumerate(pad):
        vib = 1 + 0.003 * math.sin(TAU * (0.17 + 0.05 * k) * t)
        v += math.sin(TAU * f * vib * t + k) + 0.25 * math.sin(TAU * 2 * f * vib * t)
    out[i] += v * 0.012 * env

# 3) Cajita musical de feria, desafinada y lejana (con eco)
mel = [659.25, 587.33, 523.25, 493.88, 523.25, 440.0, 415.30, 440.0]   # menor, con 2da aumentada
t0 = 6.0
while t0 < DUR - 8:
    frase = random.randint(3, 6)
    for n in range(frase):
        f = mel[(n + random.randint(0, 2)) % len(mel)] * (0.985 + random.random() * 0.01)
        start = int((t0 + n * 0.62) * SR)
        for eco, g in ((0, 1.0), (0.31, 0.45), (0.62, 0.22), (0.93, 0.1)):
            s0 = start + int(eco * SR)
            for j in range(int(SR * 1.6)):
                u = j / SR
                e = math.exp(-u * 3.2) * g
                wob = 1 + 0.004 * math.sin(TAU * 5.5 * u)
                v = (math.sin(TAU * f * wob * u) + 0.3 * math.sin(TAU * 3.01 * f * u) * math.exp(-u * 9))
                add(s0 + j, v * e * 0.035)
    t0 += frase * 0.62 + random.uniform(9, 16)

# 4) Latido lejano (dos golpes graves), de vez en cuando
t0 = 14.0
while t0 < DUR - 4:
    for off, g in ((0.0, 1.0), (0.28, 0.7)):
        s0 = int((t0 + off) * SR)
        for j in range(int(SR * 0.35)):
            u = j / SR
            add(s0 + j, math.sin(TAU * (52 - 18 * u) * u) * math.exp(-u * 14) * 0.13 * g)
    t0 += random.uniform(11, 19)

# Loop sin corte: mezcla los ultimos 4 s sobre los primeros 4 s
X = int(SR * 4)
for j in range(X):
    a = j / X
    out[j] = out[j] * a + out[N - X + j] * (1 - a)
out = out[:N - X]

# Normaliza bajo (pico -9 dB) para que se perciba suave
pk = max(abs(v) for v in out) or 1
g = 0.35 / pk
dst = os.path.join(os.path.dirname(__file__), '..', 'MUSICA_PARA_UNREAL', 'Musica_Terror_Menu.wav')
with wave.open(os.path.abspath(dst), 'wb') as w:
    w.setnchannels(1); w.setsampwidth(2); w.setframerate(SR)
    w.writeframes(b''.join(struct.pack('<h', int(max(-1, min(1, v * g)) * 32767)) for v in out))
print('OK', os.path.abspath(dst), round(len(out) / SR, 1), 's')
