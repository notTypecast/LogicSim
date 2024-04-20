#include "gui/resource_loader.hpp"
#include "gui/component_label.hpp"

namespace logicsim
{
    namespace gui
    {
        namespace resources
        {
            const std::vector<std::pair<double, double>> &getComponentIOPositionVector(COMPONENT comp_type, bool is_input)
            {
                return is_input ? comp_io_rel_pos[comp_type].first : comp_io_rel_pos[comp_type].second;
            }

            const std::pair<int, int> getComponentIORelativePos(ComponentLabel *component, bool is_input, int idx)
            {
                if (is_input)
                {
                    return {
                        static_cast<int>(comp_io_rel_pos[component->comp_type()].first[idx].first * component->width()),
                        static_cast<int>(comp_io_rel_pos[component->comp_type()].first[idx].second * component->height())
                    };
                }
                else
                {
                    return {
                        static_cast<int>(comp_io_rel_pos[component->comp_type()].second[idx].first * component->width()),
                        static_cast<int>(comp_io_rel_pos[component->comp_type()].second[idx].second * component->height())
                    };
                }
            }

            QPixmap getBorder(int width, int height)
            {
                return border->scaled(width, height);
            }

            QPixmap getLine(LINE_TYPE line_type, int size)
            {
                switch (line_type)
                {
                case HORIZONTAL:
                default:
                    return hselline->scaled(size, LINE_THICKNESS);
                case VERTICAL:
                    return vselline->scaled(LINE_THICKNESS, size);
                }
            }

            QPixmap getWire(LINE_TYPE line_type, int size)
            {
                switch (line_type)
                {
                case HORIZONTAL:
                default:
                    return hwire->scaled(size, WIRE_THICKNESS);
                case VERTICAL:
                    return vwire->scaled(WIRE_THICKNESS, size);
                }
            }

            void load()
            {
                comp_images =
                {
                    {COMPONENT::NOT_GATE, {QPixmap(IMG_PATH + "NOT.png")}},
                    {COMPONENT::AND_GATE, {QPixmap(IMG_PATH + "AND.png")}},
                    {COMPONENT::OR_GATE, {QPixmap(IMG_PATH + "OR.png")}},
                    {COMPONENT::XOR_GATE, {QPixmap(IMG_PATH + "XOR.png")}},
                    {COMPONENT::NAND_GATE, {QPixmap(IMG_PATH + "NAND.png")}},
                    {COMPONENT::NOR_GATE, {QPixmap(IMG_PATH + "NOR.png")}},
                    {COMPONENT::XNOR_GATE, {QPixmap(IMG_PATH + "XNOR.png")}},
                    {COMPONENT::CONSTANT, {QPixmap(IMG_PATH + "CONSTANT0.png"), QPixmap(IMG_PATH + "CONSTANT1.png")}},
                    {COMPONENT::SWITCH, {QPixmap(IMG_PATH + "SWITCH0.png"), QPixmap(IMG_PATH + "SWITCH1.png")}},
                    {COMPONENT::OSCILLATOR, {QPixmap(IMG_PATH + "OSCILLATOR.png")}},
                    {COMPONENT::KEYPAD, {QPixmap(IMG_PATH + "KEYPAD.png")}},
                    {COMPONENT::LED, {QPixmap(IMG_PATH + "LED0.png")}},
                    {COMPONENT::_7SEG_5IN, {QPixmap(IMG_PATH + "7SEG5IN.png")}},
                    {COMPONENT::_7SEG_8IN, {QPixmap(IMG_PATH + "7SEG8IN.png")}},
                    {COMPONENT::SRLATCH, {QPixmap(IMG_PATH + "SRLATCH.png")}},
                    {COMPONENT::JKLATCH, {QPixmap(IMG_PATH + "JKLATCH.png")}},
                    {COMPONENT::TLATCH, {QPixmap(IMG_PATH + "TLATCH.png")}},
                    {COMPONENT::DLATCH, {QPixmap(IMG_PATH + "DLATCH.png")}},
                    {COMPONENT::SRFLIPFLOP, {QPixmap(IMG_PATH + "SRFF.png")}},
                    {COMPONENT::JKFLIPFLOP, {QPixmap(IMG_PATH + "JKFF.png")}},
                    {COMPONENT::TFLIPFLOP, {QPixmap(IMG_PATH + "TFF.png")}},
                    {COMPONENT::DFLIPFLOP, {QPixmap(IMG_PATH + "DFF.png")}},
                };

                border = new QPixmap(IMG_PATH + "border.png");
                hselline = new QPixmap(IMG_PATH + "hselline.png");
                vselline = new QPixmap(IMG_PATH + "vselline.png");
                hwire = new QPixmap(IMG_PATH + "hwire.png");
                vwire = new QPixmap(IMG_PATH + "vwire.png");

                comp_io_rel_pos =
                {
                    {COMPONENT::NOT_GATE, {{{0.04, 0.48}}, {{0.9, 0.48}}}},
                    {COMPONENT::AND_GATE, {{{0.12, 0.26}, {0.12, 0.7}}, {{0.9, 0.48}}}},
                    {COMPONENT::OR_GATE, {{{0.08, 0.2}, {0.08, 0.76}}, {{0.86, 0.48}}}},
                    {COMPONENT::XOR_GATE, {{{0.08, 0.2}, {0.08, 0.76}}, {{0.86, 0.48}}}},
                    {COMPONENT::NAND_GATE, {{{0.04, 0.26}, {0.04, 0.7}}, {{0.92, 0.48}}}},
                    {COMPONENT::NOR_GATE, {{{0.04, 0.2}, {0.04, 0.76}}, {{0.92, 0.48}}}},
                    {COMPONENT::XNOR_GATE, {{{0.04, 0.2}, {0.04, 0.76}}, {{0.92, 0.48}}}},
                    {COMPONENT::CONSTANT, {{}, {}}}, // TODO: remake graphics for constant
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
            }

            void deallocate()
            {
                delete border;
                delete hselline;
                delete vselline;
                delete hwire;
                delete vwire;
            }
        }
    }
}
