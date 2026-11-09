# Auditoría World-Builder — Mundo Mágico: Turno Nocturno

Agente: @world-builder (diseño espacial, iluminación, narrativa ambiental). Fecha: 2026-11-09. Modo: SOLO LECTURA (no se tocó el proyecto ni se usó unreal-mcp).

## Fuentes revisadas

- `Content/MundoMagico/Maps/L_Pabellon4.umap` (5,2 MB, guardado 11-09 11:22). Saqué su tabla de nombres con un regex sobre el binario (19 800 nombres).
- **Corrección al contexto:** el mapa **NO usa One File Per Actor**. `Content/__ExternalActors__` solo tiene carpetas de packs (AnimatedStorageBox, PunkGirl_Bundle, ThirdPerson…), ninguna de `MundoMagico/Maps/L_Pabellon4`. Todos los actores viven dentro del .umap, un solo archivo binario.
- `ThirdPerson/Blueprints/BP_ThirdPersonCharacter.uasset` (10 MB): de aquí saqué 147 textos de narrativa y objetivos. También revisé BP_SustoPasillo, BP_SustoEnergia, BP_Despertar, BP_Muneca y BP_Enemigo.
- `Isidora3D/parque/actores.json`: 1449 actores, fechado 11-04 18:47. Es **anterior** a la reestructuración orgánica de ese mismo día, así que las coordenadas de atracciones, páginas y enemigos pueden estar desfasadas.
- `arboles_datos.json`, `trazado.json`, `Config/DefaultEngine.ini`, logs `Saved/Logs/MUNDO_MÁGICO_*.log`, `FogArea/Materials/M_FogArea`.
- Memorias: parque-html-a-unreal, gpu-cierres-modo-liviano, pendiente-packs-fab, perfil-a-pendiente-ram.

---

## 1. Resumen

El recorrido tiene una estructura narrativa sólida y bien escrita: despertar, linterna, pasillo de Urgencias con susto, glitch, parque, energía, generador, bodega, teatro, casa de Alfredo, cabina VIDEO y regreso al Pabellón 4. Tiene puzzles con ambientación (reloj "8 4 2", fonógrafo "LAS OCHO", pedestales en orden, caja fuerte), 4 cintas, 62 páginas y 3 finales o logros.

Los problemas son sobre todo de **ritmo en el parque** y de **iluminación sin validar**:

1. **Nadie ha jugado nunca el parque iluminado.** PIE se cuelga, el viewport está atascado en Unlit y el hardware no aguanta. El ritmo y la luz del 80 % del juego son teóricos.
2. **El parque es enorme (unos 240 × 240 m) y tiene pocos picos de tensión.** Los 4 enemigos se concentran en la avenida central. Las 62 páginas convierten la exploración en una búsqueda de coleccionables.
3. **La iluminación va al revés del género.** Al restaurar la energía el parque se llena de luz: neones, guirnaldas, 65 point lights. La tensión baja justo cuando debería subir.
4. **La escena es cara para lo que muestra:** cielo atmosférico más nubes volumétricas de noche, niebla volumética con una rejilla unas 8 veces más fina que la estándar, unos 1134 árboles Nanite de 200k a 2,4M tris, unos 1417 StaticMeshActors y 72 luces dinámicas.

---

## 2. Mapa de ritmo de tensión

Escala de tensión: 1 = calma, 5 = pico.

