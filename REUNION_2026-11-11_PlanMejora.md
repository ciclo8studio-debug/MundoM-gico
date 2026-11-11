# Reunión de super-agentes · plan de mejora rápida (11-11, noche)

**Pedido del CTO:**
- El juego mejoró, pero hay que darle una vuelta y tratarlo como el mejor juego que hemos hecho.
- Revisar completo lo hecho y lo que viene, y proponer mejoras a corto plazo.
- El CTO quiere ajustar detalles a mano.

**Participantes:** @tech-lead, @gameplay-engineer, @world-builder, @ux-toolsmith y @qa-publisher. Las cinco revisiones fueron independientes y solo de lectura.

---

## 1. En qué coincidieron (4 o 5 de 5)

1. **Se congelan las features hasta validar.** La v0.3.40 trae unas 10 novedades y nadie la ha jugado entera. Desde ahora solo entran arreglos, hasta tener un playtest limpio.
2. **El CTO no puede ajustar nada sin Claude.** Los valores de diseño están escritos dentro de 6–8 Blueprints, y cada ajuste cuesta editar por MCP, 15 min de build y 30 min de juego. Los cinco proponen un **kit de afinación** (punto 3).
3. **Los sustos repiten el mismo molde y nadie controla el ritmo.** El payaso, Isidora y la silueta hacen lo mismo: aparecer lejos y desaparecer. El teatro y la noria también son iguales entre sí. Además, cada susto lleva su propio reloj.
4. **El hospital, que es el primer minuto del juego, se ve por debajo de lo que podría.** El pack **`Horror_Hospital` ya está instalado** (740 mallas) y casi no se usa: el mapa no toma de ahí ni una pared, puerta, techo o lámpara. No hace falta esperar al pack Derelict Corridor.
5. **El rendimiento y la descarga no corresponden al hardware.**
   - Siguen activos el ray tracing y MegaLights, y se sumaron muchas luces volumétricas.
   - El ZIP pesa 3,3 GB para una demo de unos 15 min, porque se cocinan packs que no se usan.
   - `Build/` ocupa **144 GB** con 44 builds.

## 2. Hallazgos que hay que revisar ya

- 🔴 **El teatro y la bodega se cerraron con rejas en la 0.3.0.** El susto nuevo del teatro y 2 de las 3 Isidoras podrían quedar **fuera del recorrido**. Hay que confirmarlo en la prueba del 12-11.
- 🟠 **En la demo corta no hay ninguna amenaza real.** Los enemigos solo actúan en la persecución, que queda fuera de la demo, así que los escondites y la respiración no sirven para nada.
- 🟠 **La luz va al revés.** La avenida tiene unas 65 luces y es justo donde hay enemigos. La referencia de arte es casi todo negro, con un solo punto de luz y la linterna mandando.
- 🟠 **La página de itch.io no calza con el build:**
  - nombra la v0.3.35;
  - promete 25–35 min;
  - no dice nada de SmartScreen;
  - el teaser recomendado es el que tiene el cielo negro, e itch no acepta MP4, así que hay que subirlo a YouTube.
- 🟡 Quedan primitivas a la vista: hojas planas, puestos que son cajas, conos usados como pasto y el suelo marrón liso.

## 3. El kit de afinación para el CTO (consenso)

| Pieza | Qué es | Horas |
|---|---|:-:|
| **`DA_Tuning`** | Un Data Asset con unos 40 valores en español: batería, payaso, Isidora, latido, sustos, cámara y luces. Incluye **un interruptor por feature** para apagar en minutos lo que falle. Se edita en el editor, en el panel Details. | 6 |
| **Saltos de prueba** | `-ir=teatro\|noria\|parque\|bodega`, `-dev` y `-susto=Teatro` al abrir el `.exe`, usando los puntos de respawn que ya existen. Así se prueba una zona en 2 min en vez de 30. | 4 |
| **`Probar.bat`** | Un menú del 1 al 6 que abre el juego en la zona elegida, más un overlay `-dev` con fps, zona, batería y tiempo hasta el próximo susto. | 3 |
| **`GUIA_TUNING.md`** | Una página con cada valor, dónde se toca, su rango seguro y si necesita build. También explica las luces y el post-proceso a mano. | 2 |

