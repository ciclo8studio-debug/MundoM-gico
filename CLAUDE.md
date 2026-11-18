# Ciclo Studio Game — Mundo Mágico: Turno Nocturno

Juego de terror y supervivencia en **Unreal Engine 5.8** (solo Blueprints). Es una arquitectura de 3 niveles: Hospital → Casona → Parque.
- Demo pública en itch.io el **10-12-2026**: Nivel 1 completo y un adelanto de la Casona. Testers privados hacia el 26-11.
- El usuario es el **CTO**: decide, escribe en español de Chile y quiere respuestas cortas, con pocos tokens.

Este repositorio (scripts, documentos, builds) usa una instalación **selectiva** de
[Claude Code Game Studios](https://github.com/Donchitos/Claude-Code-Game-Studios) (MIT):
- `.claude/agents`: 31 agentes de estudio y de Unreal.
- `.claude/skills`: comandos como `/sprint-plan`, `/playtest-report`, `/bug-report`, `/perf-profile`, `/scope-check`, `/launch-checklist` y `/patch-notes`.
- `.claude/rules`, `.claude/docs` y `docs/engine-reference/unreal`.
- Configuración en `project.yaml`.

**No** se instalaron los hooks, la barra de estado ni el chequeo de actualizaciones.

## Proyecto

| Qué | Dónde |
|---|---|
| Proyecto Unreal | `C:\Users\pansh\Documents\Unreal Projects\MUNDO_MÁGICO` (repo git propio, rama `unreal`, LFS) |
| Este repo | rama `sprint-1-demo-corta` |
| Plan maestro | `ARQUITECTURA_3_NIVELES.md` |
| Afinación sin tocar BPs | `GUIA_TUNING.md` (`DA_Tuning`) |
| Historial | `CHANGELOG.md` (versión en `Config/DefaultGame.ini` → `ProjectVersion`) |
| Tablero de agentes | `BOARD.md` (los agentes se comunican solo por ahí) |
| Build | `herramientas/Construir_Version.ps1` (Unreal cerrado) → `Build\v<versión>` |
| Probar | `Probar.bat` (abre la build más nueva, por zonas, con el panel de FPS `-dev`) |

## Reglas del estudio (mandan sobre las del framework)

- **Ritmo:** se trabaja de forma autónoma.
  - No se pregunta "¿puedo escribir X?" por cada archivo; se pregunta solo por decisiones del CTO.
  - Se confirma antes de borrar algo, publicar o gastar.
- **PC de 8 GB e iGPU:**
  - Nunca 2 editores ni editor + juego a la vez.
  - El PIE no se usa: se prueba solo con builds empaquetadas.
  - Unreal siempre en modo liviano: `-dx11 -ModelContextProtocolStartServer -culture=en -dpcvars=r.RayTracing.Enable=0,...`.
  - Nada de Perfil A (cambios de renderer) sin el OK del CTO.
- **Blueprints por MCP** (`unreal-mcp`, puerto 8000):
  - DSL con `-culture=en`.
  - Antes de reescribir una función, borrar sus nodos salvo la entrada.
  - `save_assets []` antes de cerrar.
- **Cada feature** lleva un interruptor en `DA_Tuning`, una entrada en el `CHANGELOG` y la versión subida.
  - Sin Event Tick nuevo.
  - Sin coordenadas escritas a mano en BPs nuevos.
- **Git:**
  - Commits locales que terminan con `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.
  - El `git push` lo hace el CTO; se le dan los comandos.
  - Los repos son privados.
- **Prohibido:** escribir contraseñas, capturar la pantalla del usuario sin pedir permiso y borrar sin confirmar.

## Agentes y comandos del framework

- Coordinación: `@.claude/docs/coordination-rules.md`
- Guía de flujo: `docs/WORKFLOW-GUIDE.md`
- Unreal 5.8: `docs/engine-reference/unreal/VERSION.md`
- Agentes primarios: `unreal-specialist`, `ue-blueprint-specialist` y `ue-umg-specialist`. Dirección: `creative-director`, `technical-director` y `producer`.
- Los comandos web (`/prototype`, el "grow" en Vite y TypeScript) no aplican a este proyecto Unreal.
