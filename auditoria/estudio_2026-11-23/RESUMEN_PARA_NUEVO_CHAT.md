# Resumen para retomar en un chat nuevo (cierre del 23-11-2026, noche)

Lee primero `CLAUDE.md` y `CONTEXTO_ESTUDIO.md`. Este archivo es el detalle de la sesión larga del 21 al 23-11. Todo lo hecho está en `CHANGELOG.md` (versiones 0.3.85 a 0.3.113) y en el panel del estudio.

## 1. Quién y cómo
- **CTO:** Francisco. Dicta por voz en español de Chile (frases cortas, a veces con palabras mal transcritas: "Sprit" = Sprint, "cortador de páginas" = contador). Quiere respuestas cortas, que se avance sin preguntar por cada archivo y que se le pregunte solo lo que decide él.
- **Reglas del estudio:** PC de 8 GB e iGPU; nunca editor y juego a la vez; solo builds empaquetadas (sin PIE); borrar solo con su OK; el `git push` lo hace él; commits con `Co-Authored-By`. Cada feature con interruptor en `DA_Tuning`; sin Event Tick nuevo; Perfil A (renderer) solo con su OK.
- **Repos:** proyecto Unreal `C:\Users\pansh\Documents\Unreal Projects\MUNDO_MÁGICO` (rama `unreal`); documentos y scripts `C:\Users\pansh\Desktop\MundoMagico_Project` (rama `sprint-1-demo-corta`). Hay unos 40 commits sin subir en los dos.

## 2. Qué se hizo (21 al 23-11)
- **Casona (0.3.88–0.3.108):** objetivo y HUD propios, sin minimapa del hospital; segundo piso recorrible; exterior de castillo; texturas más nítidas (filtro lineal, con OK del CTO); luna con relieve, domo de estrellas (`M_Estrellas`, sin colisión), luz azulada, exposición 0,55; cristales de ventana cálidos alineados a las ventanas con marco (`VentLuz_*`, `MI_Ventana_Casona`); niebla volumétrica apagada (colgaba la GPU); agachado visual por escala 0,72 (truco; el CTO lo acepta). Casona **congelada y aprobada** ("falta un piso", por revisar).
- **Hospital y recorrido (0.3.109–0.3.113):** Plan B activado y revertido (`Demo_SinParque` falso); `RevisarSalida` ya no se salta (mínimo -30000); `Salida_Y` de -12600 a **-4600**; `RecogerObjeto` solo suena y actúa con un objeto a menos de 200 cm; `Sombra_RepeleSeg` 2,5 → 1,5 s; mensajes de diagnóstico de la linterna en `RevisarLinterna`.
- **Estudio y proceso:** reunión corta del `producer` (`estado_plan.md`); criterios de 10/10 medibles por departamento (`camino_10_10.md`: tech 6, gameplay 4, world 6, ux 3, qa 3); panel del estudio con estado, camino al 10/10 y cuatro diagramas; kit de testers (`kit_testers_2026-11-26.md`); hojas `humo/rc1_2026-11-24.md` (cerrada por el CTO) y `humo/g0_parque_2026-11-23.md`.
- **Herramientas:** `Limpiar_Builds.ps1` (40 builds borradas con su OK: `Build/` de 85 a 12 GB), `Empaquetar_ZIP.ps1` (ZIP de 1,21 GB en 165 s), `Probar.bat` (prefiere la `_dev`; 8 = Casona; 9 = Shipping con panel), `ajustar_tuning.py`.

## 3. Decisiones del CTO
Casona visual congelada; RC1 armada (Shipping y `_dev` de la 0.3.108); G0 corrido y no pasó; "Plan B ya" y luego "reconecta" el parque; adelantar `Salida_Y`; Sprint A cerrado sin ejecutar el humo completo; seguir con el Sprint B. Pendiente de decidir: si el adelanto de la Casona entra a la demo pública; página bilingüe en itch.io; instalar butler.

