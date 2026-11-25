# Acta de la reunión de los 5 superagentes — 25-11-2026
Tema: alinear la nueva historia (Mateo Salinas, su perro Chuño, «La Cofradía», ruta de Chile) y evaluar la biblioteca de Fab con toque chileno. Alcance: solo Blackline. Informes: `tech-lead.md`, `gameplay-engineer.md`, `world-builder.md`, `ux-toolsmith.md`, `qa-publisher.md`. Inventario: `INVENTARIO_FAB.md`.

## 1. Acuerdos (los 5 coinciden)
- **La historia es viable para el slice de 5 contratos** y casi toda ya corre en el juego como texto en pantalla.
- **Nunca importar un pack entero de Fab.** Se migra solo lo que se usa y se mide. Tope propuesto: **1,5 GB de Fab dentro del proyecto**; build ≤ 4 GB y descarga ≤ 2,5 GB (a confirmar).
- **No usar:** ChatAvatar (plugin de IA en la nube), packs de zombis, monstruos, ciencia ficción, feria, metro, MetaHuman y personas.
- **Linterna:** usar solo la V4; la V2 está duplicada (768 MB).
- **Chuño se siente por ausencia y sonido, nunca herido:** asiento vacío, collar en el espejo, prueba de vida por radio.
- **Lo que más falta es audio** (hoy no hay ningún sonido) y los assets propiamente chilenos, que **no existen en Fab**.

## 2. Contradicciones y cómo se resuelven (recomendación del acta)
| Tema | Posiciones | Resolución propuesta |
|---|---|---|
| Derelict (4,8 GB) y Underground (5,2 GB) | World-builder: «usar, son los mejores». Tech-lead y QA: «no importar». | **Piezas sueltas tras medir.** Derelict: ~30 mallas (galpones, cables) para minería y pesqueras. Underground: solo luminarias y módulos de túnel, y solo para C23 (túnel viejo). Nada de packs enteros. |
| Rocas estilizadas (11 MB) | Tech-lead: «usar». World-builder: «el estilo choca». | **Solo como relleno lejano** hasta tener rocas realistas. |
| Bienestar de Chuño | Gameplay y tech-lead: se llena solo. | **Inicio 65%, bono +5%** por entrega a tiempo. Topes por arco y margen de plazo decreciente (25% a 8%). |
| HUD actual vs «sin HUD» (doc 13) | UX: choca. | **Modo ayuda opcional**, activado por defecto en el slice. |
| Decisión moral y final de policía | Gameplay: fin del acto 3. Tech-lead: después del slice. | **Después del slice.** El slice cierra con 3 finales de texto (rescate, con costo, malo). |
| Lumen | Tech-lead: riesgo para 58 FPS. | **El slice sigue sin Lumen** (ya está así en el perfil liviano). Lumen se evalúa en una sola zona, más adelante. |

## 3. Packs de Fab: decisión consolidada
- **Usar (livianos):** FogArea, Free Thunder Sounds, iconos FREE15, FlashLight V4 (guardias), papeles de FREE Horror (manifiestos, sin los de sangre).
- **Usar recortado (migrar solo lo listado):** Apocalypse (decals de calzada, lámparas, señalética, conos y barreras), Untitled1590 (farolas de sodio), Free Shrubs (3-4 arbustos con LOD, sin viento experimental), Detective, SICKAMOUNT, Stylized, Animated (cajas, pallets, tambores, caja de madera para C12), Misc Props (mobiliario de fonda), Gettingt (props de bencinera), Derelict y Underground (ver arriba), SoulCity (solo si su estilo calza: gaviotas, luces de puerto).
- **No usar:** Horror Hospital (18,9 GB), Space Nebula, SUW, Turnstile, Carnival, Carousel, Roundabout, Claw Machine, Clownmon, Whisper, Sci-Fi Beast, Corpses, Horror Framework, PortalFX, ChatAvatar, First-Person Template, personas y MetaHuman.
- **Sin verificar:** el estilo de SoulCity y Gettingt se evaluó solo por nombres de assets. RadialWheel no aparece en el inventario.

