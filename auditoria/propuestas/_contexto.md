# Contexto para la reunión de propuestas (2026-11-09)

**Juego:** "Mundo Mágico: Turno Nocturno". Survival horror en 3.ª persona hecho en UE 5.8 por un estudio unipersonal chileno.
**Protagonista:** Isidora.
**Historia y ambientación:**
- 1998. Isidora despierta en el Pabellón 4 de un hospital.
- Un glitch la lleva a un parque de diversiones abandonado, "Mundo Mágico", tras un incendio.
- Personajes: Alfredo, Rafaela y Luisa.
- Hay 62 páginas de notas y 4 cintas VHS.
**Recorrido actual de la demo corta v0.3.1 (validada, 32 min sin cierres):**
1. Sala de despertar con la linterna en el mostrador.
2. Pasillo de Urgencias con susto de criatura (sonido 3D).
3. Glitch a la entrada del parque.
4. Sombra bajo el letrero.
5. 4 interruptores (en cualquier orden).
6. Luces y cinemática en órbita.
7. Avenida en penumbra.
8. Fin frente al castillo.
**Demo completa (implementada, sin validar):** Carrusel (gira solo + muñeca), bodega (llave), Casa de Alfredo (fonógrafo "LAS OCHO", reloj 8-4-2, caja fuerte con fusible), apagón, payaso y huida a la entrada.
**Mecánicas existentes:**
- Linterna con batería y pilas manuales.
- Botella para distraer (MakeNoise).
- Agacharse, correr con aliento y latido.
- Mochila, botiquín, notas y galería.
- Candado con código.
- Generador con ruido.
- Guardado y continuar.
- Mando PS4 completo (✕ salta, □ interactúa).
- Escondites `BP_Hiding_Door` (rotos: el jugador se atasca; fuera de la demo).
**Enemigos:**
- Payaso (clown_monster), oso Willy, león y muñeca: FSM sobre Tick, sin Behavior Tree.
- Criatura del pasillo (SK_Whisper).
**Mapa:**
- Hospital (sala más pasillo con habitaciones laterales).
- Parque orgánico con cerros, unos 1.100 árboles y unos 15 atracciones.
- "Parque vivo": niebla, luces que parpadean, ruidos metálicos y lluvia.
**Técnico:**
- Todo está en un BP de jugador monolítico de 10 MB.
- PC de desarrollo con iGPU Intel Iris Plus y 8 GB (no corre PIE; se prueba con builds Shipping, unos 15 min).
- Calidad Baja por defecto.
- Decisión de hardware de un partner el 17-11.
**Plan:** demo corta 13-11 (itch.io) y demo completa v0.5.0 el 10-12-2026, con sprints de 1 semana.
**Packs disponibles sin usar o archivados:** UndergroundTunnel, SciFi_Beast06, RoninGirl (sigilo), DynamicFalling (voltereta), RadialWheel (plugin), FreeSurvivalIconsKG, Horror_Hospital (18 GB, se usan unos 40 assets), SoulCity, DetectiveOffice.
**Documentos del estudio:** BOARD.md, AUDIT_REPORT.md, PLAN_DE_TRABAJO.md, CHANGELOG.md y auditoria/*.md (auditoría previa, diseño del BT, diseño Carrusel/Casa).
