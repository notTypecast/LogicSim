#include "gui/properties.hpp"
#include "gui/component_label.hpp"

namespace logicsim
{
    namespace gui
    {
        std::vector<QString> splitParamString(std::string param_string)
        {
            utils::StringSplitter splitter;
            splitter.reset(param_string, ';');

            std::vector<QString> res;

            while (splitter.has_next())
            {
                res.push_back(QString::fromUtf8(splitter.next().c_str()));
            }

            return res;
        }

        Properties::Properties(QWidget *parent) :
            QWidget(parent)
        {
            setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
        }

        Properties::~Properties()
        {
        }

        void Properties::setup(ComponentLabel *component)
        {
            _component = component;
            QObject::connect(this, SIGNAL (setValue(int)), component, SLOT (setResourceByIdx(int)));
            QObject::connect(this, SIGNAL (setParams(std::string)), component, SLOT (setParams(std::string)));

            switch (component->comp_type())
            {
            case CONSTANT:
                _addTitleGroup("Constant Properties");
                _addExclusiveGroup("Type", {{"0", "0"}, {"1", "1"}});
                break;
            case SWITCH:
                _addTitleGroup("Switch Properties");
                _addExclusiveGroup("State", {{"Off", "0"}, {"On", "1"}});
                break;
            case OSCILLATOR:
            {
                _addTitleGroup("Oscillator Properties");
                std::vector<QString> option_values = splitParamString(component->paramString());
                option_values[1] = QString::number(option_values[1].toInt() - option_values[0].toInt());

                _addValueGroup("Cycle", {{"Low", option_values[0]}, {"High", option_values[1]}},
                [](std::vector<QLineEdit *> params) {
                    bool ok1, ok2;
                    int low = params[0]->text().toInt(&ok1), high = params[1]->text().toInt(&ok2);
                    if (!(ok1 && ok2))
                    {
                        return std::string();
                    }
                    return params[0]->text().toStdString() + ';' + std::to_string(low + high);
                });
                break;
            }
            default:
                break;
            }
        }

        void Properties::done()
        {
            if (_close_func != nullptr)
            {
                _close_func();
            }
            close();
        }

        void Properties::_addTitleGroup(QString title)
        {
            QVBoxLayout *main_layout = new QVBoxLayout(this);
            main_layout->setAlignment(Qt::AlignTop);
            QGroupBox *title_group = new QGroupBox(title, this);
            QVBoxLayout *title_layout = new QVBoxLayout;
            title_group->setLayout(title_layout);
            main_layout->addWidget(title_group);
            _title_group_layout = title_layout;

            QPushButton *ok_button = new QPushButton("OK", this);
            QObject::connect(ok_button, SIGNAL (clicked()), this, SLOT (done()));
            main_layout->addWidget(ok_button);
        }

        void Properties::_addExclusiveGroup(QString title, std::vector<std::pair<QString, std::string>> options)
        {
            int checked_idx = _component->resourceIdx();
            QGroupBox *group = new QGroupBox(title, this);
            QVBoxLayout *layout = new QVBoxLayout;

            int i = 0;
            for (const auto &option_pair : options) {
                QRadioButton *radio_button = new QRadioButton(option_pair.first, this);
                layout->addWidget(radio_button);
                QObject::connect(radio_button, &QRadioButton::clicked, this, [this, i, option_pair]() {
                    emit setValue(i);
                    emit setParams(option_pair.second);
                });
                if (i++ == checked_idx)
                {
                    radio_button->setChecked(true);
                }
            }

            group->setLayout(layout);
            layout->setAlignment(Qt::AlignTop);
            _title_group_layout->addWidget(group);
        }

        void Properties::_addValueGroup(QString title, std::vector<std::pair<QString, QString>> options, std::function<std::string(std::vector<QLineEdit *>)> create_params)
        {
            QGroupBox *group = new QGroupBox(title, this);
            QVBoxLayout *layout = new QVBoxLayout;

            std::vector<QLineEdit *> entries(options.size());

            int i = 0;
            for (const auto & option_pair: options)
            {
                QHBoxLayout *option_layout = new QHBoxLayout;
                option_layout->setAlignment(Qt::AlignTop);

                QLabel *option_name = new QLabel(option_pair.first, this);
                option_layout->addWidget(option_name);
                entries[i] = new QLineEdit(this);
                entries[i]->setText(option_pair.second);
                option_layout->addWidget(entries[i]);

                layout->addLayout(option_layout);
                ++i;
            }

            _close_func = [this, create_params, entries]()
            {
                std::string param_string = create_params(entries);
                if (!param_string.empty())
                {
                    emit setParams(param_string);
                }

            };

            group->setLayout(layout);
            layout->setAlignment(Qt::AlignTop);
            _title_group_layout->addWidget(group);
        }
    }
}
