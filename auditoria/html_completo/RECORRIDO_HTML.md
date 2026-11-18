# RECORRIDO COMPLETO — "Mundo Mágico: Turno Nocturno" (versión HTML)

Auditoría de QA + diseño + narrativa. Fuente: `C:\Users\pansh\Desktop\MundoMagico_Web\index.html` (Three.js 0.160, módulo único ~3.200 líneas de código + data-URIs). Reconstruido **leyendo el código**, no jugando. Las coordenadas son del HTML (X este/oeste, Z: +Z = sur/entrada, -Z = norte/castillo; 1 unidad ≈ 1 m). El archivo está construido por capas: muchas funciones se **redefinen** más abajo (`objective`, `endGame`, `goWard`, `useDoor`, `modalKey`, `takeMem`, `playTapes`…). Aquí se documenta **el comportamiento efectivo final**.

Hallazgos que sorprenden (léelos antes de replicar):
- Los enemigos Oso Willy y León **solo aparecen al recoger la 4.ª cinta**. Antes, las amenazas son el payaso "ángel" (desde la 3.ª cinta), la muñeca (desde la 1.ª) y los sustos scriptados.
- Los enemigos **solo hacen daño con la linterna apagada**. La luz los repele, pero también amplía su visión (16 m frente a 6 m). La tensión real viene de la batería (0,9 %/s).
- El **auto chocador no se puede conducir**: el punto de interacción está en (1e5, 1e5). El código de conducción existe, pero no se puede alcanzar. Los autos son ambientales y te empujan.
- La cinemática del cómic al terminar las cintas (`playComic`) quedó **anulada**: el último `endTapes` va directo a la elección.
- La "entidad de ojos rojos" original (mirarla 3 s = daño) está **desactivada**, porque `entTick` fuerza `entityActive=false`.
- Los interruptores son una **puerta blanda**: guían el objetivo, pero no impiden recoger cintas.

---

## 1. Flujo completo de inicio a fin

### 1.0 Menú y portada
- La pantalla `#menu` muestra una imagen de portada con efecto Ken Burns, niebla animada, luces que parpadean, lluvia en canvas y relámpagos. Subtítulo: **"EL DESPERTAR DE ISIDORA"**. Un aviso parpadea: "♪ Haz clic para activar el sonido". El primer clic o tecla arranca la música procedural del menú (drones, campanas en escala menor, golpes y "rises").
- Botones:
  - **INICIAR JUEGO**: bloquea el puntero y empieza la partida.
  - **CONTINUAR / CARGAR**: carga `localStorage['mm_turno_nocturno_v1']`. Si no hay datos, muestra "No hay partida guardada".
  - **CONFIGURACIÓN**: Música (0–100), Sensibilidad (50–200), Calidad (Ultra/Alta/Media/Baja), casilla "Voz de Alfredo" y botón VOLVER.
- Al bloquear el puntero sale el mensaje "Es tu turno de noche. Mantén la linterna encendida.", se oculta el menú y se detiene su música.

### 1.1 Despertar: Pabellón 4 · Psiquiatría (sala inicial)
- Sala de 14×16 m centrada en (-41,-39), entre x -48..-34 y z -47..-31. Carteles: "PABELLÓN 4 · PSIQUIATRÍA" (rojo) y "Urgencias" con cruz roja. Tiene camas metálicas, un mostrador de recepción al este, ventanas azules con rayos de luna, papeles en el suelo, charcos, reloj de pared y silla de ruedas. Cartel verde **SALIDA** sobre la puerta (-41,-31.15).
- Estado inicial: cámara **acostada en la cama** (-45.5, 1.1, -40.9) mirando al techo, HUD sin linterna, franjas de cine arriba y abajo. Objetivo: **"Despierta..."**.
- Mensaje "Despiertas en una sala oscura. No recuerdas cómo llegaste.", con desenfoque de 7 px que se aclara y un parpadeo negro a los 2,3 s.
- Prompt **"[E / A] Incorporarte"** (también vale Espacio o W). La animación de 3 s sienta al jugador y lo gira hacia el este. Al terminar: "Está oscuro. Busca algo con qué iluminar."
- **Objetivo: "Encuentra algo con qué iluminar"**. La linterna está sobre el mostrador de recepción (-35, 1.28, -37.2) y brilla con una luz puntual cálida.
  - Al recogerla (E): batería **80 %**, linterna encendida, mensaje "Encontraste la linterna. F para encenderla / apagarla".
- **Objetivo: "Sal de la sala por la puerta del sur"** (pista: "puerta con el cartel verde SALIDA"). La puerta está en (-41,-31.5) y responde a menos de 2,8 m. Sin linterna dice "Está demasiado oscuro para salir. Busca una luz."
- En esta sala hay **6 páginas**: índices 3, 4, 7, 8, 14 y 15 (págs. 7, 8, 11, 12, 18 y 19).

