# Reunión de AVANCE · 4 super-agentes · 18-11-2026 (tarde)

**Base:** acta `docs/historico/REUNION_2026-11-18_ReplicaHTML.md`, informes `reunion_*.md`, `estado_parque_2026-11-18.md`, CHANGELOG 0.3.61–0.3.68 y BOARD.
**Hecho clave:** la v0.3.68 (sin `CastShadows` en 14 luces, sol y SkyLight) **no arregló el parque**. `-ir=parque` y `-ir=teatro` se cayeron a las 11:54 y 11:57, y D9/D10 también se cayeron según los reportes (el CHANGELOG y el BOARD dicen lo contrario y hay que corregirlos). D2–D10 cayeron todas con unos 800 MB libres, y el CTO "parte en la sala del hospital" antes del cierre. Falta la build Development `v0.3.68_dev` con logs. **El bloqueo n.º 1 sigue abierto.** Además, el número v0.3.68 ya se usó, así que la hoja de ruta corre un número.

## 🕹️ @gameplay-engineer
- **Hecho (Sprint A):** A3 (la linterna delata y ahuyenta), A5 (la botella distrae) y A7 (pausa tras el golpe y anti-atasco) en `BP_AI_Sombra` (0.3.64–0.3.65).
- **Del plan HTML:** 0 de 10 builds. Su orden empieza por la escalada por cintas y Willy/León como hijos de la Sombra, **todo en `L_Parque` → bloqueado por el parque**.
- **Puede avanzar ya en `L_Hospital`:**
  - **Progresión:** flags con nombres del HTML, `FlagCambiado` y guardado v2 (autoguardado por evento).
  - **Acto 3 completo:** flag `PersecucionFinal`, payaso de la persecución (380/190/800), puerta SALA 4 y `MostrarFinal` con umbrales en `DA_Tuning`.
  - **Aliento con FOV** (requisito de la persecución) y `IA_SoloDanaOscuras` en `RevisarLinterna`, que se prueba con la Sombra del hospital y queda apagado ahí.
- **Riesgo:** el BP del jugador tiene la lógica del parque con coordenadas fijas y sin probar desde la v0.4.0. Cuando el parque cargue, aparecerán bugs viejos todos juntos.
- **Próximo paso:** v0.3.70 = flags + guardado v2, y v0.3.71 = Acto 3 en el hospital, probado con `-ir=persecucion`.

## 🏚️ @world-builder
- **Hecho:** A1 (look del HTML en el hospital), las 15 páginas de la Novela (`DA_NovelaData`, `WBP_Pagina`) y los 7 `BP_TuboFalla`. La propuesta de canon (RECUERDOS 1–62 + EXPEDIENTE E1–E15) quedó aprobada (decisión 4).
- **En curso o pendiente:** D0 · datos (19–21-11). Está **en plazo y no depende del parque**.
- **Puede avanzar ya:**
  - `DA_NovelaData` a 62 filas (ID, zona, imagen, pie, flashback, cinta, segmento) + `DA_Expediente` (15).
  - `REC_Pagina` con **ID fijo**, el HUD a "/62" y los 16 Recuerdos y papeles del hospital colocados.
  - Textos de finales, cabina, TV y las 15 notas en `ST_Mensajes`, más el rótulo "CINTA 1 · 23:41".
  - **Escenografía del Acto 3 en `L_Hospital`:** puerta SALA 4, expediente brillante y puertas rotuladas; también la silueta tras el vidrio.
- **Bloqueado por el parque:** cintas en posiciones HTML, los 46 Recuerdos del parque, la tele de la casa, el tren fantasma y el público del teatro (D2/D3).
- **Riesgo:** colocar 46 páginas exige trazas en PIE, y este PC no aguanta PIE del parque. Hacen falta coordenadas validadas en el editor liviano.
- **Próximo paso:** D0 completo el 20-11 y escenografía del Acto 3 el 21-11.

## 🎨 @ux-toolsmith
- **Hecho:** A2 (avisos [E]/[□] según el dispositivo) y el `Probar.bat` nuevo (sin sonido, borrar partida, ZIP de logs, submenú P con D1–D10).
- **Atrasado:** la **interacción por mirada** estaba planeada para la v0.3.68 y no entró, porque la build se usó para las sombras. No depende del parque.
- **Puede avanzar ya** (casi todo su plan es UI o datos):
  - `ElegirInteractuable()` (timer de 0,1 s, `UI_InteraccionMirada`).
  - `DT_Objetivos` + pista H/LT + "¿PERDIDA?".
  - `DA_Notas`/`WBP_Notas`.
  - **`WBP_VideoCabina` + `WBP_Eleccion` como widgets** que se disparan con el atajo `-dev` "4 cintas + fusible" desde el hospital.
  - `WBP_FinalDemo` (3 finales + "TE ENCONTRÓ").
  - `WBP_Subtitulos` + tamaño de texto.
