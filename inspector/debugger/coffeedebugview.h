#ifndef COFFEEDEBUGVIEW_H
#define COFFEEDEBUGVIEW_H

#include <QWidget>

class CoffeeCodeEditor;
class QTreeWidget;

namespace Ui {
class CoffeeDebugView;
}

class CoffeeDebugView : public QWidget
{
    Q_OBJECT

public:
    explicit CoffeeDebugView(QWidget *parent = 0);
    ~CoffeeDebugView();

    CoffeeCodeEditor *getCodeEditor();
    QTreeWidget *getBacktraceTree();
private:
    QTreeWidget* m_backtree;
    CoffeeCodeEditor* m_editor;
    Ui::CoffeeDebugView *ui;
};

#endif // COFFEEDEBUGVIEW_H
