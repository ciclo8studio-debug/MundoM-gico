# Propuestas @world-builder: mapa, historia, personajes e identidad

**Reunión con el CTO, 2026-11-09.** Fuentes: `_contexto.md`, `auditoria/world-builder.md` (hallazgos W1–W13) y `diseno_sprint3_demo.md`.

**Criterio:** impacto por hora de prueba. El CTO tiene unas 4 h por semana, una iGPU y builds Shipping de 15 min. Por eso se priorizan audio, texto, luces que se apagan y reutilizar lo que ya existe, antes que geometría nueva.

**Sprints:** S1 = demo corta (13-11). S2 a S5 = semanas hasta v0.5.0 (10-12). "Post" = después de v0.5.0.

**★ = TOP 3**

---

### 1. ★ Radio AM a pilas: la voz de Luisa, auxiliar del turno nocturno
- **Qué vive el jugador:** en la boletería, Isidora encuentra una radio portátil de pilas. Entre estática, una radio AM genérica de madrugada ("AM 1180, son las tres y diez") da la hora, el tiempo y una canción de época sin licencia. De pronto se cuela una voz: Luisa, la auxiliar del Pabellón 4, que le habla a Isidora en coma ("Isi, si me escuchas, apriétame la mano"). En momentos clave, Luisa sirve de guía ("dicen que el fusible lo guardaba el caballero de la casa"). Si se acerca un enemigo, la radio se satura, como un detector.
- **Impacto:** muy alto. Le da al juego un personaje aliado sin modelar ni animar a nadie. Explica el título ("Turno Nocturno"). Reemplaza unas 20 páginas de texto, guía sin flechas y suma una mecánica de tensión: la estática avisa que hay peligro.
- **Esfuerzo:** S–M. Son entre 10 y 15 líneas grabadas con celular por una voz chilena real (no TTS), con subtítulos. Se dispara por zonas que ya existen (`ActualizarZona`), y el volumen de la estática depende de la distancia al enemigo más cercano.
- **Assets:** un modelo de radio (SM_ de SoulCity, DetectiveOffice o Horror_Hospital), estática generada con `sonidos_terror.py`, la estructura de `SonidoLluvia` y la mochila (como objeto equipable).
- **Prueba:** unos 20 min (escuchar los disparadores en un recorrido).
- **Sprint:** S2 (radio y 5 líneas) y S3 (resto de líneas y detector).

### 2. ★ La réplica: un temblor que apaga el parque
- **Qué vive el jugador:** justo después de la cinemática de las luces, cuando el parque "se ve seguro", viene un temblor de unos 6 s. La cámara se sacude, el mando vibra, las guirnaldas se balancean, cae polvo, la noria cruje y suena una alarma de auto lejana. Las luces se cortan sector por sector y solo queda la avenida a medias. Antes, en el hospital, un temblor chico hace tintinear los frascos de Farmacia: el jugador ya lo conoce y lo teme. Las notas mencionan "el del 85" (el terremoto de 1985) como recuerdo de los adultos.
- **Impacto:** muy alto. Es identidad chilena inmediata: todo chileno reconoce un temblor. Además corrige el hallazgo crítico W2, porque la luz deja de recompensar al jugador y el parque vuelve a ser penumbra con riesgo. Es un set-piece memorable y barato.
- **Esfuerzo:** S. Camera shake (asset de `CameraShake`), el sonido grave que ya existe más un rumble, un timeline que apaga los grupos `LUZG_` y `NEON_` y `BP_FlickeringLight` en las que sobreviven. Los grupos de luz ya existen. Apagar luces además **mejora el rendimiento**.
- **Assets:** LuzGrupo/LuzParque, FX_ de polvo y papeles (Parque/Vivo), SND_Metal*, la vibración del mando (falta FFE_Golpe, ya pendiente).
- **Prueba:** unos 15 min.
- **Sprint:** S2. Se puede reemplazar el "apagón" de la demo completa por la réplica.

### 3. ★ De 62 páginas a 12 objetos de 1998 que cuentan una sola historia
- **Qué vive el jugador:** en vez de hojas sueltas, encuentra objetos de época con una línea de texto y, a veces, audio. Por ejemplo: la entrada del parque de abril de 1992 con un "8" escrito a mano; el carné de control del consultorio de Isidora; un recorte de diario genérico ("Incendio en parque de entretenciones: un cuidador desaparecido"); un cassette con la voz de Rafaela; un álbum de láminas incompleto; una boleta de la feria; la libreta de Alfredo. Cada objeto se ve en la galería y tiene un hueco en la mochila. Con los 12 se desbloquea el final verdadero. Las 4 VHS se mantienen como "capítulos".
- **Impacto:** alto. Corrige W6 (coleccionable como ruido), sube la calidad de escritura y le da al jugador una razón para explorar las zonas laterales, lo que ayuda con W3.
- **Esfuerzo:** M. Es sobre todo escritura. Hay que reasignar 12 `REC_` y desactivar 50, pero la galería y las notas ya existen. Son pocos meshes, sacados de Horror_Hospital, DetectiveOffice y SoulCity.
- **Assets:** BP_Recogible, galería, mochila, SM_NewsPaper (pack de calle) y la papelería de DetectiveOffice.
- **Prueba:** unos 30 min (recorrido de recogida).
- **Sprint:** S3 (escritura en S2, en paralelo).

