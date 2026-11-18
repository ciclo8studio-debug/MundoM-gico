# Reunión "Replicar el HTML completo" · @world-builder (zonas, recorrido, narrativa, páginas)

Solo lectura. Fuentes: `RECORRIDO_HTML.md`, CHANGELOG 0.3.40–0.3.67, `ARQUITECTURA_3_NIVELES.md`, `reingenieria_2026-11-18/world-builder.md`, `auditoria/world-builder.md` (dump de actores del parque, puede estar desfasado), memoria del parque.
Leyenda: ✅ existe · 🟡 existe distinto o a medias · ❌ falta · ❓ verificar en PIE/build.

## 0. Decisiones previas que propongo (para el CTO)
1. **Estructura = la del HTML, 3 actos en 2 mapas:** Acto 1 `L_Hospital` (sala + pasillo) → Acto 2 `L_Parque` → Acto 3 **otra vez `L_Hospital`** con flag `PersecucionFinal` (aparecer en el extremo del pasillo, payaso, Sala 4, expediente). Volver a cargar el hospital (432 actores) **libera el parque** de la iGPU: es más barato que construir la persecución en el parque.
2. **El sótano/morgue de la reingeniería (Z5–Z12, builds B-1..B-3) se congela** hasta después del 10-12. No cabe junto al parque, y el HTML no lo tiene. Sus papeles se reubican (ver §2).
3. **Casa del parque vs. Casona:** la casa del parque es la **"casa-camerino de Alfredo"** (donde vivía cuando era el Inspector Metete). La Casona del Nivel 2 es su casa real, de después. Así ambos documentos son ciertos.
4. Cierre de los finales: "Continuará · Capítulo 2: **La Casona**" (el HTML dice "Pabellón 4", que choca con los 3 niveles).

