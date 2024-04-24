#include "gui/main_window.hpp"
#include "ui_main_window.h"
#include <iostream>

namespace logicsim
{
    namespace gui
    {
        MainWindow::MainWindow(QWidget *parent) : QMainWindow{parent}, _ui(new Ui::MainWindow)
        {
            resources::load();

            _ui->setupUi(this);

            _tool_group = new QActionGroup(this);

            QObject::connect(_tool_group, &QActionGroup::triggered, [this, last_action = static_cast<QAction *>(_ui->actionSelect)](QAction* action) mutable
            {
                if (action == _ui->actionStart)
                {
                    if (last_action != nullptr)
                    {
                        last_action->setChecked(false);
                        last_action = nullptr;
                    }
                }
                else
                {
                    last_action = action;
                }
            });

            _insert_actions =
            {
                {_ui->actionBuffer, COMPONENT::BUFFER, -1},
                {_ui->actionNOT, COMPONENT::NOT_GATE, -1},
                {_ui->actionAND, COMPONENT::AND_GATE, -1},
                {_ui->actionOR, COMPONENT::OR_GATE, -1},
                {_ui->actionXOR, COMPONENT::XOR_GATE, -1},
                {_ui->actionNAND, COMPONENT::NAND_GATE, -1},
                {_ui->actionNOR, COMPONENT::NOR_GATE, -1},
                {_ui->actionXNOR, COMPONENT::XNOR_GATE, -1},
                {_ui->actionConstant_0, COMPONENT::CONSTANT, 0},
                {_ui->actionConstant_1, COMPONENT::CONSTANT, 1},
                {_ui->actionSwitch, COMPONENT::SWITCH, 0},
                {_ui->actionOscillator, COMPONENT::OSCILLATOR, -1},
                {_ui->actionKeypad, COMPONENT::KEYPAD, -1},
                {_ui->actionLED, COMPONENT::LED, -1},
                {_ui->action7_Segment_Display_5_input, COMPONENT::_7SEG_5IN, -1},
                {_ui->action7_Segment_Display_8_input, COMPONENT::_7SEG_8IN, -1},
                {_ui->actionSR_Latch, COMPONENT::SRLATCH, -1},
                {_ui->actionJK_Latch, COMPONENT::JKLATCH, -1},
                {_ui->actionT_Latch, COMPONENT::TLATCH, -1},
                {_ui->actionD_Latch, COMPONENT::DLATCH, -1},
                {_ui->actionSR_Flip_Flop, COMPONENT::SRFLIPFLOP, -1},
                {_ui->actionJK_Flip_Flop, COMPONENT::JKFLIPFLOP, -1},
                {_ui->actionT_Flip_Flop, COMPONENT::TFLIPFLOP, -1},
                {_ui->actionD_Flip_Flop, COMPONENT::DFLIPFLOP, -1},
            };

            // Design
            _tool_group->addAction(_ui->actionSelect);
            _ui->actionSelect->setChecked(true);
            QObject::connect(_ui->actionSelect, SIGNAL (triggered()), _ui->designArea, SLOT (setSelectMode()));

            _tool_group->addAction(_ui->actionWire);
            QObject::connect(_ui->actionWire, SIGNAL (triggered()), _ui->designArea, SLOT (setWireMode()));

            for (const auto &triplet : _insert_actions)
            {
                QAction *action = std::get<0>(triplet);
                _tool_group->addAction(action);
                action->setProperty("component-type", static_cast<int>(std::get<1>(triplet)));
                int res_idx = std::get<2>(triplet);
                if (res_idx != -1)
                {
                    action->setProperty("resource-idx", res_idx);
                }
                QObject::connect(action, SIGNAL (triggered()), _ui->designArea, SLOT (setInsertMode()));
            }

            // Simulation
            _tool_group->addAction(_ui->actionStart);
            QObject::connect(_ui->actionStart, SIGNAL (triggered()), _ui->designArea, SLOT (setSimulationMode()));
            QObject::connect(_ui->actionStart, SIGNAL (triggered()), this, SLOT (setSimulationMenu()));

            QObject::connect(_ui->actionStop, SIGNAL (triggered()), _ui->designArea, SLOT (stopSimulationMode()));
            QObject::connect(_ui->actionStop, SIGNAL (triggered()), this, SLOT (setDesignMenu()));

            QObject::connect(_ui->actionPause, SIGNAL (triggered()), _ui->designArea, SLOT (pauseSimulation()));
            QObject::connect(_ui->actionPause, SIGNAL (triggered()), this, SLOT (enableContinue()));

            QObject::connect(_ui->actionStep, SIGNAL (triggered()), _ui->designArea, SLOT (stepSimulation()));

            QObject::connect(_ui->actionContinue, SIGNAL (triggered()), _ui->designArea, SLOT (continueSimulation()));
            QObject::connect(_ui->actionContinue, SIGNAL (triggered()), this, SLOT (enablePause()));

            QObject::connect(_ui->actionReset, SIGNAL (triggered()), _ui->designArea, SLOT (resetSimulation()));

            _ui->designArea->setStatusBar(_ui->statusbar);

        }

        MainWindow::~MainWindow()
        {
            resources::deallocate();
            delete _ui;
        }

        void MainWindow::setSimulationMenu()
        {
            _sim_paused = false;
            _setSimulationMenu(true);
        }

        void MainWindow::setDesignMenu()
        {
            _setSimulationMenu(false);
            _ui->actionSelect->setChecked(true);
            emit _ui->actionSelect->triggered();
        }

        void MainWindow::_setSimulationMenu(bool enabled)
        {
            _ui->actionSelect->setEnabled(!enabled);
            _ui->actionWire->setEnabled(!enabled);
            for (const auto &triplet : _insert_actions)
            {
                std::get<0>(triplet)->setEnabled(!enabled);
            }

            _ui->actionStop->setEnabled(enabled);
            if (_sim_paused)
            {
                _ui->actionStep->setEnabled(enabled);
                _ui->actionContinue->setEnabled(enabled);
            }
            else
            {
                _ui->actionPause->setEnabled(enabled);
            }
            _ui->actionReset->setEnabled(enabled);

            _ui->actionStart->setEnabled(!enabled);
        }

        void MainWindow::enableContinue()
        {
            _ui->actionContinue->setEnabled(true);
            _ui->actionStep->setEnabled(true);
            _ui->actionPause->setEnabled(false);
            _sim_paused = true;
        }

        void MainWindow::enablePause()
        {
            _ui->actionPause->setEnabled(true);
            _ui->actionStep->setEnabled(false);
            _ui->actionContinue->setEnabled(false);
            _sim_paused = false;
        }
    }
}
