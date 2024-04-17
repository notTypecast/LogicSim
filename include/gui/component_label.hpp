#ifndef LOGICSIM_GUI_COMPONENT_HPP
#define LOGICSIM_GUI_COMPONENT_HPP

#include <QLabel>
#include <QMouseEvent>

namespace logicsim
{
    namespace gui
    {
        class ComponentLabel : public QLabel
        {
            Q_OBJECT
        public:
            explicit ComponentLabel(QWidget *parent = nullptr);

            void setBorder(QLabel *border);
            QLabel *border() const;

            void mouseMoveEvent(QMouseEvent *ev);
            void mousePressEvent(QMouseEvent *ev);
            void mouseReleaseEvent(QMouseEvent *ev);

        protected:
            int _press_x, _press_y;
            bool _select_mode = true;
            QLabel *_border = nullptr;

        signals:
            void selected(ComponentLabel *component, bool ctrl);
            void selected_nocheck(ComponentLabel *component);
            void moved(int x, int y);

        protected slots:
            void selectMode(bool enabled);
            void checkRangeQuery(int min_x, int min_y, int max_x, int max_y);

        };
    }
}

#endif // GUI_COMPONENT_HPP
