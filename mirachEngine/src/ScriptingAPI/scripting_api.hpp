#ifndef SCRIPTINGAPI_HPP
#define SCRIPTINGAPI_HPP

#include <string>
#include "Program/scene.hpp"
#include "sol/sol.hpp"

class ScriptingAPI 
{
private:
  ScriptingAPI() {};
  ~ScriptingAPI(){};

  static sol::state m_lua;

public:

  /**
   *  @brief Setup Lua environment.
   */
  static void Init();

  /**
   *  @brief Set a value onto the Lua stack.
   */
  template<typename T>
  static void Set(std::string name, T value);

  /**
   *  @brief Register a class onto Lua (TODO).
   */
  static void RegisterClass(std::string p_name);

  /**
   *  @brief Execute a Lua script from a file.
   */
  static void ScriptFile(std::string p_path);
};

#endif //ScriptingAPI_HPP