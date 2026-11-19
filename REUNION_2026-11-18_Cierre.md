# Ciclo Studio Game: reunión corta de cierre (18-11-2026)

Fechas: G0 parque **22-11** · rc1 **25-11** · rc2 3-12 · gold 8-12 · pública **10-12**. Plan B: `Demo_SinParque` (hospital + Acto 3).

## Estado por super-agente
| Super-agente | Hecho desde la reunión de avance | Pendiente / bloqueo |
|---|---|---|
| @tech-lead | Causa del cierre = H2 (partida de otro mapa), corregida en v0.3.69. v0.3.70 fuerza perfil liviano en el parque (`Parque_PerfilBajo`). v0.3.72 arregla la marca de transición (clase abstracta). | **FPS del parque en Shipping desconocido** (D11 midió 3 FPS antes de v0.3.70; las Development no sirven). Perfil A vetado. G0 en riesgo medio. |
| @gameplay-engineer | v0.3.74 pistas (A4). v0.3.75 Acto 3: `BP_PayasoPersecucion` (5,2 m/s, 1 daño cada 1,5 s, sin Tick). | Acto 3 sin probar; falta expediente y 3 finales completos. Escalada por cintas, Willy y León siguen detrás de G0. |
| @world-builder | 62 Recuerdos y E1–E15 como datos (en plazo). Cofres del parque sin teclado (v0.3.71). | Nadie toca `L_Parque` hasta pasar G0. Sala 4 y expediente pendientes. |
| @ux-toolsmith | Interacción por mirada: recoge lo que se mira (v0.3.73). | Cabina VIDEO y elección E/Q como widgets + subtítulos (v0.3.72 del plan viejo) no empezados. |
| @qa-publisher | Builds v0.3.73 y v0.3.75 listas. | **Ninguna probada por el CTO.** Sin medición Shipping del parque. Checklist de rc1 sin armar. |

## Riesgos
| Riesgo | Prob. | Impacto | Mitigación |
|---|---|---|---|
| El parque sigue lento en Shipping (3 FPS previo a v0.3.70) | Media | Alto | Prueba D11 en v0.3.75; si falla, Plan B ampliado el 20-11 |
| Acto 3 sin probar rompe el flujo hospital -> parque -> hospital | Media | Alto | Prueba n.º 3 del CTO; corregir en v0.3.76 |
| Se acumulan cambios sin validar (v0.3.69–75 = 7 versiones) | Alta | Medio | Una sola ronda de prueba hoy; congelar features nuevas hasta tener resultado |
| Cuello de botella: 1 build/día y 1 tester (CTO) | Alta | Medio | Agrupar arreglos; priorizar lo crítico |
| Fecha rc1 (25-11) sin Perfil A ni más hardware | Baja | Alto | Gate firme; sale lo que pase G0 |

## Plan de los próximos 3 días
| Día | Qué | Responsable |
|---|---|---|
| **19-11** | El CTO prueba v0.3.75 (ver abajo). Mientras, solo datos/docs: Sala 4, expediente y finales como datos; borrador de checklist rc1. Sin tocar `L_Parque`. | @world-builder, @qa-publisher |
| **20-11** | Con resultados: arreglos en v0.3.76 (una build). **20:00 el producer decide** G0: parque sí / Plan B ampliado. | @tech-lead, CTO |
| **21-11** | Si G0 va: cabina VIDEO + E/Q + subtítulos (widgets). Si no: Acto 3 y finales en Plan B. Congelar features el 22-11 salvo bugs. | @ux-toolsmith, @gameplay-engineer |

## Qué debe probar el CTO (máx. 5)
1. `Probar.bat` -> **5** (borrar partida) -> **D1** (parque directo): ¿arranca y no se cae 5 min?
2. En el parque, **D11**: anotar FPS y ms de GPU (build Shipping, no Development).
3. Jugar hospital -> glitch -> parque -> cabina -> elegir "sala" (v0.3.75): ¿vuelve al hospital y el payaso persigue?
4. Mirar un objeto entre varios y recoger con E/□; pedir la pista (v0.3.74): ¿recoge lo mirado y la pista corresponde?
5. Guardar en el parque, salir y **Continuar**: ¿aparece en el parque sin ir al menú?

Si falla 1 o 2 el 20-11: Plan B ampliado. Para fallos, **7** (ZIP de logs).
