# Compone "SFX_Latido.wav": un latido grave (lub-dub) de 1 s, en loop exacto y sin ruido.
# En el juego se acelera subiendo el pitch (BP_LatidoMiedo). Solo usa la libreria estandar.
import math, struct, wave, os

SR = 44100
N = SR  # 1.0 s = 60 lpm a pitch 1
TAU = 2 * math.pi
out = [0.0] * N

def golpe(t0, f0, f1, dur, amp):
    s0 = int(t0 * SR)
    for j in range(int(dur * SR)):
        u = j / SR
        f = f0 + (f1 - f0) * (u / dur)
        env = (1 - math.exp(-u * 300)) * math.exp(-u * 18)
        v = math.sin(TAU * f * u) + 0.35 * math.sin(TAU * 2 * f * u) * math.exp(-u * 40)
        i = s0 + j
        if i < N:
            out[i] += v * env * amp

golpe(0.02, 62, 38, 0.30, 1.0)    # lub
golpe(0.30, 70, 44, 0.25, 0.72)   # dub

pk = max(abs(v) for v in out) or 1
g = 0.9 / pk
dst = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'MUSICA_PARA_UNREAL', 'SFX_Latido.wav')
with wave.open(os.path.abspath(dst), 'wb') as w:
    w.setnchannels(1); w.setsampwidth(2); w.setframerate(SR)
    w.writeframes(b''.join(struct.pack('<h', int(max(-1, min(1, math.tanh(v * g * 1.2))) * 32767)) for v in out))
print('OK', os.path.abspath(dst))
