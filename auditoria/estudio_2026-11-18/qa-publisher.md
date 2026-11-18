# @qa-publisher · Calidad y preparación para publicar (18-11-2026)

Equipo: `qa-lead`, `qa-tester`, `release-manager`, `localization-lead`. Fuentes: CHANGELOG 0.3.40–0.3.60, BOARD, `Probar.bat`, `Construir_Version.ps1`, `Empaquetar_Demo.ps1`, `auditoria/`, `Build/`, `Build_log_v0.3.60.txt` y el juego HTML (`MundoMagico_Web/index.html`, solo el código y no el base64). No se ejecutó nada.

## 1. Resumen
- La cadena de build está sana. La v0.3.60 compila con 0 errores y 28 avisos inofensivos (PoseAsset obsoletos de `FlashLight_System`). La build ocupa 1,44 GB, lo que deja el ZIP dentro de la meta de 1,5 GB.
- Faltan pruebas. Desde la 0.3.43 entraron en bloque la 1ª persona, la Sombra, los escondites, el payaso que avanza, los flags y el arreglo de GPU del parque. Ninguna checklist cubre todo eso de punta a punta, y no hay una prueba de humo por zona escrita para cada build.
- Lo que se publica está desactualizado. La página de itch.io dice "tercera persona", un ZIP de 3,3 GB y fechas del 13 y 16-11. El formulario de testers apunta al 13–15-11. No hay textos en inglés. Frente al HTML faltan el mapa, la vida con botiquín, la sensibilidad y el archivo de notas.

**Nota de preparación para publicar: 5/10.** El pipeline está bien, pero faltan la validación y los materiales de la tienda.

## 2. Hallazgos por agente
**qa-lead (estrategia y gates)**
- No hay criterio de salida formal por build. Propongo este: 0 cierres, ≥25 fps en Baja con Iris Plus (panel `-dev`) y recorrido completo Sala → Parque.
- La deuda de validación es alta. De la 0.3.43 a la 0.3.57 son 15 versiones "sin build" que se probaron juntas recién en las 0.3.57 a 0.3.60. El CHANGELOG solo registra un playtest real del parque: el GPUCrash de la 0.3.57, corregido en la 0.3.58. No consta una confirmación posterior en el parque.
- `checklist_v0.3.40.md` y `checklist_humo.md` describen la cámara al hombro y el payaso que solo mira. Ya no sirven como regresión.

**qa-tester (casos y checklists)**
- `Probar.bat` no tiene opciones para los escondites (`ESC_HideSpot_1/2`, se llega por la zona 2), para la Casona ni para Continuar y persistencia (los flags de la 0.3.48).
- Hay casos de regresión sin cubrir: la linterna flotante (0.3.60), el sonido falso de puerta (0.3.60), la tecla E que abría el candado en el hospital (0.3.53), el atasco en el clóset (0.3.57) y "Efectos" que silenciaba la música (0.3.51).
- No hay una instrucción para limpiar la partida guardada antes de cada prueba, que es imprescindible para probar Iniciar juego, los flags y el glitch.

**release-manager (pipeline y tienda)**
- Build: Shipping, cook iterativo de unos 6 min, `-pak` con IoStore (`.ucas` de 1,1 GB). `Empaquetar_Demo.ps1` excluye el `.pdb`. El ZIP de unos 1,2–1,3 GB pasa de 1 GB, así que la subida va con **butler** (ya documentado).
- El disco tiene 28 GB en `Build/` (11 builds) y 9 GB en `Publicar/` (3 ZIP viejos de 3+ GB, de las v0.3.1, v0.3.35 y v0.3.40). En un PC de 8 GB el espacio también es un riesgo, porque el cook copia a `C:\MM_Build`.
- `itchio_pagina_demo.md` está obsoleto en 6 puntos: la cámara (1ª persona), el tamaño (≈1,3 GB y unos 3,5 GB en disco), las fechas, la versión v0.5.0, la etiqueta `third-person` → `first-person` y el contenido (Sombra, escondites, Casona). Faltan además la portada de 630×500 y las capturas nuevas.

**localization-lead (idioma)**
- El juego está solo en español (el HTML usa `lang="es"`) y los textos están escritos directo en los BP. StringTables sigue en el backlog. Traducir la UI antes del 10-12 es tamaño L y un riesgo de regresión.
- itch.io es mayormente angloparlante. Una página bilingüe (EN arriba, ES abajo) y la etiqueta "Spanish only (English subtitles coming)" amplían el alcance sin tocar el juego.
- La fuente de respaldo `DroidSansFallback` ya cubre los glifos. Hay que revisar la ñ y las tildes en la pantalla final y en los mensajes nuevos de llaves y escondites.

