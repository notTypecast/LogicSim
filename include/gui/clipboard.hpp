#ifndef LOGICSIM_GUI_CLIPBOARD_HPP
#define LOGICSIM_GUI_CLIPBOARD_HPP

#include <QString>

#include <vector>
#include <unordered_map>

#include "gui/design_area.hpp"
#include "gui/component_label.hpp"
#include "gui/wire.hpp"
#include "gui/resource_loader.hpp"

namespace logicsim
{
    namespace gui
    {
        inline const int PASTE_OFFSET_Y = 10;

        struct ComponentRepr
        {
            COMPONENT comp_type;
            int res_idx;
            QString params;
            int x, y;
            // keeps component and output index for each input
            std::vector<std::pair<ComponentRepr *, int>> inputs;
        };

        class Clipboard
        {
        public:
            Clipboard(DesignArea *design_area);

            void copy(std::vector<ComponentLabel *> components);
            void paste();

            bool empty() const;

        protected:
            DesignArea *_design_area;
            std::vector<ComponentRepr> _component_reprs;
        };
    }
}

#endif // LOGICSIM_GUI_CLIPBOARD_HPP
