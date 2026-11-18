# Plan consolidado del estudio: evaluación del 18-11-2026

Orquestado por los 5 super-agentes, cada uno con su equipo de agentes de Claude Code Game Studios (ver `ESTUDIO_AGENTES.md`). Informes completos en `auditoria/estudio_2026-11-18/`.

## Notas
| Departamento | Nota | Diagnóstico en una línea |
|---|---|---|
| 🛡️ @tech-lead | 6/10 | Los sistemas base ya están. Faltan el contenido del Nivel 1, la calidad automática y un pipeline de release ordenado. |
| 🕹️ @gameplay-engineer | 6/10 | Las piezas existen (Sombra, ruido, escondites), pero falta el **bucle** del HTML: la linterna delata y ahuyenta, y hay pistas. |
| 🏚️ @world-builder | Arte 5 · narrativa UE 5 (HTML 8) | La atmósfera del HTML sale barata (post, niebla, luna) y aún no se porta. Faltan el 2.º piso, la morgue y las páginas 1–15. |
| 🎨 @ux-toolsmith | 6,5/10 | Lo visible ya está portado. Falta lo invisible: subtítulos, música dinámica y avisos según el mando. |
| 🧪 @qa-publisher | 5/10 en preparación | La build pesa 1,44 GB, lo que está bien. Las versiones 0.3.43–0.3.57 casi no se han jugado y los documentos de itch.io están viejos. |

**Hallazgos transversales:**
- Mucho de lo hecho no se ha probado en el `.exe`.
- El HTML se siente mejor por cómo **guía y tensiona**, no por los gráficos.
- El tiempo hasta el 10-12 obliga a recortar.

## Sprint A: paridad con el HTML (19-11 → build de testers el 25-11)
Una o dos features por build, cada una con su interruptor en `DA_Tuning`.

| # | Mejora | Origen | Esfuerzo | Interruptor |
|---|---|---|---|---|
| A1 | **Post-proceso del HTML + LUT verdosa + niebla de interior** en `L_Hospital` | world-builder | S | `Activar_PostHTML` |
| A2 | **Avisos según el dispositivo** (□ ✕ △ con mando, E/F con teclado) | ux-toolsmith | S | `UI_AvisosPorDispositivo` |
| A3 | **Dilema de la linterna:** encendida te delata (te ven a 16 m) y apuntada 2,5 s ahuyenta a la Sombra; batería pasa a `DA_Tuning` | gameplay-engineer | M | `Sombra_RepeleLinterna`, `Bateria_GastoSeg` |
| A4 | **Objetivos y pistas:** [H]/△ y pista automática a los 180 s | tech-lead + gameplay | M | `Pistas_Activas`, `Pista_AutoSeg` |
| A5 | **La botella distrae a la Sombra** + barra de ruido en el HUD | gameplay-engineer | S | `Botella_Distrae`, `HUD_Ruido` |
| A6 | **Sombras de linterna por calidad + calidad automática** (en vez de por mapa) | tech-lead | S | `Calidad_Auto`, `Linterna_SombraMinCalidad` |
| A7 | **Sombra más justa:** pausa después del golpe y anti-atasco | gameplay-engineer | S | `Sombra_PausaGolpe` |
| A8 | **Contador de páginas, pantalla final de la demo y "tic" de UI** | ux-toolsmith | S | `UI_ContadorPaginas`, `UI_FinalDemo` |
| A9 | **QA:** prueba de humo por zona en cada build, más opciones en `Probar.bat` (escondites, borrar partida) y `checklist_v0.3.60` | qa-publisher | S | — |
| A10 | **Deuda técnica:** `MoverLuz` del menú pasa del Tick a un timer de 0,033 s (regla de no usar Event Tick) | tech-lead | S | — |

## Sprint B: contenido del Nivel 1 (26-11 → freeze del 1-12)
- **B1** Páginas 1–15 en `DA_NovelaData` con los textos del HTML (registro de Urgencias, diario, "Paciente I.").
- **B2** Sótano y morgue con circuito: negatoscopio (código 0333), patrulla de la Sombra con 3 escondites, `LlaveMorgue` y salida. *Recorte:* el 2.º piso queda fuera.
- **B3** Subtítulos con descripciones de los sustos (activos por defecto) y mezcla de audio (`SC_Musica`/`SC_Voz`/`SC_UI`, pausa, límite de sonidos simultáneos).
- **B4** Música y drone de tensión ligados al Director de miedo.
- **B5** Linterna con inercia, FOV y aliento al correr.
- **B6** Empuje del Director cada 40–65 s y autoguardado en llaves, páginas y zonas.
- **B7** Teaser de la Casona de 60–90 s: TV → VHS "CINTA 1 · 23:41", vestíbulo en ámbar, apagón y "Continuará". Sin enemigo.
- **B8** Maniquí "luz roja, luz verde" en la morgue (hijo de `BP_PayasoAcecho`).

## Release (1-12 → 10-12)
- **RC1** el 25-11 (testers), **RC2** el 3-12 y **gold** el 8-12.
- Criterio de salida: 0 cierres, al menos 25 fps en Baja y el recorrido completo.
- Página de itch.io v0.5.0 bilingüe, con 1ª persona, ZIP de unos 1,3 GB y subida con butler.
- Kit de testers: fechas nuevas, ruta de logs y cierres, y al menos 2 testers con iGPU.
- `DirectoriesToNeverCook`, ZIP medido en cada build y solo 3 builds guardadas.

## Decisiones que necesita el CTO
1. **¿La linterna ahuyenta a la Sombra?** Recomendación: sí, de forma híbrida (A3).
2. **Canon de Isidora:** el HTML dice "corazón débil" y `referencia_pelicula.md` dice "leucemia terminal". Hay que elegir antes de escribir las páginas.
3. **Recorte del Nivel 1:** sin 2.º piso, negatoscopio en el sótano y la Casona solo como teaser. Recomendación: sí.
4. **¿El parque sale de la build pública del 10-12?** El @tech-lead recomienda sacarlo por riesgo de GPU y peso.
5. **Fechas:** feature freeze de gameplay el 25-11 y de contenido el 1-12.
6. **Borrar builds viejas:** unos 16 GB en `Build/` y unos 33 GB en total, contando ZIP. Se haría con lista y OK del CTO.
7. **¿Página de itch.io bilingüe** con el juego en español?

## Cómo se ejecuta
- Claude implementa en Unreal, con un editor y una build por día, en el orden A1 → A10 salvo que el CTO decida otra cosa.
- Cada build pasa la prueba de humo de @qa-publisher.
- El estado de cada ítem se actualiza en `BOARD.md`.