### 1.2 Pasillo del hospital abandonado (Urgencias)
- Al abrir la puerta: fundido a negro y teletransporte a (-57.4,-40), mirando al oeste. Mensaje: "Un pasillo largo y a oscuras. Es un hospital abandonado."
- Pasillo de 48 m: x -56..-104, z -43..-37, techo a 3,7 m. Tiene azulejo blanco sucio, muros verde-azulados descascarados, vigas, fluorescentes colgando, escombros y charcos. Al fondo hay un resplandor naranja.
- Puertas decorativas: "HAB. 4", "HAB. 5", "ARCHIVO", "BODEGA". Hay 3 camillas con cortinas, 2 sillas de ruedas y una camilla suelta.
- Objetivo: **"Recorre el pasillo y sal por Urgencias"** (pista: "puertas con luz fría y el cartel SALIDA, al fondo").
- Contenido:
  - Mostrador "Urgencias" con ventanal en (-63.5,-37.5). Interactuable "Revisar el registro de ingreso" → nota **REGISTRO DE URGENCIAS · 12/03/1998** ("Paciente I., 9 años. Corazón débil… Alfredo no puede volver a visitarla").
  - Botiquín en (-60,-38.1) y pilas sobre la camilla (-72,-42).
  - **10 páginas** a lo largo del pasillo (índices 17–26, págs. 21–30).
  - La puerta "SALA 4" en el extremo este (-57.5,-40) dice "La puerta de la sala está atrancada. Sigue hacia Urgencias." Se usa en el final.
- Sustos:
  1. Al entrar: glitch.
  2. **Silueta tras el vidrio** (-66,-37.14). Si la iluminas a menos de 14 m y la miras (producto escalar > 0,93), desaparece con "Algo se movió detrás del vidrio...".
  3. **Apagón en cadena + silla de ruedas** (una sola vez, con x entre -70 y -100):
     - "Las luces empiezan a apagarse..."; los fluorescentes se apagan de este a oeste cada 0,5 s.
     - A los 2,4 s, oscuridad forzada (la linterna se anula) y "...".
     - Una silla de ruedas con una paciente de bata azul rueda 18,6 m hacia ti en 1,8 s, con chirridos espaciales.
     - Se detiene a 3,4 m. A los 1,7 s se le cae el pelo y alza la cabeza (grito agudo, vibración).
     - Se lanza hasta 1,1 m, destello blanco y desaparece. Quedan manchas en paredes y suelo, vuelven las luces.
     - Nota **LA PACIENTE DEL PASILLO** ("No era Isidora. Pero llevaba su mismo pañuelo") y mensaje "Algo estuvo aquí... · nota añadida [N]".
- Salida: puerta con cartel SALIDA en (-103,-40), opción "Salir por Urgencias". Fundido y aparición en la **entrada del parque (0, 1.45, 46.5)** mirando al norte. Mensaje: "Sales del hospital. Frente a ti, la entrada de Mundo Mágico." Se marca `outside=true`.

### 1.3 Entrada del parque: sin energía (interruptores 0/4)
- La explanada exterior va de z +30 a +50. Tiene pasto seco y un estandarte rojo con calavera y farol (-15.2, 37). El pórtico está en z=30, con letrero gigante "MUNDOMAGICO" (neón rosa) y casetas "ENTRADA" y "JUEGO" en x ±9,5. La reja perimetral (±50) tiene un hueco entre x -7 y 7.
- **Estatua del payaso gigante** en (-8, 41) mirando al pórtico. Al principio es estática y sólida.
- Mensaje automático: "Todo el parque está sin energía. Enciende las luces de la entrada." Todas las luces del parque, salvo la luna, están apagadas.
- **Objetivo: "Enciende las luces de la entrada (n/4)"**. Hay 4 palancas en z=34.5:
  - x=-4.5 **LETRERO** (letrero y globo del pórtico).
  - x=-1.5 **FAROL** (farol del estandarte).
  - x=1.5 **FAROLES** (8 faroles del camino central).
  - x=4.5 **GUIRNALDAS** (bombillas sobre la avenida).
  - Las palancas tienen indicador rojo pulsante; encendidas pasan a verde. Mensaje "Luces encendidas (n/4)".
- **Cortocircuito**: el 4.º interruptor activado **falla siempre la primera vez** (destello blanco, se apagan las luces, palanca de vuelta a rojo). Mensaje "¡Cortocircuito! Las luces se apagan un instante. Inténtalo otra vez." Hay que reintentarlo.
- Con 4/4 se ejecuta `powerAll`:
  - Se enciende todo el parque de forma escalonada (0,3–2,8 s), con campanas, glitch y mensaje "¡Se restablece la energía! Las luces del parque se encienden...". Nota **ENERGÍA RESTABLECIDA**.
  - **Susto**: a los 1,2 s aparece bajo el letrero (0,28) una **silueta negra gigante con sombrero y brazos abiertos**, con halo rojo pulsante, durante 3,6 s. Luego se apaga el letrero y sale "Alguien estaba bajo el letrero...". Nota **SILUETA BAJO EL LETRERO**.
- En esta zona: **mapa** (4,37), botiquín (12,26), botellas (6,40), (-14,30) y (22,34), y páginas 2 y 61 (y 13 en el camino).

### 1.4 Parque abierto: búsqueda de las 4 cintas VHS (orden libre)
Objetivo: **"Encuentra las cintas VHS (faltan N)"**. La flecha de navegación (arriba, con metros) apunta a la cinta más cercana que no esté en la casa cerrada. En el minimapa las cintas son puntos rojos. Cada cinta flota, gira y emite luz naranja. Se recoge a menos de 2,4 m, o a menos de 4,5 m si la miras. Cada una da **batería +20 %** y el mensaje "Cinta VHS recuperada (n/4). Batería +20%".

