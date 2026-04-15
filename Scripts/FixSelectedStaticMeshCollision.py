import unreal


def _set_complex_collision(static_mesh):
    body_setup = static_mesh.get_editor_property("body_setup")
    if not body_setup:
        unreal.log_warning("Skipping {0}: no BodySetup".format(static_mesh.get_name()))
        return False

    try:
        unreal.EditorStaticMeshLibrary.remove_collisions(static_mesh)
    except Exception as error:
        unreal.log_warning("Could not remove simple collision for {0}: {1}".format(static_mesh.get_name(), error))

    body_setup.set_editor_property(
        "collision_trace_flag",
        unreal.CollisionTraceFlag.CTF_USE_COMPLEX_AS_SIMPLE,
    )
    return True


def _save_mesh(static_mesh):
    try:
        unreal.EditorAssetLibrary.save_loaded_asset(static_mesh, False)
    except TypeError:
        unreal.EditorAssetLibrary.save_loaded_asset(static_mesh)


selected_assets = unreal.EditorUtilityLibrary.get_selected_assets()
static_meshes = set(asset for asset in selected_assets if isinstance(asset, unreal.StaticMesh))

selected_actors = unreal.EditorLevelLibrary.get_selected_level_actors()
for actor in selected_actors:
    for component in actor.get_components_by_class(unreal.StaticMeshComponent):
        static_mesh = component.get_editor_property("static_mesh")
        if static_mesh:
            static_meshes.add(static_mesh)

if not static_meshes:
    unreal.log_warning("Select building actors in the level or Static Mesh assets in the Content Browser, then run this script.")
else:
    changed_count = 0
    for mesh in static_meshes:
        if _set_complex_collision(mesh):
            changed_count += 1
            _save_mesh(mesh)

    unreal.log("Updated collision on {0} selected Static Mesh asset(s).".format(changed_count))
