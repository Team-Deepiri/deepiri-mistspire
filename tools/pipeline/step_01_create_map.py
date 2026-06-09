"""
Mistspire Pipeline Step 1: Create Main_WP World Partition map.

Run in UE Editor: Tools > Execute Python Script
Or: py "tools/pipeline/step_01_create_map.py"
"""
import unreal

def create_main_wp():
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    editor_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)

    # Check if map already exists
    if unreal.EditorAssetLibrary.does_asset_exist("/Game/Maps/Main_WP"):
        unreal.log_warning("Main_WP already exists. Opening...")
        editor_subsystem.load_level("/Game/Maps/Main_WP")
        return

    # Create new level with World Partition
    new_map = "/Game/Maps/Main_WP"
    world_partition = unreal.WorldPartition

    # Use Empty Open World template
    asset_tools.create_asset(new_map, "/Game/Maps", unreal.World, None)

    # Load it
    editor_subsystem.new_level_from_template("/Game/Maps/Main_WP", "/Engine/WorldTemplates/OpenWorld/OpenWorld")
    world = editor_subsystem.get_editor_world()

    # Configure World Settings
    world_settings = world.get_world_settings()
    world_settings.set_editor_property("enable_world_partition", True)

    # Set game mode
    world_settings.default_game_mode = unreal.load_asset("/Game/Mistspire/MistspireGameMode.MistspireGameMode_C")

    # Set World Partition cell size
    wp_settings = world.get_subsystem(unreal.WorldPartition)
    if wp_settings:
        wp_settings.set_editor_property("cell_size", 25600)

    # Create data layers
    dl_editor_subsystem = unreal.get_editor_subsystem(unreal.DataLayerEditorSubsystem)
    layers = [
        "DL_Landmarks_Authored",
        "DL_PCG_Biome_Mist",
        "DL_PCG_Biome_Forest",
        "DL_PCG_Biome_Arid",
        "DL_PCG_Biome_Ember",
        "DL_PCG_Biome_Crystal",
        "DL_PCG_Biome_Void",
        "DL_PCG_Biome_Tundra",
        "DL_PCG_Biome_Aether",
        "DL_PCG_Biome_Sanctum",
        "DL_PCG_Biome_Pinnacle",
        "DL_Weather_Dynamic",
    ]

    for layer_name in layers:
        dl_editor_subsystem.create_data_layer(layer_name)

    # Place player start
    player_start = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.PlayerStart, unreal.Vector(0, 0, 0), unreal.Rotator(0, 0, 0))

    # Save
    editor_subsystem.save_current_level()
    unreal.log("Main_WP created successfully!")

if __name__ == "__main__":
    create_main_wp()
