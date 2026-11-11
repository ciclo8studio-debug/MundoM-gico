# Importa IES_Linterna.ies (TextureLightProfile) y se lo asigna a la linterna del jugador.
# Unreal cerrado: UnrealEditor-Cmd <uproject> -run=pythonscript -script=<este archivo> -unattended -nosplash -nullrhi
import unreal
t = unreal.AssetImportTask()
t.filename = r"C:/Users/pansh/Desktop/MundoMagico_Project/Fuentes_descarga/IES_Linterna.ies"
t.destination_path = "/Game/MundoMagico/Luces"
t.destination_name = "IES_Linterna"
t.automated = True
t.replace_existing = True
t.save = True
unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([t])
ies = unreal.load_asset("/Game/MundoMagico/Luces/IES_Linterna")
unreal.log("IES_OK %s" % (ies.get_path_name() if ies else None))

bp = unreal.load_asset("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter")
sub = unreal.get_engine_subsystem(unreal.SubobjectDataSubsystem)
handles = sub.k2_gather_subobject_data_for_blueprint(bp)
hecho = False
for h in handles:
    data = unreal.SubobjectDataBlueprintFunctionLibrary.get_data(h)
    obj = unreal.SubobjectDataBlueprintFunctionLibrary.get_object(data)
    if obj and obj.get_name().startswith("Linterna") and isinstance(obj, unreal.SpotLightComponent):
        obj.set_editor_property("ies_texture", ies)
        obj.set_editor_property("use_ies_brightness", False)
        hecho = True
        unreal.log("LINTERNA_IES_OK %s" % obj.get_path_name())
if hecho:
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    unreal.EditorAssetLibrary.save_loaded_asset(bp, False)
else:
    unreal.log_warning("LINTERNA_NO_ENCONTRADA")