| Cinta | Posición | Lugar |
|---|---|---|
| 1 | (-26, -10) | Entre la noria (-26,6), la torre del reloj (-30,-5) y el jardín (-18,-4) |
| 2 | (38.5, -30.5) | **Dentro de la casa de Alfredo** (cerrada con llave) |
| 3 | (-22, -34) | Detrás del tren infantil y del castillo, lado oeste, junto a la montaña rusa |
| 4 | (6, -19) | Final de la avenida central, frente al castillo |

Mapa del parque (lugares y coordenadas):
- **Avenida central**: camino de 9×62 m (x ±4,5, z -29..33). Tiene 8 faroles en x ±6 (z 26, 10, -8, -20) y 4 líneas de guirnaldas.
- **Maqueta del parque** en miniatura (-6,14), con trencito que gira, plataneras y árboles (z 3,5). La muñeca empieza en (8,14).
- Kioscos: **COMPLETOS** (-11,3), **ALGODÓN DE AZÚCAR** (11,5), **SOPAIPILLAS** (-11,-14).
- **Cabina VIDEO** (16,22), con pantalla verde y neón (ver 1.7).
- **Bodega de MANTENIMIENTO** (-30,24) (ver 1.5). **Noria** (-26,6) y **torre del reloj** (-30,-5), estáticas.
- **Jardín de hojas gigantes** (-18,-4): disco de musgo de 7,5 m, 13 plantas de 5,5–8,5 m, hongos de neón y luciérnagas.
- **Carrusel** (25,-8), estático con neón cian. **Autos chocadores**: pista de x 26,9–41,1 y z 2,9–13,1, 6 autos (5 con IA y 1 "del jugador" vacío), neón rosa y cian.
- **Tren infantil**: aro de 6 m en (-20,-22) con 3 vagones circulando. **Castillo** (0,-30) con 4 torres. **Montaña rusa** decorativa en todo el borde norte (z≈-47).
- **Teatro Mundo Mágico** (20,-22) (ver 1.6). **Tren Fantasma** (-40,-20) (ver 1.6). **Casa de Alfredo** (36,-30) (ver 1.5).
- Ambiente: lluvia y gotas en el lente (solo al aire libre y con calidad mayor que Baja), niebla baja, árboles secos, charcos, estrellas, luna con halo y melodía de campanas cerca del carrusel.

Recursos que quedan en el mapa (la capa "recursos escasos" eliminó las pilas de (-9,33) y el botiquín de (34,-22)):
- **Pilas**: (-20,22), (28,17), (-40,-26) dentro del Tren Fantasma, cajón de la casa, bodega (cae de los disfraces), premio del teatro y hospital (-72,-42). En total 7.
- **Botiquines**: (12,26), (-18,-4) en el jardín, armario de la casa, premio del teatro y hospital (-60,-38.1). En total 5.
- **Botellas**: 10, en (6,40), (-14,30), (-22,16), (16,22), (-6,-4), (10,-14), (30,-6), (-26,-18), (22,34) y (-3,18). Se recogen pasando a menos de 1,3 m. Se empieza con 2.

### 1.5 Cadena llave → casa → fusible
1. **Casa de Alfredo** (36,-30), 11,3×9,3 m, con puerta en la cara sur (32.5,-25.35). La primera vez que intentas abrir dice "Está cerrada con llave · nota añadida [N]" y añade la nota **PUERTA CON LLAVE** ("…las llaves… en la bodega de mantenimiento, entre los disfraces"). El objetivo cambia a "Busca la llave de la casa", con la flecha apuntando a los disfraces.
2. **Bodega de mantenimiento** (-30,24), 7×6 m. La puerta está en la cara este (-26.5, 24.9) y hay que abrirla con E.
   - Al entrar: "Huele a naftalina. Algo gotea en la oscuridad...", con goteo cada 2–6 s.
   - Tiene lámpara apagable, escritorio, peluca sobre un maniquí, perchero con 5 disfraces a rayas y cajas.
   - Interactuables:
     - "Examinar la peluca" → nota **PELUCA DE ISIDORA**.
     - "Revisar el escritorio" → nota **LISTA DE PERSONAJES**: "El Oso Rey · El Cerdo · El Mago · El Oso Willy · La Mascota… La protagonista: ______". Es la clave del teatro.
     - "Revisar los disfraces" (-33,24): los disfraces se balancean, glitch y "Los disfraces se mecen sin viento... algo cae al suelo." Aparecen la **llave** (-32.2,24.7) y unas **pilas**.
   - Al recoger la llave: nota **LLAVE DE LA CASA** y "Llave de la casa guardada".
3. Con la llave, el objetivo pasa a "Abre la casa de Alfredo". En la puerta: "Usar la llave" → "Usas la llave. La puerta se desbloquea."
4. Interior de la casa: dos cuartos separados por un tabique en x=37.
   - Al entrar: "...La casa huele a humedad. Algo se mueve arriba." y glitch.
   - Interactuables:
     - Luz de la sala: se enciende o apaga y parpadea.
     - **Tele**: estática con frases cada 5–10 s: "...Isidora... despierta...", "...no mires la cinta...", "...ya viene por ti...".
     - **Cajón de la cómoda** (cuarto oeste): pilas + **fusible**.
     - **Armario** (cuarto este): botiquín.
     - Mesa: "Leer la nota" → **CARTA SIN TERMINAR** ("…cuatro cintas, cuatro noches. La niña del pabellón sabe dónde están. — Alfredo").
     - Escritorio: "Leer el diario" → **DIARIO DE ALFREDO · 13/03/1998** ("…La cabina VIDEO funciona si juntas las cuatro cintas. Si la linterna se apaga, vienen por ti. La luz los detiene…").
   - **Cinta 2** y las páginas 1, 5, 9, 29 y 30 (31 junto a la fachada oeste).
