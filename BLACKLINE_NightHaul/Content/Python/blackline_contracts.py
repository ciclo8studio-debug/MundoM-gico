"""Blackline: 25 contratos (ficcionalizados, inspirados en casos y rutas de Chile).

Uso fuera del editor (valida y regenera el CSV):
    python blackline_contracts.py            # valida + escribe Content/Data/DT_BlacklineContracts.csv
Uso en el editor (consola Python o -ExecutePythonScript):
    import blackline_contracts as c; c.import_datatable()   # crea /Game/Data/DT_BlacklineContracts

Columnas = propiedades de FBlacklineContractRow. La primera columna ('---') es el nombre de fila.
"""
import csv
import os

# (id, titulo, arco, cargo, cobertura, pago, kg, fragilidad, EM, nivel_doc, clima, terreno, sesgo_lateral, nota)
C = [
    ("C01", "Operación Pudahuel", 1, "Contenedores biométricos", "Equipamiento de control de acceso para centro logístico (Servitec Andes).", 4500, 180, 0.20, 0.85, 1, "ClearNight", "Asphalt", 0.0, "Extracción rápida; usa el inhibidor GPS."),
    ("C02", "Turno Nocturno en Bóveda", 1, "Lingotes de aleación monetaria", "Insumos metálicos para acuñación industrial (Metalúrgica del Maipo).", 6200, 1400, 0.10, 0.20, 1, "LightRain", "Asphalt", 0.0, "Peso alto: frena con anticipación y planea los pórticos."),
    ("C03", "Valija Diplomática", 1, "Servidores encriptados", "Valija de correspondencia consular sellada (Cancillería Austral).", 5800, 250, 0.45, 0.50, 2, "UrbanFog", "Asphalt", 0.0, "Sigilo urbano; caleteras sin pórticos."),
    ("C04", "Interceptación en Ruta 68", 1, "Servidores bancarios", "Reposición de cajeros automáticos (Banco Costanera Sur).", 7000, 600, 0.50, 0.55, 1, "ModerateRain", "DirtBypass", 0.0, "Operativo en ruta: desvío por caminos laterales de tierra."),
    ("C05", "El Efecto Dominó", 1, "Maletines con balizas pulsantes", "Muestras de equipos de medición (Instrumentos Tronador).", 8500, 420, 0.35, 1.00, 2, "Storm", "Asphalt", 0.0, "Refúgiate bajo pasos a nivel y apaga el motor."),

    ("C06", "Minutos Contados", 2, "Cronógrafos de alta gama", "Relojería suiza para tienda de aeropuerto (Horologia Sur).", 6500, 90, 0.30, 0.15, 2, "ColdNight", "Asphalt", 0.0, "Carga ligera; manifiesto nivel 2 en el retén."),
    ("C07", "Lote Fantasma", 2, "Microprocesadores desviados", "Componentes electrónicos para ensambladora (Nexo Silicio SpA).", 7200, 500, 0.40, 0.30, 2, "LightRain", "Asphalt", 0.0, "Inspección de sellos en el retén."),
    ("C08", "Brillo Opaco", 2, "Joyería en doble fondo", "Bisutería importada para feria (Importadora Cordillera).", 8000, 120, 0.15, 0.10, 3, "ClearNight", "Asphalt", 0.0, "Inspección con linterna en cabina; manifiesto nivel 3."),
    ("C09", "Prototipo Cero", 2, "Chasis experimental asimétrico", "Prototipo de vehículo para ensayo de pista (Taller Quimera).", 7800, 1100, 0.40, 0.50, 2, "LightRain", "Asphalt", 0.7, "Peso cargado a un lado: cuida las curvas."),
    ("C10", "Cuentas Saldadas", 2, "Cajas fuertes portátiles", "Mobiliario de seguridad para sucursales (Cofres del Pacífico).", 8900, 950, 0.05, 0.20, 1, "Windy", "DirtRoad", -0.3, "El ruido metálico en baches sube la alerta acústica."),

    ("C11", "Patrimonio Oculto", 3, "Cerámicas precolombinas", "Réplicas decorativas para exposición (Artesanías del Norte).", 9200, 300, 0.95, 0.05, 2, "HumidNight", "Asphalt", 0.0, "Fragilidad extrema: evita golpes y lomos de toro."),
    ("C12", "Reliquias de Madera", 3, "Esculturas antiguas embaladas", "Mobiliario rústico restaurado (Carpintería Los Lagos).", 9800, 480, 0.80, 0.05, 1, "HeavyRain", "DeepMud", 0.0, "Barro profundo: usa 4x4 con suavidad."),
    ("C13", "El Lienzo Enrollado", 3, "Pinturas en tubos presurizados", "Material gráfico para imprenta (Gráfica Nueva Aurora).", 10500, 150, 0.75, 0.10, 3, "DenseFog", "Asphalt", 0.0, "Controles en niebla densa y control térmico."),
    ("C14", "Platería Ancestral", 3, "Cofres de platería histórica", "Utensilios de hotelería de lujo (Hostal Gran Valle).", 10200, 650, 0.50, 0.90, 2, "ClearNight", "Asphalt", 0.0, "Alta firma EM en arcos de escaneo."),
    ("C15", "Fósil del Desierto", 3, "Bloque paleontológico en yeso", "Material de construcción en bloque (Yesería Atacama).", 12000, 1350, 0.90, 0.15, 2, "Windy", "Gravel", 0.2, "Peso alto y fragilidad muy alta."),

    ("C16", "Oro Rojo", 4, "Planchas de aleación conductora", "Chatarra industrial para fundición (Reciclajes Biobío).", 11000, 1850, 0.10, 0.40, 2, "ClearNight", "Asphalt", 0.0, "Sobrecarga: la distancia de frenado es larga."),
    ("C17", "Contenedor 404", 4, "Válvulas hidráulicas pesadas", "Repuestos agrícolas (Maquinarias del Valle Central).", 11500, 1600, 0.25, 0.20, 1, "CoastalFog", "DirtBypass", 0.0, "Evita las básculas por caminos agrícolas."),
    ("C18", "Bajando la Cuesta", 4, "Baterías industriales de litio", "Acumuladores para respaldo eléctrico (Energía Austral).", 12400, 1500, 0.60, 0.65, 2, "HeavyRain", "WetSlope", 0.0, "Bajada pronunciada bajo lluvia sin bloquear las ruedas."),
    ("C19", "Químicos sin Etiqueta", 4, "Bidones de reactivos mineros", "Insumos de laboratorio (Laboratorios Sierra Norte).", 13000, 1200, 0.65, 0.30, 2, "HumidNight", "Gravel", 0.0, "Fuga visible si baja la integridad: alerta a patrullas."),
    ("C20", "Carga Fantasma en Muelle", 4, "Generadores navales robados", "Equipos de respaldo para astillero (Astilleros del Sur).", 13500, 1450, 0.30, 0.45, 1, "PortFog", "Dockyard", 0.0, "Sal con luces apagadas esquivando focos."),

    ("C21", "Paso Fantasma", 5, "Suministros médicos experimentales", "Insumos de salud para posta rural (Clínica Cumbres).", 14500, 550, 0.55, 0.60, 3, "Freezing", "MountainRoad", 0.0, "Aduana con escáner e inspección en ventanilla."),
    ("C22", "Ventisca en la Altura", 5, "Componentes telecom cifrados", "Repetidores para refugio de alta montaña (Telecom Altiplano).", 15200, 800, 0.40, 0.70, 2, "Blizzard", "IceSnow", 0.0, "Hielo y nieve: 4x4 con bloqueo de diferencial."),
    ("C23", "El Túnel Viejo", 5, "Equipo táctico clasificado", "Material de rescate minero (Seguridad Subterránea Ltda.).", 16000, 900, 0.35, 0.50, 2, "ModerateRain", "OldTunnel", 0.0, "Túnel estrecho: sin margen para el roce."),
    ("C24", "Contrabando Inverso", 5, "Muestras biológicas criogénicas", "Muestras de investigación agrícola (Instituto Vitis).", 17500, 400, 0.70, 0.75, 3, "DenseFog", "MountainRoad", 0.0, "Doble retén consecutivo y disipación térmica."),
    ("C25", "Último Envío", 5, "Núcleo energético inestable", "Reactor de investigación en traslado (Consorcio Núcleo Austral).", 25000, 1550, 0.85, 0.95, 3, "Storm", "MixedOffroad", 0.0, "Clímax: aduana nivel 3 y persecución en tormenta."),
]

