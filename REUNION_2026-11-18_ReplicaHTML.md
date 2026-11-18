# Ciclo Studio Game: reunión "Replicar el HTML completo en Unreal" (18-11-2026)

**Convoca:** el CTO, porque el fin de la demo estaba mal planteado. El HTML no termina en el hospital: sigue en el parque y termina con la persecución y el expediente. **Asisten:** los 5 super-agentes con sus equipos.

**Insumos:**
- `auditoria/html_completo/RECORRIDO_HTML.md`: el equipo recorrió el HTML completo leyendo su código; incluye 36 momentos clave.
- `auditoria/html_completo/reunion_*.md`: un informe por departamento.

## Lo que es el juego (según el HTML)
| Acto | Dónde | Qué pasa |
|---|---|---|
| **1** | `L_Hospital` | Pabellón 4: despertar y linterna → pasillo de Urgencias → salida por el glitch → TV/VHS. |
| **2** | `L_Parque` | Entrada: 4 interruptores (el 4.º hace cortocircuito una vez) → **4 cintas VHS** → llave en los disfraces de la bodega → casa de Alfredo → fusible en el cajón → **cabina VIDEO** (video nítido o dañado según las páginas) → elegir con E/Q. |
| **3** | `L_Hospital` | **Persecución del payaso** (5,2 m/s, la luz no lo detiene) → Sala 4 → **expediente** → **3 finales** por páginas: 62 verdadero, ≥31 "Lo que recuerdas", <31 "Expediente". |

- Amenazas que **escalan con las cintas**: muñeca (1.ª) → payaso que avanza cuando no lo miras (3.ª) → Willy y León (4.ª).
- Willy y León **solo hacen daño con la linterna apagada**. La luz los ahuyenta, pero hace que te vean de más lejos.
- **Duración:** 15–25 min por la ruta crítica; 45–75 min para el 100 %.

## Estado de Unreal frente al HTML
- **Acto 1:** casi completo. Faltan la silueta tras el vidrio, la paciente del pañuelo y la puerta SALA 4.
- **Acto 2:** **las zonas existen** y el BP del jugador **ya tiene la lógica heredada** (interruptores, llave, casa, fusible, cintas, cabina, `EleccionSala`, finales). **Pero no se juega desde la v0.4.0**, porque el parque cierra el juego en la iGPU.
  - Faltan: la escalada por cintas, el daño solo a oscuras, la IA nueva para Willy y León, la cabina con video, la tele de la casa, el fantasma del tren, el público del teatro y el cortocircuito.
- **Acto 3 y finales:** no existen en `L_Hospital`. Hoy la persecución ocurre en el parque.
- **UX:** de 28 elementos, 2 están hechos, 20 parciales y 6 faltan. Faltan archivo de notas, cabina, elección E/Q, barra de ruido, tele y avisos del teatro.
- **Hallazgo de @tech-lead:** el parque **sí corría** en la iGPU cuando era un solo mapa (v0.3.1 y v0.4.0, el 9-11). Desde la separación en dos mapas (v0.3.42) **no se ha jugado estable**. Hay 2 sospechosos: la memoria del hospital que queda cargada y el render (Lumen, VSM, unas 45 luces, niebla volumétrica).

## Bloqueo n.º 1: que el parque no se caiga
| Paso | Qué | Riesgo |
|---|---|---|
| **P0** (CTO, sin Unreal) | **6 pruebas con `Probar.bat` → P → D1…D6**, de 3 min cada una. Anotar si se cae y los fps. D1 (directo) contra D5 (ruta normal) separa memoria de render. | Nulo |
| P1 | `AplicarPerfilMapa`: cvars por mapa en runtime (la combinación que gane en P0), aplicados en negro antes de `OpenLevel`. Si apunta a memoria: mapa de paso + `obj gc`. | Bajo |
| P2 | Luces sin sombra, ≤10 visibles, neones emisivos, `CullDistanceVolume`, ocultar por zona. Meta: <1.200 draw calls y ≥25 fps en Baja. | Bajo-medio |
| P3 | Sub-niveles (Oeste, Este, Norte) con Level Streaming, sin World Partition. | Medio |
| Plan B | `Demo_SinParque` (ya existe): publicar solo el hospital. | — |

