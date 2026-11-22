# BLACKLINE: NIGHT HAUL — Tablero de avance

Actualizado: 2026-11-22 · Versión: 0.1.0-dev · Motor: UE 5.8.4 · Meta: Vertical Slice jugable a 60 FPS (8–16 GB, GPU media)
Leyenda: ✅ hecho · 🔄 en curso · ⬜ pendiente · ⚠️ riesgo/bloqueo

## Estado actual
| Área | Estado | Nota |
|---|---|---|
| Diseño y documentación (docs 00–23) | ✅ | 31 agentes; cifras estimadas, sin medir |
| Módulo C++ `Blackline` (carga, retén, persecución, terreno, ruedas, furgón, HUD) | ✅ | Compila en target de juego |
| 25 contratos (CSV + subsistema) | ✅ | DataTable aún sin importar |
| Mapa plano `L_FlatRoad_Chile` + `BP_BlacklineVan` + inputs + GameMode | ✅ | Generados por script; no vistos en pantalla |
| Target de editor con el C++ nuevo | 🔄 | Reconstruyendo |
| Importar `DT_BlacklineContracts` en el editor | ⬜ | Tras el build de editor |
| Manejo del furgón probado | ⬜ | Carrocería placeholder (OffroadCar) |
| Render nocturno y rendimiento | ⬜ | Snippet sin aplicar; iGPU limita |
| Audio (MetaSounds) | ⬜ | Sin samples |
| Assets propios (furgón, cabina, cargas) | ⬜ | Tareas para nube |

## Plan hasta el final
Cada fase termina con una build compilada y probada. No se avanza de fase con criterios sin cumplir. Una sola sesión de editor a la vez.

### Fase 0 — Cerrar la base (1 sesión)
- [ ] Terminar build de editor; abrir proyecto sin errores.
- [ ] Importar `DT_BlacklineContracts` (`blackline_contracts.import_datatable()`).
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
- [ ] Alcance del slice: **3 contratos jugables** (uno por escenario: asfalto, barro, retén doble); los otros 22 quedan en datos.
- **Listo cuando:** se puede aceptar, jugar y cerrar 3 contratos de punta a punta con pago.

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
1. Alcance final del slice: ¿3 contratos o más? (propuesto: 3).
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
