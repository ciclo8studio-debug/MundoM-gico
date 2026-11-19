# Reingeniería 18-11 · @gameplay-engineer (Gameplay)
Base: evaluación `estudio_2026-11-18/gameplay-engineer.md`, `docs/historico/PLAN_ESTUDIO_2026-11-18.md` (recorte N1: sin 2.º piso, negatoscopio en sótano), CHANGELOG v0.3.37–0.3.61, `GUIA_TUNING.md`, HTML de referencia. Solo lectura. Lentes: game-designer, systems-designer, gameplay-programmer, ai-programmer, ue-blueprint-specialist, prototyper.

## 1. Visión
- **Un solo dilema por pasillo:** la luz te salva (ahuyenta, ves) y te delata (te ven a 16 m), y cuesta batería.
- **Una sola IA:** el componente `AC_Percepcion` da vista, oído, mirada y estados a todas las amenazas. El `BP_DirectorMiedo` decide quién actúa y cuándo.
- **Presión que respira:** ciclo calma → sospecha → persecución → refugio → calma. Nunca más de una persecución a la vez.
- **Fallar enseña, no castiga:** 4 golpes, el enemigo se retira tras golpear, guardado automático por eventos y pistas de una sola vez.
- **Todo es dato:** cada número va en `DA_Tuning` y cada feature tiene su interruptor. El HTML sirve de banco de pruebas de 1 minuto antes de cada build.

## 2. Bucle rediseñado: Nivel 1 `L_Hospital` (0–25 min)
| Min | Zona | Objetivo [H]/△ | Tensión (Director) | Recursos | Enseña |
|---|---|---|---|---|---|
| 0–3 | Sala del despertar | "Encuentra la linterna" → "Sal de la sala" | 0–10, sin amenaza | Batería 100 %. Pág. 1 | Movimiento, linterna, [H] |
| 3–6 | Pasillo de Urgencias | "Sigue las balizas" | Susto actual + alarma (pico de 40), luego decae | 1.ª botella. Pág. 2–3 | Barra de ruido y agacharse (pista al primer "te oyeron") |
| 6–10 | Recepción y pasillos en L | "Busca el acceso al sótano" | La Sombra se ve **de lejos**, de espaldas (25–45) | Pila +35 %. Pág. 4–6 | Linterna apagada = invisible a más de 6 m. Escondite 1 |
| 10–15 | Sótano (patrulla) | "Lee las radiografías" (negatoscopio 0333) | Patrulla con 3 escondites. **Empuje** cada 40–65 s (30–65) | 2.ª botella, pila. Pág. 7–10. La batería ronda 30–50 % | Botella distrae. Apuntar 2,5 s ahuyenta (pista al primer intento) |
| 15–19 | Morgue | "Encuentra la llave de la morgue" | Maniquí luz roja/verde. La Sombra queda en Inactivo (respiro) | Botiquín. Pág. 11–13 | Mirar = congelar. La luz te protege y a la vez te gasta |
| 19–23 | Clímax: retorno | "Escapa: puerta exterior" | Tomar `LlaveMorgue` → alarma → la Sombra en Persecución directa (80–100). Isidora en la puerta (aparición) | Pila garantizada junto a la llave. Pág. 14 | Todo junto: correr, botella, repeler y esconderse |
| 23–25 | Salida | Puerta exterior → glitch + TV | Corte a 0 | Pág. 15. Guardado | Cierre del Nivel 1 → Casona |
Reglas del bucle: hay un descanso garantizado de al menos 60 s tras cada persecución (Director `Respiro_Seg`). Los recursos se colocan con `TargetPoint` y tag `Recurso_<Zona>`, nunca con coordenadas. Si el jugador lleva 180 s sin avanzar el objetivo, sale la pista automática (A4).

