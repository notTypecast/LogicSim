#include "gui/resource_loader.hpp"

namespace logicsim
{
    namespace gui
    {
        namespace resources
        {
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
                    {COMPONENT::NOT_GATE, QPixmap(IMG_PATH + "NOT.png")},
                    {COMPONENT::AND_GATE, QPixmap(IMG_PATH + "AND.png")},
                    {COMPONENT::OR_GATE, QPixmap(IMG_PATH + "OR.png")},
                    {COMPONENT::XOR_GATE, QPixmap(IMG_PATH + "XOR.png")},
                    {COMPONENT::NAND_GATE, QPixmap(IMG_PATH + "NAND.png")},
                    {COMPONENT::NOR_GATE, QPixmap(IMG_PATH + "NOR.png")},
                    {COMPONENT::XNOR_GATE, QPixmap(IMG_PATH + "XNOR.png")},
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
                    {COMPONENT::XNOR_GATE, {{{0.04, 0.2}, {0.04, 0.76}}, {{0.92, 0.48}}}}
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
