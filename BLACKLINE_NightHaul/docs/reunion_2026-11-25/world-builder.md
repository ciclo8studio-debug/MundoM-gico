# World-builder: Blackline, mundo y ambientación (25-11-2026)

Alcance: solo Blackline (Enzo, Pancito, La Cofradía, ruta en 4 zonas). Se ignora el contenido de otros juegos del estudio.

## 1. Alineación historia, mundo y arte
- **Contradicción a cerrar:** `04_nivel_vertical_slice` (muelle, bosque, barro) y `08_arte` (puerto neón) son un slice genérico. La historia pide 4 zonas y una ruta larga. El slice pasa a ser el **tutorial en Pudahuel/Ruta 68** (acto 1). Lluvia, retén y T1/T2/T3 se mantienen como encuentros, no como el mundo.
- **Paleta por zona** (sobre la base #0B1020 / sodio #FFA640; rojo solo para amenaza):

| Zona | Luz y color | Superficie | Clima | Ancla visual |
|---|---|---|---|---|
| Norte | Cielo estrellado, sodio escaso, luna fría | Asfalto seco, ripio, arena | Seco, viento, polvo | Cactus, minas, salitreras, animitas |
| Centro | Sodio denso, neón de ciudad, tráfico | Asfalto, túneles, cuestas | Despejado/nublado | Santiago (Costanera y autopista), Valparaíso (cerros, puerto), Lo Prado |
| Sur | Verde oscuro, sodio tenue, niebla | Asfalto mojado, barro, ripio | Lluvia constante | Pesqueras, aserraderos, Puerto Montt |
| Cordillera | Azul hielo, linterna, sin ciudad | Caracoles, nieve, ripio | Frío, nieve, niebla | Paso Los Libertadores, aduana, túnel |

- **Cabina como refugio cálido** (pilar de `08_arte`): el collar de Pancito, la pelota y la foto son los únicos objetos con luz propia. El asiento vacío del copiloto es el ancla emocional; recomiendo que sí vaya (pregunta abierta 4).
- **Lugares reales como hitos de ruta** (Ruta 5, Cuesta Lo Prado, Cuesta La Dormida, Pelluhue no; solo tramos nombrados en la historia). Cada lugarteniente = un "carácter" de zona: el Ingeniero es autopista y cámaras, el Arriero es cabina, nieve y silencio.
- **Narrativa ambiental:** animitas con velas (identidad chilena y pista de peligro), camión volcado, afiche "SE BUSCA: quiltro canela" cada vez más viejo en postes, radio con locución chilena.

## 2. Evaluación de packs Fab (para Chile de noche)
Veredicto: **A** = usar, **P** = parcial (solo piezas sueltas), **N** = descartar. Regla de peso: no importar packs enteros (Horror Hospital pesa 18 GB); migrar solo lo usado.

| Pack | Veredicto | Uso con toque chileno |
|---|---|---|
| Derelict (4,8 GB, 241 mallas) | **A** | Mejor pack: galpones, cables, soportes. Minería, pesqueras, bodegas de puerto, aduana vieja. Repintar óxido y sal. |
| Underground (5,2 GB, luces de túnel) | **A** | Túneles de Lo Prado, Chacabuco y paso cordillerano; luminarias de túnel (`BP_LightFixture01b_tunnel`). |
| Untitled1590 (lámparas planas/altas) | **A** | Farolas de sodio de ruta; variantes con parpadeo para sectores abandonados. |
| FogArea | **A** | Niebla local: Sur, caracoles de cordillera, bajo viaductos. |
| Free Shrubs (viento) | **A** | Espino y matorral nortino/central; retexturar a tonos secos. Falta flora chilena propia. |
| Thunder Sounds | **A** | Tormentas del Sur. Sumar lluvia y radio propias. |
| SoulCity (419 mallas, gaviota, agua) | **P** | Revisar el estilo antes: si es realista, puertos de Valparaíso y Puerto Montt (gaviotas, agua, luces parpadeantes). Si es estilizado, descartar. |
| Apocalypse Playground (carreteras, lámpara) | **P** | Solo decals de calzada y lámparas (`M_Road_*`). Fuera cancha y juegos. |
| Detective Office / SICKAMOUNT / Stylized / Animated Crate | **P** | Cajas, pallets, tachos y letreros (`BP_Billboard`): sirven para bodegas, patios y fondas. Fuera el mobiliario de oficina. |
| Misc Props UB | **P** | Sillones y vajilla para el interior de fondas y picadas. Fuera arañas y jarrones de lujo. |
| FREE Horror (materiales de papel) | **P** | Manifiestos, avisos de Pancito, boletas. Quitar los de sangre. |
| Horror Hospital (18 GB) | **P** | Solo `M_Flicker`, decals de suciedad, puertas y rejas para una posta rural. Nunca el pack entero. |
| Flashlight | **P** | Solo `BP_Flashlight` para guardias de retén y la linterna del Arriero. |
| Gettingt (aire acondicionado, props) | **P** | Fachadas y techos de edificios menores, si el estilo calza. |
| Untitled06 (SUW pasillos) / Turnstile (metro) | **N** | Metro y pasillos: no hay en la ruta. Torniquetes no valen como peaje. |
| FreePack Rocks (estilizadas) | **N** | Estilo choca con lo realista. Mejor rocas fotogramétricas para desierto y cordillera. |
| Carnival, Carousel, Roundabout, Claw Machine | **N** | Feria y parque. Tono equivocado para Blackline. |
| Corpses, Horror Framework, Clownmon, Whisper, Sci-Fi Beast, Space Nebula, PortalFX | **N** | Zombis, monstruos, ciencia ficción y portales. |
| Mannequins, Quinn, RoninGirl, PunkGirl, Vernal, Models x5, ChatAvatar | **P/N** | Maniquí solo de relleno (camioneros, guardias, hasta tener modelos propios). El resto, descartar. |
| First-Person template, AutoFoot, Essentials, Ultimate, 15 Icons | **N** | La cabina y el movimiento son propios. Revisar si Essentials/Ultimate traen sonidos o materiales útiles. |

## 3. Qué falta y no está en el inventario
Prioridad: **(1)** desbloquea la identidad de Chile, **(2)** zona clave, **(3)** pulido.

| Prioridad | Asset | Cómo conseguirlo |
|---|---|---|
| 1 | Quiltro canela, oreja caída (modelo, animación sentado/ladrido, y silueta en cabina) | Generar en la nube (Meshy o similar) y reanimar con esqueleto de perro |
| 1 | Camiones y buses chilenos (tractocamión, ramplas), señales de la Ruta 5, pórtico de peaje | Fab (vehículos) más material propio; las señales, generar por texto |
| 1 | Flora chilena: cardón, palma chilena, araucaria, espino, eucalipto, álamo | Quixel/Fab (cactus, coníferas) como base; ajustar por tinte |
| 2 | Fondas y picadas (ramada, banderas), bencineras con letrero genérico (sin marcas reales) | Kit modular propio sobre Derelict; carteles generados |
| 2 | Población (casas de albañilería, mediaguas, rejas, cables), animitas | Generar en la nube (modular, 5-8 variantes) |
| 2 | Pesquera (cajones, grúa, redes) y minería (tolvas, chimenea, camión minero) | Derelict + modelos generados |
| 3 | Paso fronterizo (aduana Los Libertadores), caracoles con nieve | Kit propio; terreno con heightmap |
| 3 | Audio: radio AM chilena, locutores, cuecas lejanas, ladridos de Pancito | Generar y grabar; **revisar derechos** |
| 3 | Collar, pelota y foto de Pancito para la cabina | Modelar a mano (bajo poly) |

## 4. Cinco entregables priorizados (para el CTO)
1. **Biblia de zonas** (1 pág. por zona): paleta, superficie, clima, hitos reales y qué lugarteniente manda. Con esto se cierra el desfase entre slice y ruta larga.
2. **Quiltro Pancito listo para el juego:** modelo, animación y silueta en la cabina (collar, pelota, foto). Es el corazón emocional y cuesta poco.
3. **Kit "Ruta 5 de noche":** farola de sodio, señales, camión, peaje, tachos, conos, decals de calzada, hechos con Untitled1590, Apocalypse (decals) y Derelict. Se usa en las 4 zonas.
4. **Pack de flora chilena** (cardón, palma, araucaria, espino) más variantes secas y mojadas.
5. **Zona Centro jugable:** Santiago y Valparaíso con SoulCity (si el estilo calza) más túnel con Underground. Es la zona con más contratos y la que testean primero.

**Riesgos:** (a) packs de 5 a 18 GB: migrar solo lo usado, por la RAM de 8 GB; (b) no usar marcas reales (Copec, Shell, etc.) en letreros; (c) confirmar el estilo de SoulCity y Gettingt antes de planificar sobre ellos.
