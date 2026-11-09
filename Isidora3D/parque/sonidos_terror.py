# Sonidos de terror organicos (sin tonos electronicos). Uso: blender -b --python sonidos_terror.py
import wave
import numpy as np

OUT = r"C:/Users/pansh/Desktop/MundoMagico_Project/Isidora3D/parque/audio/"
SR = 44100
rs = np.random.RandomState(13)


def guardar(n, x, est=False):
    x = x / (np.abs(x).max() + 1e-9) * .92
    if est and x.ndim == 1:
        x = np.column_stack([x, x])
    x = (x * 32767).astype('<i2')
    with wave.open(OUT + n + ".wav", "wb") as w:
        w.setnchannels(2 if x.ndim == 2 else 1)
        w.setsampwidth(2)
        w.setframerate(SR)
        w.writeframes(x.tobytes())
    print("OK", n, round(len(x) / SR, 2))


def fft_filtro(x, curva):
    X = np.fft.rfft(x)
    f = np.fft.rfftfreq(len(x), 1 / SR)
    return np.fft.irfft(X * curva(f), len(x))


def banda(x, lo, hi):
    return fft_filtro(x, lambda f: 1 / (1 + (lo / np.maximum(f, 1)) ** 4) / (1 + (f / hi) ** 4))


def convolver(a, b):
    n = len(a) + len(b)
    return np.fft.irfft(np.fft.rfft(a, n) * np.fft.rfft(b, n), n)


def reverb(x, seg=1.4, mezcla=.35, oscuro=4500, pre=.012):
    """Sala de azulejos: ecos tempranos + cola oscura."""
    n = int(seg * SR)
    t = np.arange(n) / SR
    ir = rs.randn(n) * np.exp(-t * 6.9 / seg)
    ir = fft_filtro(ir, lambda f: 1 / (1 + (f / oscuro) ** 2))
    for d, g in ((.011, .7), (.019, .55), (.031, .45), (.047, .3), (.071, .22)):
        ir[int(d * SR)] += g * 18
    ir = np.r_[np.zeros(int(pre * SR)), ir]
    y = convolver(x, ir)
    y = y / (np.abs(y).max() + 1e-9) * np.abs(x).max()
    y[:len(x)] = y[:len(x)] * mezcla + x * (1 - mezcla)
    y[len(x):] *= mezcla
    return y


def formantes(x, F):
    """Tracto vocal: suma de resonancias (frecuencia, ancho, ganancia)."""
    def c(f):
        r = np.zeros_like(f)
        for fc, bw, g in F:
            r = r + g / (1 + ((f - fc) / (bw / 2)) ** 2)
        return r + .02
    return fft_filtro(x, c)


def voz(f0, jit=.03, ruido=.3):
    """Pulso glotal irregular a partir de una curva de tono."""
    n = len(f0)
    w = banda(rs.randn(n), 3, 28)
    w = w / (np.abs(w).max() + 1e-9)
    fr = f0 * (1 + jit * w + jit * .4 * rs.randn(n))
    ph = np.cumsum(fr) / SR
    fase = ph % 1
    g = np.where(fase < .42, np.sin(np.pi * fase / .42) ** 2, 0) - .18
    g = np.diff(g, prepend=0) * 30
    amp = 1 + .5 * banda(rs.randn(n), 12, 70) / .05 * .05
    return g * amp + ruido * rs.randn(n) * np.clip(np.abs(g) * 3 + .25, 0, 1)


def env(t, puntos):
    return np.interp(t, [p[0] for p in puntos], [p[1] for p in puntos])


