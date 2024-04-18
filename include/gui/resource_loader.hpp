#ifndef LOGICSIM_GUI_RESOURCE_LOADER_HPP
#define LOGICSIM_GUI_RESOURCE_LOADER_HPP

#include <QLabel>
#include <QPixmap>

#include <unordered_map>

namespace logicsim
{
    namespace gui
    {
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
            inline QString IMG_PATH = "../LogicSim/res/";

            inline std::unordered_map<COMPONENT, QPixmap> comp_images;

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

            QPixmap getLine(LINE_TYPE line_type, int size);
            QPixmap getWire(LINE_TYPE line_type, int size);
            void load();
            void deallocate();
        }
    }
}

#endif //LOGICSIM_GUI_RESOURCE_LOADER_HPP
