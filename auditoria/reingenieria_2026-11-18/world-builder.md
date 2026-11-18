# Reingeniería 18-11 · @world-builder (Mundo, Narrativa y Arte)

Solo lectura. El diagnóstico está en `auditoria/estudio_2026-11-18/world-builder.md`. Base: recorte aprobado del Nivel 1 (sin 2.º piso; negatoscopio en el sótano) y v0.3.61 (LUT verdosa y niebla #070b14 ya aplicadas).
Lentes: level-designer, world-builder, narrative-director, writer, art-director y technical-artist.

## 1. Visión
- **Una sola noche, una sola hora:** Isidora despierta muerta en el Pabellón 4 la noche del 12/03/1998 y baja, literalmente, hacia la hora de su muerte (**03:33**).
- **El hospital se lee como un expediente.** Cada página es un papel clínico: ficha, orden, registro o placa. El jugador arma el diagnóstico que nadie le dijo.
- **La luz guía y el rojo avisa:** fluorescente frío = seguro, faro naranja o SALIDA verde = hacia adelante, rojo = peligro. Sin flechas en el HUD.
- **Bajar es el arco:** de la planta baja (frío, iluminada) al sótano (oscuro, húmedo) y a la morgue (blanca y helada). La tensión sube con cada tramo de escalera.
- **Todo barato para la iGPU:** luz horneada, 1 material de post, niebla sin volumétrica, tarjetas unlit. 0 luces nuevas con sombra.

## 2. Layout del Nivel 1 · `L_Hospital` (25–30 min)
```
PLANTA BAJA (existe; se viste con Horror_Hospital)
 [Z1 Sala P4]──[Z2 Pasillo HX]──[Z3 Recepción/Urgencias]──[Z4 Pasillo Urgencias (Sombra)]
  inicio,       puertas laterales  registro, TV sala espera    2 ESC_HideSpot (ya están)
  linterna      entreabiertas      silueta tras el vidrio      balizas rojas, alarma v0.3.40
                                                                   │
                                              [Z5 Escalera de servicio] (ascensor muerto)
                                                                   │  ▼ baja
SÓTANO (nuevo) ── anillo de servicio alrededor de un bloque central ───────────────
        ┌──────────[Z6 Pasillo de calderas]──────────┐
        │  ESC_Sot_1 (lockers)        faro naranja    │
 [Z7 Archivo]        ┌[BLOQUE: Z8 Rayos X]┐     [Z9 Lavandería]
  placa de I.M.      │ negatoscopio 0333  │      ESC_Sot_2 (carro de ropa)
  ESC_Sot_3 (armario)└────────────────────┘      faro SALIDA verde (lejano)
        │                                             │
        └──────────[Z10 Antesala morgue]──────────────┘
                         │ BP_DoorLocked (Código 0333)
                   [Z11 Morgue] gaveta 4 → LlaveMorgue
                         │
                   [Z12 Rampa de ambulancias] BP_DoorLocked (Llave) → glitch + TV → L_Casona
```
**Ruta crítica (11 pasos):** Z1 despertar y linterna (mostrador) → Z2 → Z3 registro (pág. 4) → Z4 pasa la Sombra (tutorial de escondite) → Z5 baja → Z6 → Z7 recoger la **placa de tórax** (pág. 10) → Z8 ponerla en el **negatoscopio** → se lee "13/03/98 · 03:33" (pág. 13) → Z10 código **0333** → Z11 gaveta 4 (`LlaveMorgue`, pág. 14) → Z12 salida.
**Bucles:**
- **Anillo del sótano:** Archivo y Rayos X quedan en lados opuestos, así que se cruza el anillo dos veces. La Sombra 2 patrulla el anillo en sentido horario con 2 rutas (corta por calderas, larga por lavandería).
- **Retorno con presión:** al sacar la llave suena la alarma de la morgue; la Sombra corta el camino corto y obliga a volver por el lado opuesto del anillo hasta Z12.
- **Atajo que se abre:** desde Z9, una puerta de servicio con barra antipánico (solo se abre desde adentro) vuelve a la antesala. Recompensa por explorar.

**Escondites (5):** `ESC_HideSpot_1/2` (Z4, ya colocados), `ESC_Sot_1` lockers (Z6), `ESC_Sot_2` carro de ropa (Z9), `ESC_Sot_3` armario de archivo (Z7). Ninguno a más de 12 m de otro en el anillo.

**Puntos de susto (todos piden turno al `BP_DirectorMiedo`):**
| Marcador | Zona | Tipo | Qué pasa |
|---|---|---|---|
| `SUS_TVSalaEspera` | Z3 | audio | La TV se enciende sola: estática y "...Isidora... despierta..." |
| `SUS_SiluetaVidrio` | Z3 | visual | Silueta tras el vidrio de Urgencias; se oculta al entrar en el cono de la linterna |
| `BP_SustoPasillo` | Z4 | enemigo | Ya existe (balizas y alarma) |
| `SUS_TuboCae` | Z5 | físico | Un tubo fluorescente cae en la escalera; se apaga el tramo |
| `SUS_CamillaRueda` | Z6 | físico | Una camilla rueda sola hacia el jugador por el pasillo de calderas |
| `SUS_NegatoscopioParpadeo` | Z8 | visual | Al leer la placa, el negatoscopio parpadea y en el reflejo aparece Isidora detrás |
| `SUS_ManiquiMorgue` (B8) | Z11 | enemigo | Maniquí "luz roja, luz verde" entre las gavetas |
| `SUS_AlarmaMorgue` | Z11 | audio | Al tomar la llave: alarma y luz roja; la Sombra se activa en modo caza |

**Páginas (marcadores `PAG_01..15`, actores `BP_PaginaNovela`):** 1 cama de Z1 · 2 mostrador de Z1 · 3 Z2, sala lateral · 4 Z3 mesón · 5 Z3 casillero · 6 Z4 box · 7 Z3 portería · 8 Z5 descanso · 9 Z6 tablero · 10 Z7 (placa, obligatoria) · 11 Z9 · 12 Z6 escondite · 13 Z8 (obligatoria) · 14 Z11 (obligatoria) · 15 Z12 junto a la puerta.
Obligatorias: 4, 10, 13 y 14. Las otras 11 son opcionales y suman al contador "15/62".

## 3. Biblia narrativa
**Premisa.** Isidora M. (9) muere a las 03:33 del 13/03/1998 en el Pabellón 4. Despierta en una versión de sombra del hospital y, sin saber que murió, junta las páginas de "la novela" (su propia historia). Alfredo, ex actor del parque Mundo Mágico ("Inspector Metete") y auxiliar del hogar El Buen Samaritano, le prometió una obra. Las **4 cintas VHS** son de ella: graban el parque y la noche de su muerte (23:41, 23:58, 00:17 y 03:33).
**Personajes:**
- **Isidora:** la jugadora. Curiosa, testaruda, no duerme sin su cuento.
- **Alfredo (67):** la quiere como a una nieta. Tiene prohibido visitarla. Su casa es la Casona (Nivel 2).
- **Dra. Rafaela (36):** directora del hogar. Prohíbe las visitas "para no alterarla". No es villana; es miedo.
- **Tía Luisa:** auxiliar de turno noche. Cómplice de Alfredo.
- **La Sombra:** lo que queda del miedo de la noche (el hospital que no la dejó ir). Cada nivel tiene la suya.

**Canon propuesto (resuelve el choque):** **leucemia linfoblástica aguda en recaída** (canon de la película) **con miocardiopatía por la quimioterapia** (el "corazón débil" del HTML). Así los dos textos son ciertos: el hogar la trata por leucemia y esa noche ingresa por el corazón. Bonus de diseño: la radiografía de tórax del negatoscopio muestra un **corazón agrandado**, que es la imagen central del nivel.
**Lugares y fechas:** el Pabellón 4 es el ex pabellón psiquiátrico que el hogar El Buen Samaritano usa para los pacientes terminales. El juego mantiene 1998 (la película dice 2001; se declara "adaptación").
**Voces:** en el hospital, **clínica** (formularios, abreviaturas, hora exacta, firma ilegible), con notas al margen a mano de Luisa o Alfredo; en la Casona, **íntima** (cartas de Alfredo).

### Las 15 páginas (`DA_NovelaData`, ID 1–15)
1. **FICHA DE INGRESO · PABELLÓN 4** — Paciente: I. M., 9 años. Procedencia: Hogar El Buen Samaritano. Ingreso: 12/03/1998, 22:50. Dx: LLA en recaída; miocardiopatía secundaria. Pertenencias: ver inventario.
2. **HOJA DE ENFERMERÍA · TURNO NOCHE** — 23:05, paciente despierta, pide "el cuento del inspector". 23:20, se niega a dormir. Se deja la linterna de ronda en el mostrador. — L. (Luisa)
3. **ORDEN DE LA DIRECCIÓN** — Se suspenden las visitas no familiares a la paciente I. M. hasta nuevo aviso. Motivo: evitar emociones fuertes. Firma: Dra. Rafaela, directora. Al margen, a mano: "Ella pregunta por él cada noche."
4. **REGISTRO DE URGENCIAS · 12/03/1998** — Última anotación: "Paciente I., 9 años. Corazón débil. Se negó a dormir sin su cuento". Firma ilegible. Debajo, con otra letra: "Alfredo no puede volver a visitarla".
5. **INVENTARIO DE PERTENENCIAS** — 1 vestido blanco. 1 dibujo de un león con sombrero. 4 cintas VHS rotuladas "MUNDO MÁGICO". 1 sobre: "DIENTE · ISIDORA M." Entregar a: (en blanco).
6. **ELECTROCARDIOGRAMA · BOX 2** — Taquicardia sinusal. Fracción de eyección 30 %. Indicación: reposo absoluto, sin sobresaltos. Al reverso, un corazón dibujado con lápiz de cera.
7. **LIBRO DE NOVEDADES · PORTERÍA** — 23:41. La TV de la sala de espera se encendió sola, sin señal. Se escuchó una voz en la estática. Se desenchufó. Siguió encendida.
8. **PARTE DE MANTENCIÓN N.º 114** — Ascensor de servicio fuera de servicio desde dic. 1997. Usar la escalera. Luz del sótano intermitente: no bajar sin linterna. No dejar la linterna apagada.
9. **SOLICITUD DE RADIOGRAFÍA** — Tórax AP, urgente. Paciente: I. M. Sospecha: cardiomegalia. La placa queda en el Archivo, rotulada con la hora del examen. Leer en el negatoscopio de Rayos X.
10. **ARCHIVO · CARPETA I. M.** — Cuatro traslados del hogar al hospital en seis meses. Contiene placa de tórax del 13/03/1998. Nota adhesiva: "No es un número de ficha. Es la hora."
11. **LAVANDERÍA · VALE DE ROPA** — Devuelto a lavandería: 1 traje de "Inspector", talla adulto, rasgado en la manga. Propietario: Sr. Alfredo. Observación: "Lo usó en la sala. La paciente rió por primera vez en semanas."
12. **HOJA ADJUNTA AL SUMARIO** (letra de Alfredo) — Hoy la linterna se apagó sola en el pasillo de Urgencias. Escuché una voz: "Isidora, despierta". Si la linterna se apaga, vienen por ti. La luz los detiene.
13. **RADIOGRAFÍA DE TÓRAX · I. M.** — Silueta cardíaca aumentada de tamaño. Rótulo de la placa: 13/03/98 · 03:33. (Se lee en el negatoscopio; da el código de la morgue.)
14. **LIBRO DE INGRESO · MORGUE** — 13/03/1998, 03:33. I. M., 9 años. Gaveta 4. Retira el cuerpo: (en blanco). Al margen: "Nadie vino." La llave de la rampa cuelga de la gaveta.
15. **CERTIFICADO DE DEFUNCIÓN (borrador)** — Causa: insuficiencia cardíaca; leucemia en recaída. Hora: 03:33. Sin firma. Encima, a mano: "No lo firmen todavía. Le prometí el show. — A."

**Pantalla final de la demo:** "Isidora M. ingresó al Pabellón 4 la noche del 12 de marzo de 1998. Páginas: N/15 · 62 en el juego completo." + teaser de la Casona.

## 4. Biblia de arte y luz
| Nivel | Base | Acento seguro | Guía | Peligro | Post |
|---|---|---|---|---|---|
| Hospital | muro #4b6562, azulejo blanco sucio hasta 1,5 m, sombras #0c1a1a | fluorescente #9fb8e0 | SALIDA verde #3cff7a, faro naranja #ff8a3c | rojo #ff2020 (solo balizas y ojos) | LUT `T_LUT_HospitalHTML` |
| Casona | madera #3a2a1e, papel mural #5a4a36 | tungsteno #ffb46a | ventana de luna #7a9be0 | rojo terciopelo #6e0f14 (telón, cinta) | LUT ámbar sucio; tras el apagón, solo azul de luna |
| Parque | negro azul #03050a | luna #5577bb | luces de feria apagadas | rojo baliza | igual que hoy |

**Lenguaje de luz:** cada tramo tiene **un faro** al fondo (SALIDA verde o puerta con luz naranja). Fluorescentes con parpadeo por material (`Time`), 1 luz débil cada 2 tubos, horneada; máximo 1–2 dinámicas que parpadean por pasillo. Rayos de luna por ventanas con planos aditivos unlit #7a9be0, opacidad 0,13. En el sótano, la mitad de los tubos muertos: la linterna es la fuente principal. En la morgue, blanco frío plano (la única sala "bien" iluminada, para que el maniquí se vea).

**Reglas de post y niebla (iGPU):**
- 1 PostProcessVolume global (`PPV_PostHTML`) más uno por zona solo para LUT o exposición; sin Lumen, sin SSR, sin volumétrica en interiores.
- Niebla #070b14 con densidad 0,02 en la planta baja, 0,035 en el sótano y 0,05 en la morgue (por `BP_VolumenZona`, no por Tick).
- Niebla baja con tarjetas unlit translúcidas #8fa0b4, opacidad 0,17, panner lento: ≤8 por sala, solo en Z6, Z10 y Z11.
- Bloom 0,5 con umbral alto: solo brillan los tubos, SALIDA y el negatoscopio.
- Exposición dinámica al apagar la linterna: 2.º PostProcess con ExposureBias +0,5 y blend por Timeline (decisión CTO #3 del estudio).
- Presupuesto: <1200 draw calls por vista y texturas a 1024 en `Horror_Hospital`. MI compartidos: `MI_Azulejo`, `MI_Muro`, `MI_Tubo`.

**Assets por pack:**
- **`Horror_Hospital`:** `SM_Wall_01..26_v`, `SM_Wallplinth_*`, `SM_Ceiling_Square_01_p` y `SM_Ceiling_Frame`, `SM_Door_01..05` (la 03 entreabierta en Z2), `SM_Bed_01` con cobertor (Z1, y también como camilla de Z6), biombos y cortinas (Z1, Z3), `SM_Rack_*` y refrigeradores (Z11 como cámaras), `SM_Desk_*` y `SM_Cabinet_*` (Z3, Z7), decals de suciedad y cables.
- **Otros que ya están en el proyecto:** `LampsPack` (tubos), `TA_Corpses` (bajo sábana, solo en Z11 y sin rostro), `SM_Notes` (papeles de las páginas).
- **`UnrealBucket` (Casona):** sillones, sillas, araña, consola y jarrones para el vestíbulo del teaser.
- **`SICKA_MouldingSet` (Casona):** molduras de muro y cornisas del vestíbulo.
- **Falta (low-poly o un plano emisivo):** negatoscopio (caja + plano emisivo con la textura de la placa) y gavetas de la morgue (se arman con `SM_Cabinet_*` escalado).

## 5. Teaser de la Casona (75 s, sin enemigo, sin páginas)
| Tiempo | Imagen | Sonido |
|---|---|---|
| 0–6 s | Rampa de ambulancias: la puerta se abre y todo hace **glitch** y se encoge a la línea de TV y luego a un punto | "tuk" + pitido, silencio 1,5 s |
| 6–12 s | Negro. Texto VHS: "▶ CINTA 1 · 12/03/1998 · 23:41" con tracking | clac del mecanismo, zumbido de cinta |
| 12–30 s | Vestíbulo de la Casona en ámbar (`UnrealBucket` + `SICKA`): escalera central, araña encendida. El jugador puede moverse | reloj de pared, madera que cruje al pisar |
| 30–45 s | Sobre la consola: un traje de Inspector doblado y un sobre: "Para Isidorita. Falta poquito. — A." (no es página; es gancho) | lluvia en las ventanas |
| 45–55 s | En lo alto de la escalera, a contraluz, una niña de vestido blanco sube y desaparece | un paso arriba, luego nada |
| 55–65 s | Las lámparas mueren una por una de la puerta a la escalera; queda solo la luna | chasquido de tungsteno ×3, drone grave |
| 65–75 s | Fundido a negro: "CONTINUARÁ · Nivel 2: La Casona" y "Páginas N/15 · 62 en el juego completo" | silencio y una nota del piano de la portada |
Se activa con el flag `SalidaHospital`. Es un mapa propio (`L_Casona_Teaser`, solo el vestíbulo) para no cargar la Casona completa.

## 6. Plan por builds
| Build | Fecha | Mío (con interruptor) |
|---|---|---|
| Sprint A (hasta RC1 del 25-11) | 19–25-11 | Textos 1–15 cargados en `DA_NovelaData` (solo datos, riesgo nulo); faros SALIDA y naranja en Z2–Z4; tarjetas de niebla baja off por defecto (`Atmos_NieblaBaja`) |
| B-1 | 26–27-11 | Blockout gris del sótano (Z5–Z12) con el anillo; marcadores `PAG_*`, `ESC_Sot_*` y `SUS_*`; prueba de recorrido |
| B-2 | 28–29-11 | Kit `Horror_Hospital` en el sótano, negatoscopio y placa, código 0333, gaveta 4 y `LlaveMorgue`; luz horneada |
| B-3 | 30-11–1-12 (freeze) | Sustos conectados al Director, maniquí B8, alarma y retorno; `L_Casona_Teaser`; fps medidos con `Probar.bat` |
| RC2 / gold | 3-12 / 8-12 | Solo arreglos de texto, luz y colisión |
Riesgo principal: B-1 y B-2 dependen de tener 1 editor libre 4 días seguidos. Si se atrasa, se corta primero el atajo de Z9 y después el maniquí.

## 7. Decisiones para el CTO
1. **Canon médico:** leucemia en recaída + miocardiopatía por la quimio ("corazón débil" como complicación). Recomiendo que sí: concilia la película y el HTML y da sentido a la radiografía.
2. **¿La Casona es la casa de Alfredo?** Recomiendo que sí: el teaser se apoya en el traje y el sobre de Alfredo.
3. **Año del juego 1998** (la película dice 2001). Recomiendo mantener 1998: ya está en las cintas, el registro y la UI; se declara "inspirado en".
