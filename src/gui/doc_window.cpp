#include "gui/doc_window.hpp"
#include "ui_doc_window.h"

namespace logicsim
{
namespace gui
{
DocWindow::DocWindow(QWidget *parent)
  : QMainWindow { parent }
  , _ui(new Ui::DocWindow)
{
    _ui->setupUi(this);

    QScreen *screen          = QGuiApplication::primaryScreen();
    QRect    screen_geometry = screen->geometry();

    int x = (screen_geometry.width() - width()) / 2;
    int y = (screen_geometry.height() - height()) / 2;

    move(x, y);

    _model = new QStandardItemModel;

    _model->setHorizontalHeaderLabels({ "Sections" });

    QStandardItem *root_item = _model->invisibleRootItem();

    _populateModel(root_item, resources::DOC_PATH);

    _ui->browseView->setModel(_model);

    _ui->browseView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(_ui->browseView,
            &QAbstractItemView::clicked,
            this,
            qOverload<const QModelIndex &>(&DocWindow::_displayFileContents));

    _displayFileContents(resources::DOC_PATH + "00 Introduction.html");
}

DocWindow::~DocWindow()
{
    delete _ui;
    delete _model;
}

void DocWindow::selectItemByFile(const QString &file_path)
{
    const QModelIndex &index = _file_map[file_path];
    _ui->browseView->setCurrentIndex(index);

    QModelIndex parent_index = index.parent();
    while (parent_index.isValid())
    {
        _ui->browseView->expand(parent_index);
        parent_index = parent_index.parent();
    }

    _ui->browseView->scrollTo(index);

    _displayFileContents(index);
}

void DocWindow::_populateModel(QStandardItem *parent_item, const QString &path)
{
    QDir dir(path);

    QFileInfoList entries =
      dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);

    for (const QFileInfo &entry : entries)
    {
        if (entry.isDir())
        {
            QString        title = entry.fileName();
            QStandardItem *folder_item =
              new QStandardItem(title.mid(title.indexOf(" ") + 1));
            parent_item->appendRow(folder_item);

            _populateModel(folder_item, entry.filePath());
        }
        else if (entry.suffix() == "html")
        {
            QString        title = entry.baseName();
            QStandardItem *file_item =
              new QStandardItem(title.mid(title.indexOf(" ") + 1));
            file_item->setData(entry.filePath());
            parent_item->appendRow(file_item);

            _file_map[entry.filePath()] = _model->indexFromItem(file_item);
        }
    }
}

void DocWindow::_displayFileContents(const QModelIndex &index)
{
    QStandardItem *item      = _model->itemFromIndex(index);
    QString        file_path = item->data().toString();
    if (file_path.isEmpty())
    {
        return;
    }

    _displayFileContents(file_path);
}

void DocWindow::_displayFileContents(const QString &file_path)
{
    QFile file(file_path);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        QString     html_content = stream.readAll();
        file.close();

        html_content.replace("[BASE_DOC_PATH]", resources::DOC_PATH);

        _ui->htmlViewer->document()->setHtml(html_content);
    }
}
}
}
