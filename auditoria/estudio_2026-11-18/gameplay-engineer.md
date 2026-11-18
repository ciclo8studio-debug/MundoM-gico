# Estudio 18-11 · @gameplay-engineer (Gameplay)
Fuentes: CLAUDE.md, ARQUITECTURA_3_NIVELES.md, CHANGELOG v0.3.41–0.3.60, GUIA_TUNING.md, BOARD.md, plan_hospital_horror.md y `MundoMagico_Web/index.html` (código leído por funciones: `PRO.ent`, `PRO.tick`, `clownTick`, `objective`, `saveGame`, `throwBottle`, `moveFx`). Solo lectura, sin Unreal.

## 1. Resumen
- **Unreal ya tiene las piezas** (Sombra con vista, oído y persecución, ruido por suelo, escondites, Director, llaves guardadas, latido). Lo que falta es el **bucle que las une**: en el HTML la linterna es a la vez escudo y delator, y la batería se agota rápido. Eso obliga a decidir en cada pasillo.
- **Casi todo lo de E2–E3 dice "sin build":** nada del sigilo se ha jugado en el `.exe`. El Hospital todavía no tiene 2.º piso, negatoscopio, morgue ni páginas 1–15 (E4 parte el 21-11).
- **Nota del gameplay: 6/10.** Los sistemas están bien hechos y bien afinables, pero falta que el jugador entienda qué hacer (pistas y objetivo), que haya presión de recursos y que esté probado.

## 2. Hallazgos por agente
**game-designer**
- El HTML tiene un dilema central que Unreal no tiene: con la linterna encendida te ven desde 16 m (6 m apagada), pero apuntarla fija 2,5 s **ahuyenta** a la entidad. En UE la linterna del jugador no afecta a la Sombra.
- El HTML enseña jugando: el primer "te vieron" o "te oyeron" muestra un mensaje de qué hacer (agacharse, apagar, lanzar botella). En UE no hay ninguna pista contextual, y un tester público se pierde.

**systems-designer**
- Economía de batería: el HTML gasta 0,9 %/s (×1,3 en difícil). Las pilas dan +40 % y cada cinta +20 %. UE gasta 0,22 %/s y el valor está fijo en el BP, así que no hay presión. Hay que pasarlo a `DA_Tuning` (pendiente desde la GUIA §4).
- Dificultad en el HTML: un solo multiplicador (enemigo ×1,25 en vista y velocidad, ruido ×1,3, gasto ×1,3). La fila "Dificultad" del menú UE no está conectada a valores de `DA_Tuning` que se puedan verificar.
- Ruido del HTML: agachado 3,5 m, caminar 10 m, correr 24 m, con una **barra de ruido en el HUD**. En UE el ruido solo se ve en el panel `-dev`.

**gameplay-programmer**
- La botella del HTML hace que **todas** las entidades a menos de 40 m investiguen el punto de impacto (`alertAt`). En UE, `BP_BotellaLanzada` hace `MakeNoise`, pero la Sombra solo escucha `AC_Ruido`. Hay que verificar que la Sombra reaccione a la botella.
- El HTML autoguarda cada 8 s y al pausar (`saveGame`). UE guarda flags (v0.3.48), pero no tiene puntos de control automáticos (aprobados en S2 y sin hacer).
- Estamina del HTML: gasta 24/s al correr, recupera 12–22/s y bloquea la carrera hasta llegar a 35. Si UE no la tiene, el jugador corre sin parar y el ruido de carrera pierde peso.

**ai-programmer**
- Patrón "empuje" del HTML: cada 40–65 s, la entidad que patrulla más lejos va a investigar dónde estás. Es barato y evita que el jugador esquive la amenaza para siempre. En UE encaja como evento del `BP_DirectorMiedo`.
- El HTML tiene un estado `search` (gira 5 s mirando antes de volver) y un **anti-atasco** (si en 2,5 s no avanzó 0,5 m, cambia de destino). En UE la Sombra pasa de Investigar a Patrulla a los 6 s, pero no hay anti-atasco, y sin PIE no se puede ver si se queda trabada.
- Tras golpear, la entidad del HTML **retrocede 7 m y busca**. Así no hay golpes encadenados. La Sombra de UE golpea cada 1,5 s, lo que puede matar en cadena en un pasillo.

**ue-blueprint-specialist**
- Todo lo nuevo debe ir en **componentes o hijos**, no en `BP_ThirdPersonCharacter` (10 MB): `AC_Pistas`, `WBP_BarraRuido`. Los temporizadores con `SetTimerByEvent` de 0,1–0,25 s reemplazan al Tick.
- La muñeca o maniquí "luz roja, luz verde" del HTML se reutiliza como **hijo de `BP_PayasoAcecho`**, cambiando la malla y los valores. Los puntos de aparición son `TargetPoint` con tag, sin coordenadas.

**prototyper**
- **El HTML es nuestro banco de pruebas de 1 minuto.** Antes de gastar una build de 15 min, se ajustan ahí los números (cuánto tarda en ahuyentar, gasto de batería, radio de la botella). Después se pasan tal cual a `DA_Tuning` (m × 100 = cm).