**Gates:**
- **G0**, go/no-go el 22-11: el parque aguanta 3 sesiones de 15 min en Baja sin cierre y a ≥25 fps.
- **Gate final**, el 1-12: un tester con iGPU aguanta 10 min sin cierre.

## Hoja de ruta (reemplaza la del 18-11; el Sprint B del sótano y la morgue pasa a después del 10-12)
| Build | Fecha | Contenido |
|---|---|---|
| — | 19-11 | **P0 diagnóstico** (CTO) + v0.3.67 lista (parque reconectado) |
| v0.3.68 | 20-11 | P1 perfil por mapa + **interacción por mirada** (se toma el objeto que miras) |
| v0.3.69 | 21-11 | P2 recorte de render del parque |
| v0.3.70 | 22-11 | Progresión: flags con nombres del HTML, `FlagCambiado`, objetivos encadenados, guardado v2 · **G0** |
| v0.3.71 | 23-11 | Acto 2 completo: cortocircuito del 4.º interruptor, 4 cintas, llave → casa → fusible en el cajón |
| v0.3.72 | 24-11 | Cabina VIDEO + elección E/Q + **Acto 3 en `L_Hospital`** (persecución, Sala 4, expediente, 3 finales) |
| **v0.4.0-rc1** | **25-11** | Freeze → testers. Si G0 falló: rc1 con Plan B (solo el hospital). |
| v0.4.x | 26-11 → 1-12 | Escalada por cintas (muñeca → payaso → Willy y León como hijos de `BP_AI_Sombra`, daño solo a oscuras), 62 páginas + expediente E1–E15, notas, mochila y minimapa, subtítulos y mezcla, sustos faltantes, P3 si hace falta |
| rc2 / gold / pública | 3-12 / 8-12 / 10-12 | Paridad: 23/23 momentos obligatorios + ≥8/13 deseables y 2 partidas completas sin cierre |

**Si no alcanza, se recorta en este orden:** puzle del teatro → tren fantasma y notas → muñeca → payaso ángel → un solo enemigo en vez de Willy y León → páginas del video solo las 15 + `T_Pag_*`.
**Núcleo intocable:** linterna, hospital, energía, 4 cintas, llave/casa/fusible, cabina, persecución y expediente con finales.

## Divergencias aceptadas (no cuentan como falla de paridad)
1ª persona, Sombra en el hospital, transición TV/VHS, vida de 4 golpes y batería ajustada por mapa (0,35 %/s en el hospital y unos 0,6 %/s en el parque).

## Decisiones del CTO: ✅ las 9 aprobadas como se recomendaron (18-11)
1. **El parque reemplaza al Sprint B** (sótano y morgue) para el 10-12 (sí).
2. **Cvars por mapa en runtime**, y si no bastan, ajustes en `DefaultScalability.ini`. No se recompilan shaders ni se aplica el Perfil A al proyecto (sí).
3. **Gates G0 (22-11) y final (1-12)**, con Plan B `Demo_SinParque` si fallan (sí).
4. **Páginas:** **RECUERDOS 1–62** (imágenes y pies del HTML) cuentan para el video y los finales 62/31; el HUD vuelve a "/62". Las 15 clínicas pasan a **EXPEDIENTE E1–E15** en el archivo de notas (sí).
5. **Vida:** mantener 4 golpes (aprobado) o volver a los 6 corazones del HTML (mantener 4).
6. **Fusible en el cajón**, como en el HTML, en vez del puzle del reloj (sí).
7. **Voces:** 8 flashbacks con voz grabada offline y el resto con subtítulos (sí).
8. **Cortocircuito del 4.º interruptor** de vuelta (sí).
9. **RC1 del 25-11 con alcance reducido** si G0 no pasa (sí).

## Lo que necesita hacer el CTO ahora
1. Hacer la build **v0.3.67** (parque reconectado) para la prueba D5 de la ruta normal.
2. Correr **P0: `Probar.bat` → P → D1…D6** y anotar en cada prueba si se cae y los fps. Después, `Probar.bat` → 7 (ZIP de logs).
