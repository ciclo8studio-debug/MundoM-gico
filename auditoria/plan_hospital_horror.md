# Plan: el hospital con `Horror_Hospital` (prioridad del CTO, 11-11)

**Cuándo:** se empieza después del kit de afinación y entra en la build del 15-11. Mientras tanto no se toca el mapa, porque las features siguen congeladas.

## Qué trae el pack (ya instalado en `Content/Horror_Hospital`)
- **Estructura:**
  - 26 muros (`SM_Wall_01..26_v`);
  - 9 zócalos (`SM_Wallplinth_01..09_v`);
  - techos (`SM_Ceiling_Square_01_p`, `SM_One_Side_Ceiling_03_v`, `SM_Ceiling_Frame/Detail`);
  - claraboyas;
  - ventanas en 3 modelos.
- **Puertas:** 5 modelos (`SM_Door_01..05`), con variantes `_p` y `_v`.
- **Mobiliario:**
  - camas (`SM_Bed_01`), con cobertores y almohadas;
  - veladores, escritorios, gabinetes, estantes (`SM_Rack_*`) y refrigeradores;
  - biombos y cortinas médicas.
- **Detalle:** cables, vendas, trapos, libros y decals.
- **Mapas de muestra:** `Maps/DemoMap` y `Maps/LV_Horror_Hospital_Overview`, con el kit armado para tomar como referencia de escala y combinación.

## Lo que se cambia (sin mover nada de lógica)
| Zona | Hoy | Con el pack |
|---|---|---|
| **Sala del Pabellón 4** (x ±810, y 2490–3510) | Cajas `P4_Muro_*`, camas armadas con cubos, `P4_Mostrador_*` | Muros `SM_Wall_*` con zócalo, 4 camas `SM_Bed_01` con cobertor, biombos entre camas, mostrador con `SM_Desk_*` y `SM_Cabinet_*`, techo modular con claraboya rota |
| **Pasillo de salida HX** (x ±210, y 540–2490) | Cajas `HX_*` | Muros y zócalos del pack, puertas `SM_Door_*` entreabiertas hacia las salas laterales, refrigeradores y estantes caídos |
| **Pasillo de Urgencias** (x ±200, y −600 a −3500) | Muros con textura estirada, puerta gris, techo plano | Muros del pack con la mitad baja de **azulejo** (referencia de arte), techo con marcos y **tubos fluorescentes emisivos** cada 6 m (`LampsPack`), y **suelo mojado** (roughness 0,15) |

## Reglas para no romper nada
- Las mallas viejas **no se borran**: se ocultan y se mueven a la carpeta `Hospital/Viejo`. Así se puede volver atrás en un minuto.
- No se mueven las puertas que tienen lógica (`BP_Creepy_Door`, `P4_PuertaCruje`), ni `BP_SustoPasillo`, los respawn, los `REC_*`, los clósets ni los triggers.
- Se mantiene la colisión: los muros del pack bloquean y la utilería pequeña va sin colisión.
- **Rendimiento:**
  - no se agregan luces nuevas con sombra;
  - los fluorescentes son material emisivo con una sola luz débil cada 2 tubos;
  - se miden los fps en el `.exe` antes y después.

## Orden de trabajo, unas 8–10 h
1. Abrir `LV_Horror_Hospital_Overview` en modo liviano para medir el tamaño de los módulos (2 m, 4 m) y tomar nota.
2. Sala del Pabellón 4: muros, zócalos, techo y luego mobiliario.
3. Pasillo HX.
4. Pasillo de Urgencias, con azulejo, fluorescentes y suelo mojado.
5. Capturas del antes y el después para el CTO, y build v0.3.42.
