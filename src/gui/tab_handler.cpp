#include "gui/tab_handler.hpp"

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
            addDesignArea();
            if (QCoreApplication::arguments().size() > 1)
            {
                currentDesignArea()->readFromFile(QCoreApplication::arguments().at(1));
            }
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

            QObject::connect(new_area, SIGNAL (newUndoActionPerformed(bool, bool, bool)), this, SLOT (performUndoAction(bool, bool, bool)));
            QObject::connect(new_area, SIGNAL (newSelection(bool, bool)), this, SLOT (performSelectionAction(bool, bool)));

            addTab(new_area, "Untitled*");
            setCurrentWidget(new_area);

            _file_undo_state[new_area] = {false, 0, false, false};
            emit undoActionPerformed(false, false);
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
            if (count() == 1 && design_area->empty() && !std::get<0>(_file_undo_state[design_area]))
            {
                return true;
            }

            if (!design_area->empty() && std::get<1>(_file_undo_state[design_area]) != 0)
            {
                // TODO: improve visually
                QMessageBox save_dialog;
                save_dialog.setWindowTitle("Save Changes");
                save_dialog.setText("Circuit modified.");

                QString info_str = "Save changes";
                QString filepath = design_area->filepath();

                if (!filepath.isEmpty())
                {
                    QFileInfo info(filepath);
                    info_str += " to " + info.fileName();
                }

                info_str += '?';

                save_dialog.setInformativeText(info_str);

                save_dialog.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

                save_dialog.setWindowFlags(Qt::Window);
                QPoint window_pos = parentWidget()->parentWidget()->pos();
                QSize window_size = parentWidget()->parentWidget()->size();

                QSize dialog_size = save_dialog.sizeHint();
                save_dialog.move(window_pos.x() + window_size.width()/2 - dialog_size.width()/2, window_pos.y() + window_size.height()/2 - dialog_size.height()/2);

                switch (save_dialog.exec())
                {
                case QMessageBox::Save:
                    try
                    {
                        design_area->writeToFile();
                    }
                    catch (const std::invalid_argument &)
                    {
                        return false;
                    }

                    break;
                case QMessageBox::Discard:
                    break;
                case QMessageBox::Cancel:
                    return false;
                }
            }

            _file_undo_state.erase(design_area);

            if (_selected_idx == count() - 1 && _selected_idx != 0)
            {
                --_selected_idx;
            }
            QString filepath = design_area->filepath();
            _open_files.erase(filepath);

            QFileInfo info(filepath);
            QString filename = info.fileName();
            _open_filenames[filename].erase(std::remove(_open_filenames[filename].begin(), _open_filenames[filename].end(), design_area), _open_filenames[filename].end());
            switch (_open_filenames[filename].size())
            {
            case 1:
            {
                DesignArea *area_left = _open_filenames[filename][0];
                QFileInfo info2(area_left->filepath());
                setTabText(indexOf(area_left), info2.fileName());
                break;
            }
            case 0:
                _open_filenames.erase(filename);
                break;
            default:
                break;
            }

            delete design_area; // also removes tab

            if (count() == 0)
            {
                addDesignArea();
            }

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
            _file_undo_state[design_area] = {true, 0, false, false};

            try {
                design_area->readFromFile(filepath);
            }
            catch (const std::invalid_argument &)
            {
                QMessageBox message_box;
                message_box.critical(0, "Error", "File could not be opened; it might be corrupted.");

                message_box.setWindowFlags(Qt::Window);
                QPoint window_pos = parentWidget()->parentWidget()->pos();
                QSize window_size = parentWidget()->parentWidget()->size();

                QSize dialog_size = message_box.sizeHint();
                message_box.move(window_pos.x() + window_size.width()/2 - dialog_size.width()/2, window_pos.y() + window_size.height()/2 - dialog_size.height()/2);
                if (add)
                {
                    removeDesignArea();
                }
                return;
            }
            _setupTab(design_area);
            if (!add)
            {
                design_area->clearUndoStack();
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
            bool success;
            DesignArea *design_area = currentDesignArea();
            bool tab_setup = new_file ||  design_area->filepath().isEmpty();
            try {
                success = design_area->writeToFile(new_file);
            }
            catch (const std::invalid_argument &) {
                return;
            }

            if (!success)
            {
                QMessageBox message_box;
                message_box.critical(0, "Error", "File could not be saved.");

                message_box.setWindowFlags(Qt::Window);
                QPoint window_pos = parentWidget()->parentWidget()->pos();
                QSize window_size = parentWidget()->parentWidget()->size();

                QSize dialog_size = message_box.sizeHint();
                message_box.move(window_pos.x() + window_size.width()/2 - dialog_size.width()/2, window_pos.y() + window_size.height()/2 - dialog_size.height()/2);
                return;
            }

            if (tab_setup)
            {
                _setupTab(design_area);
                _file_undo_state[design_area] = {true, 0, false, false};
            }
            if (std::get<1>(_file_undo_state[design_area]) != 0)
            {
                std::get<1>(_file_undo_state[design_area]) = 0;
                _removeUnsavedIcon();
            }
        }

        void TabHandler::changeArea(int idx)
        {
            if (_selected_idx == idx || idx < 0)
            {
                return;
            }

            DesignArea *design_area = currentDesignArea();
            if (_selected_idx < count())
            {
                design_area->pauseState();
            }

            DesignArea *new_design_area = _designArea(idx);
            _selected_idx = idx;
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
            emit undoActionPerformed(std::get<2>(_file_undo_state[new_design_area]), std::get<3>(_file_undo_state[new_design_area]));
        }

        void TabHandler::setSelectMode()
        {
            currentDesignArea()->setMode(TOOL::SELECT);
            emit designToolChanged(TOOL::SELECT);
        }

        void TabHandler::setMoveMode()
        {
            currentDesignArea()->setMode(TOOL::MOVE);
            emit designToolChanged(TOOL::MOVE);
        }

        void TabHandler::setWireMode()
        {
            currentDesignArea()->setMode(TOOL::WIRE);
            emit designToolChanged(TOOL::WIRE);
        }

        void TabHandler::setWireRemoveMode()
        {
            currentDesignArea()->setMode(TOOL::WIRE_REMOVE);
            emit designToolChanged(TOOL::WIRE_REMOVE);
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

        void TabHandler::undoAction()
        {
            currentDesignArea()->undoAction();
        }

        void TabHandler::redoAction()
        {
            currentDesignArea()->redoAction();
        }

        void TabHandler::performUndoAction(bool was_undo, bool undo_enabled, bool redo_enabled)
        {
            emit undoActionPerformed(undo_enabled, redo_enabled);
            DesignArea *design_area = currentDesignArea();

            std::tuple<bool, int, bool, bool> &state = _file_undo_state[design_area];

            std::get<2>(state) = undo_enabled;
            std::get<3>(state) = redo_enabled;

            if (was_undo)
            {
                if (--std::get<1>(state) == 0)
                {
                    _removeUnsavedIcon();
                }
                else if (std::get<1>(state) == -1)
                {
                    _addUnsavedIcon();
                }
            }
            else
            {
                if (++std::get<1>(state) == 0)
                {
                    _removeUnsavedIcon();
                }
                else if (std::get<1>(state) == 1)
                {
                    _addUnsavedIcon();
                }
            }
        }

        void TabHandler::_addUnsavedIcon()
        {
            if (!std::get<0>(_file_undo_state[currentDesignArea()]))
            {
                return;
            }
            setTabText(_selected_idx, tabText(_selected_idx) + '*');
        }

        void TabHandler::_removeUnsavedIcon()
        {
            if (!std::get<0>(_file_undo_state[currentDesignArea()]))
            {
                return;
            }
            QString tab_text = tabText(_selected_idx);
            tab_text.chop(1);
            setTabText(_selected_idx, tab_text);
        }

        bool TabHandler::saveAndCloseAll()
        {
            for (int i = count() - 1; i >= 0; --i)
            {
                setCurrentIndex(i);
                if (!removeDesignArea(i))
                {
                    return false;
                }
            }

            return true;
        }

        void TabHandler::cutAction()
        {
            currentDesignArea()->cutAction();
        }
        void TabHandler::copyAction()
        {
            currentDesignArea()->copyAction();
        }
        void TabHandler::pasteAction()
        {
            currentDesignArea()->pasteAction();
        }
        void TabHandler::deleteAction()
        {
            currentDesignArea()->deleteAction();
        }

        void TabHandler::performSelectionAction(bool have_select, bool have_clipboard)
        {
            emit selectionActionPerformed(have_select, have_clipboard);
        }

        void TabHandler::toggleWireColor()
        {
            _wire_color = !_wire_color;
            for (int i = 0; i < count(); ++i)
            {
                _designArea(i)->setColorWires(_wire_color);
            }
        }

        void TabHandler::zoomIn()
        {
            DesignArea *design_area = currentDesignArea();
            design_area->zoomIn(design_area->width()/2, design_area->height()/2);
        }

        void TabHandler::zoomOut()
        {
            DesignArea *design_area = currentDesignArea();
            design_area->zoomOut(design_area->width()/2, design_area->height()/2);
        }

        void TabHandler::resetZoom()
        {
            currentDesignArea()->resetZoom();
        }
    }
}