## 4. Hallazgos técnicos clave
- **Cuelgue de GPU tras cada viaje de mapa** (4 de 4): un fotograma de 10 s con 790 a 1.240 MB de RAM libre. El PC tiene 5,7 GB ocupados con el juego cerrado (Claude 1,4 GB, `svchost` 580, Edge 259, Defender 210, Epic ~200). Descartado: la calidad; Lumen/VSM no concluyente (D5 también se colgó). **Prueba pendiente:** repetir con ≥ 4 GB libres.
- **Glitch del hospital:** `RevisarSalida` (cada tick del jugador) marca `SalidaHecha` en Y ≤ -3470 y `GlitchLlegar` teletransporta a (0,-4300,100) (la calle de afuera, mismo mapa); `BP_GestorNivel.VigilarSalida` abre `L_Parque` al pasar `Salida_Y`. El título "PARQUE MUNDO MÁGICO" y el minimapa del parque en esa calle son el diseño por coordenadas, no un choque.
- **Linterna (A3):** la lógica es correcta (aim > 0,96, distancia < 1800, +0,2 s por revisión; huye a `Sombra_RepeleSeg`); en 13,5 min no salió ningún mensaje de diagnóstico.
- **MCP:** `set_pin_value` no marca el archivo como modificado: mover un nodo (`set_node_position`), guardar y verificar la fecha del `.uasset`. Las lecturas largas se guardan en archivos: leerlas con Python. Nunca detener procesos de forma amplia (`dotnet`): el PC también empaqueta otro proyecto y el sistema lo bloquea.
- **Teclas reales:** F linterna, E o □ recoger, **Y** pista, T o R2 botella, ✕ saltar.

## 5. Pendientes, en orden
1. **Prueba de RAM** (cerrar Claude, Edge y Epic; `Probar.bat` → 3; hospital → parque; 2 min en el parque). Si no se cuelga: bajar el consumo tras el viaje y fijar requisitos para testers. Si se cuelga: otras hipótesis (por ejemplo un mapa intermedio de carga).
2. **Botella en el Pabellón 4** para probar A5 (lectura de mapas hecha: `L_Hospital` ya tiene 1 botella en (140,500,15), cerca del inicio; `L_Parque` tiene 10; falta ubicar el Pabellón 4 para poner una más) (tag `Botella`; había una lectura de mapas en curso para ubicar botellas, `scratchpad/info_mapas.py`).
3. **Flecha en vez del triángulo** del marcador de objetivo (pedido del CTO).
4. **Linterna (A3):** probar de nuevo con la Sombra de frente y quitar los mensajes de diagnóstico cuando pase.
5. **Interruptores faltantes en `DA_Tuning`:** botella y barra de ruido (A5), agachado, cielo y ventanas de la Casona. Una sola sesión de editor.
6. **Sprint B:** subtítulos, mezcla de audio, música ligada a la tensión, autoguardado y empuje del Director.
7. **Release:** instalar butler, página de itch.io v0.5.0, completar `[LINK]`, `[CLAVE]` y teclas de agacharse y correr en el kit, G0 formal del parque cuando el viaje no se cuelgue.
8. **Menores:** "falta un piso" en la Casona; Event Tick heredado del jugador (después de la demo); avisos `SetTimer` en cero (`LimpiarMensaje`, `AntesDePagina`); subir los commits (`git push origin unreal` y `git push origin sprint-1-demo-corta`).

## 6. Dónde está todo
`BOARD.md` (tablero) · `CHANGELOG.md` · `GUIA_TUNING.md` · `ESTUDIO_AGENTES.md` · `auditoria/estudio_2026-11-23/` (estado, camino al 10/10, panel, este resumen) · `auditoria/humo/` · `auditoria/kit_testers_2026-11-26.md` · panel en https://claude.ai/artifact/2jHerSAK3SJcb9rZeii2Nd (privado; copia local `panel.html`, ignorada por git).

## 7. Mensaje para abrir el chat nuevo
> Continúa Mundo Mágico (Ciclo Studio Game). Lee `CLAUDE.md`, `CONTEXTO_ESTUDIO.md` y `auditoria/estudio_2026-11-23/RESUMEN_PARA_NUEVO_CHAT.md`. Estamos en la v0.3.113 con el Sprint A cerrado. Sigue con la prueba de RAM del cuelgue tras el viaje al parque, la botella en el Pabellón 4 y la flecha del objetivo; luego el Sprint B.
