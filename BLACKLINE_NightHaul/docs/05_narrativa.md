# 05 — Narrativa: BLACKLINE: NIGHT HAUL

> **SUPERADO el 2026-11-22 por `24_historia_secuestro.md`.** Se conserva como historial; no usar para nuevos textos.


Todo ficticio: empresas, lugares, personas, cargas y regímenes inventados. Sin referencias a entidades reales.

## Premisa
En la Costa Ceniza, un corredor costero bajo "Ley de Tránsito Sellado", toda carga debe viajar con manifiesto aprobado por la Autoridad de Corredores (AdC). Lo que el sistema no puede mover legalmente, igual se mueve: de noche, por la Línea Negra, los caminos sin registro. Eres un transportista nocturno: una furgoneta, un manifiesto falsificado, y la pregunta de qué llevas realmente atrás.

Pilares narrativos: (1) la carga es la historia, no el cuadro de texto; (2) el jugador deduce, no se le explica; (3) cada control de ruta es una escena de tensión; (4) el cierre siempre cuesta algo.

## Protagonista: Mara Quiroga
- Ex conductora de flota de Transportes Halcón del Sur, quebrada y absorbida por Vantor Logística. Despedida con deuda de la furgoneta.
- Motivación: saldar la deuda (créditos = `ValueCredits`) y recuperar su licencia de corredor.
- Voz: seca, práctica, humor negro. Habla por radio con el despachador; nunca monologa.
- Arco del slice: de "no pregunto qué llevo" a "necesito saber qué llevo".
- Función de juego: su ética es el contrapeso a la ganancia; el jugador elige cuánto inspeccionar de su propia carga.

## Empleador: Vantor Logística / "Despacho Faro"
- Intermediario con fachada legal (manifiestos `CompanyName`), que subcontrata en negro. Contacto: voz de radio "Faro" (anónima, cordial, evasiva).
- Paga bien, no da explicaciones, deniega conocimiento si te atrapan.
- Giro de fondo: Vantor sí vende a la AdC la información de sus propios conductores cuando conviene. La lealtad es unilateral.
- Manifiestos: `ForgeryQuality01` varía por contrato; Faro siempre dice "es de primera" (casi nunca lo es).

## Estructura del slice: 3 contratos
Cada contrato = carga + ruta + control(es) + decisión moral. Dificultad creciente en persecución y terreno.

### Contrato 1 — "Ruta de Prueba" (Estándar/Volátil)
- Carga: Núcleos Energéticos Inestables, declarados como "baterías industriales".
- Ruta: asfalto húmedo y grava; un control. Tutorial de manejo, carga y manifiesto.
- Tono: rutina nerviosa. Faro tranquiliza.
- Cierre: entrega limpia, pago bueno. Mara nota el zumbido de las cajas.

### Contrato 2 — "Silencio de Servidores" (IA / Frágil)
- Carga: Servidores de IA Desconectados, declarados como "equipo de oficina usado".
- Ruta: barro y bosque (4x4), inhibidor de señal clave, dos controles; persecución nivel 1-2.
- Tono: paranoia. Si el inhibidor falla, los servidores emiten (`UnshieldedSignature01`).
- Cierre: el destinatario no recibe la carga con alivio sino con miedo.

### Contrato 3 — "Hielo y Herencia" (Criogénico + Reliquias)
- Carga mixta: Contenedores Criogénicos y Reliquias, declarados como "material de laboratorio y museografía".
- Ruta: la más larga, cadena de controles, persecución nivel 3, gestión de frío y fragilidad.
- Tono: clímax y dilema. Mara decide si abre un contenedor.
- Final del slice: entrega, o desvío (gancho a futura campaña). Faro corta la radio.

## Cómo cada carga cuenta algo

| Carga (`ECargoClass`) | Lo que dice el mundo | Cómo se siente al jugar | Pista narrativa |
|---|---|---|---|
| Núcleos Energéticos Inestables (`Volatile`) | La Costa Ceniza se queda sin energía legal; hay mercado negro de combustible. | Impactos y vuelco son letales; la carga vibra y avisa. | Etiquetas de una planta cerrada tras un "accidente". |
| Servidores de IA Desconectados (`AIServer`) | Alguien apagó a la fuerza máquinas que pensaban; las quieren de vuelta. | Alta `ContrabandSignature01`: exige inhibidor y blindaje. | Un LED que parpadea con la furgoneta apagada. |
| Reliquias (`Relic`) | El corredor vende su pasado para sobrevivir; saqueo con valor cultural. | Frágiles y valiosas: conducir suave paga más que conducir rápido. | Etiquetas de procedencia borradas con tinta. |
| Contenedores Criogénicos (`Cryogenic`) | Alguien conserva algo (o a alguien) que la ley no reconoce. | Sensibles a golpes y tiempo; la inspección secundaria es riesgosa. | Peso y sonido interior inconsistentes con el manifiesto. |

Regla: la discrepancia entre `Manifest` declarado y carga real es la fuente de toda revelación; el jugador la lee por masa, firma y pistas ambientales.

## Ludonarrativa
- Conducir suave = respeto por la carga (integridad, valor).
- Controles de ruta = interrogatorio; la ventanilla y el manifiesto son el diálogo.
- Persecución = consecuencia de la mentira; esconderse (motor y luces apagados) es la tensión de "Lost".
- Riesgo a vigilar: si el pago recompensa ignorar el contenido, la historia de dudas de Mara choca con el sistema; el pago debe premiar también la integridad de la carga.

## Entrega narrativa
Radio de Faro (briefing/ cierre), notas del manifiesto (`Notes`), etiquetas en cajas y objetos del tablero. Sin cinemáticas ni cutscenes en el slice.

## Pendientes
- Aprobación del CTO del nombre de Mara, Vantor y Faro.
- Guion de radio por contrato (delegar a `writer`).
- Verificar con game-designer la curva de `ValueCredits` frente a la deuda.

## Contradicciones
Sin contradicciones con `CONTRATO.md` (tipos `ECargoClass`, `FManifestLine`, `UManifestDataAsset`, estados de checkpoint y persecución). Proyecto independiente de Mundo Mágico.
