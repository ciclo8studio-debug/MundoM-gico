# @qa-publisher · Reunión "Replicar el HTML completo en Unreal" (18-11-2026)

Equipo: qa-lead (gate y criterio), qa-tester (casos), release-manager (calendario), localization-lead (textos). Base: `RECORRIDO_HTML.md` (36 momentos), CHANGELOG 0.3.40–0.3.67, `reingenieria_2026-11-18/qa-publisher.md`, `Probar.bat`, BOARD.

## 1. Reglas del guion de paridad
- **Paridad de experiencia, no numérica.** Se compara el *beat* (qué pasa, en qué orden, qué siente el jugador), no los números. Divergencias ya aprobadas por el CTO que **no** cuentan como fallo: 1ª persona, vida de 4 golpes (HTML 6), batería 0,35 %/s (HTML 0,9), 15 páginas (HTML 62), la Sombra y los escondites en el hospital, y la transición glitch → TV → "PLAY 03:17" en vez del fundido.
- **Estado esperado en UE** es una estimación hecha leyendo el CHANGELOG y el BOARD, no jugando: ✅ hay (probado en `.exe`) · 🟡 hay, sin probar · 🔀 distinto al HTML · ❌ falta · ❓ sin evidencia (verificar).
- **Prioridad:** **O** = obligatorio para la demo del 10-12 · **D** = deseable.
- Antes de cada sesión: cerrar Unreal y el navegador, `Probar.bat` → 5 (borrar partida), calidad **Baja**, panel `-dev`. Anotar los fps mínimos por zona. Resultado en `auditoria/humo/paridad_v<ver>.md` con ✅/❌/⚠️/**BLOQ** (no se pudo probar por el parque).
- **`-ir=` propuestos** (no existen hoy; tarea S para tools-programmer): `casa`, `cabina` (4 cintas + fusible), `tren`, `persecucion` (hospital oeste con `chase.on`), `expediente` (Pabellón 4 final). Sin ellos, los casos del final solo se prueban con la ruta completa (≈20 min por intento).

## 2. Casos de paridad (los 36 momentos)
Columna "Llegar": opción de `Probar.bat` (1 sala, 2 pasillo, 3 juego normal, P1 entrada, P2 teatro, P3 noria, P4 bodega, P5 autos) o `-ir` propuesto (*cursiva*).

| N.º | P | Zona · Llegar | Pasa si (igual que el HTML) | UE hoy |
|---|:-:|---|---|:-:|
| 1 | O | Menú · 3 | Portada animada, lluvia, música al primer clic; Iniciar/Continuar/Config | ✅ |
| 2 | O | Sala · 3 | Despiertas acostada, desenfoque + parpadeo, objetivo "Despierta..." | ❓ |
| 3 | D | Sala · 1 | "[E] Incorporarte", animación ~3 s, giro al este | ❌ |
| 4 | O | Sala · 1 | Linterna brilla en el mostrador; al tomarla 80 %, aviso de F | ✅ |
| 5 | O | Sala · 1 | Sin linterna: "Está demasiado oscuro"; con ella → pasillo | ✅ |
| 6 | D | Pasillo · 2 | Silueta tras el vidrio se esfuma al iluminarla | ❌ |
| 7 | D | Pasillo · 2 | Registro de Urgencias 12/03/1998 legible | 🔀 (página de la Novela) |
| 8 | O | Pasillo · 2 | Luces se apagan en cadena → silla de ruedas → grito → manchas | 🔀 (susto + Sombra + tubos) |
| 9 | O | Pasillo → parque · 3 | Salida por Urgencias → llegada a la entrada del parque, **sin cierre** | 🔀 🔴 GPUCrash iGPU |
| 10 | O | Entrada · P1 | "Todo el parque está sin energía", solo luna | 🟡 |
| 11 | O | Entrada · P1 | 4 palancas en cualquier orden, rojo → verde, "Luces encendidas (n/4)" | ✅ (v0.3.1) |
| 12 | D | Entrada · P1 | La 4.ª palanca falla la 1.ª vez (cortocircuito) | ❓ |
| 13 | O | Entrada · P1 | 4/4: parque se enciende escalonado + silueta gigante bajo el letrero | 🔀 (silueta al llegar) |
| 14 | D | Entrada · P1 | Recoger el mapa → minimapa ampliable | ❓ |
| 15 | O | Parque · P3 | Cinta 1 flota con luz naranja, +batería; aparece la muñeca | ❓ / muñeca 🟡 |
| 16 | O | Casa · *casa* | Puerta cerrada → nota "PUERTA CON LLAVE", objetivo "Busca la llave" | 🟡 |
| 17 | O | Bodega · P4 | Abrir con E, "Huele a naftalina", goteo | 🟡 |
| 18 | D | Bodega · P4 | Nota LISTA DE PERSONAJES en el escritorio | ❌ |
| 19 | O | Bodega · P4 | Disfraces se mecen → caen llave + pilas | 🔀 (`BP_LlaveFlag`) |
| 20 | O | Casa · *casa* | Usar la llave abre; "Algo se mueve arriba" | 🟡 |
| 21 | D | Casa · *casa* | Tele con voces, carta y diario de Alfredo | ❌ |
| 22 | O | Casa · *casa* | Cajón: fusible + pilas; armario: botiquín | 🟡 (clímax Sprint 2) |
| 23 | O | Casa · *casa* | Cinta 2 dentro de la casa | ❓ |
| 24 | D | Parque · P1 | Tras la 3.ª cinta el payaso avanza solo si no lo miras | 🔀 (`BP_PayasoAcecho`, por Director) |
| 25 | D | Teatro · P2 | "Nadie respira" + figura en el escenario | 🟡 |
| 26 | D | Teatro · P2 | Puzle Oso Rey → Cerdo → Mago → Willy → Mascota; error reinicia | ❌ |
| 27 | D | Tren fantasma · *tren* | Fantasma salta + nota VAGÓN 4 | ❌ |
| 28 | O | Parque · *cabina* | 4.ª cinta: cabina se enciende, despiertan Willy y León | 🔀 (enemigos ya activos) |
| 29 | O | Parque · *cabina* | Enemigos: te ven más con luz, la linterna los ahuyenta, botella distrae, daño solo a oscuras | 🔀 (solo la Sombra lo tiene) |
| 30 | O | Cabina · *cabina* | Video de las cintas; páginas faltantes se ven dañadas; se salta con E | ❌ |
| 31 | D | Cabina · *cabina* | Elección E (sala) / Q (quedarse y volver) | ❌ |
| 32 | O | Hospital · *persecucion* | "¡Corre a la sala!" desde el extremo oeste | 🔀 (huida a la entrada del parque) |
| 33 | O | Hospital · *persecucion* | ¡BANG! payaso persigue; caminando te alcanza, corriendo llegas justo | 🔀 |
| 34 | O | Hospital · *persecucion* | Puerta SALA 4 → "Lee el expediente de Isidora" | ❌ |
| 35 | O | Sala · *expediente* | Cinemática + final según páginas (15 / ≥8 / <8) con estadísticas | 🟡 (contador 15; A8 pendiente) |
| 36 | D | Sala · *expediente* | Q vuelve al parque; con todas las páginas, tesoro "Novela completa" | ❌ |

Conteo estimado hoy: O = 23 (4 ✅, 6 🟡, 8 🔀, 2 ❌, 3 ❓). D = 13 (casi todos ❌/❓).

## 3. Playthrough de referencia (ruta crítica, sin `-ir`)
Lo juega el CTO de una vez, con cronómetro, en `Probar.bat` → 3. Ruta fija para que los tiempos sean comparables entre builds. Tolerancia: **±30 % por tramo** frente a la ventana; si un tramo sale >50 % más largo, es un bug de guía (S3) o de bloqueo (S2).

| Hito (marcar la hora) | Ventana acumulada | Momentos |
|---|:-:|---|
| Iniciar → incorporarse → linterna | 0:00 – 2:00 | 2–4 |
| Puerta SALIDA → pasillo → apagón/silla | 2:00 – 4:00 | 5, 8 |
| Salida → TV/VHS → entrada del parque (**fps al llegar**) | 4:00 – 5:30 | 9–10 |
| 4/4 interruptores + silueta | 5:30 – 7:30 | 11–13 |
| Avenida: cinta 4 (frente al castillo) → casa cerrada | 7:30 – 10:00 | 15, 16 |
| Cinta 3 (oeste del castillo) → cinta 1 (noria) | 10:00 – 13:00 | 15, 24 |
| Bodega: disfraces → llave | 13:00 – 15:00 | 17, 19 |
| Casa: llave → fusible → cinta 2 (4/4: despiertan enemigos) | 15:00 – 18:00 | 20, 22, 23, 28 |
| Cabina VIDEO → video → E | 18:00 – 20:30 | 29–31 |
| Persecución → SALA 4 → expediente → final | 20:30 – 22:00 | 32–35 |

Referencia HTML: 15–25 min. Meta UE: **18–25 min** (el hospital con la Sombra suma unos 2 min). Anotar también: golpes recibidos, batería al llegar a la cabina, fps mínimos por tramo y cualquier momento en que no supo qué hacer.

## 4. Gate previo: el parque estable (G0)
El parque cierra el juego en la iGPU (0.3.63/0.3.67). **Sin G0, los casos 10–31 se marcan BLOQ, no ❌**, y no se juntan bugs de contenido sobre un mapa que se cae.
- **G0 pasa si:** 3 sesiones seguidas de **15 min en `L_Parque`** (P1 y llegada por la ruta normal), calidad Baja, Iris Plus, **0 GPUCrash**, ≥25 fps en la avenida (≥20 aceptable en la cabina y la casa), y el log sin "out of video memory".
- Mientras tanto se prueba el **Acto 3 por su cuenta**: los casos 32–35 viven en `L_Hospital` y no necesitan el parque (de ahí la urgencia de `-ir=persecucion` y `-ir=expediente`).
- Cada arreglo de rendimiento del parque se prueba con su interruptor en `DA_Tuning` apagado y prendido (regla 6 de "listo").

## 5. Criterio de "paridad alcanzada" (demo del 10-12)
- **Obligatorio:** los **23 O en ✅** (⚠️ solo si es S3/S4) + G0 + playthrough de referencia completo sin `-ir` en la ventana de 18–25 min, **dos veces seguidas** sin cierre.
- **Deseable:** ≥8 de los 13 D (en total ≥31/36 = **86 %**). Los D que no entren van a la página como "llegan en la próxima actualización", no se prometen.
- **Simplificaciones aceptables** (no rompen la paridad si el CTO las aprueba): video de la cabina con 4 placas de 6–8 s en vez de 12 segmentos; elección E/Q reducida a "E: ir a la sala"; finales con umbrales 15 / ≥8 / <8.
- **No aceptable:** saltarse la cabina, que la 4.ª cinta no cambie el parque, o terminar sin el expediente (el final es la recompensa por las páginas).
- Se mantienen las reglas del gate firme: 0 S1 para RC, 0 S2 para gold.

## 6. Calendario (release-manager)
| Fecha | Antes | Propuesta |
|---|---|---|
| 19–22-11 | Sprint A | **G0 del parque** + `-ir` nuevos + casos 32–35 en el hospital. **22-11: go/no-go del parque (CTO)** |
| **25-11 RC1** | Hospital | **Se mantiene la fecha y se recorta el alcance.** Si G0 pasa: Acto 1 + entrada del parque (casos 1–14) con un aviso "Continuará" tras la energía. Si G0 falla: hospital solo, con `Demo_SinParque` = verdadero (pantalla FIN de la 0.3.66) |
| 26–28-11 | Testers | Igual. El formulario suma una pregunta sobre el parque (cierres o lentitud) si entra |
| 1-12 | Freeze | **Freeze de contenido = los 23 O implementados** (cintas, cabina, persecución inversa, SALA 4, finales) |
| **3-12 RC2** | Sprint B | **Primera build con paridad completa.** Playthrough de referencia ×2 del CTO |
| 4–7-12 | Arreglos | Solo S1/S2 de paridad; capturas y página sobre la RC2 |
| 8-12 / 10-12 | Gold / Public | Sin cambios **si** el 1-12 hay ≥20/23 O. Si no: **plan B**, se publica Acto 1 + parque hasta la casa con "Continuará", y la paridad completa pasa a una actualización (≈20-12) |

Riesgo: faltan o difieren 13 de los 23 O, y además está el G0. Una build al día con humo es la única forma de llegar; **no se recomienda mover la RC1**, sino moverle el alcance. La página de itch.io (hoy promete "hospital + Sombra") se reescribe sobre la RC2 con el parque solo si G0 pasó.

## 7. Localización (localization-lead)
Los textos nuevos de paridad (15 notas, mensajes de cintas/cabina/persecución, finales) van directo a `ST_Mensajes`/`ST_UI` en español, copiados del HTML (ya en español) y revisados con el canon aprobado (03:33 del 13/03/1998). QA de idioma: ñ, tildes y "«»" en notas y finales.

## 8. Decisiones CTO
1. ¿Apruebas los **23 obligatorios** y el criterio 23/23 O + ≥8/13 D?
2. ¿RC1 el 25-11 **con alcance recortado** (en vez de mover la fecha) y go/no-go del parque el 22-11?
3. ¿Plan B si el 1-12 hay menos de 20/23 O: publicar el 10-12 hasta la casa con "Continuará"?
4. ¿Aprobar las simplificaciones de la cabina (4 placas) y de la elección E/Q?
