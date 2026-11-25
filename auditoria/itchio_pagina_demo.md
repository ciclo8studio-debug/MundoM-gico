# Página de itch.io — Demo v0.5.0 (pública el 10-12-2026; testers privados hacia el 26-11)

**Responsable:** `@qa-publisher` · **Estado:** borrador al día (24-11) para copiar y pegar. El CTO publica.
**Versión a subir:** la RC que pase el humo (hoy v0.3.134_dev); la pública será la v0.5.0.
**Alcance real (Plan B, 23-11):** hospital → glitch y TV → pantalla **FIN DE LA DEMO**. El parque y la Casona **no** están en esta demo; no se prometen.
**Por confirmar por el CTO:** (1) cámara: BOARD dice 1ª persona como modo principal (3ª opcional); (2) duración real, hoy estimada en 30–40 min; (3) teclas de agacharse.

---

## Título
**Mundo Mágico: Turno Nocturno — Demo**

## Lema (1 línea)
*Despiertas en el Pabellón 4. Algo en la oscuridad ya sabe que estás despierta.*

## Descripción corta (120 caracteres o menos)
Terror psicológico: un hospital vacío, una linterna que delata y una sombra que te busca. Juega con audífonos.

## Descripción
> Isidora despierta en una sala del Pabellón 4 sin recordar cómo llegó. El hospital está vacío, las luces fallan y algo recorre los pasillos. Lee las páginas que dejaron atrás, cuida las pilas de tu linterna y, sobre todo, **no hagas ruido**.
>
> Al final del recorrido, un televisor encendido espera.

**En esta demo (Nivel 1):**
- 🏥 La sala del despertar, el pasillo de Urgencias y el Pabellón 4.
- 🔦 Una linterna que ilumina… y también delata. Apuntarle a la Sombra puede hacerla huir.
- 🍾 Botellas para distraer lo que te busca, con una barra que muestra cuánto ruido haces.
- 📄 Páginas de la novela, subtítulos de los sustos y música que sube con la tensión.
- ⏱️ Unos **30–40 minutos** de juego.

**Características:**
- Linterna con batería y pilas que se cambian a mano.
- Sonido que viene de su lugar y un latido que avisa cuando algo se acerca.
- Guardado automático cada 60 segundos y al entrar a cada zona.
- Teclado y mouse, o mando completo con vibración (PlayStation o Xbox).
- Textos y subtítulos en español.

## Requisitos (mínimos probados)
| | Mínimo | Recomendado |
|---|---|---|
| SO | Windows 10/11 64-bit | Windows 11 64-bit |
| CPU | Intel Core i7-1065G7 | 6 núcleos o más |
| RAM | 8 GB | 16 GB |
| GPU | Intel Iris Plus (calidad **Baja**) | NVIDIA GTX 1660 / RTX 2060 o superior |
| DirectX | 11 | 11/12 |
| Disco | 5 GB libres (ZIP de ~1,3 GB + juego descomprimido) | SSD |

> Con tarjeta integrada, juega en **Calidad Baja**, que viene así por defecto. Meta: 25 fps o más (hospital ≈ 30 en la v0.3.84; se remide con la RC).

## Controles
Teclado: **WASD** moverse · mouse mirar · **F** linterna · **E** interactuar o recoger · **Y** pista · **T** lanzar botella · **Shift** correr · agacharse: `[CONFIRMAR]`.
Mando: ✕ saltar · □ interactuar · R2 lanzar botella.

## Aviso de Windows (pegar al final de la descripción)
> **Windows puede mostrar "Windows protegió su PC".** El juego no tiene firma digital porque somos un estudio independiente. Para jugar, haz clic en **Más información** y luego en **Ejecutar de todas formas**.

## Etiquetas
`horror`, `survival-horror`, `atmospheric`, `psychological-horror`, `exploration`, `singleplayer`, `spanish`, `unreal-engine`, `demo`

## Capturas (5) — de la versión que se sube
Usa **Win + Alt + Impr Pant** (se guardan en `Videos\Capturas`).
1. La sala del despertar, con la linterna encendida.
2. El pasillo de Urgencias en penumbra.
3. La Sombra huyendo de la linterna.
4. La portada del menú (mano y niebla).
5. El televisor del final.

Portada de la página: `Fuentes/UI/T_PortadaCine.png` (arte de la novela, autorizado por IMAGINARIO). Video: teaser en **YouTube como no listado** (itch.io pide enlace, no acepta MP4); revisar que el teaser no muestre el parque.

## Antes de publicar (checklist)
- [ ] Humo de la RC jugado por el CTO ([`humo/sprintB_2026-11-24.md`](humo/sprintB_2026-11-24.md)), sin cierres.
- [ ] ZIP con `herramientas/Empaquetar_ZIP.ps1` (1,21 GB en 165 s).
- [ ] Subida con butler probada en canal privado `windows-rc1` ([`guia_publicar_itchio.md`](guia_publicar_itchio.md)).
- [ ] 5 capturas y portada.
- [ ] 26-11 testers (página restringida con clave) → 10-12 pasar a **Público**.