## 3. Especificación de IA y sistemas
### 3.1 `AC_Percepcion` (ActorComponent, `/Game/MundoMagico/Componentes`)
**Por qué componente y no `BP_IAbase`:** la Sombra es un Character y el payaso y el maniquí son Actor. Con un componente no hay que cambiarles el padre (cambiarlo es arriesgado vía MCP).
- **Módulos (bool):** `UsaVista`, `UsaOido`, `UsaMirada` (se congela o se esfuma si lo miras: maniquí, payaso, Isidora), `UsaRepeler`. El dueño los llena en su BeginPlay desde `DA_Tuning`.
- **Timer** `Percibir`, cada `IA_TickSeg` (0,2 s), solo si `Estado ≠ Inactivo`. Sin Event Tick.
- **Vista:** `DistEf = VistaDist × (LinternaOn ? 1 : VistaLinternaOff) × (Agachado ? VistaAgachado : 1) × Dificultad_Mult`. Si el jugador está dentro del cono, a menos de `DistEf`, con `LineTrace` libre y sin tag `Escondido`, entonces `Sospecha += IA_TickSeg × VelSospecha × (1 − d/DistEf)`. Si no lo ve, `Sospecha −= IA_TickSeg × CaeSospecha`. Con `Sospecha` ≥ 0,5 pasa a Investigar (va a la última posición vista) y con 1 pasa a Persecución. A menos de `VistaCerca`, la sospecha sube a 1 de inmediato.
- **Oído:** `OirRuido(Pos, Radio, Prioridad)`. Lo llama el **Director**, no cada IA. Si `d < Radio × OidoMult` y `Prioridad ≥ PrioridadActual`, pasa a Investigar en `Pos`. La botella tiene prioridad 2 y los pasos 1.
- **Mirada:** producto punto entre la vista del jugador y la dirección al dueño mayor que `MiradaDot`, con trace libre. Maniquí: Congelado. Payaso e Isidora: `MiradaSeg` acumulados → se esfuma (lógica actual migrada).
- **Repeler:** si `UsaRepeler`, la linterna está encendida, `d < RepeleDist`, el producto punto es mayor que 0,9 y el trace está libre, entonces `Iluminado += IA_TickSeg`. Al llegar a `RepeleSeg` pasa a Huir, el jugador paga `Bateria_CostoRepeler` y el enemigo va al `TargetPoint` con tag `Refugio` más lejano del jugador. Luego queda Inactivo `RefugioSeg` y vuelve a Patrulla.
- **Anti-atasco:** cada `AntiAtascoSeg`, si se movió menos de `AntiAtascoCm` estando en Patrulla, Investigar o Persecución, elige un destino nuevo (`GetRandomReachablePointInRadius`).
- **Interfaz `BPI_IA`:** `OirRuido`, `Empujar(Pos)`, `Dormir(Si)`, `ObtenerEstado`. **Dispatcher** `OnEstadoCambia(Nuevo)`, que escuchan el Director, el latido y la música.

### 3.2 Estados (`E_EstadoIA`) y transiciones
| Estado | Velocidad | Entra cuando | Sale a |
|---|---|---|---|
| Patrulla | `VelPatrulla` | Inicio, fin de Buscar o de Refugio | Investigar (sospecha 0,5, ruido o empuje) · Persecución (sospecha 1) |
| Investigar | `VelAlerta` | Ruido, sospecha 0,5, botella o empuje | Buscar (llega al punto) · Persecución |
| Buscar | 0 (gira el haz) | Llegó al punto. Dura `BuscarSeg` | Patrulla · Persecución |
| Persecución | `VelPersecucion` | Sospecha 1. Máx. 1 enemigo a la vez (Director) | Golpe (`d < DistAtaque`) · Buscar (sin verte `PierdeSeg`) · Huir (repelido) |
| Golpe | 0 | Ataque: `Vida −= Dano` | Retirada |
| Retirada | `VelAlerta` | Retrocede `RetiradaCm` y espera `PausaGolpe` | Buscar |
| Huir | `VelPersecucion` | Repelido por la linterna | Inactivo (`RefugioSeg`) → Patrulla |
| Congelado | 0 | Maniquí mirado | Patrulla o acecho (`PasoCm` cada 0,25 s) |
| Inactivo | 0, haz apagado | `Dormir(true)` del Director, fuera de su zona o Huir | El estado previo |

