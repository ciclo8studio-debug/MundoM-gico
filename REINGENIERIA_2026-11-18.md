# Ciclo Studio Game: reingeniería del 18-11-2026

Los 5 super-agentes rediseñaron su área, cada uno con su equipo. Detalle en `auditoria/reingenieria_2026-11-18/`, un archivo por departamento. Este documento resuelve los cruces entre ellos y fija **una sola hoja de ruta**.

## Pilares del juego (@tech-lead · creative-director)
1. **La luz te salva y te delata.** La linterna ahuyenta a la Sombra, pero hace que te vea de más lejos.
2. **El sonido manda.** Pasos por suelo, oído de la IA, botella, aliento y música que sigue la tensión.
3. **Esconderse y aguantar el aliento.** Escondites con riesgo y respiración contenida.
4. **El hospital cuenta la historia de Isidora.** Páginas, fichas y el código 03:33.

**Fuera de la demo del 10-12:** el parque, el payaso, el 2.º piso, la Casona jugable y la 3ª persona, que queda oculta. La Casona entra solo como teaser de 75 s.

## Arquitectura objetivo (cruces resueltos)
| Pieza | Responsable | Qué es |
|---|---|---|
| `BP_ThirdPersonCharacter` delgado | tech-lead | Un orquestador. La lógica pasa a componentes con el patrón **estrangulador**: `Branch(DA_Tuning.Arq_X)` elige la ruta vieja o la nueva y permite volver atrás sin abrir Unreal. |
| `AC_Linterna` | tech-lead + gameplay | Batería (0,35 %/s), delata (más rango de visión) y ahuyenta (2,5 s apuntando), y sombras según la calidad. |
| `AC_Sigilo` + `AC_Ruido` | gameplay | Ruido, aliento y escondites. Te revisan el escondite si te vieron entrar. |
| `AC_Inventario` + `AC_Vida` | gameplay | Botellas, botiquín y pilas; 4 golpes y revivir en checkpoint. |
| `AC_Interaccion` | tech-lead | Va **después** del freeze del 25-11, porque es el paso de más riesgo. |
| `AC_Percepcion` + `BPI_IA` | gameplay | IA unificada para la Sombra, el maniquí e Isidora: vista con sospecha gradual, oído, mirada y repeler. Timer de 0,2 s y 9 estados. |
| `BP_DirectorMiedo` = planificador | gameplay | Tensión, empujes, respiros (≥60 s después de una persecución) y turnos. Reparte los ruidos. |
| `AC_HUDBridge` (= `AC_UI` de ux) | tech-lead + ux | Lleva una pila de pantallas y es el único que toca los widgets. El GameInstance guarda las opciones, el dispositivo y las páginas. Los avisos salen de `DT_Avisos` (teclado/mando). |
| Audio | ux | `SC_Musica`/`SFX`/`Ambiente`/`Pasos`/`Voz`/`UI`, 4 mezclas (Pausa, Novela, Susto, Interior), 7 límites de sonidos simultáneos y música de 3 capas según la tensión. |
| `BP_VolumenZona` / `BP_TransicionNivel` | tech-lead + world | Reemplazan la lógica con coordenadas fijas (`ActualizarZona`, `RevisarSalida`, `RevisarFinDemo`). |
| `DA_NovelaData` + String Tables | world + qa | Las 15 páginas en voz clínica de 1998. Todo texto nuevo va a `ST_UI`/`ST_Mensajes`/`ST_Paginas`/`ST_Subtitulos`. |
| Guardado | tech-lead | `Guardar`/`Cargar` quedan como fachada y solo se agregan campos (`Version:int`). Las partidas viejas siguen cargando. |

## Nivel 1 rediseñado (@world-builder + @gameplay-engineer)
- **Recorrido:** despertar → pasillo de Urgencias → recepción (1.ª vez que se ve la Sombra) → escalera de servicio → **sótano en anillo** (Archivo ↔ Rayos X) → negatoscopio (marca 03:33) → puerta con código **0333** → morgue, gaveta 4 (`LlaveMorgue`) → **clímax**: alarma y persecución → glitch → TV.
- Duración: unos 25 min.
- 5 escondites (3 nuevos en el sótano) y 8 sustos, todos pasando por el Director de miedo.
- Páginas 1–15; las 4, 10, 13 y 14 son obligatorias.
- Recursos en `TargetPoint` con etiqueta: alcanzan para unos 350 % de batería, lo que da luz en cerca del 66 % del nivel.
- **Look:** niebla que se espesa al bajar (0,02 → 0,05), un faro de SALIDA verde o puerta naranja al final de cada tramo, rojo solo para el peligro y menos de 1.200 draw calls.

