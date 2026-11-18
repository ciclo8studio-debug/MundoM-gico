# Reunión "Replicar el HTML completo": @gameplay-engineer (18-11-2026)
Equipo: game-designer, systems-designer, gameplay-programmer, ai-programmer, ue-blueprint-specialist y prototyper.
Fuentes: `RECORRIDO_HTML.md`, CHANGELOG 0.1.0–0.3.67, `GUIA_TUNING.md`, `REINGENIERIA_2026-11-18.md`, `auditoria/world-builder.md`, `tech-lead.md`, `diseno_sprint3_demo.md` y `diseno_bt_enemigo.md`. Solo lectura, sin abrir Unreal. **"Verificar"** = confirmarlo con una lectura MCP del BP antes de tocar nada.

## 1. Diagnóstico
- **UE ya tiene casi todo el esqueleto del parque.** `BP_ThirdPersonCharacter` conserva interruptores, generador, bodega con llave, casa (fonógrafo, reloj 8-4-2, caja fuerte con fusible), cintas (`Cintas`, `CintasVistas`, `ReproducirCintas`), cabina VIDEO, `EleccionSala`, finales por páginas y logros. El SaveGame `BP_Partida` guarda `Vida`, `Bateria`, `Pilas`, `Botiquines`, `Botellas`, `Energia`, `TieneLlave/Fusible`, `IntentosEnergia`, `PedestalPaso` y `Cintas`. Todo eso está **escrito con coordenadas fijas y sin probar en el `.exe` desde la v0.4.0**.
- **Lo que no coincide es la regla central.** En el HTML los enemigos **solo dañan con la linterna apagada**, la luz los ahuyenta y la batería se gasta rápido. Además, la amenaza escala con las cintas: muñeca (1) → payaso ángel (3) → Willy y León (4) → persecución del payaso. En UE, Willy, León y la muñeca usan la FSM vieja sobre Tick de `BP_Enemigo` y `BP_Muneca`, están activos desde el inicio y la linterna no los afecta. La IA buena (`BP_AI_Sombra`: delata, ahuyenta, pausa tras el golpe y anti-atasco) **solo está en el hospital**.
- **Bloqueante previo (fuera de gameplay):** `L_Parque` cierra el juego en la iGPU (GPUCrash, v0.3.67). Ningún build de esta lista se puede validar hasta que el parque cargue estable.

## 2. Matriz de paridad
Estados: ✅ hecho · 🟡 parcial · ❌ falta · 🔀 distinto (UE hace otra cosa).

