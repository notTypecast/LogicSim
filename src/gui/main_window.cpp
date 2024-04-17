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

            QActionGroup *tool_group = new QActionGroup(this);

            tool_group->addAction(_ui->actionSelect);

            _ui->actionSelect->setChecked(true);

            QObject::connect(_ui->actionSelect, SIGNAL (triggered()), _ui->designArea, SLOT (setSelectMode()));

            QAction *insert_actions[] = {
                _ui->actionNOT,
                _ui->actionAND,
                _ui->actionOR,
                _ui->actionXOR,
                _ui->actionNAND,
                _ui->actionNOR,
                _ui->actionXNOR,
                _ui->actionConstant,
                _ui->actionSwitch,
                _ui->actionOscillator,
                _ui->actionKeypad,
                _ui->actionLED,
                _ui->action7_Segment_Display_5_input,
                _ui->action7_Segment_Display_8_input,
                _ui->actionSR_Latch,
                _ui->actionJK_Latch,
                _ui->actionT_Latch,
                _ui->actionD_Latch,
                _ui->actionSR_Flip_Flop,
                _ui->actionJK_Flip_Flop,
                _ui->actionD_Flip_Flop,
                _ui->actionT_Flip_Flop
            };

            int i = 0;
            for (const auto & action: insert_actions)
            {
                tool_group->addAction(action);
                action->setProperty("component-type", ++i);
                QObject::connect(action, SIGNAL (triggered()), _ui->designArea, SLOT (setInsertMode()));
            }
        }

        MainWindow::~MainWindow()
        {
            resources::deallocate();
            delete _ui;
        }

    }
}
