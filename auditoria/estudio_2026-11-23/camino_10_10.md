# Camino al 10/10 — criterios medibles por departamento (23-11-2026)

El 6/10 del 18-11 era la opinión de cada super-agente. Aquí cada departamento tiene **5 criterios con prueba**; 10/10 = los 5 verificados. Puntaje = (✔ ×1 + ◐ ×0,5) × 2.
**✔** verificado (se jugó o se midió en el .exe) · **◐** construido, sin verificar · **✘** falta.

## 🛡️ @tech-lead — 5,0 / 10
| # | Criterio | Estado | Evidencia o falta |
|---|---|:-:|---|
| T1 | RC1 sin cierres: G0 del parque (3×15 min) y 3 sesiones de la RC1 | ✘ | G0 sin correr; el log del 23-11 en la Casona salió limpio |
| T2 | 25 FPS o más en Baja, medido en Shipping: hospital, Casona y parque | ◐ | Hospital ~30 (0.3.84) y Casona 30–42 en `_dev`; falta Shipping y el parque |
| T3 | Calidad automática (A6) | ✔ | Medido en 0.3.84: hospital de 3 a ~30 FPS |
| T4 | Pipeline de release: ZIP medido, builds ordenadas y subida a itch.io probada | ◐ | ZIP de la v0.3.87: **1,21 GB en 165 s** (`Empaquetar_ZIP.ps1`); `Build/` de 85 a 12 GB; `NeverCook` no hace falta (la build ya pesa 1,46 GB); falta butler: no está instalado y requiere descargarlo y entrar a itch.io (lo hace el CTO) |
| T5 | Reglas técnicas: cero Event Tick y un interruptor en `DA_Tuning` por feature | ◐ | El jugador conserva un `ReceiveTick` heredado (llama `ActualizarBateria`); sin interruptor: agachado, luna, exposición, textos por nivel, botella y barra de ruido |

## 🕹️ @gameplay-engineer — 4,0 / 10
| # | Criterio | Estado | Evidencia o falta |
|---|---|:-:|---|
| G1 | Linterna que delata y ahuyenta (A3), probada | ◐ | Construida 0.3.64; caso N2 sin ejecutar |
| G2 | Pistas con H/△ y a los 180 s (A4), probadas | ◐ | 0.3.74; caso N3 sin ejecutar |
| G3 | Botella que distrae y barra de ruido (A5), probadas y con interruptor | ◐ | 0.3.65; sin `Botella_Distrae` ni `HUD_Ruido` en `DA_Tuning` |
| G4 | Sombra justa: pausa tras el golpe y anti-atasco (A7), probada | ◐ | 0.3.65; caso N5 sin ejecutar |
| G5 | Director de miedo: empuje cada 40–65 s y autoguardado (B6) | ✘ | Sin evidencia de avance |

## 🏚️ @world-builder — 5,0 / 10
| # | Criterio | Estado | Evidencia o falta |
|---|---|:-:|---|
| W1 | Atmósfera del HTML portada (post, LUT, niebla, luna) | ◐ | Casona validada por el CTO; hospital (A1) sin humo |
| W2 | Casona recorrible de punta a punta: 3 pisos, escondites y salida del ático | ◐ | Escalera revisada por geometría; recorrido completo sin jugar |
| W3 | Páginas 1–15 en `DA_NovelaData`, leídas en el juego | ◐ | Cargadas (0.3.63); lectura sin probar |
| W4 | Canon de Isidora decidido | ✔ | Aprobado el 18-11 (leucemia más miocardiopatía, 03:33 del 13/03/1998) |
| W5 | Recorrido Hospital → Casona → Parque cerrado | ✘ | Pasillo que no sale; parque sin G0 |

## 🎨 @ux-toolsmith — 3,0 / 10
| # | Criterio | Estado | Evidencia o falta |
|---|---|:-:|---|
| U1 | Avisos según el dispositivo (A2), probados | ◐ | 0.3.62; caso N1 sin ejecutar |
| U2 | Contador de páginas y pantalla final (A8), probados | ◐ | 0.3.63–0.3.83; casos N6 sin ejecutar |
| U3 | Subtítulos con descripción de sustos (B3) | ✘ | Sin evidencia |
| U4 | Mezcla de audio y música ligada a la tensión (B3, B4) | ✘ | Sin evidencia |
| U5 | Menú de opciones completo (volumen, calidad, mando) probado | ◐ | Existe; caso H1 sin ejecutar |