### 3.3 `BP_DirectorMiedo` como planificador (amplía el actual)
- **Registro:** cada IA se registra en BeginPlay (`Registrar(Self)`), lo que evita `GetAllActorsOfClass` en cada paso. Es el **hub de ruido**: `AC_Ruido` y la botella llaman a `Director.Ruido(Pos, Radio, Prioridad)` y él reparte a las IA registradas.
- **Tensión** (0–100): visto +15, persecución +30, golpe +20, susto según su intensidad. Decae a `Director_Decae`/s.
- **Planificación** (timer de 1 s): a) **Empuje:** si la tensión es menor que `Empuje_TensionMax` durante un intervalo al azar entre `EmpujeMin` y `EmpujeMax`, el enemigo activo más cercano recibe `Empujar(PosJugador ± Empuje_ErrorCm)`. b) **Respiro:** tras una persecución, `Dormir` del resto durante `Respiro_Seg` y bloqueo de sustos. c) **Turnos:** `PedirTurno` (actual) también para las apariciones de Isidora y el maniquí. d) Si el Director está apagado, cada IA actúa sola y el empuje no ocurre.

### 3.4 Sistemas del jugador (en componentes, sin tocar más de lo necesario el BP del jugador, de 10 MB)
- **`AC_Linterna`** (migrar desde el jugador): `Estado` Off/On/Enfocada. Timer de 0,25 s: `Bateria −= GastoSeg × 0,25 × (Enfocada ? GastoEnfocada : 1) × Dificultad`. Por debajo de `Bateria_Parpadeo` parpadea (timer al azar) y en 0 se apaga. Hacen falta las funciones `RecargarBateria(Cant)` (pila o página) y `Gastar(Cant)` (repeler). La linterna solo **delata** a través de `AC_Percepcion`, que lee `EstaEncendida`.
- **Sigilo:** `AC_Ruido` (existe) ahora también llama a `Director.Ruido`. Se agrega `WBP_BarraRuido`, que lee el último ruido con un timer de 0,15 s (azul por debajo de 0,5, amarillo por debajo de 1,5, rojo por encima).
- **Botella:** `Botellas` (Int, máximo `Botella_Max`). Se lanza con T/R1 (`BP_BotellaLanzada`). `OnHit` → sonido + `Director.Ruido(Impacto, Botella_Radio, 2)`. Los enemigos le dan prioridad durante `Botella_DistraeSeg`.
- **Escondites (`BP_HideSpot`):** sin cambios de base. Contener la respiración (Shift) dura como máximo `Respira_MaxSeg`; al pasarse, jadeo = `Director.Ruido(Pos, 800, 1)`. **Revisión:** si un enemigo en Persecución te vio entrar a menos de `Escondite_RevisaCm`, espera `Escondite_RevisaSeg` frente al escondite. Si no contienes la respiración en ese lapso, te saca y te golpea.
- **Daño y vida (`AC_Vida`):** `Vida` hasta `Vida_Max`. Al recibir daño: sacudida, viñeta roja y, si `Vida ≤ Vida_Critica`, el flag de latido y respiración. Botiquín con 4/↑: `+Botiquin_Cura`. En 0: fundido, `Revivir` en el último checkpoint guardado, con `Vida = Vida_Revivir`, `Bateria = max(actual, Bateria_Revivir)`, las IA en Patrulla y tensión 20. Las llaves y páginas se conservan.
- **Guardado automático (en el GI):** `Guardar()` al tomar una llave, una página o un flag, al cruzar un `BP_VolumenZona` con `Checkpoint = true` (su `TargetPoint` de tag `Respawn` es el punto de regreso) y al pausar. Respeta un mínimo de `Guardado_MinSeg` entre guardados y **se pospone si hay una IA en Persecución** (se reintenta cuando sale de ese estado). Guarda flags, páginas, vida, batería, botellas, botiquines y checkpoint.
- **`AC_Pistas`:** mensajes de una sola vez con flag en el GI (primer "te vieron" o "te oyeron", batería baja, primer escondite, primera botella). Objetivo [H]/△ según flags desde `DT_Objetivos`. Pista automática a los `Pista_AutoSeg`.
- **Estamina** (después del 26-11): timer de 0,1 s, `MaxWalkSpeed`. Por debajo de `Estamina_Bloqueo` no se puede correr ("Sin aliento...").

