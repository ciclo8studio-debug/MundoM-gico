"""Blackline: crea BP del furgon, ruedas, inputs, GameMode y el mapa plano con ruta estilo chilena.

Ejecucion headless (editor cerrado):
  UnrealEditor-Cmd.exe Blackline.uproject -ExecutePythonScript="<ruta>/blackline_build_map.py" -unattended -nullrhi -nosplash
Idempotente: reusa assets existentes; el mapa se regenera siempre.
Convencion de ruta: Chile conduce por la derecha (+Y es derecha mirando +X), eje central doble amarillo,
bordes blancos, berma de tierra y postes delineadores blancos cada 50 m.
"""
import unreal

TAG = "BLK: "
AT = unreal.AssetToolsHelpers.get_asset_tools()
EAL = unreal.EditorAssetLibrary
MEL = unreal.MaterialEditingLibrary

ROOT = "/Game/Blackline"
DIR_VEH = ROOT + "/Vehicles"
DIR_IN = ROOT + "/Input"
DIR_MAT = ROOT + "/Materials"
DIR_CORE = ROOT + "/Core"
DIR_MAP = ROOT + "/Maps"
MAP_NAME = "L_FlatRoad_Chile"

SKM_PATH = "/Game/Vehicles/OffroadCar/SKM_Offroad"
ABP_PATH = "/Game/Vehicles/OffroadCar/Offroad_AnimBP"
CUBE = "/Engine/BasicShapes/Cube.Cube"
BASIC_MAT = "/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"


def log(msg):
    unreal.log(TAG + str(msg))


def warn(msg):
    unreal.log_warning(TAG + str(msg))


def setp(obj, names, value):
    """Intenta varias variantes del nombre de propiedad (Python las convierte a snake_case)."""
    for n in names:
        try:
            obj.set_editor_property(n, value)
            return True
        except Exception:
            continue
    warn("no pude fijar %s en %s" % (names, obj))
    return False


def ensure_dir(path):
    if not EAL.does_directory_exist(path):
        EAL.make_directory(path)


def make_bp(folder, name, parent_cls):
    path = "%s/%s" % (folder, name)
    if EAL.does_asset_exist(path):
        log("reuso " + path)
        return EAL.load_asset(path)
    f = unreal.BlueprintFactory()
    f.set_editor_property("parent_class", parent_cls)
    bp = AT.create_asset(name, folder, unreal.Blueprint, f)
    log("creado " + path)
    return bp


def save(path):
    EAL.save_asset(path, only_if_is_dirty=False)


# ----------------------------------------------------------------------------- Materiales
def make_mi(name, color, roughness=None):
    path = "%s/%s" % (DIR_MAT, name)
    if EAL.does_asset_exist(path):
        mi = EAL.load_asset(path)
    else:
        f = unreal.MaterialInstanceConstantFactoryNew()
        mi = AT.create_asset(name, DIR_MAT, unreal.MaterialInstanceConstant, f)
    parent = unreal.load_asset(BASIC_MAT)
    mi.set_editor_property("parent", parent)
    names = [str(n) for n in MEL.get_vector_parameter_names(parent)]
    pname = "Color" if "Color" in names else (names[0] if names else "Color")
    MEL.set_material_instance_vector_parameter_value(mi, pname, unreal.LinearColor(*color))
    MEL.update_material_instance(mi)
    save(path)
    log("MI %s param=%s" % (name, pname))
    return mi


def build_materials():
    ensure_dir(DIR_MAT)
    return {
        "asphalt": make_mi("MI_Asphalt", (0.025, 0.025, 0.03, 1)),
        "ground": make_mi("MI_Ground", (0.12, 0.09, 0.06, 1)),
        "shoulder": make_mi("MI_Shoulder", (0.18, 0.15, 0.11, 1)),
        "yellow": make_mi("MI_LineYellow", (0.9, 0.65, 0.0, 1)),
        "white": make_mi("MI_LineWhite", (0.85, 0.85, 0.85, 1)),
        "mud": make_mi("MI_Mud", (0.06, 0.035, 0.02, 1)),
    }


