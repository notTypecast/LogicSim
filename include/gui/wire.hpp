#ifndef LOGICSIM_GUI_WIRE_HPP
#define LOGICSIM_GUI_WIRE_HPP

#include <QWidget>
#include <QLabel>

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
            int idx;
            int x, y;
        };

        class Wire
        {
        public:
            Wire(QWidget *parent);
            ~Wire();

            void setComponent1(ComponentLabel *component, int x, int y);
            void setComponent2(ComponentLabel *component, int x, int y);
            void clearComponent2();
            void repositionDest(int dest_x, int dest_y);
            bool saveInComponents();
            void reposition();

            int getComponent1x() const;
            int getComponent1y() const;

            bool finalized() const;

        protected:
            QWidget *_parent;
            QLabel *_hwire1 = nullptr;
            QLabel *_vwire = nullptr;
            QLabel *_hwire2 = nullptr;

            const std::vector<std::pair<double, double>> *_v[2];

            WireConnection _conns[2];

            void _setComponent(int idx, ComponentLabel *component, int x, int y);
            void _calculatePosition(int idx);
        };
    }
}

#endif // LOGICSIM_GUI_WIRE_HPP
