#ifndef LOGICSIM_GUI_MAIN_WINDOW_HPP
#define LOGICSIM_GUI_MAIN_WINDOW_HPP

#define LOGICSIM_VERSION "1.10.18"

#include <QActionGroup>
#include <QApplication>
#include <QButtonGroup>
#include <QMainWindow>
#include <QSettings>
#include <QString>

#include "gui/doc_window.hpp"
#include "gui/properties.hpp"
#include "gui/resource_loader.hpp"

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

namespace logicsim
{
namespace gui
{
class MainWindow : public QMainWindow
{
    Q_OBJECT
  public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void keyPressEvent(QKeyEvent *ev);
    void keyReleaseEvent(QKeyEvent *ev);
    void closeEvent(QCloseEvent *ev);

    void showDocs(const QString &file_path = "");

  protected:
    DocWindow *_doc_window;

    Ui::MainWindow *_ui;
    QActionGroup   *_tool_group;

    struct InsertAction
    {
        QAction  *action;
        COMPONENT comp_type;
        int       res_idx;
    };

    std::vector<InsertAction> _insert_actions;

    void _setSimulationMenu(bool enabled);
    bool _sim_paused;

    TOOL      _last_design_tool    = TOOL::SELECT;
    COMPONENT _last_insert_comp    = COMPONENT::NONE;
    int       _last_insert_res_idx = -1;

    QFrame *_toolbar;
    void    _setupToolbar();

    QPushButton *_select_button, *_move_button, *_wire_button,
      *_wire_remove_button;
    std::vector<QPushButton *> _insert_buttons;
    QPushButton *_start_sim_button, *_stop_sim_button, *_pause_sim_button,
      *_step_sim_button, *_reset_sim_button;

    QSettings *_settings;

  protected slots:
    void addDesignArea();

    // no_apply: do not try to apply simulation mode (already applied)
    void setSimulationMenu(bool running = true, bool no_apply = false);
    void setDesignMenu();
    void setLastDesignTool(TOOL tool, COMPONENT comp_type = COMPONENT::NONE,
                           int res_idx = -1);

    void enableContinue();
    void enablePause();

    void simulationProperties();
    void setSimulationFrequency(QString freq);

    void setUndoActionState(bool undo_enabled, bool redo_enabled);
    void setSelectActionState(bool have_select, bool have_clipboard);

    void quit();

    void toggleToolbar();

    void showVersionPopup();
    void showDocumentation();
};

}
}
#endif // LOGICSIM_GUI_MAIN_WINDOW_HPP
