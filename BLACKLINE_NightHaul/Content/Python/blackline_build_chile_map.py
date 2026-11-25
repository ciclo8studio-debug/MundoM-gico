"""Blackline: crea el material de vertex color, importa los waypoints y arma el mapa L_ChileRoute (Chile comprimido 1:100).

Ejecucion headless (editor cerrado):
  UnrealEditor-Cmd.exe Blackline.uproject /Engine/Maps/Entry -ExecutePythonScript="<ruta>/blackline_build_chile_map.py" -unattended -nullrhi -nosplash
Requiere haber corrido antes blackline_build_map.py (GM_Blackline, BP del furgon, materiales) y el C++ compilado.
La ruta y el terreno se generan al iniciar el juego (AChileRouteActor); el mapa solo trae luces, clima y el actor.
"""
import os
import sys

import unreal

TAG = "BLK: "
AT = unreal.AssetToolsHelpers.get_asset_tools()
EAL = unreal.EditorAssetLibrary
MEL = unreal.MaterialEditingLibrary
DIR_MAT = "/Game/Blackline/Materials"
DIR_MAP = "/Game/Blackline/Maps"
MAP_NAME = "L_ChileRoute"


def log(m):
    unreal.log(TAG + str(m))


def warn(m):
    unreal.log_warning(TAG + str(m))


def setp(obj, names, value):
    for n in names:
        try:
            obj.set_editor_property(n, value)
            return True
        except Exception:
            continue
    warn("no pude fijar %s en %s" % (names, obj))
    return False


