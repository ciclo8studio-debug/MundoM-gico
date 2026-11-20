# Resumen para continuar en otro Claude (traspaso 20-11-2026)

> Pégale este documento completo a Claude al empezar la conversación. Ese Claude **no tiene acceso a mi PC ni a Unreal**: sirve para pensar, diseñar, redactar y decidir. Lo que requiera tocar el proyecto lo hace el Claude Code de este PC.

## 1. Quién y qué
- **Estudio:** Ciclo Studio Game. Yo soy el **CTO**: decido, escribo en español de Chile y quiero respuestas cortas y directas.
- **Juego:** *Mundo Mágico: Turno Nocturno*, terror y supervivencia en primera persona, **Unreal Engine 5.8, solo Blueprints**. PC de 8 GB de RAM con iGPU Intel Iris Plus: nunca editor y juego a la vez, solo se prueba con builds empaquetadas (Shipping).
- **Fechas:** rc1 el **25-11**; gate del parque (G0) el 22-11; **demo pública en itch.io el 10-12-2026** (Nivel 1 completo y adelanto de la Casona).
- **Repos (privados, el `git push` lo hago yo):**
  - Unreal: `C:\Users\pansh\Documents\Unreal Projects\MUNDO_MÁGICO` (rama `unreal`, LFS).
  - Scripts/docs: `C:\Users\pansh\Desktop\MundoMagico_Project` (rama `sprint-1-demo-corta`).
  - Hay muchos commits locales sin subir (v0.3.57 a v0.3.81).
- **Versión actual:** v0.3.81. La última **build** probada por mí es antigua; **v0.3.73 a v0.3.81 no están probadas** (v0.3.75 se construyó pero no se jugó el final).

## 2. Los 3 niveles (decisión del 19-11)
| Mapa | Contenido |
|---|---|
| `L_Hospital` | Pabellón 4 (sala de inicio, pasillo de Urgencias) + **Ala de Internación nueva** (66 m, 12 cuartos, sala de espera, atmósfera de azulejo, fluorescentes, camillas, escombros). |
| `L_Parque` | Parque de diversiones completo (~480×275 m, ~1.430 actores). Tiene el **castillo** (portón en (0,-12140)). |
| `L_Casona` | Mansión de 2 pisos + ático, construida en bloqueo con packs (UnrealBucket, DetectiveOffice, SICKA). |
`L_Pabellon4` (el monolito original) y `L_HospitalGrande` se retiraron; todo lo del original está en Hospital y Parque (lo validé actor por actor). Respaldo: tag git `v0.3.41-monolito` y `Documents\Unreal Projects\Respaldos_Config`.

## 3. Recorrido (circular), todo con glitch + TV
Hospital → (final del ala larga, tras el glitch) → **Parque** → (portón del castillo) → **Casona** → (ático) → **Parque** → (cabina VIDEO, elegir "sala") → **Hospital** al fondo del ala con **payaso persiguiendo** (5,2 m/s, la linterna no lo frena) hasta el **Pabellón 4** → expediente y **3 finales** (62 páginas = verdadero, ≥31 "Lo que recuerdas", <31 "Expediente"). Detalle en `docs/RECORRIDO_JUEGO.md`.

## 4. Lo hecho en esta sesión (v0.3.57 a v0.3.81, resumen)
- Instalación selectiva de *Claude Code Game Studios* (31 agentes bajo 5 super-agentes), reingeniería, protocolo de eficiencia (agentes leen `CONTEXTO_ESTUDIO.md`, reunión corta = 1 agente).
- Causa del cierre del parque: el guardado traía la posición del hospital (se corrigió con `BP_Partida.Mapa`). **Probar.bat abría `L_Parque` con nombre corto y en Shipping caía a `L_Hospital`**: ya usa la ruta completa `/Game/MundoMagico/Maps/...` (sin confirmar en juego).
- Sistema: linterna, escondites, sombra con linterna, tubos que fallan, pistas (tecla Y), recoger lo que se mira, teclados de código desactivados (solo el del teatro), `DA_Tuning` con un interruptor por feature.
- Acto 3, castillo→casona, hospital largo, Casona (≈700 actores), marcadores, `-ir=casona|castillo`.
- Documentos clave en el repo de scripts: `CLAUDE.md`, `CONTEXTO_ESTUDIO.md`, `CHANGELOG.md`, `GUIA_TUNING.md`, `BOARD.md`, `docs/DISENO_CASONA.md`, `docs/RECORRIDO_JUEGO.md`, `ORDEN_PROYECTO.md`, `auditoria/mapas_2026-11-19.md`.