## 4. Tabla de balance (`DA_Tuning`; cm, s, %). Los valores nuevos van en **negrita**
| Clave | Propuesto | Rango | Nota |
|---|---|---|---|
| **Bateria_GastoSeg** | 0,35 %/s | 0,2–0,9 | Unos 4,8 min de uso continuo. HTML 0,9; UE hoy 0,22 |
| **Bateria_GastoEnfocada** | ×1,8 | 1–3 | L2 |
| **Bateria_Pila / Bateria_Pagina** | 35 / 5 | 20–50 / 0–10 | 5 pilas + 15 págs. ≈ 350 % total ≈ 66 % del nivel con luz |
| **Bateria_Parpadeo / Bateria_CostoRepeler** | 15 / 8 | 5–25 / 0–20 | |
| Sombra_VistaDist / VistaAngulo | 1600 / 22 | 1000–4000 / 10–45 | Igual al HTML (16 m) con la linterna encendida |
| **Sombra_VistaLinternaOff / VistaAgachado / VistaCerca** | 0,4 / 0,65 / 250 | 0,2–1 / 0,4–1 / 100–400 | Apagada ≈ 6,4 m |
| **IA_TickSeg / VelSospecha / CaeSospecha** | 0,2 / 2,0 / 0,5 | 0,1–0,3 / 0,5–5 / 0,2–2 | Unos 0,7 s de vista directa a 5 m hasta la persecución |
| Sombra_OidoDist / Ruido_RadioBase | 1800 / 1500 | 600–3000 / 500–3000 | Sin cambios |
| Sombra_PierdeSeg / **BuscarSeg** | 4 / 5 | 2–10 / 2–10 | |
| Sombra_VelPatrulla / Alerta / Persecucion | 140 / 240 / 420 | 80–600 | El jugador corre a unos 500 |
| Sombra_DistAtaque / **Sombra_Dano** | 150 / 1 | 100–250 / 1–2 | |
| **Sombra_PausaGolpe / RetiradaCm** | 3 / 700 | 1–6 / 300–1200 | Evita golpes encadenados |
| **Sombra_RepeleSeg / RepeleDist / RefugioSeg** | 2,5 / 2200 / 20 | 1–5 / 1000–3000 / 10–60 | |
| **AntiAtascoSeg / AntiAtascoCm** | 2,5 / 50 | 1–5 / 20–150 | |
| **Botella_Max / Botella_Radio / Botella_DistraeSeg** | 3 / 4000 / 8 | 1–5 / 1500–6000 / 4–15 | 4 botellas en el nivel |
| **Vida_Max / Vida_Critica / Vida_Revivir** | 4 / 1 / 3 | 2–6 / 1–2 / 1–6 | |
| **Botiquin_Cura** | 2 | 1–4 | 2 botiquines en el nivel |
| **Maniqui_Dano / Maniqui_MiradaDot** | 2 / 0,85 | 1–3 / 0,7–0,95 | Hereda Payaso_PasoCm (110) y DistGolpe (230) |
| **Respira_MaxSeg / Escondite_RevisaCm / RevisaSeg** | 6 / 600 / 4 | 3–10 / 300–1200 / 2–8 | |
| Director_Enfriamiento / Decae | 40 / 1,5 | 20–120 / 0,5–5 | Sin cambios |
| **Director_EmpujeMin / EmpujeMax** | 40 / 65 | 0 = apagado; 20–120 | |
| **Empuje_TensionMax / Empuje_ErrorCm / Respiro_Seg** | 30 / 600 / 60 | 10–60 / 0–1500 / 20–120 | |
| **Guardado_MinSeg / Pista_AutoSeg** | 20 / 180 | 5–60 / 60–300 | |
| **Dificultad_Mult** (Fácil / Normal / Difícil) | 0,8 / 1 / 1,25 | 0,5–1,5 | Multiplica la vista, la velocidad, el ruido y el gasto |
| **Estamina_Gasto / Recupera / Bloqueo** | 20 / 15 / 30 | 10–40 / 5–30 / 10–50 | Después del 26-11 |

