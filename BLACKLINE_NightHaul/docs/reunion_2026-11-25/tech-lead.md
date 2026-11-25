# Reunión 25-11 · @tech-lead · Blackline: Night Haul (v0.1.20, UE 5.8.4, 8 GB + iGPU)

## 1. ¿La historia es viable para el slice de 5 contratos?
**Sí, y casi toda está hecha.** Briefings, bienestar de Chuño, plazo, pruebas de vida y finales ya corren como texto en pantalla. Es solo texto y datos, así que no pesa nada en GPU ni RAM. Lo que falta es poco y barato. Ajustes:
- **Bienestar:** parte en 80% y suma +10% por entrega, así que con 5 entregas llega a 100% sin esfuerzo. Para el slice conviene partir en 60-70% o bajar el bono a +5%, para que fallar un plazo duela.
- **Un contrato por arco:** la «prueba de vida al final del arco» pasa a ser «al cerrar cada contrato». Solo hay que moverla de lugar en el flujo.
- **Riesgo real: las 4 zonas.** El slice usa C01 y C09 (centro), C12 (norte), C18 (sur) y C21 (cordillera). Solo la zona 2 está probada. Las zonas 1, 3 y 4 son el mayor riesgo técnico, mayor que la historia.
- **Sin viaje de mapa en vivo.** Cada contrato carga su zona con `OpenLevel` desde el menú, o se relanza con `-zone=N`. Hay que probar la carga de una zona a otra con la RAM libre antes de prometerla.
- **Audio de la prueba de vida:** hoy no hay ningún sonido. El ladrido y la respiración deben ser grabados o con licencia clara. En el slice basta 1 audio por arco, sin pantalla propia.
- **Decisión moral, finales de policía y arresto:** fuera del slice (ver cortes).
- **Cabina:** el asiento del copiloto vacío con el collar es barato, un solo prop sobre la plantilla. Sí entra (pregunta 4 del doc).

## 2. Packs Fab (criterio: MB en disco, Nanite/Lumen, riesgo de cocción)
Regla general: **jamás agregar un pack entero al proyecto**. Se migran solo los assets que se usan (Migrate) y se mide. Meta: **menos de 1,5 GB de Fab dentro del proyecto**. Los packs de 5 a 19 GB son de interiores de terror y no sirven para una ruta nocturna.

| Pack (MB) | Veredicto | Motivo |
|---|---|---|
| HorrorHospital (18 869), Underground (5 256), Derelict (4 808) | **No usar** | Interiores, miles de assets, materiales pesados. Cocción larga y cero uso en carretera. |
| Detective (2 657), MiscProps (2 475) | **Recortado** | Solo cajas y bultos para la carga, y mobiliario para el restaurante (unos 50 meshes). El resto no. |
| SpaceNebula (1 718), Untitled06 SUW (1 156), Turnstile (191) | **No usar** | Cielo de nebulosa y metro: no encajan. El cielo se resuelve con SkyAtmosphere o cielo propio. |
| SoulCity (1 591) | **Recortado, opcional** | Solo luces parpadeantes y props de puerto para la zona sur, si sobra tiempo. Revisar que no arrastre agua cara. |
| Apocalypse (1 393) | **Recortado** | Solo señalética, conos, barreras y decals de calle para retenes y bencinera. Sin el entorno completo. |
| FreeShrubs (1 374) | **Recortado** | 3-4 arbustos con LOD para bordes de ruta. Sin viento Nanite/foliage experimental: usar LOD tradicional e instancias. |
| Gettingt (1 005) | **Recortado** | Solo props de bencinera o galpón (aire acondicionado, estructuras). Revisar licencia y emisivos. |
| SICKAMOU (672), Stylized (52), Animated (405) | **Recortado** | Cajas y tambores para la carga y la bencinera. De Animated, solo la caja de madera (C12 Reliquias de Madera). |
| clownmon (855), SciFiBeast (396), Whisper x2 (202), RoninGirl (88), Untitled71 (267), Corpses (61) | **No usar** | Personajes y criaturas fuera de tono. Costo de esqueleto sin retorno. |
| FlashLight x2 (768, duplicado), FirstPerson (417), HorrorFramework (115), PortalFX (9) | **No usar** | Plantillas de personaje a pie. El furgón tiene su propia cámara. Un FlashLight sobra: confirmar al CTO antes de borrar. |
| FogArea (27), Free Thunder (15), FreePack rocas (11) | **Usar** | Livianos y útiles. Niebla por zona (medir el costo), truenos para la lluvia y rocas para barro y cordillera. |
| FreeHorror papeles (184) | **Recortado** | Solo las texturas de papel para manifiesto y tablet. |
| DynamicF (68), Essentials (47), AutoFoot (5), Ultimate (156), FreeThumb (15) | **Revisar, bajo riesgo** | Mannequin para el guardia provisorio y utilidades. Abrir solo si hace falta. |
| Biblioteca Fab: carnaval, carrusel, garra, MetaHuman, ChatAvatar | **No usar** | Fuera de alcance. |

**Render:** en la iGPU, Nanite solo en props estáticos recortados. Lumen y Virtual Shadow Maps son el mayor riesgo para los 58 FPS. Propuesta: para el slice, iluminación sin Lumen (luces móviles con pocas sombras, niebla barata) y evaluar Lumen por software en una sola zona. Cada cambio de renderer se mide solo, una vez por build.
**Cocción y empaquetado:** hoy hay una sola sesión de editor por build. Con Fab recortado y Shipping se estima 15-25 min de cocción. Una migración grande o un pack con materiales maestros pesados podrían duplicar ese tiempo.

## 3. Orden de trabajo y cortes para el siguiente hito
1. **Probar en pantalla v0.1.20:** retén, patrullas y teclas de motor/luces (sin probar). Empezar por aquí.
2. **Probar zonas 1, 3 y 4** con `-zone=N`: carga, FPS y RAM. Corregir lo que falle.
3. **Audio mínimo:** motor, lluvia, sirena, trueno (Free Thunder) y 1 audio de prueba de vida. Es lo que más sube la sensación de juego por esfuerzo.
4. **Menú de contratos y cambio de zona** con los 5 contratos.
5. **Arresto simple y finales con texto.** El bienestar a 0% ya existe.
6. **Fab recortado** (cajas, rocas, señalética, mobiliario del restaurante), migrado de a poco y midiendo.
7. **Cabina:** collar y pelota como props. Cabina completa y guardia con modelo, solo si queda tiempo.

**Cortes recomendados:** decisión moral y final de policía (post-slice), pantalla propia de prueba de vida, clima visible en los 5 contratos (solo lluvia y barro), remapeo y localización completa, y los 20 contratos restantes (quedan como datos).

## Decisiones del CTO
- **Render:** ¿se aprueba el slice sin Lumen (o Lumen en una sola zona)? Sin esto, los 58 FPS están en duda.
- **Fab:** ¿tope de 1,5 GB y migrar solo lo listado? ¿Se puede borrar el FlashLight duplicado (768 MB)?
- **Historia:** plazo en tiempo real (recomendado, ya implementado) o por «días»; decisión moral después del slice; collar de Chuño en el copiloto (recomendado: sí).
- **Balance:** bienestar inicial 60-70% para el slice.
- **Audio:** ¿grabación propia o banco con licencia para ladridos y voces?
- **Fecha y plataforma** de publicación del slice (sigue abierto en el tablero).
