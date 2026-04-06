#include "scripting_murdoch_scene.hpp"

#include "Agent/Dog/dog.hpp"

void MurdochScripting::Init(std::shared_ptr<MurdochScene> const newScene)
{
	std::cout << "[ScriptingAPI] Initializing..." << std::endl;
	// open some common libraries
	m_lua.open_libraries(sol::lib::base, sol::lib::package);

	m_scene = newScene;

	// Set functions accessible in Lua.

	m_lua.new_usertype<MurdochScene>("MurdochScene",
		"spawn_dog_bed", &MurdochScene::SpawnDogBed,
		"spawn_water_bowl", &MurdochScene::SpawnWaterBowl,
		"spawn_food_bowl", &MurdochScene::SpawnFoodBowl,
		"spawn_dog", &MurdochScene::SpawnDog,
		"set_dog_breed", &MurdochScene::SetDogBreed
	);

	m_lua["GERMAN_SHEPHERD"] = Dog::Breed::GermanShepherd;
	m_lua["GOLDEN_RETRIEVER"] = Dog::Breed::GoldenRetriever;
	m_lua["TOY_POODLE"] = Dog::Breed::ToyPoodle;

	m_lua["active_scene"] = m_scene;

	std::cout << "[ScriptingAPI] Success." << std::endl;
}

void MurdochScripting::Execute(const std::string& scriptFile)
{
	// Check if file exists.
	std::ifstream file(scriptFile.c_str());
	if (!file.good())
	{
		std::cout << "Warning: Lua script does not exist." << std::endl;
		return;
	}

	// Check if lua script is valid.
	sol::load_result script = m_lua.load_file(scriptFile);
	if (!script.valid())
	{
		std::cout << "Warning: Lua script contains errors." << std::endl;
		return;
	}
	m_lua.script_file(scriptFile);
}

bool MurdochScripting::TryExecute(const std::string& content)
{
	sol::load_result script = m_lua.load(content);

	if (!script.valid())
	{
		std::cout << "[Scripting] Lua script failed." << std::endl;
		return false;
	}

	try 
	{
		m_lua.do_string(content);
	}
	catch (std::exception e) 
	{
		std::cout << "Warning: Lua function call critical failure." << std::endl;
		std::cout << e.what();
		return false;
	}

	
	std::cout << "[Scripting] Lua script executed." << std::endl;
	return true;
}
