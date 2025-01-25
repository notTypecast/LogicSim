#include "gui/properties.hpp"
#include "gui/component_label.hpp"
#include "gui/main_window.hpp"

namespace logicsim
{
namespace gui
{
Properties::Properties(QString title, QWidget *parent)
  : Properties(title, "", parent)
{
}

Properties::Properties(QString title, const QString &doc_file_path,
                       QWidget *parent)
  : QWidget(parent)
{
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);

    QVBoxLayout *main_layout = new QVBoxLayout(this);
    main_layout->setAlignment(Qt::AlignTop);

    QHBoxLayout *title_layout = new QHBoxLayout;
    main_layout->addLayout(title_layout);

    QLabel *title_label = new QLabel(title, this);
    title_layout->addWidget(title_label);

    QPushButton *help_button = new QPushButton(this);
    help_button->setIcon(
      QApplication::style()->standardIcon(QStyle::SP_MessageBoxQuestion));
    help_button->setFixedWidth(30);
    connect(help_button,
            &QPushButton::clicked,
            [this, doc_file_path]()
            {
                QObject *current = this;

                while (current->parent() != nullptr)
                {
                    current = current->parent();
                }

                dynamic_cast<MainWindow *>(current)->showDocs(doc_file_path);
            });
    title_layout->addWidget(help_button);

    _group_box              = new QGroupBox(this);
    QVBoxLayout *box_layout = new QVBoxLayout;
    _group_box->setLayout(box_layout);
    main_layout->addWidget(_group_box);
    _title_group_layout = box_layout;
    _group_box->hide();

    QPushButton *ok_button = new QPushButton("OK", this);
    connect(ok_button, &QPushButton::clicked, this, &Properties::done);
    main_layout->addWidget(ok_button);
}

Properties::~Properties() {}

void Properties::done()
{
    for (const auto &func : _close_funcs)
    {
        func();
    }
    close();
}

void Properties::addValueEntry(QString option_name, QString option_value,
                               std::function<QString(QLineEdit *)> create_value,
                               QString                             after)
{
    if (_group_box->isHidden())
    {
        _group_box->show();
    }

    int          group_idx = _groups++;
    QHBoxLayout *layout    = new QHBoxLayout;

    QLabel      *name_label = new QLabel(option_name, this);
    QFontMetrics fm(name_label->font());
    name_label->setFixedWidth(fm.horizontalAdvance(option_name) +
                              VALUE_ENTRY_OFFSET);
    layout->addWidget(name_label);

    QLineEdit *entry = new QLineEdit(this);
    entry->setText(option_value);
    layout->addWidget(entry);

    QLabel *after_label = new QLabel(after, this);
    layout->addWidget(after_label);

    _close_funcs.push_back(
      [this, group_idx, create_value, entry]()
      {
          QString value_string = create_value(entry);
          if (!value_string.isEmpty())
          {
              emit optionValue(value_string, group_idx);
          }
      });

    layout->setAlignment(Qt::AlignTop);
    _title_group_layout->addLayout(layout);
}

void Properties::addExclusiveGroup(
  QString title, std::vector<std::pair<QString, QString>> options,
  int init_checked_idx)
{
    if (_group_box->isHidden())
    {
        _group_box->show();
    }

    int          group_idx   = _groups++;
    int          checked_idx = init_checked_idx;
    QGroupBox   *group       = new QGroupBox(title, this);
    QVBoxLayout *layout      = new QVBoxLayout;

    int i = 0;
    for (const auto &option_pair : options)
    {
        QRadioButton *radio_button = new QRadioButton(option_pair.first, this);
        layout->addWidget(radio_button);
        connect(radio_button,
                &QRadioButton::clicked,
                this,
                [this, group_idx, i, option_pair]()
                {
                    emit optionIndex(i, group_idx);
                    emit optionValue(option_pair.second, group_idx);
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

void Properties::addValueGroup(
  QString title, std::vector<std::pair<QString, QString>> options,
  std::function<QString(std::vector<QLineEdit *>)> create_value)
{
    if (_group_box->isHidden())
    {
        _group_box->show();
    }

    int          group_idx = _groups++;
    QGroupBox   *group     = new QGroupBox(title, this);
    QVBoxLayout *layout    = new QVBoxLayout;

    std::vector<QLineEdit *> entries(options.size());

    int                   max_label_width = -1;
    std::vector<QLabel *> option_labels(options.size());

    int i = 0;
    for (const auto &option_pair : options)
    {
        QHBoxLayout *option_layout = new QHBoxLayout;
        option_layout->setAlignment(Qt::AlignTop);

        QLabel *option_name = new QLabel(option_pair.first, this);
        option_layout->addWidget(option_name);

        option_labels[i] = option_name;
        QFontMetrics fm(option_name->font());
        max_label_width =
          std::max(max_label_width, fm.horizontalAdvance(option_pair.first));

        entries[i] = new QLineEdit(this);
        entries[i]->setText(option_pair.second);
        option_layout->addWidget(entries[i]);

        layout->addLayout(option_layout);
        ++i;
    }

    for (const auto &option_name : option_labels)
    {
        option_name->setFixedWidth(max_label_width + VALUE_ENTRY_OFFSET);
    }

    _close_funcs.push_back(
      [this, group_idx, create_value, entries]()
      {
          QString value_string = create_value(entries);
          if (!value_string.isEmpty())
          {
              emit optionValue(value_string, group_idx);
          }
      });

    group->setLayout(layout);
    layout->setAlignment(Qt::AlignTop);
    _title_group_layout->addWidget(group);
}
}
}
