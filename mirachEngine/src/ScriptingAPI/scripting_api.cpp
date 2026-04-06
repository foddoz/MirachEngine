#include "scripting_api.hpp"

#include "ECS/entity_manager.hpp"

sol::state ScriptingAPI::m_lua;

void ScriptingAPI::Init() 
{
  std::cout << "[ScriptingAPI] Initializing..." << std::endl;
  // open some common libraries
  m_lua.open_libraries(sol::lib::base, sol::lib::package);
  
  // initialize all classes that are accessible via the scripting API (TODO).
  EntityManager::RegisterScriptingAPI();


  std::cout << "[ScriptingAPI] Success." << std::endl;
}



template<typename T>
static void ScriptingAPI::Set(std::string name, T value) 
{
  m_lua.set(name, value);
}

void ScriptingAPI::RegisterClass(std::string p_name) 
{

}

void ScriptingAPI::ScriptFile(std::string p_path) 
{
  m_lua.script_file(p_path);
}