def ensure_material():
    """M_VertexColor: color de vertice + un minimo emisivo (ambiente de noche: el suelo nunca queda negro del todo)."""
    path = DIR_MAT + "/M_VertexColor"
    if EAL.does_asset_exist(path):
        EAL.delete_asset(path)  # se regenera para aplicar cambios del material
    if not EAL.does_directory_exist(DIR_MAT):
        EAL.make_directory(DIR_MAT)
    mat = AT.create_asset("M_VertexColor", DIR_MAT, unreal.Material, unreal.MaterialFactoryNew())
    vc = MEL.create_material_expression(mat, unreal.MaterialExpressionVertexColor, -600, 0)
    MEL.connect_material_property(vc, "RGB", unreal.MaterialProperty.MP_BASE_COLOR)
    emi = MEL.create_material_expression(mat, unreal.MaterialExpressionMultiply, -300, 200)
    emi.set_editor_property("const_b", 0.10)
    MEL.connect_material_expressions(vc, "RGB", emi, "A")
    MEL.connect_material_property(emi, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    rough = MEL.create_material_expression(mat, unreal.MaterialExpressionConstant, -600, 400)
    rough.set_editor_property("r", 0.92)
    MEL.connect_material_property(rough, "", unreal.MaterialProperty.MP_ROUGHNESS)
    mat.set_editor_property("two_sided", True)
    MEL.recompile_material(mat)
    EAL.save_asset(path, only_if_is_dirty=False)
    log("material regenerado " + path)


def import_waypoints():
    sys.path.insert(0, os.path.join(unreal.SystemLibrary.get_project_directory(), "Content", "Python"))
    import blackline_chile_route as c
    errs = c.validate()
    if errs:
        raise RuntimeError("waypoints invalidos: %s" % errs)
    c.import_datatable()
    import blackline_contracts as k
    errs = k.validate()
    if errs:
        raise RuntimeError("contratos invalidos: %s" % errs)
    k.import_datatable()


def build_map():
    if not EAL.does_directory_exist(DIR_MAP):
        EAL.make_directory(DIR_MAP)
    lvl = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    actor_sub = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    map_path = "%s/%s" % (DIR_MAP, MAP_NAME)
    umap = os.path.join(unreal.SystemLibrary.get_project_directory(), "Content", "Blackline", "Maps", MAP_NAME + ".umap")
    if os.path.exists(umap):
        os.remove(umap)
        unreal.AssetRegistryHelpers.get_asset_registry().scan_paths_synchronous([DIR_MAP], True)
    if not lvl.new_level(map_path):
        raise RuntimeError("new_level fallo para " + map_path)

    # Luces de noche, livianas (sin sombras de luna, sin recaptura del cielo por cuadro).
    ds = actor_sub.spawn_actor_from_class(unreal.DirectionalLight, unreal.Vector(0, 0, 1000))
    ds.set_actor_label("Luna")
    ds.set_actor_rotation(unreal.Rotator(roll=0, pitch=-38, yaw=60), False)
    dc = ds.get_component_by_class(unreal.DirectionalLightComponent)
    dc.set_editor_property("intensity", 2.0)
    dc.set_editor_property("light_color", unreal.Color(150, 175, 255, 255))
    dc.set_mobility(unreal.ComponentMobility.MOVABLE)
    setp(dc, ["atmosphere_sun_light"], False)
    setp(dc, ["cast_shadows"], False)
    # Luz de relleno azul tenue (ambiente de noche sin recaptura del cielo): sin sombras, sin sol de atmosfera.
    fill = actor_sub.spawn_actor_from_class(unreal.DirectionalLight, unreal.Vector(0, 0, 1100))
    fill.set_actor_label("Relleno_Noche")
    fill.set_actor_rotation(unreal.Rotator(roll=0, pitch=-25, yaw=240), False)
    fc2 = fill.get_component_by_class(unreal.DirectionalLightComponent)
    fc2.set_editor_property("intensity", 0.9)
    fc2.set_editor_property("light_color", unreal.Color(120, 140, 215, 255))
    fc2.set_mobility(unreal.ComponentMobility.MOVABLE)
    setp(fc2, ["atmosphere_sun_light"], False)
    setp(fc2, ["cast_shadows"], False)
    actor_sub.spawn_actor_from_class(unreal.SkyAtmosphere, unreal.Vector(0, 0, 0))
    sk = actor_sub.spawn_actor_from_class(unreal.SkyLight, unreal.Vector(0, 0, 500))
    skc = sk.get_component_by_class(unreal.SkyLightComponent)
    skc.set_editor_property("intensity", 0.4)
    skc.set_mobility(unreal.ComponentMobility.MOVABLE)
    skc.set_editor_property("real_time_capture", False)
    fog = actor_sub.spawn_actor_from_class(unreal.ExponentialHeightFog, unreal.Vector(0, 0, 0))
    fc = fog.get_component_by_class(unreal.ExponentialHeightFogComponent)
    setp(fc, ["fog_density"], 0.02)
    setp(fc, ["enable_volumetric_fog", "volumetric_fog"], False)
    # Niebla azulada: da ambiente a la distancia en vez de negro.
    setp(fc, ["fog_inscattering_luminance", "fog_inscattering_color"], unreal.LinearColor(0.02, 0.03, 0.07, 1.0))

    try:
        ppv = actor_sub.spawn_actor_from_class(unreal.PostProcessVolume, unreal.Vector(0, 0, 0))
        ppv.set_actor_label("PP_Noche")
        ppv.set_editor_property("unbound", True)
        st = ppv.get_editor_property("settings")
        st.set_editor_property("override_auto_exposure_bias", True)
        st.set_editor_property("auto_exposure_bias", -1.0)
        ppv.set_editor_property("settings", st)
    except Exception as e:
        warn("post proceso fallo: %r" % (e,))

    # Ruta de Chile (se construye al iniciar el juego) y director de clima.
    route_cls = unreal.load_class(None, "/Script/Blackline.ChileRouteActor")
    route = actor_sub.spawn_actor_from_class(route_cls, unreal.Vector(0, 0, 0))
    route.set_actor_label("Ruta_Chile")
    setp(route, ["zone_id"], 2)
    wd_cls = unreal.load_class(None, "/Script/Blackline.WeatherDirector")
    wd = actor_sub.spawn_actor_from_class(wd_cls, unreal.Vector(0, 0, 200))
    wd.set_actor_label("Clima")

    # PlayerStart provisorio: la ruta teletransporta al jugador al inicio real de la zona.
    ps = actor_sub.spawn_actor_from_class(unreal.PlayerStart, unreal.Vector(0, 0, 3000))
    ps.set_actor_label("Inicio")

    world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    gm = unreal.load_asset("/Game/Blackline/Core/GM_Blackline")
    world.get_world_settings().set_editor_property("default_game_mode", gm.generated_class())

    lvl.save_current_level()
    log("mapa guardado: " + map_path)


def main():
    log("inicio chile")
    ensure_material()
    import_waypoints()
    build_map()
    log("FIN OK CHILE")


try:
    main()
except Exception as e:
    import traceback
    unreal.log_error(TAG + "FALLO CHILE: %r\n%s" % (e, traceback.format_exc()))
finally:
    unreal.SystemLibrary.quit_editor()
