#include "Globals.h"

/* Scripting */
sol::state Globals::lua;

/* UI */
long Globals::selected;
bool Globals::debug;
bool Globals::mouseIsCaptured;