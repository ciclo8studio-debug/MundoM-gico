# Propuestas @gameplay-engineer: mecánicas, movilidad e IA (2026-11-09)

**Regla técnica para todas:** nada nuevo dentro del BP de jugador de 10 MB. Cada mecánica va en su propio **ActorComponent** (`BPC_Sigilo`, `BPC_Movilidad`, `BPC_Mando`) o en el `AIC_Enemigo`. Así se prueba aislada en builds Shipping y el monolito no crece.
**Sprints:** S2 = 16-22 nov · S3 = 23-29 nov · S4 = 30 nov-6 dic · S5 = 7-10 dic (cierre v0.5.0).

| # | Propuesta | Impacto | Esfuerzo | Sprint |
|---|---|---|---|---|
| 1 ⭐ | Payaso stalker con Behavior Tree y director de tensión | Alto | L | S2-S3 |
| 2 ⭐ | Escondites que funcionan y contener la respiración | Alto | M | S2 |
| 3 ⭐ | Linterna que delata y pilas escasas | Alto | S | S2 |
| 4 | Muñeca que solo se mueve si no la miras | Alto | S | S3 |
| 5 | Payaso que imita sonidos | Medio | M | S4 |
| 6 | Voltereta de escape | Medio | M | S3 |
| 7 | Pegarse a la pared y asomarse | Medio | M | S4 |
| 8 | Sostener y bloquear puertas | Alto | M | S4 |
| 9 | Mando PS4 como instrumento de miedo | Medio | S/M | S2 (vibración) y S4 (luz y panel) |
| 10 | Crafteo mínimo con rueda radial | Medio | M | S5 |

---

### 1 ⭐ Payaso stalker con Behavior Tree y director de tensión
- **Qué siente el jugador:** “me sigue, pero no sé dónde está”. El payaso no persigue sin parar. Se acerca, se deja ver de reojo, se retira y vuelve. Cuando la tensión está al máximo, Isidora recibe un respiro de unos 60 segundos.
- **Cómo:** se migra la FSM de Tick a `AIC_Enemigo`, `BB_Enemigo` y `BT_Enemigo`, según `diseno_bt_enemigo.md`. Se suma un **director**: una variable `Tensión` de 0 a 100 que depende de la distancia, de si el payaso es visible y del tiempo sin verlo. Con tensión baja, el payaso recibe un punto EQS cerca de Isidora pero fuera de su vista. Con tensión alta, se retira a `Casa`. Persigue solo cuando la ve.
- **Assets:** las animaciones `clown_monster` (`anim_rage`, `anim_Walk_Back`, `anim_Turn_*` y `anim_idle_1-3`). También el `MakeNoise` de la botella, que ya existe.
- **Riesgo:** sin PIE, el BT se depura a ciegas. Hace falta un HUD de debug en Shipping (tecla F9) que muestre el estado y la tensión.

### 2 ⭐ Escondites que funcionan y contener la respiración
- **Qué siente el jugador:** el pánico de estar dentro del casillero mientras el payaso olfatea a 1 metro.
- **Cómo:** se rehace `BP_Hiding_Door`. El fallo actual es que el jugador se atasca porque la cápsula sigue colisionando. Al esconderse, el sistema guarda la transformada, desactiva la colisión y el movimiento, cambia la cámara a una fija con la rendija y anima la entrada. Al salir, restaura todo.
- Contener la respiración (mantener R2) consume el aliento que ya existe. Si el aliento se acaba, Isidora jadea, emite `MakeNoise` y el enemigo la descubre.
- El BT sabe si vio a Isidora entrar. Si la vio, abre el escondite. Si no, solo revisa.
- **Assets:** casilleros, camas y armarios de `Horror_Hospital`, y en el parque las casetas de feria. El latido y el aliento ya existen.

### 3 ⭐ Linterna que delata y pilas escasas
- **Qué siente el jugador:** la luz es su única seguridad y también lo que lo mata. Apagarla en la oscuridad es una decisión dura.
- **Cómo:**
  - **Visión del enemigo:** con la linterna encendida, el radio de vista de AIPerception aumenta un 60 %. El cono de luz que toca al enemigo cuenta como “visto”.
  - **Aviso de proximidad:** con el enemigo a menos de 8 m, la linterna parpadea (ya existe el parpadeo de las luces del parque).
  - **Recursos:** se baja el drenaje de batería y se ponen menos pilas: 2 o 3 por zona.
  - **Recarga:** recargar a mano (sacudir la linterna) da un 5 % de batería a cambio de ruido.
- **Assets:** `FlashLight_System` y la batería y las pilas actuales.

