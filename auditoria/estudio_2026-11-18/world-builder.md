# Estudio 18-11 · @world-builder (Mundo, Narrativa y Arte)

Solo lectura. Fuentes: CLAUDE.md, ARQUITECTURA_3_NIVELES.md, plan_hospital_horror.md, CHANGELOG (v0.3.40–0.3.60), BOARD.md, GUIA_TUNING.md, referencia_pelicula.md y `MundoMagico_Web/index.html` (código, sin base64).
Lentes: level-designer, world-builder, narrative-director, writer, art-director y technical-artist.

## 1. Resumen
- El juego HTML tiene un **look coherente y barato**: niebla exponencial azul-negra, luna fría, un post-proceso único (desaturación, tinte frío, curva S, viñeta, grano y aberración), niebla baja en tarjetas, polvo en el haz y una luz de rebote falsa. Ninguno de esos trucos necesita sombras ni Lumen.
- Unreal ya copió la linterna (v0.3.60), la transición TV/VHS, la Sombra, los escondites y las llaves. Pero el **Nivel 1 de la demo sigue sin construirse**: no hay 2.º piso, negatoscopio, morgue, LUT verdosa ni `DA_NovelaData` 1–15, y E4 cierra el 26-11.
- La narrativa del HTML (cintas, registro de Urgencias, diario de Alfredo, flashbacks y 3 finales) está escrita y probada, pero **casi no llegó al hospital de Unreal**.
- **Notas:** mundo 7 · narrativa 8 (HTML) / 5 (UE) · arte UE 5. La media sube a 7–8 si entran las mejoras 1 a 3, que son de bajo esfuerzo.

## 2. Hallazgos por agente
**level-designer**
- El hospital de Unreal es lineal (sala P4 → HX → Urgencias). Faltan el **bucle vertical** (2.º piso → sótano) y un punto de retorno, que son lo que da los 25–30 min previstos.
- El HTML marca el ritmo con **vigas y pilastras cada 7 m**, puertas azules con vanos negros y un **resplandor naranja al fondo** que guía. Ese faro de navegación se puede usar en Unreal sin flechas en el HUD.
- La Sombra y los 2 `BP_HideSpot` están en el mismo pasillo. El 2.º piso necesita su propio circuito de patrulla, con 3 escondites y 2 rutas.

**world-builder**
- Hay **canon fuerte**: 12/03/1998, Pabellón 4 · Psiquiatría, "Paciente I., 9 años", El Buen Samaritano, Alfredo, Rafaela y Luisa.
- **Choque de lore:** el HTML dice "corazón débil" y `referencia_pelicula.md` dice "leucemia terminal". Hay que fijar una sola versión antes de escribir las páginas 1–15.
- La Casona no tiene identidad en el lore. En el HTML, la "Casa de Alfredo" (llave entre disfraces y fusible) ya cumple ese papel, y la arquitectura dice que sus piezas se mudan a la Casona.

**narrative-director**
- Arco de la demo propuesto: despertar → registro de Urgencias (quién es "Paciente I.") → radiografía con el código → morgue y llave → salida → TV → **teaser: CINTA 1 · 23:41** en la Casona.
- Los 3 finales por páginas (verdadero con 62, "lo que recuerdas" con la mitad o más, y el corto) no se ven en la demo. Hay que **anunciar en la pantalla final las "15/62 páginas"** para crear deseo.

**writer**
- Hay textos listos para reutilizar: las 4 cintas con su hora, `REGISTRO DE URGENCIAS`, `DIARIO DE ALFREDO`, `CARTA SIN TERMINAR`, `LISTA DE PERSONAJES`, los murmullos de la TV ("...Isidora... despierta...") y los flashbacks `FB` de las páginas 2–20.
- Hace falta un **tono único en las notas de Unreal**: voz clínica (fichas y registros) en el hospital y voz íntima (cartas de Alfredo) en la Casona.