> **Resultado:** el CTO ajusta y prueba solo con el control de PS4. Claude queda para la lógica nueva.

## 4. Plan de 7 días

| Día | CTO | Claude y agentes |
|---|---|---|
| **12-11** | Playtest de la v0.3.40 con `checklist_humo.md` + `checklist_v0.3.40.md`, desde **Iniciar juego**. Medir fps. Confirmar si se llega al teatro y a la bodega. | Corregir la página de itch.io (v0.3.40, duración real, SmartScreen). Preparar `DirectoriesToNeverCook` para bajar el tamaño, sin build. |
| **13-11** | **itch.io en modo restringido**, solo para 3–5 testers con claves (al menos 2 con GPU integrada) y un formulario de 8 preguntas. | Kit de afinación: `DA_Tuning` + saltos de prueba + `Probar.bat` + guía. |
| **14-11** | Primeros ajustes a mano con el kit. | **v0.3.41**: arreglos del playtest, kit y tamaño reducido. Una sola build. |
| **15-11** | Revisar la v0.3.41. | **Hospital con `Horror_Hospital`**: paredes, puertas, techo, zócalos y lámparas, suelo mojado, tubos fluorescentes que parpadean. |
| **16-11** | **Lanzamiento público** si los testers terminan sin cierres. Si no, se publica la v0.3.35. | Arreglos de los testers. |
| **17-11** | **Decisión de hardware.** | `BP_DirectorMiedo`: un enfriamiento global entre sustos y tramos de calma. Los sustos piden permiso antes de dispararse. |
| **18-11** | Sesión de afinación del ritmo con el Director. | Variar los sustos (el teatro pasa a ser solo de audio, Isidora deja pistas), cambiar las primitivas y hacer la pasada de luz en 3 niveles. |

**Más adelante, en el camino a la v0.5.0:**
- una amenaza real y suave en la demo, con el payaso que avanza cuando no lo miras;
- cambiar las coordenadas fijas por volúmenes de zona;
- pasar la lógica del Tick a timers;
- reemplazar las atracciones por los packs Carousel y Ferris Wheel.

## 5. Reglas nuevas de proceso

1. **Una build al día, sin excepciones.** La excepción del 11-11 terminó siendo la build más riesgosa.
2. **Entran 1–2 features por build**, cada una con su interruptor en `DA_Tuning` y su línea en la checklist.
3. **Se congela 48 h antes de publicar.** En ese tiempo solo se arreglan bugs.
4. **Los Blueprints nuevos no llevan `Event Tick` ni coordenadas fijas.**
5. **Primero ajusta el CTO, después se le pide a Claude:** los valores se afinan en `DA_Tuning`, y a Claude solo se le pide lógica nueva.
6. **Higiene de builds:** etiqueta git por versión y se guardan solo las últimas 5 builds más la publicada.

## 6. Decisiones del CTO

1. **Fecha de publicación:**
   - **(a)** 13-11 en modo restringido y 16-11 público. La recomiendan @qa-publisher y @gameplay-engineer.
   - **(b)** 13-11 público con la v0.3.40, si la checklist sale limpia. Esta es la postura de @tech-lead.
2. **Prioridad después del kit:** el hospital con `Horror_Hospital` (@world-builder) o el Director de miedo (@gameplay-engineer).
3. **Perfil A parcial:** apagar el ray tracing y MegaLights y bajar a calidad Scalable. Son unas 3 h, y se puede hacer ya o esperar al 17-11.
4. **Borrar builds viejas** para liberar más de 100 GB, conservando las últimas 5 y la publicada. Claude prepara la lista y el CTO confirma antes de borrar.

---
Las revisiones completas de cada agente están en el chat del 11-11. Los informes anteriores siguen en `auditoria/<agente>.md`.
