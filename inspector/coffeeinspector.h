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

private slots:
    void updateInformation();

private:
    CoffeeObjectExplorer* m_explorer;
    QTimer *refreshTimer;
    QObjectList engineRoot;

    Ui::CoffeeInspector *ui;

};

#endif // COFFEEINSPECTOR_H