| # | Zona (coords aprox.) | Propósito | Tensión | Problemas |
|---|---|---|---|---|
| 1 | Sala de despertar / Pabellón 4 (0,+3000; PlayerStart -600,+3200) | Tutorial: oscuridad, buscar la linterna en el mostrador (585,2880). Cinemática CAM_CineDespertar. | 2 | Bien planteada ("Está oscuro. Busca algo con qué iluminar"). Falta un sonido u objeto que guíe la mirada hacia el lado este, el del mostrador. |
| 2 | Pasillo de Urgencias (y 0..-3530; HX_Reanimacion al oeste, HX_Farmacia al este) | Primer miedo: luces que se apagan, silla de ruedas que se mueve sola, BP_SustoPasillo (SK_Whisper Run+Attack1). | 4 | Es el mejor momento del juego. Riesgo: solo hay un susto y llega pronto. Las salas laterales HX_ no tienen recompensa ni amenaza propia. |
| 3 | Glitch de salida (GlitchPulso rojo y teleport a 0,-4300) | Transición de mundo, de clínico a feria. | 5 → 1 | Pasa de golpe del pico a la calma total, sin descompresión. El patio HX_PatioSalida es un parche porque SM_Terr_Cerros no tiene colisión cerca del portón: hay riesgo de caer fuera del mapa. |
| 4 | Entrada del parque (y -4500..-4800; ENT_, torniquetes, boleterías, CAM_CineParque) | Momento de impacto visual: letrero MUNDOMAGICO, 4 interruptores, BP_SustoEnergia (silueta con sombrero bajo el letrero). | 3 | El susto de la silueta está bien, pero ocurre en la zona más iluminada (LUZG_Entrada y barra de neón). Una silueta se lee peor a contraluz. Conviene verificarlo. |
| 5 | Avenida central (x=0, y -4900..-11900; LUZG_X0..X4, LUZG_L0..L3, 4 expendedoras, faroles) | Columna vertebral y orientación. | 2 | 5 guirnaldas y 8 faroles hacen de la avenida un "pasillo seguro" muy iluminado. Según el dump, aquí se concentran Muñeca (650,-7700), Oso Willy (-2000,-9000), León (2000,-11000) y Payaso (0,-11900), así que la zona segura es a la vez la zona de amenaza. Esto mezcla las reglas ("la luz los detiene"). |
| 6 | Carrusel (-2000,-7000) / Jardín de hojas (-5500,-8500, NIE_Jardin) | Exploración temprana, páginas. | 2 | El jardín con niebla es buena zona de tensión, pero está lejos de la avenida y no tiene un evento propio. |
| 7 | Autos chocones / Pista B (1500,-7200; generador) | Puzzle de energía: tirar la cuerda, ruido que atrae. | 3–4 | Buen diseño de riesgo y recompensa ("el ruido puede atraer algo"). Hay que confirmar que el ruido de verdad dirige a un enemigo hacia el jugador. |
| 8 | Bodega (-7800,-5200; BOD_, llave, lista de la obra) | Llave de la casa y lista del orden del teatro. Diálogo de Alfredo, disfraces que se mecen. | 3 | Es un callejón sin salida en el extremo oeste, sin amenaza al salir. Es buen lugar para una emboscada ligera. |
| 9 | Teatro (-5740,-12500; NIE_Teatro, FAR_Teatro, 5 pedestales) | Puzzle del reparto, "una sombra en la última fila". | 3–4 | Narrativa fuerte. El fallo del puzzle solo da texto ("El telón cruje"). Le falta una consecuencia física: apagón o enemigo. |
| 10 | Noria (5500,-10300) / Tren fantasma (5400,-6100) | Cintas 1 y 2 (REC_Cinta01 5650,-6100; Cinta02 5500,-9500), sonidos de metal. | 2–3 | En el lado este hay mucha caminata con poca amenaza. El texto "La noria gira sola" no tiene un evento visible que lo respalde. |
| 11 | Castillo (0,-12700) / CAM_CinePayaso (350,-11450) | Remate de la avenida, cinemática del payaso. | 4 | Es un buen punto focal. El payaso queda en la línea de visión directa desde toda la avenida, lo que adelanta su revelación. |
| 12 | Casa de Alfredo (9200,-15100; NIE_Casa, FAR_Casa) | Llave → fusible y cinta 4. Muebles de UnrealBucket. | 4 | Está en el extremo SE, a unos 140 m de la cabina VIDEO, que está en la entrada. Volver es un trayecto largo y vacío: dead time. |
| 13 | Torre (1600,-16000) / Montaña rusa (-3000,-17000) | Ambientación (FX_ChispasTorre, SND_MetalTorre/Montaña), páginas. | 2 | No tienen función en la misión principal. Son zonas opcionales sin recompensa de historia clara. |
| 14 | Cabina VIDEO (cerca de la entrada) → "¡Corre! Vuelve al Pabellón 4" | Clímax: persecución de vuelta y lectura del expediente. | 5 | Es el clímax correcto. Hay que verificar que el regreso al Pabellón sea físicamente posible, porque la ida fue un teleport de un solo sentido (SalidaHecha). |

Curva resultante: **2 → 4 → 5 → 1 → 3 → 2 (unos 20–40 min de parque con picos de 3–4 dispersos) → 5.** El valle del parque es demasiado largo y plano.

---

## 3. Hallazgos

### 🔴 Críticos