5. El fusible añade la nota **FUSIBLE** ("…repuestos para el tablero de la cabina VIDEO"). Si se llega a la cabina sin fusible: "La cabina no enciende. Falta un fusible · nota añadida [N]" y la nota **CABINA VIDEO**.

### 1.6 Lugares opcionales con contenido
- **Teatro Mundo Mágico** (20,-22), 14,4×10,4 m, con entrada por el sur (z -17).
  - Tiene marquesina con neón dorado, escenario con telón de terciopelo, 3 conos de luz y estrella brillante. El público son maniquíes en 24 butacas (62 % ocupadas).
  - Al entrar: "Los asientos están ocupados. Nadie respira." y glitch.
  - **Puzle de pedestales**: 5 pedestales de color sobre el escenario, en orden de pantalla Mago, Mascota, Oso Rey, Oso Willy, Cerdo. Hay que tocarlos en el orden de la LISTA: **Oso Rey → Cerdo → Mago → Oso Willy → Mascota**.
    - El primer toque añade la nota **EL REPARTO**.
    - Cada acierto ilumina el pedestal: "Correcto (n/5)".
    - Un error reinicia todo: "El público se ríe bajito... Orden equivocado."
    - Al resolverlo: "El público aplaude... en silencio. Algo cae del telón." Aparecen **pilas + botiquín** y la nota "EL REPARTO (resuelto)". El público gira la cabeza para seguirte.
  - Aparición única: al acercarte a menos de 9 m, una figura negra de ojos blancos aparece a 12 m, a un lado: "Alguien está de pie en el escenario...". Se esfuma al mirarla 0,7 s o a los 9 s.
  - Hay unas 6 páginas dentro o alrededor (índices 45–50).
- **Tren Fantasma** (-40,-20): túnel de 14 m con fachada de ojos gigantes, neón verde "TREN FANTASMA", vías, vagón con pasajero esqueleto y ojos en las paredes.
  - "Revisar el vagón" → nota **VAGÓN 4** ("…Función final · Turno nocturno · 12/03/1998… 'El público no se fue'").
  - **Susto del fantasma**: en la zona del vagón, un fantasma blanco se lanza 1,4 m desde la pared (grito, vibración). Se repite cada 12 s si sales y vuelves.
  - Aparición única: "En el túnel del tren fantasma, una sombra te observa."
  - Contiene las pilas de (-40,-26) y la página 43.
- **Autos chocadores**: ambientales. 5 autos con IA chocan entre sí con física simple, chispas y sonido, y **empujan al jugador**. No se puede subir (ver hallazgos).

### 1.7 Cabina VIDEO, cintas y elección
- Condición: 4/4 cintas **y** fusible. Prompt según el estado: "Faltan cintas VHS" / "Falta un fusible" / "[E / A] Ver las cintas" / "[E / A] Ir a la sala psiquiátrica" (si ya la viste).
- Al recoger la 4.ª cinta, aunque no haya fusible:
  - La cabina se enciende con neón verde.
  - **Despiertan Willy y el León**, a 34 y 46 m: "Algo despertó en la niebla... La luz los mantiene lejos."
  - HUD: "VHS: completas (4/4)".
- **Reproducción** (cámara cinemática en la pantalla, de (17.2,1.95,27.5) a (16.1,1.65,24.9), con franjas de cine):
  - 12 segmentos de 3,8 s (≈46 s), 3 páginas por cinta.
  - Si tienes la página, se ve nítida; si no, dañada, borrosa y con el texto "Recoge la página para recuperar la imagen".
  - Subtítulos: "CINTA 1 · 12/03/1998 · 23:41", "CINTA 2 · 12/03/1998 · 23:58", "CINTA 3 · 13/03/1998 · 00:17", "CINTA 4 · 13/03/1998 · 03:33", junto al pie de foto de la página.
  - Se salta con E/A.
- **Elección** (modal "FIN DE LA GRABACIÓN · La puerta de la cinta 4 existe. Está dentro del parque."):
  - **[E / A] Ir a la sala psiquiátrica** → 1.8.
  - **[Q / B] Quedarte en el parque** → "El parque te espera...". Objetivo "Vuelve a la cabina VIDEO cuando estés lista"; desde entonces `objective()` muestra "Vuelve a la sala psiquiátrica". Volver a la cabina abre otra vez la elección. El payaso queda desactivado y los enemigos siguen.

