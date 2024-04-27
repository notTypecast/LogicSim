#ifndef LOGICSIM_GUI_WIRE_HPP
#define LOGICSIM_GUI_WIRE_HPP

#include <QWidget>
#include <QLabel>

#include <limits>

#include "gui/resource_loader.hpp"

namespace logicsim
{
    namespace gui
    {
        class ComponentLabel;

        struct WireConnection
        {
            ComponentLabel *component = nullptr;
            bool is_input;
            int idx; // index of input/output on component
            int x, y; // global coordinates of wire node that connects to component
        };

        class Wire
        {
        public:
            Wire(QWidget *parent);
            ~Wire();

            // sets source component, based on relative position of click (dx, dy)
            // must be called before setComponent2
            // returns false if component 1 could not be set (i/o doesn't exist)
            bool setComponent1(ComponentLabel *component, int dx, int dy);
            // sets destination component
            bool setComponent2(ComponentLabel *component, int dx, int dy);

            // sets source component based on given i/o index
            // returns false if i/o index doesn't exist
            bool setComponent1(ComponentLabel *component, bool is_input, int io_idx);
            // sets destination component based on given i/o index
            bool setComponent2(ComponentLabel *component, bool is_input, int io_idx);

            // repositions wire based on given global destination coordinates (dest_x, dest_y)
            // source coordinates are in _conns[0]
            void repositionDest(int dest_x, int dest_y);
            // saves this wire in the components it is connected to through _conns
            // both components must have been set
            // returns false if connection failed (both inputs/ouputs, already connected input)
            bool saveInComponents();
            // repositions wire to current location
            void reposition();

            void hide();
            void show();

            // returns information about the component which is being driven
            // pair: {Component * of output component, index of output}
            std::pair<ComponentLabel *, int> outputComponentInfo() const;
            ComponentLabel *component1() const;
            ComponentLabel *component2() const;

            // returns position of component 1
            int getComponent1x() const;
            int getComponent1y() const;

            // whether component 2 has been set
            bool finalized() const;

            void saveInOppositeComponent(ComponentLabel *component);
            // removes wire from opposite component (component passed must be one of the wire's components)
            void removeFromOppositeComponent(ComponentLabel *component);
            void removeFromComponents();

            // based on given component and relative position (dx, dy), calculates position of closest input/output
            // returns false if none is found (component has no inputs or outputs that match click position), else true
            // if found, writes position of input/output to x and y in global coordinates, and writes bool is_input
            // to indicate if input was found, as well as io_idx to indicate the index of the input or output chosen
            static bool calculateWireTargetPos(ComponentLabel *component, int dx, int dy, int &x, int &y, bool &is_input, int &io_idx);

        protected:
            QWidget *_parent;
            // made up of 3 parts
            QLabel *_hwire1 = nullptr;
            QLabel *_vwire = nullptr;
            QLabel *_hwire2 = nullptr;

            WireConnection _conns[2];

            bool _setComponent(int idx, ComponentLabel *component, int dx, int dy);
            bool _setComponent(int idx, ComponentLabel *component, bool is_input, int io_idx);
            void _updatePosition(int idx);
        };
    }
}

#endif // LOGICSIM_GUI_WIRE_HPP
