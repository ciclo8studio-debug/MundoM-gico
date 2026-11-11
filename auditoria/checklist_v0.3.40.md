# ✅ Checklist de prueba — v0.3.40 (CTO · 45–60 min, 12-11)

**Cómo usarla:**
1. Cierra Unreal y el navegador.
2. Abre `Build\v0.3.40\Windows\MundoMagico.exe`, ojalá con control y audífonos.
3. **Borra la partida guardada** (Retroceso en el menú) y entra con **Iniciar juego**, no con Continuar. Así se prueban el glitch de salida y los sustos desde cero.
4. Marca cada punto con ✅, ❌ o ⚠️ y anota una línea si algo falla.

Esta lista revisa lo nuevo entre la v0.3.36 y la v0.3.40. Lo de antes está en `checklist_v0.3.34.md`.

## A. Sensación general (5 min)
- [ ] La linterna se ve cálida, con un cono marcado y polvo en el haz. Con la batería baja parpadea. *(v0.3.36)*
- [ ] La cámara está al hombro, más cerca, y no atraviesa paredes. *(v0.3.36)*
- [ ] Los pasos cambian de sonido según el suelo: baldosa, tierra, madera, metal y charcos. *(v0.3.37)*
- [ ] El eco cambia por zona: la sala suena seca, el pasillo con eco largo y el parque abierto. *(v0.3.39)*

## B. Hospital (15 min)
- [ ] Al recoger cosas **ya no suena "abrir puerta"**. Solo suena al abrir una puerta de verdad. *(v0.3.38)*
- [ ] Al abrir la puerta de la sala aparece la **pantalla de carga** "PASILLO DE URGENCIAS". *(v0.3.38)*
- [ ] Hay **cuerpos** en la sala y en el pasillo de salida, y no flotan ni atraviesan el suelo. *(v0.3.40)*
- [ ] Hay **notas con sangre** en las paredes y papeles arrugados en el suelo, y no estorban al caminar. *(v0.3.40)*
- [ ] Las **páginas** que se recogen son papeles viejos, quemados o con sangre, acostados en el suelo, y se pueden recoger igual que antes. *(v0.3.40)*
- [ ] Hay una **baliza roja girando** en el techo del pasillo de salida. *(v0.3.40)*
- [ ] **Susto del pasillo:** después del destello blanco se encienden **3 balizas rojas**, suena la **alarma**, que se apaga sola, y aparecen un cuerpo y una nota con sangre. *(v0.3.40)*
- [ ] Al salir del hospital ocurre el **glitch rojo** y apareces en la entrada del parque con la pantalla de carga "MUNDO MÁGICO". *(v0.3.38)*

## C. Parque (20 min)
- [ ] **Payaso acosador:** tras unos minutos lo ves **a lo lejos**, quieto y mirándote. *(v0.3.40)*
  - [ ] Te mira de frente, no de lado ni de espaldas.
  - [ ] No queda enterrado ni flotando.
  - [ ] Desaparece al mirarlo fijo o al acercarte, y se oye una respiración detrás.
  - [ ] Mientras está a la vista, **el latido se acelera**.
  - [ ] No aparece tan seguido que moleste.
- [ ] **Isidora fantasma** (teatro, bodega o noria): suena un glitch de radio y el control vibra, la ves a lo lejos y desaparece si te acercas. *(v0.3.40)*
- [ ] **Teatro:** al entrar se enciende un foco sobre una figura en el escenario, se apaga, y la figura **aparece frente a ti** con chillido, vibración y corte a negro. *(v0.3.40)*
  - [ ] La figura queda bien parada en el escenario y el foco la ilumina.
- [ ] **Noria:** al acercarte se ilumina una figura arriba, en la cabina más alta, y luego aparece frente a ti. *(v0.3.40)*
- [ ] **Respawn:** al morir apareces en la atracción más cercana: rueda, autos chocadores, teatro, etc. *(v0.3.35)*
  - [ ] Si mueres cerca de la noria, revisa que el susto no se dispare raro al reaparecer.

## D. Estabilidad (todo el rato)
- [ ] No hay cierres ni pantallas congeladas.
- [ ] Los fps se sienten iguales o mejores que en la v0.3.35.

## Decisión para itch.io (13-11)
- Si **B y D** están bien, se publica la **v0.3.40**.
- Si algo grave falla en el hospital o hay cierres, se publica la **v0.3.35**, que ya tiene ZIP listo.
