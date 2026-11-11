# Guía para publicar la demo en itch.io (13-11)

Son unos 30–45 min y la hace el CTO, porque es su cuenta. Claude no inicia sesión ni sube nada por ti.

## 12-11 · Validar la versión
1. Juega `Build\v0.3.35\Windows\MundoMagico.exe` de punta a punta con **Iniciar juego**, siguiendo `auditoria/checklist_v0.3.34.md`.
2. Toma las 5 capturas mientras juegas (**Win + Alt + Impr Pant**). La lista está en `itchio_pagina_demo.md`.
3. Si no hay cierres ni bloqueos, se publica la **v0.3.35**. Si algo grave falla, se publica la **v0.3.1**.

## 13-11 · Crear la página
1. Entra a itch.io → **Dashboard** → **Create new project**.
2. Completa:
   - **Title:** `Mundo Mágico: Turno Nocturno — Demo`
   - **Kind of project:** Downloadable
   - **Classification:** Games
   - **Pricing:** No payments (o "Donate" si quieres)
   - **Short description:** copia la de `itchio_pagina_demo.md`
   - **Genre:** Survival / Horror
   - **Tags:** los de la página
   - **Cover image:** `Fuentes/UI/T_PortadaCine.png` (itch pide 630×500; recórtala o súbela y ajústala ahí)
   - **Screenshots:** las 5 capturas
   - **Description:** copia la descripción, la lista de la demo, las características y los requisitos
3. **Visibility:** déjalo en **Draft** mientras subes el archivo.

## Subir el juego
- El ZIP ya está en `Publicar\MundoMagico_Demo_v0.3.35_Win64.zip` (y el de `v0.3.1`).
- **Si pesa menos de 1 GB:** en *Uploads* → **Upload files**, elige el ZIP y marca **Windows**.
- **Si pesa más de 1 GB** (lo más probable, porque el juego ocupa 3,3 GB): itch.io no lo acepta desde el navegador, así que hay que subirlo con su programa oficial, **butler**:
  1. Descárgalo desde https://itch.io/docs/butler/installing.html.
  2. `butler login`. Se abre el navegador e inicias sesión tú.
  3. `butler push "Publicar\MundoMagico_Demo_v0.3.35_Win64.zip" TU_USUARIO/mundo-magico-turno-nocturno:windows-demo --userversion 0.3.35`
  4. Vuelve a la página: el archivo aparece en *Uploads*, marcado como Windows.

## Publicar
1. Revisa la página con **View page**.
2. Cambia **Visibility** a **Public** y guarda.
3. Comparte el enlace (y el teaser, si quieres).
