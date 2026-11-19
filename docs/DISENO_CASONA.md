# Casona — diseño del nivel 2 (`L_Casona`)

Base: `ARQUITECTURA_3_NIVELES.md` §4 (terror íntimo, 20–25 min, ámbar sucio, páginas 16–40). Construcción del 19-11: **blockout vestido** con los packs; sin lógica de puzles todavía.

## Planta (cm; puerta principal en (0,0), la casa crece hacia +Y)
| Piso | Zona | Rango | Contenido |
|---|---|---|---|
| Planta baja | **Vestíbulo** | X -400..400, Y 0..900 | Consola, espejo, reloj de pie, araña, entrada. Linterna sobre la consola (para pruebas) |
| | **Comedor** | X -1000..-400, Y 0..900 | Mesa y 6 sillas, piano, aparador |
| | **Biblioteca** | X 400..1000, Y 0..900 | 4 libreros, sofá y sillón victorianos, **4 marcos de álbum** (puzle de fotos por año) |
| | **Cocina** | X -1000..-150, Y 900..1600 | Mesa de trabajo, cocina, armarios |
| | **Estudio** | X 150..1000, Y 900..1600 | Escritorio, tablero de evidencias (DetectiveOffice) |
| | **Escalera central** | X -150..150, Y 900..1440 | 19 peldaños hacia el pasillo alto |
| Planta alta | **Pasillo** (alfombra, silencioso) | X -1000..1000, Y 1400..1600 | Marcos, alfombra; escalera al ático en el extremo este |
| | **Dormitorio principal** | X -1000..-250, Y 500..1400 | Cama gótica, ropero (escondite), cómoda |
| | **Baño principal** | X -250..250, Y 900..1400 | Lavabo y bañera de bloqueo |
| | **Dormitorio B** | X 250..1000, Y 500..1400 | Cama, ropero (escondite), mesita |
| Ático | **Ático** | X -1000..1000, Y 500..1600, Z 680..1000 | Cajas (AnimatedWoodenCrate), muebles tapados, salida al parque (VHS) |

## Superficies y luz
- Muros: `MI_wallpaper` sobre friso `MI_wooden_panel` (0–120 cm). Piso: `MI_wood_1` (planta baja, cruje) y `MI_wood_2`; pasillo alto `MI_carpet_1` (silencioso). Techo `MI_PlasterCream`.
- Luz **ámbar tungsteno** (2700 K), puntuales sin sombras para la iGPU (≈ 24 luces). Tras el apagón (`ApagonCasona`, pendiente): solo azul de luna por las ventanas.
- Decals de `Horror_Hospital` (grunge, grietas, filtraciones) en las paredes.

## Marcadores y hooks (sin lógica aún)
`Entrada_Hospital` (vestíbulo), `Salida_Parque` (ático), `Album_1..4` (biblioteca), `Escondite_*` (roperos), `Evento_Apagon` (pasillo bajo), `Sombra_Anciana_Inicio` (cocina).

## Pendiente (después de este blockout)
Puzle de álbumes, apagón, Sombra anciana (oído ×2), 25 páginas, conexión Hospital → Casona (glitch + TV) y Casona → Parque (VHS), horneado de luz.

## Packs usados
`UnrealBucket/ArchViz` (muebles góticos), `DetectiveOffice` (marcos, libros, alfombras, escritorio), `SICKA_MouldingSet` (molduras, columnas), `Horror_Hospital` (decals), `LampsPack`, `AnimatedWoodenCrate`, `SM_Notes`.
