# Mundo Mágico: Turno Nocturno — herramientas

Scripts que construyen y actualizan el juego de terror hecho en Unreal Engine 5.8. El proyecto de Unreal vive aparte, en `Documents/Unreal Projects/MUNDO_MÁGICO`. Este repositorio guarda solo el código y la documentación: no incluye modelos, audio, builds ni respaldos.

## Contenido

| Carpeta | Qué hace |
|---|---|
| `Isidora3D/` | Construye el modelo 3D de Isidora en Blender sobre el esqueleto del maniquí de UE. |
| `Isidora3D/parque/` | Genera e importa el parque: terreno, caminos, reja, árboles, sonidos, mapa del HUD. |
| `herramientas/` | Abre el editor en DirectX 11 con el servidor MCP. |
| `Perfil_A_pendiente_mejora_RAM/` | Configuración gráfica reservada para cuando el equipo tenga más RAM. |
| `Mundo Mágico_ Turno Nocturno.html` | Versión original del juego en HTML, usada como referencia. |

## Flujo habitual

1. Generar en Blender: `blender -b --python Isidora3D/parque/<script>.py`
2. Importar sin abrir el editor: `UnrealEditor-Cmd.exe <proyecto>.uproject -run=pythonscript -script=<importar_*.py> -unattended -nullrhi`
3. Colocar y ajustar en el mapa desde el editor, con MCP.

Los archivos `.json` de `Isidora3D/parque/` son datos generados por estos scripts, como posiciones de actores, trazado y árboles.
