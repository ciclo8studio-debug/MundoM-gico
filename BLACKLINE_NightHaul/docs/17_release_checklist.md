# 17 - Checklist de release: vertical slice jugable (BLACKLINE: NIGHT HAUL)

Motor: UE 5.8.4, C++, modulo `Blackline`. Plataforma: PC (Windows). Estado: Production, rigor minimal.
Fecha: 2026-11-21. Version objetivo: **0.1.0** (propuesta, el CTO confirma).

## Estado de evaluacion
- Codebase (TODO/FIXME/HACK): NOT ASSESSED - no se escaneo `Source/Blackline/` en esta pasada.
- Bugs (`production/qa/bugs/`): NOT ASSESSED - sin registros de bugs.
- Resultados de tests/CI: NOT ASSESSED - no hay logs.
- Certificacion: NOT ASSESSED - `platform.cert_tier` sin definir (none | itch | steam | console). Pregunta abierta al CTO. No se emite ninguna pista de cert.

## 1. Build
- [ ] Build limpia Development y Shipping Win64, sin errores de compilacion
- [ ] Modulo `Blackline` y dependencias Build.cs (Core, CoreUObject, Engine, InputCore, EnhancedInput, ChaosVehicles, PhysicsCore, Niagara, AIModule, GameplayTags, UMG) enlazan
- [ ] Todos los `// VERIFY-5.8` resueltos o documentados
- [ ] Sin warnings nuevos
- [ ] Canal de traza `ECC_GameTraceChannel1` ("Interact") y tags `Dash.*` presentes en la build
- [ ] Reproducible desde el commit etiquetado
- [ ] Build corre en maquina limpia (sin editor ni VS)

## 2. Packaging
- [ ] Cook completo: DataAssets (CargoItem, Manifest), mapas, Niagara, UMG incluidos
- [ ] Input Mapping Context (`DefaultIMC`) e IA_* cargan en build empaquetada
- [ ] Mapa de arranque correcto (ruta completa de mapa)
- [ ] Tamano de build dentro del presupuesto: [definir]
- [ ] Empaquetado como .zip con nombre `Blackline_v<version>_Win64`
- [ ] Probada solo en build empaquetada (sin PIE, regla de hardware del estudio)

## 3. Vertical slice jugable (criterios minimos)
- [ ] Furgoneta conduce: acelerar, frenar, freno de mano, direccion, RWD/4x4
- [ ] Camara cockpit y tercera persona alternan
- [ ] Botones del tablero responden (Headlights, Wipers, Window, SignalJammer, DiffLock, Ignition)
- [ ] Carga: masa, centro de masa, integridad e impactos funcionan
- [ ] Terreno: superficies, friccion y atasco en 4WD requerido
- [ ] Checkpoint: manifiesto, inspeccion, veredicto (CleanPass / SecondaryInspection / PursuitAlert)
- [ ] Persecucion: Hunting, Searching, Lost, StopPursuit
- [ ] Bucle completo inicio a fin sin crash
- [ ] Sin Tick en actores (excepto carga a 20 Hz)

## 4. Rendimiento y requisitos minimos de hardware
Sin mediciones aun; los valores son pendientes de medir.
- [ ] Hardware minimo medido y documentado: CPU [?], RAM [?], GPU [?], VRAM [?], disco [?], SO Windows 10/11 64-bit, DirectX 11 (validar DX12)
- [ ] Hardware recomendado documentado: [?]
- [ ] FPS objetivo en minimo: [definir, ej. 30 FPS a 1080p calidad Baja]
- [ ] Prueba en el PC de 8 GB / iGPU del estudio como piso real
- [ ] Memoria dentro de presupuesto, sin fugas en sesion de 30+ min
- [ ] Tiempo de carga dentro de presupuesto: [definir]
- [ ] Fisica Chaos estable (sin explosiones de vehiculo) a framerate bajo
- [ ] Escalabilidad: niveles de calidad guardan y cargan
- [ ] Modos ventana / borderless / pantalla completa; teclado+mouse y mando (Xbox)

## 5. Versionado
- [ ] SemVer `MAJOR.MINOR.PATCH`; build interna `MAJOR.MINOR.PATCH.BUILD`
- [ ] Version fijada en `Config/DefaultGame.ini` (`ProjectVersion`) = 0.1.0
- [ ] Tag git `v0.1.0` aplicado al commit de la build (un tag nunca se mueve ni se reutiliza)
- [ ] Version visible en el menu o en el log de arranque (`LogBlackline`)
- [ ] Correcciones posteriores salen como 0.1.1 (hotfix desde el tag, luego merge a `main`)
- [ ] `git push` lo hace el CTO; se le entregan los comandos

## 6. Notas de version
- [ ] Entrada en CHANGELOG con la version (`/write-changelog 0.1.0`)
- [ ] Notas para jugadores (`/patch-notes 0.1.0`), en espanol, sin jerga interna
- [ ] Incluyen: contenido del slice, controles, requisitos minimos, problemas conocidos
- [ ] Problemas conocidos honestos (fisica, IA, rendimiento)
- [ ] Canal de reporte de bugs indicado
- [ ] Revision de tono por community-manager

## 7. Legal y distribucion basica
- [ ] Licencias de terceros / creditos del motor y assets
- [ ] Destino de distribucion decidido (depende de `cert_tier`): [pendiente CTO]

## Go / No-Go: NOT ASSESSED
Faltan: escaneo de codigo, registros de bugs, resultados de test, `cert_tier`, mediciones de hardware.
No hay bloqueos conocidos (no se evaluaron).

Firmas requeridas: [ ] QA Lead  [ ] Technical Director  [ ] Producer  [ ] Creative Director
