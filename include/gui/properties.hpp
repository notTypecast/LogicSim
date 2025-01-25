#ifndef LOGICSIM_GUI_PROPERTIES_HPP
#define LOGICSIM_GUI_PROPERTIES_HPP

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QString>
#include <QStyle>
#include <QVBoxLayout>
#include <QWidget>

#include <functional>

#include "gui/doc_window.hpp"
#include "gui/resource_loader.hpp"

#include "utils.hpp"

namespace logicsim
{
namespace gui
{
class ComponentLabel;

class Properties : public QWidget
{
    Q_OBJECT

  public:
    explicit Properties(QString title, const QString &doc_file_path,
                        QWidget *parent = nullptr);
    explicit Properties(QString title, QWidget *parent = nullptr);
    ~Properties();

    // adds a single option for a user-provided value
    // this counts as an individual group
    // create_value: function to create value to be emitted by optionValue based
    // on entry after: text to place to the right of text entry
    void addValueEntry(QString option_name, QString option_value,
                       std::function<QString(QLineEdit *)> create_value,
                       QString                             after = "");
    // adds group of radio options, out of which one can be selected
    // options: {{option name, option value}, ...}
    // when an option is selected, emits optionIndex with its index, and
    // optionValue with its value
    void addExclusiveGroup(QString                                  title,
                           std::vector<std::pair<QString, QString>> options,
                           int init_checked_idx = 0);
    // adds group of options with user-provided values
    // options: {{option name, option value}, ...}
    // create_value: function with input: vector of option values, output:
    // string to emit when OK is pressed, emits optionValue with a string
    // created using create_value
    void addValueGroup(
      QString title, std::vector<std::pair<QString, QString>> options,
      std::function<QString(std::vector<QLineEdit *>)> create_value);

  protected:
    unsigned int _groups             = 0;
    QVBoxLayout *_title_group_layout = nullptr;
    QGroupBox   *_group_box;

    std::vector<std::function<void()>> _close_funcs;

    static const size_t VALUE_ENTRY_OFFSET = 25;

  signals:
    void optionIndex(int option_idx, int group_idx);
    void optionValue(QString value, int group_idx);

  protected slots:
    void done();
};
}
}

#endif // LOGICSIM_GUI_PROPERTIES_HPP
