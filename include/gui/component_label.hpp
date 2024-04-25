#ifndef LOGICSIM_GUI_COMPONENT_HPP
#define LOGICSIM_GUI_COMPONENT_HPP

#include <QLabel>
#include <QMouseEvent>

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
        class ComponentLabel : public QLabel
        {
            Q_OBJECT
        public:
            explicit ComponentLabel(COMPONENT comp_type, int resource_idx, QWidget *parent = nullptr);
            ~ComponentLabel();

            COMPONENT comp_type() const;

            model::component::Component *component_model() const;

            void setBorder(QLabel *border);
            QLabel *border() const;

            int resourceIdx() const;

            // keep wire connected to this component
            // is_input: whether wire is input connection
            // idx: index of input/output on component
            bool saveWire(Wire *wire, bool is_input, int idx);
            void removeWire(Wire *wire, bool is_input, int idx);
            // update wire positions based on current position
            void moveWires();

            void mouseMoveEvent(QMouseEvent *ev);
            void mousePressEvent(QMouseEvent *ev);
            void mouseReleaseEvent(QMouseEvent *ev);
            void mouseDoubleClickEvent(QMouseEvent *ev);

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
            QLabel *_border = nullptr;

            Properties *_properties_popup = nullptr;

            // used before simulation start, to keep current parameters
            std::string _base_params;
            int _base_resource_idx;

            // Component Model
            model::component::Component *_component_model;

            void _setupProperties();

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
            void writeComponent(std::ofstream &file);

        signals:
            // emitted when a mouse press registers on component during select mode
            void selected(ComponentLabel *component, bool ctrl);
            // emitted to respond to range query
            void selected_nocheck(ComponentLabel *component);
            // emitted when mouse is moved while component is selected
            void moved(int dx, int dy);
            // emitted when a mouse press registers on component during wire mode
            void wireSource(ComponentLabel *component, int dx, int dy);
            // emitted when a wire being created is moved
            void wireMoved(int dx, int dy);
            // emitted to respond to a wireSnap request, when position can snap to an input/output
            void wireSnapFound(ComponentLabel *component, int x, int y);
            // emitted when a wire being created is released
            void wireReleased();

        };
    }
}

#endif // GUI_COMPONENT_HPP