### 1.8 Persecución final y Pabellón 4
- Fundido y aparición en el **extremo oeste del pasillo del hospital** (-99,-40), mirando al este. Mensaje: "¡Corre a la sala! Algo viene por el pasillo." **Objetivo: "¡Corre a la sala!"** (pista: "Llega a la puerta del fondo del pasillo, al este. **La luz no lo detiene.**").
- Al cruzar x>-90: "¡BANG! Algo derribó las puertas de Urgencias!". El **payaso gigante** (escala 0,58) aparece en (-104,-40) y persigue a **5,2 m/s** en X, con brazos agitándose y latido cada 0,4 s.
  - Golpe a menos de 1,9 m: -1 corazón, cooldown 2,2 s, y el payaso retrocede 8 m.
  - Distancia: unos 41 m hasta la puerta "SALA 4" (-57.5,-40). Caminando a 4 m/s te alcanza; hay que esprintar a 6,8 m/s, pero el aliento dura unos 4 s.
- Puerta "Entrar a la sala": fundido y aparición en el Pabellón 4 (-41,1.45,-35). Mensaje "Pabellón 4 · Sala psiquiátrica". **Objetivo: "Lee el expediente de Isidora"**. El expediente brilla sobre la mesa del fondo (-41,0.95,-45.8).
- Con E a menos de 3,5 m ("[E / A] Leer el expediente"): cinemática de 4,5 s acercándose al expediente y después la **pantalla de final** (sección 5).

### 1.9 Tras el final
- **[E / A] Reiniciar**: recarga la página.
- **[Q / B] Volver al parque a buscar lo que falta**: aparición en (0,43) con "Sigue explorando: aún puedes completar la novela." Se puede leer el expediente de nuevo con más páginas para mejorar el final.
- Muerte (0 corazones): overlay **"TE ENCONTRÓ · FIN DE LA DEMO"**, "Recuperaste X/4 cintas. No debiste mirar atrás." y botón REINTENTAR (recarga; se puede continuar desde el último autoguardado).

---

## 2. Cadena de objetivos y progresión

Prioridad efectiva de `objective()` (se evalúa cada 0,25 s, de arriba abajo):
1. `chase.on` → "¡Corre a la sala!"
2. Energía no restablecida y en el parque → "Enciende las luces de la entrada (n/4)"
3. En el hospital → "Recorre el pasillo y sal por Urgencias"
4. Sin linterna → "Encuentra algo con qué iluminar"
5. Sin haber salido → "Sal de la sala por la puerta del sur"
6. En la sala (final) → "Lee el expediente de Isidora"
7. Cintas < 4 → "Encuentra las cintas VHS (faltan N)" → "Busca la llave de la casa" → "Abre la casa de Alfredo"
8. Cintas = 4 sin haber visto el video → sin fusible: llave / abrir casa / **"Consigue un fusible"** ("abre el cajón de la cómoda del cuarto del oeste"); con fusible: **"Ve a la cabina VIDEO"**
9. Después → "Vuelve a la sala psiquiátrica"

Dependencias duras: **linterna** (para la puerta) → **4 cintas + fusible** (para la cabina) → **elección E** → **puerta SALA 4** → **expediente**. La cinta 2 y el fusible dependen de la **llave** (bodega). Los interruptores solo guían el objetivo; al cargar partida se dan por hechos si ya hay alguna cinta o llave.

**Páginas del cómic ("Recuerdos")**: **62** (índices 0–61 = págs. 4–65 de la novela). Brillan, flotan y en el minimapa son puntos blancos; con E se recogen. Las de índice par lanzan un **flashback en tinta** (shader de cómic con trama, globos de diálogo, onomatopeyas como PLUNK, RIIIP o JA JA JA, y voz TTS en español con tono por personaje: Alfredo, Rafaela, Luisa, Isidora, portero, médico, narrador). Las impares muestran la página directamente. Galería con J o ↓ y navegación ←/→. Con las 62: tesoro "★ NOVELA COMPLETA ★" (portada).

Reparto aproximado por zona:

| Zona | Índices | N.º |
|---|---|---|
| Pabellón 4 (sala inicial) | 3, 4, 7, 8, 14, 15 | 6 |
| Pasillo del hospital | 17–26 | 10 |
| Entrada y avenida sur | 2, 13, 61, 52 | 4 |
| Bodega y alrededores | 0, 10, 16, 27, 28 | 5 |
| Casa de Alfredo y alrededores | 1, 5, 9, 29, 30, 31 | 6 |
| Maqueta, kioscos y avenida | 6, 33, 35, 51, 53, 54, 55, 56, 57 | 9 |
| Noria, torre y jardín | 12, 32, 40, 41, 42 | 5 |
| Carrusel y autos chocadores | 11, 36, 37, 38, 39, 60 | 6 |
| Teatro y alrededores | 45–50 | 6 |
| Castillo y tren fantasma | 43, 44, 58, 59 | 4 |
| **Total** | | **62** (algunas 21+ se reubican si chocan con colisiones) |

**Archivo de notas** (N o ←, navegación ←/→). 15 notas: PUERTA CON LLAVE, LLAVE DE LA CASA, FUSIBLE, CABINA VIDEO, CARTA SIN TERMINAR, DIARIO DE ALFREDO, PELUCA DE ISIDORA, LISTA DE PERSONAJES, EL REPARTO, EL REPARTO (resuelto), VAGÓN 4, REGISTRO DE URGENCIAS, LA PACIENTE DEL PASILLO, ENERGÍA RESTABLECIDA, SILUETA BAJO EL LETRERO.

---

## 3. Enemigos y amenazas

