# Propuestas @qa-publisher: QA, lanzamiento y público (reunión 2026-11-09)

Objetivo: pasar de "demo que no se cae" a "demo que la gente termina, recomienda y agrega a su wishlist". Las cifras externas llevan fuente. Lo que no pude verificar está marcado **(sin verificar)**.

**TOP 3:** ⭐ P1 (playtests + métricas de abandono), ⭐ P3 (página de Steam + Next Fest de junio de 2027), ⭐ P6 (Fondo Audiovisual, línea Videojuegos 2027).

---

## ⭐ P1. Playtests con jugadores chilenos y métricas de abandono
- **Por qué:** la demo se validó con 32 min sin cierres, pero solo la ha jugado el CTO. No sabemos en qué punto la gente se pierde o se aburre (los 4 interruptores y la avenida en penumbra son los sospechosos).
- **Qué:** 5 a 8 testers chilenos externos por versión. Grabación de pantalla y voz (OBS), más el `checklist_humo.md` sección F como encuesta. En el build: un log local `telemetria.csv` con timestamp por hito (despertar, linterna, pasillo, glitch, cada interruptor, cinemática y castillo), muertes, uso de pilas y salida del juego. Es solo local y opcional (el tester envía el archivo), sin servicios externos.
- **Métrica clave:** el % que llega a cada hito (embudo) y el tiempo hasta el primer interruptor. Meta: al menos 70 % termina la demo corta.
- **Impacto:** alto, porque decide qué arreglar antes del 10-12. **Esfuerzo:** M.
- **Cuándo:** la ronda 1 con el build del 13-11 (sprint 14 al 20-11) y la ronda 2 con la v0.5.0 RC (sprint del 1 al 7-12).

## P2. Checklist de release y "gate" de build
- **Por qué:** cada build Shipping toma unos 15 min y la iGPU no corre PIE, así que un build roto cuesta un día. Ya existe el checklist de humo, pero no hay un criterio de "no se publica si...".
- **Qué:** convertir `checklist_humo.md` en un gate de 3 niveles. **Bloqueante:** crash, quedarse atascado o perder el guardado. **Mayor:** texto sin traducir o un susto que no se dispara. **Menor:** visual. Agregar una matriz de hardware mínima: la iGPU Iris Plus de 8 GB actual más el PC del partner (decisión del 17-11), y anotar los FPS en la avenida y en la cinemática. Mantener los `BP_Hiding_Door` fuera hasta que pasen el gate.
- **Impacto:** alto (estabilidad = reseñas). **Esfuerzo:** S.
- **Cuándo:** el sprint actual, antes del 13-11.

