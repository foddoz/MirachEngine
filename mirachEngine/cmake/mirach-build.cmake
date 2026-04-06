# Output executable and game files to MIRACH_BUILD_DIR.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MIRACH_BUILD_DIR})

if(MIRACH_BUILD_GAME)
    # Parse tags and sort game assets.
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/mirach-assets.cmake")

    add_executable(mirach_game
            ${MIRACH_GAME_ASSETS}
            "${MIRACH_SOURCE_DIR}/mirach.cpp"
            "${MIRACH_SOURCE_DIR}/Camera/camera.cpp"
            "${MIRACH_SOURCE_DIR}/Graphics/Meshes/mesh3d.cpp"
            "${MIRACH_SOURCE_DIR}/Graphics/Meshes/meshFile.cpp"
            "${MIRACH_SOURCE_DIR}/Graphics/Meshes/meshTerrain.cpp"
            "${MIRACH_SOURCE_DIR}/Graphics/Meshes/terrain_chunk.cpp"
            "${MIRACH_SOURCE_DIR}/Graphics/Meshes/meshSkyDome.cpp"
            "${MIRACH_SOURCE_DIR}/Graphics/Animation/MD2.cpp"
            "${MIRACH_SOURCE_DIR}/Graphics/Animation/animation_system.cpp"

            "${MIRACH_SOURCE_DIR}/Math/math.cpp"


            "${MIRACH_SOURCE_DIR}/Program/program.cpp"
            "${MIRACH_SOURCE_DIR}/Program/window.cpp"
            "${MIRACH_SOURCE_DIR}/Program/terrain_scene.cpp"
            "${MIRACH_SOURCE_DIR}/Program/murdoch_scene.cpp"

            "${MIRACH_SOURCE_DIR}/Graphics/Texturing/texture.cpp"
            "${MIRACH_SOURCE_DIR}/Graphics/Texturing/image.cpp"

            "${MIRACH_SOURCE_DIR}/Graphics/Rendering/image_renderer.cpp"
            "${MIRACH_SOURCE_DIR}/Graphics/Rendering/render_system.cpp"

            "${MIRACH_SOURCE_DIR}/Graphics/UI/crossAim.cpp"

            "${MIRACH_SOURCE_DIR}/Transform/Transform.cpp"
    
            "${MIRACH_SOURCE_DIR}/Terrain/terrain_manager.cpp"
    
            "${MIRACH_SOURCE_DIR}/Events/input.cpp"
            "${MIRACH_SOURCE_DIR}/Events/input_manager.cpp"
    
            "${MIRACH_SOURCE_DIR}/Scripts/player_script.cpp"
            "${MIRACH_SOURCE_DIR}/Scripts/floating_player_script.cpp"
            "${MIRACH_SOURCE_DIR}/Scripts/grounded_player_script.cpp"
    
            "${MIRACH_SOURCE_DIR}/ECS/entity_manager.cpp"

            "${MIRACH_SOURCE_DIR}/ScriptingAPI/scripting_api.cpp"
            "${MIRACH_SOURCE_DIR}/ScriptingAPI/scripting_murdoch_scene.cpp"
    
            "${MIRACH_SOURCE_DIR}/Physics/physics_system.cpp"

            "${MIRACH_SOURCE_DIR}/Physics/ScratchPhysics/scratch_world.cpp"
            "${MIRACH_SOURCE_DIR}/Physics/ScratchPhysics/scratch_rigidbody.cpp"
            "${MIRACH_SOURCE_DIR}/Physics/ScratchPhysics/scratch_collider.cpp"

            "${MIRACH_SOURCE_DIR}/Physics/ScratchPhysics/ScratchColliders/scratch_sphere_collider.cpp"
            "${MIRACH_SOURCE_DIR}/Physics/ScratchPhysics/ScratchColliders/scratch_box_collider.cpp"
            "${MIRACH_SOURCE_DIR}/Physics/ScratchPhysics/ScratchColliders/scratch_capsule_collider.cpp"

            "${MIRACH_SOURCE_DIR}/Physics/CollisionCallbacks/collision_callback.cpp"
            "${MIRACH_SOURCE_DIR}/Physics/CollisionCallbacks/log_collision_callback.cpp"
            "${MIRACH_SOURCE_DIR}/Physics/CollisionCallbacks/attach_collision_callback.cpp"


            "${MIRACH_SOURCE_DIR}/FSM/enemy.cpp"
            "${MIRACH_SOURCE_DIR}/FSM/enemyState.cpp"
            "${MIRACH_SOURCE_DIR}/FSM/hoverEnemyState.cpp"
            "${MIRACH_SOURCE_DIR}/FSM/NPC/animal.cpp"
           
            "${MIRACH_SOURCE_DIR}/Emotion/emotion_system.cpp"
            "${MIRACH_SOURCE_DIR}/Emotion/nature.cpp"

            "${MIRACH_SOURCE_DIR}/Agent/Dog/dog.cpp"
            "${MIRACH_SOURCE_DIR}/Agent/Dog/dog_behaviour_component.cpp"

            "${MIRACH_SOURCE_DIR}/Wellness/wellness.cpp"
            "${MIRACH_SOURCE_DIR}/Affordance/affordance.cpp"
            "${MIRACH_SOURCE_DIR}/Affordance/AffordanceManager.cpp"
            "${MIRACH_SOURCE_DIR}/Debug/debug.cpp"
    )
    target_link_libraries(mirach_game mirach)

endif()

if(MIRACH_BUILD_INTERPRETER)
    #add_executable (mirach_interpreter "${MIRACH_SOURCE_DIR}/mirach_interpreter.cpp")
    #target_link_libraries (mirach_interpreter mirach)
endif()

if(MIRACH_BUILD_COMPILER)
    #add_executable (mirach_compiler "${MIRACH_SOURCE_DIR}/mirach_compiler.cpp")
    #target_link_libraries (mirach_compiler mirach)
endif()