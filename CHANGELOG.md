# Changelog — Mundo Mágico: Turno Nocturno

Formato basado en [Keep a Changelog](https://keepachangelog.com/es-ES/1.1.0/). Versionado `MAYOR.MENOR.PARCHE`.
La versión del juego vive en `Config/DefaultGame.ini` → `ProjectVersion`.

## [0.3.1] — 2026-11-09 · Demo corta para publicar el 13 de noviembre (candidata)

- Es la demo corta (final frente al castillo) con todo lo corregido después de la v0.3.0:
  - entrada sin torniquetes duplicados;
  - modo Calidad Baja más liviano (v0.4.1);
  - calidad Baja por defecto.
- Nueva variable `DemoCorta` (bool) en el jugador: si está activa, `RevisarFinDemo` usa el final del castillo (Y < -11450); si no, el final es la huida de la Casa.
- **Estado del proyecto:** `DemoCorta = true` en el CDO. Para volver a generar la demo completa hay que ponerla en `false` y `ProjectVersion=0.4.x`.

## [0.4.1] — 2026-11-09 · Rendimiento

### Cambiado
- **El modo Calidad Baja ahora es más agresivo** (`AplicarOpciones`, solo si `Calidad == 0`):
  - resolución interna al 55 %;
  - `r.ViewDistanceScale 0.6`;
  - sin niebla volumétrica ni desenfoque de movimiento;
  - follaje al 50 %;
  - sombras a 512;
  - sin VSM ni Lumen (difusa y reflejos).

  Al pasar a Media o Alta se restauran los valores (distancia, niebla, blur, follaje y sombras).
- Bosques: distancia de corte de 70–95 m en las plantillas de los 7 `BP_Bosque_*`. Las instancias del mapa conservan su valor guardado de 120 m.
- Se corrigió el clímax de la Casa (radio de 16 m).

## [0.4.0] — 2026-11-09 · Demo completa (build de prueba, sin validar por el CTO)

### Añadido
- **Evento del Carrusel** (`EventoCarrusel` en el jugador, se llama cada tick después de `RevisarFinDemo`; variable `FaseCarrusel`):
  1. A menos de 13 m del carrusel, la música de feria (tag `CarruselSonido`) se detiene, el carrusel y sus luces (tag `CarruselGira`) empiezan a girar y aparece el mensaje "La música del carrusel se apagó... y empezó a girar solo."
  2. A menos de 4 m del centro, el carrusel se detiene en seco, la muñeca (tag `Muneca`) aparece entre el carrusel y la avenida (-1400, -7600) y se ejecuta `MostrarSusto`.
- **Clímax de la Casa de Alfredo** (`EventoCasa`, variable `FaseCasa`, se llama después de `EventoCarrusel`): al obtener el fusible de la caja fuerte (`TieneFusible`), se corta la linterna (`ForzarOscuridad`), aparece el mensaje "Se cortó la luz. Afuera, algo se ríe… ¡Vuelve a la entrada del parque!", el payaso (tag `Payaso`) aparece frente a la casa (8200, -14300) con un chillido posicional, y empieza la huida.
- **Nuevo final de la demo completa:** después del clímax, llegar a la entrada del parque (Y > -5300, |X| < 900) muestra la pantalla final.

### Cambiado
- El final frente al castillo (`RevisarFinDemo`) queda desactivado para v0.4.0 (umbral -99999). La demo corta v0.3.0 ya publicada no cambia.
- Se abrió la bodega (`BLQ_Bodega` a `Parque/Demo/Sobrantes`), porque la llave de la casa está ahí.

### Corregido (después del build 0.4.0, entra en la próxima versión)
- El clímax de la Casa solo se dispara **dentro de un radio de 16 m de la casa**. Así no se repite al cargar una partida guardada en otro lugar.

### Añadido (cont.)
- Mezcla propia: `SC_SFX` y `SC_Ambiente` (`MundoMagico/Audio/Mezcla`). El volumen general ahora aplica a las clases hijas.
- Diseño del tramo de la demo completa: `auditoria/diseno_sprint3_demo.md`.

## [0.3.0] — 2026-11-09 · Demo corta (objetivo de publicación: 13 de noviembre)

### Añadido
- **Fin de la demo:** la nueva función `RevisarFinDemo` del jugador, que corre cada tick después de `RevisarSalida`, marca `Terminado` al llegar al final de la avenida frente al castillo (|X| < 700, Y < -11450). Así se activa la pantalla final existente: "CAPÍTULO 1 · COMPLETADO… Continúa en el Capítulo 2".
- **Bloqueos de demo** (`Parque/Demo/Bloqueos`): una reja y un cono en el camino al tren, la torre y la montaña, y en las puertas del castillo, el teatro y la bodega.
- Checklist de prueba de humo: `auditoria/checklist_humo.md`.

### Corregido
- **No se podía entrar al parque:** 4 torniquetes duplicados (`ENT_TorniqueteD3` a `D6`) tapaban el centro del portón. Se movieron bajo el suelo a `Parque/Demo/Sobrantes`. Ahora hay 5,2 m de paso libre, verificado con trazos de colisión.

### Cambiado
- **Calidad Baja por defecto** en el primer arranque (valor por defecto de `Calidad` en el CDO = 0). El jugador puede subirla en Configuración.
- Plan acelerado a sprints de 1 semana (`PLAN_DE_TRABAJO.md`).

## [0.2.0] — 2026-11-09 (Sprint 1: Cimientos de Tensión e Interacción)

### Corregido
- La camilla `HX_Camilla3` encajonaba al jugador junto a la cama inicial al ir por la linterna. Se movió a la esquina sureste de la sala (330, 2620). Lo detectó la prueba de humo del build.
- `BP_ThirdPersonCharacter.AngelPayaso`: acceso a `Payasos[4]` con un array de 1 elemento. Ahora usa el índice 0.
- `BP_ThirdPersonCharacter.OcultarPagina`: `Accessed None` sobre `WidgetPagina`. Ahora valida con `IsValid` antes de `RemoveFromParent`.
- `Vigilar` y `RevisarFinal`: `SetTimer` con tiempo 0 se usaba para cancelar los timers de `MostrarSusto` y `MostrarFinal`. Ahora un `Branch` decide entre `SetTimer` y `ClearTimerbyFunctionName`.
- `MostrarFinal`: timer de `AbrirPanel` de 0,01 a 0,05 s.

### Añadido
- Parque: 69 props de Fab en `Parque/Fab2`:
  - 34 rocas en senderos y cerros.
  - 20 faroles apagados en senderos secundarios.
  - 4 columnas en el teatro y el castillo.
  - Señalética de metro en la avenida.
  - Herramientas afuera de la bodega.
- Parque vivo:
  - Lluvia, viento y ruidos metálicos al azar (`BP_RuidoAzar`).
  - Nieblas locales, luces que parpadean y partículas.
  - Torniquetes, boleterías y máquinas expendedoras.
- Herramientas:
  - `Abrir_MundoMagico_Liviano.bat`: abre Unreal sin RayTracing, VSM ni Lumen, solo mientras está abierto.
  - `Respaldar_Proyecto.bat`: zip con fecha de lo propio del proyecto. Conserva los últimos 10.
- Documentación del estudio: `BOARD.md`, `AUDIT_REPORT.md`, `auditoria/*.md`.

### Cambiado
- `DefaultGame.ini`: `ProjectName`, `CompanyName` y `ProjectVersion=0.2.0`.
- Render para iGPU:
  - SkyAtmosphere y VolumetricCloud ocultos.
  - 29 luces decorativas sin sombras.
  - Niebla volumétrica con rejilla 16/64.
- Audio 3D:
  - Atenuaciones `ATT_Corto`, `ATT_Medio` y `ATT_Largo` en los 12 `SND_*` y 7 `SFX`.
  - Sustos del pasillo posicionales (`PlaySoundAtLocation`).
- Fuentes: respaldo `DroidSansFallback` para ▲ ✕ ↑ ↓ ← →.
- Iluminación por zonas:
  - Avenida en penumbra.
  - Carrusel y Noria menos intensos.
- Oso Willy y León movidos a los laterales del parque.
- La botella emite `MakeNoise` al romperse.
- La noria gira sin colisión física; corrige el aviso de escala 0.
- Renombres:
  - `SM_SM_*` → `SM_*`.
  - `Ambiente_*` → `AMB_*`.
  - `Musica_*` → `MUS_*`.
- 7 packs de Fab sin uso (6 GB) archivados fuera de `Content/`.
- Nuevo `Construir_Version.bat`: sincroniza a `C:\MM_Build` y genera `Build/v<versión>`.
- Build Shipping v0.2.0: cook con 0 errores y 0 warnings.

### Mantenimiento
- 20 archivos `.tmp` de guardados interrumpidos en `Saved/` enviados a la Papelera.

### Problemas conocidos
- En el PC de desarrollo (Intel Iris Plus, 8 GB) el editor y PIE se cuelgan por GPU (`DXGI_ERROR_DEVICE_HUNG`). Ver `AUDIT_REPORT.md`.

## [0.1.0] — 2026-11-02
- Primer build Shipping Win64, 2,2 GB, 0 errores, generado desde `C:\MM_Build`.
- Incluye:
  - Hospital y pasillo.
  - Parque con atracciones.
  - Enemigo payaso.
  - Guardado y continuación (`BP_Partida`).
  - Mochila y pilas manuales.
  - Cinemáticas.
  - Menú con opciones.
