# @qa-publisher · Reingeniería de QA y publicación (18-11-2026)

Equipo: qa-lead (proceso y gates), qa-tester (casos), release-manager (pipeline y tienda), localization-lead (idioma). El diagnóstico está en `auditoria/estudio_2026-11-18/qa-publisher.md` y aquí no se repite.

## 1. Visión
- Toda build que sale de `Construir_Version.ps1` pasa un **humo de 25 min** antes de llamarse "probada". Desde hoy no se acumulan versiones sin build.
- Un solo registro de bugs (`auditoria/bugs.md`) con 4 severidades. El gate RC/gold se decide con números y no por sensación.
- La checklist se escribe **por build** a partir del CHANGELOG: cada entrada nueva genera al menos 1 caso.
- Los testers reciben un kit cerrado (link, instrucciones, formulario y ruta de logs) y responden en 48 h.
- La página de itch.io se escribe sobre la RC, no sobre el plan. Es bilingüe y el juego queda en español.

## 2. Proceso de calidad
**Definición de "listo" por feature** (los 6 puntos, o no se marca ✅ en el BOARD):
1. Interruptor en `DA_Tuning`, entrada en el CHANGELOG y versión subida.
2. Sin Event Tick nuevo y sin coordenadas escritas a mano.
3. Build Shipping con 0 errores; los avisos nuevos se anotan.
4. Caso agregado a la checklist de la versión, con el formato "pasos → esperado".
5. Probada en el `.exe` con `Probar.bat`, en calidad Baja y con el panel `-dev`: ≥25 fps en su zona.
6. Probada también con el interruptor **apagado**: el juego vuelve al comportamiento anterior sin errores.