**Oso Willy** (velocidad base 2 m/s, grande y robusto, ojos naranja brillantes, ojo colgando, costuras, ropa rota, relleno salido, garras) y **El León** (2,8 m/s, delgado y alto, melena de rastas, corona dorada, costillas, mandíbula caída, moño roto).
- Aparición: solo al completar 4/4 cintas. Inactivos en el Pabellón 4 y el hospital (x<-51). Al volver se recolocan a 34 y 46 m.
- Estados de su máquina de estados:
  - **patrulla**: 0,55× velocidad entre 12 waypoints.
  - **investigar**: 1,1×, va a un ruido.
  - **búsqueda**: 5 s mirando alrededor.
  - **persecución**: con luz encendida van a 1× y se paran a 3,5 m; con luz apagada van a **1,9×**.
  - **esconderse**: 1,2× hacia el escondite más lejano de 6, esperan 6 s y vuelven a patrullar.
- **Visión**: 16 m con linterna encendida y 6 m apagada; ×0,65 agachada; ×1,25 en difícil. Cono de ±72° (1,25 rad), o 360° a menos de 4 m.
- **Oído**: el radio de ruido del jugador es 24 m corriendo, 10 m caminando, 3,5 m agachada en movimiento y 0 quieta (×1,3 en difícil). Una barra azul/amarilla/roja del HUD lo muestra.
- **Linterna que ahuyenta**: si apuntas a uno a menos de 22 m (cono de ~25°), retrocede a 0,6× su velocidad. Tras 2,5 s iluminado se va a **esconder**. Primera vez: "La linterna los repele. No la apagues."
- **Linterna que delata**: la luz casi triplica la distancia a la que te ven.
- Pierden el rastro tras 4 s sin verte ni oírte, o a más de 34 m.
- **Daño**: solo con la **linterna apagada** y a menos de 1,7 m. Quita 1 corazón, cooldown 2,5 s, y el enemigo retrocede 7 m.
- Presión: cada 40–65 s, el patrullero más lejano va a investigar tu posición.
- Primeros avisos: "Te vieron. Agáchate, apaga la linterna o lanza una botella para distraerlos." / "Te oyeron. Agáchate [C] para hacer menos ruido."
- Audio: gruñidos y pasos espaciales (HRTF), latido según la distancia (0,45 s en persecución) y drone de tensión que sube de tono.

**Payaso gigante**:
- (a) **Estatua** en la entrada (-8,41), sólida y estática.
- (b) **Modo "ángel lloroso"** desde la **3.ª cinta** y hasta ver el video. Pierde la colisión. Si no está en tu campo de visión, cada 3,5–6 s cambia de pose (4 poses) y avanza hasta 3,2 m hacia ti, con campana y golpe espacial. Con la linterna apagada y a menos de 2,8 m quita 1 corazón (cooldown 3 s) y se teletransporta a 14 m.
- (c) **Persecución final** en el hospital (1.8): 5,2 m/s y golpe a 1,9 m. La luz no lo detiene.

**Muñeca** (vestido rojo, 1,3×). Desde la **1.ª cinta**, en (8,14). Cuando no la miras durante 2,6 s avanza hasta 2,4 m hacia ti ("Esa muñeca... ¿estaba más lejos antes?"). Si se pone a menos de 3,2 m: "¡Estaba justo detrás de ti!" y se teletransporta a un waypoint. No hace daño.

**Sustos scriptados**:
1. Despertar (parpadeo).
2. Silueta tras el vidrio (hospital).
3. Apagón y silla de ruedas (hospital, x<-70).
4. Cortocircuito del 4.º interruptor.
5. Silueta gigante bajo el letrero al volver la energía.
6. Entrada a la bodega.
7. Disfraces que se mecen.
8. Entrada a la casa ("algo se mueve arriba") y voces de la tele.
9. Teatro: público inmóvil, figura en el escenario y público que se gira tras el puzle.
10. Fantasma del Tren Fantasma (repetible) y sombra del túnel.
11. Despertar de los enemigos (4.ª cinta).
12. Puertas de Urgencias derribadas (persecución).

**Escondites**: no hay armarios donde meterse. El sigilo se basa en agacharse (C), apagar la luz, alejarse y lanzar botellas (T: parábola; al caer alerta a los enemigos en 40 m).

---

## 4. Sistemas del jugador

- **Movimiento**:
  - Caminar 4 m/s; correr (Shift o LB) 6,8 m/s; agachada ×0,5; con 1 corazón ×0,85 y cojera.
  - **Aliento**: 100; correr gasta 24/s (≈4 s). Recupera 22/s quieta y 12/s caminando. Al agotarse: "Sin aliento...", jadeos, y no se puede correr hasta 35.
  - FOV 70→80 al correr. Cámara: **tercera persona por defecto** (V o R3 alterna con primera; en interiores el offset baja al 55 %).
- **Linterna**:
  - Encender/apagar con F, 1 o X. Batería inicial 80 %, consumo **0,9 %/s** (×1,3 en difícil), es decir unos 111 s de luz por cada 100 %.
  - Cinta +20 %. Pilas (R o B): **+40 %**, no se pueden usar con 95 % o más ("La batería aún está casi llena").
  - Con menos del 15 % parpadea. Avisos con menos del 20 % ("Batería baja: busca pilas") y del 8 % ("¡La linterna se apaga!"). A 0 se apaga.
  - Efectos: haz volumétrico con retraso y balanceo, polvo, luz de rebote y destello de lente en neones y luna.
  - Presupuesto total de la partida: 80 % inicial + 4×20 % de cintas + 7×40 % de pilas ≈ 440 % ≈ 8 min de luz. Hay que **racionar**.