**art-director**
- **Paleta del hospital en el HTML:** muros verde-azulados descascarados (#4b6562), azulejo blanco sucio hasta 1,5 m, fluorescentes fríos (#9fb8e0), señal SALIDA verde, cruz roja, charcos casi espejo y cortinas azul marino. Calza con la "LUT verde y cian" del plan.
- El rojo se reserva para el peligro (balizas y ojos de la entidad). Hay que mantener esa regla y no usar rojo decorativo en el hospital.

**technical-artist**
- El post del HTML cabe en **un solo material de post-proceso** (unas 15 instrucciones) y su "glitch" está parametrizado con `amount`. Así sale más barato que sumar los efectos nativos de UE uno por uno.
- El HTML sube la exposición ×1,45 al apagar la linterna. En Unreal, la exposición fija (Min = Max) mata ese "acostumbrarse a la oscuridad". Se puede imitar con un segundo PostProcess de blend 0→1, sin auto-exposure.
- `Horror_Hospital` viene en 4K. `DefaultDeviceProfiles` ya limita a 2048/1024. Faltan instancias de material compartidas (un `MI` de azulejo y uno de muro) para bajar draw calls.

## 3. Brechas HTML → Unreal
| HTML (three.js) | Estado en UE | Cómo replicarlo (iGPU) |
|---|---|---|
| `FogExp2(#070b14, 0.03)` + fondo #03050a | Niebla del parque sí; en `L_Hospital` sin perfil propio | `ExponentialHeightFog`, color #070b14, densidad 0,02–0,05 y **sin volumétrica** en interiores |
| Luna #5577bb 0.55 + halo + rayos aditivos por ventanas | Parque con luna; hospital sin haces | Planos aditivos unlit #7a9be0 (opacidad 0,13, degradado) en las ventanas. 0 luces |
| Ambient #1a2438 0.45 | Variable por mapa | `SkyLight` bajo, tinte azul y intensidad fija; nada horneado de más |
| Post: desat 22 %, tinte (.93,.98,1.08), curva S 35 %, viñeta fuerte, grano, aberración | `PostProcessVolume` con sliders (viñeta, grano, saturación 0,85) | `M_PP_MundoMagico` con un parámetro `Glitch`, más la **LUT verdosa** en `L_Hospital` |
| Bloom 0.55 / umbral 0.85 | Bloom por defecto | Bloom 0,5 y umbral alto: solo brillan los tubos y la señal SALIDA |
| Exposición ×1,45 con la linterna apagada | Exposición fija | 2.º PostProcess (ExposureBias +0,5) con blend que sube al apagar (Timeline, sin Tick) |
| Niebla baja: 14 tarjetas 16×16 m, opacidad .17, #8fa0b4, giran | No existe | Planos unlit translúcidos con panner lento en la morgue y el sótano (≤8 por sala) |
| Polvo en el haz (130 partículas) + **luz de rebote** #dfe8ff 0.9 / 7 m | El haz tiene polvo; no hay rebote | Niagara CPU de 100 partículas pegado a la linterna, y una PointLight sin sombra en el impacto del haz (trace cada 0,1 s) |
| Fluorescentes con parpadeo, emisivo + luz cada tubo | Plan: emisivo y una luz cada 2 tubos | MI con parpadeo por `Time` (gratis), luces horneadas y solo 1–2 dinámicas parpadeando por pasillo |
| Charcos (rough .03, metal .6) + goteo `DR` | Suelo mojado previsto (rough 0,15) | Decal de charco con roughness 0,05 y sonido de gota (cue de 3D corto) |
| Silueta tras el vidrio de Urgencias; entidad que **daña si la miras** (`gaze`) | Isidora aparece y desaparece; payaso con luz roja/luz verde | Silueta estática que se oculta al entrar al cono de la linterna. El `gaze` sube el parámetro `Glitch` |
| Notas, registro, diario, cintas y flashbacks | `REC_Pagina*` sin `DA_NovelaData` | Tabla `DA_NovelaData` 1–15 con los textos del HTML (§4 #4) |

## 4. Top 6 mejoras priorizadas
| # | Mejora | Impacto | Esfuerzo | Costo iGPU | Responsable |
|:-:|---|:-:|:-:|:-:|---|
| 1 | **`M_PP_MundoMagico` + LUT verdosa en `L_Hospital`**: post del HTML en 1 material, `Glitch` expuesto y blend de oscuridad al apagar la linterna. Interruptor `Activar_PostHTML` | 5 | S | Muy bajo (1 pase) | technical-artist + art-director |
| 2 | **Atmósfera de interior**: niebla #070b14 sin volumétrica, tarjetas de niebla baja en el sótano y la morgue, rayos de luna aditivos y luz de rebote de la linterna | 5 | S–M | Bajo (0 sombras, 1 luz) | technical-artist |
| 3 | **Lenguaje de luz del hospital**: tubos emisivos que parpadean por material, faro naranja o SALIDA verde al fondo de cada tramo, rojo solo con peligro, y Lightmass horneado | 4 | M | Bajo (horneado) | art-director + level-designer |
| 4 | **Páginas 1–15 y notas en `DA_NovelaData`**, con los textos del HTML: registro de Urgencias, diario, ficha "Paciente I." y radiografía. Voz clínica | 4 | M | Nulo | writer + narrative-director |
| 5 | **2.º piso y sótano con circuito**: negatoscopio (radiografía de tórax de "Paciente I.", **código 0333**, como la hora de la cinta 4), patrulla de la Sombra con 3 escondites, morgue con `LlaveMorgue` y salida. Vigas cada 7 m | 5 | L | Medio (vigilar <1200 draw calls) | level-designer |
| 6 | **Teaser de la Casona (60–90 s)**: TV → VHS "▶ CINTA 1 · 12/03/1998 · 23:41", vestíbulo `UnrealBucket` en ámbar, una lámpara que muere y "Continuará", con 15/62 páginas en pantalla. Sin enemigo | 4 | M | Bajo | world-builder + narrative-director |

**Orden sugerido:** 1 y 2 el 19–20-11 (dan screenshots para itch.io), 4 en paralelo, 5 del 21 al 25-11 y 6 del 27-11 al 4-12. Todo con interruptor en `DA_Tuning` y medición de fps con `Probar.bat` antes y después.

## 5. Decisiones para el CTO
1. **Canon de Isidora:** ¿"corazón débil" (HTML) o "leucemia terminal" (película)? Afecta la ficha, la radiografía del negatoscopio y las páginas 1–15.
2. **¿La Casona es la casa de Alfredo (y Rafaela)?** Recomiendo que sí: reutiliza la llave entre disfraces, el fusible y las cartas, y le da sentido al teaser.
3. **¿Exposición dinámica al apagar la linterna?** Va con un PostProcess de blend, sin auto-exposure. Recomiendo que sí: es la sensación de "los ojos se acostumbran" del HTML y no cuesta GPU.