**Antes de cada prueba:** cerrar Unreal y el navegador. Borrar la partida con **Retroceso** en el menú, o borrando `%LOCALAPPDATA%\MundoMagico\Saved\SaveGames\`. Calidad **Baja**.

**Humo por zona** (unos 25 min, `Probar.bat`, panel `-dev`; anotar los fps mínimos de cada zona):
| # | Opción `Probar.bat` | Pasos exactos | Qué mirar | Pasa si | Min |
|---|---|---|---|---|:-:|
| H1 | 8 (juego normal) | Portada → mover el mouse → Configuración → mover General/Efectos/Música → Volver → Iniciar juego | Luz sin borde, sliders independientes | Efectos 0 % no silencia la música; las opciones se guardan al reabrir | 3 |
| H2 | 1 hospital | Caminar a la linterna → recogerla → F encender → L2/clic derecho enfocar → recoger 1 página | Look verdoso, niebla, haz de 23°, sin linterna flotante | Ningún sonido de puerta al recoger; `SFX_Pagina`; E no abre ningún candado | 4 |
| H3 | 2 pasillo | Avanzar hasta el susto → balizas y alarma → hacer ruido corriendo → entrar a `ESC_HideSpot_1` (E) → Shift → salir → repetir en `_2` | La Sombra investiga el ruido, persigue y pierde | Escondido no te ve; al salir no te atascas; el latido sube con la Sombra cerca | 5 |
| H4 | 8 → salir del hospital | Recorrido hasta el glitch → TV → VHS "PLAY 03:17" → reja | Transición completa, fps al llegar | **Sin GPUCrash**; ≥25 fps en la reja; el payaso no aparece antes de 60 s | 4 |
| H5 | 3 parque | Caminar 10 min por la avenida; darle la espalda al payaso y después mirarlo | Avanza si no lo miras, se congela si lo miras, golpea a 2,3 m | 10 min sin cierre; Isidora y el payaso no aparecen a la vez (Director) | 10 |
| H6 | 4 teatro / 5 noria | Entrar → susto → dejarse morir cerca → respawn | Susto una sola vez | El respawn no repite el susto | 3 |
| H7 | 6 bodega / 7 autos | Recoger `BP_LlaveFlag` → `BP_DoorLocked` → salir al menú → **Continuar** | Mensaje "Encontraste…", la puerta abre | La llave sigue tras Continuar; Iniciar juego la borra | 3 |

**Regresión por build:** humo H1–H7 + los casos de la checklist de esa versión + 3 casos al azar de checklists anteriores (rotan). El resultado se anota en `auditoria/humo/v<ver>.md` (fecha, fps por zona, ✅/❌/⚠️, bugs). Si un caso H falla, la build **no avanza** y el siguiente trabajo es el arreglo.

**Severidades:**
| Sev | Definición | Ejemplo | Regla |
|---|---|---|---|
| S1 🔴 | Cierre, congelamiento, partida corrupta, bloqueo sin salida | GPUCrash al llegar al parque | Bloquea la RC; se arregla el mismo día |
| S2 🟠 | Rompe una mecánica o impide avanzar, pero hay salida | Atasco en el clóset; llave que no persiste | Bloquea gold |
| S3 🟡 | Se nota y molesta sin romper | Linterna flotante; sonido falso de puerta | Se arregla si cabe; va a "conocidos" |
| S4 ⚪ | Cosmético menor o texto | Tilde mal en un mensaje | Backlog |

**Plantilla de bug** (una por bloque en `auditoria/bugs.md`):
```
### BUG-### · <título corto> · S<1-4> · v0.3.xx · <Abierto|Arreglado vX|Verificado vY|No se arregla>
Zona: <Probar.bat opción> · Repro: <x/5 intentos> · Fuente: <CTO|tester N|humo>
Pasos: 1) … 2) … 3) …   Esperado: …   Obtenido: …
Evidencia: captura / `Saved\Logs\MundoMagico.log` / carpeta de Crashes · fps del panel -dev
Interruptor relacionado en DA_Tuning: …
```

**Criterio de salida:**
- **RC** (RC1 y RC2): 0 S1 abiertos; humo H1–H7 completo ✅; ≥25 fps en Baja en todas las zonas; recorrido completo Iniciar → final de la demo sin `-ir`; ZIP ≤1,5 GB.
- **Gold:** lo de la RC + 0 S2 abiertos + 2 recorridos completos seguidos del CTO sin cierre + ≥1 tester con iGPU sin cierre + página y capturas hechas sobre esa misma build. Después del gold solo entran arreglos S1 (hotfix con versión +0.0.1).

## 3. Checklist v0.3.61 (regresión 0.3.41–0.3.61, unos 50 min)
Prepara así: borrar la partida, calidad Baja, mando y audífonos, `Probar.bat`. Marca ✅/❌/⚠️ más una línea.
**A. Menú y opciones**
- [ ] 0.3.58/59/61: la luz de la portada sigue al mouse **sin borde rectangular** ni en las esquinas; el menú no se traba (ya no usa el Tick).
- [ ] 0.3.51: Efectos al 0 % → la música sigue sonando; Música al 0 % → los pasos siguen sonando.
- [ ] 0.3.52: la fila General baja todo junto; sirve con teclado, mando y mouse; "Volver al menú" está en la fila 8; se guarda al reabrir el `.exe`.
- [ ] 0.3.41: con `-dev`, el panel muestra fps, X/Y, batería, ruido y tensión (0.3.45).
**B. Hospital (opciones 1 y 2)**
- [ ] 0.3.43: arranca en **1ª persona** con los ojos a buena altura; el botón de vista cambia a 3ª y vuelve.
- [ ] 0.3.53: Isidora **no** parte con la linterna; aparece al recogerla. E cerca de nada no abre ningún candado.
- [ ] 0.3.55/60: el haz nace abajo a la derecha y sigue la mirada; **no hay linterna flotante**; el círculo tiene anillo y borde suave (IES); L2 lo cierra a 14° y al soltar vuelve a 23°.
- [ ] 0.3.60: Cuadrado/E junto a una puerta cerrada → solo el mensaje "Está cerrada con llave", **sin sonido**. Las puertas suenan solo al abrirse o cerrarse.
- [ ] 0.3.60: recoger una página → roce de papel + nota grave.
- [ ] 0.3.61: tono verdoso, viñeta, grano y niebla oscura. Apagar `PostHTML_Activo` → vuelve el look anterior.
- [ ] 0.3.45: correr sobre baldosa sube el "ruido" del panel; agachado lo baja.
- [ ] 0.3.47: la Sombra patrulla con su linterna; te ve en el haz; si corres va al ruido; si la pierdes 4 s, vuelve a investigar; chillido la primera vez.
- [ ] 0.3.57: `ESC_HideSpot_1` y `_2`: entrar con E mirando a la puerta; la Sombra pasa y no te ve; Shift contiene la respiración; al salir quedas donde estabas, sin atasco (5 veces cada uno).
- [ ] 0.3.50: el latido se acelera al acercarse la Sombra.
**C. Transición y parque**
- [ ] 0.3.42: glitch → TV que se apaga (1,6 s) → "▶ PLAY 03:17" (2,4 s) → reja; no caes durante la TV.
- [ ] 0.3.58: **sin GPUCrash** al llegar; 10 min en el parque sin cierre; la linterna no proyecta sombras en el parque.
- [ ] 0.3.49: el payaso avanza si no lo miras, se congela si lo miras, desaparece si lo miras fijo y golpea a <2,3 m (chillido, vibración, 1 de daño); se va a los 20 s.
- [ ] 0.3.44: entre sustos pasan ≥40 s; no salen dos del mismo tipo seguidos; tras el teatro/noria el payaso espera.
- [ ] 0.3.46/48: llave → puerta con la misma etiqueta abre; sin llave, "Está cerrada. Algo tiene que abrirla."; salir → Continuar → la llave sigue; Iniciar juego → la llave se borra.
- [ ] 0.3.42: Continuar con una partida que ya salió del hospital → aparece directo en el parque.
- [ ] Partida vieja: Continuar con un save de la 0.3.5x no cierra el juego (si cierra → S1; invalidar los saves por versión).
**D. Build:** ZIP ≤1,5 GB, sin `.pdb`; texturas a 2048 sin borrones visibles de cerca (0.3.56).

## 4. Kit de testers (26-11, RC1 v0.3.6x)
**Mensaje para mandar (lo envía el CTO):**
> Gracias por probar *Mundo Mágico: Turno Nocturno* (demo privada, no la compartas). 1) Descarga el ZIP desde tu enlace de itch.io y descomprímelo en una carpeta **sin tildes** (por ejemplo `C:\Juegos\MundoMagico`). 2) Abre `MundoMagico.exe`. Si sale "Windows protegió su PC": **Más información → Ejecutar de todas formas**. 3) Juega con audífonos, de noche si puedes; el mando es opcional. Con tarjeta integrada usa calidad **Baja**. 4) Si se cierra, manda la carpeta más nueva de `%LOCALAPPDATA%\MundoMagico\Saved\Crashes\` y el archivo `%LOCALAPPDATA%\MundoMagico\Saved\Logs\MundoMagico.log` (pega la ruta en la barra del Explorador). 5) Responde el formulario en **48 h**.

**Formulario v2** (Google Form; ✱ = obligatoria):
1. ✱ Tu PC: tarjeta de video o modelo de notebook, RAM, Windows 10/11.
2. ✱ ¿Llegaste al final de la demo? Sí/No. Si no, ¿dónde paraste y por qué?
3. ✱ ¿Se cerró, se congeló o se puso lento? ¿Dónde? Adjunta Crashes/Logs.
4. ✱ Minutos jugados en total.
5. **1ª persona:** ¿la cámara te mareó o se sintió flotante? (1–5) ¿Cambiaste a 3ª persona?
6. **Linterna:** ¿se ve bien el haz y la batería te generó tensión? (1–5) ¿Viste la linterna flotando?
7. **Sombra:** ¿entendiste que te ve con su linterna y te oye al correr? ¿Te pareció justa? (1–5)
8. **Escondites:** ¿los encontraste? ¿Te quedaste atascado al entrar o salir? ¿Te sentiste a salvo?
9. **Look:** ¿el tono verdoso y la niebla del hospital dan miedo o molestan para ver? (1–5)
10. ¿Qué momento te dio más miedo? ¿Hubo un momento en que no supiste qué hacer?
11. ¿Algo se veía o sonaba raro (objetos flotando, sonidos que no corresponden, texto cortado)?
12. Del 1 al 5, ¿te gustó? ¿Jugarías la versión completa? ¿Qué cambiarías primero?

**Metas:** ≥80 % termina · **0 cierres** (un cierre = S1) · nota ≥3,5 · ≥2 testers con iGPU. Triage de las respuestas a `bugs.md` dentro de las 24 h siguientes.

## 5. Release e itch.io
**Versionado:** `0.3.x` = builds internas · **RC1 = v0.4.0-rc1** (25-11) · **RC2 = v0.4.x-rc2** (3-12) · **gold = v0.5.0** (8-12) · hotfix = 0.5.1. `ProjectVersion` en `DefaultGame.ini` = `--userversion` de butler = nombre del ZIP.

**Calendario:**
| Fecha | Hito | Gate |
|---|---|---|
| 19–24-11 | Sprint A, una build al día, humo en cada una | Humo ✅ |
| **25-11** | **RC1** + página en Restricted + claves para testers | Criterio RC |
| 26–28-11 | Testers (48 h) → triage | 0 S1 |
| 1-12 | Feature freeze de contenido | — |
| **3-12** | **RC2** (Sprint B + arreglos de testers) | Criterio RC |
| 4–7-12 | Solo arreglos; capturas y página sobre la RC2 | 0 S2 |
| **8-12** | **Gold v0.5.0** subida en Draft/Restricted | Criterio gold |
| **10-12** | Visibility → **Public** (lo hace el CTO) | — |

**butler** (el CTO inicia sesión; Claude no sube nada): `herramientas\Empaquetar_Demo.ps1 -Version 0.5.0` → `butler login` → `butler push "Publicar\MundoMagico_Demo_v0.5.0_Win64.zip" USUARIO/mundo-magico-turno-nocturno:windows-demo --userversion 0.5.0` → `butler status USUARIO/mundo-magico-turno-nocturno` (esperar a "processed") → en la página, marcar el canal como Windows. Mismo canal para RC1/RC2/gold: los testers reciben la actualización en la app de itch.

**Página v0.5.0** (EN arriba, ES abajo; reemplaza `itchio_pagina_demo.md` sobre la build real):
- **Title:** Mundo Mágico: Turno Nocturno — Demo · **Tags:** `horror`, `survival-horror`, `first-person`, `atmospheric`, `psychological-horror`, `stealth`, `singleplayer`, `spanish`, `unreal-engine`, `demo` · **Cover** 630×500 · 5 capturas de la gold.
- **Short (EN):** First-person horror: an empty hospital, a Shadow that hunts by its own flashlight, and nowhere to hide but the dark.
- **EN:** *Isidora wakes up in Ward 4 with no memory of how she got there. Something walks the ER corridor with a flashlight of its own — if its beam finds you, it's over. Run and it hears you. Hide and hold your breath. At the end of the hall, a broken TV drags her somewhere worse: Mundo Mágico, the amusement park that closed years ago.* **In this demo:** Ward 4 and the ER corridor · the Shadow (sight cone + hearing) · hiding spots · flashlight with limited battery · footsteps that sound different on each floor · ~20–30 min (confirm in RC2). **Language:** Spanish text only — English subtitles coming in a later update. **Requirements:** Windows 10/11 64-bit, 8 GB RAM, Intel Iris Plus on Low (30 fps) / GTX 1660 recommended, DirectX 11, ~1.3 GB download, ~3.5 GB on disk. **Windows SmartScreen:** the game is not code-signed (indie studio). Click *More info → Run anyway*.
- **ES:** *Isidora despierta en el Pabellón 4 sin saber cómo llegó. Algo recorre el pasillo de Urgencias con su propia linterna: si su luz te encuentra, se acabó. Si corres, te oye. Escóndete y contén la respiración. Al final del pasillo, una TV rota la arrastra a algo peor: Mundo Mágico, el parque que cerró hace años.* **En esta demo:** Pabellón 4 y Urgencias · la Sombra (te ve en su haz y te oye) · escondites · linterna con batería · pasos que suenan distinto según el suelo · unos 20–30 min. **Idioma:** español. **Requisitos:** los mismos de arriba. **Aviso de Windows:** *Más información → Ejecutar de todas formas*.
- Si el CTO saca el parque de la build pública (decisión 4 del plan), se borra la última frase de las dos descripciones y se usa "Continuará" en su lugar.

**Checklist de publicación (8–10-12):** ZIP medido · prueba de una descarga limpia desde itch en otro PC/usuario · los links de la página funcionan · teaser en YouTube no listado · `Build/` con solo 3 builds (con OK del CTO) · nota de cambios en el devlog.

## 6. Localización
- **Para el 10-12: el juego solo en español** y la página bilingüe con la línea "Spanish text only — English subtitles coming". Traducir la UI ahora es tamaño L y arriesga regresiones en la etapa de freeze.
- **Qué hacer ya (S, sin riesgo):** todo texto **nuevo** de Sprint A/B (pistas A4, contador y pantalla final A8, páginas B1, subtítulos B3) se escribe desde el inicio en String Tables y no a mano en el BP: `ST_UI`, `ST_Mensajes`, `ST_Paginas`, `ST_Subtitulos`, con claves como `UI.Menu.Iniciar` o `MSG.Puerta.Cerrada`.
- **Qué extraer después de la demo** (orden de valor/esfuerzo): 1) menú, configuración y pausa (`WBP_Menu`, filas 0–8); 2) mensajes de interacción ("Está cerrada con llave", "Encontraste: <nombre>" → `FText::Format`); 3) pantallas de carga y transición ("PASILLO DE URGENCIAS", "MUNDO MÁGICO" queda sin traducir, "PLAY 03:17"); 4) páginas de la novela en `DA_NovelaData` (pasan a claves de `ST_Paginas`); 5) subtítulos de sustos. Los nombres propios (Isidora, Mundo Mágico, Pabellón 4) no se traducen.
- **QA de idioma por build:** ñ y tildes en los mensajes nuevos, en la pantalla final y en las llaves (fuente de respaldo `DroidSansFallback`); el texto en inglés ocupa ~30 % más, así que los cuadros de UI nuevos deben aguantar ese ancho.
- La localización EN entra después del 10-12 con el localization dashboard de Unreal (export PO → traducción → import) y la culture `en` en el menú.

## 7. Decisiones CTO
1. **Gate firme:** ¿apruebas que una build con un caso H fallido o un S1 abierto **no avanza** (ni a testers ni a itch), aunque eso mueva una fecha?
2. **Versionado:** ¿RC1 = v0.4.0-rc1 y gold = v0.5.0, con butler en el canal `windows-demo` para todos los hitos?
3. **Texto nuevo directo en String Tables** desde el Sprint A (recomendado: sí, coste casi cero y evita la extracción después).
