#ifndef LOGICSIM_GUI_DOC_WINDOW_HPP
#define LOGICSIM_GUI_DOC_WINDOW_HPP

#include <QDir>
#include <QFile>
#include <QMainWindow>
#include <QScreen>
#include <QStandardItemModel>
#include <QTextStream>

#include <unordered_map>

#include "gui/properties.hpp"
#include "gui/resource_loader.hpp"

QT_BEGIN_NAMESPACE
namespace Ui
{
class DocWindow;
}
QT_END_NAMESPACE

namespace logicsim
{
namespace gui
{
class DocWindow : public QMainWindow
{
    Q_OBJECT
  public:
    explicit DocWindow(QWidget *parent = nullptr);
    ~DocWindow();

    void selectItemByFile(const QString &file_path);

  protected:
    Ui::DocWindow      *_ui;
    QStandardItemModel *_model;

    std::unordered_map<QString, QModelIndex> _file_map;

    void _populateModel(QStandardItem *parent_item, const QString &path);

  protected slots:
    void _displayFileContents(const QModelIndex &index);
    void _displayFileContents(const QString &file_path);
};
}
}

#endif // LOGICSIM_GUI_DOC_WINDOW_HPP
