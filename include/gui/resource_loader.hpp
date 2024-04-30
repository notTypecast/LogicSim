#ifndef LOGICSIM_GUI_RESOURCE_LOADER_HPP
#define LOGICSIM_GUI_RESOURCE_LOADER_HPP

#include <QLabel>
#include <QPixmap>

#include <unordered_set>
#include <unordered_map>
#include <bitset>
#include <sstream>
#include <iomanip>

#include "model/mapped_data.hpp"
#include "model/component.hpp"
#include "model/gates.hpp"
#include "model/inputs.hpp"
#include "model/outputs.hpp"
#include "model/memory.hpp"

namespace logicsim
{
    namespace gui
    {
        class ComponentLabel;

        enum TOOL
        {
            SELECT,
            WIRE,
            WIRE_REMOVE,
            INSERT,
            SIMULATE
        };

        enum COMPONENT
        {
            NONE,
            BUFFER,
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

        inline const std::unordered_map<COMPONENT, std::string> comp_type_to_ctype = {
            {BUFFER, "BUFFER"},
            {NOT_GATE, "NOT"},
            {AND_GATE, "AND"},
            {OR_GATE, "OR"},
            {XOR_GATE, "XOR"},
            {NAND_GATE, "NAND"},
            {NOR_GATE, "NOR"},
            {XNOR_GATE, "XNOR"},
            {CONSTANT, "CONSTANT"},
            {SWITCH, "SWITCH"},
            {OSCILLATOR, "OSCILLATOR"},
            {KEYPAD, "KEYPAD"},
            {LED, "OUTPUT"},
            {_7SEG_5IN, "5IN_7SEGMENT"},
            {_7SEG_8IN, "8IN_7SEGMENT"},
            {SRLATCH, "SRLATCH"},
            {JKLATCH, "JKLATCH"},
            {DLATCH, "DLATCH"},
            {TLATCH, "TLATCH"},
            {SRFLIPFLOP, "SRFLIPFLOP"},
            {JKFLIPFLOP, "JKFLIPFLOP"},
            {DFLIPFLOP, "DFLIPFLOP"},
            {TFLIPFLOP, "TFLIPFLOP"}
        };

        inline const int WIRE_REMOVE_DISTANCE_THRESHOLD = 40;

        namespace resources
        {
            inline const std::unordered_map<std::string, COMPONENT> ctype_to_component_t = {
                {"AND", COMPONENT::AND_GATE},
                {"OR", COMPONENT::OR_GATE},
                {"XOR", COMPONENT::XOR_GATE},
                {"NAND", COMPONENT::NAND_GATE},
                {"NOR", COMPONENT::NOR_GATE},
                {"XNOR", COMPONENT::XNOR_GATE},
                {"NOT", COMPONENT::NOT_GATE},
                {"BUFFER", COMPONENT::BUFFER},
                {"CONSTANT", COMPONENT::CONSTANT},
                {"SWITCH", COMPONENT::SWITCH},
                {"OSCILLATOR", COMPONENT::OSCILLATOR},
                {"KEYPAD", COMPONENT::KEYPAD},
                {"SRLATCH", COMPONENT::SRLATCH},
                {"JKLATCH", COMPONENT::JKLATCH},
                {"DLATCH", COMPONENT::DLATCH},
                {"TLATCH", COMPONENT::TLATCH},
                {"SRFLIPFLOP", COMPONENT::SRFLIPFLOP},
                {"JKFLIPFLOP", COMPONENT::JKFLIPFLOP},
                {"DFLIPFLOP", COMPONENT::DFLIPFLOP},
                {"TFLIPFLOP", COMPONENT::TFLIPFLOP},
                {"OUTPUT", COMPONENT::LED},
                {"5IN_7SEGMENT", COMPONENT::_7SEG_5IN},
                {"8IN_7SEGMENT", COMPONENT::_7SEG_8IN}
            };

            inline const std::unordered_set<COMPONENT> components_with_properties =
            {
                COMPONENT::CONSTANT,
                COMPONENT::SWITCH,
                COMPONENT::OSCILLATOR
            };

            inline const QString IMG_PATH = "../LogicSim/res/";

            // images for each component
            // initialized when load is called, because QPixmap cannot be created here
            inline std::unordered_map<COMPONENT, std::vector<QPixmap>> comp_images;

            inline QPixmap *border;
            inline QPixmap *hselline, *vselline;
            inline QPixmap *hwire, *vwire;
            inline QPixmap *hwire_up, *hwire_down, *vwire_left, *vwire_right;
            inline QPixmap *hwire_on, *vwire_on;

            inline const std::unordered_map<size_t, size_t> _7seg_5in_res_map =
            {
                {119, 1},
                {65, 2},
                {110, 3},
                {107, 4},
                {89, 5},
                {59, 6},
                {63, 7},
                {97, 8},
                {127, 9},
                {123, 10},
                {125, 11},
                {31, 12},
                {54, 13},
                {79, 14},
                {62, 15},
                {60, 16},
                {247, 17},
                {193, 18},
                {238, 19},
                {235, 20},
                {217, 21},
                {187, 22},
                {191, 23},
                {225, 24},
                {255, 25},
                {251, 26},
                {253, 27},
                {159, 28},
                {182, 29},
                {207, 30},
                {190, 31},
                {188, 32},
            };

