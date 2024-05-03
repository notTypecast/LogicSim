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
                return is_input ? comp_io_rel_pos.at(comp_type).first : comp_io_rel_pos.at(comp_type).second;
            }

            const std::pair<int, int> getComponentIORelativePos(ComponentLabel *component, bool is_input, int idx)
            {
                if (is_input)
                {
                    return {
                        static_cast<int>(comp_io_rel_pos.at(component->comp_type()).first[idx].first * component->width()),
                        static_cast<int>(comp_io_rel_pos.at(component->comp_type()).first[idx].second * component->height())
                    };
                }
                else
                {
                    return {
                        static_cast<int>(comp_io_rel_pos.at(component->comp_type()).second[idx].first * component->width()),
                        static_cast<int>(comp_io_rel_pos.at(component->comp_type()).second[idx].second * component->height())
                    };
                }
            }

            QPixmap getComponentResource(COMPONENT comp_type, int res_idx, double scale)
            {
                QPixmap &res = resources::comp_images.at(comp_type)[res_idx];
                return res.scaled(static_cast<int>(scale * res.width()), static_cast<int>(scale * res.height()));
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

            QPixmap getWire(LINE_TYPE line_type, int size, double scale, bool on)
            {
                switch (line_type)
                {
                case HORIZONTAL:
                default:
                {
                    QPixmap *wire = on ? hwire_on : hwire;
                    return wire->scaled(size, static_cast<int>(scale * WIRE_THICKNESS));
                }
                case VERTICAL:
                {
                    QPixmap *wire = on ? vwire_on : vwire;
                    return wire->scaled(static_cast<int>(scale * WIRE_THICKNESS), size);
                }
                }
            }

            QPixmap getWireMarking(LINE_TYPE line_type, int size, double scale, bool dir_ul)
            {
                switch (line_type)
                {
                case HORIZONTAL:
                default:
                {
                    QPixmap *pixmap = dir_ul ? hwire_up : hwire_down;
                    return pixmap->scaled(size, static_cast<int>(scale * WIRE_MARKING_THICKNESS), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                }
                case VERTICAL:
                {
                    QPixmap *pixmap = dir_ul ? vwire_left : vwire_right;
                    return pixmap->scaled(static_cast<int>(scale * WIRE_MARKING_THICKNESS), size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                }
                }
            }

            void load()
            {
#ifndef QT_DEBUG
                IMG_PATH = QApplication::applicationDirPath() + "/res/";
#endif

                comp_images =
                {
                    {COMPONENT::BUFFER, {QPixmap(IMG_PATH + "BUFFER.png")}},
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
                    {COMPONENT::LED, {QPixmap(IMG_PATH + "LED0.png"), QPixmap(IMG_PATH + "LED1.png")}},
                    {COMPONENT::SRLATCH, {QPixmap(IMG_PATH + "SRLATCH.png")}},
                    {COMPONENT::JKLATCH, {QPixmap(IMG_PATH + "JKLATCH.png")}},
                    {COMPONENT::TLATCH, {QPixmap(IMG_PATH + "TLATCH.png")}},
                    {COMPONENT::DLATCH, {QPixmap(IMG_PATH + "DLATCH.png")}},
                    {COMPONENT::SRFLIPFLOP, {QPixmap(IMG_PATH + "SRFF.png")}},
                    {COMPONENT::JKFLIPFLOP, {QPixmap(IMG_PATH + "JKFF.png")}},
                    {COMPONENT::TFLIPFLOP, {QPixmap(IMG_PATH + "TFF.png")}},
                    {COMPONENT::DFLIPFLOP, {QPixmap(IMG_PATH + "DFF.png")}},
                };

                comp_images[COMPONENT::_7SEG_5IN] = std::vector<QPixmap>(33);
                comp_images[COMPONENT::_7SEG_5IN][0] = QPixmap(IMG_PATH + "7SEG5IN.png");

                for (int i = 1; i < 33; ++i)
                {
                    comp_images[COMPONENT::_7SEG_5IN][i] = QPixmap(IMG_PATH + "7SEG5IN_" + QString::fromStdString(std::bitset<5>(i - 1).to_string()) + ".png");
                }

                comp_images[COMPONENT::_7SEG_8IN] = std::vector<QPixmap>(256);
                comp_images[COMPONENT::_7SEG_8IN][0] = QPixmap(IMG_PATH + "7SEG8IN.png");

                for (int i = 1; i < 256; ++i)
                {
                    comp_images[COMPONENT::_7SEG_8IN][i] = QPixmap(IMG_PATH + "7SEG8IN_" + QString::fromStdString(std::bitset<8>(i).to_string()) + ".png");
                }

                comp_images[COMPONENT::KEYPAD] = std::vector<QPixmap>(17);
                comp_images[COMPONENT::KEYPAD][0] = QPixmap(IMG_PATH + "KEYPAD.png");

                for (int i = 1; i < 17; ++i)
                {
                    std::stringstream ss;
                    ss << "KEYPAD" << std::hex << std::uppercase << i - 1 << ".png";
                    comp_images[COMPONENT::KEYPAD][i] = QPixmap(IMG_PATH + QString::fromStdString(ss.str()));
                }

                border = new QPixmap(IMG_PATH + "border.png");
                hselline = new QPixmap(IMG_PATH + "hselline.png");
                vselline = new QPixmap(IMG_PATH + "vselline.png");
                hwire = new QPixmap(IMG_PATH + "hwire.png");
                vwire = new QPixmap(IMG_PATH + "vwire.png");
                hwire_up = new QPixmap(IMG_PATH + "hwire_mark_up.png");
                hwire_down = new QPixmap(IMG_PATH + "hwire_mark_down.png");
                vwire_left = new QPixmap(IMG_PATH + "vwire_mark_left.png");
                vwire_right = new QPixmap(IMG_PATH + "vwire_mark_right.png");
                hwire_on = new QPixmap(IMG_PATH + "hwire_on.png");
                vwire_on = new QPixmap(IMG_PATH + "vwire_on.png");

                select_icon = new QIcon(IMG_PATH + "select_icon.png");
                move_icon = new QIcon(IMG_PATH + "move_icon.png");
                wire_icon = new QIcon(IMG_PATH + "wire_icon.png");
                wire_remove_icon = new QIcon(IMG_PATH + "wire_remove_icon.png");

                comp_icons =
                {
                    {COMPONENT::BUFFER, {new QIcon(IMG_PATH + "BUFFER.png")}},
                    {COMPONENT::NOT_GATE, {new QIcon(IMG_PATH + "NOT.png")}},
                    {COMPONENT::AND_GATE, {new QIcon(IMG_PATH + "AND.png")}},
                    {COMPONENT::OR_GATE, {new QIcon(IMG_PATH + "OR.png")}},
                    {COMPONENT::XOR_GATE, {new QIcon(IMG_PATH + "XOR.png")}},
                    {COMPONENT::NAND_GATE, {new QIcon(IMG_PATH + "NAND.png")}},
                    {COMPONENT::NOR_GATE, {new QIcon(IMG_PATH + "NOR.png")}},
                    {COMPONENT::XNOR_GATE, {new QIcon(IMG_PATH + "XNOR.png")}},
                    {COMPONENT::CONSTANT, {new QIcon(IMG_PATH + "CONSTANT0.png"), new QIcon(IMG_PATH + "CONSTANT1.png")}},
                    {COMPONENT::SWITCH, {new QIcon(IMG_PATH + "SWITCH0.png")}},
                    {COMPONENT::OSCILLATOR, {new QIcon(IMG_PATH + "OSCILLATOR.png")}},
                    {COMPONENT::KEYPAD, {new QIcon(IMG_PATH + "KEYPAD.png")}},
                    {COMPONENT::LED, {new QIcon(IMG_PATH + "LED0.png")}},
                    {COMPONENT::_7SEG_5IN, {new QIcon(IMG_PATH + "7SEG5IN.png")}},
                    {COMPONENT::_7SEG_8IN, {new QIcon(IMG_PATH + "7SEG8IN.png")}},
                    {COMPONENT::SRLATCH, {new QIcon(IMG_PATH + "SRLATCH.png")}},
                    {COMPONENT::JKLATCH, {new QIcon(IMG_PATH + "JKLATCH.png")}},
                    {COMPONENT::TLATCH, {new QIcon(IMG_PATH + "TLATCH.png")}},
                    {COMPONENT::DLATCH, {new QIcon(IMG_PATH + "DLATCH.png")}},
                    {COMPONENT::SRFLIPFLOP, {new QIcon(IMG_PATH + "SRFF.png")}},
                    {COMPONENT::JKFLIPFLOP, {new QIcon(IMG_PATH + "JKFF.png")}},
                    {COMPONENT::TFLIPFLOP, {new QIcon(IMG_PATH + "TFF.png")}},
                    {COMPONENT::DFLIPFLOP, {new QIcon(IMG_PATH + "DFF.png")}},
                };

                start_sim_icon = new QIcon(IMG_PATH + "start_sim_icon.png");
                stop_sim_icon = new QIcon(IMG_PATH + "stop_sim_icon.png");
                pause_sim_icon = new QIcon(IMG_PATH + "pause_sim_icon.png");
                step_sim_icon = new QIcon(IMG_PATH + "step_sim_icon.png");
                reset_sim_icon = new QIcon(IMG_PATH + "reset_sim_icon.png");
            }

            void deallocate()
            {
                delete border;
                delete hselline;
                delete vselline;
                delete hwire;
                delete vwire;
                delete hwire_up;
                delete hwire_down;
                delete vwire_left;
                delete vwire_right;
                delete hwire_on;
                delete vwire_on;
                delete select_icon;
                delete move_icon;
                delete wire_icon;
                delete wire_remove_icon;

                for (const auto &p : comp_icons)
                {
                    for (const auto &icon : p.second)
                    {
                        delete icon;
                    }
                }

                delete start_sim_icon;
            }
        }
    }
}
