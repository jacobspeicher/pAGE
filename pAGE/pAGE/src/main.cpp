#include <string>

#include "ProjectManager.h"

#include "Engine/EventBus/EventBus.h"
#include "Engine/Events/ProjectLoadedEvent.h"

int main(int argc, char* argv[])
{
    ProjectManager manager;

    manager.Initialize();
    manager.Run();

    return 0;
}