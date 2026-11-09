import unreal

for k in "ABCI":
    p = "/Game/MundoMagico/Arboles/SM_Arbol_" + k
    if unreal.EditorAssetLibrary.does_asset_exist(p):
        unreal.log("BORRA %s %s" % (k, unreal.EditorAssetLibrary.delete_asset(p)))
unreal.log("BORRA_FIN")