            // ranges apply to both rows and columns, because keypad is symmetric
            inline const std::vector<std::pair<double, double>> keypad_rel_pos_range =
            {
                {0.14, 0.29},
                {0.33, 0.48},
                {0.52, 0.67},
                {0.71, 0.86}
            };

            inline const std::unordered_map<COMPONENT, std::pair<std::vector<std::pair<double, double>>, std::vector<std::pair<double, double>>>> comp_io_rel_pos =
            {
                {COMPONENT::BUFFER, {{{0.04, 0.48}}, {{0.9, 0.48}}}},
                {COMPONENT::NOT_GATE, {{{0.04, 0.48}}, {{0.9, 0.48}}}},
                {COMPONENT::AND_GATE, {{{0.12, 0.26}, {0.12, 0.7}}, {{0.9, 0.48}}}},
                {COMPONENT::OR_GATE, {{{0.08, 0.2}, {0.08, 0.76}}, {{0.86, 0.48}}}},
                {COMPONENT::XOR_GATE, {{{0.08, 0.2}, {0.08, 0.76}}, {{0.86, 0.48}}}},
                {COMPONENT::NAND_GATE, {{{0.04, 0.26}, {0.04, 0.7}}, {{0.92, 0.48}}}},
                {COMPONENT::NOR_GATE, {{{0.04, 0.2}, {0.04, 0.76}}, {{0.92, 0.48}}}},
                {COMPONENT::XNOR_GATE, {{{0.04, 0.2}, {0.04, 0.76}}, {{0.92, 0.48}}}},
                {COMPONENT::CONSTANT, {{}, {{0.84, 0.48}}}},
                {COMPONENT::SWITCH, {{}, {{0.84, 0.48}}}},
                {COMPONENT::OSCILLATOR, {{}, {{0.84, 0.48}}}},
                {COMPONENT::KEYPAD, {{}, {{0.96, 0.55}, {0.96, 0.65}, {0.96, 0.75}, {0.96, 0.85}}}},
                {COMPONENT::LED, {{{0.12, 0.48}}, {}}},
                {COMPONENT::_7SEG_5IN, {{{0.12, 0.44}, {0.12, 0.54}, {0.12, 0.63}, {0.12, 0.72}, {0.12, 0.82}}, {}}},
                {COMPONENT::_7SEG_8IN, {{{0.12, 0.16}, {0.12, 0.26}, {0.12, 0.35}, {0.12, 0.44}, {0.12, 0.54}, {0.12, 0.63}, {0.12, 0.72}, {0.12, 0.82}}, {}}},
                {COMPONENT::SRLATCH, {{{0.08, 0.22}, {0.08, 0.38}, {0.08, 0.76}}, {{0.88, 0.22}, {0.88, 0.76}}}},
                {COMPONENT::JKLATCH, {{{0.08, 0.22}, {0.08, 0.38}, {0.08, 0.76}}, {{0.88, 0.22}, {0.88, 0.76}}}},
                {COMPONENT::TLATCH, {{{0.08, 0.22}, {0.08, 0.76}}, {{0.88, 0.22}, {0.88, 0.76}}}},
                {COMPONENT::DLATCH, {{{0.08, 0.22}, {0.08, 0.76}}, {{0.88, 0.22}, {0.88, 0.76}}}},
                {COMPONENT::SRFLIPFLOP, {{{0.08, 0.22}, {0.08, 0.38}, {0.08, 0.76}}, {{0.88, 0.22}, {0.88, 0.76}}}},
                {COMPONENT::JKFLIPFLOP, {{{0.08, 0.22}, {0.08, 0.38}, {0.08, 0.76}}, {{0.88, 0.22}, {0.88, 0.76}}}},
                {COMPONENT::TFLIPFLOP, {{{0.08, 0.22}, {0.08, 0.76}}, {{0.88, 0.22}, {0.88, 0.76}}}},
                {COMPONENT::DFLIPFLOP, {{{0.08, 0.22}, {0.08, 0.76}}, {{0.88, 0.22}, {0.88, 0.76}}}},
            };

            enum LINE_TYPE
            {
                HORIZONTAL,
                VERTICAL
            };

            const int LINE_THICKNESS = 1;
            const int WIRE_THICKNESS = 2;
            const int WIRE_MARKING_THICKNESS = WIRE_THICKNESS/2;

            const std::vector<std::pair<double, double>> &getComponentIOPositionVector(COMPONENT comp_type, bool is_input);
            const std::pair<int, int> getComponentIORelativePos(ComponentLabel *component, bool is_input, int idx);

            QPixmap getBorder(int width, int height);
            QPixmap getLine(LINE_TYPE line_type, int size);
            QPixmap getWire(LINE_TYPE line_type, int size, bool on = false);
            QPixmap getWireMarking(LINE_TYPE line_type, int size, bool dir_ul = true);
            void load();
            void deallocate();
        }
    }
}

#endif //LOGICSIM_GUI_RESOURCE_LOADER_HPP
