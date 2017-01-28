#ifndef BLAMDATAVIEWER_H
#define BLAMDATAVIEWER_H

#include <QMainWindow>

class QFileDialog;
class QTreeWidgetItem;

class BlamMapHandler;

namespace Coffee{
namespace Blam{
struct index_item_t;
}
}

namespace Ui {
class BlamDataViewer;
}

class BlamDataViewer : public QMainWindow
{
    struct BlamFileContext;

    enum FileDialSelectMode
    {
        MapSelect,
        BitmapSelect,
        SoundSelect,
    };

    Q_OBJECT

public:
    explicit BlamDataViewer(QWidget *parent = 0);
    ~BlamDataViewer();

private slots:
    void on_actionOpen_map_triggered();
    void on_actionImport_bitmap_triggered();
    void on_actionImport_sounds_triggered();

    void openFileDialog(FileDialSelectMode mode);
    void handleFileDialogOpen(QString const& filename);

    void openMapFile(QString const& filename);
    void openBitmap(QString const& filename);
    void openSound(QString const& filename);

    void openMap(BlamFileContext* map);

    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int);

private:
    BlamFileContext *createFileMapping(QString const& file);

    Ui::BlamDataViewer *ui;

    FileDialSelectMode m_file_select;
    QFileDialog* m_file_dialog = nullptr;

    BlamMapHandler* m_map_handle = nullptr;

    BlamFileContext* m_bitmap_file = nullptr;
    BlamFileContext* m_sound_file = nullptr;

    QList<BlamFileContext*> m_opened_files;

    QVector<QImage> m_textures_ref;
    QVector<const Coffee::Blam::index_item_t *> m_textures_name_ref;
};

#endif // BLAMDATAVIEWER_H
