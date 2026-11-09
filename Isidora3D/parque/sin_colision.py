import unreal

F = "/Game/MundoMagico/Parque/SM_"
N = ['Ent_Hierro', 'Ent_Letras', 'Ent_NeonLetras', 'Ent_NeonBarra', 'Ent_Bombillas', 'Ent_Reja', 'Ent_Porton', 'Bol_Metal', 'Bol_Panel', 'Bol_Bombillas',
     'Gui_Poste', 'Gui_Cable', 'Gui_Bombillas', 'Kio_ToldoRojo', 'Kio_ToldoCrema', 'Kio_Letrero', 'Kio_Bombillas', 'Auto_Antena', 'Auto_Foco', 'Auto_Asiento', 'Auto_Goma',
     'PistaA_NeonRosa', 'PistaA_NeonCian', 'PistaA_Bombillas', 'PistaB_NeonRosa', 'PistaB_NeonCian', 'PistaB_Bombillas', 'Carr_Neon', 'Carr_Bombillas', 'Noria_Neon',
     'Cast_Neon', 'Cast_Ventanas', 'Tea_Bombillas', 'Tea_Neon', 'Montana', 'Montana_Carro', 'Tren_Riel', 'Tren_Durmientes', 'Tren_Ruedas']
lib = unreal.EditorStaticMeshLibrary
for n in N:
    m = unreal.load_asset(F + n)
    lib.remove_collisions(m)
    unreal.EditorAssetLibrary.save_loaded_asset(m, False)
unreal.log("SINCOL_FIN")