## 3. Riesgos de release
| Riesgo | Prob. | Impacto | Mitigación |
|---|:-:|:-:|---|
| GPUCrash en el parque con Iris Plus (sombras, 1.400 actores) | Media | Alto | Humo de la zona 3 en cada build durante 10 min; `Linterna_SombrasParque` apagado; anotar los fps en la checklist |
| Regresiones por entrar muchas features juntas sin probar | Alta | Alto | Feature freeze el 1-12; solo arreglos después; humo por zona en cada build |
| Un tester no puede jugar (SmartScreen, falta de VC++, iGPU) | Media | Alto | Aviso de SmartScreen en la página, `-prereqs` ya está incluido, requisitos honestos |
| Sin registro de cierres de los testers | Alta | Medio | Pedir que adjunten `%LOCALAPPDATA%\MundoMagico\Saved\Crashes` y `Logs` en el formulario |
| La página promete algo distinto de lo que hay | Alta | Medio | Reescribir la página sobre la RC y no sobre el plan |
| El disco se llena durante el cook | Media | Medio | Borrar builds y ZIP viejos (con OK del CTO) y dejar solo el respaldo v0.3.42 y las 2 últimas |
| La partida de una versión vieja rompe Continuar | Media | Medio | Probar Continuar con una partida de la 0.3.5x; si falla, invalidar la partida por versión |
| No alcanza el calendario con 1 build al día (22 días) | Media | Alto | RC1 el 25-11, RC2 el 3-12 y gold el 8-12, con 2 días de margen |

## 4. Top 6 mejoras priorizadas
| # | Mejora | Impacto | Esfuerzo | Responsable |
|:-:|---|:-:|:-:|---|
| 1 | **Humo por zona en cada build** (tabla abajo) y `checklist_v0.3.60.md` con las regresiones de la 0.3.51 a la 0.3.60. Agregar a `Probar.bat`: escondites, Casona y "borrar partida" | 5 | S | qa-tester / qa-lead |
| 2 | **Calendario de RC y feature freeze** el 1-12; criterio de salida: 0 cierres, ≥25 fps en Baja y recorrido completo | 5 | S | release-manager |
| 3 | **Paridad con el HTML (P1):** mapa del parque (M), HUD de vida con botiquín (1–4/↑) y slider de sensibilidad. El HTML ya los ofrece y el jugador los espera. Cada uno con su interruptor en `DA_Tuning` | 4 | M | ue-umg-specialist + gameplay (QA valida) |
| 4 | **Página de itch.io v0.5.0 bilingüe:** 1ª persona, ZIP de unos 1,3 GB, Sombra y escondites, 5 capturas nuevas, portada de 630×500, subida con butler y canal `windows-demo` | 4 | S | release-manager + localization-lead |
| 5 | **Kit de testers del 26-11:** formulario con fechas nuevas y preguntas sobre la 1ª persona, los escondites y el mapa; ruta de los logs y crashes; 2 de los testers con iGPU | 4 | S | qa-lead |
| 6 | **Limpieza de disco y de cook** (con OK del CTO): unos 33 GB de builds y ZIP viejos; `DirectoriesToNeverCook` para los packs que no se usan | 3 | S | release-manager |

**Plan de humo por zona (unos 25 min por build, con `Probar.bat`, sin partida guardada, calidad Baja):**
| Zona (opción) | Qué verificar | Pasa si |
|---|---|---|
| Menú (8) | Portada con la luz que sigue al mouse, Iniciar, Configuración (General, Efectos y Música por separado) y Volver | Cada slider afecta solo lo suyo y se guarda |
| Hospital (1) | Recoger la linterna; sin linterna flotante en 1ª persona; E no abre el candado; página con `SFX_Pagina` | Sin sonido de puerta al interactuar |
| Pasillo (2) | Susto, 3 balizas y alarma; la Sombra ve dentro del cono y oye al correr; escondite 1 y 2 (entrar, salir y Shift) | Salir del clóset sin atascarse; la Sombra no ve al jugador escondido |
| Glitch → Parque (8, juego normal) | Transición TV → VHS y llegada a la reja | Sin GPUCrash; ≥25 fps; el payaso no aparece antes de 60 s |
| Parque (3) | Payaso: avanza si no lo miras, se congela si lo miras, golpea a 2,3 m; Isidora; latido | 10 min sin cierre |
| Teatro y Noria (4, 5) | Susto una sola vez; respawn cercano sin que el susto se repita | No se repite al morir |
| Bodega y Autos (6, 7) | Llave y puerta (`BP_LlaveFlag` y `BP_DoorLocked`) | La puerta abre solo con la llave |
| Persistencia (8) | Recoger la llave → salir → Continuar | La llave sigue; Iniciar juego la borra |

## 5. Decisiones para el CTO
1. **Idioma:** ¿página bilingüe ES/EN con el juego solo en español (recomendado, S) o traducir la UI antes del 10-12 (L, con riesgo)?
2. **Alcance de paridad con el HTML para el 10-12:** ¿entran solo el mapa, la vida con botiquín y la sensibilidad (recomendado)? Las 4 cintas con su final, los autos conducibles, el archivo de notas y la calidad automática quedarían para después de la demo.
3. **Freeze y limpieza:** ¿feature freeze el 1-12, y autorizas borrar los builds y ZIP viejos (unos 33 GB), dejando el respaldo v0.3.42 y las 2 últimas?