## 4. Lo que falta (no está en Fab) — para la nube o compra
Prioridad 1: **quiltro canela Chuño** (modelo, animación, silueta en cabina), **camiones y buses chilenos, señales de la Ruta 5 y peaje**, **flora chilena** (cardón, palma chilena, araucaria, espino).
Prioridad 2: fondas y bencineras con letrero genérico (sin marcas reales), poblaciones y animitas, pesquera, minería.
Prioridad 3: aduana del paso, caracoles con nieve, **audio** (motor diésel, lluvia, quiltro, sirenas de policía ficticia, radio AM/FM chilena, cueca y cumbia libres de derechos), collar, pelota y foto de Chuño.

## 5. Toque chileno acordado
- **Mundo:** animitas con velas, afiche «SE BUSCA: quiltro canela» cada vez más viejo en los postes, camión volcado, fondas y picadas, pesqueras y minería, cuestas y caracoles, paso fronterizo.
- **Zonas:** Norte (desierto, salitreras), Centro (Santiago, Valparaíso, Lo Prado), Sur (lluvia, aserraderos, Puerto Montt), Cordillera (caracoles, nieve, aduana).
- **Radio:** 3 emisoras ficticias (AM, FM, cueca y cumbia). Jerga moderada, sin caricatura. La banda trata a Mateo de «usted».
- **Legal y tono:** policía ficticia, sin marcas ni logos reales, descargo de ficción y advertencias de contenido (crimen, extorsión, animal en peligro sin daño mostrado, fotosensibilidad). Revisar que «La Cofradía» no coincida con una empresa real.

## 6. Reglas que se adoptan
- **Arresto:** velocidad < 10 km/h, 2 o más patrullas a menos de 20 m, durante 4 s. Cuesta la carga, 25% de los viáticos y -20% de bienestar. Con 3 antecedentes, final malo.
- **Plazo:** reloj en pausa solo en la bencinera; la fonda da +5% de bienestar.
- **Prueba de vida:** pasa de «por arco» a **por contrato** (un contrato por arco en el slice).

## 7. Orden de trabajo acordado
1. Probar en pantalla la v0.1.20 (retén, patrullas, teclas, volcado).
2. Probar las zonas 1, 3 y 4 (`-zone=N`): carga, FPS y RAM.
3. **Audio mínimo:** motor, lluvia, sirena, trueno y 1 prueba de vida por contrato.
4. Menú de contratos y cambio de zona.
5. Arresto y finales de texto; retoque de bienestar y plazo.
6. **Fab recortado** (cajas, señalética, mobiliario de fonda, papeles), migrado de a poco midiendo.
7. Cabina: collar, pelota y foto; Chuño listo para el juego.
Cortes: decisión moral, pantalla propia de prueba de vida, clima completo, remapeo y localización completa, y los 20 contratos restantes (quedan como datos).

## 8. Decisiones que le corresponden al CTO
1. Slice **sin Lumen** (recomendado: sí).
2. **Tope de Fab de 1,5 GB** y migrar solo lo listado (recomendado: sí). ¿Se borra la linterna duplicada V2 (768 MB)? (recomendado: sí, con respaldo).
3. **Plazo en tiempo real** (ya implementado; recomendado: sí).
4. **Bienestar inicial 65%** y bono +5% (recomendado: sí).
5. **Decisión moral y final de policía después del slice** (recomendado: sí).
6. **HUD como modo ayuda** opcional (recomendado: sí).
7. **Collar de Chuño en el asiento del copiloto** (recomendado: sí).
8. **Origen del audio:** banco con licencia más grabación propia, o generado. Revisar derechos.
9. **Fecha y plataforma de publicación** (sigue abierto).
10. **Revisión de licencias de Fab e itch.io:** el informe de QA las dio por conocimiento general; hay que leer los textos vigentes antes de publicar.