# ----------------------------------------------------------------------------- Input
def make_input_asset(name, cls, factory_names):
    path = "%s/%s" % (DIR_IN, name)
    if EAL.does_asset_exist(path):
        return EAL.load_asset(path)
    factory = None
    for fn in factory_names:
        fcls = getattr(unreal, fn, None)
        if fcls:
            try:
                factory = fcls()
                break
            except Exception:
                pass
    asset = AT.create_asset(name, DIR_IN, cls, factory)
    if asset is None:
        warn("no se pudo crear " + path)
    return asset


def build_input():
    ensure_dir(DIR_IN)
    VT = unreal.InputActionValueType
    specs = [
        ("IA_Throttle", VT.AXIS1D), ("IA_Steer", VT.AXIS1D), ("IA_Brake", VT.AXIS1D),
        ("IA_Handbrake", VT.BOOLEAN), ("IA_Look", VT.AXIS2D), ("IA_ToggleCamera", VT.BOOLEAN),
        ("IA_Interact", VT.BOOLEAN), ("IA_DriveMode", VT.BOOLEAN),
        ("IA_ToggleHeadlights", VT.BOOLEAN), ("IA_ToggleEngine", VT.BOOLEAN),
        ("IA_ToggleJammer", VT.BOOLEAN), ("IA_Recover", VT.BOOLEAN),
    ]
    actions = {}
    for name, vt in specs:
        a = make_input_asset(name, unreal.InputAction, ["InputAction_Factory", "InputActionFactory"])
        if a:
            a.set_editor_property("value_type", vt)
            save("%s/%s" % (DIR_IN, name))
            actions[name] = a
    imc = make_input_asset("IMC_Van", unreal.InputMappingContext,
                           ["InputMappingContext_Factory", "InputMappingContextFactory"])
    if imc:
        # Se construyen las asignaciones completas (accion + tecla + modificadores) y se escriben de una vez:
        # map_key() devuelve una copia, asi que modificar su resultado no cambia el IMC.
        def key(n):
            k = unreal.Key()
            k.set_editor_property("key_name", n)
            return k

        def mod(cls):
            return unreal.new_object(cls, outer=imc)

        rows = []

        def add(action, keyname, negate=False, swizzle=False, deadzone=False):
            m = unreal.EnhancedActionKeyMapping()
            m.set_editor_property("action", actions[action])
            m.set_editor_property("key", key(keyname))
            mods = []
            if deadzone:
                mods.append(mod(unreal.InputModifierDeadZone))
            if swizzle:
                mods.append(mod(unreal.InputModifierSwizzleAxis))
            if negate:
                mods.append(mod(unreal.InputModifierNegate))
            m.set_editor_property("modifiers", mods)
            rows.append(m)

        # --- Teclado y mouse
        add("IA_Throttle", "W"); add("IA_Throttle", "Up")
        add("IA_Brake", "S"); add("IA_Brake", "Down")
        add("IA_Steer", "D"); add("IA_Steer", "Right")
        add("IA_Steer", "A", negate=True); add("IA_Steer", "Left", negate=True)
        add("IA_Handbrake", "SpaceBar")
        add("IA_ToggleCamera", "V")
        add("IA_Interact", "E"); add("IA_Interact", "LeftMouseButton")
        add("IA_DriveMode", "F")
        add("IA_ToggleHeadlights", "L")
        add("IA_ToggleEngine", "I")
        add("IA_ToggleJammer", "J")
        add("IA_Recover", "R")
        add("IA_Look", "MouseX")
        add("IA_Look", "MouseY", swizzle=True)
        # --- Mando (PS4/DualShock vista como XInput): R2 acelera, L2 frena, stick izq. gira, stick der. mira
        add("IA_Throttle", "Gamepad_RightTriggerAxis")
        add("IA_Brake", "Gamepad_LeftTriggerAxis")
        add("IA_Steer", "Gamepad_LeftX", deadzone=True)
        add("IA_Look", "Gamepad_RightX", deadzone=True)
        add("IA_Look", "Gamepad_RightY", swizzle=True, deadzone=True)
        add("IA_Handbrake", "Gamepad_FaceButton_Left")      # Cuadrado
        add("IA_ToggleCamera", "Gamepad_FaceButton_Top")    # Triangulo
        add("IA_Interact", "Gamepad_FaceButton_Right")      # Circulo
        add("IA_DriveMode", "Gamepad_LeftShoulder")         # L1
        add("IA_ToggleHeadlights", "Gamepad_DPad_Up")
        add("IA_ToggleEngine", "Gamepad_DPad_Down")
        add("IA_ToggleJammer", "Gamepad_DPad_Left")
        add("IA_Recover", "Gamepad_Special_Left")           # Select / Share

        dkm = imc.get_editor_property("default_key_mappings")
        dkm.set_editor_property("mappings", rows)
        imc.set_editor_property("default_key_mappings", dkm)
        save(DIR_IN + "/IMC_Van")
        log("IMC_Van: %d asignaciones" % len(rows))
    return actions, imc


