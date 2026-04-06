/**
 * @file main.cpp
 * @brief Entry point of the Mirach engine. Initializes engine subsystems, loads assets,
 * creates entities, and starts the main rendering loop.
 */

//#include "Program/terrain_scene.hpp"
#include "Program/murdoch_scene.hpp"

/**
 * @brief Main entry point of the engine.
 * Initializes subsystems, loads models, sets up ECS, and runs the main loop.
 *
 * @return int Exit code.
 */
int main() 
{
    std::shared_ptr<MurdochScene> game = std::make_shared<MurdochScene>();

    game->Run();

    return 0;
}