- **Vida**: **6 corazones** (retrato de Isidora que palidece con el daño). Botiquín (4 o ↑): +1 corazón, no se usa con 6.
  - Daño: viñeta roja, sacudida e inclinación.
  - Con 2 o menos: viñeta pulsante, respiración y latido.
  - Con 0: "TE ENCONTRÓ".
- **Mochila** (2, Tab, I o Y): cintas, pilas, botiquines, mapa, llave (sí/usada/no), fusible, recuerdos n/62 y notas.
  - Barra de casillas 1–4: linterna, mochila, mapa, botiquín.
  - Contador de botellas: "BOTELLAS n [T] · AGACHARSE [C]".
- **Mapa**: el minimapa circular (arriba a la derecha) siempre está, con norte, flecha del jugador, colisiones, cintas en rojo, páginas en blanco y marcador del objetivo. Recoger el mapa (4,37) permite **ampliarlo** ×2,2 (3, M o Back).
- **Pistas**: H o LT muestra el texto del objetivo, además de "Recuerdos n/62" y aviso de batería baja. Aparece sola tras 180 s sin progreso ("¿PERDIDA? OBJETIVO"). Flecha arriba con distancia en metros.
- **Guardado**: autoguardado cada 8 s (si hay linterna y no hay modal) y al pausar. Guarda cintas, objetos y páginas, inventario, vida, batería, posición, orientación, notas, interruptores y llave usada. **No** guarda el puzle del teatro ni los sustos.
- **Opciones** (panel en pausa, también navegable con mando): Volumen, Sensibilidad, Calidad, Subtítulos de flashbacks, Voces, **Modo difícil** ("enemigos más rápidos y más atentos").
- **Calidad**: Ultra / Alta / Media (por defecto) / Baja. G las recorre. Se baja sola un nivel si hay menos de 36 fps. Baja quita sombras, bloom, relieve, niebla en sprites, lluvia y polvo.
- **Teclado**: WASD, Shift, F, E (interactuar/aceptar), Q (rechazar/cerrar), 1–4, Tab/I, M, R, G, V, J, N, H, C, T, Esc (pausa), F2 (QA: teletransporte a la siguiente página pendiente).
- **Mando**: LS mover, RS cámara, LB correr, RB agacharse, RT lanzar botella, A recoger/aceptar, B pilas o rechazar, X linterna, Y mochila, ↑ botiquín, → mapa, ↓ recuerdos, ← notas, LT pista, R3 cámara, Back mapa grande, Start pausa. Vibración en eventos.
- **Audio**:
  - Música MP3 del parque solo en exteriores (baja al 25 % en pausa).
  - Lluvia exterior / ahogada en interiores.
  - Pasos según la superficie (baldosa, madera, metal, hormigón, camino, mojado).
  - Goteos espaciales en bodega, pabellón y hospital.

---

## 5. Finales

Se calculan al leer el expediente, según `got` = páginas reunidas (de 62). Encabezado común: "CAPÍTULO 1 · COMPLETADO".

1. **FINAL VERDADERO · LA OBRA TERMINÓ** (got = 62): "Entre las hojas del expediente están todas las páginas. Reconoces la letra de Isidora: «Don Alfredo cumplió su promesa». Las luces del parque se encienden una a una. Por primera vez, alguien aplaude."
2. **FINAL · LO QUE RECUERDAS** (got ≥ 31): "Reconoces muchas páginas, pero faltan algunas… En el parque, una sombra se queda sentada en la última fila."
3. **EXPEDIENTE · ISIDORA M.** (got < 31): "Ingresó al Pabellón 4 la noche del 12 de marzo de 1998. Las cintas eran suyas. Hay mucho que no recuerdas. Las páginas del parque guardan el resto."
4. Fuera de los finales, **muerte**: "TE ENCONTRÓ · FIN DE LA DEMO".

Estadísticas que se muestran en los finales: tiempo en minutos, cintas 4/4, páginas n/62, golpes recibidos, botellas lanzadas y "Modo difícil" si aplica. Todos cierran con "Continuará · Capítulo 2: Pabellón 4" y las opciones [E] Reiniciar / [Q] Volver al parque. Las cintas siempre son 4/4 en un final, porque la cabina lo exige.

---

## 6. Duración estimada

| Tramo | Ruta crítica | Con exploración |
|---|---|---|
| Menú, despertar, linterna, salir del Pabellón 4 | 1–2 min | 3–4 min (6 páginas) |
| Pasillo del hospital (sustos, registro) | 1,5–2 min | 3–5 min (10 páginas) |
| Interruptores, cortocircuito, silueta | 1–2 min | 2–3 min |
| Cintas 1, 3 y 4 en el parque abierto | 5–8 min | 10–15 min |
| Casa cerrada → bodega (llave) → casa (cinta 2 y fusible) | 4–6 min | 6–10 min (notas, tele) |
| Opcionales: teatro (puzle), tren fantasma, jardín | — | 6–10 min |
| Cabina VIDEO (video de 46 s) y elección | 1–2 min | 1–2 min |
| Persecución y expediente | 1 min | 1 min |
| **Total** | **≈ 15–25 min** (final "expediente") | **45–75 min** (62 páginas, final verdadero) |

