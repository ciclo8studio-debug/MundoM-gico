# 07 — Mundo: BLACKLINE: NIGHT HAUL

> **SUPERADO el 2026-11-22 por `24_historia_secuestro.md`.** Se conserva como historial; no usar para nuevos textos.


Canon: Provisional (a la espera de narrative-director). Visible al jugador: Discoverable salvo donde se indica.
Fuente: `CONTRATO.md` (tipos, checkpoint, persecución, carga, terreno). Todo ficticio; sin marcas reales.
Contradicciones: ninguna con CONTRATO.md (clases de carga, veredictos, superficies, Signal Jammer, 4x4).

## 1. Región: La Cuenca de Veldmark

Cuenca montañosa ficticia, fría y húmeda, cerrada por la cordillera Sorrel al norte y el estuario de Haldane al sur. Una sola autopista, la **Ruta 9 "Cinta"**, une el puerto interior de **Karrow** con las ciudades-fábrica del este. Fuera de ella hay caminos de grava, barro de turbera y bosque cerrado de pino negro.

- Clima: lluvia casi constante de otoño tardío, niebla de valle, noches largas. Justifica `WetAsphalt`, `Mud`, limpiaparabrisas y faros como mecánicas.
- Economía: la Cuenca es un cuello de botella; todo bien de alto valor pasa por la Cinta o no pasa.
- Gobierno: la **Mancomunidad de Veldmark**, federación débil de distritos, con aduana centralizada y corporaciones con poder real.

## 2. Facciones

### Aduana — Dirección de Tránsito y Gravamen (DTG)
- Motivación: control fiscal y de seguridad de la carga; cuotas de incautación.
- Poder: burocrática y jerárquica. Inspectores de puesto, supervisores de zona, mando de persecución (patrullas "Gavilán").
- Personalidad: cortés, lenta, implacable con la papelería inconsistente. Cree en los manifiestos, no en las personas.
- Mecánica asociada: `ACheckpointController`, veredictos (`CleanPass`, `SecondaryInspection`, `PursuitAlert`), `UPursuitDirectorSubsystem`.
- Territorio: puestos de control en la Cinta y accesos al puerto.

### Empleador — Consorcio Orlane Meridiano
- Motivación: mover carga sin que figure; negación plausible total.
- Poder: lógica mercantil. Un despachador anónimo ("la Mesa") asigna rutas, manifiestos y pago en créditos (`ValueCredits`). Pierde al conductor, no la carga.
- Personalidad: amable, cálido por radio, frío en los hechos. Paga bien y a tiempo; no rescata.
- Mecánica asociada: `UManifestDataAsset` (empresa fachada, `ForgeryQuality01`, `RouteCode`).
- Territorio: bodegas en Karrow y depósitos de relevo sin nombre.

### Contrabandistas — Los Cuervos de Turba
- Motivación: autonomía; evadir tanto a la DTG como al Consorcio.
- Poder: red flexible de clanes de camineros. Sin jefe único; prestigio por rutas conocidas.
- Personalidad: supersticiosos, prácticos, solidarios con quien conoce el barro. Hostiles con quien huele a Aduana.
- Mecánica asociada: caminos secundarios (`Gravel`, `Mud`, `Forest`), `bRequires4WD`, trucos de blindaje y del bloqueador.
- Territorio: turberas, aserraderos abandonados, sendas del bosque.

### Relaciones
- DTG vs Cuervos: guerra de baja intensidad.
- Consorcio usa a ambos: soborna a la DTG a nivel local y compra información a los Cuervos.
- El jugador (conductor) no pertenece a ninguna facción; es prescindible para las tres.

## 3. Reglas del mundo (ciencia ficción sutil)

Época: cercana y ligeramente alterada. Nada es abiertamente imposible; lo extraño se insinúa.

1. **Firma de carga:** cierta carga emite una huella detectable por sensores de puesto ("firma"). Sin blindaje, la huella se filtra (`UnshieldedSignature01`, `bRequiresShielding`). Se entiende como radiación de fondo "no declarada"; nadie explica su origen.
2. **El bloqueador de señal (Signal Jammer):** dispositivo ilegal que satura los sensores de la DTG. Tiene costo: llama la atención de equipos de escucha y no oculta la firma física de la carga, solo la señal. Su uso sale en la instantánea de sospecha (`bSignalJammerOn`).
3. **Clases de carga:** Standard, Fragile, Volatile, Cryogenic, Relic y AIServer. Reliquias ("Relic") son objetos de origen incierto anteriores al Corte; los servidores de IA ("AIServer") están restringidos por ley y son lo más caro y vigilado.
4. **El Corte:** hace unos veinte años una falla de comunicaciones inexplicada borró archivos y redes de la Cuenca. Por eso el papel manda sobre lo digital y las IA están reguladas. Es la raíz de la cultura del manifiesto. Misterio abierto: no se confirma causa (Hidden).
5. **Calor de persecución:** la DTG rastrea por motor encendido y luces (`ReportEngineAndLights`). Apagar ambos y quedarse quieto en la oscuridad puede hacer que pierdan el rastro: sigilo en lugar de velocidad.
6. **Tecnología:** vehículos mecánicos, tablero analógico con botones táctiles; sin navegación satelital fiable (consecuencia del Corte). Conducción por reconocimiento de ruta y notas de ruta.
7. **Física de la carga:** la masa y el centro de gravedad importan; la carga frágil se daña y se desplaza (`FOnCargoShifted`).

Implicación para game-designer (no decidido aquí): la regla 5 (apagar y esconderse) y el costo del bloqueador (regla 2) son mecánicas que requieren definición propia.

## 4. Lugares del slice

| Lugar | Contenido | Superficie |
|---|---|---|
| Bodega Quinta de Karrow | Punto de carga del Consorcio; carga y manifiesto | Asphalt |
| Ruta 9 "Cinta" | Tramo de autopista nocturna bajo lluvia | WetAsphalt |
| Puesto de Control Paso Alder | Barrera, luz de inspección, ventanilla; inspección de manifiesto | Asphalt |
| Camino de la Turba | Desvío de los Cuervos tras fallar o evitar el puesto | Gravel, Mud |
| Pinar Negro | Bosque para esconderse y perder a la persecución | Forest |
| Depósito Sin Nombre | Entrega final (Consorcio) | Gravel |

## 5. Glosario

- **Cinta:** Ruta 9, la autopista de la Cuenca.
- **Manifiesto:** documento de carga declarada; lo que la DTG verifica.
- **Firma:** huella detectable de una carga (contrabando).
- **Blindaje:** protección que oculta la firma.
- **Inspección secundaria:** revisión más profunda tras sospecha media.
- **Gavilán:** patrulla de persecución de la DTG.
- **La Mesa:** despachador anónimo del Consorcio.
- **El Corte:** falla de comunicaciones de hace veinte años.
- **Turba:** turbera; territorio de los Cuervos.
- **Camionero de barro:** conductor que conoce los caminos 4x4.
- **Crédito:** moneda de pago (`ValueCredits`).
- **Quedarse a oscuras:** apagar motor y luces para perder a la persecución.
