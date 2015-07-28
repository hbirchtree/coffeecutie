#ifndef COFFEEINSPECTOR_H
#define COFFEEINSPECTOR_H

#include <QWidget>
#include <QTimer>

class CoffeeObjectExplorer;

namespace Ui {
class CoffeeInspector;
}

class CoffeeInspector : public QWidget
{
    Q_OBJECT

public:
    CoffeeInspector(QWidget *parent = 0,
                    QObjectList engineRoot = QObjectList());
    ~CoffeeInspector();

signals:
    void showTerminal();
    void showInformation();

private slots:
    void updateInformation();

    void on_rendererBtn_clicked();

    void on_pushButton_clicked();

private:
    CoffeeObjectExplorer* m_explorer;
    QTimer *refreshTimer;
    QObjectList engineRoot;

    Ui::CoffeeInspector *ui;

};

#endif // COFFEEINSPECTOR_H
