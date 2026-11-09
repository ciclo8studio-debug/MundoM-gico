# Diseño · Sprint 3: el tramo de la demo completa (Carrusel y Casa de Alfredo)

**Autores:** `@world-builder` y `@gameplay-engineer` · **Estado:** diseño listo para implementar después de la demo corta del 13 de noviembre.
**Base:** reutilizar sistemas que ya existen (páginas, llave, fusible, cintas, generador, `MostrarSusto`, `BP_RuidoAzar`, nieblas, `CAM_*`) y no inventar mecánicas nuevas.

## Recorrido de la demo completa (20–30 min)
```
Sala ─► Pasillo (susto + glitch) ─► Entrada (descompresión 25 s)
   ─► Avenida en penumbra ─► CARRUSEL (evento + página clave) ─► Bodega (llave)
   ─► Ruta este ─► CASA DE ALFREDO (fusible + cinta 4 + clímax)
   ─► Atajo E→entrada ─► Persecución del payaso ─► "Continuará…"
```
> Cambio respecto a la demo corta: se abre `BLQ_Bodega`, porque la llave de la casa está en la bodega. El final pasa del castillo a la persecución (`RevisarFinDemo` se desactiva).

## 1. Carrusel (-2000, -7000): de "texto" a evento
**Objetivo de diseño:** el primer pico de tensión del parque, corto y memorable (unos 2 min).

| Paso | Qué pasa | Implementación |
|---|---|---|
| 1 | Al acercarse a menos de 12 m, la música de feria (`SND_FeriaCarrusel`) baja de golpe a silencio | Trigger y volumen 0 en 1,5 s |
| 2 | El carrusel (`ROT_Carrusel_Gira`) **empieza a girar solo**, lento, con las luces (`ROT_Carrusel_Luces`) parpadeando | `BP_Giratorio.Encender(true)` más `FAR_` local |
| 3 | En un caballo aparece una **página clave** (la de Rafaela) | `REC_` página con `bHidden` hasta el paso 2 |
| 4 | Al tomarla, el carrusel se detiene en seco y **la muñeca aparece del otro lado** (ya patrulla la avenida) | `ENE_Muneca` con teleport a un `BP_Marcador` y `MostrarSusto` |
| 5 | Salida: el jugador debe apagar la linterna y agacharse para pasar (sigilo) | Usa la visión actual: linterna más agachado |

**Assets:** solo existentes (`BP_Giratorio`, `SND_FeriaCarrusel`, `FAR_`, páginas). **Riesgo:** bajo.

## 2. Casa de Alfredo (9200, -15100): exploración y clímax
**Objetivo:** el punto más alto de tensión y de narrativa de la demo (unos 8 min).

| Zona | Contenido | Sistemas que se reutilizan |
|---|---|---|
| Porche | Puerta con llave (llave de la bodega) y nota de Alfredo | `UsarPuerta`, llave `PAB_`/`BOD_CajaLlave` |
| Sala | Muebles de UnrealBucket, fonógrafo ("LAS OCHO"), niebla `NIE_Casa`, luz `FAR_Casa` | `UsarFonografo`, `BP_FogArea` |
| Estudio | Reloj (8 4 2) que abre la caja con el **fusible** | `UsarReloj`, `AbrirCaja`/`ComprobarCaja` |
| Dormitorio | **Cinta 4** (VHS) y página de Isidora | `ReproducirCintas`, `CuadroCinta` |
| **Clímax** | Al tomar la cinta se corta la luz de la casa (`ForzarOscuridad`), se oye la risa del payaso cerca y se abre el **atajo este** | `ForzarOscuridad`, `SFX_T_*` con `ATT_Medio`, `BP_RuidoAzar` |

## 3. Atajo y persecución final (resuelve W4: el retroceso vacío)
- **Atajo este:** se abre un portón lateral en la reja perimetral E, cerca de la casa, hacia un sendero que vuelve a la entrada. Corta unos 140 m a unos 60 m.
- **Persecución:** el payaso (`ENE_Payaso`) se activa en modo persecución. El jugador corre por el sendero oscuro con la linterna y, al cruzar el portón de entrada, empieza la cinemática "Continuará…" (`CAM_CinePayaso` reubicada).
- **Seguridad:** si el payaso atrapa al jugador, este reaparece en el atajo, no en la sala, para no castigar la demo.

## 4. Lenguaje de luz y audio en el tramo
| Zona | Luz | Audio |
|---|---|---|
| Entrada (segura) | Cálida y estable (`LUZG_Entrada`) | Lluvia suave, sin amenaza |
| Avenida (penumbra) | Focos con huecos (ya aplicado) | Viento, metal al azar |
| Carrusel (evento) | Luces del carrusel parpadeando | Silencio repentino, luego un chirrido |
| Casa (oscuridad) | Solo `FAR_Casa` y la linterna | Interior seco (reverb corta) y crujidos |
| Persecución | Oscuridad total y linterna | Respiración, pasos del payaso, música de persecución (`MUS_Persecucion`) |

## 5. Orden de implementación sugerido
1. Marcadores `BP_Marcador` (sustituyen las coordenadas fijas) en el carrusel, la casa y el atajo.
2. Evento del Carrusel (pasos 1–4).
3. Casa: verificar que la cadena llave → fonógrafo → reloj → fusible → cinta funciona en el `.exe`.
4. Clímax, atajo y persecución.
5. Desactivar `RevisarFinDemo` y mover el final a la persecución.
6. Build v0.4.0 y prueba del CTO con la checklist (secciones D y E más una nueva sección "Casa").