## Hoja de ruta por builds (una por día)
| Build | Fecha | Contenido | Verificación |
|---|---|---|---|
| — | 19-11 | **Herramientas sin Unreal:** `Probar.bat` nuevo (zonas, último log, borrar partida en `.bak`, modo mudo) y `Recoger_Logs.ps1` | Ejecutar el `.bat` |
| v0.3.62 | 19-11 | **A6** sombras según la calidad + **A2** avisos según el dispositivo | Humo H1–H3 en Baja y Media, mando y teclado |
| v0.3.63 | 20-11 | **`DA_NovelaData`** con las 15 páginas + **contador de páginas** | Recoger 2 páginas y ver el contador y el texto |
| v0.3.64 | 21-11 | **`AC_Linterna`** (`Arq_Linterna`): delata, ahuyenta y batería en `DA_Tuning` (**A3**) | Que la Sombra huya al apuntarle 2,5 s |
| v0.3.65 | 22-11 | **`AC_Percepcion` v1 en la Sombra:** sospecha gradual, pausa tras el golpe y anti-atasco (**A7**) + **botella** (**A5**) | Lanzar una botella y ver que investiga |
| v0.3.66 | 23-11 | **Pistas y objetivos** (**A4**) + **pantalla final de la demo** (**A8**) | Esperar 180 s y ver la pista |
| v0.3.67 | 24-11 | **Pipeline:** `-humo` automático, `metricas.csv`, calidad automática, `DirectoriesToNeverCook` y cook sin SM6 (si se aprueba) | ZIP < 1,3 GB y humo en verde |
| **v0.4.0-rc1** | **25-11** | **Freeze de gameplay** + checklist v0.3.61 completa → **testers** | Gate: 0 S1, humo completo, ≥25 fps en Baja |
| v0.4.x | 26-11 → 1-12 | **Sprint B:** sótano y morgue (blockout → kit → sustos), subtítulos y mezcla, música de 3 capas, `AC_Vida` + autoguardado, clímax de la llave, maniquí y teaser `L_Casona_Teaser` | Humo por zona |
| v0.4.x-rc2 | 3-12 | Freeze de contenido y feedback de testers | Gate RC |
| **v0.5.0** | **8-12 gold · 10-12 pública** | itch.io bilingüe por butler (canal `windows-demo`, lo sube el CTO) | Gate gold: 0 S1/S2 y un tester con iGPU sin cierres |

**Si el calendario aprieta, se corta en este orden:** el atajo de la lavandería → el maniquí → la música de 3 capas (queda en 2) → la revisión de escondites.

## Calidad (@qa-publisher)
- **"Listo"** = 6 puntos, entre ellos probar con el interruptor de `DA_Tuning` **apagado**.
- Humo H1–H7 de unos 25 min por build.
- Bugs con severidad S1–S4 en `auditoria/bugs.md`.
- Kit de testers con mensaje listo, formulario v2 de 12 preguntas, rutas de logs y crashes, y metas: 0 cierres y ≥80 % termina.

## Decisiones del CTO (recomendación entre paréntesis)
1. **Patrón `Arq_*` para partir el BP del jugador** (sí).
2. **Demo sin parque ni payaso**, con hospital y teaser de la Casona: unos 400 MB menos (sí).
3. **Cook sin shaders SM6:** unos 250 MB menos y sin efecto, porque el juego corre en DX11. Sería una excepción acotada al veto del Perfil A (sí).
4. **Canon:** Isidora tiene leucemia en recaída más miocardiopatía por la quimio (el "corazón débil" del HTML). Muere a las 03:33 del 13/03/1998. La Casona es la casa de Alfredo. El año queda en 1998, no 2001 (sí a todo).
5. **Vida:** 4 golpes y checkpoint (sí) o muerte de un golpe.
6. **Clímax de la llave y revisión de escondites en la demo** (sí; la revisión es lo primero que se corta).
7. **UX:** subtítulos activos por defecto, música de 3 capas y mudar Pausa/Opciones después del 10-12 (sí).
8. **QA:**
   - Gate firme aunque mueva fechas (sí).
   - Versiones v0.4.0-rc1 → v0.5.0 (sí).
   - Texto nuevo directo a String Tables (sí).
