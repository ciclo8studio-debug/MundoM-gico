# 08 — Dirección de Arte (Blackline: Night Haul)

Fuente de verdad visual. Pilar: tensión nocturna legible; el furgón es el único refugio cálido.

## 1. Paleta neo-noir nocturna
| Rol | Color | Hex | Uso |
|---|---|---|---|
| Noche base | Azul tinta | #0B1020 | Cielo, sombras, ambiente |
| Asfalto | Gris azulado | #1A2030 | Caminos (mojado: más brillo, mismo tono) |
| Sodio | Ámbar | #FFA640 | Farolas, interior de cabina |
| Neón puerto | Cian / Magenta | #20E0D0 / #E02080 | Carteles, muelles, accentos |
| Peligro | Rojo | #E02020 | Control policial, alertas, daño |
| Faro furgón | Blanco frío | #E8F0FF | Haz propio |

Reglas: máx. 2 acentos saturados por escena; negro puro prohibido (piso #05070D); el rojo se reserva solo a amenaza. Verificar daltonismo: amenaza también por forma/parpadeo, no solo color.

## 2. Lenguaje de luz
- **Faros (jugador):** 2 SpotLight, cono interno 18°/externo 38°, 5500 K, alcance ~60 m. Son la herramienta principal de lectura; apagarlos reduce firma visible (ver `bHeadlightsOn`).
- **Sodio:** luces puntuales ámbar 2200 K, espaciadas 25-35 m, forman ritmo y guían la ruta. Sin sombras dinámicas salvo las 4 más cercanas.
- **Neón de puerto:** emisivo + falso reflejo en asfalto mojado (SSR/planar fake o decals); sin luces dinámicas por cartel.
- **Control policial:** foco blanco duro (`InspectionLight`) + giratorias rojo/azul; contraste alto marca zona de decisión.
- Niebla volumétrica baja, densidad ligera; priorizar luces estáticas/bakeadas y Lumen solo si el presupuesto lo permite (iGPU: perfil bajo con luces estáticas).
- Estado por paleta: persecución desplaza el ambiente a rojo/azul frío; escondido = oscuridad y ámbar tenue.

## 3. Furgón y cabina
**Exterior:** furgón de carga mediano, silueta cuadrada, ciega y utilitaria; pintura mate gris oscuro, óxido y barro por zonas, sin logos legibles (manifiesto falso). Faros circulares grandes como rasgo reconocible; luces traseras rojo tenue. Ruedas 4x4 con perfil alto.

**Cabina (cámara principal):** volante grande, tablero analógico desgastado, iluminación interior mínima ámbar.
- 6 controles diegéticos con tags `Dash.*`: faros, limpiaparabrisas, ventana, inhibidor de señal, bloqueo de diferencial, encendido.
- Cada control: forma distinta (palanca, perilla, interruptor, botón) + LED de estado propio; legible a oscuras por emisivo.
- Parabrisas con gotas/suciedad (material, no geometría); limpiaparabrisas visible.
- Manifiesto y objetos pequeños colgados/sobre tablero dan vida sin costo.
- Vista tercera persona: misma silueta, carga atada visible en la caja.

## 4. Presupuesto por asset (LOD0, hero = jugador)
| Asset | Tris | Texturas (PBR, 1 set) | Mats |
|---|---|---|---|
| Furgón exterior | 60k | 2K ×1 atlas | 3 |
| Cabina interior | 40k | 2K ×1 | 2 |
| Rueda (×4, instancia) | 6k | 1K compartida | 1 |
| Control policial (barrera+caseta) | 15k | 2K | 2 |
| Vehículo PNJ/policía | 35k | 2K | 2 |
| Prop de carga (caja/contenedor) | 1.5k | 512-1K, trim | 1 |
| Farola / cartel neón | 2k / 3k | trim compartido 1K | 1 |
| Edificio modular de puerto | 8k | trim 2K compartido | 2 |
| Vegetación (árbol) | 5k + impostor | 1K atlas | 1 |

Texturas: BC1/BC7, ORM empaquetado (R=AO, G=Rough, B=Metal), emisivo aparte solo si hace falta. Total de escena objetivo: ≤3M tris visibles, ≤150 draw calls de props, memoria de texturas ≤1.5 GB. LODs: 3 niveles (100/50/20 %), distancias por pantalla. Furgón: LOD1 no se usa en cabina.
Nombres: `[categoría]_[nombre]_[variante]_[tamaño].ext` (p. ej. `env_farola_sodio_large`, `veh_van_body_01`).

## 5. Reglas Nanite
- **Solo mallas estáticas opacas** (edificios, rocas, props de carga fijos, terreno decorativo).
- **No Nanite:** furgón y ruedas (Chaos Vehicle, mallas esqueléticas), skeletal meshes, mallas con World Position Offset/vegetación animada, translúcidos/vidrio, decals, luces con máscaras especiales.
- Cargas dinámicas (`UCargoItemDataAsset::Mesh`) de bajo poly sin Nanite; si se simulan con física, colisión simple (cajas/convex).
- Colisión nunca desde malla Nanite; proxies manuales.
- Verificar soporte en 5.8 (`// VERIFY-5.8`) y comportamiento en iGPU con `r.Nanite` perfilado antes de generalizar.

## 6. UI diegética
- Sin HUD plano: la información vive en el vehículo.
  - Velocidad: velocímetro analógico (aguja/emisivo) en tablero.
  - Integridad de carga: luz de aviso en tablero + sonido físico visible (golpe del contenedor); sin barras.
  - Sospecha/persecución: luz rojo-azul en espejo retrovisor y tinte ambiente, no números.
  - Combustible/motor: testigos en cluster.
- Interacción: puntero/mira de punto pequeño que resalta el control (trace `Interact`, `ECC_GameTraceChannel1`); resalte por brillo emisivo, no por contorno.
- UMG solo para menús, manifiesto en pantalla (papel físico en 3D preferido) y pausa; tipografía industrial sans, alto contraste, mín. 24 px a 1080p.
- Manifiesto/controles: diegético primero, con opción de accesibilidad que muestre etiquetas de texto.
- Feedback de estado en cada control (`FOnDashControlChanged`) por LED, no por texto.
