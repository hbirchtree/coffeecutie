#ifndef COFFEEGAMEEDITOR_H
#define COFFEEGAMEEDITOR_H

#include <QMainWindow>
#include <QGridLayout>
#include <QSplitter>

namespace Ui {
class CoffeeGameEditor;
}

class CoffeeGameEditor : public QMainWindow
{
    Q_OBJECT
public:
    CoffeeGameEditor(QWidget* parent);
    ~CoffeeGameEditor();

public slots:
    void setPrimaryView(QWidget* primary);
    void addInfoTab(QWidget* tab,const QString& name);
    void addInspectorTab(QWidget* tab, const QString &name);

signals:
    void requestShutdown();

private slots:
    void on_actionQuit_triggered();

    void on_actionToolbar_toggled(bool arg1);

private:
    QTabWidget* m_inspectWidget;
    QTabWidget* m_infoWidget;
    QSplitter* m_viewSplit;
    QSplitter* m_inspectSplit;
    Ui::CoffeeGameEditor *ui;
};

#endif // COFFEEGAMEEDITOR_H
