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

            _settings = new QSettings("notTypecast", "LogicSim");

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
                {_ui->actionButton, COMPONENT::BUTTON, 0},
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

            showMaximized();

            // initialize status bar and pass to tab handler
            QLabel *ticks_label = new QLabel(_ui->statusbar);
            ticks_label->setObjectName("perm-label");
            _ui->statusbar->addPermanentWidget(ticks_label);
            _ui->tabHandler->setStatusBar(_ui->statusbar);

            // handle menu changes (action enable/disable logic)
            QObject::connect(_ui->tabHandler, SIGNAL (designToolChanged(TOOL, COMPONENT, int)), this, SLOT (setLastDesignTool(TOOL, COMPONENT, int)));
            QObject::connect(_ui->tabHandler, SIGNAL (designTabChosen()), this, SLOT (setDesignMenu()));
            QObject::connect(_ui->tabHandler, &TabHandler::simulationTabChosen, [this](bool running) {
                // if triggered by simulationTabChosen, simulation is already running
                // so don't re-apply
                setSimulationMenu(running, true);
            });
            QObject::connect(_ui->tabHandler, SIGNAL (undoActionPerformed(bool, bool)), this, SLOT (setUndoActionState(bool, bool)));
            QObject::connect(_ui->tabHandler, SIGNAL (selectionActionPerformed(bool, bool)), this, SLOT (setSelectActionState(bool, bool)));

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

            QObject::connect(_ui->actionExit, SIGNAL (triggered()), this, SLOT (quit()));

            // Edit menu
            _ui->actionUndo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));
            QObject::connect(_ui->actionUndo, SIGNAL (triggered()), _ui->tabHandler, SLOT (undoAction()));

            _ui->actionRedo->setShortcuts({QKeySequence(Qt::CTRL + Qt::Key_Y), QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Z)});
            QObject::connect(_ui->actionRedo, SIGNAL (triggered()), _ui->tabHandler, SLOT (redoAction()));

            _ui->actionCut->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_X));
            QObject::connect(_ui->actionCut, SIGNAL (triggered()), _ui->tabHandler, SLOT (cutAction()));

            _ui->actionCopy->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
            QObject::connect(_ui->actionCopy, SIGNAL (triggered()), _ui->tabHandler, SLOT (copyAction()));

            _ui->actionPaste->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_V));
            QObject::connect(_ui->actionPaste, SIGNAL (triggered()), _ui->tabHandler, SLOT (pasteAction()));

            _ui->actionDelete->setShortcut(QKeySequence(Qt::Key_Delete));
            QObject::connect(_ui->actionDelete, SIGNAL (triggered()), _ui->tabHandler, SLOT (deleteAction()));

            // View menu
            QObject::connect(_ui->actionToolbar, SIGNAL (triggered()), this, SLOT (toggleToolbar()));
            if (!_settings->value("toolbar", true).toBool())
            {
                _ui->actionToolbar->trigger();
            }

            QObject::connect(_ui->actionWire_Color, &QAction::triggered, [this]()
            {
                _settings->setValue("wire-color", _ui->actionWire_Color->isChecked());
                _ui->tabHandler->toggleWireColor();
            });
            if (_settings->value("wire-color", false).toBool())
            {
                _ui->actionWire_Color->trigger();
            }

            QObject::connect(_ui->actionZoom_In, SIGNAL (triggered()), _ui->tabHandler, SLOT (zoomIn()));

            QObject::connect(_ui->actionZoom_Out, SIGNAL (triggered()), _ui->tabHandler, SLOT (zoomOut()));

            QObject::connect(_ui->actionReset_Zoom, SIGNAL (triggered()), _ui->tabHandler, SLOT (resetZoom()));

            // Info menu
            QObject::connect(_ui->actionVersion, SIGNAL (triggered()), this, SLOT (showVersionPopup()));

            // Tools & Insert menu
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

            // Design actions
            _tool_group->addAction(_ui->actionSelect);
            _ui->actionSelect->setChecked(true);
            QObject::connect(_ui->actionSelect, SIGNAL (triggered()), _ui->tabHandler, SLOT (setSelectMode()));

            _tool_group->addAction(_ui->actionMove);
            QObject::connect(_ui->actionMove, SIGNAL (triggered()), _ui->tabHandler, SLOT (setMoveMode()));

            _tool_group->addAction(_ui->actionWire);
            QObject::connect(_ui->actionWire, SIGNAL (triggered()), _ui->tabHandler, SLOT (setWireMode()));

            _tool_group->addAction(_ui->actionWire_Remove);
            QObject::connect(_ui->actionWire_Remove, SIGNAL (triggered()), _ui->tabHandler, SLOT (setWireRemoveMode()));

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

            _setupToolbar();
        }

        MainWindow::~MainWindow()
        {
            delete _settings;
            resources::deallocate();
            delete _ui;

        }

        void MainWindow::setLastDesignTool(TOOL tool, COMPONENT comp_type, int res_idx)
        {
            _last_design_tool = tool;
            _last_insert_comp = comp_type;
            _last_insert_res_idx = res_idx;
        }

        void MainWindow::addDesignArea()
        {
            _ui->tabHandler->addDesignArea();
        }

        void MainWindow::setSimulationMenu(bool running, bool no_apply)
        {
            if (!(no_apply || _ui->tabHandler->setSimulationMode()))
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
                // TODO: don't use loop here
                for (const auto &triplet: _insert_actions)
                {
                    int res_idx = std::get<2>(triplet);
                    if (std::get<1>(triplet) == _last_insert_comp && (res_idx == -1 || res_idx == _last_insert_res_idx))
                    {
                        QAction *action = std::get<0>(triplet);
                        action->setChecked(true);
                        emit action->triggered();
                        break;
                    }
                }
                break;
            case MOVE:
                _ui->actionMove->setChecked(true);
                emit _ui->actionMove->triggered();
                break;
            case WIRE:
                _ui->actionWire->setChecked(true);
                emit _ui->actionWire->triggered();
                break;
            case WIRE_REMOVE:
                _ui->actionWire_Remove->setChecked(true);
                emit _ui->actionWire_Remove->triggered();
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
            _ui->actionUndo->setEnabled(!enabled);
            _ui->actionRedo->setEnabled(!enabled);

            _ui->actionSelect->setEnabled(!enabled);
            _select_button->setEnabled(!enabled);
            _ui->actionMove->setEnabled(!enabled);
            _move_button->setEnabled(!enabled);
            _ui->actionWire->setEnabled(!enabled);
            _wire_button->setEnabled(!enabled);
            _ui->actionWire_Remove->setEnabled(!enabled);
            _wire_remove_button->setEnabled(!enabled);
            for (const auto &triplet : _insert_actions)
            {
                std::get<0>(triplet)->setEnabled(!enabled);
            }
            for (const auto &button : _insert_buttons)
            {
                button->setEnabled(!enabled);
            }

            _ui->actionStop->setEnabled(enabled);
            _stop_sim_button->setEnabled(enabled);

            _ui->actionStep->setEnabled(enabled && _sim_paused);
            _step_sim_button->setEnabled(enabled && _sim_paused);
            _ui->actionContinue->setEnabled(enabled && _sim_paused);
            _ui->actionPause->setEnabled(enabled && !_sim_paused);
            _pause_sim_button->setEnabled(enabled);

            _pause_sim_button->blockSignals(true);
            _pause_sim_button->setChecked(enabled && _sim_paused);
            _pause_sim_button->blockSignals(false);

            _ui->actionReset->setEnabled(enabled);
            _reset_sim_button->setEnabled(enabled);

            _ui->actionStart->setEnabled(!enabled);
            _start_sim_button->setEnabled(!enabled);
        }

        void MainWindow::enableContinue()
        {
            _ui->actionContinue->setEnabled(true);
            _ui->actionStep->setEnabled(true);
            _step_sim_button->setEnabled(true);
            _ui->actionPause->setEnabled(false);
            _sim_paused = true;
        }

        void MainWindow::enablePause()
        {
            _ui->actionPause->setEnabled(true);
            _ui->actionStep->setEnabled(false);
            _step_sim_button->setEnabled(false);
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

        void MainWindow::setUndoActionState(bool undo_enabled, bool redo_enabled)
        {
            _ui->actionUndo->setEnabled(undo_enabled);
            _ui->actionRedo->setEnabled(redo_enabled);
        }

        void MainWindow::setSelectActionState(bool have_select, bool have_clipboard)
        {
            _ui->actionCut->setEnabled(have_select);
            _ui->actionCopy->setEnabled(have_select);
            _ui->actionPaste->setEnabled(have_clipboard);
            _ui->actionDelete->setEnabled(have_select);
        }

        void MainWindow::closeEvent(QCloseEvent *ev)
        {
            quit();
            ev->ignore();
        }

        void MainWindow::quit()
        {
            if (!_ui->tabHandler->saveAndCloseAll())
            {
                return;
            }

            QApplication::quit();
        }

        void MainWindow::_setupToolbar()
        {
            std::unordered_map<QAction *, QPushButton *> toolbar_actions;

            _toolbar = new QFrame(this);
            _toolbar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
            _toolbar->setMinimumWidth(150);
            _toolbar->setMaximumWidth(150);
            _toolbar->setFrameShape(QFrame::StyledPanel);
            _toolbar->setFrameShadow(QFrame::Sunken);
            _toolbar->setLineWidth(1);

            QGridLayout *grid = new QGridLayout;
            grid->setAlignment(Qt::AlignTop);
            _toolbar->setLayout(grid);

            QSize button_size(40, 40);

            QButtonGroup *button_group = new QButtonGroup(_toolbar);

            _select_button = new QPushButton(_toolbar);
            _select_button->setIcon(*resources::select_icon);
            _select_button->setFixedSize(button_size);
            _select_button->setCheckable(true);
            _select_button->setChecked(true);
            QObject::connect(_select_button, SIGNAL (pressed()), _ui->actionSelect, SLOT (trigger()));
            grid->addWidget(_select_button, 0, 0);
            button_group->addButton(_select_button);
            toolbar_actions[_ui->actionSelect] = _select_button;


            _move_button = new QPushButton(_toolbar);
            _move_button->setIcon(*resources::move_icon);
            _move_button->setFixedSize(button_size);
            _move_button->setCheckable(true);
            QObject::connect(_move_button, SIGNAL (pressed()), _ui->actionMove, SLOT (trigger()));
            grid->addWidget(_move_button, 0, 1);
            button_group->addButton(_move_button);
            toolbar_actions[_ui->actionMove] = _move_button;

            _wire_button = new QPushButton(_toolbar);
            _wire_button->setIcon(*resources::wire_icon);
            _wire_button->setFixedSize(button_size);
            _wire_button->setCheckable(true);
            QObject::connect(_wire_button, SIGNAL (pressed()), _ui->actionWire, SLOT (trigger()));
            grid->addWidget(_wire_button, 0, 2);
            button_group->addButton(_wire_button);
            toolbar_actions[_ui->actionWire] = _wire_button;

            _wire_remove_button = new QPushButton(_toolbar);
            _wire_remove_button->setIcon(*resources::wire_remove_icon);
            _wire_remove_button->setFixedSize(button_size);
            _wire_remove_button->setCheckable(true);
            QObject::connect(_wire_remove_button, SIGNAL (pressed()), _ui->actionWire_Remove, SLOT (trigger()));
            grid->addWidget(_wire_remove_button, 1, 0);
            button_group->addButton(_wire_remove_button);
            toolbar_actions[_ui->actionWire_Remove] = _wire_remove_button;

            QFrame *hline1 = new QFrame(_toolbar);
            hline1->setFrameShape(QFrame::HLine);
            hline1->setFrameShadow(QFrame::Sunken);
            grid->addWidget(hline1, 2, 0, 1, 3);

            const std::vector<COMPONENT> split_after = {COMPONENT::XNOR_GATE, COMPONENT::KEYPAD, COMPONENT::_7SEG_8IN};

            size_t curr_cmp = 0;
            int row = 3, col = 0;
            for (const auto &triplet : _insert_actions)
            {
                QPushButton *button = new QPushButton(_toolbar);
                int idx = std::get<2>(triplet);
                button->setIcon(*(resources::comp_icons.at(std::get<1>(triplet))[idx == -1 ? 0 : idx]));
                button->setIconSize(QSize(30, 30));
                button->setFixedSize(button_size);
                button->setCheckable(true);
                QObject::connect(button, SIGNAL (pressed()), std::get<0>(triplet), SLOT (trigger()));
                grid->addWidget(button, row, col);
                button_group->addButton(button);
                toolbar_actions[std::get<0>(triplet)] = button;

                row = row + col / 2;
                col = (col + 1) % 3;

                _insert_buttons.push_back(button);

                if (std::get<1>(triplet) == split_after[curr_cmp])
                {
                    if (++curr_cmp == split_after.size())
                    {
                        break;
                    }
                    QFrame *hline = new QFrame(_toolbar);
                    hline->setFrameShape(QFrame::HLine);
                    hline->setFrameShadow(QFrame::Sunken);
                    if (col == 0)
                    {
                        grid->addWidget(hline, row, col, 1, 3);
                    }
                    else
                    {
                        grid->addWidget(hline, ++row, 0, 1, 3);
                        col = 0;
                    }
                    ++row;
                }
            }

            if (col != 0)
            {
                ++row;
                col = 0;
            }

            QFrame *hline2 = new QFrame(_toolbar);
            hline2->setFrameShape(QFrame::HLine);
            hline2->setFrameShadow(QFrame::Sunken);
            grid->addWidget(hline2, ++row, col, 1, 3);

            _start_sim_button = new QPushButton(_toolbar);
            _start_sim_button->setIcon(*resources::start_sim_icon);
            _start_sim_button->setFixedSize(button_size);
            QObject::connect(_start_sim_button, SIGNAL (pressed()), _ui->actionStart, SLOT (trigger()));
            grid->addWidget(_start_sim_button, ++row, col++);
            button_group->addButton(_start_sim_button);

            _stop_sim_button = new QPushButton(_toolbar);
            _stop_sim_button->setIcon(*resources::stop_sim_icon);
            _stop_sim_button->setFixedSize(button_size);
            QObject::connect(_stop_sim_button, SIGNAL (pressed()), _ui->actionStop, SLOT (trigger()));
            grid->addWidget(_stop_sim_button, row, col++);
            _stop_sim_button->setEnabled(false);

            _pause_sim_button = new QPushButton(_toolbar);
            _pause_sim_button->setIcon(*resources::pause_sim_icon);
            _pause_sim_button->setFixedSize(button_size);
            _pause_sim_button->setCheckable(true);
            QObject::connect(_pause_sim_button, &QPushButton::toggled, [this](int checked)
            {
                if (checked)
                {
                    enablePause();
                    _ui->actionPause->trigger();
                }
                else
                {
                    enableContinue();
                    _ui->actionContinue->trigger();
                }
            });
            grid->addWidget(_pause_sim_button, row, col);
            _pause_sim_button->setEnabled(false);

            col = 0;

            _step_sim_button = new QPushButton(_toolbar);
            _step_sim_button->setIcon(*resources::step_sim_icon);
            _step_sim_button->setFixedSize(button_size);
            QObject::connect(_step_sim_button, SIGNAL (pressed()), _ui->actionStep, SLOT (trigger()));
            grid->addWidget(_step_sim_button, ++row, col++);
            _step_sim_button->setEnabled(false);

            _reset_sim_button = new QPushButton(_toolbar);
            _reset_sim_button->setIcon(*resources::reset_sim_icon);
            _reset_sim_button->setFixedSize(button_size);
            QObject::connect(_reset_sim_button, SIGNAL (pressed()), _ui->actionReset, SLOT (trigger()));
            grid->addWidget(_reset_sim_button, row, col++);
            _reset_sim_button->setEnabled(false);

            QObject::connect(_tool_group, &QActionGroup::triggered, [last_button = static_cast<QPushButton *>(_select_button), toolbar_actions, button_group](QAction* action) mutable
            {
                if (toolbar_actions.find(action) == toolbar_actions.end())
                {
                    if (last_button != nullptr)
                    {
                        button_group->setExclusive(false);
                        last_button->setChecked(false);
                        button_group->setExclusive(true);
                        last_button = nullptr;
                    }

                    return;
                }

                last_button = toolbar_actions[action];
                last_button->setChecked(true);
            });

            _ui->horizontalLayout->insertWidget(0, _toolbar);
        }

        void MainWindow::toggleToolbar()
        {
            if (_toolbar->isHidden())
            {
                _toolbar->show();
            }
            else
            {
                _toolbar->hide();
            }
            _settings->setValue("toolbar", !_toolbar->isHidden());
        }

        void MainWindow::showVersionPopup()
        {
            QMessageBox version_popup;
            version_popup.setWindowTitle("Version Info");
            version_popup.setText(QString("Version: ") + LOGICSIM_VERSION);

            QSize dialog_size = version_popup.sizeHint();
            version_popup.move(x() + width()/2 - dialog_size.width()/2, y() + height()/2 - dialog_size.height()/2);

            version_popup.exec();
        }
    }
}
