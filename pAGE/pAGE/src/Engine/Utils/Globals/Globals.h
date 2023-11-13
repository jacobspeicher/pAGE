#pragma once

#include <sol/sol.hpp>

class Globals {
public:
	/* Scripting */
	static sol::state lua;

	/* UI */
	static long selected;
	static bool mouseIsCaptured;
	static bool debug;
};