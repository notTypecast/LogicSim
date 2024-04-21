#ifndef PROPERTIES_HPP
#define PROPERTIES_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QPushButton>
#include <QLineEdit>
#include <QString>

#include <functional>

#include "utils.hpp"
#include "gui/resource_loader.hpp"

namespace logicsim
{
    namespace gui
    {
        class ComponentLabel;

        std::vector<QString> splitParamString(std::string param_string);

        class Properties : public QWidget
        {
            Q_OBJECT

        public:
            explicit Properties(QWidget *parent = nullptr);
            ~Properties();

            void setup(ComponentLabel *component);

        protected:
            ComponentLabel *_component;
            QVBoxLayout *_title_group_layout;

            std::function<void()> _close_func = nullptr;

            // adds title
            void _addTitleGroup(QString title);
            // adds group of radio options, out of which one can be selected
            // options: {{option name, param string}, ...}
            void _addExclusiveGroup(QString title, std::vector<std::pair<QString, std::string>> options);
            // adds group of options with user-provided values
            // options: {{option name, current value}, ...}
            // create_params: function with input: vector of option values, output: param string to emit
            void _addValueGroup(QString title, std::vector<std::pair<QString, QString>> options, std::function<std::string(std::vector<QLineEdit *>)> create_params);

        signals:
            void setValue(int value);
            void setParams(std::string params);

        protected slots:
            void done();
        };
    }
}

#endif // PROPERTIES_HPP
