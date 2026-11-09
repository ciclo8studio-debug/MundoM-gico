import unreal

OUT = r"C:/Users/pansh/Desktop/MundoMagico_Project/Isidora3D/SKM_Quinn_Simple.fbx"
mesh = unreal.load_asset("/Game/Characters/Mannequins/Meshes/SKM_Quinn_Simple")
task = unreal.AssetExportTask()
task.object = mesh
task.filename = OUT
task.automated = True
task.prompt = False
task.replace_identical = True
opts = unreal.FbxExportOption()
opts.collision = False
opts.level_of_detail = False
task.options = opts
ok = unreal.Exporter.run_asset_export_task(task)
unreal.log("EXPORT_QUINN_RESULT %s" % ok)
