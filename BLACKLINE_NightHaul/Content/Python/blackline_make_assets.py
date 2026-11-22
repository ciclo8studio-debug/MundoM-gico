"""Blackline: crea DA_Cargo_* y DA_Manifest_* de ejemplo (editor UE Python).

Uso (consola Python del editor):
    import blackline_make_assets as b; b.run()
    b.run(overwrite=True)   # regenera los existentes

Requiere el modulo C++ Blackline compilado (clases CargoItemDataAsset y
ManifestDataAsset). Los nombres de clase/enum en Python siguen la convencion
UE (sin prefijo U/E/F). // VERIFY-5.8: confirmar nombres con dir(unreal).
"""
import unreal

CARGO_DIR = "/Game/Blackline/Data/Cargo"
MANIFEST_DIR = "/Game/Blackline/Data/Manifests"

# Valores de ejemplo. Campos segun CONTRATO (UCargoItemDataAsset).
# (nombre_asset, ItemId, DisplayName, ECargoClass, MassKg, Fragility01,
#  ContrabandSignature01, bRequiresShielding, BoxExtentCm(x,y,z), ValueCredits)
CARGOS = [
    ("DA_Cargo_Standard", "std_crate", "Caja estandar", "STANDARD", 120.0, 0.1, 0.0, False, (50, 50, 40), 200.0),
    ("DA_Cargo_Fragile", "fragile_glass", "Instrumental de vidrio", "FRAGILE", 60.0, 0.9, 0.0, False, (40, 40, 40), 450.0),
    ("DA_Cargo_Volatile", "volatile_drum", "Tambor volatil", "VOLATILE", 180.0, 0.6, 0.5, False, (35, 35, 55), 700.0),
    ("DA_Cargo_Cryogenic", "cryo_unit", "Unidad criogenica", "CRYOGENIC", 150.0, 0.5, 0.3, True, (45, 45, 60), 900.0),
    ("DA_Cargo_Relic", "relic_case", "Reliquia sellada", "RELIC", 80.0, 0.7, 0.8, True, (40, 30, 30), 1500.0),
    ("DA_Cargo_AIServer", "ai_server", "Servidor de IA", "AI_SERVER", 200.0, 0.8, 0.9, True, (60, 50, 70), 3000.0),
]

# (nombre_asset, ManifestId, CompanyName, RouteCode, Forgery01, Notes, [(cargo_asset, cantidad)])
MANIFESTS = [
    ("DA_Manifest_Clean", "mf_clean", "Transportes Norte S.A.", "N-101", 1.0,
     "Manifiesto legitimo.", [("DA_Cargo_Standard", 4), ("DA_Cargo_Fragile", 1)]),
    ("DA_Manifest_Forged", "mf_forged", "Logistica Aurora", "A-202", 0.6,
     "Declara carga estandar; lleva reliquia.", [("DA_Cargo_Standard", 3)]),
    ("DA_Manifest_Heavy", "mf_heavy", "Cryo Freight", "C-303", 0.85,
     "Cadena de frio declarada.", [("DA_Cargo_Cryogenic", 2), ("DA_Cargo_Volatile", 1)]),
]


def _get_or_create(name, folder, cls, overwrite):
    path = f"{folder}/{name}"
    if unreal.EditorAssetLibrary.does_asset_exist(path):
        if not overwrite:
            unreal.log(f"[Blackline] existe, se omite: {path}")
            return unreal.EditorAssetLibrary.load_asset(path)
        unreal.EditorAssetLibrary.delete_asset(path)
    factory = unreal.DataAssetFactory()
    factory.set_editor_property("data_asset_class", cls)  # VERIFY-5.8
    tools = unreal.AssetToolsHelpers.get_asset_tools()
    asset = tools.create_asset(name, folder, cls, factory)
    if asset is None:
        unreal.log_error(f"[Blackline] no se pudo crear {path}")
    return asset


def _make_cargo(row, overwrite):
    name, item_id, disp, cls_name, mass, frag, sig, shield, ext, value = row
    a = _get_or_create(name, CARGO_DIR, unreal.CargoItemDataAsset, overwrite)
    if a is None:
        return None
    a.set_editor_property("item_id", item_id)
    a.set_editor_property("display_name", unreal.Text(disp))
    a.set_editor_property("cargo_class", getattr(unreal.CargoClass, cls_name))
    a.set_editor_property("mass_kg", mass)
    a.set_editor_property("fragility01", frag)
    a.set_editor_property("contraband_signature01", sig)
    a.set_editor_property("b_requires_shielding", shield)
    a.set_editor_property("box_extent_cm", unreal.Vector(*ext))
    a.set_editor_property("value_credits", value)
    return a


def _make_manifest(row, cargo_by_name, overwrite):
    name, mid, company, route, forgery, notes, lines = row
    a = _get_or_create(name, MANIFEST_DIR, unreal.ManifestDataAsset, overwrite)
    if a is None:
        return None
    a.set_editor_property("manifest_id", mid)
    a.set_editor_property("company_name", unreal.Text(company))
    a.set_editor_property("route_code", unreal.Text(route))
    a.set_editor_property("forgery_quality01", forgery)
    a.set_editor_property("notes", unreal.Text(notes))
    declared = []
    for cargo_name, qty in lines:
        line = unreal.ManifestLine()
        line.set_editor_property("item", cargo_by_name[cargo_name])
        line.set_editor_property("quantity", qty)
        declared.append(line)
    a.set_editor_property("declared_cargo", declared)
    return a


def run(overwrite=False):
    for d in (CARGO_DIR, MANIFEST_DIR):
        unreal.EditorAssetLibrary.make_directory(d)
    cargo_by_name = {}
    with unreal.ScopedSlowTask(len(CARGOS) + len(MANIFESTS), "Blackline: creando assets") as task:
        task.make_dialog(True)
        for row in CARGOS:
            task.enter_progress_frame(1, row[0])
            a = _make_cargo(row, overwrite)
            if a:
                cargo_by_name[row[0]] = a
        for row in MANIFESTS:
            task.enter_progress_frame(1, row[0])
            missing = [c for c, _ in row[6] if c not in cargo_by_name]
            if missing:
                unreal.log_error(f"[Blackline] {row[0]}: faltan cargos {missing}")
                continue
            _make_manifest(row, cargo_by_name, overwrite)
    unreal.EditorAssetLibrary.save_directory("/Game/Blackline/Data", only_if_is_dirty=True, recursive=True)
    unreal.log(f"[Blackline] listo: {len(CARGOS)} cargos, {len(MANIFESTS)} manifiestos")


if __name__ == "__main__":
    run()