# Voz de cada lugarteniente de La Cofradia. Pancito (el perro de Enzo) es la presion, nunca se le muestra herido.
BRIEFINGS = {
    "C01": "Contenedores a Pudahuel antes del amanecer. Inhibidor GPS encendido. Pancito come a su hora si tu llegas a la tuya.",
    "C02": "Aleacion pesada y portico de peaje. Frena con tiempo. Yo mido todo, Salinas, incluido a Pancito.",
    "C03": "Valija por calles chicas, sin porticos. Cero errores: yo mido cada error contra tu perro.",
    "C04": "Hay operativo en la Ruta 68. Desviate por tierra, no por miedo. Pancito sigue esperando.",
    "C05": "Las balizas pulsan: escondete bajo un paso a nivel y apaga el motor. Si te ven, el que paga es el.",
    "C06": "Relojes finos, reten fino. Sonrie, carino. Pancito tambien sabe esperar bonito.",
    "C07": "Microprocesadores en cajas de manzanas, querido. Que los sellos aguanten la mirada del guardia.",
    "C08": "Joyas en doble fondo y un guardia con linterna. Tu mejor cara, Enzo: la que le pondrias a Pancito.",
    "C09": "Un prototipo descuadrado: peso a un lado, curvas lentas. Si lo vuelcas, no hay segunda prueba.",
    "C10": "Cajas fuertes que suenan en cada bache. Mejor tierra suave que un escandalo, ¿no crees?",
    "C11": "Ceramica milenaria. Cada golpe es un pedazo de historia y de tu amigo. Despacio en los lomos de toro.",
    "C12": "Madera tallada bajo la lluvia. El barro no perdona y la talla tampoco. Suave con el 4x4.",
    "C13": "Lienzos enrollados en niebla espesa. Un solo rasguño y lo lamentaras. El ladra por ti.",
    "C14": "Plateria que brilla en los escaneres. Preparate para explicar lo que no se explica.",
    "C15": "Un fosil pesadisimo en yeso. Pierde un trozo y pierdes mas que el pago.",
    "C16": "Planchas de casi dos toneladas. Frenar toma distancia: usala. Tienes hasta el alba.",
    "C17": "Valvulas por caminos agricolas. Evita las basculas. Plazo corto, Salinas.",
    "C18": "Baterias de litio por la cuesta mojada. No bloquees ruedas. Si no llegas, el espera mas.",
    "C19": "Bidones sin etiqueta. Si gotean, te siguen. Que no goteen.",
    "C20": "Generadores en el muelle. Luces apagadas y paciencia. Tienes una hora.",
    "C21": "El paso, con escaner y ventanilla. Papeles sin faltas. Estas mas cerca de Pancito de lo que crees.",
    "C22": "Telecom cifrada, hielo y altura. 4x4 con diferencial bloqueado. Ya casi, Enzo.",
    "C23": "El tunel viejo, sin margen. Ni un roce. Pancito te oye desde algun lado.",
    "C24": "Doble reten y muestras frias. Manten el aire y los nervios frios. Falta poco.",
    "C25": "El ultimo envio: nucleo inestable, aduana nivel 3 y tormenta. Entregalo y Pancito vuelve a la cabina.",
}