## 5. Pendiente (en orden)
1. **Hacer la build v0.3.81 y probarla:** `Probar.bat` → **5** (borrar partida) → **P → 1** (¿aparezco en el parque y no en el pabellón?) → **P → 6** (castillo) → **P → 7** (casona). Para fallos, opción **7** (ZIP de logs).
2. Verificar: portón del castillo se activa, salida del ático, persecución de vuelta (navmesh dinámico), FPS del parque en Shipping (hubo 3 FPS en una build Development, no sirve para medir).
3. **Decisiones mías abiertas:** qué da por terminado el parque (¿páginas de la Casona o la cabina?), si el portón se cierra tras volver, dónde termina el regreso (Sala 4 o recepción), y si el regreso al hospital cambia de ambiente.
4. Casona: ajustar a ojo la posición y rotación de los muebles (se colocaron por coordenadas sin verlos), puzle de álbumes (4 marcos), apagón, Sombra anciana (oído ×2), páginas 16–40.
5. Orden del proyecto: acciones que requieren editor o son destructivas (mover packs, borrar `HeartBeats`/`AutoFootsteps`, renombrar `ThirdPerson`) **después del 10-12**. Detalle en `ORDEN_PROYECTO.md`.
6. Roadmap: guardado v2, subtítulos, música, escalada por cintas, itch.io, kit de testers, tráiler (Remotion + OBS).

## 6. Reglas del estudio (mandan sobre todo)
- Trabajo autónomo: no preguntar por cada archivo; preguntar solo decisiones del CTO. **Confirmar antes de borrar, publicar o gastar.**
- Nunca 2 editores ni editor + juego a la vez. Sin PIE. Unreal siempre en modo liviano (`-dx11 -ModelContextProtocolStartServer -culture=en -dpcvars=r.RayTracing.Enable=0,...`). **Prohibido** cambiar el renderer (Perfil A) sin mi OK.
- Cada feature: interruptor en `DA_Tuning`, entrada en `CHANGELOG`, versión subida (`Config/DefaultGame.ini` → `ProjectVersion`). Sin Event Tick nuevo. Sin coordenadas a mano en BPs nuevos.
- Commits locales terminan con `Co-Authored-By: Claude ... <noreply@anthropic.com>`. El push lo hago yo.
- No escribir contraseñas, no capturar mi pantalla sin permiso, no borrar sin confirmar.

## 7. Trucos técnicos aprendidos (por si vuelves al Claude Code)
- Blueprints por MCP (`unreal-mcp`, puerto 8000) con `ProgrammaticToolset.execute_tool_script`. El DSL exige el editor con `-culture=en`; antes de reescribir una función hay que borrar sus nodos (salvo la entrada); `save_assets []` antes de cerrar.
- Un `MostrarMensaje` en la misma clase necesita parámetros con nombre (`:Texto`, `:Duracion`). Getters de DA sin guiones bajos.
- Las builds **Development** distorsionan los FPS (3 FPS vs fluido en Shipping): nunca medir con ellas.
- Primera llamada MCP tras abrir el editor suele fallar: reintentar.

## 8. Cómo pedirle ayuda a este otro Claude
Pídele cosas que no necesiten mi PC: diseñar el puzle de álbumes, escribir los textos de las 25 páginas de la Casona, el diálogo de la Sombra anciana, el guion del tráiler, la página de itch.io, la lista de pruebas para testers, o decidir las 4 decisiones abiertas. Lo que genere me lo traigo al Claude Code de este PC para aplicarlo.
