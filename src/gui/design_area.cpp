#include "gui/design_area.hpp"
#include <iostream>
namespace logicsim
{
    namespace gui
    {
        DesignArea::DesignArea(QWidget *parent): QScrollArea{parent}, _border_img(QPixmap(IMG_PATH + "border.png"))
        {
            setWidget(new QWidget(this));
            widget()->resize(width(), height());

        }

        void DesignArea::mousePressEvent(QMouseEvent *ev)
        {
            switch (_selected_tool)
            {
            case SELECT:
                _unselect();
                break;

            case WIRE:
                break;

            case INSERT:
                QPixmap img(IMG_PATH + COMPONENT_RES[_insert_component]);
                ComponentLabel *label = new ComponentLabel(widget());
                label->setPixmap(img);

                QObject::connect(label, SIGNAL (selected(ComponentLabel *)), this, SLOT (setSelected(ComponentLabel *)));
                QObject::connect(label, SIGNAL (moved(ComponentLabel *, int, int)), this, SLOT (moveComponent(ComponentLabel *, int, int)));
                QObject::connect(this, SIGNAL (selectMode(bool)), label, SLOT(selectMode(bool)));

                label->move(ev->x(), ev->y());
                label->show();

                setSelected(label);
                // emit selectMode(true);
                break;
            }
        }

        void DesignArea::mouseReleaseEvent(QMouseEvent *ev)
        {
            switch (_selected_tool)
            {
            case INSERT:
                _unselect();
                // emit selectMode(false);
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

        void DesignArea::setSelected(ComponentLabel *component)
        {
            if (_selected_component != component)
            {
                _unselect();
            }

            if (_selected_border != nullptr)
            {
                return;
            }
            _selected_component = component;
            _selected_border = new QLabel(widget());
            _selected_border->setPixmap(_border_img);
            _selected_border->setAttribute(Qt::WA_TransparentForMouseEvents);

            _selected_border->move(component->x(), component->y());
            _selected_border->show();
        }

        void DesignArea::_unselect()
        {
            _selected_component = nullptr;
            delete _selected_border;
            _selected_border = nullptr;
        }

        void DesignArea::moveComponent(ComponentLabel *component, int x, int y)
        {
            component->move(x + component->x(), y + component->y());
            _selected_border->move(component->x(), component->y());
        }

        void DesignArea::setInsertMode()
        {
            _selected_tool = TOOL::INSERT;
            emit selectMode(false);
            _insert_component = static_cast<COMPONENT>(QObject::sender()->property("component-type").value<int>());
        }
    }
}
