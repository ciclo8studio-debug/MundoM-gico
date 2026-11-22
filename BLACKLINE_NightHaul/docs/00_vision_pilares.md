# BLACKLINE: NIGHT HAUL — Visión y pilares

**Fantasía central:** eres el conductor que nadie ve. Un furgón blindado 4x4 mediano, una carga que no debe existir y una noche entera de caminos que no están en el mapa.
**Gancho:** "Como SnowRunner, Y ADEMÁS cada retén es una escena de actuación con tu carga en juego."
**Estética prioritaria (MDA):** Desafío > Fantasía > Sensación > Narrativa emergente.

## Pilares
1. **El peso es el personaje.** La carga cambia cómo se maneja el furgón (masa, centro de gravedad, desplazamiento lateral, integridad). *Test:* si hay que elegir entre un manejo "arcade amable" y uno que haga sentir la carga, gana la carga.
2. **El retén es tensión, no menú.** Inspeccionar es una escena física: frenar, apagar luces, bajar la ventanilla, presentar el manifiesto. La sospecha sale del estado real del furgón (`FSuspicionSnapshot`), no de una barra oculta. *Test:* todo veredicto debe poder explicarse con lo que el jugador hizo o dejó a la vista.
3. **Cada ruta es una apuesta.** Asfalto rápido y vigilado vs. barro y bosque lento y seguro. Terreno, 4x4 y diferencial son decisiones con costo. *Test:* ninguna ruta es estrictamente mejor.
4. **Todo pasa en el tablero.** Cabina primero: luces, limpiaparabrisas, ventanilla, inhibidor, diferencial y encendido se manejan en la cabina. *Test:* si una acción clave exige un menú, se rediseña como control físico.

## Tono
Neo-noir nocturno, íntimo y frío. Mundo ficticio y plausible; las cargas clasificadas son inventadas. Oscuridad real, luz que cuenta (faros, linternas de retén, lluvia en el parabrisas). Audio seco: motor, grava, radio lejana. Poco texto, mucha implicación. Sin heroísmo: el conductor sobrevive, no gana.

## Qué NO es
- No es un juego de combate ni de disparos.
- No es un simulador de camiones de "relajación" (el riesgo siempre es activo).
- No es mundo abierto gigante: rutas compactas y densas.
- No usa marcas, empresas ni cargas reales.
- No hay árbol de habilidades ni RPG pesado en la demo.
- La persecución es de sigilo y huida, no de choques gratuitos.

## Vertical slice: criterios de "se siente bien"
- **Manejo:** a los 30 s el jugador distingue furgón vacío de cargado, y notan el desplazamiento de carga en una curva cerrada.
- **Superficie:** pasar de asfalto mojado a barro se nota en tracción y sonido; sin 4x4 en barro profundo, el furgón se atasca.
- **Cámara:** cabina y tercera persona se alternan sin corte brusco ni mareo.
- **Retén:** un retén dura 30-60 s y produce al menos una decisión de riesgo; los tres veredictos (pase limpio, secundaria, alerta) son alcanzables.
- **Persecución:** se puede perder al perseguidor apagando luces y motor; la tensión sube y baja de forma legible (`Hunting → Searching → Lost`).
- **Legibilidad:** el jugador siempre sabe por qué fue descubierto.
- **Rendimiento:** 60 FPS estables en hardware de gama media (8-16 GB RAM); carga a 20 Hz, sin Tick en actores.
- **Prueba final:** un tester nuevo completa una ruta de 10 min y puede contar una anécdota propia de ella.
