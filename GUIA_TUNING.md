# Guía de afinación para el CTO: ajusta el juego tú mismo

## 1. Cambiar valores: `DA_Tuning`
1. Abre Unreal en modo liviano con `Abrir_MundoMagico_Liviano.bat`.
2. En el Content Browser ve a **`MundoMagico/Datos/DA_Tuning`** y ábrelo con doble clic.
3. Cambia los números o marca y desmarca las casillas. Luego **Guarda** con Ctrl+S.
4. Cierra Unreal y haz la build con `herramientas\Construir_Version.bat`. Después pruébala con `Probar.bat`.

| Valor | Qué hace | Hoy | Rango seguro |
|---|---|---|---|
| **Payaso_Activo** | Enciende o apaga las apariciones del payaso acosador | ✅ | — |
| Payaso_EsperaInicial | Segundos desde que llegas al parque hasta la primera aparición | 60 | 30–180 |
| Payaso_CadaMin / CadaMax | Segundos entre una aparición y la siguiente (al azar entre ambos) | 40 / 80 | 20–180 |
| Payaso_MaxVeces | Cuántas veces aparece por partida | 6 | 1–12 |
| Payaso_DistMin / DistMax | A qué distancia aparece, en cm (2200 cm = 22 m) | 2200 / 3200 | 1200–5000 |
| Payaso_DistHuye | Si te acercas a menos de esta distancia, desaparece | 1500 | 600–2500 |
| Payaso_MiradaSeg | Segundos mirándolo fijo antes de que desaparezca | 1,0 | 0,3–3 |
| Payaso_DuracionSeg | Segundos máximos que se queda a la vista | 9 | 3–20 |
| **Isidora_Activa** | Enciende o apaga las apariciones de Isidora | ✅ | — |
| Isidora_DistMin / DistMax | Distancia a la que aparece, en cm | 1000 / 3500 | 600–5000 |
| Isidora_DistDesaparece | Si te acercas a menos de esta distancia, se esfuma | 900 | 300–1500 |
| Latido_Alcance | Desde qué distancia de un enemigo empieza a sonar el latido, en cm | 2500 | 1000–5000 |
| Latido_VolMax | Volumen máximo del latido | 1,0 | 0,3–1,5 |
| **SustoTeatro_Activo** | Enciende o apaga el susto del teatro | ✅ | — |
| **SustoNoria_Activo** | Enciende o apaga el susto de la noria | ✅ | — |
| **Director_Activo** | Enciende o apaga el Director de miedo. Si está apagado, cada susto va por su cuenta | ✅ | — |
| Director_Enfriamiento | Segundos mínimos entre un susto y el siguiente | 40 | 20–120 |
| Director_Decae | Cuánto baja la tensión por segundo | 1,5 | 0,5–5 |
| **Camara_PrimeraPorDefecto** | El juego arranca en 1ª persona (desmárcalo para volver a la 3ª) | ✅ | — |

> **Si algo falla en un playtest,** apágalo desmarcando su casilla y haz la build. No hace falta tocar ningún Blueprint.

## 2. Probar rápido: `Probar.bat`
Haz doble clic en **`Probar.bat`**, en la carpeta del proyecto. Abre la build más reciente y te deja elegir dónde empezar:

| Opción | Zona | Uso típico |
|---|---|---|
| 1 | Hospital, la sala del despertar | Inicio y linterna |
| 2 | Pasillo de Urgencias | Susto del pasillo, balizas y alarma |
| 3 | Entrada del parque | Llegada y payaso acosador (espera 60 s) |
| 4 | Teatro | Susto del teatro |
| 5 | Noria | Susto de la noria e Isidora |
| 6 | Bodega | Isidora de la bodega |
| 7 | Autos chocadores | Respawn y avenida |
| 8 | Juego normal | Partida completa |

- El juego arranca solo y en 2 segundos te deja en la zona elegida, con el mensaje "PRUEBA: zona".
- Arriba a la izquierda aparece el **panel de prueba**, con **fps, posición X/Y y batería**. Anota los fps de cada zona.
- Sin `Probar.bat` (doble clic en `MundoMagico.exe`), el juego es el normal y no muestra el panel.

> **Ojo:** al saltar se carga la partida guardada, si existe. Para una prueba "en limpio", borra la partida en el menú con **Retroceso** antes de usar `Probar.bat`.

## 3. Lo que se ajusta en el editor sin `DA_Tuning`
Se cambia en el panel **Details** de cada actor del mapa:
- **Luces** (`PointLight`, `SpotLight`): cambia solo Intensity, Attenuation Radius y Light Color. **Cast Shadows** siempre apagado, porque la iGPU no aguanta las sombras.
- **Post-proceso** (`PostProcessVolume`):
  - Exposure: Min = Max, entre −0,5 y +0,5.
  - Vignette: 0,4–0,6.
  - Grain: 0,2–0,3.
  - Saturation: 0,85.
- **Mover objetos:** usa **End** para apoyarlos en el suelo.
- Antes de cambiar algo, toma una captura de cómo estaba.

## 4. Lo que todavía pide a Claude
- La batería de la linterna, por ahora 0,22 % por segundo, sigue dentro del Blueprint del jugador. Pasa a `DA_Tuning` en la próxima fase.
- Cualquier lógica nueva: sustos, IA o mecánicas.
