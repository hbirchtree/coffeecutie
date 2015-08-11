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
    CoffeeInspector(QWidget *parent = 0);
    ~CoffeeInspector();


public slots:
    void setExplorer(CoffeeObjectExplorer *explorer);
    void updateInformation();

private:
    CoffeeObjectExplorer* m_explorer;
    QTimer *refreshTimer;

    Ui::CoffeeInspector *ui;

};

#endif // COFFEEINSPECTOR_H
