#include "coffeedebugview.h"
#include "ui_coffeedebugview.h"

#include "coffeecodeeditor.h"
#include <QTreeWidget>
#include <QSplitter>
#include <QGridLayout>

CoffeeDebugView::CoffeeDebugView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CoffeeDebugView)
{
    ui->setupUi(this);
    QSplitter* dbgSplit = new QSplitter(this);
    QGridLayout* grid = new QGridLayout(this);
    dbgSplit->setOrientation(Qt::Vertical);

    m_editor = new CoffeeCodeEditor(dbgSplit);
    m_backtree = new QTreeWidget(dbgSplit);
    m_backtree->setHeaderHidden(true);
    grid->addWidget(dbgSplit);
    grid->setContentsMargins(0,0,0,0);
    dbgSplit->addWidget(m_editor);
    dbgSplit->addWidget(m_backtree);

    this->setLayout(grid);
}

CoffeeDebugView::~CoffeeDebugView()
{
    delete ui;
}

CoffeeCodeEditor *CoffeeDebugView::getCodeEditor()
{
    return m_editor;
}

QTreeWidget *CoffeeDebugView::getBacktraceTree()
{
    return m_backtree;
}
