
#include <libsystem/Assert.h>
#include <libsystem/core/CString.h>
#include <libsystem/core/Plugs.h>
#include <libsystem/process/Launchpad.h>
#include <libutils/Path.h>

Launchpad *launchpad_create(const char *name, const char *executable)
{
    Launchpad *launchpad = __create(Launchpad);

    strcpy(launchpad->name, name);
    strcpy(launchpad->executable, executable);

    for (size_t i = 0; i < PROCESS_HANDLE_COUNT; i++)
    {
        launchpad->handles[i] = HANDLE_INVALID_ID;
    }

    launchpad->handles[0] = 0;
    launchpad->handles[1] = 1;
    launchpad->handles[2] = 2;
    launchpad->handles[3] = 3;

    auto executable_path = Path::parse(executable);
    launchpad_argument(launchpad, executable_path.basename().cstring());

    return launchpad;
}

void launchpad_destroy(Launchpad *launchpad)
{
    for (int i = 0; i < launchpad->argc; i++)
    {
        free(launchpad->argv[i]);
    }

    free(launchpad);
}

void launchpad_argument(Launchpad *launchpad, const char *argument)
{
    assert(launchpad->argc < PROCESS_ARG_COUNT);

    if (argument)
    {
        launchpad->argv[launchpad->argc] = strdup(argument);
    }
    else
    {
        launchpad->argv[launchpad->argc] = strdup("");
    }

    launchpad->argv[launchpad->argc + 1] = nullptr;

    launchpad->argc++;
}

void launchpad_handle(Launchpad *launchpad, Handle *handle_to_pass, int destination)
{
    assert(destination >= 0 && destination < PROCESS_ARG_COUNT);

    launchpad->handles[destination] = handle_to_pass->id;
}

Result launchpad_launch(Launchpad *launchpad, int *pid)
{
    int discard;

    if (pid == nullptr)
    {
        pid = &discard;
    }

    Result result = __plug_process_launch(launchpad, pid);

    launchpad_destroy(launchpad);

    return result;
}
