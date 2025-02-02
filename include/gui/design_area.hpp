#ifndef LOGICSIM_GUI_DESIGN_AREA_HPP
#define LOGICSIM_GUI_DESIGN_AREA_HPP

#include <QApplication>
#include <QCursor>
#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QSettings>
#include <QStatusBar>
#include <QString>
#include <QTimer>
#include <QUndoStack>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <unordered_set>
#include <vector>

#include "gui/component_label.hpp"
#include "gui/resource_loader.hpp"

#include "model/circuit.hpp"
#include "model/component.hpp"

namespace logicsim
{
namespace gui
{
class InsertComponentCommand;
class DeleteComponentsCommand;
class MoveComponentsCommand;
class InsertWireCommand;
class DeleteWireCommand;
class ChangeSimulationPropertiesCommand;

class Clipboard;

class DesignArea : public QWidget
{
    Q_OBJECT
  public:
    friend class InsertComponentCommand;
    friend class DeleteComponentsCommand;
    friend class MoveComponentsCommand;
    friend class InsertWireCommand;
    friend class DeleteWireCommand;
    friend class ChangeSimulationPropertiesCommand;

    friend class Clipboard;

    explicit DesignArea(QWidget *parent = nullptr);
    ~DesignArea();

    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void mouseDoubleClickEvent(QMouseEvent *ev);
    void wheelEvent(QWheelEvent *ev);

    TOOL mode() const;
    bool setMode(TOOL tool, COMPONENT comp_type = COMPONENT::NONE,
                 int res_idx = 0);
    void stopSimulationMode();
    void pauseSimulation();
    void stepSimulation();
    void continueSimulation();
    void resetSimulation();

    void setStatusBar(QStatusBar *status_bar);

    unsigned int frequency() const;
    void         setFrequency(unsigned int freq);

    // called when tab changes from/to this
    void pauseState();
    // returns true if there was a running simulation before
    bool continueState();

    // whether circuit is empty
    bool empty() const;

    QString filepath() const;
    // writes current circuit to file
    bool writeToFile(bool new_file = false);
    void readFromFile(QString filepath);

    void undoAction();
    void redoAction();

    void cutAction();
    void copyAction();
    void pasteAction();
    void deleteAction();

    void setColorWires(bool enabled);

    void   zoomIn(int origin_x, int origin_y);
    void   zoomOut(int origin_x, int origin_y);
    void   resetZoom();
    double getScale();

    void clearUndoStack();

    bool isDrawingWire();

    void nextComponent();
    void previousComponent();

  protected:
    // removes all components from selected
    void _unselectAll();

    // current tool
    TOOL _selected_tool = TOOL::SELECT;
    // current component to insert (applies to insert tool)
    COMPONENT _insert_component;
    int       _insert_resource_idx;

    // vector of pointers to all components
    // used to move view to next component
    std::vector<ComponentLabel *> _components;
    int                           _component_idx = 0;

    // vector of selected components
    std::vector<ComponentLabel *> _selected_components;

    // initial selection point for selection border
    int _select_x, _select_y;
    // labels for selection lines
    // in order, array holds two vertical (left to right),
    // then two horizontal (top to bottom)
    QLabel *_selection_border[4] = { nullptr };

    // current wire being drawn
    Wire *_wire = nullptr;

    struct WireSnapPosition
    {
        ComponentLabel *component;
        int             x;
        int             y;
    };

    // positions to snap destination to
    // wire snaps to closest
    WireSnapPosition              _wire_snap_closest;
    std::vector<WireSnapPosition> _wire_snap_positions;

    // Circuit model
    model::circuit::Circuit _circuit_model;

    unsigned int _freq  = 100;
    QTimer      *_timer = nullptr;

    QStatusBar *_status_bar;
    QLabel     *_ticks_label;

    // State information
    // Saved when tab changes, to reset when reselected
    bool    _paused_state = false;
    QString _ticks_label_text;
    bool    _state_sim_paused;

    QString _filepath;