---

### 4. Isidora y el parque: la regla del 8 y un final ramificado
- **Qué vive el jugador:** se descubre poco a poco que Isidora estuvo en Mundo Mágico el día de su cumpleaños número 8, el 08-04-92, el mismo día del incendio. Por eso el reloj marca 8-4-2 y el fonógrafo dice "LAS OCHO". El parque no es un lugar al que llega: es el recuerdo que la retiene. Hay **tres finales** según dos decisiones:
  - **Despertar:** sigue la voz de Luisa por la radio y vuelve al Pabellón 4.
  - **Quedarse:** sube al carrusel con la muñeca. El parque se enciende para siempre y en el hospital el monitor marca plano.
  - **Turno nocturno** (requiere los 12 objetos): encuentra a Alfredo y entiende qué pasó en el incendio. Despierta, y en la última escena es ella, adulta, quien hace el turno nocturno en el hospital.
- **Impacto:** alto. Le da sentido a los puzzles que ya existen, da rejugabilidad y deja un final que se comenta.
- **Esfuerzo:** M. Dos interacciones de decisión (radio y carrusel), tres cinemáticas cortas con cámaras `CAM_` que ya existen y texto en pantalla negra. No requiere modelos nuevos.
- **Assets:** carrusel, muñeca, Pabellón 4, cámaras CAM_ y el sistema de logros y finales.
- **Prueba:** unos 45 min (tres finales; conviene un atajo de depuración para saltar al final).
- **Sprint:** S4. Antes hay que cerrar la biblia de personajes en S2, porque los roles de Alfredo, Rafaela y Luisa deben coincidir con lo ya escrito.

### 5. Mitología chilena dentro del parque, sin caricatura
- **Idea central:** en los años 70, el parque tenía atracciones kitsch inspiradas en leyendas chilenas. Lo que era cartón pintado se vuelve real, con reglas propias:
  - **Chonchón:** se oye el "tué-tué" antes de que aparezca una amenaza. Es una regla de audio que el jugador aprende: si suena, apaga la linterna y escóndete. Sirve como aviso diegético. Esfuerzo S.
  - **Caleuche:** el tren fantasma está rebautizado como "El Caleuche", con un barco de madera en la entrada. Con niebla aparecen luces de fiesta y música alegre lejana, un señuelo que lleva hacia el peligro. Esfuerzo S–M.
  - **Imbunche:** reinterpretación de la muñeca, que tiene una costura y siempre está "de espaldas". Tratarlo como una criatura deformada y no mostrar a un niño mutilado. Esfuerzo S (cambio de texto e idle).
  - **Calchona:** balido de oveja en la casa de Alfredo y una fábula escrita en su libreta. Basta con el sonido. Esfuerzo S.
  - **Trauco:** **no recomiendo usarlo.** Su leyenda gira en torno a la violencia sexual, y en un juego con una protagonista adolescente el riesgo de leerse mal es muy alto.
- **Impacto:** alto como diferenciador de prensa y tienda ("survival horror con leyendas chilenas"). El Chonchón además es una mecánica de tensión casi gratis.
- **Assets:** SciFi_Beast06 (cuerpo del Chonchón visto de lejos o en silueta), NIE_/BP_FogArea, SND_, el tren fantasma y la muñeca.
- **Sprint:** S3 (Chonchón y Calchona en audio) y S5 o post (Caleuche).

### 6. Espacios reconocibles: hospital público y kiosko de 1998
- **Qué vive el jugador:**
  - **Hospital:** el pasillo de Urgencias se vuelve un hospital público reconocible: sillas plásticas atornilladas, cartel de "Saque número", diario mural con aviso de campaña de invierno, fichas de papel y una TV CRT en altura con la teleserie de las 20 h, que es ficticia.
  - **Set-piece de la teleserie:** al pasar, la actriz mira a cámara y dice "Isidora". Luego vuelve la estática.
  - **Parque:** el kiosko de la avenida tiene diarios con titulares genéricos, láminas, dulces de marcas inventadas y un teléfono público de monedas que suena.
- **Impacto:** medio–alto. Hace a Chile reconocible en los primeros 5 minutos, que son los que más se juegan en itch.io.
- **Esfuerzo:** S–M. Son carteles con texto (decals o planos), una TV con material de video y un sonido.
- **Assets:** Horror_Hospital (ya cargado, se usan unos 40), los kioskos KIO_, NY_Subway (teléfono e info stand) y SoulCity.
- **Prueba:** unos 15 min.
- **Sprint:** S2 (hospital y TV), S4 (kiosko y teléfono).