# ----------------------------------------------------------------------------- Furgon
def pick_wheel_bones(mesh_comp):
    n = mesh_comp.get_num_bones()
    bones = [str(mesh_comp.get_bone_name(i)) for i in range(n)]
    log("huesos: %s" % bones)
    found = {}
    # Esqueleto de la plantilla OffroadCar: PhysWheel_<F|B><L|R>
    for key, bone in (("FL", "PhysWheel_FL"), ("FR", "PhysWheel_FR"), ("RL", "PhysWheel_BL"), ("RR", "PhysWheel_BR")):
        if bone in bones:
            found[key] = bone
    log("ruedas detectadas: %s" % found)
    return found


def build_van(actions, imc):
    ensure_dir(DIR_VEH)
    wf_parent = unreal.load_class(None, "/Script/Blackline.BlacklineWheelFront")
    wr_parent = unreal.load_class(None, "/Script/Blackline.BlacklineWheelRear")
    van_parent = unreal.load_class(None, "/Script/Blackline.BlacklineVanPawn")
    if not (wf_parent and wr_parent and van_parent):
        raise RuntimeError("clases C++ Blackline no encontradas: compila el modulo primero")

    bp_wf = make_bp(DIR_VEH, "BP_VanWheelFront", wf_parent)
    bp_wr = make_bp(DIR_VEH, "BP_VanWheelRear", wr_parent)
    for b in (bp_wf, bp_wr):
        unreal.BlueprintEditorLibrary.compile_blueprint(b)
    save(DIR_VEH + "/BP_VanWheelFront")
    save(DIR_VEH + "/BP_VanWheelRear")

    bp = make_bp(DIR_VEH, "BP_BlacklineVan", van_parent)
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    cdo = unreal.get_default_object(bp.generated_class())

    skm = unreal.load_asset(SKM_PATH)
    abp = unreal.load_asset(ABP_PATH)
    mesh = cdo.get_editor_property("mesh")
    mesh.set_skeletal_mesh_asset(skm)
    if abp:
        mesh.set_anim_instance_class(abp.generated_class())
    bones = pick_wheel_bones(mesh)

    # Carroceria y neumaticos visuales (mallas aparte en la plantilla OffroadCar)
    body = unreal.load_asset("/Game/Vehicles/OffroadCar/SM_Offroad_Body")
    tire = unreal.load_asset("/Game/Vehicles/OffroadCar/SM_Offroad_Tire")
    for prop, asset in (("body_visual", body), ("wheel_visual_fl", tire), ("wheel_visual_fr", tire),
                        ("wheel_visual_rl", tire), ("wheel_visual_rr", tire)):
        try:
            comp = cdo.get_editor_property(prop)
            comp.set_static_mesh(asset)
        except Exception as e:
            warn("no pude asignar malla a %s: %r" % (prop, e))

    move = cdo.get_editor_property("vehicle_movement_component")
    order = [("FL", bp_wf), ("FR", bp_wf), ("RL", bp_wr), ("RR", bp_wr)]
    setups = []
    for key, wbp in order:
        if key not in bones:
            warn("falta hueso de rueda " + key)
            continue
        s = unreal.ChaosWheelSetup()
        s.set_editor_property("wheel_class", wbp.generated_class())
        s.set_editor_property("bone_name", bones[key])
        setups.append(s)
    move.set_editor_property("wheel_setups", setups)
    log("wheel_setups: %d" % len(setups))

    # Input
    setp(cdo, ["default_imc", "DefaultIMC"], imc)
    for name, prop in [("IA_Throttle", "ia_throttle"), ("IA_Steer", "ia_steer"), ("IA_Brake", "ia_brake"),
                       ("IA_Handbrake", "ia_handbrake"), ("IA_Look", "ia_look"),
                       ("IA_ToggleCamera", "ia_toggle_camera"), ("IA_Interact", "ia_interact"),
                       ("IA_DriveMode", "ia_drive_mode"),
                       ("IA_ToggleHeadlights", "ia_toggle_headlights"), ("IA_ToggleEngine", "ia_toggle_engine"),
                       ("IA_ToggleJammer", "ia_toggle_jammer"), ("IA_Recover", "ia_recover")]:
        if name in actions:
            setp(cdo, [prop, name], actions[name])

    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    save(DIR_VEH + "/BP_BlacklineVan")
    return bp


