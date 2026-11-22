# 02 — Loop de diseño: BLACKLINE: Night Haul

Fantasía: transportista nocturno de carga ilegal. Tensión sostenida, conducción pesada, sangre fría en el retén. (MDA: Desafío, Sensación, Sumisión/inmersión.)

## 1. Core loop

```
CARGAR -> CONDUCIR -> RETÉN -> (limpio) -> CONDUCIR -> ENTREGA -> pago -> CARGAR
                         \-> (alerta) -> PERSECUCIÓN -> (huir) -> CONDUCIR
                                                      -> (capturado/destruido) -> fallo
```

- **Micro (30 s):** controlar la furgoneta con carga que se desplaza (masa, centro de masa, G lateral).
- **Meso (5-10 min):** un viaje = 1 contrato: cargar, ruta, 1-2 retenes, entrega.
- **Macro:** créditos -> mejores contratos/carga más valiosa y más riesgosa.

## 2. Estados del jugador

| Estado | Entra | Sale | Sistema |
|---|---|---|---|
| Cargando | Inicio de contrato | Cierra puertas / Ignition | `AddCargo`, `FManifestLine` |
| Conduciendo | Motor encendido | Zona de retén / persecución / destino | `ABlacklineVanPawn`, `ATerrainModifierVolume` |
| Atascado | Superficie sin tracción (Mud, sin 4WD) | Diff lock / 4WD / salir | `ESurfaceKind`, `bRequires4WD` |
| Acercándose | Entra al área previa al retén (`Approaching`) | Se detiene en `InspectionZone` | `ACheckpointController` |
| Inspección | Detenido, ventana abierta (`Inspecting`) | Veredicto | `EInspectionVerdict` |
| Persecución | `PursuitAlert` o huida | `Lost` (se pierde al perseguidor) / captura | `UPursuitDirectorSubsystem` |
| Entrega | Llega a destino con carga | Pago | `ValueCredits` x integridad |

Persecución se subdivide en `Hunting` (te ven) -> `Searching` (te perdieron, buscan) -> `Lost`.

## 3. Decisiones de riesgo / recompensa

1. **Qué cargar:** valor alto = firma alta (`ContrabandSignature01`), frágil o volátil. Cargar `bRequiresShielding` sin blindaje sube la firma visible.
2. **Manifiesto vs. realidad:** diferencia entre masa/firma declarada y real = sospecha. Mejor `ForgeryQuality01` = más margen.
3. **Velocidad al retén:** llegar rápido y frenar en seco sube sospecha y daña carga; llegar lento cuesta tiempo.
4. **Tablero en el retén:** apagar faros, activar inhibidor (`SignalJammer`) reduce detección pero un inhibidor encendido es en sí sospechoso. Ventana abierta/cerrada.
5. **Ruta:** asfalto = rápido y con retenes; barro/grava/bosque = sin retenes, riesgo de atasco y daño. Pide 4WD y diff lock.
6. **Pasar o huir:** si la inspección va mal (`SecondaryInspection`), aceptarla o forzar la huida. Huir paga solo si hay terreno y cobertura.
7. **Esconderse en persecución:** motor y luces apagados (`ReportEngineAndLights`) para pasar a `Searching`, a costa de quedarse parado.

## 4. Economía mínima

Una sola moneda: **créditos**.

- **Faucet:** pago de entrega = `sum(ValueCredits x Quantity) x CargoIntegrity01` (+ bono por no ser inspeccionado: x1.2).
- **Sinks:** reparación de la furgoneta (proporcional al `DamageRatio01`), multa si hay `SecondaryInspection` (porcentaje del pago), pérdida de la carga y del pago si hay captura.
- **Progresión:** los créditos desbloquean contratos de mayor `ValueCredits` y blindaje/4WD. Sin cosméticos en el MVP.
- **Regla de equilibrio:** el contrato de mayor riesgo debe pagar ~2-3x el de riesgo bajo; si el pago esperado del riesgo alto no supera al bajo, nadie lo elige. Todo en `DataAsset`, nada fijo en código.

## 5. Qué ve / oye el jugador

| Estado | Ve | Oye |
|---|---|---|
| Cargando | Bodega, cajas, manifiesto; carga visible en la cabina | Cajas, metal, ambiente de depósito |
| Conduciendo | Cabina oscura, faros, limpiaparabrisas, tablero con interruptores; cajas que se mueven por la ventanilla trasera | Motor, lluvia, neumáticos según superficie, crujidos de carga en curva |
| Atascado | Ruedas patinando, barro, vibración de cámara | Motor al tope, barro, silencio de ambiente |
| Acercándose | Luz del retén (`InspectionLight`), barrera, fila | Radio/baliza lejana, el bajo del motor sube de tono |
| Inspección | Linterna del guardia en la cara, ventana abierta; manifiesto en la mano | Diálogo breve, respiración, latidos; el resto se silencia |
| Persecución | Luces rojas/azules en el retrovisor, polvo, HUD con indicador de calor (`GetHeat01`) | Sirenas, motor forzado, golpes de carga; música rítmica |
| Buscando | Haces de linterna que barren, oscuridad total | Sirenas lejanas, motor apagado, goteo; tensión silenciosa |
| Entrega | Destino, cajas descargadas, recibo con desglose | Alivio: motor se apaga, pago (tono seco) |
| Fallo | Pantalla de captura / furgoneta destruida | Corte brusco, silencio |

## 6. Casos límite

- Retén evitado por ruta off-road: no hay bono "limpio" automático; solo se aplica si hubo inspección sin hallazgos o no la hubo (decisión de diseño abierta).
- Persecución infinita en círculos: `Heat01` sube con el tiempo visible; la policía refuerza el nivel (`Level`).
- Carga con integridad 0: contrato fallido pero sin captura.
- Se huye del retén sin detenerse: es `PursuitAlert` directo.

## 7. Dudas abiertas

1. ¿Muerte por captura reinicia el contrato o cobra una multa persistente?
2. ¿Los contratos son fijos o generados a partir de `UManifestDataAsset`?
3. ¿El bono "sin inspección" premia el off-road?