| # | HTML | UE hoy | Estado | Cómo cerrarlo en BP (interruptor `DA_Tuning`) |
|---|---|---|---|---|
| 1 | Despertar en la cama, incorporarse, linterna con 80 % y salir por la puerta | `BP_Despertar`, `CAM_CineDespertar` y linterna en el mostrador | ✅ | Verificar que la batería inicial sea 80 (`Bateria_Inicial`) |
| 2 | Pasillo: registro de Urgencias y botiquín/pilas | 15 páginas de `DA_NovelaData` (incluye el registro) | 🟡 | Botiquín y pilas como `BP_Recogible` en el pasillo; la nota ya existe como página 4 |
| 3 | Susto: silueta tras el vidrio (iluminarla y mirarla → desaparece) | No existe | ❌ | Hijo de `BP_IsidoraAparicion` con regla "producto punto > 0,93 + linterna a < 14 m" (`Susto_SiluetaVidrio`) |
| 4 | Apagón en cadena + silla de ruedas con paciente | `BP_SustoPasillo` (criatura SK_Whisper, balizas, alarma) | 🔀 | Mantener el de UE, que es mejor y ya está probado. Agregar la nota "LA PACIENTE DEL PASILLO" al terminar |
| 5 | Salir por Urgencias → entrada del parque | Glitch → TV → `L_Parque` (v0.3.67) | ✅ | Depende del arreglo del GPUCrash |
| 6 | 4 interruptores; el 4.º falla la primera vez (cortocircuito) | 4 en cualquier orden, cortocircuito quitado en v0.3.1 (quedan `IntentosEnergia` y `FalloEnergia`) | 🔀 | Volver a conectar la rama del cortocircuito con `Energia_Cortocircuito` |
| 7 | Silueta gigante bajo el letrero **al volver la energía** | `BP_SustoEnergia` **al llegar** a la entrada | 🔀 | Disparar desde `Energia` = 4/4, a 1,2 s (`SustoEnergia_AlEncender`) |
| 8 | Cinemática y "se restablece la energía" | Órbita de 12 s (`CAM_CineParque`) | ✅ | — |
| 9 | 4 cintas en orden libre, cada una +20 % de batería, recogida a 2,4 m o 4,5 m mirándola | `REC_Cinta01/02` (tren y noria) + cinta de la casa; el dump cuenta **3** | 🟡 | Verificar cuántas `REC_Cinta` hay y poner la 4.ª. Sumar `Bateria_Cinta`=20 al recoger |
| 10 | Casa con llave → bodega (disfraces → llave) → casa | Igual, con el diálogo de Alfredo | ✅ | Verificar en `.exe` |
| 11 | Fusible en el cajón de la cómoda | Fonógrafo "LAS OCHO" → reloj 8-4-2 → caja fuerte | 🔀 | `Casa_FusibleCajon` (activo = como el HTML; apagado = puzle de UE). Acorta el tiempo muerto de la casa |
| 12 | Cabina: exige 4 cintas **y** fusible, con 4 avisos según el estado | Cabina, `CintasVistas` | 🟡 | Revisar la condición en `InteractuarZonas` y los 4 textos del prompt |
| 13 | Video de 12 segmentos (3 páginas por cinta; borroso si falta la página) + subtítulos | `ReproducirCintas` y `CuadroCinta` | 🟡 | Lista `CintaPaginas[12]` en un DA. Si la página no está recogida → material con ruido y el texto "Recoge la página…" (`Cabina_PorPaginas`) |
| 14 | Elección E (sala) / Q (quedarse; volver a la cabina la reabre) | `EleccionSala` | 🟡 | Verificar la rama Q y el objetivo "Vuelve a la cabina VIDEO" |
| 15 | Persecución final en el hospital: payaso a 5,2 m/s, golpe a 1,9 m que te empuja 8 m, "¡BANG!" | Huida casa → entrada en el parque (v0.4.0), pensada para el mapa único | 🔀 | Con 2 mapas: `BP_GestorNivel` abre `L_Hospital` con el flag `FinalCorre`, aparición en el extremo oeste y `ENE_PayasoChase` activado en x > −90 m (`Final_ComoHTML`) |
| 16 | Puerta SALA 4 → expediente → final según páginas (62 / ≥31 / <31) | `MostrarFinal` con finales y logros | 🟡 | Umbrales en `DA_Tuning` (`Final_Total`, `Final_Medio`). Ver la decisión 3 |
| 17 | Tras el final: Reiniciar o volver al parque | ¿? | ❓ | Verificar. Si falta: Q → `L_Parque` en la entrada |
| 18 | Muerte = "TE ENCONTRÓ", recargar | Respawn; aprobado: 4 golpes y checkpoint | 🔀 | Usar `AC_Vida` (aprobado). Pantalla "TE ENCONTRÓ" de 2 s antes de revivir |
| 19 | **Muñeca** desde la cinta 1: avanza 2,4 m si no la miras 2,6 s, a < 3,2 m se teletransporta, sin daño | `BP_Muneca` (no mirar) + evento del Carrusel | 🟡 | Activarla solo con `Cintas ≥ 1` (`Amenazas_PorCintas`). Mantener el evento del Carrusel |
| 20 | **Estatua del payaso** en la entrada (sólida) | `ENE_Payaso` frente al castillo; acosador desde los 60 s | 🔀 | Colocar la estatua en la entrada (malla sin IA) |
| 21 | **Payaso ángel** desde la cinta 3: cambia de pose cada 3,5–6 s fuera de vista, avanza 3,2 m, golpea a 2,8 m **solo a oscuras**, luego salta a 14 m | `BP_PayasoAcecho`: aparece por timer (máx. 6 veces), avanza 1,1 m cada 0,25 s, golpea a 2,3 m siempre | 🔀 | `Payaso_DesdeCinta`=3, `Payaso_SoloOscuras`, `Payaso_PasoCm`=320 con `Payaso_PasoCadaSeg` 3,5–6. Al golpear, teletransporte a 1400 en vez de desaparecer |
| 22 | **Willy y León** se despiertan con la cinta 4, a 34 y 46 m, con mensaje | Activos desde el inicio, FSM sobre Tick | 🔀 | `Amenazas_PorCintas`: `Dormir(true)` hasta la cinta 4, luego reubicar y mostrar el mensaje |
| 23 | Su IA: patrulla, investigar, búsqueda de 5 s, persecución, esconderse; visión 16/6 m; oído 24/10/3,5 m | `BP_Enemigo` (Tick, LineTrace, `RuidoPos`) | 🟡 | **Hijos de `BP_AI_Sombra`** (`BP_AI_Willy` y `BP_AI_Leon`) sin el haz de luz, con sus mallas. Con `Parque_IANueva` apagado vuelve `BP_Enemigo` |
| 24 | Linterna que ahuyenta (2,5 s → se esconden) | Solo en la Sombra (v0.3.64) | 🟡 | Lo heredan del punto 23 |
| 25 | **Solo dañan con la luz apagada**; con luz se detienen a 3,5 m | La Sombra daña siempre | ❌ | `IA_SoloDanaOscuras` en `RevisarLinterna`: si la linterna está encendida, frenar a 350 y no atacar. Por enemigo; apagado en la Sombra del hospital |
| 26 | Golpe: −1 corazón, pausa de 2,5 s y retroceso de 7 m | Sombra: pausa de 3 s y vuelta a su zona (v0.3.65) | ✅ | Heredado; `RetiradaCm`=700 |
| 27 | Empuje: cada 40–65 s el patrullero más lejano investiga tu posición | No existe | ❌ | Evento en `BP_DirectorMiedo` (`Director_EmpujeMin/Max`=40/65) |
| 28 | Botella: alerta a todos en 40 m. 10 en el mapa; se empieza con 2 | Botella arreglada (v0.3.65); 10 botellas | 🟡 | Que alerte a todos los `BP_AI_*`, no solo a la Sombra (`Botella_Radio`=4000) |
| 29 | Sustos: bodega (naftalina, goteo), disfraces, tele de la casa | Diálogo de Alfredo, disfraces y textos | 🟡 | Verificar la tele con voces cada 5–10 s |
| 30 | Teatro: puzle de 5 pedestales + figura en el escenario + público que gira | Pedestales (`PedestalPaso`) + `BP_SustoTeatro` (ataque frente a ti) | 🟡 | Premio de pilas + botiquín al resolverlo. El susto de UE se mantiene |
| 31 | Tren fantasma: fantasma que salta (repetible) + nota VAGÓN 4 | Solo texto | ❌ | `BP_SustoFantasma` con trigger y enfriamiento de 12 s, sin pasar por el Director (`Susto_TrenFantasma`) |
| 32 | Autos chocadores que te empujan | Zona con respawn | 🟡 | Baja prioridad. Empuje con `LaunchCharacter` al chocar |
| 33 | Aliento: 100, gasta 24/s, recupera 22/12, bloqueado hasta 35; FOV 70→80 | Solo cuenta al esconderse | 🟡 | `Aliento_Activo` con timer de 0,1 s. **Es requisito del punto 15**: sin aliento, la persecución no tiene tensión |
| 34 | Vida 1 corazón → ×0,85 y cojera | No existe | ❌ | Dentro de `AC_Vida`, `Vida_CojeraMult` |
| 35 | Mapa recogible que amplía el minimapa ×2,2 | Minimapa con `T_Mapa3` | 🟡 | Flag `TieneMapa` + zoom (después del 10-12) |
| 36 | Pista H y pista automática a los 180 s; prioridad de `objective()` | `MostrarObjetivo` con coordenadas fijas | 🟡 | A4 (`AC_Pistas`); el orden de prioridad del HTML va a `DT_Objetivos` |
| 37 | Autoguardado cada 8 s | Al entrar a cada zona + flags | 🟡 | Autoguardado por eventos (aprobado): cinta, llave, fusible, interruptores |
| 38 | Modo difícil (×1,25 vista y velocidad, ×1,3 ruido y gasto) | La fila del menú no está conectada | ❌ | `Dificultad_Mult` (después del 10-12) |
| 39 | Barra de ruido en el HUD | Solo en el panel `-dev` | ❌ | `WBP_BarraRuido` (`HUD_Ruido`) |
| — | Solo en UE: generador, carrusel, noria, Isidora, payaso acosador, la Sombra y el Director | — | ➕ | Se quedan. El generador y el carrusel no bloquean la ruta del HTML |

