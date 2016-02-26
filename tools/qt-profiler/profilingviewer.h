#ifndef PROFILINGVIEWER_H
#define PROFILINGVIEWER_H

#include <QMainWindow>
#include <QFileDialog>

namespace Ui {
class ProfilingViewer;
}

class QTreeWidget;

class ProfilingViewer : public QMainWindow
{
    Q_OBJECT

public:
    explicit ProfilingViewer(QWidget *parent = 0);
    ~ProfilingViewer();

    void loadProfileLog(const char* fname, QTreeWidget* tree);

protected:
    void setupTreeWidget(QTreeWidget* tree);

protected slots:
    void fileDialogOpenFile(const QString &fn);

private slots:
    void on_actionAbout_Qt_triggered();

    void on_actionOpen_file_triggered();

private:
    Ui::ProfilingViewer *ui;
    QFileDialog m_dlg;
};

#endif // PROFILINGVIEWER_H
