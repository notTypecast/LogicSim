#ifndef LOGICSIM_GUI_RESOURCE_LOADER_HPP
#define LOGICSIM_GUI_RESOURCE_LOADER_HPP

#include <QLabel>
#include <QPixmap>

#include <unordered_set>
#include <unordered_map>

namespace logicsim
{
    namespace gui
    {
        class ComponentLabel;

        enum TOOL
        {
            SELECT,
            WIRE,
            INSERT
        };

        enum COMPONENT
        {
            NONE,
            NOT_GATE,
            AND_GATE,
            OR_GATE,
            XOR_GATE,
            NAND_GATE,
            NOR_GATE,
            XNOR_GATE,
            CONSTANT,
            SWITCH,
            OSCILLATOR,
            KEYPAD,
            LED,
            _7SEG_5IN,
            _7SEG_8IN,
            SRLATCH,
            JKLATCH,
            TLATCH,
            DLATCH,
            SRFLIPFLOP,
            JKFLIPFLOP,
            TFLIPFLOP,
            DFLIPFLOP
        };

        namespace resources
        {
            inline std::unordered_set<COMPONENT> components_with_properties;

            inline QString IMG_PATH = "../LogicSim/res/";

            // images for each component
            inline std::unordered_map<COMPONENT, std::vector<QPixmap>> comp_images;

            inline QPixmap *border;
            inline QPixmap *hselline, *vselline;
            inline QPixmap *hwire, *vwire;

            inline std::unordered_map<COMPONENT, std::pair<std::vector<std::pair<double, double>>, std::vector<std::pair<double, double>>>> comp_io_rel_pos;

            enum LINE_TYPE
            {
                HORIZONTAL,
                VERTICAL
            };

            const int LINE_THICKNESS = 1;
            const int WIRE_THICKNESS = 2;

            const std::vector<std::pair<double, double>> &getComponentIOPositionVector(COMPONENT comp_type, bool is_input);
            const std::pair<int, int> getComponentIORelativePos(ComponentLabel *component, bool is_input, int idx);

            QPixmap getBorder(int width, int height);
            QPixmap getLine(LINE_TYPE line_type, int size);
            QPixmap getWire(LINE_TYPE line_type, int size);
            void load();
            void deallocate();
        }
    }
}

#endif //LOGICSIM_GUI_RESOURCE_LOADER_HPP