def build_gamemode(van_bp):
    ensure_dir(DIR_CORE)
    gm = make_bp(DIR_CORE, "GM_Blackline", unreal.GameModeBase)
    unreal.BlueprintEditorLibrary.compile_blueprint(gm)
    cdo = unreal.get_default_object(gm.generated_class())
    setp(cdo, ["default_pawn_class"], van_bp.generated_class())
    unreal.BlueprintEditorLibrary.compile_blueprint(gm)
    save(DIR_CORE + "/GM_Blackline")
    return gm


# ----------------------------------------------------------------------------- Mapa
def spawn_box(actor_sub, cube, mat, label, loc_m, size_m, collide=True, folder="Ruta"):
    """Caja alineada a ejes. loc_m centro (x,y,z) y size_m (x,y,z) en metros."""
    a = actor_sub.spawn_actor_from_class(
        unreal.StaticMeshActor, unreal.Vector(loc_m[0] * 100, loc_m[1] * 100, loc_m[2] * 100))
    a.set_actor_label(label)
    a.set_folder_path(folder)
    comp = a.static_mesh_component
    comp.set_static_mesh(cube)
    a.set_actor_scale3d(unreal.Vector(size_m[0], size_m[1], size_m[2]))
    if mat:
        comp.set_material(0, mat)
    if not collide:
        comp.set_collision_enabled(unreal.CollisionEnabled.NO_COLLISION)
    return a