## 3. Valores HTML frente a UE (recomendación)
Conversión: m × 100 = cm. El jugador de UE corre a unos 500 cm/s; el del HTML a 6,8 m/s, así que las velocidades de los enemigos se escalan con la misma proporción.

| Valor | HTML | UE hoy | Recomendado | Por qué |
|---|---|---|---|---|
| Gasto de batería | 0,9 %/s | 0,35 %/s | **0,35 en el hospital, ×1,7 en el parque (≈ 0,6)**, con `Bateria_MultParque` | En el HTML el presupuesto es 80 + 4×20 + 7×40 = 440 %: unos 8 min de luz en una ruta de 15–25 min. Con 0,6 serían unos 12 min (≈ 50 %), lo que obliga a apagar. 0,9 en 1ª persona y con la oscuridad real de UE es demasiado |
| Pila / cinta / batería inicial | +40 / +20 / 80 | Pila sin verificar | 40 / 20 / 80 | Paridad. No se puede usar una pila con ≥ 95 % |
| Visión de Willy y León | 16 m encendida / 6 m apagada, cono ±72°, 360° a < 4 m, ×0,65 agachada | — | **Como el HTML: 1600 / 600 / 72° / 400 / 0,65** | El cono es ancho, así que el alcance tiene que ser corto |
| Visión de la Sombra | — | 22 m / 7 m, cono 22° | **Mantener 2200 / 700** | Su cono es angosto (su propia linterna); en pasillos cubre una zona parecida |
| Corazones | 6, botiquín +1 | `Vida` (aprobado: 4 golpes + checkpoint) | **`Vida_Max`=4, `Botiquin_Cura`=1**; en el parque, 5 botiquines | Respeta la decisión 5 del CTO. Para paridad exacta, `Vida_Max`=6 y el HUD oculta los corazones que sobran |
| Daño | 1 corazón; pausa de 2,5 s (enemigo), 3 s (ángel), 2,2 s (persecución) | Sombra 1 golpe + `PausaGolpe` 3 s | 1 golpe; `PausaGolpe` 2,5 en el parque | Paridad |
| Jugador | Camina 4, corre 6,8, agachada ×0,5 | Corre ≈ 500 | **Caminar 300, correr 500, agachada 150** | Verificar `MaxWalkSpeed` en el BP |
| Willy (patrulla / investigar / persecución a oscuras) | 1,1 / 2,2 / 3,8 m/s | — | **80 / 160 / 280** | Proporción respecto a la carrera: 0,56 |
| León | 1,54 / 3,08 / 5,32 m/s | — | **115 / 225 / 390** | 0,78: lo dejas atrás solo corriendo |
| Persecución con luz encendida | ×1 y se detiene a 3,5 m | — | ×1 de su velocidad base, frena a 350 | — |
| Payaso en la persecución final | 5,2 m/s; golpe a 1,9 m, retrocede 8 m | — | **380 / 190 / 800** | 0,76 de la carrera. Con 4 s de aliento te alcanza si no lo administras |
| Payaso ángel | Avanza 3,2 m cada 3,5–6 s, golpe a 2,8 m | 1,1 m cada 0,25 s, golpe a 2,3 m | **Como el HTML** | El avance de UE es demasiado rápido para un acecho de todo el parque |
| Muñeca | Avanza 2,4 m tras 2,6 s sin mirarla; a < 3,2 m se teletransporta | ¿? | Como el HTML | Verificar `BP_Muneca` |
| Aliento | 100; −24/s; +22 quieta, +12 caminando; bloqueo hasta 35 | Solo escondida | Como el HTML | Necesario para la persecución |
| Oído (radio del ruido) | Correr 24, caminar 10, agachada 3,5 m | Base 1500 × (1 / 2,5 / 0,3) = 15 / 37,5 / 4,5 m | **`Ruido_RadioBase` 1000** → 10 / 25 / 3 m | Casi exacto al HTML; hoy correr se oye de demasiado lejos |

