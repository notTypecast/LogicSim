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
                    return hselline->scaled(size, LINE_THICKNESS);
                case VERTICAL:
                    return vselline->scaled(LINE_THICKNESS, size);
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
            }

            void deallocate()
            {
                delete border;
                delete hselline;
                delete vselline;
            }
        }
    }
}
