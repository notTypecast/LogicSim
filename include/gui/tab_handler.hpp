#ifndef LOGICSIM_GUI_TAB_HANDLER_HPP
#define LOGICSIM_GUI_TAB_HANDLER_HPP

#include <QMessageBox>
#include <QTabWidget>

#include <unordered_map>
#include <vector>

#include "gui/design_area.hpp"
#include "gui/resource_loader.hpp"

namespace logicsim
{
namespace gui
{
class TabHandler : public QTabWidget
{
    Q_OBJECT
  public:
    explicit TabHandler(QWidget *parent = nullptr);

    void setStatusBar(QStatusBar *status_bar);

    DesignArea *currentDesignArea() const;

    bool setSimulationMode();

    bool saveAndCloseAll();

    bool setTempMoveMode();
    bool endTempMoveMode();

  protected:
    struct FileUndoState
    {
        bool is_saved { true };
        int  undo_counter { 0 };
        bool undo_enabled { false };
        bool redo_enabled { false };
    };

    DesignArea *_designArea(int idx) const;
    bool        _removeDesignArea(DesignArea *design_area);
    void        _setupTab(DesignArea *design_area);

    QStatusBar *_status_bar;

    int _selected_idx = 0;

    void _saveFile(bool new_file);

    // maps each tab to its undo state
    std::unordered_map<DesignArea *, FileUndoState> _file_undo_state;
    // maps full file directory to design area
    std::unordered_map<QString, DesignArea *> _open_files;
    // maps file name to design areas (tabs) with that file name
    std::unordered_map<QString, std::vector<DesignArea *>> _open_filenames;

    bool _wire_color = false;

    void _addUnsavedIcon();
    void _removeUnsavedIcon();

    bool _temp_move = false;

  public slots:
    void addDesignArea();
    bool removeDesignArea();
    void openFile();
    void saveFile();
    void saveFileAs();
    void undoAction();
    void redoAction();
    void cutAction();
    void copyAction();
    void pasteAction();
    void deleteAction();
    void toggleWireColor();
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void nextComponent();
    void previousComponent();

    void setSelectMode();
    void setMoveMode();
    void setWireMode();
    void setWireRemoveMode();
    void setInsertMode();
    void stopSimulationMode();
    void pauseSimulation();
    void stepSimulation();
    void continueSimulation();
    void resetSimulation();

  protected slots:
    void changeArea(int idx);
    bool removeDesignArea(int idx);

    // triggered by a design area when an action which changes the undo stack is
    // performed any action that changes the circuit counts as a redo
    void performUndoAction(bool was_undo, bool undo_enabled, bool redo_enabled);
    void performSelectionAction(bool have_select, bool have_clipboard);

  signals:
    void designToolChanged(TOOL tool, COMPONENT comp_type = COMPONENT::NONE,
                           int res_idx = -1);
    void designTabChosen();
    void simulationTabChosen(bool running_sim);

    // signal to inform main window of undo/redo actions, to update edit menu
    void undoActionPerformed(bool undo_enabled, bool redo_enabled);
    void selectionActionPerformed(bool have_select, bool have_clipboard);
};

void setMouseTrackingRecursive(QWidget *widget, bool value);
}
}

#endif // LOGICSIM_GUI_TAB_HANDLER_HPP
