# Protocolo de eficiencia (Ciclo Studio Game)

Datos reales de hoy: cada agente de una reunión gastó **94k–254k tokens**; una reunión de 5 ≈ **600k**. Casi todo se iba en releer los mismos documentos.

## 1. Cómo escribir tus instrucciones (CTO → Claude)
Plantilla corta, en un solo mensaje:
```
OBJETIVO: qué quiero ver funcionando
DÓNDE:    zona, mapa o versión
SÍNTOMA / META: qué pasa hoy / cómo debería ser
LÍMITE:   qué no tocar, "sin build" o "build"
ENTREGA:  build · documento · panel · solo respuesta
```
- **Junta todos los comentarios del playtest en un mensaje, numerados.** Cada mensaje suelto cuesta una vuelta completa.
- Di **"sin build"** o **"con build"** y **"decide tú"** cuando no quieras preguntas.
- Reuniones: **"reunión corta"** = 1 agente que cubre los 5 roles (~100k). **"Reunión completa"** = 5 agentes (~600k), solo en hitos (alcance, plan, decisiones grandes).
- Si el chat se alarga, pide el **Documento de Transferencia** y sigue en una sesión nueva (la sesión larga cuesta más en cada mensaje).
- Ejemplo malo: *"mejora la luz"*. Ejemplo bueno: *"Pasillo de L_Hospital: un tubo LED que falle al azar en cada habitación. Sin build."*

## 2. Reglas para agentes y super-agentes
1. **Todos leen primero `CONTEXTO_ESTUDIO.md` (1 página) y nada más de lo que su brief liste.** Máx. 6 archivos y con Grep antes de Read; nunca el CHANGELOG entero ni el HTML completo.
2. **Brief estándar (≤12 líneas):** Rol · Objetivo en 1 frase · Leer SOLO (rutas) · No hacer · Entregable (archivo, máx. N líneas, en tablas) · Criterio de "listo".
3. **Modelos:** Sonnet por defecto; Haiku para lectura/extracción mecánica (qa-tester); Opus solo para una decisión de arquitectura grande y a pedido. Ya quedó en `.claude/agents/*.md`.
4. **No repetir diagnósticos:** si ya hay informe, el agente solo escribe lo nuevo (estado, riesgo, próximo paso).
5. **El informe vive en un archivo** y el mensaje de vuelta tiene ≤5 líneas.
6. **Un super-agente por tarea**, salvo que la tarea cruce áreas. Los 31 agentes de estudio son "lentes" dentro de un brief, no 31 sesiones.
7. **Paralelo solo si no comparten archivos.** Nadie toca `L_Parque` hasta pasar G0, salvo el equipo de diagnóstico.

## 3. Reglas para Claude (implementación)
- **Una sesión de editor por build:** abrir Unreal cuesta ~4 min y varias llamadas, así que se agrupan todos los cambios de BP de la versión en una sola sesión y en un solo script.
- Lo que se puede cambiar **sin abrir el editor** (commandlets: `ajustar_tuning.py`, importadores) se hace sin abrirlo.
- Esperar con tareas en segundo plano; nada de sondeos. No releer archivos recién editados.
- Cada versión: CHANGELOG + `GUIA_TUNING.md` + commit local. **Una build por día** y solo cuando haya algo que probar.
- Respuestas finales cortas: qué cambió, qué probar, qué decisión falta.
