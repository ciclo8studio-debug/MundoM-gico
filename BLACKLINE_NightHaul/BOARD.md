# BLACKLINE: NIGHT HAUL — Tablero de avance

Actualizado: 2026-11-22 · Versión: 0.1.0-dev · Motor: UE 5.8.4 · Meta: Vertical Slice jugable a 60 FPS (8–16 GB, GPU media)
Leyenda: ✅ hecho · 🔄 en curso · ⬜ pendiente · ⚠️ riesgo/bloqueo

## Estado actual (25-11-2026, build v0.1.20)
| Área | Estado | Nota |
|---|---|---|
| Módulo C++, 25 contratos, 3 tests de Automation | ✅ | Compila (juego y editor) |
| Chile comprimido 1:100: ruta, terreno por bioma, 4 zonas | ✅ | Zona 2 probada; zonas 1, 3 y 4 sin probar |
| Furgón: física, 165 km/h, mando PS4, combustible, volcado | ✅ | Carrocería de plantilla (placeholder) |
| Historia (Chuño y La Cofradía): briefings, bienestar, plazo, pruebas de vida, finales | ✅ | Como mensajes en pantalla |
| Bencinera, restaurante, tráfico en contra, minimapa, panel de encargo | ✅ | Probados en pantalla |
| Retén con manifiesto automático, veredictos, patrullas | 🔄 | En build v0.1.20; sin probar en pantalla |
| Teclas de motor, luces, inhibidor y volver a la carretera | 🔄 | En build v0.1.20 |
| Arresto, decisión de avisar a la policía, finales alternativos | ⬜ | |
| Cabina propia con botones, guardia y barrera con modelo | ⬜ | Arte (nube) |
| Audio (motor, lluvia, sirenas, prueba de vida) y clima visible | ⬜ | Sin ningún sonido todavía |
| Menú de contratos y cambio de zona | ⬜ | Hoy se elige con `-zone=N` / `-contract=Cxx` |
| Release (Shipping, remapeo, localización) | ⬜ | |

## Plan hasta el final
Cada fase termina con una build compilada y probada. No se avanza de fase con criterios sin cumplir. Una sola sesión de editor a la vez.

### Fase 0 — Cerrar la base (1 sesión)
- [x] Terminar build de editor.
- [x] Importar `DT_BlacklineContracts` (`blackline_contracts.import_datatable()`).
- [ ] Revisar y corregir los `VERIFY-5.8` que aparezcan en log (ruedas, diferencial, cvars).
- **Listo cuando:** el proyecto abre, el DataTable tiene 25 filas y `StartContract("C01")` carga la carga sin errores en log.

### Fase 1 — Manejo (prototipo del día 1, `docs/21`)
- [ ] Empaquetar build de prueba (sin PIE, por tu regla del PC).
- [ ] Ajustar torque, suspensión, masa y fricción hasta que el furgón sea agradable.
- [ ] Probar carga (CoM, G lateral, integridad), cámara 1ª/3ª, botones del tablero, 2WD/4WD en barro.
- **Listo cuando:** pasan las pruebas 1–5 de `docs/21` y no vuelca sin intención a 60 km/h.
- ⚠️ Si no es divertido tras 2 h de ajuste, pivotar según `docs/21`.

### Fase 2 — Retén y persecución
- [ ] BP del guardia y de las patrullas (`BP_Guard`, `BP_PatrolCar`).
- [ ] Probar los 3 veredictos (paso limpio, secundaria, alerta) con los niveles de documento.
- [ ] Persecución: patrullas, conos de búsqueda, enfriado con motor y luces apagados.
- **Listo cuando:** `docs/11` casos de retén y persecución pasan.

### Fase 3 — Campaña (contratos)
- [ ] Pantalla de tablet con lista por broker (`WBP_Contracts`), selección y manifiesto (`WBP_Manifest`).
- [ ] Clima y terreno por contrato (lluvia, niebla, barro, hielo) conectados a `WeatherType`/`TerrainType`.
- [ ] Alcance del slice (decidido 22-11): **5 contratos jugables, uno por arco**; los otros 20 quedan en datos.
  - C01 Operación Pudahuel (Arco 1): asfalto nocturno, tutorial del inhibidor GPS.
  - C09 Prototipo Cero (Arco 2): carga asimétrica en curvas.
  - C12 Reliquias de Madera (Arco 3): barro profundo con 4x4.
  - C18 Bajando la Cuesta (Arco 4): bajada bajo lluvia fuerte.
  - C21 Paso Fantasma (Arco 5): aduana con escáner y documento nivel 3.
- **Listo cuando:** se pueden aceptar, jugar y cerrar los 5 contratos de punta a punta con pago.

### Fase 4 — Arte y audio
- [ ] Furgón propio con cabina (nube/DCC), botones `Dash.*`, cargas por contrato.
- [ ] Escena nocturna lluviosa con Lumen + niebla volumétrica calibrada (`docs/D1`).
- [ ] MetaSounds de motor, lluvia y ventanilla; SFX de `docs/20`.
- **Listo cuando:** `stat unit` mantiene ≥58 FPS medios en el escenario más pesado.

### Fase 5 — Pulido, QA y accesibilidad
- [ ] Optimización según `docs/09` (presupuesto de frame, memoria, VRAM).
- [ ] Remapeo de controles, subtítulos, reducción de mareo en cabina (`docs/19`).
- [ ] Ejecutar los 44 casos de `docs/11`; arreglar los bloqueantes.
- **Listo cuando:** sin errores críticos ni mayores abiertos.

### Fase 6 — Release del slice
- [ ] Build Shipping, versión `0.1.0`, notas de versión.
- [ ] Checklist de `docs/17`; página de publicación (itch.io u otra, decide el CTO).
- **Listo cuando:** build limpia instalada y jugada de inicio a fin en un PC distinto.

## Decisiones pendientes del CTO
0. **Historia nueva (22-11):** secuestro de Chuño; nombres y 4 preguntas abiertas en `docs/24_historia_secuestro.md`.
1. ~~Alcance del slice~~ → decidido: 5 contratos, uno por arco (C01, C09, C12, C18, C21).
2. ¿Una tabla de strings o tres? (`06_textos.md` vs `18_localizacion.md`).
3. Nombres propios de narrativa y mundo (Veldmark, Mara Quiroga, Vantor, Faro): aprobar o cambiar.
4. Dónde publicar y fecha objetivo.
5. Si se compra o contrata arte 3D, o se genera en la nube.

## Riesgos abiertos
| Riesgo | Mitigación |
|---|---|
| Chaos Vehicles es experimental; APIs de 5.8 cambian | Marcar y verificar cada `VERIFY-5.8` en la Fase 0 |
| PC de 8 GB con iGPU, builds lentas (~34 min el editor completo) | Compilar incremental, una sesión de editor a la vez |
| Render nocturno (Lumen/VSM) pesado para la GPU | Aplicar el snippet de a un cambio y medir |
| Cifras de balance estimadas | Calibrar con playtests en Fase 1–2 |
| Nombres de empresas ficticias parecidos a reales | Revisión legal antes de publicar |
