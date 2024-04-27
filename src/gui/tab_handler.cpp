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
            _status_bar = status_bar;
            DesignArea *design_area = static_cast<DesignArea *>(widget(0));
            design_area->setStatusBar(status_bar);
            _selected_idx = 0;
        }

        void TabHandler::keyPressEvent(QKeyEvent *ev)
        {
            currentDesignArea()->keyPressEvent(ev);
        }

        DesignArea *TabHandler::_designArea(int idx) const
        {
            return static_cast<DesignArea *>(widget(idx));
        }

        DesignArea *TabHandler::currentDesignArea() const
        {
            return _designArea(_selected_idx);
        }

        void TabHandler::addDesignArea()
        {
            DesignArea *new_area = new DesignArea(this);
            new_area->setStatusBar(_status_bar);

            addTab(new_area, "Untitled*");
            setCurrentWidget(new_area);
        }

        bool TabHandler::removeDesignArea()
        {
            return removeDesignArea(_selected_idx);
        }

        bool TabHandler::removeDesignArea(int idx)
        {
            return _removeDesignArea(_designArea(idx));
        }

        bool TabHandler::_removeDesignArea(DesignArea *design_area)
        {
            if (count() == 1)
            {
                return false;
            }

            if (!design_area->empty() && false) // TODO: && circuit modified
            {
                QMessageBox save_dialog;
                save_dialog.setText("Circuit modified");

                QString info_str = "Save changes";
                QString filepath = design_area->filepath();

                if (!filepath.isEmpty())
                {
                    QFileInfo info(filepath);
                    info_str += " to " + info.fileName();
                }

                filepath += '?';

                save_dialog.setInformativeText(info_str);

                switch (save_dialog.exec())
                {
                case QMessageBox::Save:
                    design_area->writeToFile();
                    break;
                case QMessageBox::Discard:
                    break;
                case QMessageBox::Cancel:
                    return false;
                }
            }

            if (_selected_idx == count() - 1)
            {
                --_selected_idx;
            }
            QString filepath = design_area->filepath();
            _open_files.erase(filepath);

            QFileInfo info(filepath);
            QString filename = info.fileName();
            _open_filenames[filename].erase(std::remove(_open_filenames[filename].begin(), _open_filenames[filename].end(), design_area), _open_filenames[filename].end());
            if (_open_filenames[filename].size() == 1)
            {
                DesignArea *area_left = _open_filenames[filename][0];
                QFileInfo info2(area_left->filepath());
                setTabText(indexOf(area_left), info2.fileName());
            }

            delete design_area; // also removes tab

            return true;
        }

        void TabHandler::_setupTab(DesignArea *design_area)
        {
            QString filepath = design_area->filepath();
            _open_files[filepath] = design_area;

            const QFileInfo info(design_area->filepath());
            const QString filename = info.fileName();

            if (_open_filenames.find(filename) != _open_filenames.end())
            {
                setTabText(_selected_idx, filepath);
                if (_open_filenames[filename].size() == 1)
                {
                    DesignArea *first = _open_filenames[filename][0];
                    setTabText(indexOf(first), first->filepath());
                }
            }
            else
            {
                _open_filenames[filename] = std::vector<DesignArea *>();
                setTabText(_selected_idx, filename);
            }

            _open_filenames[filename].push_back(design_area);
        }

        void TabHandler::openFile()
        {
            const QString filepath = QFileDialog::getOpenFileName(this, "Open File", "../LogicSim/saves/", "LogicSim Circuit files (*.lsc)");

            if (filepath.isEmpty())
            {
                return;
            }

            if (_open_files.find(filepath) != _open_files.end())
            {
                setCurrentWidget(_open_files[filepath]);
                return;
            }

            DesignArea *design_area = currentDesignArea();
            bool add = !(design_area->empty() && design_area->filepath().isEmpty());
            if (add)
            {
                addDesignArea();
            }

            // update in case new was added
            design_area = currentDesignArea();

            try {
                design_area->readFromFile(filepath);
            }
            catch (std::invalid_argument)
            {
                // TODO: show fail message
                std::cout << "Failed to open" << std::endl;
                if (add)
                {
                    removeDesignArea();
                }
                return;
            }
            _setupTab(design_area);
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
            bool success;
            DesignArea *design_area = currentDesignArea();
            bool tab_setup = new_file ||  design_area->filepath().isEmpty();
            try {
                success = design_area->writeToFile(new_file);
            }
            catch (std::invalid_argument) {
                return;
            }

            if (!success)
            {
                // TODO show fail message
                std::cout << "Failed to save" << std::endl;
                return;
            }

            if (tab_setup)
            {
                _setupTab(design_area);
            }
        }

        void TabHandler::changeArea(int idx)
        {
            if (_selected_idx == idx)
            {
                return;
            }

            DesignArea *design_area = currentDesignArea();
            if (_selected_idx < count())
            {
                design_area->pauseState();
            }
            DesignArea *new_design_area = _designArea(idx);

            setCurrentIndex(idx);
            bool running_sim = new_design_area->continueState();

            if (new_design_area->mode() != TOOL::SIMULATE)
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
            currentDesignArea()->setMode(TOOL::SELECT);
            emit designToolChanged(TOOL::SELECT);
        }

        void TabHandler::setWireMode()
        {
            currentDesignArea()->setMode(TOOL::WIRE);
            emit designToolChanged(TOOL::WIRE);
        }

        void TabHandler::setInsertMode()
        {
            COMPONENT comp_type = static_cast<COMPONENT>(QObject::sender()->property("component-type").value<int>());
            QVariant res_idx = QObject::sender()->property("resource-idx");
            currentDesignArea()->setMode(TOOL::INSERT, comp_type, res_idx.isValid() ? res_idx.value<int>() : 0);
            emit designToolChanged(TOOL::INSERT, comp_type);
        }

        bool TabHandler::setSimulationMode()
        {
            return currentDesignArea()->setMode(TOOL::SIMULATE);
        }

        void TabHandler::stopSimulationMode()
        {
            currentDesignArea()->stopSimulationMode();
        }

        void TabHandler::pauseSimulation()
        {
            currentDesignArea()->pauseSimulation();
        }

        void TabHandler::stepSimulation()
        {
            currentDesignArea()->stepSimulation();
        }

        void TabHandler::continueSimulation()
        {
            currentDesignArea()->continueSimulation();
        }

        void TabHandler::resetSimulation()
        {
            currentDesignArea()->resetSimulation();
        }
    }
}
