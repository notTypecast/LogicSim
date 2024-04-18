#ifndef LOGICSIM_GUI_COMPONENT_HPP
#define LOGICSIM_GUI_COMPONENT_HPP

#include <QLabel>
#include <QMouseEvent>

#include "gui/resource_loader.hpp"
#include "gui/wire.hpp"


namespace logicsim
{
    namespace gui
    {
        class ComponentLabel : public QLabel
        {
            Q_OBJECT
        public:
            explicit ComponentLabel(QWidget *parent = nullptr);
            ~ComponentLabel();

            void setCompType(COMPONENT comp_type);
            COMPONENT comp_type() const;

            void setBorder(QLabel *border);
            QLabel *border() const;

            bool saveWire(Wire *wire, bool is_input, int idx);
            void removeWire(bool is_input, int idx);
            void moveWires();

            void mouseMoveEvent(QMouseEvent *ev);
            void mousePressEvent(QMouseEvent *ev);
            void mouseReleaseEvent(QMouseEvent *ev);

        protected:
            COMPONENT _comp_type;
            int _n_inputs;
            std::vector<Wire *> _input_wires;
            int _n_outputs;
            std::vector<Wire *> _output_wires;

            int _press_x, _press_y;
            TOOL _current_tool = TOOL::INSERT;
            QLabel *_border = nullptr;

        protected slots:
            void changeMode(TOOL tool);
            void checkRangeQuery(int min_x, int min_y, int max_x, int max_y);
            void wireSnap(int x, int y);
            void checkPosition(int x, int y);

        signals:
            void selected(ComponentLabel *component, bool ctrl);
            void selected_nocheck(ComponentLabel *component);
            void moved(int x, int y);
            void wireSource(ComponentLabel *component, int x, int y);
            void wireMoved(int x, int y);
            void wireSnapFound(int x, int y);
            void wireReleased(int x, int y);
            void positionOverlaps(ComponentLabel *component, int x, int y);

        };
    }
}

#endif // GUI_COMPONENT_HPP