## 4. Orden de implementación (1–2 cambios por build, cada uno con su interruptor)
Requisito: **B0, `L_Parque` estable en la iGPU** (tech-lead y technical-artist). Cada build se prueba con `Probar.bat -ir=parque` y se repite **con el interruptor apagado**.

| Build | Cambios | Interruptores | Verificación |
|---|---|---|---|
| v0.3.68 | Escalada por cintas (muñeca 1, payaso 3, Willy y León 4, con mensaje) + `Bateria_Cinta` | `Amenazas_PorCintas`, `Bateria_Cinta` | Sin enemigos hasta la cinta 4; batería +20 al recoger |
| v0.3.69 | `BP_AI_Willy` y `BP_AI_Leon`, hijos de la Sombra con los valores de la sección 3 | `Parque_IANueva` | Persiguen, la luz los ahuyenta y no se traban en 3 min |
| v0.3.70 | Daño solo con la luz apagada + multiplicador de batería del parque | `IA_SoloDanaOscuras`, `Bateria_MultParque` | Con la luz encendida frenan a 3,5 m; 100 % dura unos 2,8 min |
| v0.3.71 | Cortocircuito del 4.º interruptor + silueta al volver la energía | `Energia_Cortocircuito`, `SustoEnergia_AlEncender` | El 4.º falla una vez; la silueta aparece a 1,2 s |
| v0.3.72 | Fusible en el cajón + condición y textos de la cabina | `Casa_FusibleCajon` | Ruta llave → cajón → cabina sin el puzle del reloj |
| v0.3.73 | Video de la cabina por páginas + elección E/Q | `Cabina_PorPaginas` | Página que falta = segmento borroso; Q y volver reabre la elección |
| v0.3.74 | Aliento con FOV + payaso ángel como el HTML | `Aliento_Activo`, `Payaso_SoloOscuras` | 4 s de carrera y "Sin aliento"; el payaso avanza a saltos |
| v0.3.75 | Persecución final en `L_Hospital` + expediente y finales | `Final_ComoHTML` | E en la cabina → pasillo → BANG → sala → final según páginas |
| v0.3.76 | Empuje del Director + la botella alerta a todos | `Director_EmpujeMin`, `Botella_Radio` | 60 s quieta → un enemigo investiga |
| v0.3.77 | Sustos que faltan: silueta tras el vidrio y fantasma del tren | `Susto_SiluetaVidrio`, `Susto_TrenFantasma` | Cada uno ocurre una vez (el fantasma se repite cada 12 s) |
| Después del 10-12 | Mapa con zoom, barra de ruido, dificultad, autos que empujan, cojera | Uno por función | — |

Reglas de BP: nada nuevo en `BP_ThirdPersonCharacter` salvo llamadas (componentes, hijos y timers de 0,2 s; nunca Tick). Las coordenadas fijas nuevas van a `TargetPoint` con tag. Antes de cada build se prueba el número en el HTML.

## 5. Decisiones para el CTO
1. **Calendario:** son 10 builds, una por día, y chocan con el freeze del 25-11 y con el Sprint B (sótano y morgue). ¿El parque reemplaza al Sprint B en la demo del 10-12 (recomendado) o se recorta a v0.3.68–v0.3.72?
2. **Casa:** ¿fusible en el cajón como el HTML (recomendado, ruta más corta) o se mantiene el puzle fonógrafo → reloj → caja fuerte?
3. **Páginas y finales:** el HTML usa 62 páginas (umbrales 62 / 31) y la demo de UE muestra 15 con texto. ¿Hospital 1–15 con texto y parque 16–62 con imagen, con los umbrales en `DA_Tuning`?
4. **Vida:** ¿4 golpes (aprobado) o 6 corazones como el HTML?