    void _connectComponent(ComponentLabel *label, bool first_time = false);
    void _disconnectComponent(ComponentLabel *label);
    void _deleteComponents(
      std::unordered_map<std::string, ComponentLabel *> components);

    void _connectWire(Wire *wire, bool first_time = true);
    void _disconnectWire(Wire *wire);

    QUndoStack *_undo_stack;

    bool                _moved_components = false;
    std::vector<QPoint> _init_comp_positions;

    std::vector<std::pair<Wire *, int>> _proximity_wire_dist;
    Wire                               *_marked_wire = nullptr;

    Clipboard *_clipboard;

    bool _color_wires = false;

    /* Transformations
     * Keeps information about current translation/scale transform
     * Each component has a native position, as well as a transformed position
     * The transformed position, which is the actual screen position of the
     * component, is what is saved
     * There are two types of transformations:
     *
     * 1) Position transformations
     *  These consist of simple translations
     *  To calculate, multiply each point by a 2D transformation matrix (to the
     *  left), of type:
     *  | 1  0  tx |
     *  | 0  1  ty | * P
     *  | 0  0   1 |
     *  where tx, ty are the offsets to move by
     *
     * 2) Zoom transformations
     *  These consist of zooming around a certain point of origin (ox, oy)
     *  To apply such a transformation, there are three steps:
     *  * Move the point (ox, oy) to the native origin (0, 0)
     *  * Apply a scale transformation
     *  * Move the origin point back to its original location (ox, oy)
     *  To calculate, multiply each point by the 2D transformation matrix for
     *  the first translation, followed by the one for scaling, followed by the
     *  one for the second translation:
     *  | 1  0  ox |   | s  0  0 |   | 1  0  -ox |
     *  | 0  1  oy | * | 0  s  0 | * | 0  1  -oy | * P
     *  | 0  0   1 |   | 0  0  1 |   | 0  0    1 |
     *  Notice that the order is from right to left, such that we multiply first
     *  by the rightmost matrix (move to native origin), scale next and,
     *  finally, bring origin back to original position
     *  Because order of multiplication does not matter, this is equivalent to
     *  the following:
     *  | s  0  -s*ox+ox |
     *  | 0  s  -s*oy+oy | * P
     *  | 0  0         1 |
     *
     * In this representation, each 2D point P is of the following type:
     *     | x |
     * P = | y |
     *     | 1 |
     */

    /* Transformation struct
     * Keeps information about current transformation state
     * Data:
     *  * tx, ty: The current translation values of the transformation matrix:
     *  | s  0  tx |
     *  | 0  s  ty |
     *  | 0  0   1 |
     *  Which can be used to transform any native point into the current view's
     *  coordinates
     *  * tx_inv, ty_inv: The values of the inverse transformation matrix:
     *  | s  0  tx_inv |
     *  | 0  s  ty_inv |
     *  | 0  0       1 |
     *  Which can be used to transform any of the current view's points into
     *  native coordinates
     * Information about scale is unnecessary, since we can deduce it from the
     * base zoom level and the current zoom level
     *
     * Uses:
     * 1) The forward transformation values are not used to perform
     * transformations directly, since components keep their transformed
     * positions
     * As such, whenever a new transformation must be made, we simply
     * multiply from the left with the appropriate matrix
     * However, when using undo on a move action for component(s), it is
     * necessary to save initial and final coordinates as native, because the
     * view might be moved before redo is performed
     * Therefore, on redo, we need to forward transform those native coordinates
     * back to the view's coordinates, thus requiring tx and ty
     * 2) The inverse transformation values are used to return to native
     * coordinates
     * This is useful when saving to a file, where we need to save
     * native coordinates
     * It is also used to save native coordinates when performing undo on a
     * move action
     */
    struct Transformation
    {
        double tx     = 0.0;
        double ty     = 0.0;
        double tx_inv = 0.0;
        double ty_inv = 0.0;
    } _transformation;

    std::pair<int, int> _toNativeCoordinates(int x, int y);
    std::pair<int, int> _toViewCoordinates(int x, int y);

    int _init_move_x, _init_move_y;