def chillido():
    d = 2.3
    t = np.arange(int(d * SR)) / SR
    f = env(t, [(0, 210), (.07, 760), (.3, 980), (.75, 1120), (1.3, 830), (1.75, 450), (2.3, 170)])
    capas = 0
    for k, (m, g) in enumerate(((1, 1), (1.021, .75), (.497, .85), (.33, .5))):
        v = voz(f * m, .05 + .012 * k, .42)
        a = formantes(v, [(850, 240, 1), (1650, 340, .8), (3100, 520, .55), (4300, 800, .26)])
        b = formantes(v, [(480, 180, 1), (2350, 340, .95), (3400, 540, .5)])
        mz = env(t, [(0, 0), (.55, 1), (1.2, .35), (2.3, .9)])
        capas = capas + g * (a * mz + b * (1 - mz))
    capas = capas / np.abs(capas).max()
    capas = np.tanh(capas * 3.2)  # garganta desgarrada
    cruj = banda(rs.randn(len(t)), 1200, 7000) * (rs.rand(len(t)) < .012) * 2.2
    y = (capas + cruj * env(t, [(0, 1), (.8, .35), (2.3, 1)])) * env(t, [(0, 0), (.035, 1), (1.6, .9), (2.25, .12), (2.3, 0)])
    guardar("SFX_T_Chillido", reverb(y, 1.7, .36))


def respiracion():
    d = 9.0
    t = np.arange(int(d * SR)) / SR
    y = np.zeros(len(t))
    pos = .15
    while pos < d - 1.3:
        ins = rs.uniform(.65, .95)
        sal = rs.uniform(.8, 1.1)
        for k, (dur, F, tono, g) in enumerate(((ins, [(1250, 620, 1), (2650, 900, .75), (4300, 1400, .32)], 148, .85), (sal, [(650, 340, 1), (1450, 620, .6), (2600, 900, .22)], 92, 1.0))):
            n = int(dur * SR)
            u = np.linspace(0, 1, n)
            v = voz(np.full(n, tono) * (1 + .2 * u * (1 - 2 * k)), .09, 2.1) * .55 + rs.randn(n) * .85
            s = formantes(v, F) * np.sin(np.pi * u) ** .7 * g
            s = s * (1 + .5 * np.sin(2 * np.pi * 31 * u * dur) * (rs.rand() > .4))  # flema
            i = int(pos * SR)
            y[i:i + n] += s[:len(y) - i]
            pos += dur + .06
        pos += rs.uniform(.12, .32)
    guardar("SFX_T_Respiracion", reverb(y, .75, .24))


def pisada(g=1.0):
    n = int(.17 * SR)
    t = np.arange(n) / SR
    golpe = banda(rs.randn(n), 60, 520) * np.exp(-t * 48)
    piel = banda(rs.randn(n), 900, 4200) * np.exp(-t * 130) * .42
    hueso = np.sin(2 * np.pi * (92 + rs.uniform(-10, 10)) * t) * np.exp(-t * 36) * .7
    return (golpe + piel + hueso) * g


def carrera():
    d = 2.5
    y = np.zeros(int(d * SR))
    pos, paso = .02, .36
    while pos < d - .2:
        g = .25 + .75 * pos / d
        s = pisada(g * rs.uniform(.82, 1.1))
        i = int(pos * SR)
        y[i:i + len(s)] += s
        if rs.rand() < .55:  # garras contra el azulejo
            u = banda(rs.randn(int(.03 * SR)), 2500, 9000) * .2 * g
            j = i + int(.05 * SR)
            y[j:j + len(u)] += u
        pos += paso * rs.uniform(.9, 1.1)
        paso = max(paso * .955, .16)
    guardar("SFX_T_Carrera", reverb(y, 1.15, .42))


def golpe():
    d = 1.5
    t = np.arange(int(d * SR)) / SR
    cuerpo = banda(rs.randn(len(t)), 28, 240) * np.exp(-t * 11)
    sub = np.sin(2 * np.pi * (50 * np.exp(-t * 2.2) + 24) * t) * np.exp(-t * 8)
    y = cuerpo * 1.6 + sub * 1.5 + banda(rs.randn(len(t)), 650, 5200) * np.exp(-t * 60) * .75
    for k in range(13):  # cosas que caen y rebotan
        i = int(rs.uniform(.05, .85) * SR)
        m = int(.05 * SR)
        y[i:i + m] += banda(rs.randn(m), 700, 4800) * np.exp(-np.arange(m) / m * 7) * rs.uniform(.05, .22)
    guardar("SFX_T_Golpe", reverb(np.tanh(y * 1.8), 1.3, .3))


