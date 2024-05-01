#ifndef LOGICSIM_GUI_COMPONENT_HPP
#define LOGICSIM_GUI_COMPONENT_HPP

#include <QLabel>
#include <QMouseEvent>
#include <QUndoStack>

#include <vector>
#include <string>
#include <fstream>

#include "gui/resource_loader.hpp"
#include "gui/wire.hpp"
#include "gui/properties.hpp"

#include "model/component.hpp"
#include "model/mapped_data.hpp"

#include "utils.hpp"

namespace logicsim
{
    namespace gui
    {
        class ChangeComponentPropertyCommand;

        class ComponentLabel : public QLabel
        {
            friend class ChangeComponentPropertyCommand;

            Q_OBJECT
        public:
            explicit ComponentLabel(COMPONENT comp_type, int resource_idx, double scale, QUndoStack *stack, QWidget *parent = nullptr);
            ~ComponentLabel();

            COMPONENT comp_type() const;

            model::component::Component *component_model() const;

            void showBorder();
            void hideBorder();

            int resourceIdx() const;
            QString params() const;

            const std::vector<Wire *> inputWires() const;
            const std::vector<std::vector<Wire *>> outputWires() const;

            // returns pre-transformation coordinates
            QPoint getNativeCoordinates(double inverse_scale_factor, double inverse_translation_x, double inverse_translation_y);

            // keep wire connected to this component
            // is_input: whether wire is input connection
            // idx: index of input/output on component
            bool saveWire(Wire *wire, bool is_input, int idx);
            void removeWire(Wire *wire, bool is_input, int idx);
            // update wire positions based on current position
            void moveWires();

            void removeFromCircuit();
            void bringBackToCircuit();

            bool getValue();

            void mouseMoveEvent(QMouseEvent *ev);
            void mousePressEvent(QMouseEvent *ev);
            void mouseReleaseEvent(QMouseEvent *ev);
            void mouseDoubleClickEvent(QMouseEvent *ev);

            void move(int pos_x, int pos_y);

        protected:
            int _resource_idx = 0;
            COMPONENT _comp_type;
            // input wires: 1 wire per input
            std::vector<Wire *> _input_wires;
            // output wires: many wires per output
            std::vector<std::vector<Wire *>> _output_wires;

            // initial position of mouse press
            // used for keeping movement relative to initial mouse press when selected
            int _press_x, _press_y;
            TOOL _current_tool = TOOL::INSERT;
            QLabel *_border;

            Properties *_properties_popup = nullptr;

            // used before simulation start, to keep current parameters
            std::string _base_params;
            int _base_resource_idx;

            // Component Model
            model::component::Component *_component_model;

            void _setupProperties();

            QUndoStack *_undo_stack;

            ChangeComponentPropertyCommand *_property_command = nullptr;

            double _scale;

        public slots:
            // called on object creation
            // triggered by properties popup
            // sets resource index and loads resource
            void setResourceByIdx(int idx);
            // sets new parameter string for component
            void setParams(QString params);

        protected slots:
            // triggered by changeMode of DesignArea
            void changeMode(TOOL tool);
            // triggered by rangeQuery of DesignArea
            void checkRangeQuery(int min_x, int min_y, int max_x, int max_y);
            // triggered by wireSnap of DesignArea
            void wireSnap(ComponentLabel *wire_source, int x, int y);
            // triggered by evaluate of DesignArea
            void evaluate();
            // triggered by resetResource of DesignArea
            void resetResource();
            // triggered by writeComponent of DesignArea
            void writeComponent(std::ofstream &file, double inverse_scale_factor, double inverse_translation_x, double inverse_translation_y);
            // triggered by transformPosition of DesignArea
            void positionTransformationApplied(int dx, int dy);
            // triggered by transformScale of DesignArea
            void scaleTransformationApplied(double size_scale, double pos_scale, double offset_x, double offset_y);

        signals:
            // emitted when a mouse press registers on component during select mode
            void selected(ComponentLabel *component, bool ctrl);
            // emitted to respond to range query
            void selected_nocheck(ComponentLabel *component);
            // emitted when mouse is moved while component is selected
            void moved(int dx, int dy);
            // emitted when mouse click is released after move
            void moveFinished();
            // emitted when a mouse press registers on component during wire mode
            void wireSource(ComponentLabel *component, int dx, int dy);
            // emitted when a wire being created is moved
            void wireMoved(int dx, int dy);
            // emitted to respond to a wireSnap request, when position can snap to an input/output
            void wireSnapFound(ComponentLabel *component, int x, int y);
            // emitted when a wire being created is released
            void wireReleased();
            // emitted when a property value is changed
            void performPropertyUndoAction();

        };
    }
}

#endif // GUI_COMPONENT_HPP
