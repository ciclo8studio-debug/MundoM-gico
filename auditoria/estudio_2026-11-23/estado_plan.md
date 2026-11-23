# Estado del plan al 23-11-2026 (reunión corta, 1 agente)

Fuentes: CONTEXTO_ESTUDIO, PLAN_ESTUDIO_2026-11-18, CHANGELOG 0.3.107 a 0.3.84, humo v0.3.84 y v0.3.92_casona, BOARD 1-40.
Límite: no leí el CHANGELOG bajo 0.3.84; las versiones de A1-A8 salen del BOARD (línea 14). CONTEXTO_ESTUDIO dice "v0.3.81": está desfasado (última es 0.3.107).

## 1. Sprint A (A1-A10)
| # | Estado | Versión | Nota |
|---|---|---|---|
| A1 post HTML + LUT + niebla | Hecho sin probar | 0.3.61 | LUT verdosa figura como hecha en CONTEXTO |
| A2 avisos por dispositivo | Hecho sin probar | 0.3.62 | caso N1 de humo sin ejecutar |
| A3 linterna delata/ahuyenta | Hecho sin probar | 0.3.64 | N2 sin ejecutar |
| A4 pistas H/△ + 180 s | Hecho sin probar | 0.3.74 | N3 sin ejecutar |
| A5 botella + barra de ruido | Hecho sin probar | 0.3.65 | N4 |
| A6 calidad auto + sombras linterna | Hecho; parcialmente validado | 0.3.62, ampliada 0.3.82, TSR 0.3.84 | 0.3.84 sí se midió: hospital 3 a ~30 FPS en Baja |
| A7 Sombra justa | Hecho sin probar | 0.3.65 | N5 |
| A8 contador, FIN, tic | Hecho sin probar | 0.3.63, 0.3.66, 0.3.83 | interruptores creados el 20-11 |
| A9 QA humo por zona | Pendiente (preparado) | 0.3.84 | checklists v0.3.84 y v0.3.92_casona escritas, **ninguna ejecutada** (casillas vacías) |
| A10 MoverLuz sin Tick | Hecho sin probar | 0.3.61 | |

Lectura: 9 de 10 construidos, 0 de 10 con humo formal. El CTO sí probó builds `_dev` (0.3.84 a 0.3.106) y dio feedback informal, pero solo de la Casona.

## 2. Sprint B y Release
| Ítem | Estado | Comentario |
|---|---|---|
| B1 páginas 1-15 | Cubierto (CONTEXTO: 15 páginas en `DA_NovelaData`) | falta probar lectura |
| B2 sótano/morgue + negatoscopio | Reemplazado | Ala de Internación nueva (0.3.77) + recorrido circular; no verifiqué si el negatoscopio 0333 existe |
| B3 subtítulos + mezcla | Sin evidencia en lo leído | decisión aprobada el 18-11 |
| B4 música ligada al Director | Sin evidencia | |
| B5 linterna con inercia/FOV | Sin evidencia | |
| B6 empuje Director + autoguardado | Sin evidencia | flags guardados sí existen |
| B7 teaser Casona | Superado | la Casona es nivel real (0.3.79-0.3.80); el trabajo de 0.3.85-0.3.107 es visual |
| B8 maniquí | Parcial | payaso acosador en hospital (Acto 3, 0.3.75) |
| Release | Pendiente | kit testers, itch.io v0.5.0, butler, ZIP medido, `DirectoriesToNeverCook`: sin evidencia de avance |

Desvío: del 21 al 23-11 se hicieron 0.3.85-0.3.107 (23 versiones) casi todas visuales de la Casona (luna, estrellas, castillo, ventanas), sin build nueva desde `v0.3.95_dev`/pruebas `_dev`. Sprint B (B3-B6) no avanzó.

