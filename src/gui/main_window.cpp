#include "gui/main_window.hpp"
#include "ui_main_window.h"

namespace logicsim
{
    namespace gui
    {
        MainWindow::MainWindow(QWidget *parent) : QMainWindow{parent}, _ui(new Ui::MainWindow)
        {
            resources::load();

            _ui->setupUi(this);

            // group tools for exclusive selection
            QActionGroup *tool_group = new QActionGroup(this);

            // Design
            tool_group->addAction(_ui->actionSelect);
            _ui->actionSelect->setChecked(true);
            QObject::connect(_ui->actionSelect, SIGNAL (triggered()), _ui->designArea, SLOT (setSelectMode()));

            tool_group->addAction(_ui->actionWire);
            QObject::connect(_ui->actionWire, SIGNAL (triggered()), _ui->designArea, SLOT (setWireMode()));

            tool_group->addAction(_ui->actionNOT);
            _ui->actionNOT->setProperty("component-type", static_cast<int>(COMPONENT::NOT_GATE));
            QObject::connect(_ui->actionNOT, SIGNAL (triggered()), _ui->designArea, SLOT (setInsertMode()));

            tool_group->addAction(_ui->actionAND);
            _ui->actionAND->setProperty("component-type", static_cast<int>(COMPONENT::AND_GATE));
            QObject::connect(_ui->actionAND, SIGNAL (triggered()), _ui->designArea, SLOT (setInsertMode()));

            tool_group->addAction(_ui->actionOR);
            _ui->actionOR->setProperty("component-type", static_cast<int>(COMPONENT::OR_GATE));
            QObject::connect(_ui->actionOR, SIGNAL (triggered()), _ui->designArea, SLOT (setInsertMode()));

            tool_group->addAction(_ui->actionXOR);
            _ui->actionXOR->setProperty("component-type", static_cast<int>(COMPONENT::XOR_GATE));
            QObject::connect(_ui->actionXOR, SIGNAL (triggered()), _ui->designArea, SLOT (setInsertMode()));

            tool_group->addAction(_ui->actionNAND);
            _ui->actionNAND->setProperty("component-type", static_cast<int>(COMPONENT::NAND_GATE));
            QObject::connect(_ui->actionNAND, SIGNAL (triggered()), _ui->designArea, SLOT (setInsertMode()));

            tool_group->addAction(_ui->actionNOR);
            _ui->actionNOR->setProperty("component-type", static_cast<int>(COMPONENT::NOR_GATE));
            QObject::connect(_ui->actionNOR, SIGNAL (triggered()), _ui->designArea, SLOT (setInsertMode()));

            tool_group->addAction(_ui->actionXNOR);
            _ui->actionXNOR->setProperty("component-type", static_cast<int>(COMPONENT::XNOR_GATE));
            QObject::connect(_ui->actionXNOR, SIGNAL (triggered()), _ui->designArea, SLOT (setInsertMode()));

            tool_group->addAction(_ui->actionConstant_0);
            _ui->actionConstant_0->setProperty("component-type", static_cast<int>(COMPONENT::CONSTANT));
            _ui->actionConstant_0->setProperty("resource-idx", 0);
            QObject::connect(_ui->actionConstant_0, SIGNAL (triggered()), _ui->designArea, SLOT (setInsertMode()));

            tool_group->addAction(_ui->actionConstant_1);
            _ui->actionConstant_1->setProperty("component-type", static_cast<int>(COMPONENT::CONSTANT));
            _ui->actionConstant_1->setProperty("resource-idx", 1);
            QObject::connect(_ui->actionConstant_1, SIGNAL (triggered()), _ui->designArea, SLOT (setInsertMode()));

            tool_group->addAction(_ui->actionSwitch);
            _ui->actionSwitch->setProperty("component-type", static_cast<int>(COMPONENT::SWITCH));
            _ui->actionSwitch->setProperty("resource-idx", 0);
            QObject::connect(_ui->actionSwitch, SIGNAL (triggered()), _ui->designArea, SLOT (setInsertMode()));

            tool_group->addAction(_ui->actionOscillator);
            _ui->actionOscillator->setProperty("component-type", static_cast<int>(COMPONENT::OSCILLATOR));
            QObject::connect(_ui->actionOscillator, SIGNAL (triggered()), _ui->designArea, SLOT (setInsertMode()));

            tool_group->addAction(_ui->actionKeypad);
            _ui->actionKeypad->setProperty("component-type", static_cast<int>(COMPONENT::KEYPAD));
            QObject::connect(_ui->actionKeypad, SIGNAL (triggered()), _ui->designArea, SLOT (setInsertMode()));

            tool_group->addAction(_ui->actionLED);
            _ui->actionLED->setProperty("component-type", static_cast<int>(COMPONENT::LED));
            QObject::connect(_ui->actionLED, SIGNAL (triggered()), _ui->designArea, SLOT (setInsertMode()));

            tool_group->addAction(_ui->action7_Segment_Display_5_input);
            _ui->action7_Segment_Display_5_input->setProperty("component-type", static_cast<int>(COMPONENT::_7SEG_5IN));
            QObject::connect(_ui->action7_Segment_Display_5_input, SIGNAL (triggered()), _ui->designArea, SLOT (setInsertMode()));

            tool_group->addAction(_ui->action7_Segment_Display_8_input);
            _ui->action7_Segment_Display_8_input->setProperty("component-type", static_cast<int>(COMPONENT::_7SEG_8IN));
            QObject::connect(_ui->action7_Segment_Display_8_input, SIGNAL (triggered()), _ui->designArea, SLOT (setInsertMode()));

            tool_group->addAction(_ui->actionSR_Latch);
            _ui->actionSR_Latch->setProperty("component-type", static_cast<int>(COMPONENT::SRLATCH));
            QObject::connect(_ui->actionSR_Latch, SIGNAL (triggered()), _ui->designArea, SLOT (setInsertMode()));

            tool_group->addAction(_ui->actionJK_Latch);
            _ui->actionJK_Latch->setProperty("component-type", static_cast<int>(COMPONENT::JKLATCH));
            QObject::connect(_ui->actionJK_Latch, SIGNAL (triggered()), _ui->designArea, SLOT (setInsertMode()));

            tool_group->addAction(_ui->actionT_Latch);
            _ui->actionT_Latch->setProperty("component-type", static_cast<int>(COMPONENT::TLATCH));
            QObject::connect(_ui->actionT_Latch, SIGNAL (triggered()), _ui->designArea, SLOT (setInsertMode()));

            tool_group->addAction(_ui->actionD_Latch);
            _ui->actionD_Latch->setProperty("component-type", static_cast<int>(COMPONENT::DLATCH));
            QObject::connect(_ui->actionD_Latch, SIGNAL (triggered()), _ui->designArea, SLOT (setInsertMode()));

            tool_group->addAction(_ui->actionSR_Flip_Flop);
            _ui->actionSR_Flip_Flop->setProperty("component-type", static_cast<int>(COMPONENT::SRFLIPFLOP));
            QObject::connect(_ui->actionSR_Flip_Flop, SIGNAL (triggered()), _ui->designArea, SLOT (setInsertMode()));

            tool_group->addAction(_ui->actionJK_Flip_Flop);
            _ui->actionJK_Flip_Flop->setProperty("component-type", static_cast<int>(COMPONENT::JKFLIPFLOP));
            QObject::connect(_ui->actionJK_Flip_Flop, SIGNAL (triggered()), _ui->designArea, SLOT (setInsertMode()));

            tool_group->addAction(_ui->actionT_Flip_Flop);
            _ui->actionT_Flip_Flop->setProperty("component-type", static_cast<int>(COMPONENT::TFLIPFLOP));
            QObject::connect(_ui->actionT_Flip_Flop, SIGNAL (triggered()), _ui->designArea, SLOT (setInsertMode()));

            tool_group->addAction(_ui->actionD_Flip_Flop);
            _ui->actionD_Flip_Flop->setProperty("component-type", static_cast<int>(COMPONENT::DFLIPFLOP));
            QObject::connect(_ui->actionD_Flip_Flop, SIGNAL (triggered()), _ui->designArea, SLOT (setInsertMode()));

            // Simulation
            // TODO: uncheck tool group action when selecting this
            QObject::connect(_ui->actionStart, SIGNAL (triggered()), _ui->designArea, SLOT (setSimulationMode()));

        }

        MainWindow::~MainWindow()
        {
            resources::deallocate();
            delete _ui;
        }

    }
}
