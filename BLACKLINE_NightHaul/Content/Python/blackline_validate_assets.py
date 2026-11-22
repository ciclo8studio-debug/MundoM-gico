"""Blackline: valida DA_Cargo_* y DA_Manifest_* (editor UE Python).

Uso: import blackline_validate_assets as v; v.run()  -> devuelve lista de errores.
Tambien lo llama el Editor Utility descrito en docs/15_herramientas.md.
// VERIFY-5.8: nombres de propiedades Python segun convencion snake_case.
"""
import unreal

ROOT = "/Game/Blackline/Data"


def _validate_cargo(a, path, errs, ids):
    iid = str(a.get_editor_property("item_id"))
    if iid in ("", "None"):
        errs.append(f"{path}: ItemId vacio")
    elif iid in ids:
        errs.append(f"{path}: ItemId duplicado '{iid}' (ya en {ids[iid]})")
    else:
        ids[iid] = path
    if a.get_editor_property("mass_kg") <= 0:
        errs.append(f"{path}: MassKg debe ser > 0")
    for prop in ("fragility01", "contraband_signature01"):
        if not 0.0 <= a.get_editor_property(prop) <= 1.0:
            errs.append(f"{path}: {prop} fuera de [0,1]")
    e = a.get_editor_property("box_extent_cm")
    if min(e.x, e.y, e.z) <= 0:
        errs.append(f"{path}: BoxExtentCm debe ser > 0 en los 3 ejes")
    if a.get_editor_property("value_credits") < 0:
        errs.append(f"{path}: ValueCredits negativo")
    if a.get_editor_property("display_name").to_string() == "":
        errs.append(f"{path}: DisplayName vacio")


def _validate_manifest(a, path, errs, ids):
    mid = str(a.get_editor_property("manifest_id"))
    if mid in ("", "None"):
        errs.append(f"{path}: ManifestId vacio")
    elif mid in ids:
        errs.append(f"{path}: ManifestId duplicado '{mid}'")
    else:
        ids[mid] = path
    if not 0.0 <= a.get_editor_property("forgery_quality01") <= 1.0:
        errs.append(f"{path}: ForgeryQuality01 fuera de [0,1]")
    lines = a.get_editor_property("declared_cargo")
    if len(lines) == 0:
        errs.append(f"{path}: DeclaredCargo vacio")
    for i, ln in enumerate(lines):
        if ln.get_editor_property("item") is None:
            errs.append(f"{path}: linea {i} sin Item")
        if ln.get_editor_property("quantity") < 1:
            errs.append(f"{path}: linea {i} con Quantity < 1")


def run(root=ROOT):
    errs, cargo_ids, man_ids = [], {}, {}
    n = 0
    for p in unreal.EditorAssetLibrary.list_assets(root, recursive=True, include_folder=False):
        a = unreal.EditorAssetLibrary.load_asset(p)
        if isinstance(a, unreal.CargoItemDataAsset):
            _validate_cargo(a, p, errs, cargo_ids)
            n += 1
        elif isinstance(a, unreal.ManifestDataAsset):
            _validate_manifest(a, p, errs, man_ids)
            n += 1
    for e in errs:
        unreal.log_error(f"[Blackline] {e}")
    unreal.log(f"[Blackline] validados {n} assets, {len(errs)} errores")
    return errs


if __name__ == "__main__":
    run()
