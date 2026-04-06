#pragma once
#include "Program/murdoch_scene.hpp"

class MurdochScripting 
{
public:
	/// <summary>
	///  Initialize the Lua state and set the scene that the scripting object will affect.
	/// </summary>
	/// <param name="newScene"></param>
	void Init(std::shared_ptr<MurdochScene> const newScene);

	// Add a function to Lua.
	template<typename T>
	void AddGlobalFunction(std::string name, T* func);

	void Execute(const std::string& scriptFile);

	bool TryExecute(const std::string& content);

private:
	std::shared_ptr<MurdochScene> m_scene;

	sol::state m_lua;
};

template<typename T>
inline void MurdochScripting::AddGlobalFunction(std::string name, T* func)
{
	try 
	{
		m_lua.set_function(name, func);
	}
	catch (std::exception e) 
	{
		std::cout << "Warning: Lua function binding failed." << std::endl;
		std::cout << e.what();
		return;
	}

	std::cout << "[Scripting] Function binding success." << std::endl;
	return;
}