## 🧪 @qa-publisher — 3,0 / 10
| # | Criterio | Estado | Evidencia o falta |
|---|---|:-:|---|
| Q1 | Humo A9 ejecutado con la RC1 (H, N, C, G0) | ✘ | Checklists listas: `rc1_2026-11-24.md` y `g0_parque_2026-11-23.md` |
| Q2 | Kit de testers listo para enviar | ◐ | `kit_testers_2026-11-26.md`; faltan `[LINK]`, `[CLAVE]` y teclas |
| Q3 | Página de itch.io v0.5.0 al día | ✘ | `itchio_pagina_demo.md` es de la v0.3.40 |
| Q4 | Builds y espacio ordenados (pocas builds guardadas, ZIP medido) | ✔ | 40 builds borradas con OK del CTO (73 GB); quedan 6: v0.3.108_dev, v0.3.107_dev, v0.3.87, v0.3.86, v0.3.85 y v0.3.1; ZIP de 1,21 GB |
| Q5 | Log limpio: 0 cierres de GPU y 10 avisos de script o menos por sesión | ◐ | Casona v0.3.108: 6 avisos, 0 cierres; falta hospital y parque |

## Lo que más sube el puntaje (orden recomendado)
1. **Una sola prueba de humo jugada (Q1)** convierte hasta 12 criterios ◐ en ✔: es lo que más mueve los 5 departamentos. 60 minutos del CTO.
2. **G0 del parque (T1, W5)** y los FPS en Shipping (T2).
3. **Pipeline de release (T4):** falta instalar butler y probar una subida (la hace el CTO). ZIP y builds ya están.
4. **Interruptores en `DA_Tuning` (T5, G3):** una sola sesión de editor.
5. **Sprint B (G5, U3, U4):** subtítulos, mezcla, música y autoguardado. Va después de la RC1.
6. **Pasillo que no sale (W5)** y página de itch.io (Q3).

## Fuera de alcance del 10-12 (no se persigue en esta ronda)
Quitar el `ReceiveTick` heredado del jugador (refactor grande del BP de 11 MB): se deja como ◐ en T5 hasta después de la demo.

## Plan de @tech-lead para sus criterios (23-11)
**T1 y T2 (los juega el CTO):** G0 con `Probar.bat` → P → 1 (usa la `_dev`, deja logs). Para medir FPS reales en Shipping: `Probar.bat` → **9** abre la última build Shipping con el panel de FPS (no guarda logs). Medir hospital, Casona y avenida del parque en calidad Baja; meta 25 FPS o más.
**T4 (falta una cosa del CTO):** el ZIP de la RC1 pesa 1,21 GB, así que itch.io exige butler. Pasos en `auditoria/guia_publicar_itchio.md` (comando ya con el ZIP de la v0.3.108). Recomendado: probar primero la subida a un canal privado (`windows-rc1`).
**T5 (una sesión de editor, después del humo):** 12 Blueprints ya leen `DA_Tuning`: `BP_ThirdPersonCharacter`, `BP_DirectorMiedo`, `BP_GestorNivel`, `BP_IsidoraAparicion`, `BP_LatidoMiedo`, `BP_PayasoAcecho`, `BP_PostHTML`, `BP_SustoNoria`, `BP_SustoTeatro`, `BP_AI_Sombra`, `BP_PayasoPersecucion` y `AC_Ruido`. Interruptores que faltan, con su nombre propuesto:
| Feature | Interruptor | Dónde se lee | Riesgo |
|---|---|---|---|
| Agachado con escala (0.3.104) | `agachado_Activo`, `agachado_Escala` (0,72) | `BP_ThirdPersonCharacter`: `Agachar`, `Levantar` y el salto | Bajo |
| La botella distrae (A5) | `botella_Distrae` | Botella y `BP_AI_Sombra` | Medio |
| Barra de ruido en el HUD (A5) | `hud_Ruido` | `AplicarHUD` del jugador | Bajo |
| Luna, estrellas y luz de luna de la Casona | `casona_Cielo` | `BP_GestorNivel` (ocultar actores `LUNA_*` y `ESTRELLAS_Domo`) | Bajo |
| Cristales de ventana de la Casona | `casona_VentanasLuz` | `BP_GestorNivel` (ocultar `VentLuz_*`) | Bajo |
**Event Tick:** el `ReceiveTick` heredado del jugador (llama `ActualizarBateria`) se mueve a un temporizador de 0,1 s **después de la demo**; no antes, por riesgo.
