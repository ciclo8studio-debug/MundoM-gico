# Contexto de Ciclo Studio Game en una página (actualizar en cada hito)

**Juego:** "Mundo Mágico: Turno Nocturno", terror en Unreal 5.8, solo Blueprints (vía MCP). Demo pública en itch.io el **10-12-2026**; testers hacia el **25/26-11**. El CTO decide; escribe en español de Chile y quiere respuestas cortas.
**3 actos (según el HTML, ver `auditoria/html_completo/RECORRIDO_HTML.md`):** 1 Hospital (`L_Hospital`) → 2 Parque (`L_Parque`: 4 interruptores, 4 cintas VHS, llave, casa, fusible, cabina VIDEO) → 3 persecución en el hospital, expediente y 3 finales (62 / ≥31 / <31 páginas).
**Versión actual:** v0.3.69 (arreglo H2 del parque; no probado). Historia detallada solo en `CHANGELOG.md` (leer solo la entrada que necesites).
**Hecho:** 1ª persona, linterna que delata (22 m / 7 m apagada) y ahuyenta (2,5 s), botella, Sombra con pausa y anti-atasco, escondites, Director, ruido, flags guardados, 15 páginas con texto (`DA_NovelaData`), avisos □/E, tubos en falla, LUT verdosa, portada web, `Probar.bat` (D1–D10).
**Bloqueo n.º 1:** al entrar al parque el juego cierra (GPUCrash, iGPU Iris Plus 8 GB). Ni las cvars, ni las sombras, ni la memoria lo arreglaron. **Pista fuerte:** con la partida guardada borrada, `-ir=parque` NO se cayó (>5 min); todos los cierres tenían `Partida.sav`. Detalle: `auditoria/html_completo/estado_parque_2026-11-18.md`.
**Fechas:** G0 del parque 22-11 · rc1 25-11 · rc2 3-12 · gold 8-12 · pública 10-12. Plan B: `Demo_SinParque` (hospital + Acto 3).
**Restricciones duras:** 8 GB RAM + iGPU (sin PIE; solo builds, 1/día); sin Event Tick nuevo; cada feature con interruptor en `DA_Tuning`; el CTO hace `git push`; nunca editor y juego a la vez; Perfil A (renderer) vetado.
**Decisiones aprobadas (18-11):** 3 niveles; 1ª persona principal; el parque vuelve a la demo; patrón `Arq_*`; 62 Recuerdos + 15 clínicas E1–E15; vida 4 golpes; canon de Isidora (leucemia + miocardiopatía, 03:33 del 13/03/1998); gate firme.
**Mapa de documentos:** plan `REUNION_2026-11-18_ReplicaHTML.md` · avance `REUNION_2026-11-18_Avance.md` · reingeniería `REINGENIERIA_2026-11-18.md` · ajustes `GUIA_TUNING.md` · agentes `ESTUDIO_AGENTES.md` · tablero `BOARD.md`.
