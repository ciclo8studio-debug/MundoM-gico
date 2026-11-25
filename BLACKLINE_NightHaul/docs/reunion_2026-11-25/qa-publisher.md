# @qa-publisher: QA, publicación e idiomas (reunión 25-11-2026)

Alcance: solo Blackline. Lo legal es criterio de QA/publicación, no asesoría jurídica; lo marcado `[VERIFICAR]` se confirma contra el texto vigente de Fab/itch.io antes de publicar.

## 1. Riesgos de la historia para publicar

| Tema | Riesgo | Medida |
|---|---|---|
| Crimen organizado y extorsión | Medio. Es el eje de la trama; en itch.io no hay clasificación formal por edad, se declara en la ficha. | Marcar «contenido maduro / temas de crimen» en la ficha y en la pantalla de aviso inicial. Sin glorificar: Mateo es obligado (ya está en el doc 24). |
| Animal en peligro (Chuño) | Medio-bajo. Nunca se muestra herido; el miedo viene de audio y mensajes. | Mantener la regla como criterio de aceptación de QA: ningún asset, audio ni texto con maltrato explícito. Las «pruebas de vida» no llevan quejidos de dolor, solo ladridos y respiración. Aviso: «amenaza implícita a un animal; no se muestra daño». |
| Lugares reales de Chile | Bajo. Geografía real es libre. | Mantener nombres de lugares como escenario, sin acusar a personas ni comunas reales de delitos. |
| Instituciones reales | Medio. Carabineros, PDI y similares tienen marca y uniforme reales. | Usar **policía ficticia** (nombre, escudo y uniforme propios). Sin patentes reales ni logos de marcas de bencina, camiones, bancos, supermercados o fondas. Revisar carteles de bencinera y restaurante. |
| La Cofradía / personajes | Bajo. Todo inventado. | Dejar el descargo «personajes, empresas y organizaciones son ficticios» en créditos y en la ficha. «La Cofradía» es nombre provisional: buscar coincidencias con empresas reales antes de fijarlo. |
| Decisión moral (avisar a la policía) | Bajo. Pendiente de construir. | Que ninguna rama muestre a la policía como villana real; cuidar el tono. |

Advertencias de contenido sugeridas (ficha y pantalla inicial, es/en): crimen organizado, extorsión y amenazas, animal en peligro (sin daño mostrado), conducción nocturna con persecuciones, lenguaje fuerte `[confirmar con los textos finales]`. Además, un aviso de fotosensibilidad si hay luces de policía o destellos, y los del doc 17 (sin tráfico real, ficción).

## 2. Packs Fab: licencia y publicación

Bases `[VERIFICAR]`:
- La licencia estándar de Fab permite usar los assets en un juego comercial, **pero solo dentro del producto empaquetado**. No se pueden redistribuir sueltos ni subir los `.uasset` al repo público, a itch.io como descarga de assets, ni a un pack propio.
- Packs «Solo Unreal Engine» no se pueden sacar a otros motores. Los CC-BY exigen atribución en créditos. Los «de muestra/proyecto de muestra» y los «gratis de promoción» pueden traer límites propios: leer la ficha de cada uno, no asumir.
- El repo del estudio es privado, pero **no incluir contenido Fab en commits de código ni en ningún zip público** que no sea la build cocinada.

| Pack / grupo | Lectura para Blackline | Riesgo de publicación |
|---|---|---|
| ChatAvatar_Plugin | **No usar.** Plugin de IA que llama a un servicio en la nube. | Alto: términos del servicio, envío de datos, divulgación de IA en itch.io/Steam. Fuera del build y de `.uproject` salvo que el CTO lo apruebe tras leer términos. |
| Models_of_people_x_5, PunkGirlBundle, Vernal Jumpsuit (MetaHuman), Whisper, clownmon | No encajan con la historia. Personas escaneadas o con rostro. | Medio: los MetaHuman tienen licencia propia y límites; modelos de personas reales exigen que el contrato lo permita. Mejor no incluir. |
| Corpses (muestra), FREEHorror (papeles con sangre) | Contenido gráfico que pesa en la clasificación y no aporta. | Medio: se descarta por tono y por «de muestra». |
| Horror Hospital, Derelict, Underground, Horror Framework, HorrorHouse (18 GB) | Fuera de tema; cargan peso y riesgo de marcas/texto. | Bajo en licencia, alto en tamaño. No importar al proyecto. |
| Detective Office, Misc Props UB Free, Stylized props, SICKAMOUNT, Apocalypse (Playground), Soul City | Candidatos para restaurante, bencinera y calles. | Revisar texturas con carteles, logos, rostros o texto legible en inglés antes de integrar. |
| Free Thunder Sounds, Free Shrubs, FreePack Rocks, Fog Area, First-Person template | Útiles (lluvia, vegetación, ambiente). | Bajo. Registrar en `CREDITOS.md`. |
| Mannequin, FlashLight, Space Nebula, Portal FX, Claw Machine, Carnival/Carousel | Sin uso en una ruta de camión. | No importar. |