## 1. Matriz de paridad por zona
| Zona HTML | ¿En UE? | Qué falta (props · luces · sustos · textos) |
|---|---|---|
| Menú / portada | ✅ `WBP_Menu` | Nada crítico. Mensaje de inicio "Es tu turno de noche. Mantén la linterna encendida." ❓ |
| 1.1 Pabellón 4 (despertar) | ✅ sala + `CAM_CineDespertar`, linterna en mostrador | Carteles "PABELLÓN 4 · PSIQUIATRÍA" y "Urgencias" ❓; **expediente brillante en la mesa del fondo** (se usa en el Acto 3) ❌; 6 páginas en ID fijo ❌ |
| 1.2 Pasillo de Urgencias | ✅ pasillo + HX_Reanimación/Farmacia, `BP_SustoPasillo`, 7 `BP_TuboFalla`, LUT verdosa | Puertas rotuladas HAB. 4 / HAB. 5 / ARCHIVO / BODEGA ❌; **silueta tras el vidrio** ❌; apagón en cadena E→O 🟡 (hoy balizas + alarma + criatura; la paciente del pañuelo y la nota LA PACIENTE DEL PASILLO ❌); mostrador con REGISTRO 🟡 (existe como página 4 de las 15); puerta **SALA 4** atrancada ❌ (clave del Acto 3); resplandor naranja al fondo 🟡 |
| Transición hospital→parque | ✅ glitch + TV + VHS (0.3.42) | Cambiar "▶ PLAY 03:17" por **"▶ CINTA 1 · 12/03/1998 · 23:41"** (une con la página 7, portería). |
| 1.3 Entrada + interruptores | ✅ `ENT_*`, 4 `BP_Interruptor`, `BP_SustoEnergia`, `CAM_CineParque` | **Cortocircuito del 4.º** se quitó en 0.3.x → volver con interruptor `Energia_Cortocircuito`; silueta gigante **al volver la energía** (hoy sale al llegar) 🟡; estatua del payaso en (-8,41) ❓; mapa recogible, botellas ❓; notas ENERGÍA RESTABLECIDA / SILUETA BAJO EL LETRERO ❌ |
| Avenida, maqueta, kioscos | ✅ guirnaldas, `KIO_*`, faroles que mueren | Maqueta con trencito ❌ (opcional, solo prop); 9 páginas; muñeca **solo desde la 1.ª cinta** (hoy estática en la avenida) 🟡 |
| Noria / torre / jardín | ✅ (+ `BP_SustoNoria`, `APA_IsidoraNoria`, `NIE_Jardin`) | Cinta 1 del HTML va aquí (UE la tiene junto al tren fantasma) 🟡; 5 páginas |
| Carrusel + autos chocadores | ✅ (`EventoCarrusel`, muñeca; autos `AUT_*`; generador Pista B) | Autos que **empujan** (en HTML son ambientales) 🟡; el puzzle del generador no existe en el HTML → queda como opcional; 6 páginas |
| 1.5 Bodega | ✅ `BOD_*`, llave, lista, disfraces | Nota PELUCA DE ISIDORA ❓; goteo 2–6 s ❓; pilas al caer la llave ❓; 5 páginas |
| 1.5 Casa de Alfredo | ✅ (UnrealBucket, `NIE_Casa`) | Fusible: UE usa **caja fuerte** (HTML: cajón de la cómoda) → se mantiene la caja fuerte si ya funciona; **tele con 3 frases** ❌; CARTA SIN TERMINAR y DIARIO DE ALFREDO ❓; "Algo se mueve arriba" + glitch ❓; **cinta 2** (UE la llama cinta 4 → renombrar a 2, 23:58); el clímax UE "apagón + payaso + huida a la entrada" **sale** (el HTML no lo tiene; el pico va en el Acto 3) |
| 1.6 Teatro | ✅ 5 pedestales, `BP_SustoTeatro`, `APA_IsidoraTeatro` | Público que **gira la cabeza** tras resolver ❌; premio pilas + botiquín ❓; error = "El público se ríe bajito" + apagón corto (W9) ❌; 6 páginas |
| 1.6 Tren fantasma | 🟡 fachada y túnel | **Fantasma que salta** (repetible, 12 s) ❌; nota VAGÓN 4 ❌; sombra del túnel ❌; página 43 |
| Castillo / montaña rusa | ✅ props | Cinta 3 detrás del castillo (lado oeste) ❓; 4 páginas |
| 1.7 Cabina VIDEO | 🟡 cabina cerca de la entrada (neón verde) | **Video de 12 segmentos** ❌; se enciende con la 4.ª cinta ❌; elección E/Q ❌; Willy y León despiertan con la 4.ª ❌ (hoy están siempre) |
| 1.8 Persecución + expediente | ❌ (la ida fue un teleport de un solo sentido) | Todo el Acto 3 (§5) |
| 1.9 Finales y "volver al parque" | 🟡 `MostrarFinal` con "CAPÍTULO 1 · COMPLETADO" | 3 finales por umbral + estadísticas; "Volver al parque" carga `L_Parque` en (0,43) |

Escalada de amenazas (dueño: gameplay; yo solo la pido): muñeca desde la cinta 1 → payaso ángel desde la cinta 3 → Willy y León desde la cinta 4 → payaso en la persecución. Hoy todos están activos desde el inicio y juntos en la avenida (W3).