## 3. Falta para RC1 de testers (25-11)
| P | Ítem | Esf. | Riesgo | ¿CTO? |
|---|---|---|---|---|
| 1 | Congelar Casona visual (0.3.107 es el último ajuste de ventanas) y armar UNA build RC1 Shipping + `_dev` | S | Alto si se sigue tocando | Sí: aprobar el congelamiento |
| 2 | Ejecutar humo A9 (H1-H7, N1-N8, C1-C6) con la RC1; incluye verificar agachado, Isidora que se movía sola y ventanas | M | Alto: 0 de 10 ítems validados | No (lo ejecuta el CTO; ~1 h) |
| 3 | Bug "pasillo que no sale" (abre "Mundo Mágico" pero queda en el mismo mapa) reproducir y arreglar | M | Alto: bloquea el flujo hospital a Casona | No |
| 4 | Medir FPS en parque y Casona en Shipping, Baja (criterio: ≥25 fps) | S | Alto: Casona 21 a 28 FPS en `_dev`; parque desconocido | No |
| 5 | Decisión G0 del parque (22-11 vencido, sin registro) | S | Alto: define el alcance | **Sí** |
| 6 | Kit de testers: instrucciones, fechas, ruta de logs/cierres, 2 testers con iGPU | S | Medio | No |
| 7 | Verificar cierres GPU en Casona tras apagar niebla volumétrica (0.3.101) y luz nueva 6 lux | S | Medio: ya hubo 1 cuelgue | No |
| 8 | Borrar builds viejas (~16 GB) para dejar espacio | S | Bajo | **Sí** (confirmar borrado) |

## 4. Riesgos de fecha hasta el 10-12
| Riesgo | Prob. | Impacto | Si no alcanza |
|---|---|---|---|
| R1 PC a 8 GB/iGPU: 1 build/día y 1 sesión de editor; cada ciclo prueba-arreglo cuesta 1 día | Alta | Alto | Recortar a ≤1 feature por build |
| R2 Parque: G0 sin decisión, FPS Shipping desconocido, cierres GPU previos | Alta | Alto | Plan B `Demo_SinParque` (hospital + Acto 3) |
| R3 Cero humo formal de 9 features: bugs acumulados aparecerán juntos en testers | Alta | Alto | RC1 solo con A1-A8 que pasen humo; el resto apagado por `DA_Tuning` |
| R4 Deriva de alcance en la Casona (visual) mientras B3-B6 y release no avanzan | Alta | Medio | Congelar visuales Casona hasta después del 10-12 |
| R5 Cierres de GPU de la iGPU (ya 1 por niebla volumétrica; TDR) | Media | Alto | Perfiles de calidad Baja por defecto; nada de Perfil A sin OK |

Orden de recorte si falta tiempo: (1) B5 inercia/FOV, (2) B4 música dinámica, (3) B8 maniquí, (4) B6 empuje Director (dejar solo autoguardado), (5) B3 reducir a subtítulos sin mezcla, (6) sacar el parque (Plan B). No recortar: humo, FPS ≥25 y 0 cierres.

## 5. Deuda abierta (CHANGELOG)
| Deuda | Origen | Acción sugerida |
|---|---|---|
| Agachado visual por truco de escala (0.72 fijo en el BP, sin animación real) | 0.3.104 | Interruptor en `DA_Tuning` y confirmar en build; animación real después de la demo |
| Avisos script: `RecogerObjeto`, `AntesDePagina` y `LimpiarMensaje` | 0.3.90-0.3.95; log de 0.3.92_dev: 122 avisos | Confirmar en `_dev` que 0.3.95 los bajó; arreglar el resto con la misma guarda de lista vacía (S) |
| Interruptores `DA_Tuning` pendientes | luna/estrellas/exposición Casona (0.3.102), agachado (0.3.104), textos de objetivo y minimapa por nivel (0.3.88-0.3.89), `MipFilter` (0.3.97, solo perfil) | Regla del estudio: toda feature lleva interruptor; agruparlos en una sola sesión de editor |
| Ventanas con luz solo en fachada sur (4 + 2 cristales); este, oeste, norte sin luz | 0.3.107 | Recortar: dejar solo sur salvo que se vea desde el recorrido |
| Pasillo que no sale (hipótesis: flag `PersecucionFinal` guardado o `Demo_SinParque`) | 0.3.84 y humo 0.3.92 | Prioridad 3 del punto 3 |
| G0 del parque sin decisión; FPS Shipping del parque desconocido | BOARD | Decisión del CTO (prioridad 5) |
| Menores: neón de fachada eliminado (0.3.96), escalón de 22 cm al salir, `Munecas` sin ubicar | CHANGELOG | Dejar |