### 4 Muñeca que solo se mueve si no la miras
- **Qué siente el jugador:** paranoia. Gira la cámara y la muñeca está más cerca. No quiere dejar de mirarla, pero tiene que avanzar.
- **Cómo:** cada 0,1 s se calcula el producto punto entre el forward de la cámara y la dirección a la muñeca, más un line trace y `WasRecentlyRendered`. Si no la miran, avanza a saltos con un “clic” de porcelana. Con la linterna encima, se queda congelada. Si la linterna está apagada, la muñeca acelera. Así se conecta con la propuesta 3.
- **Assets:** la muñeca actual del Carrusel y su FSM, que es simple y no necesita BT.

### 5 Payaso que imita sonidos
- **Qué siente el jugador:** desconfía de sus oídos. ¿Esa botella la tiró él? ¿Es Luisa la que llama?
- **Cómo:** el payaso reproduce en otra posición sonidos que el jugador ya conoce: la botella rota, los pasos de Isidora, la risa infantil o “¿Isidora?”. Es una trampa que lleva al jugador hacia él. Hay una regla aprendible: la imitación suena **con un leve eco o desafinada**.
- **Assets:** los SFX de botella y pasos, `Thunder_Sounds` para cubrir, y las voces si hay doblaje.

### 6 Voltereta de escape
- **Qué siente el jugador:** una salvada heroica en el último segundo, que se paga cara.
- **Cómo:** ○ con dirección hace una voltereta con root motion e i-frames de 0,3 s, y consume el 40 % del aliento. Si Isidora corre, puede zafarse de un agarre una sola vez por encuentro. No sirve como spam porque el aliento ya está gastado.
- **Assets:** `DynamicFalling` (`RM_Roll_front/back/left/right/45°`, `RM_Roll_Run_F`, `AS_hard_landing` y `AS_falling_to_roll`), que está archivado. Requiere retarget al esqueleto mannequin de Isidora, que es compatible.

### 7 Pegarse a la pared y asomarse
- **Qué siente el jugador:** sigilo táctico. Ve la ronda del payaso sin exponerse.
- **Cómo:** junto a una pared, □ o L1 pega a Isidora a ella con un trace normal. El stick mueve en lateral y en el borde la cámara se asoma. Es un estado del `BPC_Movilidad`, no se trepa. Trepar queda descartado: el costo de animación es alto y el parque no está diseñado para eso.
- **Assets:** `RoninGirl` (`AS_stand_to_cover`, `AS_cover_to_stand`, `AS_left/right_cover_sneak` y `AS_crouched_sneaking_left/right`), que está archivado. Requiere retarget.

### 8 Sostener y bloquear puertas
- **Qué siente el jugador:** forcejeo físico al estilo Amnesia. La puerta empuja, el mando vibra y hay que machacar □.
- **Cómo:**
  - **Forcejeo:** la puerta tiene un ángulo controlado por las fuerzas `JugadorEmpuja` y `EnemigoEmpuja`. Si gana el enemigo, la puerta se abre de golpe. Si gana el jugador, el enemigo se rinde y busca otra ruta (BT: `Volver`).
  - **Bloqueo:** se puede arrastrar una caja o un mueble frente a la puerta para bloquearla un tiempo.
  - **Prueba en la demo:** la puerta de la Casa de Alfredo durante el apagón.
- **Assets:** `AnimatedWoodenCrateV1`, las puertas de `Horror_Hospital` y `DetectiveOffice`.

### 9 Mando PS4 como instrumento de miedo
- **Qué siente el jugador:** el miedo pasa a las manos.
- **Cómo:**
  - **Vibración (S, en S2):** el latido se traduce en un pulso doble en el motor izquierdo, con frecuencia según la tensión de la propuesta 1. También vibra en los pasos cercanos del enemigo y en el forcejeo de puertas (propuesta 8). Usa Force Feedback Effects nativos y funciona en PC sin plugin.
  - **Panel táctil:** abre el diario y las notas. Deslizar cambia de página.
  - **Lightbar (M, en S4, con riesgo):** el color indica la batería (blanco → ámbar → rojo) y parpadea con la muñeca cerca. En PC requiere un plugin DualShock o Steam Input, así que hay que validar antes de comprometerlo.
- **Assets:** ya está implementado el mapeo completo del mando.

### 10 Crafteo mínimo con rueda radial
- **Qué siente el jugador:** gestión tensa. ¿Gasto la cinta en la pila o en la botella?
- **Cómo:** hay solo 3 recetas.
  - Pila gastada + cinta = pila al 50 %.
  - Botella + trapo = botella incendiaria. Aturde 4 s y es el único “ataque”.
  - Fusible + cable = fusible improvisado, una ruta alternativa a la caja fuerte.
- La rueda radial pausa al 30 % del tiempo, no al 0 %, para que la tensión se mantenga.
- **Assets:** el plugin `RadialWheel`, `FreeSurvivalIconsKG` y la mochila actual.

---
**Orden de dependencias:** 3 → 2 → 1. La linterna y el escondite dan reglas claras al BT. Después 4 (independiente), luego 8 y 9, y al final 5, 6, 7 y 10 si queda tiempo. Hay que dejar fuera del 10-12 lo que no esté validado en Shipping antes del S5.