## 3. Brechas HTML → Unreal (gameplay)
| HTML | Estado en UE | Cómo replicarlo en BP |
|---|---|---|
| Linterna que ahuyenta: apuntada 2,5 s, la entidad retrocede y se esconde en el punto más lejano | ❌ No existe | En `BP_AI_Sombra`, dentro del timer de percepción: si `Linterna.On`, la distancia es <22 m y el producto punto con la mirada del jugador es >0,9, sube `Iluminada`. Si pasa `Sombra_RepeleSeg`, va al estado Huir, hacia el `TargetPoint` tag `SombraRefugio` más lejano |
| La linterna encendida te delata (16 m contra 6 m) | ⚠️ El cono de la Sombra no mira la linterna del jugador | Multiplicar `VistaDist` por `Sombra_VistaLinternaOff` (0,4) cuando la linterna está apagada o el jugador agachado (×0,65) |
| Batería 0,9 %/s, pilas +40 %, cinta +20 % | ⚠️ 0,22 %/s fijo en el BP | Pasar a `DA_Tuning` (`Bateria_GastoSeg`, `Bateria_Pila`, `Bateria_Pagina`). Que cada página dé batería |
| Barra de ruido en el HUD (azul, amarillo, rojo) | ⚠️ Solo en el panel `-dev` | `WBP_BarraRuido` lee `AC_Ruido` con un timer de 0,15 s |
| Botella → todas investigan en 40 m | ⚠️ `MakeNoise`, la Sombra no la oye | Al romperse, `GetAllActorsOfClass(BP_AI_Sombra)` llama a `OirRuido(Pos)` si la distancia es menor que `Botella_Radio` |
| Pistas contextuales (primer "te vieron" u "oyeron") y objetivo con [H] | ❌ | `AC_Pistas`: mensajes de una sola vez, con flag en el GI. Texto de objetivo según los flags (`LlaveMorgue`...) desde una tabla de datos |
| Estamina con "Sin aliento..." | ❓ No figura en el CHANGELOG | Variables en el jugador, timer de 0,1 s y `MaxWalkSpeed`. Los valores en `DA_Tuning` |
| Golpe → retrocede y busca. 6 corazones, botiquín +1, vida baja = respiración y latido | ⚠️ Golpea cada 1,5 s. Hay respawn | Después del golpe, la Sombra pasa a Investigar y espera `Sombra_PausaGolpe` (3 s). Con vida baja, el latido sube con un flag |
| Empuje cada 40–65 s | ❌ | Evento en `BP_DirectorMiedo`: si la tensión está baja, la Sombra investiga la posición del jugador |
| Muñeca que avanza si no la miras | ✅ En el parque (payaso), ❌ en el Hospital/demo | Hijo `BP_ManiquiAcecho` del payaso, en la morgue |
| Anti-atasco del enemigo | ❌ | En el timer: si se movió <50 cm en 2,5 s, nuevo destino |
| Autoguardado cada 8 s y al pausar | ⚠️ Solo guardado manual con flags | Guardar al tomar llave o página y al cruzar `BP_VolumenZona`. Sin timer fijo |
| Difícil = ×1,25 y ×1,3 | ❓ Fila de menú sin efecto verificado | `Dificultad_Mult` en el GI, aplicado a la vista, velocidad, ruido y gasto |

## 4. Top 6 mejoras priorizadas
| # | Mejora | Impacto | Esfuerzo | Interruptor `DA_Tuning` | Responsable |
|:-:|---|:-:|:-:|---|---|
| 1 | **Pistas contextuales y objetivo [H]/(Triángulo)** (primera vista, oído, escondite, batería baja, objetivo según flags) | 5 | M | `Pistas_Activas` | game-designer + gameplay-programmer |
| 2 | **Dilema de la linterna contra la Sombra:** delata (vista ×) y ahuyenta si la apuntas `RepeleSeg`. Batería a `DA_Tuning` | 5 | M | `Sombra_RepeleLinterna`, `Sombra_RepeleSeg` (2,5), `Bateria_GastoSeg` (0,6) | ai-programmer + systems-designer |
| 3 | **La botella distrae a la Sombra** + `WBP_BarraRuido` en el HUD | 4 | S | `Botella_Distrae`, `Botella_Radio` (4000), `HUD_Ruido` | gameplay-programmer |
| 4 | **Sombra más justa:** pausa tras el golpe (retrocede e investiga) + anti-atasco | 4 | S | `Sombra_PausaGolpe` (3), `Sombra_AntiAtasco` | ai-programmer |
| 5 | **Empuje del Director** cada 40–65 s + autoguardado en llaves, páginas y zonas | 4 | S/M | `Director_EmpujeMin/Max` (0 = apagado), `Guardado_Auto` | ai-programmer + ue-blueprint-specialist |
| 6 | **Maniquí "luz roja, luz verde" en la morgue** (hijo de `BP_PayasoAcecho`, aparición por `TargetPoint`) | 3 | S | `Maniqui_Activo` | prototyper + ue-blueprint-specialist |

**Orden de builds (1–2 features por build):** B1 = #1 + #3 (antes de testers, ~20-11) · B2 = #2 + #4 (~22-11) · B3 = #5 + #6 (con E4, ~25-11). Los valores de #2 y #4 se ajustan primero en el HTML.
**Fuera del top (después del 26-11):** estamina, dificultad conectada a multiplicadores, Sombra anciana de la Casona (hijo de `BP_AI_Sombra`, solo datos: oído ×2, vista ×0,5).

## 5. Decisiones para el CTO
1. **¿La linterna ahuyenta a la Sombra, como en el HTML?** Recomiendo que sí, de forma híbrida: te delata y te salva, con un costo de batería. Si no, el Hospital queda en sigilo puro, más difícil de entender para el público de itch.io.
2. **Congelar el gameplay el 25-11:** solo las 3 builds de arriba antes de los testers del 26-11. Después, solo arreglos y E5. ¿Apruebas el orden B1 → B2 → B3?
3. **Teaser de la Casona:** ¿con la Sombra anciana (hija de datos, riesgo bajo) o solo atmósfera, apagón y crujido de madera, sin IA? Recomiendo **sin IA** para la demo: menos riesgo y deja la amenaza para Steam.
