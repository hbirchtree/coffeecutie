#ifndef MAINWINDOWTESTER_H
#define MAINWINDOWTESTER_H

#include <QMainWindow>

namespace Ui {
class MainWindowTester;
}

class MainWindowTester : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindowTester(QWidget *parent = 0);
    ~MainWindowTester();

    void addWidget(QWidget* w);

private:
    Ui::MainWindowTester *ui;
};

#endif // MAINWINDOWTESTER_H