BROKERS = {1: "El Ingeniero", 2: "La Anticuaria", 3: "El Curador", 4: "El Capataz", 5: "El Arriero"}
WEATHER = {"ClearNight", "ColdNight", "HumidNight", "LightRain", "ModerateRain", "HeavyRain", "Storm", "Windy",
           "UrbanFog", "CoastalFog", "PortFog", "DenseFog", "Freezing", "Blizzard"}
TERRAIN = {"Asphalt", "DirtBypass", "DirtRoad", "DeepMud", "Gravel", "WetSlope", "Dockyard", "MountainRoad",
           "IceSnow", "OldTunnel", "MixedOffroad"}
HEADER = ["---", "ContractID", "Title", "Arc", "BrokerName", "CargoDisplayName", "CoverStoryManifest", "Payout",
          "CargoWeightKg", "Fragility", "EMSignature", "RequiredManifestLevel", "WeatherType", "TerrainType",
          "LateralBias", "Briefing", "DesignNote"]
CSV_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "Data", "DT_BlacklineContracts.csv")


def validate():
    errs = []
    if len(C) != 25:
        errs.append("se esperaban 25 contratos, hay %d" % len(C))
    ids = [c[0] for c in C]
    for cid in ids:
        if not BRIEFINGS.get(cid):
            errs.append('%s sin briefing' % cid)
    if len(set(ids)) != len(ids):
        errs.append("ContractID duplicado")
    for (cid, title, arc, cargo, cover, pay, kg, fr, em, lvl, wea, ter, bias, note) in C:
        if not 1 <= arc <= 5:
            errs.append("%s arco fuera de rango" % cid)
        if not 0 <= fr <= 1 or not 0 <= em <= 1:
            errs.append("%s fragilidad/EM fuera de 0..1" % cid)
        if lvl not in (1, 2, 3):
            errs.append("%s nivel de documento invalido" % cid)
        if wea not in WEATHER:
            errs.append("%s clima desconocido: %s" % (cid, wea))
        if ter not in TERRAIN:
            errs.append("%s terreno desconocido: %s" % (cid, ter))
        if pay <= 0 or kg <= 0:
            errs.append("%s pago/peso invalido" % cid)
        if not -1 <= bias <= 1:
            errs.append("%s sesgo fuera de -1..1" % cid)
    return errs