def build_map(mats):
    ensure_dir(DIR_MAP)
    lvl = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    actor_sub = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    map_path = "%s/%s" % (DIR_MAP, MAP_NAME)
    import os
    umap = os.path.join(unreal.SystemLibrary.get_project_directory(), "Content", "Blackline", "Maps", MAP_NAME + ".umap")
    if os.path.exists(umap):
        # Ejecutar con /Engine/Maps/Entry como mapa de inicio para que el editor no tenga abierto este archivo.
        os.remove(umap)  # new_level no sobrescribe; regeneramos siempre
        try:
            unreal.AssetRegistryHelpers.get_asset_registry().scan_paths_synchronous([DIR_MAP], True)
        except Exception as e:
            warn("rescan del registro fallo: %r" % (e,))
    if not lvl.new_level(map_path):
        raise RuntimeError("new_level fallo para " + map_path)
    cube = unreal.load_asset(CUBE)

    L = 1600.0       # largo de ruta (m), eje +X
    X0 = 0.0
    cx = X0 + L / 2
    lane = 3.5       # ancho de carril
    # Terreno plano 2 km x 400 m (tope en z=0)
    spawn_box(actor_sub, cube, mats["ground"], "Terreno", (cx, 0, -0.5), (L + 400, 400, 1), folder="Terreno")
    # Bermas de tierra compactada (2.5 m a cada lado) y calzada
    spawn_box(actor_sub, cube, mats["shoulder"], "Berma_Izq", (cx, -(lane + 1.25), 0.005), (L, 2.5, 0.01))
    spawn_box(actor_sub, cube, mats["shoulder"], "Berma_Der", (cx, (lane + 1.25), 0.005), (L, 2.5, 0.01))
    spawn_box(actor_sub, cube, mats["asphalt"], "Calzada", (cx, 0, 0.01), (L, lane * 2, 0.02))
    # Lineas (sin colision): doble amarilla central y blancas de borde
    top = 0.0225
    for off in (-0.12, 0.12):
        spawn_box(actor_sub, cube, mats["yellow"], "Eje_Amarillo", (cx, off, top), (L, 0.12, 0.005), False)
    for side in (-1, 1):
        spawn_box(actor_sub, cube, mats["white"], "Borde_Blanco", (cx, side * (lane - 0.2), top), (L, 0.12, 0.005), False)
    # Postes delineadores blancos cada 50 m a ambos lados (estilo Ruta 5)
    x = X0 + 25.0
    while x < X0 + L:
        for side in (-1, 1):
            spawn_box(actor_sub, cube, mats["white"], "Poste", (x, side * (lane + 2.2), 0.5), (0.1, 0.1, 1.0), True, "Postes")
        x += 50.0

    # Luz nocturna ligera
    ds = actor_sub.spawn_actor_from_class(unreal.DirectionalLight, unreal.Vector(0, 0, 1000))
    ds.set_actor_label("Luna")
    ds.set_actor_rotation(unreal.Rotator(roll=0, pitch=-38, yaw=60), False)
    dc = ds.get_component_by_class(unreal.DirectionalLightComponent)
    dc.set_editor_property("intensity", 0.6)
    dc.set_mobility(unreal.ComponentMobility.MOVABLE)
    setp(dc, ["cast_shadows"], False)  # perfil liviano: la luna no proyecta sombras
    setp(dc, ["atmosphere_sun_light"], False)  # sin sol: el cielo queda oscuro (noche)
    dc.set_editor_property("light_color", unreal.Color(150, 175, 255, 255))
    actor_sub.spawn_actor_from_class(unreal.SkyAtmosphere, unreal.Vector(0, 0, 0))
    sk = actor_sub.spawn_actor_from_class(unreal.SkyLight, unreal.Vector(0, 0, 500))
    skc = sk.get_component_by_class(unreal.SkyLightComponent)
    skc.set_editor_property("intensity", 0.4)
    skc.set_mobility(unreal.ComponentMobility.MOVABLE)
    skc.set_editor_property("real_time_capture", False)  # perfil liviano: sin recaptura por cuadro
    fog = actor_sub.spawn_actor_from_class(unreal.ExponentialHeightFog, unreal.Vector(0, 0, 0))
    fc = fog.get_component_by_class(unreal.ExponentialHeightFogComponent)
    setp(fc, ["fog_density"], 0.02)
    # presupuesto: niebla volumetrica apagada; se activa luego segun D1_escena_nocturna.md
    setp(fc, ["enable_volumetric_fog", "volumetric_fog"], False)

    # Post-proceso global: baja la exposicion para que se lea de noche (la autoexposicion levanta todo)
    try:
        ppv = actor_sub.spawn_actor_from_class(unreal.PostProcessVolume, unreal.Vector(0, 0, 0))
        ppv.set_actor_label("PP_Noche")
        ppv.set_editor_property("unbound", True)
        st = ppv.get_editor_property("settings")
        st.set_editor_property("override_auto_exposure_bias", True)
        st.set_editor_property("auto_exposure_bias", -2.0)
        ppv.set_editor_property("settings", st)
    except Exception as e:
        warn("post proceso fallo: %r" % (e,))

    # Zonas de barro (ATerrainModifierVolume) junto a la ruta + visual
    tv_cls = unreal.load_class(None, "/Script/Blackline.TerrainModifierVolume")
    for label, mx, side in (("Barro_A", 500.0, 1), ("Barro_B", 1200.0, -1)):
        loc = unreal.Vector(mx * 100, side * 38 * 100, 50)
        v = actor_sub.spawn_actor_from_class(tv_cls, loc)
        v.set_actor_label(label)
        v.set_folder_path("Terreno")
        zone = v.get_editor_property("zone")
        zone.set_box_extent(unreal.Vector(1500, 1200, 150), True)
        setp(v, ["surface"], unreal.SurfaceKind.MUD)
        setp(v, ["friction_multiplier"], 0.45)
        setp(v, ["requires4_wd", "b_requires4_wd", "requires_4wd"], True)
        spawn_box(actor_sub, cube, mats["mud"], label + "_Visual", (mx, side * 38, 0.015), (30, 24, 0.03), False, "Terreno")

    # Reten a 1000 m (carril derecho de ida)
    cp_cls = unreal.load_class(None, "/Script/Blackline.CheckpointController")
    cp = actor_sub.spawn_actor_from_class(cp_cls, unreal.Vector(1000 * 100, 0, 0))
    cp.set_actor_label("Reten_Puerto")

    # PlayerStart en carril derecho (+Y), mirando +X
    ps = actor_sub.spawn_actor_from_class(unreal.PlayerStart, unreal.Vector(100 * 100, 1.75 * 100, 120))
    ps.set_actor_rotation(unreal.Rotator(0, 0, 0), False)
    ps.set_actor_label("Inicio")

    # Zona de entrega al final de la ruta (cierra el contrato activo al detenerse)
    dz_cls = unreal.load_class(None, "/Script/Blackline.DeliveryZone")
    dz = actor_sub.spawn_actor_from_class(dz_cls, unreal.Vector(1500 * 100, 1.75 * 100, 150))
    dz.set_actor_label("Entrega_Final")

    # Director de clima: sigue el WeatherType del contrato activo
    wd_cls = unreal.load_class(None, "/Script/Blackline.WeatherDirector")
    wd = actor_sub.spawn_actor_from_class(wd_cls, unreal.Vector(0, 0, 200))
    wd.set_actor_label("Clima")

    # NavMesh para las patrullas (AAIController::MoveTo). Caja de 1.7 km x 400 m x 40 m sobre la ruta.
    nav = actor_sub.spawn_actor_from_class(unreal.NavMeshBoundsVolume, unreal.Vector(cx * 100, 0, 0))
    nav.set_actor_label("NavMesh_Ruta")
    nav.set_actor_scale3d(unreal.Vector((L + 200) * 100 / 200.0, 400 * 100 / 200.0, 40 * 100 / 200.0))

    # GameMode de este mapa
    world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    gm_cls = unreal.load_asset(DIR_CORE + "/GM_Blackline").generated_class()
    world.get_world_settings().set_editor_property("default_game_mode", gm_cls)

    lvl.save_current_level()
    log("mapa guardado: " + map_path)


def main():
    log("inicio")
    mats = build_materials()
    actions, imc = build_input()
    van = build_van(actions, imc)
    build_gamemode(van)
    build_map(mats)
    log("FIN OK")


try:
    main()
except Exception as e:
    unreal.log_error(TAG + "FALLO: %r" % (e,))
    import traceback
    unreal.log_error(TAG + traceback.format_exc())
finally:
    unreal.SystemLibrary.quit_editor()