- **🔴 W1. Ritmo e iluminación del parque nunca validados en juego.** Evidencia: memoria gpu-cierres ("PIE still hung at first gameplay frame (BasePass)"; "will edit manually without PIE"); memoria parque ("Editor viewport is stuck on Unlit"); memoria Fab2 ("need gameplay/PIE testing — user's PC can't run PIE"). Impacto: todas las decisiones de luz, niebla y sustos del parque son a ciegas.
- **🔴 W2. La luz recompensa al jugador justo cuando debería castigarlo.** Evidencia: textos "Las luces del parque se encienden una a una. Por primera vez, alguien aplaude" y "Ahora el parque está vivo... y no está solo". Hay 17 LUZG_, 13 NEON_, 5 guirnaldas sobre la avenida y 65 PointLight en el mapa. Además la regla del juego es "La luz los detiene". Con el parque encendido, el jugador queda casi siempre protegido y los enemigos pierden amenaza. Falta un lenguaje de zonas: luz segura, penumbra con riesgo y oscuridad letal.
- **🔴 W3. Los enemigos se concentran en la avenida iluminada.** Evidencia (actores.json, posiblemente desfasado): ENE_Muneca (650,-7700), ENE_OsoWilly (-2000,-9000), ENE_Leon (2000,-11000), ENE_Payaso (0,-11900). Todos están a menos de 25 m del eje x=0, mientras el parque mide unos 240 m. Las zonas laterales (bodega, casa, tren, noria) no tienen amenaza: esas caminatas son las más largas y no tienen tensión.
- **🔴 W4. Retroceso largo y vacío al final.** La misión exige la casa (9200,-15100) y luego la cabina VIDEO en la entrada (y≈-4500), unos 140 m. Después hay que volver al Pabellón. No hay atajo, evento escalado ni persecución en ese tramo. Es el riesgo más alto de aburrimiento.
- **🔴 W5. Costo de render desproporcionado para una escena nocturna** (detalle en la sección 4). Hay SkyAtmosphere y VolumetricCloud activos de noche (invisibles, pero cuestan). La niebla volumétrica usa `GridPixelSize:8` y `GridSizeZ:128` (log MUNDO_MÁGICO_4.log): unas 8 veces más celdas que la rejilla por defecto (16 / 64). Es candidato directo al cuelgue en BasePass y en la niebla.

### 🟠 Importantes

- **🟠 W6. 62 páginas es demasiado.** Evidencia: REC_Pagina01..62 y el logro "Coleccionista, encontraste las 62 páginas". Con esa densidad (una página cada 30–40 m) el coleccionable se vuelve ruido y diluye la narrativa principal. Además se suman 10 botellas, 5 pilas, 3 botiquines y 3 cintas: 87 BP_Recogible en total.
- **🟠 W7. Coordenadas fijas en el BP del personaje.** Las coordenadas están escritas a mano en ActualizarZona, RevisarCines, Vigilar, InteractuarZonas, InteractuarExtra, MostrarObjetivo, ActualizarPosObjetivo y TextoAliento (memoria parque). Cualquier ajuste de layout rompe zonas, objetivos y cinemáticas. Esto bloquea iterar el diseño de niveles.
- **🟠 W8. Transición glitch sin descompresión.** Del susto del pasillo (pico 5) se pasa por teleport directo al parque. Falta un espacio intermedio, unos 20–30 s de "respirar": un túnel, el patio o la boletería en penumbra.
- **🟠 W9. Colisión del terreno incompleta.** "SM_Terr_Cerros has no collision near the gate" y HX_PatioSalida es un parche (memoria). Hay riesgo de que el jugador salga del mapa en otros bordes de cerros.
- **🟠 W10. Sonido de lluvia sin lluvia visible.** La lluvia es solo audio (Rain_Heavy_Ext_Cue en SonidoLluvia). No hay partículas, superficies mojadas ni charcos. Es una incoherencia ambiental.
- **🟠 W11. Sustos de texto sin eventos físicos.** "La noria gira sola. Alguien sube a la última cabina", "una sombra se queda sentada en la última fila", "En el túnel del tren fantasma, una sombra te observa". Solo hay 3 BP_Susto* (Pasillo, Energía, Muñeca) para unos 14 lugares de interés. Las promesas narrativas no se cumplen en pantalla.
- **🟠 W12. actores.json desactualizado.** Es del 11-04 18:47, anterior a la reestructuración, y no incluye luces, nieblas, FX ni Fab2. Cualquier análisis automático del layout que use este archivo es incorrecto.
- **🟠 W13. 20 FAROL_APAGADO más 4 FAR_ parpadeantes.** Bien como narrativa ambiental, pero no están conectados a una mecánica (por ejemplo, encenderlos como "luz segura" temporal).

