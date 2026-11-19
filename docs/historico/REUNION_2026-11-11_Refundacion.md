# Reunión de super-agentes · Refundación (11-11, noche)

**Pedido del CTO:** "Quiero empezar de cero con todos los recursos que ya tenemos."
**Opciones evaluadas:**
- **A.** Un mapa nuevo y limpio en el mismo proyecto.
- **B.** Un proyecto Unreal nuevo, migrando lo que sirve.
- **C.** Seguir con lo actual y aplicar el plan de mejora.

**Participantes:** @tech-lead, @gameplay-engineer, @world-builder, @ux-toolsmith y @qa-publisher. Cada uno revisó por su cuenta y solo en modo lectura.

---

## 1. Veredicto: **5 de 5 votan A**

Se arma un **mapa nuevo con un núcleo nuevo, dentro del mismo proyecto**. No se borra nada.

- **Por qué no B** (lo dicen los 5): con 8 GB de RAM, un proyecto nuevo obliga a migrar `Horror_Hospital` (18 GB), rehacer LFS, el plugin MCP, los ini e Input, y recompilar todos los shaders. Son 1–2 días sin nada jugable y el juego no mejora en nada.
  - *Lo que daría B se consigue igual en A:*
    - un cook limpio, cocinando solo el mapa nuevo con `DirectoriesToNeverCook`;
    - un render limpio, aplicando el Perfil A una sola vez.
  - Si el 17-11 llega hardware nuevo, pasar a B sale barato porque el código nuevo vive aislado en `/Game/MM`.
- **Por qué no C:**
  - `BP_ThirdPersonCharacter` pesa 10,9 MB, tiene unas 136 funciones y mezcla UI, opciones, guardado y lógica en Tick.
  - Hay coordenadas fijas por todo el mapa.
  - Con esa base, cada mejora cuesta más y el CTO nunca podría afinar solo.
- **El costo hundido, en los dos sentidos:**
  - No se tira lo que funciona: audio, diseño de sustos, Isidora, packs y scripts.
  - Tampoco se sigue engordando un Blueprint que nadie puede afinar.
- `L_Pabellon4` y la v0.3.40 quedan **congelados como respaldo publicable**, en la rama `v0.3.x`, y solo reciben hotfixes.

## 2. Qué se conserva, qué se rehace y qué se tira

| Se conserva | Se rehace limpio | Se tira o queda fuera del slice |
|---|---|---|
| Los packs Fab, sobre todo `Horror_Hospital`, `LampsPack`, `TA_Corpses`, `SM_Notes`, `SoulCity`, `Apocalypse`, `GV_FreeShrubs`, `FogArea`, `clown_monster`, `Whisper` y `Horror_Template` | **Jugador delgado** con componentes: `AC_Linterna`, `AC_Miedo`, `AC_Interaccion` y `AC_Sigilo` | Las mallas de Blender a la vista: puestos, hojas, conos, terreno y atracciones de primitivas |
| Todo el audio propio (alarma, latido, glitch, respiración, música), las SoundClasses, los reverbs y `SM_Silencio` | El **guardado** separado en `SG_Opciones` y `SG_Partida`, con número de versión | Las cajas `P4_*` y `HX_*`, y las 65 luces de la avenida |
| Los scripts `componer_*`, `importar_*`, `Construir_Version` y `Versionar_Unreal` | **UI** a cargo de `BP_UIManager`: HUD mínimo y opciones completas | Ray tracing, MegaLights, Substrate y Distance Fields (Perfil A) |
| El estilo de menú tipo afiche, la pantalla de carga y las vibraciones DS4 | El **disparo de sustos**, que pasa por `BP_DirectorMiedo` | Toda lógica en `Event Tick` y toda coordenada fija |
| El diseño de los sustos, el glitch, Isidora y la historia de IMAGINARIO | Las **páginas**: de 62 a unas 6, todas con historia | Del cook: `DetectiveOffice` (2,6 GB), `UnrealBucket` (2,5 GB) y los demás packs que no se usen |