    int          _zoom_level           = 5;
    const double BASE_ZOOM_LEVEL       = 5;
    const double BASE_SCALE_FACTOR     = 1.2;
    const double INV_BASE_SCALE_FACTOR = 1 / BASE_SCALE_FACTOR;

    void _zoom(int origin_x, int origin_y, int new_zoom_level);

    // moves view to component indicated by _current_component
    void _moveToComponent();

    /* Signals and slots often transmit position information
     * Such information is referred to as global, if its frame of reference
     * is the origin point of DesignArea; in that case, it is denoted (x, y)
     * Such information is referred to as relative, if its frame of reference
     * is the origin point of a different widget (usually a ComponentLabel);
     * in that case, it is denoted as (dx, dy)
     */
  protected slots:
    // Design Slots

    // triggered by selected of ComponentLabel
    // adds component to selected (or removes, based on ctrl)
    void addSelected(ComponentLabel *component, bool ctrl = false);
    // triggered by selected_nocheck of ComponentLabel
    // adds component to selected without performing any checks
    void addSelected_nocheck(ComponentLabel *component);
    // triggered by moved of ComponentLabel
    // moves selected components by dx, dy
    void moveSelectedComponents(int dx, int dy);
    // triggered by moveFinished of ComponentLabel
    // adds move to undo stack
    void finishMove();
    // triggered by wireSource of ComponentLabel
    // sets component as source of wire, with source position dx,dy
    void getWireSource(ComponentLabel *component, int dx, int dy);
    // triggered by wireMoved of ComponentLabel
    // moves wire destionation by dx, dy from wire source position
    void moveWireDest(int dx, int dy);
    // triggered by wireSnapFound of ComponentLabel
    // sets _wire_snap_x and _wire_snap_y to x and y
    void getWireSnapPos(ComponentLabel *component, int x, int y);
    // triggered by wireReleased of ComponentLabel
    // sets final position of wire target as dx, dy away from the wire source
    // position
    void setWireDest();
    // triggered by performPropertyUndoAction of ComponentLabel
    void propertyUndoActionPerformed();
    // triggered by proximityConfirmed of Wire
    void getProximityWireDistance(Wire *wire, int distance);

    // Simulation
    // triggered by timer
    void executeTick(unsigned int ticks = 1);

  signals:
    // emitted when the mode is changed
    void modeChanged(TOOL tool);
    // emitted when a selection area is completed, to find components inside it
    void rangeQuery(int x_min, int y_min, int x_max, int y_max);
    // emitted during wire creation, when destination is moved
    // used to find components whose inputs/outputs are close to wire
    // destination for snapping
    void wireSnap(ComponentLabel *wire_source, int x, int y);
    // emitted during simulation, after every tick
    // components receiving this signal will evaluate based on their component
    // model
    void evaluate();
    // emitted for components when simulation is reset
    void resetResource();
    // emitted for wires when simulation is reset (only when colored wires is
    // enabled)
    void resetWireResource();
    // emitted when writing to file
    void writeComponent(std::ofstream &file, double inverse_scale_factor,
                        double inverse_translation_x,
                        double inverse_translation_y) const;
    // emitted when an undo action is redone/undone
    void newUndoActionPerformed(bool was_undo, bool undo_enabled,
                                bool redo_enabled);
    // emitted when mouse is moved during wire remove mode
    void wireProximityCheck(int x, int y);
    // emitted when selections change, to update edit menu
    void newSelection(bool have_selected, bool have_clipboard);
    // emitted during simulation, if color wires is enabled
    void evaluateWire();
    // emitted to hide all colored wires if color wires is disabled during
    // simulation
    void disableColorWires();
    // emitted when a move transformation is performed
    void transformPosition(int dx, int dy);
    // emitted when a zoom transformation is performed
    void transformScale(double size_scale, double pos_scale, double offset_x,
                        double offset_y);
    // emitted when a component is removed from the selection, so the component
    // doesn't emit moved signals
    void cancelMove(ComponentLabel *component);
};
}
}

#endif // DESIGN_AREA_HPP
