#include "gui/design_area.hpp"
#include <iostream>
namespace logicsim
{
    namespace gui
    {
        DesignArea::DesignArea(QWidget *parent): QScrollArea{parent}
        {
            setWidget(new QWidget(this));
            widget()->resize(width(), height());

        }

        void DesignArea::mousePressEvent(QMouseEvent *ev)
        {
            switch (_selected_tool)
            {
            case SELECT:
                _unselectAll();
                _select_x = ev->x();
                _select_y = ev->y();
                for (int i = 0; i < 4; ++i)
                {
                    _selection_border[i] = new QLabel(widget());
                    _selection_border[i]->show();
                }
                break;

            case WIRE:
                break;

            case INSERT:
                ComponentLabel *label = new ComponentLabel(widget());
                label->setPixmap(resources::comp_images.at(_insert_component));

                QObject::connect(label, SIGNAL (selected(ComponentLabel *, bool)), this, SLOT (addSelected(ComponentLabel *, bool)));
                QObject::connect(label, SIGNAL (moved(int, int)), this, SLOT (moveSelectedComponents(int, int)));
                QObject::connect(this, SIGNAL (selectMode(bool)), label, SLOT (selectMode(bool)));
                QObject::connect(this, SIGNAL (rangeQuery(int, int, int, int)), label, SLOT (checkRangeQuery(int, int, int, int)));
                QObject::connect(label, SIGNAL (selected_nocheck(ComponentLabel *)), this, SLOT (addSelected_nocheck(ComponentLabel *)));

                label->move(ev->x(), ev->y());
                label->show();

                addSelected(label);
                _just_inserted = true;
                break;
            }
        }

        void DesignArea::mouseReleaseEvent(QMouseEvent *ev)
        {
            switch (_selected_tool)
            {
            case SELECT:
            {
                for (int i = 0; i < 4; ++i)
                {
                    delete _selection_border[i];
                    _selection_border[i] = nullptr;
                }
                int min_x = std::min(_select_x, ev->x()), min_y = std::min(_select_y, ev->y());
                int max_x = std::max(_select_x, ev->x()), max_y = std::max(_select_y, ev->y());
                emit rangeQuery(min_x, min_y, max_x, max_y);
                break;
            }

            case INSERT:
                _unselectAll();
                _just_inserted = false;
                break;
            default:
                break;
            }
        }

        void DesignArea::mouseMoveEvent(QMouseEvent *ev)
        {
            switch (_selected_tool)
            {
            case SELECT:
            {
                if (_selection_border[0] == nullptr)
                {
                    return;
                }
                int width = std::abs(_select_x - ev->x()) + resources::LINE_THICKNESS, height = std::abs(_select_y - ev->y());
                QPixmap hline = resources::getLine(resources::HORIZONTAL, width);
                QPixmap vline = resources::getLine(resources::VERTICAL, height);

                int min_x = std::min(_select_x, ev->x()), min_y = std::min(_select_y, ev->y());

                _selection_border[0]->resize(resources::LINE_THICKNESS, height);
                _selection_border[0]->setPixmap(vline);
                _selection_border[0]->move(min_x, min_y);

                _selection_border[1]->resize(resources::LINE_THICKNESS, height);
                _selection_border[1]->setPixmap(vline);
                _selection_border[1]->move(std::max(_select_x, ev->x()), min_y);

                _selection_border[2]->resize(width, resources::LINE_THICKNESS);
                _selection_border[2]->setPixmap(hline);
                _selection_border[2]->move(min_x, min_y);

                _selection_border[3]->resize(width, resources::LINE_THICKNESS);
                _selection_border[3]->setPixmap(hline);
                _selection_border[3]->move(min_x, std::max(_select_y, ev->y()));

                break;
            }

            case INSERT:
                if (_just_inserted)
                {
                    _selected_components[0]->move(ev->x(), ev->y());
                    _selected_components[0]->border()->move(_selected_components[0]->x(), _selected_components[0]->y());
                }
                break;
            default:
                break;
            }
        }

        void DesignArea::setSelectMode()
        {
            _selected_tool = TOOL::SELECT;
            emit selectMode(true);
        }

        void DesignArea::addSelected(ComponentLabel *component, bool ctrl)
        {
            auto comp_iter = std::find(_selected_components.begin(), _selected_components.end(), component);
            if (comp_iter != _selected_components.end())
            {
                if (ctrl)
                {
                    component->setBorder(nullptr);
                    _selected_components.erase(comp_iter);
                }

                return;
            }

            if (!ctrl)
            {
                if (!_selected_components.empty())
                {
                    _unselectAll();
                }
            }

            addSelected_nocheck(component);
        }

        void DesignArea::addSelected_nocheck(ComponentLabel *component)
        {
            _selected_components.push_back(component);

            QLabel *border = new QLabel(widget());
            border->setPixmap(*resources::border);
            border->setAttribute(Qt::WA_TransparentForMouseEvents);

            border->move(component->x(), component->y());
            border->show();

            component->setBorder(border);

        }

        void DesignArea::_unselectAll()
        {
            for (const auto &component: _selected_components)
            {
                component->setBorder(nullptr);
            }
            _selected_components.clear();
        }

        void DesignArea::moveSelectedComponents(int x, int y)
        {
            for (const auto &component : _selected_components)
            {
                component->move(x + component->x(), y + component->y());
                component->border()->move(component->x(), component->y());
            }
        }

        void DesignArea::setInsertMode()
        {
            _selected_tool = TOOL::INSERT;
            emit selectMode(false);
            _insert_component = static_cast<COMPONENT>(QObject::sender()->property("component-type").value<int>());
        }
    }
}