La escasez de batería y los enemigos tras la 4.ª cinta alargan el tramo final si el jugador vuelve a por páginas.

---

## 7. Momentos clave (guion de réplica)

| N.º | Zona | Evento | Disparador |
|---|---|---|---|
| 1 | Menú | Portada animada y música procedural | Primer clic o tecla |
| 2 | Pabellón 4 | Despertar en la cama, desenfoque y parpadeo; "Despierta..." | INICIAR JUEGO |
| 3 | Pabellón 4 | Incorporarse (animación de 3 s) | E / Espacio / W |
| 4 | Pabellón 4 | Linterna en el mostrador (batería 80 %) | E cerca de (-35,-37.2) |
| 5 | Pabellón 4 | Puerta SALIDA → pasillo del hospital | E en (-41,-31.5) con linterna |
| 6 | Hospital | Silueta tras el vidrio desaparece | Iluminarla y mirarla a menos de 14 m |
| 7 | Hospital | Nota REGISTRO DE URGENCIAS | E en el mostrador (-63.5,-38.2) |
| 8 | Hospital | Apagón en cadena + silla de ruedas + paciente | x<-70 (una vez) |
| 9 | Hospital | Salir por Urgencias → entrada del parque (0,46.5) | E en (-103,-40) |
| 10 | Entrada | "Todo el parque está sin energía" | Primera vez en el exterior |
| 11 | Entrada | Interruptores 1–3 encienden grupos de luces | E en las palancas de z=34.5 |
| 12 | Entrada | Cortocircuito del 4.º interruptor | 4.º intento (una vez) |
| 13 | Entrada | Energía total + silueta gigante bajo el letrero | 4/4 interruptores |
| 14 | Entrada | Recoger el mapa | (4,37) |
| 15 | Parque | Cinta 1; aparece la muñeca | Recoger cinta en (-26,-10) |
| 16 | Casa | Puerta cerrada → nota PUERTA CON LLAVE | E en la puerta (32.5,-25.35) |
| 17 | Bodega | Abrir la puerta y "Huele a naftalina..." | Entrar a (-30,24) |
| 18 | Bodega | Nota LISTA DE PERSONAJES (clave del teatro) | Escritorio |
| 19 | Bodega | Los disfraces se mecen → llave y pilas | "Revisar los disfraces" |
| 20 | Casa | Usar la llave; "Algo se mueve arriba" | E en la puerta con la llave |
| 21 | Casa | Tele con voces; carta y diario de Alfredo | Interactuables |
| 22 | Casa | Cajón: fusible + pilas; armario: botiquín | Abrir cajón o armario |
| 23 | Casa | Cinta 2 | (38.5,-30.5) |
| 24 | Parque | Cinta 3 → el payaso pasa a ángel lloroso | 3.ª cinta recogida |
| 25 | Teatro | "Nadie respira" + figura en el escenario | Entrar o acercarse a menos de 9 m |
| 26 | Teatro | Puzle Oso Rey → Cerdo → Mago → Willy → Mascota | Pedestales |
| 27 | Tren Fantasma | Fantasma que salta + nota VAGÓN 4 | Zona del vagón |
| 28 | Parque | Cinta 4 → despiertan Willy y el León; la cabina se enciende | 4.ª cinta |
| 29 | Parque | IA: visión, oído, ahuyentar con luz, botellas | Desde el n.º 28 |
| 30 | Cabina VIDEO | Video de 12 páginas (nítidas o dañadas) | E con 4 cintas + fusible |
| 31 | Cabina VIDEO | Elección E (sala) / Q (quedarse) | Fin del video |
| 32 | Hospital | "¡Corre a la sala!" desde (-99,-40) | Elegir E |
| 33 | Hospital | ¡BANG! El payaso derriba Urgencias y persigue a 5,2 m/s | x>-90 |
| 34 | Pabellón 4 | Puerta SALA 4 → "Lee el expediente de Isidora" | E en (-57.5,-40) |
| 35 | Pabellón 4 | Cinemática y final según páginas (62 / ≥31 / <31) | E en el expediente (-41,-45.8) |
| 36 | Parque | Volver a buscar páginas → tesoro NOVELA COMPLETA | Q en el final / 62 páginas |

### Prioridades para Unreal
1. Respetar la **estructura de 3 actos**:
   - Pabellón 4 → hospital → parque.
   - Energía → cintas, llave, casa, fusible → cabina.
   - Persecución inversa por el hospital → expediente.
2. **La linterna como sistema central**: batería escasa (0,9 %/s), los enemigos solo dañan con la luz apagada, la luz los ahuyenta y los esconde, pero también los atrae (visión de 16 m frente a 6 m).
3. **El orden de escalada de amenazas**: muñeca (1.ª cinta) → payaso ángel (3.ª) → Willy y León (4.ª) → persecución del payaso (final).
4. **Las 62 páginas con flashbacks en tinta y voces**, el video de la cabina que depende de las páginas y los **3 finales** con umbrales de 62 y 31.
5. Sustos scriptados del hospital y de la energía, puzle del teatro y las 15 notas con sus textos.
