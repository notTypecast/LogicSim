#include "gui/tab_handler.hpp"
#include <iostream>
namespace logicsim
{
    namespace gui
    {
        TabHandler::TabHandler(QWidget *parent) : QTabWidget{parent}
        {
            setTabsClosable(true);
            QObject::connect(this, SIGNAL (currentChanged(int)), this, SLOT (changeArea(int)));
            QObject::connect(this, SIGNAL (tabCloseRequested(int)), this, SLOT (removeDesignArea(int)));
        }

        void TabHandler::setStatusBar(QStatusBar *status_bar)
        {
            _design_areas.push_back(findChild<DesignArea *>("tab_1"));
            _design_areas[0]->setStatusBar(status_bar);
            setCurrentWidget(_design_areas[0]);
            _status_bar = status_bar;            
            _selected_idx = 0;
        }

        DesignArea *TabHandler::currentDesignArea() const
        {
            return _design_areas[_selected_idx];
        }

        void TabHandler::addDesignArea()
        {
            DesignArea *new_area = new DesignArea(this);
            new_area->setStatusBar(_status_bar);
            _design_areas.push_back(new_area);

            addTab(new_area, "Untitled*");
            setCurrentWidget(new_area);
        }

        void TabHandler::removeDesignArea()
        {
            removeDesignArea(_selected_idx);
        }

        bool TabHandler::removeDesignArea(int idx)
        {
            if (_design_areas.size() == 1)
            {
                return false;
            }

            if (!_design_areas[idx]->empty() && false) // TODO: && circuit modified
            {
                QMessageBox save_dialog;
                save_dialog.setText("Circuit modified");

                QString info_str = "Save changes";
                QString filename = _design_areas[idx]->filename();

                if (!filename.isEmpty())
                {
                    QFileInfo info(filename);
                    info_str += " to " + info.fileName();
                }

                filename += '?';

                save_dialog.setInformativeText(info_str);

                switch (save_dialog.exec())
                {
                case QMessageBox::Save:
                    _design_areas[idx]->writeToFile();
                    break;
                case QMessageBox::Discard:
                    break;
                case QMessageBox::Cancel:
                    return false;
                }
            }

            delete _design_areas[idx];
            _design_areas.erase(std::next(_design_areas.begin(), idx));
            removeTab(idx);

            return true;
        }

        void TabHandler::openFile()
        {
            QString filename = QFileDialog::getOpenFileName(this, "Open File", "../LogicSim/saves/", "LogicSim Circuit files (*.lsc)");

            if (filename.isEmpty())
            {
                return;
            }

            bool add = !_design_areas[_selected_idx]->empty();
            if (add)
            {
                addDesignArea();
            }

            try {
                _design_areas[_selected_idx]->readFromFile(filename);
                const QFileInfo info(_design_areas[_selected_idx]->filename());
                setTabText(_selected_idx, info.fileName());
            }
            catch (std::invalid_argument)
            {
                // TODO: show fail message
                if (add)
                {
                    removeDesignArea();
                }
            }
        }

        void TabHandler::saveFile()
        {
            _saveFile(false);
        }

        void TabHandler::saveFileAs()
        {
            _saveFile(true);
        }

        void TabHandler::_saveFile(bool new_file)
        {
            // TODO show fail message if failed
            if (_design_areas[_selected_idx]->writeToFile(new_file))
            {
                const QFileInfo info(_design_areas[_selected_idx]->filename());
                setTabText(_selected_idx, info.fileName());
            }
        }

        void TabHandler::changeArea(int idx)
        {
            if (_selected_idx == idx)
            {
                return;
            }
            if (static_cast<size_t>(_selected_idx) < _design_areas.size())
            {
                _design_areas[_selected_idx]->pauseState();
            }

            setCurrentWidget(_design_areas[idx]);
            bool running_sim = _design_areas[idx]->continueState();

            if (_design_areas[idx]->mode() != TOOL::SIMULATE)
            {
                emit designTabChosen();
            }
            else
            {
                emit simulationTabChosen(running_sim);
            }
            _selected_idx = idx;
        }

        void TabHandler::setSelectMode()
        {
            _design_areas[currentIndex()]->setMode(TOOL::SELECT);
            emit designToolChanged(TOOL::SELECT);
        }

        void TabHandler::setWireMode()
        {
            _design_areas[currentIndex()]->setMode(TOOL::WIRE);
            emit designToolChanged(TOOL::WIRE);
        }

        void TabHandler::setInsertMode()
        {
            COMPONENT comp_type = static_cast<COMPONENT>(QObject::sender()->property("component-type").value<int>());
            QVariant res_idx = QObject::sender()->property("resource-idx");
            _design_areas[currentIndex()]->setMode(TOOL::INSERT, comp_type, res_idx.isValid() ? res_idx.value<int>() : 0);
            emit designToolChanged(TOOL::INSERT, comp_type);
        }

        void TabHandler::setSimulationMode()
        {
            _design_areas[currentIndex()]->setMode(TOOL::SIMULATE);
        }

        void TabHandler::stopSimulationMode()
        {
            _design_areas[currentIndex()]->stopSimulationMode();
        }

        void TabHandler::pauseSimulation()
        {
            _design_areas[currentIndex()]->pauseSimulation();
        }

        void TabHandler::stepSimulation()
        {
            _design_areas[currentIndex()]->stepSimulation();
        }

        void TabHandler::continueSimulation()
        {
            _design_areas[currentIndex()]->continueSimulation();
        }

        void TabHandler::resetSimulation()
        {
            _design_areas[currentIndex()]->resetSimulation();
        }
    }
}
