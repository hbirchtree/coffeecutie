#ifndef BLAMDATAVIEWER_H
#define BLAMDATAVIEWER_H

#include <QMainWindow>

class QFileDialog;
class BlamMapHandler;

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

private:
    BlamFileContext *createFileMapping(QString const& file);

    Ui::BlamDataViewer *ui;

    FileDialSelectMode m_file_select;
    QFileDialog* m_file_dialog = nullptr;

    BlamMapHandler* m_map_handle = nullptr;

    BlamFileContext* m_bitmap_file = nullptr;
    BlamFileContext* m_sound_file = nullptr;

    QList<BlamFileContext*> m_opened_files;
};

#endif // BLAMDATAVIEWER_H
