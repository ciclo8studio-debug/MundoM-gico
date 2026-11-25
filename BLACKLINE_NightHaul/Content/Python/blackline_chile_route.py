"""Blackline: waypoints de la ruta de Chile comprimida (ciudades reales, coordenadas aproximadas).

Fuera del editor:   python blackline_chile_route.py   -> valida y regenera Content/Data/DT_ChileWaypoints.csv
Dentro del editor:  import blackline_chile_route as c; c.import_datatable()

Zonas: 1 Norte (Arica-Copiapo), 2 Centro (Vallenar-Rancagua, con Valparaiso y Santiago), 3 Sur (Talca-Puerto Montt),
4 Cordillera (Santiago-Paso Los Libertadores). Compresion 1:100 (la zona 4, 1:25). Alturas reales (m) aproximadas.
"""
import csv
import math
import os

# (fila, nombre, zona, lat, lon, elevacion_m, bioma)
W = [
    ("Z1_01", "Arica", 1, -18.4783, -70.3126, 10, "DesertCoast"),
    ("Z1_02", "Pozo Almonte", 1, -20.2570, -69.7860, 1000, "DesertInterior"),
    ("Z1_03", "Quillagua", 1, -21.6500, -69.5400, 800, "DesertInterior"),
    ("Z1_04", "Baquedano", 1, -23.3300, -69.8400, 1100, "DesertInterior"),
    ("Z1_05", "Antofagasta", 1, -23.6500, -70.4000, 20, "DesertCoast"),
    ("Z1_06", "Chañaral", 1, -26.3480, -70.6220, 10, "DesertCoast"),
    ("Z1_07", "Copiapó", 1, -27.3668, -70.3323, 400, "Semiarid"),

    ("Z2_01", "Vallenar", 2, -28.5750, -70.7590, 400, "Semiarid"),
    ("Z2_02", "La Serena", 2, -29.9045, -71.2489, 25, "Semiarid"),
    ("Z2_03", "Ovalle", 2, -30.6000, -71.2000, 220, "Semiarid"),
    ("Z2_04", "Los Vilos", 2, -31.9100, -71.5100, 10, "Semiarid"),
    ("Z2_05", "Valparaíso", 2, -33.0472, -71.6127, 20, "CentralValley"),
    ("Z2_06", "Santiago", 2, -33.4489, -70.6693, 520, "CentralValley"),
    ("Z2_07", "Rancagua", 2, -34.1708, -70.7444, 500, "CentralValley"),

    ("Z3_01", "Talca", 3, -35.4264, -71.6554, 100, "CentralValley"),
    ("Z3_02", "Chillán", 3, -36.6063, -72.1034, 120, "CentralValley"),
    ("Z3_03", "Los Ángeles", 3, -37.4697, -72.3537, 140, "HumidSouth"),
    ("Z3_04", "Temuco", 3, -38.7359, -72.5904, 100, "HumidSouth"),
    ("Z3_05", "Valdivia", 3, -39.8196, -73.2452, 20, "HumidSouth"),
    ("Z3_06", "Osorno", 3, -40.5742, -73.1348, 50, "HumidSouth"),
    ("Z3_07", "Puerto Montt", 3, -41.4693, -72.9424, 10, "HumidSouth"),

    ("Z4_01", "Santiago", 4, -33.4489, -70.6693, 520, "CentralValley"),
    ("Z4_02", "Los Andes", 4, -32.8337, -70.5983, 820, "CentralValley"),
    ("Z4_03", "Río Blanco", 4, -32.9000, -70.3200, 1400, "Cordillera"),
    ("Z4_04", "Portillo", 4, -32.8350, -70.1280, 2880, "Cordillera"),
    ("Z4_05", "Paso Cristo Redentor", 4, -32.8210, -70.0860, 3150, "Cordillera"),
]
COMPRESSION = {1: 100, 2: 100, 3: 100, 4: 25}
BIOMES = {"DesertCoast", "DesertInterior", "Semiarid", "CentralValley", "HumidSouth", "Cordillera"}
HEADER = ["---", "Name", "ZoneId", "LatDeg", "LonDeg", "ElevationM", "Biome"]
CSV_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "Data", "DT_ChileWaypoints.csv")


def zone_lengths_km():
    """Largo real y comprimido de cada zona (suma de tramos entre waypoints)."""
    out = {}
    for z in COMPRESSION:
        pts = [w for w in W if w[2] == z]
        real = 0.0
        for a, b in zip(pts, pts[1:]):
            dy = (b[3] - a[3]) * 111.32
            dx = (b[4] - a[4]) * 111.32 * math.cos(math.radians((a[3] + b[3]) / 2))
            real += math.hypot(dx, dy)
        out[z] = (real, real / COMPRESSION[z])
    return out


def validate():
    errs, ids = [], [w[0] for w in W]
    if len(set(ids)) != len(ids):
        errs.append("fila duplicada")
    for (rid, name, z, lat, lon, elev, bio) in W:
        if z not in COMPRESSION or not rid.startswith("Z%d_" % z):
            errs.append("%s: zona incoherente" % rid)
        if not (-56 <= lat <= -17 and -76 <= lon <= -66):
            errs.append("%s: coordenadas fuera de Chile" % rid)
        if bio not in BIOMES:
            errs.append("%s: bioma desconocido %s" % (rid, bio))
        if not 0 <= elev <= 6000:
            errs.append("%s: altura imposible" % rid)
    for z in COMPRESSION:
        if len([w for w in W if w[2] == z]) < 2:
            errs.append("zona %d con menos de 2 puntos" % z)
    return errs


def write_csv(path=CSV_PATH):
    path = os.path.normpath(path)
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w", newline="", encoding="utf-8") as f:
        w = csv.writer(f)
        w.writerow(HEADER)
        for (rid, name, z, lat, lon, elev, bio) in W:
            w.writerow([rid, name, z, lat, lon, elev, bio])
    return path


def import_datatable(asset_dir="/Game/Data", name="DT_ChileWaypoints"):
    import unreal
    path = write_csv()
    struct = unreal.load_object(None, "/Script/Blackline.ChileWaypointRow")
    task = unreal.AssetImportTask()
    task.set_editor_property("filename", path)
    task.set_editor_property("destination_path", asset_dir)
    task.set_editor_property("destination_name", name)
    task.set_editor_property("replace_existing", True)
    task.set_editor_property("automated", True)
    task.set_editor_property("save", True)
    factory = unreal.CSVImportFactory()
    factory.get_editor_property("automated_import_settings").set_editor_property("import_row_struct", struct)
    task.set_editor_property("factory", factory)
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
    unreal.log("BLK: DataTable de waypoints importado en %s/%s" % (asset_dir, name))


if __name__ == "__main__":
    e = validate()
    if e:
        raise SystemExit("ERRORES:\n" + "\n".join(e))
    print("OK: %d waypoints -> %s" % (len(W), write_csv()))
    for z, (real, comp) in sorted(zone_lengths_km().items()):
        print("  zona %d: %.0f km reales -> %.1f km comprimidos (1:%d)" % (z, real, comp, COMPRESSION[z]))
