# Release del slice (checklist)

Alcance: vertical slice de 5 contratos. Solo Blackline. El push y la publicación los hace el CTO.

## Build
1. Cerrar editores y juegos (PC de 8 GB).
2. Desarrollo: `scripts\Rebuild_And_Package.ps1 -Version X.Y.Z`
3. Shipping: `scripts\Build.ps1 -Config Shipping -Version X.Y.Z` (sin consola ni stat).
4. Salida: `Build\vX.Y.Z-<Config>\Windows\Blackline.exe`. Comprimir esa carpeta en un ZIP.

## Antes de subir
- [ ] Jugar la campaña completa C01 → C21 en la build Shipping, en otro PC.
- [ ] Probar los 4 finales: rescate, con costo, malo y de la ley (tecla P / Círculo al llegar).
- [ ] Probar retén (guardia y barrera), arresto, volcado, lluvia y audio.
- [ ] `Saved\Logs` sin errores críticos.
- [ ] Aviso de ficción y contenido visible (sale una vez por sesión, ~13 s tras el primer encargo).
- [ ] Revisar que «La Cofradía» y otros nombres no coincidan con empresas reales.
- [ ] Leer los términos vigentes de Fab (uso en juegos) y de la plataforma antes de publicar.
- [ ] Descargo en la página: obra de ficción; crimen, extorsión, animal en peligro sin daño mostrado, luces intermitentes.

## Controles (para la página)
Teclado: W/S acelerar-frenar, A/D girar, Espacio freno, V cámara, F 4x4, L luces, I motor, J inhibidor, E interactuar, R volver a la carretera, H ocultar ayuda, F5/F6 contrato anterior/siguiente, P avisar a la policía (solo al final).
Mando PS4: R2/L2, stick, Cuadrado freno, Triángulo cámara, L1 4x4, cruceta luces/motor/inhibidor, Círculo avisar a la policía (solo al final).

## Pendiente fuera del slice
Remapeo de controles, localización completa, arte propio (furgón, Pancito, camiones chilenos), Lumen en una zona, los otros 20 contratos.
