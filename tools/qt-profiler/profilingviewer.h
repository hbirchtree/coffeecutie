#ifndef PROFILINGVIEWER_H
#define PROFILINGVIEWER_H

#include <QMainWindow>

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

protected:
    void setupTreeWidget(QTreeWidget* tree);

private slots:
    void on_actionAbout_Qt_triggered();

    void on_actionOpen_file_triggered();

private:
    Ui::ProfilingViewer *ui;
};

#endif // PROFILINGVIEWER_H
