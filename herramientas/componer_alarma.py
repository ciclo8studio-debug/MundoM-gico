# Compone "SFX_Alarma.wav": alarma de emergencia de hospital viejo, 6 s, sin loop.
# Dos tonos que suben y bajan (sirena lenta) por un parlante roto (saturado y filtrado);
# al final la corriente falla: el tono cae y se apaga. Solo usa la libreria estandar.
import math, struct, wave, os, random

SR = 44100
DUR = 6.0
N = int(SR * DUR)
TAU = 2 * math.pi
random.seed(7)
out = [0.0] * N
fase = 0.0
lp = 0.0
hp_prev_in = 0.0
hp_prev_out = 0.0
for i in range(N):
    t = i / SR
    # sirena: 0.9 s por ciclo entre 620 y 880 Hz
    ciclo = 0.5 - 0.5 * math.cos(TAU * t / 0.9)
    f = 620 + 260 * ciclo
    # falla de corriente en los ultimos 1.6 s: el tono cae
    if t > DUR - 1.6:
        u = (t - (DUR - 1.6)) / 1.6
        f *= (1 - 0.65 * u * u)
    fase += TAU * f / SR
    # onda cuadrada suavizada (timbre de bocina)
    v = math.tanh(3.5 * math.sin(fase)) + 0.25 * math.sin(2 * fase)
    # zumbido electrico de 50 Hz
    v += 0.12 * math.sin(TAU * 50 * t)
    # parlante roto: saturacion + crujidos
    v = math.tanh(v * 2.2)
    if random.random() < 0.0015:
        v += random.uniform(-0.8, 0.8)
    # filtro pasa bajos (parlante chico) y pasa altos (sin graves)
    lp += 0.18 * (v - lp)
    hp = 0.97 * (hp_prev_out + lp - hp_prev_in)
    hp_prev_in, hp_prev_out = lp, hp
    # envolvente: entra de golpe, cortes de corriente, se apaga al final
    env = min(1.0, t / 0.03)
    if 2.6 < t < 2.75 or 4.1 < t < 4.18:
        env *= 0.15
    if t > DUR - 1.6:
        env *= max(0.0, 1 - ((t - (DUR - 1.6)) / 1.6)) ** 1.5
    out[i] = hp * env

pk = max(abs(v) for v in out) or 1
g = 0.85 / pk
dst = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'MUSICA_PARA_UNREAL', 'SFX_Alarma.wav')
with wave.open(os.path.abspath(dst), 'wb') as w:
    w.setnchannels(1); w.setsampwidth(2); w.setframerate(SR)
    w.writeframes(b''.join(struct.pack('<h', int(max(-1, min(1, v * g)) * 32767)) for v in out))
print('OK', os.path.abspath(dst))
