# Reunión CTO + @tech-lead (11-11, noche): mandato AAA y separación hospital/parque

**Mandato del CTO:** @tech-lead, primero de la cadena de super-agentes, queda **autorizado a proponer mejoras** para llevar la demo a nivel AAA y a asignar el trabajo a los otros 4 agentes.

**Cambio pedido por el CTO:**
- El pasillo **no debe conectar con el parque**: son lugares distintos.
- Al parque **solo se llega con el glitch al salir, seguido de la TV**.

## ⚠️ Conflicto que el CTO debe resolver primero
A las 20:40 apareció en el repo una línea nueva que no está commiteada:
- `MUNDOMAGICO2.md`;
- `herramientas/Abrir_MundoMagico2.bat/.ps1`;
- el proyecto `Documents\Unreal Projects\MundoMagico2`, con los packs ya copiados.

`MUNDOMAGICO2.md` dice que el proyecto viejo "no se publica" y que "no hay fechas". Eso contradice el [plan de mejora rápida](PLAN_MEJORA_RAPIDA.md) y los planes para itch.io del 13 y el 16-11.

Esta propuesta asume que sigue `MUNDO_MÁGICO` con itch.io. La separación en dos mapas sirve para cualquiera de las dos líneas.

## 1. Separar el hospital y el parque: **dos mapas** (opción recomendada)

| | **(a) Dos mapas + OpenLevel** ✅ | (b) Hospital a 50 km | (c) Level Streaming |
|---|---|---|---|
| Coordenadas fijas | No se rompen: cada mapa es una copia del actual sin la otra mitad | Se rompen 4–5 funciones del jugador | No se rompen |
| Guardado y Continuar | `SalidaHecha` decide qué mapa se abre | Sin cambios | Carga y descarga por flag |
| Memoria de la iGPU | **Menor**: cada mapa carga solo lo suyo | Igual que hoy | Mejor, pero con tirones |
| Horas | **7–9** | 10–14 | 10–12 |

**Pasos:**
1. Duplicar `L_Pabellon4` en dos mapas: `L_Hospital` se queda con todo lo que está en y > −3600 y `L_Parque` con el resto.
2. En el glitch: guardar, pasar a la TV y ejecutar `OpenLevel(L_Parque)`.
3. Al entrar a `L_Parque`: cargar la partida, dejar al jugador en la reja y continuar con la TV.
4. Arreglar Continuar y el menú.
5. Cocinar los dos mapas.
6. Ajustar `Probar.bat` para que abra el mapa correcto.

## 2. Transición glitch → TV
1. El glitch se intensifica en la reja. El audio baja y la alarma se ahoga.
2. **La TV se apaga**: la imagen se encoge a una línea blanca y luego a un punto, con un "tuk" y un pitido agudo.
3. **Negro y silencio** durante 1,5 s, solo con la respiración. Mientras tanto se carga el parque.
4. **Cinta VHS**: barras de tracking y "▶ PLAY 03:17". El sonido es un VHS corto y filtrado, con el clac del mecanismo.
5. **El parque se sintoniza**: la aberración cromática se estabiliza sobre la reja y suena una caja de música a lo lejos.
6. **Descompresión** de 20–30 s, sin amenazas.

## 3. Mejoras AAA priorizadas
| # | Mejora | Impacto | Horas | Agente |
|---|---|---|---|---|
| 1 | Separar en 2 mapas + transición TV | Alto | 8 | gameplay + ux |
| 2 | Hospital vestido con `Horror_Hospital` | Alto | 9 | world-builder |
| 3 | Look propio por mapa: LUT verdosa en el hospital, sodio en el parque, niebla | Alto | 3 | world-builder |
| 4 | Luz en 3 niveles y un hito de luz por atracción | Alto | 4 | world-builder |
| 5 | `BP_DirectorMiedo` | Alto | 6 | gameplay |
| 6 | Amenaza real del payaso: avanza cuando no lo miras, golpe, escondites | Alto | 10 | gameplay |
| 7 | Reemplazar las primitivas por packs | Medio | 6 | world-builder |
| 8 | Mezcla de audio por mapa y subtítulos de sustos | Medio | 3 | ux |
| 9 | Cocinar solo los 2 mapas → ZIP de 1,5 GB o menos | Medio | 2 | qa |
| 10 | Checklist de humo por mapa, con fps | Medio | 1 por build | qa |

## 4. Orden por días (una build al día)
- **12-11:** el CTO juega la v0.3.40 como línea base. Build v0.3.41, que trae el kit.
- **13-11:** itch.io restringido con la v0.3.41. Se hace la separación con la TV y la build v0.3.42.
- **14-11:** el CTO prueba la v0.3.42. Si pasa, queda congelada como candidata para el 16-11. Empiezan el hospital AAA y el look.
- **15-11:** build interna del hospital.
- **16-11:** se publica la v0.3.42. El hospital AAA sale como actualización el 18-11.
- **17-11:** Director de miedo y Perfil A.
- **18 al 20-11:** luz y primitivas del parque.
- **Desde el 21-11:** amenaza real y pulido.

Si la v0.3.42 falla, el 16-11 se publica la v0.3.41.

## 5. Decisiones del CTO
1. ✅ **Qué línea manda. Lo decidió el CTO el 11-11: `MUNDO_MÁGICO`, con itch.io el 13-11 restringido y el 16-11 público.** `MundoMagico2` queda en pausa y no se borra. La pregunta original fue: `MUNDO_MÁGICO`, con itch.io el 13 y el 16-11, o `MundoMagico2`, que empieza de cero sin fechas.
2. ✅ **Dos mapas con transición TV: aprobado por el CTO el 11-11.** Se hacen el 13-11 en la build v0.3.42.
3. ¿El 16-11 se publica con la separación (v0.3.42) y el hospital AAA llega como actualización el 18-11? ¿O se atrasa la publicación al 18-11 para que salga todo junto?