### 7. Hub en la plaza del carrusel, portones con candado y atajos
- **Qué vive el jugador:** el carrusel pasa a ser el **hub**, con un banco y la radio como punto de guardado. Desde ahí salen tres brazos (oeste: bodega y teatro; este: tren y noria; sur: castillo y casa). Cada brazo termina en un portón de la reja que **solo se abre desde dentro**, al estilo de Resident Evil. Al volver por el atajo se recorta la caminata de unos 140 m a unos 60 m (ya diseñado en sprint 3 para el lado este).
- **Impacto:** alto. Corrige W3 y W4 (caminatas vacías) y le da al jugador un mapa mental.
- **Esfuerzo:** M. Se reutilizan el candado y la llave, más 3 portones (mesh de reja existente) y una puerta que se abre una sola vez. **Riesgo:** las coordenadas escritas a mano en el BP del jugador (W7). Conviene usar triggers (volúmenes) en vez de coordenadas.
- **Assets:** BP de candado, rejas TER_ y BP_RuidoAzar.
- **Sprint:** S3 (este, ya diseñado) y S4 (oeste y sur).

### 8. Tres sustos que cumplen lo que el texto promete
- **Qué vive el jugador:**
  - **Noria:** una cabina baja sola y se detiene frente a Isidora. Adentro hay una niña con vestido de cumpleaños y, al alumbrarla, desaparece.
  - **Teatro:** si falla el puzzle, el telón cae, se va la luz y una silueta se levanta de la última fila (es la misma de la entrada).
  - **Bodega:** al salir con la llave, los disfraces que se mecen ya no están en su lugar.
- **Impacto:** medio–alto. Corrige W11 y pone picos de 4 en el valle plano del parque.
- **Esfuerzo:** S cada uno. Reutiliza BP_Susto* (patrón del pasillo), el silueta de BP_SustoEnergia y un timeline de luces.
- **Assets:** BP_SustoPasillo y BP_SustoEnergia como plantilla, la muñeca o un maniquí y FAR_.
- **Sprint:** S3 (teatro), S4 (noria y bodega). Se prueban de a uno, unos 10 min cada uno.

### 9. El túnel de servicio: el camino de vuelta al Pabellón 4
- **Qué vive el jugador:** bajo la casa de Alfredo, una escotilla lleva a un túnel de mantención con cañerías, carteles de la constructora y agua hasta el tobillo. A mitad de camino, los azulejos cambian de feria a hospital: el parque y el Pabellón 4 están conectados. El túnel termina en el sótano del hospital. Sirve como clímax de persecución (el payaso detrás, la linterna fallando) y como respuesta a "¿cómo vuelvo?" (W4: el viaje de ida fue un teleport de un solo sentido).
- **Impacto:** alto para la versión completa. Es el set-piece de cierre y es interior y lineal, así que **rinde mejor** en iGPU que el parque abierto.
- **Esfuerzo:** L. Importar solo unos 10 módulos del pack UndergroundTunnel (5,2 GB; **no** migrar el pack entero), hacer la transición de materiales y armar la persecución.
- **Assets:** UndergroundTunnel, Horror_Hospital (sótano), ENE_Payaso y el sonido de goteo FX_.
- **Sprint:** post v0.5.0. Para v0.5.0 basta un tramo corto (unos 40 m) como teaser después de la casa.

---

## Resumen de prioridad

| # | Propuesta | Esfuerzo | Impacto por hora | Sprint |
|---|---|---|---|---|
| 2★ | Réplica que apaga el parque | S | ●●●●● | S2 |
| 1★ | Radio AM con la voz de Luisa | S–M | ●●●●● | S2–S3 |
| 3★ | 12 objetos de 1998 en vez de 62 páginas | M | ●●●● | S3 |
| 8 | Tres sustos que cumplen el texto | S c/u | ●●●● | S3–S4 |
| 5 | Mitología chilena (Chonchón primero) | S–M | ●●●● | S3–S5 |
| 6 | Hospital público, teleserie y kiosko | S–M | ●●● | S2–S4 |
| 4 | Regla del 8 y tres finales | M | ●●● | S4 |
| 7 | Hub del carrusel y atajos | M | ●●● | S3–S4 |
| 9 | Túnel de servicio | L | ●● (por ahora) | Post |

**Dependencias:**
- La biblia de personajes (roles de Alfredo, Rafaela y Luisa, y la fecha 08-04-92) va primero, en S2, porque 1, 3 y 4 dependen de ella.
- Mover las coordenadas fijas a triggers (W7) facilita las propuestas 7 y 8. Esa tarea va a @tech.
