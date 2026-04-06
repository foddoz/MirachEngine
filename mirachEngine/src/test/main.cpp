// main.cpp : Defines the entry point for the application.
//
#include "luaWrapper.h"
#include <iostream>

int main()
{
	std::cout << "Hello CMake." << std::endl;
	sol::state lua;
	lua.open_libraries(sol::lib::base);
	lua.script("print(\"Hello from Lua!\\n\")");
	return 0;
}
