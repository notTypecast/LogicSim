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

            // initialize status bar and pass to tab handler
            QLabel *ticks_label = new QLabel(_ui->statusbar);
            ticks_label->setObjectName("perm-label");
            _ui->statusbar->addPermanentWidget(ticks_label);
            _ui->tabHandler->setStatusBar(_ui->statusbar);

            // handle menu changes (action enable/disable logic)
            QObject::connect(_ui->tabHandler, SIGNAL (designToolChanged(TOOL, COMPONENT)), this, SLOT (setLastDesignTool(TOOL, COMPONENT)));
            QObject::connect(_ui->tabHandler, SIGNAL (designTabChosen()), this, SLOT (setDesignMenu()));
            QObject::connect(_ui->tabHandler, SIGNAL (simulationTabChosen(bool)), this, SLOT (setSimulationMenu(bool)));

            // File menu
            _ui->actionNew->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
            QObject::connect(_ui->actionNew, SIGNAL (triggered()), _ui->tabHandler, SLOT (addDesignArea()));

            _ui->actionOpen->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
            QObject::connect(_ui->actionOpen, SIGNAL (triggered()), _ui->tabHandler, SLOT (openFile()));

            _ui->actionClose->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
            QObject::connect(_ui->actionClose, SIGNAL (triggered()), _ui->tabHandler, SLOT (removeDesignArea()));

            _ui->actionSave->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
            QObject::connect(_ui->actionSave, SIGNAL (triggered()), _ui->tabHandler, SLOT (saveFile()));

            _ui->actionSave_As->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S));
            QObject::connect(_ui->actionSave_As, SIGNAL (triggered()), _ui->tabHandler, SLOT (saveFileAs()));

            // TODO: map to function that closes all tabs (thus prompting to save all unsaved tabs)
            QObject::connect(_ui->actionExit, &QAction::triggered, &QApplication::quit);

            // group tools for exclusive seleciton
            _tool_group = new QActionGroup(this);

            // remove action check from simulation actions
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

            // Design actions
            _tool_group->addAction(_ui->actionSelect);
            _ui->actionSelect->setChecked(true);
            QObject::connect(_ui->actionSelect, SIGNAL (triggered()), _ui->tabHandler, SLOT (setSelectMode()));

            _tool_group->addAction(_ui->actionWire);
            QObject::connect(_ui->actionWire, SIGNAL (triggered()), _ui->tabHandler, SLOT (setWireMode()));

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
                QObject::connect(action, SIGNAL (triggered()), _ui->tabHandler, SLOT (setInsertMode()));
            }

            // Simulation actions
            _tool_group->addAction(_ui->actionStart);
            QObject::connect(_ui->actionStart, SIGNAL (triggered()), this, SLOT (setSimulationMenu()));

            QObject::connect(_ui->actionStop, SIGNAL (triggered()), _ui->tabHandler, SLOT (stopSimulationMode()));
            QObject::connect(_ui->actionStop, SIGNAL (triggered()), this, SLOT (setDesignMenu()));

            QObject::connect(_ui->actionPause, SIGNAL (triggered()), _ui->tabHandler, SLOT (pauseSimulation()));
            QObject::connect(_ui->actionPause, SIGNAL (triggered()), this, SLOT (enableContinue()));

            QObject::connect(_ui->actionStep, SIGNAL (triggered()), _ui->tabHandler, SLOT (stepSimulation()));

            QObject::connect(_ui->actionContinue, SIGNAL (triggered()), _ui->tabHandler, SLOT (continueSimulation()));
            QObject::connect(_ui->actionContinue, SIGNAL (triggered()), this, SLOT (enablePause()));

            QObject::connect(_ui->actionReset, SIGNAL (triggered()), _ui->tabHandler, SLOT (resetSimulation()));

            QObject::connect(_ui->actionProperties, SIGNAL (triggered()), this, SLOT (simulationProperties()));

        }

        MainWindow::~MainWindow()
        {
            resources::deallocate();
            delete _ui;
        }

        void MainWindow::setLastDesignTool(TOOL tool, COMPONENT comp_type)
        {
            _last_design_tool = tool;
            _last_insert_comp = comp_type;
        }

        void MainWindow::addDesignArea()
        {
            _ui->tabHandler->addDesignArea();
        }

        void MainWindow::setSimulationMenu(bool running)
        {
            if (!_ui->tabHandler->setSimulationMode())
            {
                return;
            }
            _sim_paused = !running;
            _setSimulationMenu(true);
        }

        void MainWindow::setDesignMenu()
        {
            _setSimulationMenu(false);
            switch (_last_design_tool)
            {
            case INSERT:
                for (const auto &triplet: _insert_actions)
                {
                    if (std::get<1>(triplet) == _last_insert_comp)
                    {
                        QAction *action = std::get<0>(triplet);
                        action->setChecked(true);
                        emit action->triggered();
                        break;
                    }
                }
                break;
            case WIRE:
                _ui->actionWire->setChecked(true);
                emit _ui->actionWire->triggered();
                break;
            case SELECT:
            default:
                _ui->actionSelect->setChecked(true);
                emit _ui->actionSelect->triggered();
                break;
            }
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
                _ui->actionPause->setEnabled(!enabled);
            }
            else
            {
                _ui->actionStep->setEnabled(!enabled);
                _ui->actionContinue->setEnabled(!enabled);
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

        void MainWindow::simulationProperties()
        {
            Properties *sim_properties_popup = new Properties("Simulation Properties", this);

            sim_properties_popup->addValueEntry("Frequency", QString::number(_ui->tabHandler->currentDesignArea()->frequency()), [](QLineEdit *entry) {
                bool ok;
                int val = entry->text().toInt(&ok);
                if (!ok || val <= 0)
                {
                    return QString();
                }
                if (val > 1000)
                {
                    return QString("1000");
                }
                return entry->text();
            }, "Hz");

            QObject::connect(sim_properties_popup, SIGNAL (optionValue(QString, int)), this, SLOT (setSimulationFrequency(QString)));

            QSize popup_size = sim_properties_popup->sizeHint();
            sim_properties_popup->move(x() + width()/2 - popup_size.width()/2, y() + height()/2 - popup_size.height()/2);
            sim_properties_popup->show();
        }

        void MainWindow::setSimulationFrequency(QString freq)
        {
            _ui->tabHandler->currentDesignArea()->setFrequency(freq.toInt());
        }
    }
}
