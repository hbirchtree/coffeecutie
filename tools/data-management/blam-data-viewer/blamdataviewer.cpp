#include "blamdataviewer.h"
#include "ui_blamdataviewer.h"

#include <QTreeWidgetItem>
#include <QFileDialog>
#include <QString>
#include <QtDebug>

#include <blammaphandler.h>

struct BlamDataViewer::BlamFileContext
{
    QFile file;
    uchar* data;
};

BlamDataViewer::BlamDataViewer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BlamDataViewer),
    m_file_dialog(nullptr)
{
    ui->setupUi(this);
}

BlamDataViewer::~BlamDataViewer()
{
    delete ui;
}

void BlamDataViewer::on_actionOpen_map_triggered()
{
    openFileDialog(MapSelect);
}

void BlamDataViewer::on_actionImport_bitmap_triggered()
{
    openFileDialog(BitmapSelect);
}

void BlamDataViewer::on_actionImport_sounds_triggered()
{
    openFileDialog(SoundSelect);
}

void BlamDataViewer::openFileDialog(BlamDataViewer::FileDialSelectMode mode)
{
    m_file_select = mode;

    if(m_file_dialog)
    {
        m_file_dialog->deleteLater();
    }

    m_file_dialog = new QFileDialog;
    connect(m_file_dialog,&QFileDialog::fileSelected,this,&BlamDataViewer::handleFileDialogOpen);

    m_file_dialog->setNameFilter("*.map");
    if(mode == MapSelect)
        m_file_dialog->setWindowTitle(tr("Select map file to open"));
    else if(mode == BitmapSelect)
        m_file_dialog->setWindowTitle(tr("Select bitmaps.map file"));
    else if(mode == SoundSelect)
        m_file_dialog->setWindowTitle(tr("Select sounds.map file"));

    m_file_dialog->show();
}

void BlamDataViewer::handleFileDialogOpen(const QString &filename)
{
    switch(m_file_select)
    {
    case MapSelect:
        openMapFile(filename);
        break;
    case BitmapSelect:
        openBitmap(filename);
        break;
    case SoundSelect:
        openSound(filename);
        break;
    }
}

void BlamDataViewer::openMapFile(const QString &filename)
{
    m_opened_files.append(createFileMapping(filename));
    openMap(m_opened_files.first());
}

void BlamDataViewer::openBitmap(const QString &filename)
{
    if(!m_bitmap_file)
        m_bitmap_file = createFileMapping(filename);
}

void BlamDataViewer::openSound(const QString &filename)
{
    if(!m_sound_file)
        m_sound_file = createFileMapping(filename);
}

void BlamDataViewer::openMap(BlamFileContext *map)
{
    m_map_handle = new BlamMapHandler(map->data);

    if(m_map_handle->valid() && m_bitmap_file)
    {
        m_map_handle->allTextures(m_bitmap_file->data,&m_textures_ref);
//        qDebug("Number of textures: %i",images.size());

        if(m_textures_ref.size())
        {
            ui->textureViewer->setPixmap(QPixmap::fromImage(m_textures_ref.first()));
        }
    }
}

BlamDataViewer::BlamFileContext *BlamDataViewer::createFileMapping(const QString &file)
{
    BlamFileContext* m = new BlamFileContext;
    m->file.setFileName(file);

    if(!m->file.open(QFile::ReadOnly))
    {
        delete m;
        return nullptr;
    }

    m->data = m->file.map(0,m->file.size());

    if(!m->data)
    {
        delete m;
        return nullptr;
    }

    return m;
}

void BlamDataViewer::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int)
{
}