### 🟢 Fortalezas

- **🟢 W14. Escritura ambiental excelente y coherente.** Alfredo, Rafaela, Luisa e Isidora; 1998, el incendio, el expediente. Hay notas ancladas a lugares: lista en la bodega, fusible en la casa, reloj y fonógrafo.
- **🟢 W15. Puzzles con ambientación:** reloj (8 4 2), fonógrafo (LAS OCHO), orden del reparto (bodega → teatro), generador con riesgo por ruido.
- **🟢 W16. El pasillo de Urgencias está bien construido:** luces que mueren, silla de ruedas, criatura y nota posterior.
- **🟢 W17. Sistema de "parque vivo" con buena base:** 4 nieblas locales (Jardín, Casa, Teatro, Valle), 4 luces parpadeantes, 7 BP_RuidoAzar aleatorios de 14–32 s, viento, partículas de chispas, goteo, papeles y hojas. Varias capas de audio ambiental (SND_* ×12).
- **🟢 W18. Señalización con hitos visuales:** cada atracción tiene un color de neón propio (castillo rosa, carrusel cian, noria teal, teatro dorado). Sirve para orientarse en un parque grande.
- **🟢 W19. Mecánica de esconderse** (5 BP_Hiding_Door, "Te escondes... no hagas ruido") y botella lanzable para distraer. Son buenas herramientas de sigilo.

---

## 4. Costo de rendimiento de la escena (estimado desde la tabla de nombres del .umap)

| Categoría | Conteo | Nota |
|---|---|---|
| StaticMeshActor | ~1417 | Unos 1417 actores separados → draw calls y costo de culling altos para un iGPU. Sin OFPA, todo carga a la vez. |
| PointLight (Movable) | ~62–65 | Luces dinámicas. MegaLights está apagado en modo Liviano, así que cada luz con sombra cuesta un cubemap de 6 caras. |
| SpotLight | 5–7 | Incluye la linterna del jugador. |
| BP_FlickeringLight | 5 (4 FAR_ más la plantilla) | Luces parpadeantes: invalidan la caché de sombras cada frame si proyectan sombra. |
| DirectionalLight / SkyLight | 1 / 1 | Correcto (sin duplicados). |
| SkyAtmosphere + VolumetricCloud | 1 + 1 | **Innecesarios de noche.** Las nubes volumétricas son de lo más caro en un iGPU. |
| ExponentialHeightFog con niebla volumétrica | 1 | GridPixelSize 8 / GridSizeZ 128 / HistoryMissSupersample 4: rejilla unas 8 veces mayor que la estándar. |
| BP_FogArea (NIE_*) | 4 | Esferas aditivas translúcidas (M_FogArea, BLEND_Additive, SphereMask). Tienen overdraw cuando llenan la pantalla. |
| Emitter (Cascade) | 6–7 | FX_ChispasTorre, FX_Goteo×3, FX_Hojas, FX_Papeles. Costo bajo o moderado. |
| AmbientSound / BP_RuidoAzar | 8 / 7 | Costo despreciable. |
| BP_Bosque (HISM) | 6 actores, **unas 1134 instancias** (A 65, B 196, C 306, I 324, Seco1 130, Seco2 113, contando los `;` de arboles_datos.json) | Mallas GV de 200k a 2,4M tris con Nanite. En -dx11 usan un fallback de 7–15k tris: entre 8 y 17 M tris por frame si no hay culling. El follaje enmascarado con Nanite es caro. |
| TextRenderActor | ~20–24 | Bajo. |
| DecalActor | ~19–25 | Bajo o moderado (son deferred decals). |
| BP_Recogible | 87 | Bajo, salvo que cada uno haga Tick. |
| BP_Enemigo | 4 instancias | SkeletalMesh y animaciones. |
| Renderer (DefaultEngine.ini) | Lumen GI y reflejos, VSM, RayTracing, MegaLights, TraceMeshSDFs | Demasiado para Iris Plus. El modo Liviano los desactiva en tiempo de ejecución. |

**Conclusión:** para Iris Plus con 128 MB de VRAM dedicada, los mayores costos evitables son, en orden: VolumetricCloud y SkyAtmosphere, la rejilla de niebla volumétrica, las sombras de unas 70 luces Movable, los unos 1134 árboles de alta densidad y los unos 1417 actores sueltos.

---

## 5. Recomendaciones Sprint 1 (priorizadas)