## 3. Arquitectura nueva (consenso entre @tech-lead, @ux-toolsmith y @gameplay-engineer)

1. **`/Game/MM/`** guarda todo el código nuevo, sin referencias a lo viejo.
2. **`BP_MM_Character`** queda delgado (movimiento y cámara, menos de 15 funciones), y la lógica va en componentes.
3. **`DA_Tuning` desde el primer Blueprint.** Todo valor sale de ahí, con nombres en español y un interruptor `Activar_X` por feature.
4. **`BP_VolumenZona` con `ZonaID`.** Los sustos, la música, el reverb y el checkpoint reaccionan a la zona, nunca a coordenadas.
5. **`BP_DirectorMiedo`** maneja una tensión de 0 a 100 y un enfriamiento global. Los sustos le piden turno, y nunca se repite dos veces seguidas el mismo tipo: audio, visual, físico o payaso.
6. **El payaso** es la única amenaza real, con AIPerception y un BT tomado de `Horror_Template`. Acecha cuando no lo miras, investiga los ruidos y persigue si te ve.
7. **Banderas en la línea de comandos:**
   - `-ir=<zona>`: salta directo a una zona de prueba;
   - `-dev`: muestra un overlay con fps, zona, batería y el próximo susto.
8. **Presupuesto para la iGPU:**
   - 30 fps fijos;
   - como máximo 3 luces con sombra a la vista (la linterna manda) y 10 sin sombra;
   - iluminación horneada con Lightmass;
   - texturas de 1024 como máximo;
   - menos de 1 200 draw calls;
   - un ZIP de 1,5 GB como máximo.

## 4. El juego refundado

**Los 4 pilares (@gameplay-engineer):**
1. **La linterna es tu vida y te delata.** La batería escasea. Un destello aturde al payaso, pero gasta el 25 % de la carga. Con la luz encendida te ven desde el doble de distancia.
2. **Ruido y sigilo.** Correr, abrir puertas y lanzar botellas hace ruido; agachado no se oye nada.
3. **El payaso acecha.** Es un solo antagonista, bien hecho.
4. **Esconderse y aguantar la respiración.** Es la única defensa cuando no hay luz.

**Dirección de arte en 5 reglas (@world-builder):**
1. El negro es la base: cerca del 70 % de la pantalla va bajo el 10 % de brillo.
2. Solo dos temperaturas: azul acero para la luna o el exterior, y ámbar para la linterna y los emisivos. El rojo se reserva para el peligro.
3. Un solo foco por encuadre.
4. El suelo siempre está mojado (roughness 0,1–0,2).
5. Ni una primitiva ni una malla de Blender a la vista: si no viene de un pack, no entra.

**Recorrido del slice, unos 180 m:**

| Zona | Tamaño | Packs |
|---|---|---|
| H1 · Sala de camas | 12×20 m | `Horror_Hospital`, `TA_Corpses` |
| H2 · Pasillo de Urgencias | 4×36 m | `Horror_Hospital`, `LampsPack` (tubos fluorescentes), `SICKA` |
| H3 · Recepción | 8×10 m | `Horror_Hospital`, `SM_Notes` |
| T · Túnel de servicio (el glitch) | 3×40 m | `UndergroundTunnel` (en PacksSinUsar), `NY_Subway`, `FogArea` |
| P1 · Entrada del parque | 30×20 m | `Apocalypse`, `GV_FreeShrubs`, el letrero |
| P2 · Avenida | 10×60 m | `SoulCity`, rocas, `FogArea` |
| Atracciones | plazas de 25 m | **Carousel** y **Ferris Wheel**, por bajar de Fab |

Cada zona es un **Level Instance**. El CTO las mueve como bloque, con rejilla de 1 m y un kit de 40 piezas aprobadas en `Slice/Kit`.

**Kit del CTO desde el día 1 (@ux-toolsmith):**
- `DA_Tuning`;
- `-ir=` y `-dev`;
- `Probar.bat`, que lanza el juego con `-game` sin cook ni build de 15 min (hay que medir si cuelga la GPU);
- `BP_Zona` y piezas modulares ya armadas;
- la guía `GUIA_TUNING.md`;
- dos tipos de build: Development para probar, con consola, y Shipping solo para publicar.

