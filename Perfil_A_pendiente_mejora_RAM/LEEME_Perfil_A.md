# Perfil A: pendiente hasta mejorar el equipo

**Estado:** la configuración de calidad no está aplicada. Se revirtió el 2026-11-04 porque el proyecto no pudo correr después de activarlo en un PC con 8 GB de RAM e Intel Iris Plus.

**Cuándo aplicarlo:** después de aumentar la RAM. Lo mínimo razonable son 16 GB; 32 GB es lo más cómodo para trabajar en Unreal. Cambiar esta configuración recompila shaders y consume mucha memoria durante horas.

## Cambios en DefaultEngine.ini

```ini
[/Script/Engine.RendererSettings]
r.ReflectionMethod=2
r.DynamicGlobalIlluminationMethod=0
r.Shadow.Virtual.Enable=0
r.AllowStaticLighting=True
r.RayTracing=False
r.MegaLights.EnableForProject=False
r.RayTracing.RayTracingProxies.ProjectEnabled=False

[SystemSettings]
r.ContactShadows=1
r.VolumetricFog.GridPixelSize=16
r.VolumetricFog.GridSizeZ=64
```

## Cambios que ya quedaron en el mapa

Estos no se revirtieron y no hay que volver a aplicarlos:

- Post proceso `Exposicion_Noche`: aberración 0.15, viñeta 0.35, grano 0.08, desenfoque de movimiento 0.20 y bloom 0.35.
- Las 23 luces Stationary pasaron a Movable. Es necesario para activar `r.AllowStaticLighting=True` sin hornear.

## Archivos guardados

- `DefaultEngine_PerfilA.ini`: configuración completa del Perfil A.
- `L_Pabellon4_PerfilA.umap`: copia de referencia del mapa. No hace falta usarla.

## Cómo aplicarlo

1. Cerrar Unreal y hacer un respaldo.
2. Cerrar los demás programas.
3. Aplicar el bloque de configuración.
4. Abrir con `Abrir_MundoMagico.bat` y esperar a que terminen los shaders.
5. Probar con `stat fps`.
