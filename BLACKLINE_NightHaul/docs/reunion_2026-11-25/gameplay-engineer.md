# Reunión 25-11 — @gameplay-engineer (Blackline)

Fuentes: docs/24, docs/02, BOARD, INVENTARIO_FAB. Todo número va a `DA_Tuning`/DataAsset, sin Tick nuevo (temporizadores y eventos).

## 1. Coherencia de las mecánicas de la historia
**Lo que funciona:** Chuño como contador emocional, plazo con radio/tablet, pruebas de vida como premio, retén como tensión. Encaja con el loop (retén -> persecución -> fallo).

**Problemas y qué cambiar**
- **Bienestar se llena solo:** +10 por entrega con partida en 80% topa en 100 en 2 encargos y deja de importar. Cambiar: tope por arco (A1 máx 85, A2 90, A3 95, A4 100) y **-5 pasivo por arco** si nunca pasaste por fonda (el respiro tiene valor).
- **Plazo (55 km/h +25%) es trivial** con un furgón de 165 km/h. Subir exigencia por arco: margen 25% (A1), 20, 15, 10, 8% (A5). Reloj **se pausa** en bencinera pero **no** en fonda: la fonda cuesta tiempo real y da +5% bienestar (recuerdo de Chuño). Es un trade-off, no un respiro gratis.
- **Retén falso con costo "Chuño paga" sin números:** definir (ver arresto).
- **Recortar para el slice:** 5 encargos (C01, C09, C12, C18, C21), prueba de vida = texto + 1 audio de radio (sin pantalla propia), finales 1/2/4 primero; el 3 (policía) en v2. Cabina con collar: solo un objeto estático.

## 2. Arresto y decisión moral (reglas concretas)
**Arresto** (nace de `Persecución -> capturado`)
- Captura = velocidad < 10 km/h con 2 o más patrullas a menos de 20 m durante 4 s, o furgón destruido por patrulla. Heat >= 3 acelera: patrulla nueva cada 25 s.
- Costo: pierde la carga (encargo fallido), multa 25% de los viáticos, **Chuño -20%**, +1 "antecedente". Reaparece en el último retén limpio o bencinera visitada (no reinicia el arco).
- 2 antecedentes: la banda baja el margen de plazo 5 puntos. 3 antecedentes: **final malo anticipado** (Chuño no vuelve), salvo que ya tomó la ruta policía.
- Escapar (Lost) con Heat <= 2: sin costo de Chuño, solo -10% integridad si hubo choque.

**Decisión moral: avisar a la policía** (poner al **final del acto 3**, tras la primera pista real; el CTO decide, es la pregunta abierta 3)
- Dónde: teléfono público/radio en la fonda o bencinera; una sola vez, **irreversible**.
- Costo inmediato: Chuño -20%. La banda "sospecha": 25% por encargo (baja a 10% con inhibidor encendido y faros apagados en retén) de que un ojo del retén los delate: Chuño -15% y -1 intento.
- Ruta policía: 3 encargos del acto 4-5 llevan **baliza** en la carga. Se deben entregar con integridad >= 60% y Chuño > 25%. Los retenes oficiales dejan pasar sin arresto (pero sí multa), los de la banda no.
- Éxito: 3 balizas entregadas -> final 3 (la banda cae, Chuño vuelve). Fallo (2 delaciones o Chuño 0): final malo.
- Si no avisa: Final 1 con Chuño >= 70%, Final 2 con 30-69%, final 4 a 0%. Coherente y rejugable: la ruta policía es más difícil y por eso paga más (regla 2-3x de docs/02).

## 3. Packs Fab para mecánicas e IA

| Pack | Veredicto | Uso en Blackline (toque chileno) |
|---|---|---|
| FLashLig (V2 y V4 duplicados, BP_Flashlight, BP_Master_Interact) | **Sí** (usar V4, borrar V2) | Linterna del carabinero/guardia en la cara en el retén; `Master_Interact` para botones del tablero y teléfono de la fonda |
| First-Person Movement Template | **Sí** | Bajarse a pie en bencinera, fonda y retén (caminar a la caja, hablar) |
| DynamicFd / Untitled71 (Manny/Quinn) | **Sí** (placeholder) | Guardia, camionero de la fonda, empleado de bencinera; se retarga después |
| Animated crate (LineTrace/Overlap components) | **Sí** | Cajas de carga que se abren/golpean; interacción en la bodega |
| FREE Horror paper (M_AgedPaper...) | **Sí** | Manifiesto y notas del Patrón en el tablero |
| Free Thunder Sounds | **Sí** | Único audio disponible: tormenta del sur/acto 4 |
| AutoFoot (69 assets) | **Sí** | Pasos a pie (gravilla de bencinera, piso de fonda) |
| FogArea | **Quizá** | Niebla en cordillera; medir GPU (iGPU) |
| The Horror Framework (3rd person, controller) | **Quizá** (solo mirar patrones) | No adoptar el controlador; sacar solo ideas de interacción/guardado |
| Apocalypse Playground, Stylized boxes, SICKAMOU boxes | **Quizá** | Letreros, cajas y lámparas para bencinera/fonda; ojo con tamaño (1,4 GB) |
| Turnstile, Misc Props, Soul City, Derelict, Undergro | No | Estilo metro/fantasía, no calzan con ruta de Chile |
| Horror Hospital (18 GB), Corpses, Clown, Whisper, SciFiBeast, Portal, Space, Carousel, Claw, Punk/Vernal | **No** | Sin uso; además rompen el tono neo-noir y pesan |
| ChatAvatar plugin | No | Sin IA conversacional en este slice |

**Vacío importante:** ningún pack trae vehículos, patrullas ni IA de tráfico; hay que seguir con el furgón de plantilla y las patrullas propias en C++. Para el guardia, una máquina de estados simple (Esperando -> Inspeccionando -> Veredicto) basta; sin Behavior Tree.

## 4. Cinco entregables priorizados
1. **Arresto y antecedentes** (captura 4 s, multa, Chuño -20%, reaparición en último retén) en C++ + interruptor en `DA_Tuning`.
2. **Retocar bienestar y plazo** (topes por arco, margen decreciente, pausa solo en bencinera, +5% en fonda).
3. **Prueba del retén en pantalla** (v0.1.20 sin probar) + guardia con linterna V4 y `Master_Interact` en tablero/teléfono.
4. **Decisión moral + ruta policía** (teléfono en fonda, 3 balizas, final 3); si falta tiempo, queda en la 2ª demo.
5. **Primer audio** (Thunder + pasos AutoFoot + radio con prueba de vida) y caminar a pie en bencinera/fonda con el template de primera persona.