def glitch():
    d = 1.5
    n = int(d * SR)
    t = np.arange(n) / SR
    # cinta danada: pedazos de chillido y estatica que se cortan
    v = voz(env(t, [(0, 120), (.4, 600), (.9, 90), (1.5, 45)]), .07, .8)
    v = formantes(v, [(700, 300, 1), (1900, 500, .7), (3300, 700, .4)])
    v = v / np.abs(v).max()
    est = banda(rs.randn(n), 300, 7500)
    est = est / np.abs(est).max()
    corte = np.repeat(rs.rand(n // 1400 + 2) > .42, 1400)[:n].astype(float)
    y = (v * .85 * corte + est * (.42 + .6 * (1 - corte)) * env(t, [(0, 1), (1.15, .75), (1.5, 0)]))
    y = np.round(y * 14) / 14
    y += np.sin(2 * np.pi * (46 - 18 * t) * t) * env(t, [(0, 0), (.3, .55), (1.05, .8), (1.5, 0)])
    guardar("SFX_T_Glitch", reverb(y * env(t, [(0, 0), (.015, 1), (1.45, 1), (1.5, 0)]), .85, .22))


def ambiente():
    d = 48.0
    n = int(d * SR)
    t = np.arange(n) / SR
    tono = banda(rs.randn(n), 22, 180) * .5  # aire del edificio
    tono = tono / np.abs(tono).max() * .32
    y = tono + banda(rs.randn(n), 280, 1500) * .012
    # tubo fluorescente viejo, casi inaudible y con fallas
    z = (np.sin(2 * np.pi * 100 * t) + .4 * np.sin(2 * np.pi * 200 * t)) * .012 * (.45 + .55 * (banda(rs.randn(n), .1, 1.5) > 0))
    y = y + z
    eventos = np.zeros(n)
    for k in range(34):  # goteras
        i = int(rs.uniform(.5, d - 1) * SR)
        m = int(.09 * SR)
        u = np.arange(m) / SR
        eventos[i:i + m] += np.sin(2 * np.pi * rs.uniform(900, 1700) * u * (1 + 2.2 * u)) * np.exp(-u * 55) * rs.uniform(.03, .08)
    for c in rs.uniform(3, d - 7, 7):  # metal que cruje a lo lejos
        m = int(rs.uniform(1.4, 3) * SR)
        u = np.arange(m) / SR
        f = rs.uniform(120, 240) * (1 + .22 * banda(rs.randn(m), .6, 7) / .03)
        tr = (np.cumsum(f) / SR % 1) < .13
        s = banda(tr.astype(float) - .13, 170, 2100) * np.sin(np.pi * u / u[-1]) ** 2 * rs.uniform(.12, .3)
        i = int(c * SR)
        eventos[i:i + m] += s[:n - i]
    for c in rs.uniform(5, d - 7, 4):  # lamento humano muy lejos
        m = int(rs.uniform(2.4, 4) * SR)
        u = np.linspace(0, 1, m)
        v = voz(150 + 45 * np.sin(np.pi * u) - 30 * u, .05, .5)
        s = formantes(v, [(520, 200, 1), (1100, 280, .4)])
        s = s / np.abs(s).max() * np.sin(np.pi * u) ** 2 * rs.uniform(.07, .14)
        i = int(c * SR)
        eventos[i:i + m] += s[:n - i]
    for c in rs.uniform(4, d - 3, 4):  # portazo o carro a lo lejos
        m = int(.5 * SR)
        u = np.arange(m) / SR
        i = int(c * SR)
        eventos[i:i + m] += banda(rs.randn(m), 60, 500) * np.exp(-u * 14) * rs.uniform(.25, .5)
    y = y + reverb(eventos, 2.4, .72, 2600)[:n]
    f = int(3 * SR)
    r = np.linspace(0, 1, f)
    y[:f] = y[:f] * r + y[n - f:] * (1 - r)  # empalme para loop
    guardar("AMB_T_Hospital", y[:n - f])


import os
os.makedirs(OUT, exist_ok=True)
chillido(); respiracion(); carrera(); golpe(); glitch(); ambiente()
print("SONIDOS_FIN")