## 5. Hitos (cada uno deja una build jugable)

| Fecha | Build | Contenido | Control |
|---|---|---|---|
| 12–13-11 | — | El CTO juega la v0.3.40 como **línea base medida**. Se arma `/Game/MM`, `DA_Tuning`, `-ir`/`-dev` y `Probar.bat`. | Prueba de humo |
| **14-11** | v0.4.0-a | Personaje nuevo con linterna e interacción, sobre un blockout gris del recorrido. Se miden los fps. | CTO |
| 15–16-11 | — | Hospital H1–H3 con `Horror_Hospital`, volúmenes de zona y checkpoints. | |
| 17-11 | — | **Decisión de hardware.** ¿Se pasa a B? `AC_Miedo` y el Director. | |
| 18–19-11 | v0.4.0-b | Payaso con BT (acecho e investigación), escondites, se portan los sustos y el audio. | **Timebox:** si no supera al hospital actual, se vuelve a C con los componentes ya hechos |
| 20–22-11 | v0.4.1 | Túnel, entrada del parque, descompresión, Director con 6 sustos variados. | |
| **26-11** | v0.4.1 | **Puerta de decisión:** debe superar a la v0.3.40 en al menos 4 de los 7 criterios. | 2.ª ronda de testers |
| 3-12 | v0.4.5 | Avenida y atracciones. ZIP de 1,5 GB como máximo. | 3.ª ronda, candidata |
| 8-12 | — | Congelamiento de 48 h. | Regresión |
| **10-12** | v0.5.0 | Lanzamiento público. | |

**Criterios de salida del slice (@qa-publisher)**, que deben igualar o superar a la v0.3.40:

| Criterio | Meta |
|---|---|
| fps con la Iris Plus en calidad Baja | ≥30 de promedio y ≥20 en el 1 % más bajo |
| Cierres | 0 |
| Carga del menú | <30 s |
| Carga del nivel | <20 s |
| ZIP | ≤1,5 GB |
| Testers que terminan | ≥80 % |
| Nota | ≥3,5 |
| Zonas donde 2 o más testers se pierden | 0 |

## 6. Reglas desde el día 1
- **Una build al día**, con 1–2 features cada una, y cada feature con su interruptor y su línea en la checklist.
- **Prohibido:** usar `Event Tick` en Blueprints nuevos, poner coordenadas fijas y agregar luces con sombra fuera del presupuesto.
- **Etiqueta git** por versión. Se conservan solo las últimas 5 builds más la publicada.
- **Nada se valida por sensación:** cada hito se compara en una tabla contra la línea base.

## 7. Decisiones del CTO

1. **¿Se aprueba la refundación A?** Los 5 agentes la aprueban.
2. **¿Qué pasa en itch.io el 16-11?** Hay dos posturas:
   - **(a)** @tech-lead y @gameplay-engineer: publicar la v0.3.40 como estaba previsto.
   - **(b)** @qa-publisher: **no publicarla**. Seguir en modo restringido con 8–10 testers y abrir al público con el slice nuevo, para no gastar la primera impresión en algo que se va a reemplazar.
3. **Perfil A ahora**, es decir, apagar ray tracing, MegaLights, Substrate y Distance Fields. La refundación lo necesita desde el día 1. *Atención:* antes se revirtió a la espera de más RAM. Recompilar los shaders se haría una sola vez, de noche.
4. **Alcance de la demo v0.5.0.** @gameplay-engineer propone 15 min: hospital, glitch, avenida con 3 interruptores, Carrusel con la primera persecución y huida al castillo.
5. **Recortes:**
   - el oso, el león y la muñeca (queda solo el payaso);
   - los puzzles del monolito;
   - las páginas, de 62 a unas 6;
   - el motion matching, que es demasiado pesado para la Iris Plus.
   ¿Se aprueban?
