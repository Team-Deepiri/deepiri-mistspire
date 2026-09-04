# Fix duplicate LevelScriptActors that block cook/package (Ensure in LevelScriptActor.cpp).
# Emptying the Level Blueprint graph does NOT remove the duplicate actors — run this.
#
# With AncientWorld loaded in Mistspire Editor:
#   Tools → Execute Python Script → pick this file
# Or Output Log: py "e:/repo/deepiri-mistspire/tools/fix_duplicate_level_script_actors.py"
#
# Edits Valley AncientWorld.umap via the content junction.

import unreal


def _collect_lsas():
    found = []
    seen = set()

    def add(actor):
        if not actor:
            return
        key = actor.get_path_name()
        if key in seen:
            return
        seen.add(key)
        found.append(actor)

    world = None
    try:
        world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    except Exception:
        pass
    if world is None:
        try:
            world = unreal.EditorLevelLibrary.get_editor_world()
        except Exception:
            world = None

    if world:
        try:
            for a in unreal.GameplayStatics.get_all_actors_of_class(world, unreal.LevelScriptActor):
                add(a)
        except Exception as ex:
            unreal.log_warning("GameplayStatics LSA query failed: {}".format(ex))

    try:
        subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
        if subsystem:
            for a in subsystem.get_all_level_actors():
                if isinstance(a, unreal.LevelScriptActor):
                    add(a)
    except Exception as ex:
        unreal.log_warning("EditorActorSubsystem LSA query failed: {}".format(ex))

    for obj in unreal.ObjectIterator(unreal.LevelScriptActor):
        add(obj)

    # Prefer AncientWorld instances when mixed with other levels.
    ancient = [a for a in found if "AncientWorld" in a.get_path_name()]
    return ancient if len(ancient) >= 2 else found


def _reveal(actor):
    for prop, value in (
        ("listed_in_scene_outliner", True),
        ("b_listed_in_scene_outliner", True),
        ("hidden", False),
        ("b_hidden_ed", False),
        ("b_hidden_ed_level_instance", False),
        ("b_editable", True),
        ("b_is_editor_only_actor", False),
    ):
        try:
            actor.set_editor_property(prop, value)
        except Exception:
            pass


def _destroy(actor):
    name = actor.get_path_name()
    _reveal(actor)
    subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    if subsystem:
        try:
            if subsystem.destroy_actor(actor):
                return True
        except Exception as ex:
            unreal.log_warning("EditorActorSubsystem.destroy_actor: {}".format(ex))
    try:
        if unreal.EditorLevelLibrary.destroy_actor(actor):
            return True
    except Exception as ex:
        unreal.log_warning("EditorLevelLibrary.destroy_actor: {}".format(ex))
    try:
        # Last resort: detach from outer so reload cannot resurrect a sibling pair as easily.
        actor.rename("AncientWorld_C_DUPLICATE_TO_DELETE", unreal.RenameFlags.FORCE_NO_RESET_LOADERS)
        unreal.log_warning("Renamed {} for manual Outliner delete".format(name))
    except Exception as ex:
        unreal.log_error("rename failed for {}: {}".format(name, ex))
    return False


def fix_duplicate_level_script_actors(keep_suffix="_0"):
    lsas = _collect_lsas()
    unreal.log("LevelScriptActors found: {}".format(len(lsas)))
    for lsa in lsas:
        _reveal(lsa)
        unreal.log("  {} ({})".format(lsa.get_path_name(), lsa.get_class().get_name()))

    if len(lsas) < 2:
        unreal.log_warning(
            "Fewer than 2 LevelScriptActors visible to Python. "
            "Clear Message Log, reload the map, and check whether the Ensure still fires. "
            "If it does: Outliner search AncientWorld_C (enable Show Hidden), delete *_1, save."
        )
        return False

    keep = None
    for lsa in lsas:
        if lsa.get_name().endswith(keep_suffix):
            keep = lsa
            break
    if keep is None:
        keep = sorted(lsas, key=lambda a: a.get_name())[0]

    destroyed = 0
    for lsa in lsas:
        if lsa == keep:
            continue
        name = lsa.get_path_name()
        if _destroy(lsa):
            destroyed += 1
            unreal.log("Destroyed duplicate LevelScriptActor: {}".format(name))
        else:
            unreal.log_error("Could not destroy {}".format(name))

    try:
        unreal.EditorLevelLibrary.save_current_level()
    except Exception:
        pass

    if destroyed:
        unreal.log(
            "Saved after removing {} duplicate(s). Keep: {}. "
            "File → Recent → reopen AncientWorld (or restart editor), Clear Message Log, confirm Ensure is gone.".format(
                destroyed, keep.get_path_name()
            )
        )
        return True

    unreal.log_warning(
        "No duplicates destroyed. In Outliner, search AncientWorld_C — both should now be listed; "
        "delete AncientWorld_C_1, save map, reload."
    )
    return False


if __name__ == "__main__":
    fix_duplicate_level_script_actors()
