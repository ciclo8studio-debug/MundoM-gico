# Página de itch.io — Demo (pública el 10-12-2026; testers privados alrededor del 26-11)

> **12-11:** el CTO aplazó la publicación. Esta página se actualiza antes del 10-12 para la v0.5.0, con el Nivel 1 completo y la cámara en 1ª persona.

**Responsable:** `@qa-publisher` · **Estado:** listo para copiar y pegar. El CTO publica.
**Versión a subir:** **v0.3.40** si el CTO la valida el 12-11 con `checklist_v0.3.40.md` y `checklist_humo.md`. Si no, **v0.3.35** como respaldo.
**Calendario (decisión del CTO, 11-11):**
- **13-11:** página en modo **Restringido**, solo para 3–5 testers con clave de descarga.
- **16-11:** página **pública**, con la v0.3.41 si los testers terminan sin cierres.

> Corregido el 11-11. El borrador anterior prometía el Carrusel, la Casa de Alfredo, la huida y 20–30 min. Eso es de la demo completa (v0.4/v0.5), no de esta. También decía "primera persona", pero el juego es en **tercera persona**.

---

## Título
**Mundo Mágico: Turno Nocturno — Demo**

## Lema (1 línea)
*Despiertas en el Pabellón 4. El parque de diversiones abandonado todavía te espera.*

## Descripción corta (120 caracteres o menos)
Terror en tercera persona: un hospital vacío, un parque abandonado y una linterna que se apaga.

## Descripción
> Isidora despierta en una sala del Pabellón 4 sin recordar cómo llegó. Al final del pasillo de Urgencias, algo la arrastra hasta **Mundo Mágico**, el parque de diversiones que cerró hace años y que nadie volvió a abrir.
>
> Explora con tu linterna y cuida las pilas. Devuélvele la luz a la entrada, recorre la avenida y lee lo que quedó escrito. Sobre todo, **no hagas ruido**.

**En esta demo (Capítulo 1):**
- 🏥 La sala del despertar y el pasillo de Urgencias.
- ⚡ La entrada del parque: devuélvele la energía y descubre quién espera bajo el letrero.
- 🎡 La avenida principal en penumbra, el teatro y la noria, y algo que te observa desde lejos.
- ⏱️ Unos **15–25 minutos** de juego. *(El CTO confirma la duración real en el playtest del 12-11.)*

**Características:**
- Linterna con batería y pilas que se cambian a mano.
- Botella para distraer a lo que te busca.
- Sonido que viene de su lugar y un latido que te avisa cuando algo se acerca. Juega con audífonos.
- Guardado automático al entrar a cada zona.
- Teclado y mouse, o mando completo con vibración (PlayStation o Xbox).
- Textos en español.

## Requisitos (mínimos probados)
| | Mínimo | Recomendado |
|---|---|---|
| SO | Windows 10/11 64-bit | Windows 11 64-bit |
| CPU | Intel Core i7-1065G7 | 6 núcleos o más |
| RAM | 8 GB | 16 GB |
| GPU | Intel Iris Plus (calidad **Baja**) | NVIDIA GTX 1660 / RTX 2060 o superior |
| DirectX | 11 | 11/12 |
| Disco | 7 GB libres (ZIP de 3,3 GB + juego descomprimido) | SSD |

> Con tarjeta integrada, juega en **Calidad Baja**, que viene así por defecto en el primer arranque. El juego va a 30 fps.

## Aviso de Windows (pegar al final de la descripción)
> **Windows puede mostrar "Windows protegió su PC".** El juego no tiene firma digital porque somos un estudio independiente. Para jugar, haz clic en **Más información** y luego en **Ejecutar de todas formas**.

## Etiquetas
`horror`, `survival-horror`, `third-person`, `atmospheric`, `psychological-horror`, `exploration`, `singleplayer`, `spanish`, `unreal-engine`, `demo`

## Capturas (5) — tómalas mientras juegas la versión que se sube
Usa **Win + Alt + Impr Pant** (Barra de juegos de Windows). Se guardan en `Videos\Capturas`.
1. La sala del despertar, con la linterna encendida.
2. El pasillo de Urgencias justo cuando aparece la criatura.
3. La entrada del parque con el letrero MUNDO MÁGICO y las luces encendidas.
4. La silueta del sombrero bajo el letrero.
5. La avenida en penumbra, con niebla y faroles.

Como portada de la página sirve `Fuentes/UI/T_PortadaCine.png`, con el arte de la novela (autorizado por IMAGINARIO). Como video, el teaser nocturno `Teaser/Teaser_MundoMagico.mp4`, subido a **YouTube como no listado**, porque itch.io pide un enlace y no acepta MP4. El de atardecer queda fuera mientras el cielo salga negro.

## Antes de publicar (checklist)
- [ ] 12-11: el CTO juega la versión candidata de punta a punta, sin cierres.
- [ ] ZIP generado con `herramientas/Empaquetar_Demo.ps1 -Version 0.3.40` (ya está en `Publicar\`).
- [ ] 5 capturas y la portada.
- [ ] 13-11: página en modo **Restringido**, con claves para los testers y el formulario `auditoria/formulario_testers.md`.
- [ ] 16-11: si no hay cierres, cambiar a **Público**.
