#include "gui/tab_handler.hpp"
#include <iostream>
namespace logicsim
{
    namespace gui
    {
        TabHandler::TabHandler(QWidget *parent) : QTabWidget{parent}
        {
            QObject::connect(this, SIGNAL (currentChanged(int)), this, SLOT (changeArea(int)));
        }

        void TabHandler::setStatusBar(QStatusBar *status_bar)
        {
            _status_bar = status_bar;
            removeTab(0);
            addDesignArea();
            _selected_idx = 0;
        }

        DesignArea *TabHandler::currentDesignArea() const
        {
            return _design_areas[currentIndex()];
        }

        void TabHandler::addDesignArea()
        {
            DesignArea *new_area = new DesignArea(this);
            new_area->setStatusBar(_status_bar);
            _design_areas.push_back(new_area);

            addTab(new_area, "Untitled*");
            setCurrentWidget(new_area);
        }

        void TabHandler::changeArea(int idx)
        {
            if (_design_areas.size() > 1)
            {
                _design_areas[_selected_idx]->pauseState();

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
