# Ciclo Studio Game: reunión de avance (18-11-2026, tarde)

Informes: `auditoria/html_completo/avance_tech-lead.md` y `avance_equipos.md`. Hechos del cierre: `estado_parque_2026-11-18.md`.

## Estado del plan
| Super-agente | Estado |
|---|---|
| 🛡️ @tech-lead | P0 hecho, v0.3.67 y v0.3.68 hechas. **El cierre del parque sigue abierto**: D9, D10 y la v0.3.68 se cayeron. El CHANGELOG ya está corregido. P1 y la progresión van atrasados. |
| 🕹️ @gameplay-engineer | Sprint A (A3/A5/A7) hecho. Su plan del HTML (escalada por cintas, Willy y León) queda **bloqueado por el parque**. |
| 🏚️ @world-builder | **Va en plazo**: las 62 páginas (Recuerdos) y E1–E15 son datos y no dependen del parque. |
| 🎨 @ux-toolsmith | **Atrasado en la interacción por mirada**, que no depende del parque. |
| 🧪 @qa-publisher | Lo crítico: build Development `v0.3.68_dev` con logs, en curso. |

## Hipótesis del cierre del parque (@tech-lead, de más a menos probable)
1. **H1 · Lógica que corre en BeginPlay dentro del parque**: glitch, lluvia, `BP_IsidoraAparicion` y restos del mapa único. → Mirar las últimas líneas del log Development y PIE de `L_Parque`.
2. **H2 · Partida guardada + `-ir`**: el jugador aparece en la **sala del hospital, que sigue dentro de `L_Parque`** (quedan `NavMesh_Hospital` y otros restos) y se repite la secuencia del glitch. Coincide con lo que vio el CTO: "parto en la sala de hospital". → Borrar partida (opción 5) y D1, contando los `LoadMap` del log.
3. **H3 · Un shader o material cocinado** que el driver Intel en DX11 no termina (Substrate está activo, más Nanite fallback y Landscape). → Buscar `Missing shader` o device removed en el log.
4. H4 · Linterna con LightFunction · H5 · un pase de render concreto. → Pruebas D11–D16 con show flags.
5. ~~Memoria~~: descartada (había 800 MB libres). Con TdrDelay=10 es un **cuelgue**, no un cuadro lento.

## Plan reordenado (para no perder días)
- **Primero el hospital y los datos**, mientras solo @tech-lead, technical-artist y @qa-publisher buscan la causa raíz del parque. Nadie más toca `L_Parque` hasta pasar G0.
- v0.3.69: interacción por mirada + `-ir` nuevos + 62 páginas como datos.
- v0.3.70: flags, progresión y guardado v2.
- v0.3.71: **Acto 3 completo en `L_Hospital`** (persecución del payaso, Sala 4, expediente, 3 finales).
- v0.3.72: cabina VIDEO y elección E/Q como widgets + subtítulos.
- v0.3.73–74: lo del parque (Acto 2), **solo si pasa G0**.
- **Fechas:**
  - G0 del 22-11 en **riesgo alto**; el producer decide el 20-11 a las 20:00.
  - rc1 del 25-11 se mantiene. Si el parque no pasa, sale con Plan B ampliado: hospital + Acto 3.

## Lo que necesita hacer el CTO
- Cuando esté la build `v0.3.68_dev`:
  1. `Probar.bat` → **5** (borrar partida).
  2. **P → D1** (parque directo).
  3. Cuando se caiga, **7** (ZIP de logs).
- Con ese log se sabe la causa exacta.
