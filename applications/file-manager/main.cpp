#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>
#include <libsystem/io/Directory.h>
#include <libsystem/process/Launchpad.h>
#include <libsystem/process/Process.h>
#include <libwidget/Application.h>
#include <libwidget/Widgets.h>

#include "file-manager/Breadcrumb.h"
#include "file-manager/FileSystemModel.h"

enum RecordHistory
{
    RECORD_BACKWARD,
    RECORD_FOREWARD,
};

class FileExplorerWindow : public Window
{
private:
    /// --- Navigation bar --- ///
    Widget *_go_backward;
    Widget *_go_foreward;
    Widget *_go_up;
    Widget *_go_home;
    Breadcrumb *_breadcrumb;

    /// --- Table view --- ///
    Table *_table;
    RefPtr<FileSystemModel> _model;

    /// --- Navigation --- ///
    Vector<Path> _backward_history{};
    Path _current_path;
    Vector<Path> _foreward_history{};

public:
    FileExplorerWindow(const char *path)
        : Window(WINDOW_RESIZABLE), _current_path(Path::parse(path))
    {
        icon(Icon::get("folder"));
        title("File Manager");
        size(Vec2i(700, 500));

        root()->layout(VFLOW(0));

        /// --- Navigation bar --- ///
        Widget *toolbar = toolbar_create(root());

        _go_backward = toolbar_icon_create(toolbar, Icon::get("arrow-left"));

        _go_backward->on(Event::ACTION, [this](auto) {
            if (_backward_history.any())
            {
                navigate(_backward_history.pop_back(), RECORD_FOREWARD);
            }
        });

        _go_foreward = toolbar_icon_create(toolbar, Icon::get("arrow-right"));

        _go_foreward->on(Event::ACTION, [this](auto) {
            if (_foreward_history.any())
            {
                navigate(_foreward_history.pop_back(), RECORD_BACKWARD);
            }
        });

        _go_up = toolbar_icon_create(toolbar, Icon::get("arrow-up"));

        _go_up->on(Event::ACTION, [this](auto) {
            if (_current_path.length() == 0)
            {
                return;
            }

            clear_foreward_history();
            navigate(_current_path.dirpath(), RECORD_FOREWARD);
        });

        _go_home = toolbar_icon_create(toolbar, Icon::get("home"));

        _go_home->on(Event::ACTION, [this](auto) {
            clear_foreward_history();
            navigate(Path::parse("/User"), RECORD_BACKWARD);
        });

        new Separator(toolbar);

        _breadcrumb = new Breadcrumb(toolbar, _current_path);
        _breadcrumb->attributes(LAYOUT_FILL);

        new Separator(toolbar);

        auto refresh = toolbar_icon_create(toolbar, Icon::get("refresh"));

        refresh->on(Event::ACTION, [&](auto) {
            _model->update();
        });

        Widget *terminal_button = toolbar_icon_create(toolbar, Icon::get("console"));

        terminal_button->on(Event::ACTION, [](auto) {
            process_run("terminal", NULL);
        });

        update_navigation_bar();

        /// --- Table view --- ///
        _model = make<FileSystemModel>(path);

        _table = new Table(root(), _model);
        _table->attributes(LAYOUT_FILL);
        _table->empty_message("This directory is empty.");
        _table->focus();

        _table->on(Event::ACTION, [this](auto) {
            if (_table->selected() >= 0)
            {
                if (_model->file_type(_table->selected()) == FILE_TYPE_DIRECTORY)
                {
                    clear_foreward_history();
                    auto new_path = Path::join(_current_path, _model->file_name(_table->selected()));
                    navigate(new_path, RECORD_BACKWARD);
                }
                else
                {
                    Launchpad *launchpad = launchpad_create("open", "/System/Binaries/open");
                    launchpad_argument(launchpad, strdup(_model->file_name(_table->selected()).cstring()));
                    launchpad_launch(launchpad, nullptr);
                }
            }
        });
    }

    ~FileExplorerWindow() override
    {
    }

    void update_navigation_bar()
    {
        _go_backward->enable_if(_backward_history.any());
        _go_foreward->enable_if(_foreward_history.any());
        _go_up->enable_if(_current_path.length() > 0);
    }

    void navigate(Path path, RecordHistory record_history)
    {
        if (_current_path == path)
        {
            return;
        }

        _table->select(-1);
        _table->scroll_to_top();

        if (record_history == RECORD_BACKWARD)
        {
            _backward_history.push_back(_current_path);
        }
        else if (record_history == RECORD_FOREWARD)
        {
            _foreward_history.push_back(_current_path);
        }

        _current_path = path;

        _breadcrumb->navigate(path);
        _model->navigate(path);
        update_navigation_bar();
    }

    void clear_foreward_history()
    {
        _foreward_history.clear();
    }
};

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    auto window = new FileExplorerWindow("/User");
    window->show();

    return application_run();
}
