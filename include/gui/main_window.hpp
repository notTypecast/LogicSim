#ifndef LOGICSIM_GUI_MAIN_WINDOW_HPP
#define LOGICSIM_GUI_MAIN_WINDOW_HPP

#include <QApplication>
#include <QMainWindow>
#include <QActionGroup>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

namespace logicsim
{
    namespace gui
    {
        const QString IMG_PATH = "../LogicSim/res/";

        enum TOOL
        {
            SELECT,
            WIRE,
            INSERT
        };

        enum COMPONENT
        {
            NONE,
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

        const QString COMPONENT_RES[] = {
            "",
            "NOT.png",
            "AND.png",
            "OR.png",
            "XOR.png",
            "NAND.png",
            "NOR.png",
            "XNOR.png"
        };

        class MainWindow : public QMainWindow
        {
            Q_OBJECT
        public:
            explicit MainWindow(QWidget *parent = nullptr);
            ~MainWindow();

        protected:
            Ui::MainWindow *_ui;
        };

    }
}
#endif // MAIN_WINDOW_HPP