def write_csv(path=CSV_PATH):
    path = os.path.normpath(path)
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w", newline="", encoding="utf-8") as f:
        w = csv.writer(f, quoting=csv.QUOTE_MINIMAL)
        w.writerow(HEADER)
        for (cid, title, arc, cargo, cover, pay, kg, fr, em, lvl, wea, ter, bias, note) in C:
            w.writerow([cid, cid, title, arc, BROKERS[arc], cargo, cover, pay, kg, "%.2f" % fr, "%.2f" % em,
                        lvl, wea, ter, "%.2f" % bias, BRIEFINGS[cid], note])
    return path


def import_datatable(asset_dir="/Game/Data", name="DT_BlacklineContracts"):
    """Solo dentro del editor: crea el DataTable desde el CSV usando FBlacklineContractRow."""
    import unreal
    path = write_csv()
    struct = unreal.load_object(None, "/Script/Blackline.BlacklineContractRow")
    task = unreal.AssetImportTask()
    task.set_editor_property("filename", path)
    task.set_editor_property("destination_path", asset_dir)
    task.set_editor_property("destination_name", name)
    task.set_editor_property("replace_existing", True)
    task.set_editor_property("automated", True)
    task.set_editor_property("save", True)
    factory = unreal.CSVImportFactory()
    settings = factory.get_editor_property("automated_import_settings")
    settings.set_editor_property("import_row_struct", struct)
    task.set_editor_property("factory", factory)
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
    unreal.log("BLK: DataTable importado en %s/%s" % (asset_dir, name))


if __name__ == "__main__":
    e = validate()
    if e:
        raise SystemExit("ERRORES:\n" + "\n".join(e))
    print("OK: %d contratos validados -> %s" % (len(C), write_csv()))