## 2. Páginas: conciliar las 62 del HTML con las 15 nuevas
**Problema:** el HTML tiene **62 "Recuerdos"** (páginas 4–65 de la novela IMAGINARIO: imagen `T_Pag_*` + pie propio, con flashback en las pares). Las 15 nuevas son **documentos clínicos** con texto propio. Hoy el HUD dice "x/15" y ocupan los ID 1–15.
**Propuesta de canon unificado (dos colecciones, una sola historia):**
- **RECUERDOS 1–62** = las 62 del HTML, con imagen y pie del HTML (textos autorizados). Cuentan para los finales (62 / ≥31 / <31), el video de la cabina y la galería (J). El HUD vuelve a "RECUERDOS x/62".
- **EXPEDIENTE E1–E15** = las 15 clínicas. Van al **archivo de notas (N)** junto a las 15 notas del HTML; la nota REGISTRO DE URGENCIAS del HTML **se funde con E4** (mismo texto base). Archivo total: 29 entradas.
- **E13, E14 y E15** (radiografía 03:33, libro de la morgue, certificado sin firmar) no se recogen: **son el contenido del expediente final** que Isidora lee en la Sala 4. Así el final revela la hora y la muerte, igual que el HTML, pero con nuestros textos.
- Reubicar los papeles pensados para el sótano: E8 mantención → escalera/ascensor muerto del pasillo; E9 solicitud de Rx y E10 archivo → puerta ARCHIVO (HX_Reanimación); E11 lavandería → puerta BODEGA del pasillo; E12 hoja de Alfredo → HX_Farmacia.
- `DA_NovelaData` pasa a 62 filas (`ID`, `Zona`, `Imagen`, `Pie`, `Flashback`, `Voz`, `Cinta`, `Segmento`) + `DA_Expediente` de 15. Cada `REC_Pagina` lleva su **ID fijo** (se acaba el "orden de recogida").

**Numeración por zona** (Recuerdo n = índice HTML + 1; se respeta el reparto del HTML):
| Zona UE | Recuerdos | N.º | Expediente |
|---|---|---|---|
| Pabellón 4 (`L_Hospital`) | 4, 5, 8, 9, 15, 16 | 6 | E1, E2, E3, E5 |
| Pasillo + HX (`L_Hospital`) | 18–27 | 10 | E4, E6, E7, E8, E9–E12 |
| Entrada y avenida sur | 3, 14, 53, 62 | 4 | — |
| Bodega | 1, 11, 17, 28, 29 | 5 | — |
| Casa de Alfredo | 2, 6, 10, 30, 31, 32 | 6 | — |
| Maqueta, kioscos, avenida | 7, 34, 36, 52, 54–58 | 9 | — |
| Noria, torre, jardín | 13, 33, 41, 42, 43 | 5 | — |
| Carrusel y autos | 12, 37–40, 61 | 6 | — |
| Teatro | 46–51 | 6 | — |
| Castillo y tren fantasma | 44, 45, 59, 60 | 4 | — |
16 en el hospital + 46 en el parque = 62. Las coordenadas HTML se pasan a UE con la tabla de reubicación del parque orgánico (memoria del parque); el script de colocación debe validar con trazas en PIE, no headless.

## 3. Flashbacks y voz
- HTML: 31 flashbacks (índices pares) en tinta, con globos, onomatopeyas y voz TTS por personaje. UE (0.3.26): 6 flashbacks sobre negro con burbujas.
- Propuesta: **sin TTS en tiempo real.** Fase 1 (demo): **8 flashbacks con voz pregrabada** (WAV generados offline, uno por personaje clave: Alfredo ×3, Isidora ×2, Rafaela, Luisa y portero) en Recuerdos 2, 4, 10, 18, 28, 30, 46, 62. Los otros 23 pares, con globos y subtítulo, sin voz. Fase 2 (Steam): los 31 con voz.
- Respetar las opciones del HTML: "Voz de Alfredo", "Subtítulos de flashbacks" y "Voces".
- Voz de la TV de la casa (3 frases) y de la TV del hospital ("...Isidora... despierta...") con el mismo set de voces.

## 4. Cabina VIDEO
- Se enciende al recoger la **4.ª cinta** (neón verde) y se usa con **4 cintas + fusible**. Mensajes de estado: "Faltan cintas VHS" / "Falta un fusible" / "Ver las cintas".
- `WBP_CabinaVideo`: 12 segmentos × 3,8 s (≈46 s), 3 Recuerdos por cinta. Rótulos: CINTA 1 · 12/03/1998 · 23:41 · CINTA 2 · 23:58 · CINTA 3 · 13/03/1998 · 00:17 · CINTA 4 · 03:33.
- **Nítido** si tienes el Recuerdo; **dañado** si no: un solo material de UI (`MI_VHS_Danado`, ruido + desenfoque + líneas, parámetro 0/1) y el texto "Recoge la página para recuperar la imagen". Es barato: no usa video ni Media Player.
- Segmentos propuestos: cinta 1 = Recuerdos 4, 9, 16 · cinta 2 = 2, 30, 31 · cinta 3 = 46, 48, 50 · cinta 4 = 59, 61, 62 (cada cinta muestra la zona donde está).
- Se salta con E/A. Al final, modal "FIN DE LA GRABACIÓN" con [E] Ir a la sala psiquiátrica / [Q] Quedarte en el parque.