## 5. Plan por builds (1–2 cambios cada una; prueba con `Probar.bat` y panel `-dev`)
| Build | Cambio | Verificación | Interruptor |
|---|---|---|---|
| v0.3.62 (19-11) | Batería en `DA_Tuning`, más pilas y páginas que recargan (A3a) + `AC_Pistas` con objetivo [H] (A4) | Opción 1: batería ≈ 79 % a los 60 s con la luz encendida; las pistas salen una vez | `Bateria_GastoSeg`, `Pistas_Activas` |
| v0.3.63 (20-11) | Director como hub de ruido + la botella distrae + `WBP_BarraRuido` (A5) | Opción 2: botella a unos 10 m → la Sombra va al impacto. El panel muestra el estado de la IA | `Botella_Distrae`, `HUD_Ruido` |
| v0.3.64 (21-11) | `AC_Percepcion` v1 en la Sombra: sospecha gradual y la linterna delata. La lógica vieja se conserva detrás del interruptor | Opción 2: apagada + agachado a 8 m no te ve; encendida a 15 m sí | `Sombra_UsaPercepcion` (si se apaga, vuelve la lógica v0.3.47) |
| v0.3.65 (22-11) | Repeler con la linterna + retirada tras golpe + anti-atasco (A3b, A7) | Apuntar 2,5 s → huye al `Refugio`. 1 golpe → retrocede. Sin trabarse en 3 min | `Sombra_RepeleLinterna`, `Sombra_PausaGolpe` |
| v0.3.66 (24-11) | `AC_Vida` + botiquín + revivir en checkpoint | 4 golpes → revive con vida 3 y las llaves conservadas | `Vida_Activa` |
| — 25-11 | **Freeze del gameplay y RC1 para testers** | Prueba de humo de @qa-publisher | — |
| v0.4.x (27-11) | Guardado automático por eventos + empuje del Director (B6) | Tomar una página → aparece el ícono de guardado. 60 s quieto → la Sombra investiga cerca | `Guardado_Auto`, `Director_EmpujeMin`=0 |
| v0.4.x (29-11) | Maniquí (hijo del payaso, con `AC_Percepcion` en modo Mirada) en la morgue (B8) | Mirarlo = congelado. Darle la espalda = avanza | `Maniqui_Activo` |
| v0.4.x (30-11) | Revisión de escondites + clímax de la llave (alarma → persecución) | Te vio entrar → espera 4 s frente al escondite | `Escondite_Revisa`, `Climax_Activo` |
| Después del 10-12 | Estamina + `Dificultad_Mult` conectada | — | `Estamina_Activa` |
Regla: antes de cada build se prueba el valor en el HTML (m × 100 = cm). Si algo falla en un playtest, se apaga el interruptor y se rehace la build sin tocar Blueprints.

## 6. Decisiones CTO
1. **IA unificada como componente `AC_Percepcion`** (recomendado), en vez de una clase base `BP_IAbase`. Se migra la Sombra primero, con interruptor para volver a la v0.3.47.
2. **Vida de 4 golpes con revivir en checkpoint**, conservando llaves y páginas (recomendado para el público de itch.io), en vez de morir de 1 golpe.
3. **Clímax de la llave de la morgue** (alarma → persecución → salida) y **revisión de escondites:** ¿entran en la demo del 10-12 (recomendado, ambos con interruptor) o quedan para Steam?
