# Kit de testers · RC1 de Mundo Mágico (privado, hacia el 26-11)

**Para el CTO:** copia el mensaje de la sección 1 a cada tester (3 a 5 personas, al menos 2 con iGPU o notebook sin tarjeta dedicada). Respuesta en 48 h. Reemplaza a `formulario_testers.md` (13–15-11).
**Antes de mandarlo:** completa `[LINK]`, `[CLAVE]` y `[VERSIÓN]` con la build RC1 que pase la prueba de humo (`auditoria/humo/rc1_2026-11-24.md`). **Plan B (23-11):** la demo es hospital más Acto 3, sin parque ni Casona (versión 0.3.109).

---

## 1. Mensaje para el tester (copiar y pegar)

Hola. Gracias por probar **Mundo Mágico: Turno Nocturno**, un juego de terror que estamos terminando. Esta es una versión de prueba privada: por favor no la compartas ni subas videos todavía.

**Qué necesitas**
- Windows 10 u 11 de 64 bits, 8 GB de RAM o más y unos 5 GB libres. Funciona en notebooks sin tarjeta de video dedicada, pero va justo.
- Audífonos. Control (tipo PlayStation o Xbox) opcional; con teclado y mouse también se juega.

**Cómo instalarlo**
1. Descarga el ZIP: `[LINK]` (clave: `[CLAVE]`). Pesa cerca de 1,3 GB.
2. Descomprímelo en una carpeta tuya (no en "Descargas" si tienes poco espacio) y abre `MundoMagico.exe`.
3. En **Configuración** deja la calidad en **Baja**. Si se ve muy lento, avísanos, no la subas.

**Cómo se juega**
- Teclado: WASD moverse, mouse mirar, **F** linterna, **E** interactuar o recoger, **H** pista. Agacharse y correr: `[CONFIRMAR TECLAS]`.
- Control: ✕ saltar, □ interactuar o recoger, △ pista.
- Juega **de corrido 30 a 40 minutos**: el hospital y el pasillo de Urgencias, hasta la pantalla **FIN DE LA DEMO**. Esta versión termina ahí; el parque y la casona no están en la demo.
- Si algo se cierra o se congela, vuelve a abrir el juego y sigue. **Continuar** retoma tu partida.

**Qué hacer cuando falle algo**
1. Anota qué estabas haciendo y a qué hora.
2. Abre el Explorador y pega esta ruta: `%LOCALAPPDATA%\MundoMagico\Saved`
3. Comprime las carpetas **Logs** y **Crashes** en un ZIP y mándalo junto con tus respuestas.

Cuando termines, responde las preguntas de abajo. Gracias.

---

## 2. Preguntas para el tester

1. **Tu PC.** Modelo o tarjeta de video (por ejemplo "notebook con Intel Iris"), RAM y Windows 10 u 11.
2. **¿Cuántos minutos jugaste y hasta dónde llegaste?** (sala del despertar, pasillo, televisor, pantalla FIN, otra)
3. **¿El juego se cerró, se congeló o se puso muy lento?** ¿Dónde? Si viste los fps, anótalos.
4. **¿Quedó la imagen muy oscura, muy clara o borrosa en algún lugar?** ¿Dónde?
5. **¿Isidora se movió sola o quedó pegada en algún momento?** ¿Qué estabas haciendo?
6. **¿Se te cayó la partida o Continuar no te dejó donde estabas?**
7. **¿Hubo algún momento en que no supiste qué hacer o hacia dónde ir?** ¿Dónde?
8. **¿Qué momento te dio más miedo?** Una línea.
9. **¿Algo se veía raro?** Objetos flotando, paredes que se atraviesan, textos cortados, luces extrañas. Captura si puedes.
10. **Del 1 al 5, ¿cuánto te gustó y cuánto querrías jugar la versión completa?** ¿Qué cambiarías primero?

---

## 3. Qué mide el estudio (para @qa-publisher)

| Métrica | Pregunta | Meta para el 10-12 |
|---|---|---|
| Cierres o congelamientos | 3 | **0**; uno solo detiene la publicación |
| Rendimiento en iGPU | 1 y 3 | 25 fps o más en Baja |
| Imagen oscura, clara o borrosa | 4 | Ningún tester la reporta en el recorrido principal |
| Isidora se mueve sola o se pega | 5 | 0 reportes (arreglado en 0.3.105; falta confirmar) |
| Partida y Continuar | 6 | 0 fallas |
| Dónde se pierden | 7 | Ajuste de guía y pistas |
| Susto que mejor funciona | 8 | Sirve para el Director de miedo |
| Bugs visuales | 9 | Triage 🔴 / 🟠 / 🟡 |
| Nota | 10 | Promedio de 3,5 o más |

## 4. Fechas (plan del 18-11, ajustado al 23-11)
| Fecha | Hito |
|---|---|
| 24-11 | Congelar la Casona visual y armar la RC1 |
| 25-11 | Humo A9 del CTO (`rc1_2026-11-24.md`) y decisión del G0 del parque |
| 26-11 | Envío a testers |
| 28-11 | Respuestas y logs; triage 🔴 / 🟠 / 🟡 |
| 3-12 | RC2 con las correcciones |
| 8-12 | Gold |
| 10-12 | Demo pública en itch.io |

## 5. Pendientes del kit
- Probar en limpio el ZIP de la RC1 (descomprimir en otra carpeta y abrir) antes de enviarlo.
- Confirmar las teclas de agacharse y correr (no están documentadas en el repo; revisar las acciones de Input del jugador) y ponerlas en la sección 1.
- Decidir si se entrega la versión `_dev` (con panel de FPS) a los testers con iGPU; es más fácil medir, pero muestra números en pantalla.
