# Ferramentas de vídeo (Shorts)

Gera Shorts verticais (1080×1920) do Ferrugem 2D com imagens do jogo de verdade e música feita do zero.

```bash
SAIDA=/pasta/temporaria node shorts.js primeira     # ou: energia, evolucao
python3 musica_short.py musica.wav 32 118 11 C,G,Am,F 3,7.5,12,15.5,19,24,28
ffmpeg -framerate 30 -pattern_type glob -i "frames_primeira/*.jpg" -i musica.wav \
  -c:v libx264 -crf 19 -pix_fmt yuv420p -c:a aac -shortest Short-primeira.mp4
```

Precisa de Node + Playwright (Chromium), Python + NumPy e ffmpeg.
