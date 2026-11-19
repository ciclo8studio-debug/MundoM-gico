# Ciclo Studio Game — organigrama del estudio: 5 super-agentes y 31 agentes de Claude Code Game Studios

**Cadena:** CTO → **@tech-lead** (primero de la cadena, autorizado a proponer mejoras AAA) → los otros 4 super-agentes → sus equipos.

Reglas:
- Los agentes de estudio **no actúan solos**: los convoca su super-agente.
- Todo se coordina por `BOARD.md`. No hay correos ni notificaciones externas.
- Los agentes **leen, auditan y proponen**. La implementación en Unreal la hace Claude, de a un cambio por vez, con un editor y una build al día.
- Cada mejora lleva su interruptor en `DA_Tuning` y su entrada en el `CHANGELOG`.

| Super-agente | Departamento | Agentes de estudio (31) |
|---|---|---|
| 🛡️ **@tech-lead** | Dirección, técnica, rendimiento, build | `creative-director` · `technical-director` · `producer` · `lead-programmer` · `unreal-specialist` · `engine-programmer` · `performance-analyst` · `devops-engineer` |
| 🕹️ **@gameplay-engineer** | Mecánicas, IA, Blueprints | `game-designer` · `systems-designer` · `gameplay-programmer` · `ai-programmer` · `ue-blueprint-specialist` · `prototyper` |
| 🏚️ **@world-builder** | Niveles, narrativa, arte | `level-designer` · `world-builder` · `narrative-director` · `writer` · `art-director` · `technical-artist` |
| 🎨 **@ux-toolsmith** | UX, UI, audio, herramientas | `ux-designer` · `ui-programmer` · `ue-umg-specialist` · `accessibility-specialist` · `tools-programmer` · `audio-director` · `sound-designer` |
| 🧪 **@qa-publisher** | QA, publicación, idiomas | `qa-lead` · `qa-tester` · `release-manager` · `localization-lead` |

## Comandos útiles por super-agente
- **@tech-lead:** `/milestone-review`, `/scope-check`, `/perf-profile`, `/architecture-decision`, `/sprint-plan`, `/cut`
- **@gameplay-engineer:** `/quick-design`, `/balance-check`, `/design-system`, `/team-combat`
- **@world-builder:** `/team-level`, `/team-narrative`, `/art-bible`, `/asset-audit`
- **@ux-toolsmith:** `/team-ui`, `/team-audio`, `/ux-review`, `/localize`
- **@qa-publisher:** `/playtest-report`, `/bug-report`, `/smoke-check`, `/release-checklist`, `/patch-notes`, `/team-release`

## Estado (18-11, cierre)
Versión v0.3.75; builds v0.3.73 y v0.3.75 sin probar por el CTO. Cierre del parque resuelto (v0.3.69); FPS en Shipping por medir. Decisión G0 el 20-11 20:00. Detalle: `REUNION_2026-11-18_Cierre.md`.

## Evaluaciones
- 18-11-2026, evaluación completa (HTML→Unreal y demo): `auditoria/estudio_2026-11-18/`, un informe por super-agente, y `PLAN_ESTUDIO_2026-11-18.md`, que es el consolidado.