## ⭐ P3. Embudo itch.io, luego página de Steam con wishlist, luego Steam Next Fest
- **Por qué:** itch.io sirve para recibir feedback, mientras que las wishlists de Steam determinan la visibilidad al lanzar. Next Fest es la mayor vitrina gratuita para demos.
- **Fechas verificadas (Valve, calendario publicado en julio de 2026):**
  - Next Fest de **febrero de 2027: del 22-02 al 01-03**. La **inscripción cierra el 10-01-2027**.
  - Next Fest de **junio de 2027: del 14 al 21-06**. Hay otra edición prevista para octubre de 2027.
  - Fuentes: [Steamworks feb 2027](https://partner.steamgames.com/doc/marketing/upcoming_events/nextfest/feb_2027), [Steamworks jun 2027](https://partner.steamgames.com/doc/marketing/upcoming_events/nextfest/june_2027) y [PC Gamer](https://www.pcgamer.com/games/steam-reveals-all-the-sale-dates-and-themed-events-for-the-first-half-of-2027/).
- **Recomendación:** apuntar a **junio de 2027**, no a febrero. Con la v0.5.0 el 10-12 y sin validar, febrero deja menos de un mes de wishlists y sin pulido. Para junio hay que tener la página de Steam publicada ("Coming Soon") como mínimo unos meses antes, para acumular wishlists **(Valve exige que la página esté publicada antes del festival; verificar el plazo exacto en Steamworks)**.
- **Pasos:**
  1. itch.io el 13-11 (ya hay borrador en `itchio_pagina_demo.md`) con un enlace "Wishlist en Steam" apenas exista la página.
  2. Pagar el Steam Direct fee y crear la app en diciembre.
  3. Página Coming Soon en enero de 2027 con tráiler y 5 capturas.
  4. Demo en Steam en abril o mayo.
  5. Inscripción a Next Fest de junio.
- **Impacto:** muy alto. **Esfuerzo:** M.
- **Cuándo:** itch el 13-11, la app de Steam en el sprint del 8 al 14-12 y la página pública en enero de 2027.

## P4. Steam Deck y mando como argumento de venta
- **Por qué:** el juego ya soporta el mando PS4 completo y corre en calidad Baja en una iGPU. Eso lo hace un buen candidato a Steam Deck Verified, que da visibilidad en la tienda del Deck.
- **Requisitos verificados:** texto de al menos **9 px a 1280×800** (se recomiendan 12 px), glifos que correspondan al dispositivo activo (no mostrar teclas de teclado con el mando) y entrada de texto usable con el mando (el candado con código ya cumple). Fuente: [Steamworks: Steam Deck compat](https://partner.steamgames.com/doc/steamdeck/compat). El tamaño de texto es la falla más citada (dato anecdótico, [GamingOnLinux](https://www.gamingonlinux.com/2026/01/steam-machine-verification-will-have-fewer-constraints-than-steam-deck-but-text-sizing-worries-me/)). Según Valve, lo que es Verified en Deck también es Verified en Steam Machine.
- **Qué:** una prueba de UI a 1280×800, el swap de glifos ✕/□ ↔ E/Espacio y la fuente de respaldo para los glifos faltantes (ya detectado: BarlowCondensed sin ↑↓✕%).
- **Impacto:** medio-alto. **Esfuerzo:** S-M.
- **Cuándo:** sprint del 15 al 21-12.

## P5. Localización: inglés global, con el chileno como identidad
- **Por qué:** sin inglés el alcance en Steam y con YouTubers internacionales es mínimo. Pero lo que diferencia al juego es Chile 1998, así que la traducción no debe "neutralizarlo".
- **Qué:**
  - Migrar los textos hardcodeados del BP del jugador a `ST_Textos` (StringTable), con las cultures `es` (nativa) y `en`. Esto ya lo recomendé en la auditoría y es un requisito técnico previo.
  - Escribir una **guía de estilo**: los nombres propios no se traducen ("Mundo Mágico", "Pabellón 4", "LAS OCHO" se queda en el fonógrafo y lleva subtítulo). Los chilenismos se conservan en el VO o en los textos del mundo, con una nota en inglés. Las 62 notas y las 4 VHS se traducen al final, primero la UI.
  - Traducción del inglés por un nativo (pagado o en un intercambio de la comunidad) y QA de texto (desbordes) en las dos lenguas.
- **Impacto:** alto. **Esfuerzo:** L (por el volumen de notas).
- **Cuándo:** la StringTable en el sprint del 24 al 30-11. La UI en inglés para la v0.5.0. Las notas completas antes de la demo de Steam (abril de 2027).

## ⭐ P6. Financiamiento: Fondo de Fomento Audiovisual, línea de Producción de Videojuegos 2027
- **Por qué:** es la fuente pública principal para videojuegos en Chile y puede costear el inglés, el audio y QA, o un vertical slice.
- **Encontrado:**
  - Las bases de la **convocatoria 2027, línea Producción de Videojuegos**, están publicadas en [fondosdecultura.cl (Rex 2214)](https://www.fondosdecultura.cl/wp-content/uploads/2026/07/Rex2214-Videojuegos.pdf). Los recursos están "sujetos a disponibilidad presupuestaria".
  - En el concurso **regional 2027** hay una categoría **"Semilla Gamedev"** (vertical slice o postproducción): [bases regional](https://www.fondosdecultura.cl/wp-content/uploads/2026/07/produccion-regional-audiovisual.pdf).
  - Hay líneas de **Mercados** (ferias y encuentros) para viajar o exponer: [bases mercados audiovisuales 2026](https://www.fondosdecultura.cl/wp-content/uploads/2026/08/bases-mercados-audiovisuales-2026.pdf).
- **No verifiqué** los montos, los plazos de cierre ni si la convocatoria 2027 sigue abierta hoy. **Hay que leer las bases esta semana**, porque estos concursos suelen cerrar entre agosto y octubre del año anterior. Si ya cerró, la meta es la convocatoria 2028 y la de Mercados.
- **Qué:** que el CTO revise la elegibilidad (persona natural o empresa) y preparar la carpeta (GDD, presupuesto, carta Gantt y video de la demo corta). La demo del 13-11 sirve como evidencia.
- **Otras:** Corfo y otros concursos **(sin verificar, no busqué bases vigentes)**.
- **Impacto:** muy alto. **Esfuerzo:** M.
- **Cuándo:** revisar las bases el 10-11. Postular en la próxima ventana abierta.

## P7. Comunidad: TikTok y creadores chilenos de terror
- **Por qué:** el survival horror indie crece con los let's plays. Una demo corta (unos 30 min) cabe en un video, y un susto en el pasillo con sonido 3D se presta para un clip.
- **Qué:**
  - En TikTok, Shorts y Reels: 2 clips por semana (el glitch, la sombra bajo el letrero, la cinemática y el "devlog de un estudio unipersonal con un notebook de 8 GB", que es una historia en sí misma). Hashtags en español e inglés.
  - Lista de 20 a 30 YouTubers y streamers chilenos y latinos de terror, con su tamaño de canal, email y si juegan demos. Enviar una clave o enlace con un presskit de 1 página. **No incluyo nombres porque no los verifiqué.** La lista se arma a mano revisando YouTube y Twitch con las etiquetas "terror", "horror" y "Chile".
  - Un Discord simple, con un canal de bugs vinculado al gate de P2.
- **Impacto:** alto. **Esfuerzo:** M (constante).
- **Cuándo:** el primer clip y la primera tanda de envíos con el lanzamiento en itch (del 13 al 20-11). Segunda tanda con la v0.5.0.

## P8. Prensa, presskit y eventos chilenos
- **Qué:**
  - Presskit (presskit() o una página en itch): ficha, 6 capturas, logo, GIF, tráiler de 60 a 90 s, historia del estudio y contacto.
  - Notas de prensa a medios chilenos de videojuegos (LevelUp Chile, CNN Chile Bits y otros que ya cubren estudios locales).
  - Eventos: **Festigame 2026** (venta de entradas abierta en octubre de 2026, [G5 Noticias](https://g5noticias.cl/2026/10/05/comienza-la-venta-de-entradas-para-festigame-2026/)). Antes ha abierto postulaciones para que los devs chilenos muestren sus juegos ([LevelUp](https://www.levelup.com/noticias/devs-chilenos-pueden-postular-a-festigame-para-tener-acceso-gratuito-los-tres-dias-y-mostrar-sus-juegos-al-publico/)). Hay que confirmar si la edición 2026 tiene zona indie y cuál es su plazo.
  - Afiliarse a **VG Chile**, que articula las delegaciones a GDC, Gamescom y Tokyo Game Show con MINCAP y ProChile ([CNN Chile TGS 2026](https://www.cnnchile.com/bits/chile-en-la-tokyo-game-show-2026-los-estudios-y-titulos-que-representaran-al-pais-en-la-feria-de-videojuegos-mas-importante-de-japon/)).
  - "Chile Game Showcase 2026 / Indie Checkpoint" aparece en una sola fuente (LinkedIn), así que está **sin verificar**.
- **Impacto:** medio-alto. **Esfuerzo:** S (presskit) a M (eventos).
- **Cuándo:** el presskit con el lanzamiento en itch (13-11). Eventos y VG Chile en el sprint del 24 al 30-11.

## P9. Rating y edad
- **Chile:** la **Ley 20.756 (2014)** regula la venta y el arriendo de videojuegos "excesivamente violentos" a menores de 18 años. El Consejo de Calificación Cinematográfica califica, con etiquetado por decreto (2017-2018). Fuentes: [BCN Ley Fácil](https://www.bcn.cl/portal/leyfacil/recurso/calificacion-de-videojuegos) y [Wikipedia](https://en.wikipedia.org/wiki/Chilean_video_game_content_rating_system). Apunta sobre todo a la venta física y comercial. **No verifiqué** si aplica a una distribución digital gratuita; consultar la BCN o al Consejo antes de vender.
- **Steam:** usa su propio cuestionario de contenido (self-rating), no IARC. Algunos países exigen una clasificación aparte: Alemania (USK) e Indonesia (IGRS). IARC es gratis para los devs, pero solo funciona en tiendas que lo integran. Fuentes: [globalratings FAQ](https://globalratings.com/faq/) y [heise](https://heise.de/-9960810).
- **Qué:** declarar con honestidad violencia, sangre y miedo intenso, más una advertencia de fotosensibilidad (por el glitch y las luces que parpadean). En itch, marcar el contenido y la edad sugerida **16+**. Agregar una opción de accesibilidad para reducir el parpadeo.
- **Impacto:** medio (evita bloqueos y es parte de la confianza). **Esfuerzo:** S.
- **Cuándo:** la advertencia de fotosensibilidad el 13-11. El cuestionario de Steam al crear la app (diciembre).

---

## Calendario resumido
| Fecha | Hito |
|---|---|
| 10-11 | Leer las bases del Fondo Audiovisual 2027 y el gate de release (P2, P6) |
| 13-11 | itch.io + presskit + advertencia de fotosensibilidad + primer clip (P3, P7, P8, P9) |
| 14 al 20-11 | Playtest de la ronda 1 + embudo de abandono (P1) |
| 24 al 30-11 | StringTable `es`/`en`, VG Chile y eventos (P5, P8) |
| 1 al 7-12 | Playtest de la ronda 2 con la v0.5.0 RC (P1) |
| 8 al 21-12 | App de Steam, cuestionario de rating y prueba de UI en Steam Deck (P3, P4, P9) |
| Ene 2027 | Página Coming Soon + tráiler (P3) |
| Abr-may 2027 | Demo en Steam con inglés completo (P3, P5) |
| 14 al 21-06-2027 | Steam Next Fest (P3) |