## 5. Acto 3 y finales
- [E] → `WBP_Transicion` (TV) → `L_Hospital` con flag `PersecucionFinal`. Aparece en el extremo de salida del pasillo, mirando hacia la sala. "¡Corre a la sala! Algo viene por el pasillo." Pista: "La luz no lo detiene."
- A los ~10 m: "¡BANG!" Se derriban las puertas de Urgencias y el payaso persigue a 5,2 m/s (sin Willy, León ni Sombra en este tramo; la Sombra del pasillo queda desactivada con el flag).
- La puerta SALA 4 se abre solo con el flag → Pabellón 4 → expediente brillante → cinemática de 4,5 s → **expediente E13–E15** → final según Recuerdos:
  - **62:** FINAL VERDADERO · LA OBRA TERMINÓ.
  - **≥31:** LO QUE RECUERDAS.
  - **<31:** EXPEDIENTE · ISIDORA M.
  - **Muerte:** TE ENCONTRÓ.
  - Estadísticas: tiempo, cintas, Recuerdos, golpes y botellas. Textos del HTML tal cual.
- [Q] "Volver al parque" → `L_Parque` en la entrada, con las cintas y el video ya hechos. Con 62/62 se otorga NOVELA COMPLETA.

## 6. Orden por builds (con interruptor en `DA_Tuning` para cada cosa)
| Build | Fecha | Mío / narrativa | Depende de |
|---|---|---|---|
| **D0 · datos** | 19–21-11 | 62 filas de Recuerdos + 15 de Expediente + 15 notas HTML; textos de finales, cabina y TV; ID fijo por `REC_Pagina`; HUD "/62" | Nada (riesgo nulo) |
| **D1 · RC1** | 22–25-11 | Acto 3 en `L_Hospital` (flag, puerta SALA 4, expediente, payaso, 3 finales) + `WBP_CabinaVideo` + escalada por cintas | **Que el parque cargue sin GPUCrash** (tech-artist/engine): es el bloqueo n.º 1 |
| **D2 · paridad del parque** | 26–28-11 | Cintas en posiciones HTML (renombrar la de la casa a 2); cortocircuito y silueta al volver la energía; tele de la casa; fantasma y VAGÓN 4 en el tren; público que gira; notas del archivo | D1 |
| **D3 · freeze** | 29-11–1-12 | Colocar los 46 Recuerdos del parque y los 16 del hospital; 8 flashbacks con voz; silueta tras el vidrio y puertas rotuladas del pasillo; transición "CINTA 1 · 23:41" | D2 + PIE en un PC que lo aguante |
| RC2 / gold | 3-12 / 8-12 | Solo textos, luz y colisión | — |
Si nos atrasamos, se corta primero: maqueta → público que gira → flashbacks 7–8 → fantasma del tren. **No se corta:** cabina, Acto 3 y finales (son la columna del HTML).

## 7. Riesgos
- **Parque = GPUCrash en la iGPU:** sin eso resuelto no hay Acto 2. Propuesta de ambientación: menos `PointLight` encendidas (los neones emisivos bastan) y zonas de oscuridad que además arreglan W2 (luz segura / penumbra / oscuridad).
- Textos de los Recuerdos: usar los **pies del HTML** (IMAGINARIO autorizado 11-11); no inventar sobre la novela.
- Retroceso casa → cabina (~140 m, W4): con la escalada (Willy y León tras la 4.ª cinta) deja de estar vacío. Se agrega un atajo por un portón lateral solo si el playtest lo pide.
