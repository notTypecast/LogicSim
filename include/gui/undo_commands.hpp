#ifndef LOGICSIM_GUI_UNDO_COMMANDS_HPP
#define LOGICSIM_GUI_UNDO_COMMANDS_HPP

#include <QUndoCommand>

#include "gui/design_area.hpp"
#include "gui/component_label.hpp"
#include "gui/wire.hpp"

namespace logicsim
{
    namespace gui
    {
        class InsertComponentCommand : public QUndoCommand
        {
        public:
            InsertComponentCommand(DesignArea *design_area, QMouseEvent *ev);
            InsertComponentCommand(DesignArea *design_area, ComponentLabel *component);
            ~InsertComponentCommand();

            void redo();
            void undo();

            ComponentLabel *component() const;

        protected:
            DesignArea *_design_area;
            ComponentLabel *_component;
            bool _first_time = true;
        };

        class DeleteComponentsCommand : public QUndoCommand
        {
        public:
            DeleteComponentsCommand(DesignArea *design_area, std::vector<ComponentLabel *> &deleted_components);

            void redo();
            void undo();

        protected:
            DesignArea *_design_area;
            std::vector<ComponentLabel *> _deleted_components;
        };

        class InsertWireCommand : public QUndoCommand
        {
        public:
            InsertWireCommand(DesignArea *design_area, Wire *wire);
            ~InsertWireCommand();

            void redo();
            void undo();

            bool connected() const;

        protected:
            DesignArea *_design_area;
            Wire *_wire;
            bool _conn_success;
            bool _disconnected = false;
            bool _first_time = true;
        };

        class DeleteWireCommand : public QUndoCommand
        {
        public:
            DeleteWireCommand(DesignArea *design_area, Wire *wire);

            void redo();
            void undo();

        protected:
            DesignArea *_design_area;
            Wire *_wire;
        };

        class MoveComponentsCommand : public QUndoCommand
        {
        public:
            MoveComponentsCommand(std::vector<ComponentLabel *> &moved_components, std::vector<QPoint> init_positions, std::vector<QPoint> final_positions);

            void redo();
            void undo();

        protected:
            std::vector<ComponentLabel *> _moved_components;
            std::vector<QPoint> _init_positions;
            std::vector<QPoint> _final_positions;
        };

        class ChangeComponentPropertyCommand : public QUndoCommand
        {
        public:
            ChangeComponentPropertyCommand(ComponentLabel *component, int prev_res_idx = -1, int _res_idx = -1);

            void setParams(const QString &prev_params, const QString &params);

            void redo();
            void undo();

        protected:
            ComponentLabel *_component;
            int _prev_res_idx, _res_idx;
            QString _prev_params, _params;
        };

        class ChangeSimulationPropertiesCommand : public QUndoCommand
        {
        public:
            ChangeSimulationPropertiesCommand(DesignArea *design_area, int prev_freq, int freq);

            void redo();
            void undo();

        protected:
            DesignArea *_design_area;
            int _prev_freq, _freq;
        };
    }
}

#endif // LOGICSIM_GUI_UNDO_COMMANDS_HPP
