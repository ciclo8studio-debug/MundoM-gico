# Plan de mejora rápida (decisión del CTO, 11-11)

El CTO **descartó la refundación**: se sigue con el juego actual y se le suma lo mejor de las dos reuniones del 11-11 ([plan de mejora](REUNION_2026-11-11_PlanMejora.md) y [refundación](REUNION_2026-11-11_Refundacion.md)).

**itch.io (actualizado el 12-11):** se cancelan el restringido del 13-11 y el público del 16-11. Habrá testers privados alrededor del 26-11 y la publicación es el **10-12** con el Nivel 1 completo. El plan de trabajo vigente está en [`ARQUITECTURA_3_NIVELES.md`](ARQUITECTURA_3_NIVELES.md).

## Fases

| Fase | Fechas | Qué | Para qué |
|---|---|---|---|
| **0 · Línea base** | 12-11 | El CTO juega la v0.3.40 con las checklists. Mide fps y duración, y revisa si se puede llegar al teatro y a la bodega. | Saber de dónde partimos |
| **1 · Kit del CTO** | 11–13-11 | Saltos `-ir=<zona>` y overlay `-dev`. `Probar.bat`. **`DA_Tuning`** con unos 25 valores e interruptores. `GUIA_TUNING.md`. | Que el CTO ajuste y pruebe solo |
| **1b · Dos mapas** | 13-11 | `L_Hospital` y `L_Parque` separados. Al parque solo se llega con el glitch y la transición TV/VHS (`OpenLevel`). Build v0.3.42. ([acta](REUNION_2026-11-11_TechLead.md)) | Hospital y parque como lugares distintos; menos memoria por mapa |
| **2 · Hospital AAA** | 14–15-11 | Vestir el hospital con `Horror_Hospital`: muros, zócalos, techo, puertas y camas, más suelo mojado y tubos fluorescentes ([plan](auditoria/plan_hospital_horror.md)). | El primer minuto es el que vende |
| **3 · Ritmo del miedo** | 16–17-11 | `BP_DirectorMiedo`: un enfriamiento global y tramos de calma; los sustos le piden turno. El teatro pasa a ser solo audio. Isidora deja una pista. Los sustos que queden fuera del recorrido se mueven. | Que no se repita el molde y la curva se pueda diseñar |
| **4 · Parque** | 18–20-11 | Luz en 3 niveles: se apaga la mitad de la avenida. Las primitivas se cambian por packs: silueta → payaso en negro, hojas → `GV_FreeShrubs`, puestos → `SoulCity`. Un hito de luz por atracción. | Que la luz cuente la historia y no haya formas de juguete |
| **5 · Amenaza y pulido** | 21-11 en adelante | El payaso avanza cuando no lo miras, con golpe y respawn; se usan escondites y linterna. ZIP más liviano (`DirectoriesToNeverCook`). Slider de SFX, subtítulos de sustos y botones de PS4. Limpieza de builds viejas. | Riesgo real y una publicación más sólida |

El **Perfil A** (apagar el ray tracing y MegaLights) se decide junto con el hardware el 17-11.

## Reglas
- **Una build al día**, con 1–2 features cada una. Cada feature lleva su interruptor en `DA_Tuning` y su línea en la checklist.
- **Prohibido** en Blueprints nuevos: `Event Tick` y coordenadas fijas.
- **Congelar 48 h antes de publicar.**
- **Primero ajusta el CTO**, y a Claude se le pide solo lógica nueva.
