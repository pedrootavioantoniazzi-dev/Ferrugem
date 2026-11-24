# Trilha curta para os Shorts, sintetizada do zero (sem samples).
# Uso: python3 musica_short.py saida.wav duracao bpm semente Am,F,C,G corte1,corte2,...
import numpy as np, wave, sys
out, DUR, BPM, seed = sys.argv[1], float(sys.argv[2]), float(sys.argv[3]), int(sys.argv[4])
PROGN = sys.argv[5].split(','); CUTS = [float(x) for x in sys.argv[6].split(',')]
SR = 44100; N = int(SR * DUR); L = np.zeros(N); Rc = np.zeros(N); rng = np.random.default_rng(seed)
BEAT = 60 / BPM; BAR = 4 * BEAT; END = DUR - 4.0; START = CUTS[0] if CUTS else 2.0
CH = {'Am': (55.0, [220.0, 261.63, 329.63]), 'F': (43.65, [174.61, 220.0, 261.63]), 'C': (65.41, [261.63, 329.63, 392.0]),
      'G': (49.0, [196.0, 246.94, 293.66]), 'Em': (41.2, [164.81, 196.0, 246.94]), 'Dm': (36.71, [146.83, 174.61, 220.0])}
def chord(t): return CH[PROGN[int(t // BAR) % len(PROGN)]]
def env(n, tau): return np.exp(-np.arange(n) / (tau * SR))
def add(s, t0, g=1.0, pan=0.0):
    i = int(t0 * SR); j = min(N, i + len(s))
    if i >= N or j <= i: return
    s = s[:j - i] * g; L[i:j] += s * (1 - max(0, pan)); Rc[i:j] += s * (1 + min(0, pan))
def saw(f, n, h=8):
    t = np.arange(n) / SR; o = np.zeros(n)
    for k in range(1, h + 1):
        if f * k > 9000: break
        o += np.sin(2 * np.pi * f * k * t) / k
    return o * .55
def lp(x, a):
    y = np.zeros_like(x); s = 0.0
    for i in range(len(x)): s += a * (x[i] - s); y[i] = s
    return y
b = 0
while b * BAR < END:
    t0 = b * BAR; _, notes = chord(t0); n = int((BAR + .5) * SR)
    a = np.minimum(1, np.arange(n) / (.3 * SR)) * np.minimum(1, (n - np.arange(n)) / (.5 * SR))
    sig = sum(saw(f * d, n, 6) for f in notes for d in (.997, 1.003)) / 6
    g = .26 if t0 < START else .14; add(sig * a, t0, g, -.25); add(sig * a, t0 + .012, g, .25); b += 1
n = int(1.6 * SR); x = rng.standard_normal(n); add((x - lp(x, .2)) * np.linspace(0, 1, n) ** 2.2, max(0, START - 1.6), .1)
def kick():
    n = int(.45 * SR); t = np.arange(n) / SR; f = 45 + 95 * np.exp(-t / .045); return np.sin(2 * np.pi * np.cumsum(f) / SR) * np.exp(-t / .16)
K = kick(); n = int(.08 * SR); HAT = np.diff(np.concatenate([[0], rng.standard_normal(n)])) * env(n, .018)
n = int(.25 * SR); CLAP = rng.standard_normal(n) * env(n, .07) * .6
t = START; k = 0
while t < END:
    add(K, t, .9); add(HAT, t + BEAT / 2, .1, .3)
    if k % 2 == 1: add(CLAP, t, .2, -.1)
    root, notes = chord(t)
    for h in range(2):
        m = int(BEAT / 2 * .95 * SR); add(saw(root * 2, m, 8) * env(m, .09), t + h * BEAT / 2, .3)
    for s in range(4):
        f = notes[(k * 4 + s) % 3] * (2 if (k // 2) % 2 else 1); m = int(.2 * SR); tt = np.arange(m) / SR
        add(2 / np.pi * np.arcsin(np.sin(2 * np.pi * f * tt)) * env(m, .06), t + s * BEAT / 4, .09, .35 if s % 2 else -.35)
    t += BEAT; k += 1
def boom(g):
    n = int(2 * SR); t = np.arange(n) / SR; low = np.sin(2 * np.pi * (42 + 30 * np.exp(-t / .08)) * t) * np.exp(-t / .7)
    return (low + lp(rng.standard_normal(n), .05) * 1.2 * np.exp(-t / .5)) * g
for c in CUTS: add(boom(.7), c, .7)
add(boom(1.0), END, .75)
n = int(4 * SR); t = np.arange(n) / SR; root, notes = chord(0)
fin = sum(saw(f, n, 8) for f in [root * 2, root * 3, root * 4] + notes) / 6; add(fin * np.minimum(1, t / .3) * np.exp(-t / 2.2), END, .5)
def room(x, ds):
    y = x.copy()
    for d, g in ds: k = int(d * SR); y[k:] += x[:-k] * g
    return y
L = room(L, [(.071, .28), (.157, .18)]); Rc = room(Rc, [(.083, .28), (.149, .18)])
fade = np.ones(N); fl = int(1.2 * SR); fade[-fl:] = np.linspace(1, 0, fl) ** 1.5; L *= fade; Rc *= fade
pk = max(abs(L).max(), abs(Rc).max()); L = np.tanh(L / pk * 1.25) / np.tanh(1.25) * .89; Rc = np.tanh(Rc / pk * 1.25) / np.tanh(1.25) * .89
st = np.empty(2 * N, np.int16); st[0::2] = (L * 32767).astype(np.int16); st[1::2] = (Rc * 32767).astype(np.int16)
with wave.open(out, 'wb') as w: w.setnchannels(2); w.setsampwidth(2); w.setframerate(SR); w.writeframes(st.tobytes())
print('ok', out, DUR)