- **Herramientas:** agregar `-ir=persecucion`, `expediente` y `cabina` a `Probar.bat`; sin ellos, el Acto 3 cuesta unos 20 min por intento.
- **Riesgo:** la mochila con minimapa y la mezcla por zonas (`BP_ZonaAudio`) se validan en el parque y quedan BLOQ.
- **Próximo paso:** la v0.3.69 = interacción por mirada + `-ir` nuevos, mañana 19-11.

## 🧪 @qa-publisher
- **Hecho:** diagnóstico P0 (D1–D10), lectura de los reportes de cierre (los que desmienten D9/D10 y la v0.3.68), `Recoger_Logs.ps1` y el guion de paridad de los 36 momentos.
- **En curso:** build `v0.3.68_dev` para leer el `DXGI_ERROR_DEVICE_*` y el pase de GPU. **Es su tarea crítica.**
- **Pista nueva:** "parto en la sala" + `-ir` carga `Partida.sav`. Hay que repetir D1 **con la partida borrada** (`Probar.bat` → 5) para descartar estado o transición antes de seguir recortando render. También falta probar sin Lumen ni VSM por cvar (`r.DynamicGlobalIlluminationMethod=0`, `r.Shadow.Virtual.Enable=0`), que no estaba en D1–D10.
- **Puede avanzar ya:**
  - Casos 1–9 y **32–36 en el hospital** (Acto 3) con los `-ir` nuevos.
  - Humo diario por build en `auditoria/humo/`.
  - Checklist del tester y QA de idioma de los textos D0.
- **Bloqueado:** casos 10–31 = **BLOQ** (no ❌) hasta G0.
- **Riesgo:** G0 exige 3×15 min sin cierre. Con todas las hipótesis de render descartadas, el 22-11 es poco probable sin la causa raíz.
- **Próximo paso:** logs de la `_dev` + D1 sin partida, el 19-11.

## Reordenamiento propuesto (hospital y datos primero, el parque en paralelo)
| Build | Fecha | Contenido (todo probable sin `L_Parque`) |
|---|---|---|
| v0.3.69 | 19-11 | Interacción por mirada + `-ir=persecucion/expediente/cabina` + D0 de datos (62 Recuerdos, E1–E15, notas) |
| v0.3.70 | 20-11 | Flags HTML + `FlagCambiado` + `DT_Objetivos` + guardado v2 |
| v0.3.71 | 21-11 | **Acto 3 en `L_Hospital`:** persecución, aliento, SALA 4, expediente y `WBP_FinalDemo` (3 finales) |
| v0.3.72 | 22-11 | `WBP_VideoCabina` + elección E/Q (por `-dev`) + subtítulos · **G0** |
| v0.3.73 | 23-11 | *Si G0 pasa:* escalada por cintas, cortocircuito, fusible en el cajón, cabina en su sitio. *Si no:* pulido del hospital |
| v0.3.74 | 24-11 | *Si G0 pasa:* `BP_AI_Willy`/`Leon` + daño solo a oscuras. *Si no:* Plan B |
| rc1 | 25-11 | Freeze |

- **En paralelo**, con @tech-lead, technical-artist y @qa-publisher: solo la causa raíz del parque (logs de la `_dev`, D1 sin partida, sin Lumen/VSM y P3 de sub-niveles si apunta a carga). Nadie más toca `L_Parque` hasta G0.
- **Lo que se gana:** el Acto 3, los finales, la cabina y los datos (unas 6 de las 10 piezas del núcleo intocable) quedan hechos y probados **antes** del 22-11, pase o no el parque.

## Impacto en fechas
- **G0 (22-11): en riesgo alto.** La hipótesis de las sombras cayó y no hay causa raíz. Se mantiene la fecha; decidir con los logs de la `_dev`.
- **rc1 (25-11): la fecha se mantiene y el alcance mejora.**
  - Si G0 pasa: Actos 1 + 2 (hasta la cabina) + 3.
  - Si G0 falla: Plan B ampliado, con el hospital + el Acto 3 ya construido. Hay que decidir si el Acto 3 entra sin el parque (por ejemplo, TV → "Continuará" o un puente narrativo); eso es una **decisión del CTO**.
- La paridad completa sigue apuntando a rc2 el 3-12. Lo que dependa del parque se recorta en el orden aprobado del acta.