1. **P0 — Hacer el mapa jugable en el hardware actual** (desbloquea W1):
   - Desactivar o quitar VolumetricCloud_0 y SkyAtmosphere_0. De noche el cielo se puede cubrir con una esfera emisiva o una cubemap en la SkyLight.
   - Bajar la niebla volumétrica a GridPixelSize 16 y GridSizeZ 64, o apagarla y dejar solo la ExponentialHeightFog con las NIE_.
   - Poner `CastShadows=false` en todas las PointLight decorativas (neones, guirnaldas, LUZG_). Dejar sombra solo en la linterna y en 2–3 luces narrativas.
   - Crear un sublevel o capa "Parque_Pesado" (árboles Seco, rocas Fab2, faroles apagados) que se pueda desactivar para probar.
2. **P0 — Lenguaje de luz para el horror** (W2):
   - Definir 3 zonas: segura (cálida, cabina, boletería, faroles encendidos), penumbra (fría, avenida con guirnaldas a medias) y oscuridad (lateral, nieblas, interior de la casa y el teatro).
   - Al restaurar la energía, encender solo la avenida y la entrada. Las atracciones laterales quedan oscuras o con parpadeo, y algunas guirnaldas fallan.
3. **P1 — Redistribuir enemigos y sumar eventos por zona** (W3, W11). Mover 2 enemigos a los laterales: León al lado este (noria y tren), Oso Willy a la bodega o el teatro. Agregar 3–4 sustos físicos baratos que cumplan los textos: cabina de la noria que se mueve, sombra en la butaca del teatro, sombra en el túnel del tren, apagón al fallar el puzzle de pedestales.
4. **P1 — Resolver el retroceso del final** (W4): un atajo que se abre desde la casa (portón lateral E hacia la entrada) o mover la cabina VIDEO al centro (frente al castillo). Además, una persecución escalada con el Payaso en el regreso.
5. **P1 — Descompresión tras el glitch** (W8): de 20 a 30 s de espacio tranquilo en penumbra (boletería, torniquetes, cinemática del parque) antes de los interruptores. Ya existe CAM_CineParque, solo hay que ajustar los tiempos.
6. **P2 — Reducir las páginas de 62 a unas 20–25 con contenido**, agrupadas en 5 hitos (bodega, teatro, casa, noria, castillo). Las demás se pueden fusionar o volver lore opcional.
7. **P2 — Sacar las coordenadas fijas del BP del personaje** (W7) y reemplazarlas por volúmenes de zona (TriggerBox con tag de zona y objetivo) colocados en el mapa.
8. **P2 — Coherencia ambiental:** partículas de lluvia ligeras alrededor del jugador o charcos con decals (W10). Revisar la colisión en todo el borde de SM_Terr_Cerros (W9).
9. **P3 — Regenerar actores.json** con dump_actores.py tras cada cambio de layout, incluyendo luces, nieblas, FX y Fab2 (W12).

---

## 6. Solicitudes a otros agentes

- **@tech-art / rendimiento:** medir el costo real de VolumetricCloud, de la niebla volumétrica 8/128 y de las sombras de las luces Movable (`stat gpu`, `profilegpu`) en modo Liviano. Proponer un preset "Iris" de scalability y cvars de dispositivo. Evaluar LOD o fallback de baja densidad para BP_Bosque (unas 1134 instancias GV) y la fusión de StaticMeshActors (unos 1417) con HLOD o Merge Actors.
- **@gameplay / programación:** reemplazar las coordenadas fijas del BP_ThirdPersonCharacter por volúmenes de zona. Confirmar que el ruido del generador atrae a un enemigo. Implementar la vuelta al Pabellón 4 si el teleport es de un solo sentido. Revisar Tick en los 87 BP_Recogible y en RevisarSalida.
- **@ai / enemigos:** comportamiento por zona de luz (los enemigos evitan la luz segura y patrullan la penumbra). Reubicar spawns en los laterales. Persecución del clímax de regreso.
- **@narrative / diseño:** reducir y curar las 62 páginas. Mapear cada texto de "susto prometido" (noria, butaca, túnel) a un evento físico.
- **@audio:** stingers por zona de tensión. Confirmar que el sonido de lluvia es coherente con lo que se ve, o cambiarlo por viento o llovizna.
- **@qa / producción:** conseguir una máquina o build de prueba capaz de correr PIE (W1). Sin un playtest real del parque, este ritmo de tensión no se puede validar.
- **@level-tools:** mantener `Isidora3D/parque/actores.json` actualizado y convertir el mapa a One File Per Actor (o World Partition) para que las ediciones del parque no reescriban un .umap único de 5,2 MB.