Tamaño: el VaultCache suma unos 45 GB, la mayor parte en tres paquetes (Horror Hospital 18,9 GB, Underground 5,3 GB, Derelict 4,8 GB). Importar un pack entero al proyecto infla el repo y el cook. Regla: **migrar solo los assets usados** (Migrate), no el pack. Presupuesto propuesto: build cocinada ≤ 4 GB y zip de descarga ≤ 2,5 GB `[CONFIRMAR con el CTO]`; medir tras el primer cook, porque UE comprime distinto según texturas.

Qué NO se redistribuye: archivos fuente de los packs, el contenido sin cocinar, plugins en código con licencia de terceros, y audio con licencia solo para uso en producto. Hacer un `CREDITOS.md` con autor, pack y licencia de cada asset usado.

## 3. Plan de pruebas del próximo hito

Idioma principal: español de Chile (voseo y modismos medidos: «bencinera», «fonda», «retén», «quiltro», «viáticos»). Plan es/en: todo texto de la tablet, HUD, briefings y aviso inicial en una tabla de cadenas (`StringTable`) desde ya; en el hito siguiente solo es-CL y la tabla lista; en/neutral al hito de la demo `[CONFIRMAR]`. Prueba de idioma: cadena sin traducir, texto cortado en HUD, tildes y «ñ» (Chuño, Cofradía) en la fuente.

| Área | Casos clave (pasa/falla) | Sev. si falla |
|---|---|---|
| Volcado (3 s) | Volcar en asfalto y barro: la cuenta de 3 s corre y repone; no repone en movimiento ni dos veces; carga y plazo se conservan; Chuño no baja por volcar; sin repetir el volcado seguido (bucle). | S2 |
| Retén y manifiesto | Pasar limpio, con faros apagados, con carga sin blindar y con manifiesto vacío; veredicto estable; manifiesto automático correcto en cada zona; arresto baja Chuño. | S2 |
| Persecución propia | Patrulla inicia, pierde y vuelve al ver; apagar motor y luces; dos patrullas sin duplicar timers; termina limpio al cambiar de mapa. | S2 |
| Bencinera | Cargar combustible y pagar con viáticos; sin viáticos; salir con la manguera; restaurante: diálogo de Chuño una vez por local. | S3 |
| Combustible | Se gasta en forma monótona; vacío detiene el camión con aviso; recarga llega a 100 %; valores coherentes en el HUD. | S2 |
| Plazo y Chuño | El plazo cuenta, vence y resta 15 %; entrega a tiempo suma 10 %; carga perdida resta 25 %; bienestar llega a 0 % y dispara el final malo; bienestar nunca fuera de 0-100; sin acentos rotos en mensajes. | S1 si bloquea el avance, S2 si no |
| Mando PS4 | Detectar mando, mapa de botones, zonas muertas, gatillos analógicos para acelerar/frenar, vibración opcional, menú navegable sin mouse, teclas del HUD con iconos del mando, desconectar y reconectar a mitad de juego. | S2 |
| Humo y estabilidad | Build abre, ruta de 30 min, 5 persecuciones, sin crash; FPS en el PC de 8 GB (≥ 30 en Baja). | S1 |

Regla de pruebas del estudio: solo builds empaquetadas, un proceso a la vez, sin PIE.

## 4. Cinco entregables priorizados

1. **`CREDITOS.md` + auditoría de assets usados** (licencia, autor, atribución; sin ChatAvatar, sin personas escaneadas). Bloquea la publicación.
2. **Ficha itch.io borrador**: descripción, advertencias de contenido, descargo de ficción, nota de IA (ninguna), requisitos mínimos medidos.
3. **Revisión de marcas y policía ficticia**: lista de carteles, patentes, logos y uniformes en bencinera, restaurante, tráfico y patrullas.
4. **Tabla de cadenas es-CL (+ esqueleto en)** y casos de texto/acentos en el plan de pruebas.
5. **Suite de humo del hito** con los casos de la sección 3, más medición de tamaño del primer cook (build y zip) contra el presupuesto.

Pendientes del CTO: presupuesto de tamaño, destino (itch.io decidido, Steam después) y `cert_tier` del doc 17, que sigue «NOT ASSESSED».
