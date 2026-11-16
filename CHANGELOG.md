# Changelog — Mundo Mágico: Turno Nocturno

Formato basado en [Keep a Changelog](https://keepachangelog.com/es-ES/1.1.0/). Versionado `MAYOR.MENOR.PARCHE`.
La versión del juego vive en `Config/DefaultGame.ini` → `ProjectVersion`.

## [0.3.54] — 2026-11-16 · Calidad Baja más liviana (sin build)

### Cambiado
- **`CalidadExtra` en el jugador**, llamada desde `AplicarOpciones`. Solo en calidad Baja: la linterna deja de proyectar sombras, suavizado TAA en vez de TSR, tope de 30 fps, sin sombras de contacto, sin oclusión por campos de distancia y sin MegaLights. En Media o superior se restaura todo.

## [0.3.53] — 2026-11-16 · Arreglos del primer playtest (sin build)

### Corregido
- **La tecla E abría el candado en el hospital.** `ActualizarCandado` medía la distancia a un candado que en `L_Hospital` no existe y la daba por 0 ("cerca"). Ahora exige que el objeto exista. Mismo arreglo en `ActualizarCerca` para fonógrafo, reloj y caja fuerte.
- **Isidora partía con la linterna en la mano.** `LinternaMano` nunca se ocultaba. Ahora parte oculta y `RefrescarLinternaMano` la muestra solo cuando `TieneLinterna` es verdadero.

## [0.3.52] — 2026-11-16 · Volumen general en el menú (etapa E2, sin build)

### Añadido
- **Fila "General" en Configuración y en la pausa.** Baja o sube todo junto (efectos, ambiente y música). Queda entre "Dificultad" y "Volver al menú principal", que pasa a ser la fila 8.
  - Se guarda con las demás opciones (slot "Opciones"). Las partidas con opciones antiguas cargan con General al 100 %.
  - Funciona con teclado, mando y mouse (clic y hover), igual que las otras filas.
- Funciones nuevas en el jugador: `LineaGeneral`, `AjustarGeneral`, `CargarGeneral`, `RefrescarFila7` y `CfgSel7`. En `WBP_Menu`: `CfgBtn7` y `CfgRow7`.

## [0.3.51] — 2026-11-15 · Efectos y música por separado (etapa E2, sin build)

### Cambiado
- **La opción "Efectos" del menú ya no baja la música.** Antes movía el volumen general (clase `Master` con sus hijas), así que "Efectos 0 %" también silenciaba la música. Ahora controla solo los efectos y el ambiente (`Master` sin hijas, `SC_SFX` y `SC_Ambiente`); "Música" sigue controlando la música. No cambian las filas del menú ni el guardado de opciones.
- **Nuevo valor `Audio_MezclaAmbiente` en `DA_Tuning` (1,0):** multiplica el volumen del ambiente (lluvia, zumbidos) respecto a los efectos.
- **Nuevo interruptor `Latido_Sombra` en `DA_Tuning` (activo):** apaga o enciende la reacción del latido a la Sombra.

## [0.3.50] — 2026-11-15 · El latido siente a la Sombra (etapa E3, sin build)

### Cambiado
- **`BP_LatidoMiedo` ahora reacciona a `BP_AI_Sombra`.** Además de `BP_Enemigo`, busca todas las Sombras y se queda con la distancia más corta: el corazón se acelera cuando la Sombra se acerca, igual que con los otros enemigos.
  - Usa los mismos valores de `DA_Tuning`: `Latido_Alcance` y `Latido_VolMax` (con `Latido_VolMax` en 0 se apaga).
  - No se tocaron los nodos de audio (`AdjustVolume`, `SetPitchMultiplier`).

## [0.3.49] — 2026-11-15 · El payaso como amenaza real (etapa E3, sin build)

### Cambiado
- **`BP_PayasoAcecho` ya no solo mira: se acerca.** Mientras está a la vista:
  - **Si no lo miras** (producto punto < 0,5), da un paso hacia ti cada 0,25 s, de `Payaso_PasoCm` (110 cm) por paso. Se queda pegado al suelo y se oye un paso suave.
  - **Si lo miras, se congela.** Si lo miras fijo durante `Payaso_MiradaSeg`, desaparece y se oye una respiración detrás de ti.
  - **Si llega a menos de `Payaso_DistGolpe` (2,3 m), te golpea:** chillido, vibración y 1 de daño. Después desaparece hasta su próxima aparición.
  - Con el avance activo ya no huye cuando te acercas (`Payaso_DistHuye` solo cuenta si `Payaso_Avanza` está apagado). Se va al cumplir `Payaso_DuracionSeg`, que **sube de 9 a 20 s**.
- **Nuevos valores en `DA_Tuning`:** `Payaso_Avanza` (activo), `Payaso_PasoCm` (110) y `Payaso_DistGolpe` (230).

## [0.3.48] — 2026-11-15 · Las llaves y los flags se guardan en la partida (cierre de la etapa E2, sin build)

### Agregado
- **Los flags del GameInstance (llaves y eventos) ahora se guardan con la partida**, así que sobreviven a cerrar el juego y volver con **Continuar**.
  - **`BP_GI_MundoMagico`** tiene 3 funciones nuevas:
    - `GuardarFlags`: guarda los flags en el slot `Flags`, usando el SaveGame `BP_Partida`, que ahora tiene el arreglo `Flags`.
    - `CargarFlags`: los recupera.
    - `LimpiarFlags`: vacía los flags y borra ese slot.
  - **El jugador:**
    - `Guardar` llama a `GuardarFlags` justo después de guardar la partida.
    - `Cargar` llama a `CargarFlags` apenas lee la partida.
    - **Iniciar juego** (`MenuConfirmar`, opción 0) llama a `LimpiarFlags`, para que no queden llaves de una partida anterior.

## [0.3.47] — 2026-11-15 · La Sombra con linterna (etapa E3)

### Agregado
- **`BP_AI_Sombra`** (`/Game/MundoMagico/IA`): un enemigo nuevo hecho con el modelo y las animaciones de la IA del Horror Template, con lógica propia sin Behavior Tree y todos sus valores en `DA_Tuning`.
  - **Linterna en la mano** (`Haz`): un foco cálido de 22°, con polvo en el haz y sin sombras, para cuidar la iGPU. **El haz es su cono de visión.**
  - **Vista:** te ve si estás dentro del cono (22°) y a menos de 22 m, sin nada en medio. A menos de 2,5 m te ve igual, aunque estés detrás. **Escondido no te ve.**
  - **Oído:** escucha los pasos fuertes de `AC_Ruido` (correr, metal, madera) a menos de 18 m y **va a investigar** ese punto.
  - **Estados:**
    1. **Patrulla:** camina a puntos al azar a menos de 12 m de su posición inicial.
    2. **Investigar:** va al último ruido o al último lugar donde te vio, y a los 6 s vuelve a patrullar.
    3. **Persecución:** corre hacia ti. Si te pierde de vista más de 4 s, vuelve a investigar.
  - **Ataque:** a menos de 1,5 m golpea, con un intervalo de 1,5 s entre golpes, usando el mismo daño que el enemigo actual.
  - **Al verte por primera vez** suena un chillido y se registra en el Director de miedo como tipo enemigo con intensidad 30.
- **Primera Sombra en el mapa:** `ENE_Sombra_Pasillo`, en el tramo final del Pasillo de Urgencias (0, −3200), después del susto. Está en la carpeta `Hospital/IA`.
- **Nuevos valores en `DA_Tuning`:** `Sombra_Activa`, `Sombra_VistaDist`, `Sombra_VistaAngulo`, `Sombra_OidoDist`, `Sombra_PierdeSeg`, `Sombra_VelPatrulla` (140), `Sombra_VelAlerta` (240), `Sombra_VelPersecucion` (420), `Sombra_DistAtaque` y `Sombra_RadioPatrulla`.

## [0.3.46] — 2026-11-12 · Llaves con nombre, puertas cerradas y GameInstance (etapa E2, sin build)

### Agregado
- **`BP_GI_MundoMagico`** (`/Game/MundoMagico/Core`): ahora es el **GameInstance del proyecto** (`GameInstanceClass` en `DefaultEngine.ini`).
  - Guarda los **flags** (llaves y eventos) y se conservan aunque cambies de mapa (hospital → parque).
  - Tiene `AgregarFlag(Flag)`. Las puertas leen `Flags` directamente.
- **`BP_DoorLocked`** (`/Game/MundoMagico/Mundo`): una puerta del Horror Template con ícono y animación de abrir.
  - **Se abre solo si existe el flag que lleva como primera etiqueta** (por ejemplo `LlaveMorgue`).
  - Si no está, suena el picaporte y aparece "Está cerrada. Algo tiene que abrirla."
- **`BP_LlaveFlag`**: una llave que se recoge con □/E.
  - **Agrega su primera etiqueta como flag** y muestra "Encontraste: <nombre>".
  - Una llave con la etiqueta `LlaveMorgue` abre la puerta con la misma etiqueta.
- Las dos usan la interfaz de interacción que ya existía (`BPI_Interaction`), así que funcionan con el mismo botón sin tocar `UsarPuerta`.

### Pendiente
- **Los flags todavía no se guardan en la partida:** se pierden al salir del juego y volver con Continuar. Entra con `SG_Partida`.
- **Las puertas con código numérico** van con el puzle del negatoscopio (E4).

## [0.3.45] — 2026-11-12 · Ruido según el suelo (etapa E2, sin build)

### Agregado
- **`AC_Ruido`** (componente en `/Game/MundoMagico/Componentes`, agregado al jugador). En cada paso calcula **qué tan fuerte suena** según el suelo y cómo te mueves:
  - **Superficie:**

    | Superficie | Multiplicador |
    |---|---|
    | Metal, reja, hierro y la zona de los autos | 1,8 |
    | Madera, escenario y las zonas del teatro y la casa | 1,6 |
    | Charco | 1,4 |
    | Baldosa | 1,0 |
    | Grava (camino) | 0,9 |
    | Tierra, pasto y la zona de la bodega | 0,6 |
    | Alfombra | 0,3 |

  - **Forma de moverte:** caminar ×1,0, correr ×2,5 y agachado ×0,3.
  - **Ruido = movimiento × superficie.** El radio en que se oye es `Ruido_RadioBase` × ruido (1500 cm caminando sobre baldosa).
  - El componente emite un **evento de ruido de IA** (`ReportNoiseEvent`, etiqueta "Paso"). Lo va a escuchar la Sombra con linterna en la etapa E3.
  - Los pasos fuertes (ruido de 1,2 o más, como correr o pisar metal o madera) **también alertan al enemigo actual**: actualizan `RuidoPos` y `RuidoT`.
- **Nuevos valores en `DA_Tuning`:**
  - `Ruido_Activo`;
  - un multiplicador por superficie (`Ruido_Metal`, `Ruido_Madera`, `Ruido_Charco`, `Ruido_Baldosa`, `Ruido_Grava`, `Ruido_Tierra`, `Ruido_Alfombra`);
  - `Ruido_Caminar`, `Ruido_Correr` y `Ruido_Agachado`;
  - `Ruido_RadioBase`.
- **El panel `-dev` muestra además** el **ruido** del último paso (×10) y la **tensión** del Director.

## [0.3.44] — 2026-11-12 · Director de miedo (etapa E2, sin build)

### Agregado
- **`BP_DirectorMiedo`** (actor `DEV_DirectorMiedo` en los dos mapas): un solo reloj para todos los sustos.
  - Lleva una **tensión** de 0 a 100 que baja con el tiempo, a `Director_Decae` puntos por segundo.
  - Fija un **enfriamiento global** de `Director_Enfriamiento` segundos (40 por defecto) entre sustos.
  - **No repite el mismo tipo** dos veces seguidas, salvo que haya pasado el doble del enfriamiento. Los tipos son: 0 audio, 1 visual, 2 físico, 3 enemigo y 4 guion.
  - `PedirTurno(Tipo, Intensidad)` lo usan los sustos que pueden esperar. `Registrar(Tipo, Intensidad)` lo usan los sustos guionados, que siempre ocurren pero cuentan para el ritmo.
- **Qué sustos usa cada uno:**
  - **El payaso acosador** pide turno antes de aparecer, como tipo enemigo con intensidad 25. Si el Director lo niega, reintenta a los 8 s.
  - **Isidora** pide turno antes de aparecer, como tipo visual con intensidad 15.
  - **El teatro y la noria** registran su susto como tipo guion con intensidad 40. Así, después de uno de ellos, el payaso o Isidora esperan su turno.
- **Nuevos valores en `DA_Tuning`:** `Director_Activo`, `Director_Enfriamiento` (40 s) y `Director_Decae` (1,5 por segundo).

## [0.3.43] — 2026-11-12 · Cámara en 1ª persona como modo principal (etapa E2, sin build)

### Cambiado
- **El juego arranca en 1ª persona** (decisión del CTO). La 3ª persona sigue como opción con el botón de cambiar vista que ya existía (`CambiarCamara`).
  - `BP_GestorNivel.AplicarCamara` se ejecuta 0,4 s después de cargar cada mapa. Si `DA_Tuning.Camara_PrimeraPorDefecto` está activo, cambia a 1ª persona y quita el retraso de cámara para que no se sienta flotante.
  - **Altura de los ojos:** en 1ª persona la cámara sube de 25 cm sobre el centro de la cápsula (la altura del pecho) a 57 cm, con 10 cm hacia adelante (`SocketOffset` = 10, 0, 32).
  - **Nuevo valor en `DA_Tuning`:** `Camara_PrimeraPorDefecto` (activo). Si lo desmarcas, el juego vuelve a arrancar en 3ª persona.

## [0.3.42] — 2026-11-12 · Hospital y parque en dos mapas (etapa E1 de la arquitectura en 3 niveles). La build del 12-11 no trae la transición TV/VHS

### Cambiado
- **El mapa único `L_Pabellon4` se separó en `L_Hospital` y `L_Parque`.** `L_Pabellon4` queda congelado como respaldo.
  - **`L_Hospital`:** sala, pasillo de urgencias y patio de salida. Tiene 432 actores; se le quitaron 1 404 del parque, junto con el terreno, el bosque y las rejas.
  - **`L_Parque`:** entrada, avenida y atracciones. Tiene 1 427 actores; se le quitaron 409 del hospital. El `PlayerStart` quedó en la reja de entrada (0, −4300).
  - Las coordenadas no cambian, así que todas las funciones del jugador siguen funcionando igual.
- **Al parque solo se llega con el glitch** (`BP_GestorNivel`, actor `DEV_GestorNivel` en los dos mapas):
  - En el hospital, cuando el glitch de salida te lleva al otro lado, se guarda la partida, se crea el aviso `Transicion` y se abre `L_Parque`.
  - En el parque, se lee ese aviso, la partida arranca sola con la pantalla "MUNDO MÁGICO" y te deja en la reja de entrada.
  - Si abres el parque sin venir del glitch (y sin `-ir=`), te devuelve al hospital, que es donde está el menú.
  - **Continuar** con una partida que ya salió del hospital te lleva directo al parque.
- **"Volver al menú"** abre `L_Hospital`.
- **Configuración y herramientas:**
  - `GameDefaultMap` y `EditorStartupMap` ahora son `L_Hospital`.
  - La build cocina los dos mapas.
  - `Abrir_MundoMagico*.ps1` abre `L_Hospital`.
  - `Probar.bat` abre el mapa que corresponde a cada zona.

### Agregado (12-11, después de la build)
- **Transición TV → VHS entre los dos mapas** (`WBP_Transicion`, a cargo de `BP_GestorNivel`):
  - **Salida del hospital, 1,6 s:** la imagen se encoge a una línea blanca y luego a un punto que se apaga, con un "tuk" del tubo y un pitido agudo que se va (`SFX_TV_Apagar`). Después viene el negro y se abre `L_Parque`.
  - **Llegada al parque, 2,4 s:** en negro aparece "▶ PLAY 03:17" parpadeando y un grano VHS que tiembla, con el clac del mecanismo, el motor y el soplido de la cinta (`SFX_VHS_Play`, sin estática de TV). Después el parque aparece con un fundido.
  - Los sonidos se componen con `herramientas/componer_transicion_tv.py` y se importan con `importar_transicion_tv.py`.
- **`DEV_PisoGlitch`:** un piso invisible en el punto de llegada del glitch dentro de `L_Hospital`, para que el jugador no caiga mientras la TV se apaga.

## [0.3.41] — 2026-11-11 · Kit de afinación del CTO (fase 1 del plan de mejora rápida, sin build)

### Agregado
- **`DA_Tuning`** (`/Game/MundoMagico/Datos`, clase `BP_AfinacionDatos`): 18 valores e interruptores que el CTO cambia con doble clic, sin tocar Blueprints.
  - Payaso acosador: espera, frecuencia, veces, distancias, mirada y duración.
  - Isidora: distancias.
  - Latido: alcance y volumen.
  - Interruptores de los sustos del teatro y la noria.
  - Los leen en `BeginPlay` `BP_PayasoAcecho`, `BP_IsidoraAparicion`, `BP_LatidoMiedo`, `BP_SustoTeatro` y `BP_SustoNoria`. El cook incluye siempre `/Game/MundoMagico/Datos` (`DirectoriesToAlwaysCook`).
- **Saltos de prueba `-ir=<zona>`** (`BP_KitPrueba`, actor `DEV_KitPrueba` en `Juego/Dev`):
  - Al abrir el `.exe` con `-ir=teatro`, el juego arranca solo y te deja en la zona.
  - Zonas: hospital, pasillo, parque, teatro, noria, bodega y autos. Usan los `RSP_*` con la etiqueta `Ir_<zona>`.
- **Panel `-dev`:** un texto arriba a la izquierda (`DevTexto` en `WBP_HUD`) con fps, posición X/Y y batería. Se actualiza cada 0,5 s.
- **`Probar.bat`** en la raíz del repo: un menú del 1 al 8 que abre la build más reciente en la zona elegida, con el panel encendido.
- **`GUIA_TUNING.md`**: qué valor hace qué, su rango seguro, cómo probar y qué se ajusta a mano en el editor.

## [0.3.40] — 2026-11-11 · Hospital AAA, parte 1 (sin build: entra en la build del 12-11)

### Agregado
- **Balizas de emergencia giratorias** (`LampsPack`, `BP_Lamp_Flat_Crazy`), colgadas del techo y teñidas de rojo (luz roja de 750 de radio, sin sombras para cuidar el rendimiento, con dispersión volumétrica). Materiales nuevos: `MI_Baliza_Vidrio_Rojo` y `MI_Baliza_Brillo_Rojo` en `/Game/MundoMagico/Luces`.
  - Hay una en el pasillo de salida de la sala, encendida desde el inicio.
  - Hay tres en el Pasillo de Urgencias, **apagadas hasta el susto**: aparecen con la etiqueta `SangrePasillo` en el destello blanco, como una alarma que se dispara después de que pasa la criatura.
- **Utilería macabra** (`TA_CorpsesPackSample`): hay un cuerpo en la sala, otro en el pasillo de salida y otro en el Pasillo de Urgencias. El de Urgencias aparece solo después del susto.
- **Papeles físicos** (`SM_Notes`):
  - Hay notas con sangre en las paredes de la sala y de los dos pasillos; la de Urgencias aparece después del susto.
  - Hay seis papeles arrugados en el suelo.
- **Alarma de emergencia** (`SFX_Alarma`, compuesta con `herramientas/componer_alarma.py` e importada con `importar_alarma.py`): es una sirena lenta de dos tonos que suena por un parlante roto. Al final se corta la corriente, el tono cae y se apaga. Suena en `BP_SustoPasillo` justo cuando se encienden las balizas, después del destello.
- **Las 62 páginas recogibles cambian a papeles de terror** (`SM_Notes`): ahora usan papel envejecido, viejo, quemado, con esquina rota y con sangre, en lugar del papel limpio de oficina. Cada una tiene un giro distinto en el suelo.

- **Sustos reales en el teatro y la noria** (`BP_SustoTeatro`, `BP_SustoNoria`, en la carpeta `Parque/Sustos`). Cada uno se activa una sola vez, cuando el jugador entra a la zona:
  1. Baja el sonido y se escucha una respiración.
  2. Se enciende un foco que deja ver a la criatura: de pie en el escenario, o arriba de la noria, en la cabina más alta.
  3. El foco se apaga y la criatura desaparece.
  4. A los 1,1 s aparece **frente al jugador** con el ataque, el chillido y la vibración del control. Después hay un corte a negro.
  5. Sale un mensaje en pantalla.

- **Payaso acosador** (`BP_PayasoAcecho`, actor `ENE_PayasoAcecho` en `Parque/Sustos`): aparece a lo lejos en el parque, fuera de la persecución.
  - **Cuándo aparece:**
    - La primera vez, a los 45 s de juego como mínimo. Después, cada 40–80 s.
    - Como máximo 6 veces por partida, y solo en el parque (y < −4700).
    - Nunca durante la persecución ni después del final.
  - **Dónde aparece:** a 22–32 m, dentro de ±30° de donde mira la cámara, y solo si la línea de visión está despejada. Se para sobre el suelo real, te mira y queda quieto (`anim_idle_1`).
  - **Cuándo desaparece:**
    - Si te acercas a menos de 15 m.
    - Si lo miras fijo durante 1 s.
    - Si pasan 9 s.
  - Al desaparecer, suena una **respiración detrás del jugador**.

- La utilería nueva no tiene colisión, así que no estorba al caminar ni cambia el sonido de los pasos. Todo está en la carpeta `Hospital/AAA` del Outliner.

### Cambiado (playtest del CTO: "no noté el latido ni a Isidora")
- **Isidora** (`BP_IsidoraAparicion`, en el teatro, la bodega y la noria):
  - Ahora aparece entre **10 y 35 m**; antes, entre 11 y 23 m.
  - Al aparecer, suena un **glitch de radio** desde donde está y el control da una vibración suave.
  - Desaparece cuando te acercas a menos de 9 m, con una respiración.
- **Latido del miedo** (`BP_LatidoMiedo`):
  - Ahora también se acelera cuando **el payaso acosador está en pantalla**; antes solo reaccionaba al enemigo de la persecución.
  - Volumen máximo de 1,0 (antes 0,85) y alcance de 28 m (antes 25 m).
- **Descompresión tras el glitch:** el payaso acosador no aparece hasta **60 s después de llegar al parque**. Antes podía aparecer apenas llegabas, porque contaba desde el inicio de la partida.

## [0.3.39] — 2026-11-11 · Reverb por zona

### Agregado
- **Efectos de reverb** (`/Game/MundoMagico/Audio/Mezcla`), creados con `herramientas/crear_reverbs.py`:
  - `RV_Sala`: cuarto chico, seco, con decay de 0,75 s.
  - `RV_Pasillo`: pasillo largo de baldosa, con decay de 2,6 s y reflexiones marcadas.
  - `RV_Exterior`: parque abierto, con cola leve.
- **`AplicarReverb`** (BP del jugador): se llama al cambiar de zona, en `ActualizarZona` después de `Guardar`. Aplica el reverb que corresponde con un fundido de 1,5 s:
  - Pasillo de Urgencias: `RV_Pasillo`.
  - Pabellón 4, Teatro, Casa y Bodega: `RV_Sala`.
  - Parque: `RV_Exterior`.
- El reverb afecta los pasos, la criatura, las puertas y todo el sonido del juego, así que cada lugar suena distinto.

## [0.3.38] — 2026-11-11 · Puertas con pantalla de carga (build del día, junto con la 0.3.37)

### Corregido
- **El sonido de "abrir puerta" sonaba al recoger cosas.** `UsarPuerta` toma el interactuable más cercano a menos de 2,3 m, y cualquiera que no fuera clóset, llave o puerta con llave (por ejemplo los 85 `BP_Recogible`) reproducía `SW_Open_Door`. Ahora solo suena si el objeto es una puerta real (`CastToBP_Creepy_Door`).

### Agregado
- **Pantalla de carga al estilo PlayStation** (`WBP_HUD`):
  - `CargaFondo`: negro a pantalla completa (ZOrder 30).
  - `CargaTitulo`: el nombre del lugar en Special Elite 46.
  - `CargaSub`: "CARGANDO" abajo a la derecha.
- **`MostrarCarga(Texto)` y `OcultarCarga`** (BP del jugador): muestran la pantalla 2,4 s con los controles bloqueados. Se usan:
  - al abrir la puerta de la sala al pasillo (`BP_Creepy_Door`): "PASILLO DE URGENCIAS";
  - al llegar al parque después del glitch (`GlitchLlegar`): "MUNDO MÁGICO".
- La silueta del sombrero ahora espera 3,2 s (antes 1,8), para aparecer después de la pantalla de carga del parque y no detrás de ella.

## [0.3.37] — 2026-11-11 · Pasos según el suelo real

### Agregado
- **Packs de Fab** agregados por el CTO: **Essential Footsteps** (botas en 10 superficies), **AutoFootsteps** (agua y madera) y **HeartBeats** (latidos de terror, médicos y de acción, para usar después).
- **`PasoSuelo2` y `TocarPaso`** (BP del jugador): cada paso mira **la malla que pisa Isidora** (`GetMovementBaseActor` → `StaticMesh`) y elige el sonido de bota, caminando o corriendo:

  | Pisa… | Suena |
  |---|---|
  | charco | barro mojado |
  | rejas, hierro, metal, o la zona de autos chocones | metal |
  | madera o escenario, o la zona del teatro o la casa | madera |
  | el camino del parque | grava |
  | terreno o pasto, o la zona de la bodega | tierra |
  | cualquier otra cosa (hospital) | concreto |

- `Paso` ahora llama a `PasoSuelo2`. El `PasoSuelo` antiguo, que elegía solo según la zona, queda sin uso.

### Nota
- Los packs de Fab no entran en `Versionar_Unreal.ps1`, así que viven solo en este PC. Para reconstruir en otro equipo hay que agregarlos desde Fab.

## [0.3.36] — 2026-11-11 · Slice AAA, Fase 1: linterna y cámara (build del día)

### Agregado
- **`IES_Linterna`** (`/Game/MundoMagico/Luces`): perfil de luz IES generado por script (`herramientas/crear_ies_linterna.py`), con centro caliente, anillo del reflector y caída suave. Se importa con `importar_ies.py`, que también lo asigna a la linterna.
- **`ParpadeoLinterna`** (BP del jugador): con la batería bajo 20 %, la linterna baja de intensidad y parpadea. Mientras menos batería, más seguido. Se llama desde `ActualizarBateria`.
- **Recursos de Fab**: revisión de los 114 de la biblioteca, con un plan de uso por lotes en `auditoria/fab_recursos_slice.md`.

### Cambiado
- **Linterna**: 4700 K, cálida como una incandescente de 1998 (antes 6300 K). Conos interior y exterior de 12° y 30° (antes 9° y 34°). Dispersión volumétrica de 3,5 (antes 2), para que el haz se vea en la niebla.
- **Cámara al hombro**, estilo RE2 remake: brazo de 190 cm (antes 270) y desplazamiento lateral de 60 y vertical de 55 (antes 50 y 65). Cambia en el arranque, en `CambiarCamara` y en `CamaraDinamica`, que mantiene el balanceo al caminar.

## [0.3.35] — 2026-11-11 · Correcciones de la prueba del CTO (build única del día)

Responde a lo que el CTO vio jugando la v0.3.34 con `auditoria/checklist_v0.3.34.md`.

### Cambiado
- **Visibilidad**: era el problema principal, porque no dejaba ver casi nada de lo nuevo.
  - Exposición: `autoExposureMin/MaxBrightness` baja de 1,1 a 0,4 (la imagen se ve más clara).
  - Luna: de 0,5 a 1,2 lux.
  - SkyLight: de 0,16 a 0,3.
- **Faroles con halo**:
  - 49 luces del parque con `VolumetricScatteringIntensity` 3: el haz se ve en la niebla.
  - `BP_FarolMuere`: de 7 a 18 cd, radio 10 m y dispersión volumétrica 5, para que se note cuando se apagan.
  - Faroles con parpadeo: `LightBrightness` de 1000 a 2500.
- **Rocas**: seguían claras porque la textura del pack es casi blanca. BaseColor ×0,22 (antes 0,55).
- **Linterna**: gasta 0,22 % por segundo en vez de 0,4. Dura unos 7,5 min en vez de 4.
- **Reaparecer**: nueva función `PuntoRespawn`. `Revivir` te lleva al punto fijo **más cercano** de los 10 `TargetPoint` con tag `Respawn` (carpeta `Juego/Respawn`): sala, pasillo, entrada, avenida, avenida al fondo, noria, teatro, bodega, casa y autos chocones. Antes el punto de control se actualizaba también al cambiar de objetivo, y te dejaba en lugares raros.
- **Silueta del sombrero más terrorífica**:
  - más alta y delgada (cuerpo con escala 2,3 × 3,2);
  - sombrero y ala más arriba;
  - **dos ojos rojos encendidos** (`OjoI`/`OjoD` con `MI_LedRojo`).
  - `ENT_SustoEnergia` se volvió a colocar para que tome los cambios.

### Por revisar
- El glitch de salida del hospital no se activó en la prueba. Es probable que la partida viniera de **Continuar**, porque `SalidaHecha` se guarda. Hay que probar con **Iniciar juego**.

## [0.3.34] — 2026-11-11 · Teaser "El último turno" (del atardecer a la noche)

### Agregado
- **`BP_TeaserDirector`**: en modo `-teaser` ahora hace un ciclo de atardecer a noche en los primeros 18 s.
  - Luz direccional: el sol baja de −35° a +5° y pasa de naranja a rojo, mientras su intensidad cae de 8 a 0. Después vuelve la luna (0,5 lux, azul).
  - Luz de cielo: baja de 1,6 a 0,16.
  - Recién de noche aparecen las estrellas (tag `Estrellas`) y la cúpula del cielo (tag `CieloDomo`).
  - La silueta del sombrero aparece solo cuando ya es de noche.
  - Las 7 tomas parten a través de la reja, mirando hacia el atardecer.
- **Banda sonora compuesta por script** (`herramientas/componer_teaser_vals.py` y la versión corta `_corto.py`):
  - un vals de feria (cajita y organillo) que se desafina, se ralentiza y se apaga como cinta estirada;
  - un dron grave y un latido que se acelera;
  - un segundo de silencio total y un golpe con chirrido metálico;
  - notas finales de cajita desafinada sobre el título.
- **`herramientas/montar_teaser_final.ps1`**: arma el corte final de 39 s con dos textos, "EL PARQUE CERRÓ HACE AÑOS." y "PERO NADIE LE AVISÓ A ELLA.".
- **Video**: `Teaser/Teaser_ElUltimoTurno.mp4`, más una copia liviana `_movil.mp4`. No se versionan.

### Pendiente
- El cielo sigue negro durante el atardecer: solo el suelo y los árboles toman la luz dorada.
- En la grabación el juego se cerró una toma antes, así que la toma de la silueta se tomó del primer teaser.

## [0.3.31] — 2026-11-10 · Luz de luna (build de prueba)

### Corregido
- **La luna no iluminaba.** `DirectionalLight_0` tenía `LightColor` en (0,0,0), así que aportaba cero luz y lejos de los faroles todo quedaba negro. Probablemente quedó así en una pasada de colores anterior.
- Ahora tiene el azul frío original de la especificación del parque (0x5577BB), con intensidad 0,5 lux y sin temperatura. Las sombras siguen activas, así que no entra al hospital.

### Notas
- Falta verificarla jugando: la captura automática no pudo grabar la ventana del juego.
- Si queda muy clara o muy oscura, se ajusta con un solo número (`Intensity`).

## [0.3.30] — 2026-11-10 · Audio espacial (build de prueba)

### Agregado
- **`ATT_Puntual`** (SoundAttenuation), creado con `herramientas/audio_espacial.py`.
  - Esfera de 2,5 m más caída natural de 35 m.
  - Filtro de agudos por distancia (a 35 m solo pasan hasta 2,5 kHz), para que lo lejano suene apagado.
  - Dentro de 1,2 m deja de ser espacial, así un sonido pegado al jugador no salta de oído.
- **Asignado a 34 sonidos que salen de un lugar concreto**:
  - metales que crujen y lámparas que se sacuden;
  - chispas y vapor;
  - golpes, chillido y carrera de la criatura, vidrio y pedestal;
  - puertas, llave y cajas de madera.
- No se tocan los ambientes generales (viento, lluvia, música) ni los pasos del jugador, que siguen sonando en todas partes. Los sonidos 2D ignoran la atenuación.

## [0.3.29] — 2026-11-10 · Vibración del control (build de prueba)

### Agregado
- **Efectos de vibración** en `/Game/MundoMagico/FX`, creados con `herramientas/crear_vibraciones.py`. Las curvas se pusieron por MCP.
  - `FFE_Golpe`: 0,35 s, fuerte, con todos los motores.
  - `FFE_Susto`: 0,8 s, sube de golpe y se va apagando, con todos los motores.
  - `FFE_Trueno`: 1,6 s, retumbo grave, solo con los motores grandes.
- **Dónde vibra** (`ClientPlayForceFeedback` sobre el PlayerController):
  - Al recibir un golpe (`RecibirGolpe`).
  - En cada trueno (`Trueno`).
  - En `MostrarSusto`.
  - Cuando aparece la silueta del sombrero (`BP_SustoEnergia`).
  - Cuando aparece la criatura del pasillo (`BP_SustoPasillo`).

## [0.3.28] — 2026-11-10 · Guardado al entrar a cada zona (build de prueba)

### Agregado
- **Punto de control por zona.** Al entrar a una zona nueva (las mismas que muestran su nombre en pantalla: Pabellón 4, Pasillo de Urgencias, Parque, Bodega, Teatro, Autos Chocones y Casa del Parque), `ActualizarZona` guarda tu posición en `PosControl` y llama a `Guardar`.
- Si mueres, `Revivir` te devuelve a la entrada de la última zona. **Continuar** en el menú también parte desde ahí.
- El guardado automático cada 30 s y al pausar sigue igual.

## [0.3.27] — 2026-11-10 · Silencio antes del susto (build de prueba)

### Agregado
- **`SM_Silencio`** (SoundMix): baja SC_Ambiente y Music a 0,03. Entra en 0,35 s y sale en 2,5 s. Los efectos (SC_SFX) no se tocan. Se crea con `herramientas/crear_mezcla_silencio.py`.

### Cambiado
- **`BP_SustoEnergia`** (silueta del sombrero): al dispararse, el ambiente y la música se apagan. La espera antes de mostrar la silueta sube de 1,2 a 1,8 s, para que se note el vacío. El sonido vuelve poco a poco cuando la silueta desaparece.
- **`BP_SustoPasillo`**: el silencio entra con el mensaje "Las luces empiezan a apagarse" y dura todo el susto. El ambiente vuelve con el último sonido.

## [0.3.26] — 2026-11-10 · Flashbacks sobre negro (build de prueba)

### Cambiado
- **Flashbacks de páginas** (las páginas 1, 5, 8, 12, 14 y 17 muestran burbujas de diálogo).
  - Antes aparecían sobre el juego en movimiento, con el fondo y la lluvia. Ahora la pantalla queda en negro durante todo el flashback y el juego vuelve cuando termina.
  - `WBP_HUD`: imagen nueva `FBNegro`, negra y a pantalla completa, con ZOrder 5. Las burbujas `FBBurbuja` y `FBOnoma` pasan a ZOrder 10, para quedar encima.
  - `BP_ThirdPersonCharacter`: `AntesDePagina` sube la opacidad de `FBNegro` a 1 y `SiguienteLineaFB` la baja a 0 al terminar.

## [0.3.25] — 2026-11-10 · Rocas que brillaban en la noche (build de prueba)

### Corregido
- **`M_RocksStylized`** (pack FreePack-RocksStylized, usado por las 34 rocas del parque).
  - Antes era un material sin iluminación (Unlit) con la textura en Emissive, así que las rocas brillaban blancas en plena noche. Se vio en la grabación del teaser.
  - Ahora es un material iluminado normal (DefaultLit): la textura × 0,55 va a BaseColor y la rugosidad es 0,92.
  - Ese pack no está en la lista de `Versionar_Unreal.ps1`, así que este cambio vive solo en el proyecto local.

### Notas
- No se pudo verificar con captura, porque Windows negó el acceso a la duplicación de escritorio en ese momento. Hay que confirmarlo jugando.
- La iluminación general del juego empaquetado (DX11) se ve muy oscura lejos de las luces. Se dejó así a propósito; queda pendiente de decidir.

## [0.3.24] — 2026-11-10 · Modo teaser y primer corte del teaser

### Agregado
- **`BP_TeaserDirector`** (`TEASER_Director` en el mapa)
  - Solo se activa al abrir el juego con `-teaser`. Entonces quita los widgets, despausa, toma la cámara (CineCamera de 24 mm, sin desenfoque) y muestra la silueta y las apariciones de Isidora.
  - Recorre 7 tomas definidas en los arreglos `IniPos`, `FinPos`, `Mira` y `Dur`, con un movimiento suave. Al terminar, cierra el juego.
- **`herramientas/montar_teaser.ps1`**: arma el teaser con FFmpeg a partir de la grabación.
  - Elige los mejores tramos y les aplica etalonaje, franjas de cine, fundidos y textos en Special Elite.
  - Agrega la tarjeta final "MUNDO MÁGICO / TURNO NOCTURNO / PRÓXIMAMENTE", la música de terror y el latido acelerado sobre la silueta.
- **Primer corte**: `Teaser/Teaser_MundoMagico.mp4`, de 25 s y 1280×720. No se versiona.

### Cómo grabar
- Ejecuta `MundoMagico.exe -teaser -windowed -ResX=1280 -ResY=720 -WinX=60 -WinY=60 -nosound`.
- Graba con FFmpeg `ddagrab` en la zona de la ventana. `gdigrab` deja la imagen negra porque no captura DirectX.

## [0.3.23] — 2026-11-10 · Isidora fantasma (build de prueba)

### Agregado
- **`M_Isi_Fantasma`**: material translúcido sin iluminación, de un blanco frío que brilla más en los bordes del cuerpo (Fresnel), con opacidad de 0,12 a 0,65.
- **`BP_IsidoraAparicion`**
  - Usa SKM_Isidora3 con las 11 ranuras de material en `M_Isi_Fantasma`, sin sombra ni colisión, en reposo lento (MM_Idle a 0,35).
  - Empieza invisible.
  - Aparece cuando el jugador está a entre 11 y 23 m, y se gira para mirarlo de frente.
  - Si el jugador se acerca a menos de 9,5 m, desaparece con una respiración y no vuelve.
- **3 apariciones** (`Parque/Vivo/Isidora`): `APA_IsidoraTeatro` (−5000, −11000), `APA_IsidoraBodega` (−6500, −5900) y `APA_IsidoraNoria` (2500, −9800).

## [0.3.22] — 2026-11-10 · Latido del miedo (build de prueba)

### Agregado
- **`SFX_Latido`**: latido grave (lub-dub) de 1 s, sin ruido y en loop exacto, con clase SC_SFX. Se compone con `herramientas/componer_latido.py` y se importa con `importar_latido.py`.
- **`BP_LatidoMiedo`** (`SND_LatidoMiedo`, en `Parque/Vivo`)
  - Cada 0,15 s mide la distancia a la amenaza más cercana: payaso, oso Willy o león.
  - A más de 25 m está en silencio. A menos de 3 m suena al máximo.
  - El volumen sube en curva (t²) y el ritmo se acelera de unos 51 a 105 latidos por minuto (pitch de 0,85 a 1,75).
  - No es espacial: se siente dentro de la cabeza.

## [0.3.21] — 2026-11-10 · Niebla en la avenida y faroles que mueren (build de prueba)

### Agregado
- **`BP_FarolMuere`** (Blueprint nuevo)
  - Es una luz cálida de 7 cd, 3100 K y sin sombras.
  - Cuando el jugador se acerca a menos de 7 m, parpadea, se apaga y queda a oscuras unos 9 s. Después vuelve con un parpadeo y se rearma 8 s más tarde.
  - Revisa la distancia cada 0,2 s.
- **8 faroles que se apagan al pasar** (`FAR_Muere*`) y **4 faroles con parpadeo verdoso** (`FAR_Parpadeo*`, `BP_FlickeringLight`), puestos en la cabeza de 12 de los 20 faroles que antes no tenían luz (carpeta `Parque/Faroles`). Los otros 8 siguen apagados.
- **7 bancos de niebla baja en la avenida y sus costados** (`NIE_Avenida*`, `BP_FogArea`). Usan el mismo color frío que `NIE_Jardin`, con densidad 0,45.

### Notas técnicas
- La lógica se escribió con `write_graph_dsl`, que solo funciona con el editor en inglés. Para usarla hay que abrir Unreal con `-culture=en`.

## [0.3.20] — 2026-11-10 · Portada estilo afiche de cine (build de prueba)

### Cambiado
- **Portada del menú** (`T_PortadaHTML`, con el mismo asset y otra imagen).
  - **Fondo:** el arte de la novela gráfica (`portadamuestra.jpg`), desaturado, oscurecido y con tinte frío.
  - **Encuadre:** degradado negro a la izquierda, viñeta y franjas negras de cine arriba y abajo.
  - **Título:** "MUNDO MÁGICO" en Special Elite, con una línea roja, "TURNO NOCTURNO" espaciado y la frase "El parque cerró hace años. Alguien sigue de turno."
  - **Créditos:** un bloque al pie, al estilo de un afiche.
  - **Herramientas:** se genera con `herramientas/componer_portada_cine.py` (Pillow) y se importa sin abrir Unreal con `importar_portada_cine.py`. La imagen anterior sigue en `Fuentes/UI/T_PortadaHTML.png`.
- **`WBP_Menu`**
  - Los 3 botones pasan del recuadro central a la izquierda, bajo el título, alineados a la izquierda.
  - La lluvia del menú baja a 0,12 de opacidad.

## [0.3.19] — 2026-11-10 · Manchas en muros del parque y telas reales (build de prueba)

### Agregado
- **32 decals en paredes del parque** (`DEC_Muro_Parque*`, carpeta `Parque/Decals`).
  - Hay grafitis, óxido chorreado, filtraciones, grunge y concreto dañado.
  - Se colocaron con trazados laterales desde la avenida (y −4.700 a −12.700), con alturas, tamaños y giros variados.

### Cambiado
- **`MI_Bata` y `MI_Colchon`** pasaron de `BasicShapeMaterial` a `M_Triplanar`, con tela de Horror_Hospital (funda y colcha sucias). Se conservó el color original.
- `MI_Charco` (agua brillante) y `MI_Hoja` (hoja verde) se dejaron planos a propósito.

## [0.3.18] — 2026-11-10 · Decals en el parque y silueta con cuerpo real (build de prueba)

### Agregado
- **47 decals en el suelo del parque** (carpeta `Parque/Decals`, `DEC_Parque*`).
  - Usan suciedad, grietas, daño de concreto, filtraciones, hundimientos y grunge de Horror_Hospital.
  - Están repartidos en una grilla irregular, desde la entrada hasta y −12.700.
  - Se colocaron con trazado al suelo, saltándose los techos y las atracciones.
- **`MI_Silueta`**: material negro propio para la silueta, copiado de `MI_Oscuro`.

### Cambiado
- **`BP_SustoEnergia`** (la silueta del sombrero en la entrada) tiene un cuerpo real.
  - Nuevo componente `Cuerpo`: SKM_Manny en negro, escala 2,75, con respiración lenta (MM_Idle a 0,6).
  - Las esferas del torso, los brazos, las piernas y la cabeza quedan ocultas en el juego. El sombrero, el resplandor y la luz roja siguen igual.
- **`ENT_SustoEnergia`** se volvió a colocar en la misma posición, para que tome el componente nuevo.

## [0.3.17] — 2026-11-10 · Materiales con suciedad (build de prueba)

### Cambiado
- **`M_Triplanar`**, base de 17 materiales del parque: se le agregó una capa de suciedad.
  - Usa `WorldAlignedTexture` con `T_Dirt_World_Align_01_A_D` (Horror_Hospital) en dos escalas, 300 y 1700, para que no se note la repetición.
  - Se mezcla con el color base mediante el parámetro `Suciedad` (0,65).
  - La rugosidad sube hacia 0,92, para quitar el brillo plástico.
  - Parámetro nuevo: `EscalaSuciedad`.
- **5 materiales que eran color plano** (`BasicShapeMaterial`) pasan a `M_Triplanar` con texturas reales:
  - `MI_Concreto` → `T_Soul_Concrete_Generic01`
  - `MI_Escombro` → `T_Cave_Floor`
  - `MI_Azulejo` y `MI_Marco_Azul` → `T_PaintPeelOff`
  - `MI_Oscuro` → `T_Slums_Wall`

## [0.3.16] — 2026-11-10 · Volumen general funcional (build de prueba)

### Corregido
- Ningún sonido queda sin Sound Class: 267 assets (SoundWave y SoundCue) recibieron una.
  - Loops, viento, lluvia, truenos y metal → `SC_Ambiente`.
  - El resto → `SC_SFX`.
- `DefaultEngine.ini`: `[/Script/Engine.AudioSettings] DefaultSoundClassName=SC_SFX`, por si aparece un sonido nuevo sin clase.
- Ahora el deslizador de Volumen general (override de Master con hijos) controla todo el audio.
- Nota: los pasos pasan a sonar con `SC_SFX` (1,25) y los truenos con `SC_Ambiente` (0,6).

## [0.3.15] — 2026-11-10 · Encontrado el ruido de "TV sin señal" (build de prueba)

### Corregido
- **Causa del ruido:** los sonidos de lluvia `Rain_Heavy_Ext01` (SoulCity, que usa `Rain_Heavy_Ext_Cue` en el personaje) y `SFX_Lluvia`. Se encontró por descarte:
  - 0.3.13: todo mudo menos pasos y truenos → sin ruido.
  - 0.3.14: mudos solo 17 ruidos blancos → sin ruido.
  - 0.3.15: mudas solo las 2 lluvias → sin ruido.
- Las dos lluvias quedan con volumen de asset 0. La lluvia se sigue viendo (`NS_Lluvia`) y los truenos se mantienen.
- Los demás sonidos volvieron a su volumen original (respaldo en `herramientas/volumenes_originales.json`).

### Cambiado (0.3.12)
- `t.MaxFPS=30` en `DefaultEngine.ini`: es la mejora aprobada "30 fps iGPU".
- Se devolvieron las respiraciones, `SFX_VHS`, `SFX_T_Glitch`, `MUS_Persecucion` y la lluvia exterior a 0,25, porque no eran la causa.

### Pendiente
- El deslizador de Volumen general no afecta a los sonidos sin Sound Class (pasos, truenos, lluvia y SoulCity). Hay que asignarles `SC_SFX` o `SC_Ambiente`.
- Si se quiere volver a escuchar lluvia, buscar una lluvia más suave y limpia.

## [0.3.11] — 2026-11-10 · Música aislada (build de diagnóstico)

### Cambiado
- `MUS_Menu` y `MUS_Persecucion` con volumen de asset 0. Así queda igual que con el deslizador de Música en 0, que es cuando desaparece la estática.
- Nueva `MUS_Ambiente_Terror` (copia de la pista compuesta, volumen 0,5, loop), que suena como **una sola instancia 2D** en `AmbientSound_2` (sin espacialización, volumen 0,4).
- Diagnóstico: si la estática vuelve, el problema está en la pista; si no vuelve, estaba en cómo se disparaban las músicas viejas.

## [0.3.10] — 2026-11-10 · Música de terror nueva (build de prueba)

### Cambiado
- **`MUS_Menu`:** ahora usa `Musica_Terror_Menu.wav` (92 s en loop, volumen 0,6, sin ruido), generada con `herramientas/componer_musica_terror.py`. Tiene un dron grave disonante, un sub-grave, un acorde frío que respira, una cajita musical de feria desafinada con eco y un latido lejano. El CTO identificó la música anterior como el ruido de "TV sin señal" (al bajarla a 0, desapareció).
- **`AmbientSound_2`** (la música en el parque): volumen de 0,55 a 0,3.

## [0.3.9] — 2026-11-09 · Cortes de audio (build de prueba)

### Corregido
- **Cortes de audio que sonaban a estática** en la portada y el parque. En `DefaultEngine.ini` se cambió `AudioNumBuffersToEnqueue` de 1 a 4 y `AudioCallbackBufferFrameSize` de 1024 a 2048. También quedan `AudioMaxChannels=32` y `AudioNumSourceWorkers=2`, para que el CPU tenga margen.

## [0.3.8] — 2026-11-09 · Respiraciones silenciadas (build de prueba)

### Quitado
- **`SFX_Respiracion` y `SFX_T_Respiracion`:** volumen del asset en 0. Según el CTO, sonaban a estática de TV. La mecánica de contener la respiración sigue activa, pero sin sonido; está pendiente reemplazarla por una respiración grabada.

## [0.3.7] — 2026-11-09 · Sin estática a ratos (build de prueba)

### Quitado
- **`SFX_T_Glitch`** (estática de cinta dañada, 1,5 s, que sonaba a ratos en la portada y con el glitch): el volumen del asset pasa a 0. Por eso el susto del glitch queda sin sonido hasta reemplazarlo.

## [0.3.6] — 2026-11-09 · Sin estática de TV (build de prueba)

### Quitado
- **`SFX_VHS`** (estática VHS de 1,5 s que se repetía en la portada y en el juego): el volumen del asset pasa a 0. Lo confirmó el CTO; era el ruido de "TV sin señal".

## [0.3.5] — 2026-11-09 · Thriller y sin ruido de estática (build de prueba)

### Cambiado
- **Color estilo thriller** (antes era un look de película "bonito"):
  - Saturación 0,55; sombras verde-azuladas y altas luces amarillo pálido.
  - Tinte verdoso, FilmToe 0,68, viñeta 0,75 y bloom 0,22.
- **Luces:** hospital en fluorescente frío (4300 K) y parque en sodio pálido (3100 K).

### Quitado
- **Estática del hospital:** los 3 `AmbientSound` con `AMB_T_Hospital` (ruido generado) quedan apagados (`bAutoActivate` false, volumen 0).
- **Siseo de lluvia en el parque:** en `ActualizarLluvia`, el volumen exterior pasa de 0,25 a 0. La lluvia se sigue viendo y los truenos se mantienen.

## [0.3.3] — 2026-11-09 · Look de película (post-proceso + luces, build de prueba)

### Cambiado
- **Post-proceso (`PostProcessVolume_0`):**
  - Saturación 0,72.
  - Sombras frías y desaturadas, y altas luces ámbar.
  - Contraste 1,14; FilmToe 0,62 (negros más profundos).
  - Grano 0,26, viñeta 0,62 y aberración de color 0,35.
  - Bloom 0,6, nitidez 0,35 y AO 0,7.
  - Exposición fija sin cambios.
- **Luces:** 45 luces blancas pasan a temperatura de ampolleta. Las `LuzParque` quedan en 2600 K, los focos del teatro en 3200 K y el resto en 2900 K. No se tocaron las luces que colorea `BP_Interruptor` ni los rellenos altos.
- **SkyLight:** 0,22 → 0,16, para tener negros reales.

## [0.3.2] — 2026-11-09 · Sprint 2: escondites y respiración (build de prueba)

### Corregido
- **Escondites (`BP_Hiding_Door`):** al entrar o salir, el jugador se teletransportaba con el centro de la cápsula a la altura de los pies del marcador (z = 4 cm), y quedaba medio enterrado y atascado. Ahora se suma +100 cm en Z y `bTeleport = true`. Los 3 clósets (`ESC_Armario_*`) volvieron a `Hospital/Escondites`.

### Añadido
- **Contener la respiración** (función `Respiracion`, se llama cada tick después de `EventoCasa`; variable `Jadeo`):
  - Escondida + mantener **Correr (L3 / Shift)**: gasta aliento a 25/s.
  - Si el aliento llega a 0: jadeo con `SFX_Respiracion`, mensaje y **ruido de 15 m durante 3 s** (`RuidoPos`/`RuidoT`/`RadioRuido`), al que los enemigos acuden.
  - Escondida respirando normal con un `BP_Enemigo` a menos de 7 m: ruido suave de 7 m (te escucha).
  - El mensaje al esconderse explica el control.
- **Versionado del proyecto Unreal:** rama `unreal` con Git LFS (`herramientas/Versionar_Unreal.bat`).

## [0.3.1] — 2026-11-09 · Demo corta para publicar el 13 de noviembre (candidata)

- Es la demo corta (final frente al castillo) con todo lo corregido después de la v0.3.0:
  - entrada sin torniquetes duplicados;
  - modo Calidad Baja más liviano (v0.4.1);
  - calidad Baja por defecto.
- **Corregido el bloqueo real de la entrada:** las mallas `SM_Ent_Reja` (barrotes de 15 m) y `SM_Ent_Hierro` (arco) tenían una colisión simple en forma de caja que tapaba el hueco central. El rayo de visión pasaba, pero el personaje no. Se quitó su colisión, como en el diseño original (`sin_colision.py`). Los costados siguen cerrados por `ENT_BloqueoI/D`.
- **Interruptores de la entrada, corregidos:**
  - `UsarInterruptor` ahora se llama **primero** al presionar E (`EventGraph`, antes de `Interactuar`). Antes estaba al final de la cadena de más de 200 nodos de `InteractuarZonas`, donde se perdía.
  - Se quitó el cortocircuito del 4.º interruptor. Los 4 se encienden **en cualquier orden** y con un solo intento cada uno.
  - El radio pasó de 2,3 m a 3 m.
  - **Sombra bajo el letrero (`BP_SustoEnergia`):** ahora aparece **al llegar a la entrada desde el pasillo** (jugador con Y < -4200, después del glitch) como primer susto del parque. Antes requería energía y además cortaba la luz con un segundo cortocircuito. Ya no corta la energía: los interruptores encienden las luces de forma definitiva.
  - **Audio de terror priorizado:**
    - Lluvia del jugador (`SonidoLluvia`, Rain_Heavy de SoulCity): filtro pasa-bajos a 2,8 kHz (quita el siseo de "radio") y volumen en el parque de 0,55 a 0,25.
    - `SC_Ambiente` al 60 %, `SC_SFX` al 125 %.
    - `SND_Ambiente_Parque` y `SND_VientoParque` a la mitad.
    - Pasos (`PasoSuelo`) de 0,5 a 0,8.
  - **Lluvia visual** (`NS_Lluvia`): 2.600 → 1.400 gotas/s; color más oscuro y transparente (0,30 / 0,36 / 0,45, alfa 0,35).
  - **Salto con ✕ (mando), blindado:** antes de `Jump` ahora llama a `UnCrouch` y deja `Agachada = false`, así que si el personaje quedó agachado, se levanta y salta. Además, ✕ solo confirma en el menú principal si `WidgetMenu` está en pantalla **y visible** (evita un menú "fantasma").
  - **Mando:** el botón de interactuar (`Gamepad_FaceButton_Left`) solo llamaba a `Interactuar`, así que nunca revisaba los interruptores ni llamaba a `RecogerObjeto` / `InteractuarZonas`. Ahora usa la misma cadena que la E.
- **Cinemática del parque:** ahora se activa **al volver la energía** (4 interruptores, `Energia`) y no al cruzar el portón. Dura 12 s (antes 5). `CAM_CineParque` va enganchada a `CINE_Pivote` (un `BP_Giratorio` invisible en el centro del parque, a 22°/s), así que la cámara **orbita el parque** a 75 m de radio y 31 m de altura mostrando las atracciones iluminadas.
- Nueva variable `DemoCorta` (bool) en el jugador: si está activa, `RevisarFinDemo` usa el final del castillo (Y < -11450); si no, el final es la huida de la Casa.
- **Estado del proyecto:** `DemoCorta = true` en el CDO. Para volver a generar la demo completa hay que ponerla en `false` y `ProjectVersion=0.4.x`.

## [0.4.1] — 2026-11-09 · Rendimiento

### Cambiado
- **El modo Calidad Baja ahora es más agresivo** (`AplicarOpciones`, solo si `Calidad == 0`):
  - resolución interna al 55 %;
  - `r.ViewDistanceScale 0.6`;
  - sin niebla volumétrica ni desenfoque de movimiento;
  - follaje al 50 %;
  - sombras a 512;
  - sin VSM ni Lumen (difusa y reflejos).

  Al pasar a Media o Alta se restauran los valores (distancia, niebla, blur, follaje y sombras).
- Bosques: distancia de corte de 70–95 m en las plantillas de los 7 `BP_Bosque_*`. Las instancias del mapa conservan su valor guardado de 120 m.
- Se corrigió el clímax de la Casa (radio de 16 m).

## [0.4.0] — 2026-11-09 · Demo completa (build de prueba, sin validar por el CTO)

### Añadido
- **Evento del Carrusel** (`EventoCarrusel` en el jugador, se llama cada tick después de `RevisarFinDemo`; variable `FaseCarrusel`):
  1. A menos de 13 m del carrusel, la música de feria (tag `CarruselSonido`) se detiene, el carrusel y sus luces (tag `CarruselGira`) empiezan a girar y aparece el mensaje "La música del carrusel se apagó... y empezó a girar solo."
  2. A menos de 4 m del centro, el carrusel se detiene en seco, la muñeca (tag `Muneca`) aparece entre el carrusel y la avenida (-1400, -7600) y se ejecuta `MostrarSusto`.
- **Clímax de la Casa de Alfredo** (`EventoCasa`, variable `FaseCasa`, se llama después de `EventoCarrusel`): al obtener el fusible de la caja fuerte (`TieneFusible`), se corta la linterna (`ForzarOscuridad`), aparece el mensaje "Se cortó la luz. Afuera, algo se ríe… ¡Vuelve a la entrada del parque!", el payaso (tag `Payaso`) aparece frente a la casa (8200, -14300) con un chillido posicional, y empieza la huida.
- **Nuevo final de la demo completa:** después del clímax, llegar a la entrada del parque (Y > -5300, |X| < 900) muestra la pantalla final.

### Cambiado
- El final frente al castillo (`RevisarFinDemo`) queda desactivado para v0.4.0 (umbral -99999). La demo corta v0.3.0 ya publicada no cambia.
- Se abrió la bodega (`BLQ_Bodega` a `Parque/Demo/Sobrantes`), porque la llave de la casa está ahí.

### Corregido (después del build 0.4.0, entra en la próxima versión)
- El clímax de la Casa solo se dispara **dentro de un radio de 16 m de la casa**. Así no se repite al cargar una partida guardada en otro lugar.

### Añadido (cont.)
- Mezcla propia: `SC_SFX` y `SC_Ambiente` (`MundoMagico/Audio/Mezcla`). El volumen general ahora aplica a las clases hijas.
- Diseño del tramo de la demo completa: `auditoria/diseno_sprint3_demo.md`.

## [0.3.0] — 2026-11-09 · Demo corta (objetivo de publicación: 13 de noviembre)

### Añadido
- **Fin de la demo:** la nueva función `RevisarFinDemo` del jugador, que corre cada tick después de `RevisarSalida`, marca `Terminado` al llegar al final de la avenida frente al castillo (|X| < 700, Y < -11450). Así se activa la pantalla final existente: "CAPÍTULO 1 · COMPLETADO… Continúa en el Capítulo 2".
- **Bloqueos de demo** (`Parque/Demo/Bloqueos`): una reja y un cono en el camino al tren, la torre y la montaña, y en las puertas del castillo, el teatro y la bodega.
- Checklist de prueba de humo: `auditoria/checklist_humo.md`.

### Corregido
- **No se podía entrar al parque:** 4 torniquetes duplicados (`ENT_TorniqueteD3` a `D6`) tapaban el centro del portón. Se movieron bajo el suelo a `Parque/Demo/Sobrantes`. Ahora hay 5,2 m de paso libre, verificado con trazos de colisión.

### Cambiado
- **Calidad Baja por defecto** en el primer arranque (valor por defecto de `Calidad` en el CDO = 0). El jugador puede subirla en Configuración.
- Plan acelerado a sprints de 1 semana (`PLAN_DE_TRABAJO.md`).

## [0.2.0] — 2026-11-09 (Sprint 1: Cimientos de Tensión e Interacción)

### Corregido
- La camilla `HX_Camilla3` encajonaba al jugador junto a la cama inicial al ir por la linterna. Se movió a la esquina sureste de la sala (330, 2620). Lo detectó la prueba de humo del build.
- `BP_ThirdPersonCharacter.AngelPayaso`: acceso a `Payasos[4]` con un array de 1 elemento. Ahora usa el índice 0.
- `BP_ThirdPersonCharacter.OcultarPagina`: `Accessed None` sobre `WidgetPagina`. Ahora valida con `IsValid` antes de `RemoveFromParent`.
- `Vigilar` y `RevisarFinal`: `SetTimer` con tiempo 0 se usaba para cancelar los timers de `MostrarSusto` y `MostrarFinal`. Ahora un `Branch` decide entre `SetTimer` y `ClearTimerbyFunctionName`.
- `MostrarFinal`: timer de `AbrirPanel` de 0,01 a 0,05 s.

### Añadido
- Parque: 69 props de Fab en `Parque/Fab2`:
  - 34 rocas en senderos y cerros.
  - 20 faroles apagados en senderos secundarios.
  - 4 columnas en el teatro y el castillo.
  - Señalética de metro en la avenida.
  - Herramientas afuera de la bodega.
- Parque vivo:
  - Lluvia, viento y ruidos metálicos al azar (`BP_RuidoAzar`).
  - Nieblas locales, luces que parpadean y partículas.
  - Torniquetes, boleterías y máquinas expendedoras.
- Herramientas:
  - `Abrir_MundoMagico_Liviano.bat`: abre Unreal sin RayTracing, VSM ni Lumen, solo mientras está abierto.
  - `Respaldar_Proyecto.bat`: zip con fecha de lo propio del proyecto. Conserva los últimos 10.
- Documentación del estudio: `BOARD.md`, `AUDIT_REPORT.md`, `auditoria/*.md`.

### Cambiado
- `DefaultGame.ini`: `ProjectName`, `CompanyName` y `ProjectVersion=0.2.0`.
- Render para iGPU:
  - SkyAtmosphere y VolumetricCloud ocultos.
  - 29 luces decorativas sin sombras.
  - Niebla volumétrica con rejilla 16/64.
- Audio 3D:
  - Atenuaciones `ATT_Corto`, `ATT_Medio` y `ATT_Largo` en los 12 `SND_*` y 7 `SFX`.
  - Sustos del pasillo posicionales (`PlaySoundAtLocation`).
- Fuentes: respaldo `DroidSansFallback` para ▲ ✕ ↑ ↓ ← →.
- Iluminación por zonas:
  - Avenida en penumbra.
  - Carrusel y Noria menos intensos.
- Oso Willy y León movidos a los laterales del parque.
- La botella emite `MakeNoise` al romperse.
- La noria gira sin colisión física; corrige el aviso de escala 0.
- Renombres:
  - `SM_SM_*` → `SM_*`.
  - `Ambiente_*` → `AMB_*`.
  - `Musica_*` → `MUS_*`.
- 7 packs de Fab sin uso (6 GB) archivados fuera de `Content/`.
- Nuevo `Construir_Version.bat`: sincroniza a `C:\MM_Build` y genera `Build/v<versión>`.
- Build Shipping v0.2.0: cook con 0 errores y 0 warnings.

### Mantenimiento
- 20 archivos `.tmp` de guardados interrumpidos en `Saved/` enviados a la Papelera.

### Problemas conocidos
- En el PC de desarrollo (Intel Iris Plus, 8 GB) el editor y PIE se cuelgan por GPU (`DXGI_ERROR_DEVICE_HUNG`). Ver `AUDIT_REPORT.md`.

## [0.1.0] — 2026-11-02
- Primer build Shipping Win64, 2,2 GB, 0 errores, generado desde `C:\MM_Build`.
- Incluye:
  - Hospital y pasillo.
  - Parque con atracciones.
  - Enemigo payaso.
  - Guardado y continuación (`BP_Partida`).
  - Mochila y pilas manuales.
  - Cinemáticas.
  - Menú con opciones.
