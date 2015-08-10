#include "coffeegameeditor.h"
#include "ui_coffeegameeditor.h"

#include <QTabWidget>
#include <QInputDialog>

CoffeeGameEditor::CoffeeGameEditor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CoffeeGameEditor)
{
    ui->setupUi(this);
    m_inspectSplit = new QSplitter(ui->centralwidget);
    m_viewSplit = new QSplitter(m_inspectSplit);

    m_inspectWidget = new QTabWidget(m_inspectSplit);

    m_inspectSplit->addWidget(m_viewSplit);
    m_inspectSplit->addWidget(m_inspectWidget);
    m_inspectSplit->setOrientation(Qt::Horizontal);

    m_infoWidget = new QTabWidget(m_viewSplit);

    m_viewSplit->addWidget(m_infoWidget);
    m_viewSplit->setOrientation(Qt::Vertical);
    ui->centralwidget->layout()->addWidget(m_inspectSplit);
    ui->centralwidget->layout()->setContentsMargins(0,0,0,0);

    ui->actionToolbar->setChecked(true);
}

CoffeeGameEditor::~CoffeeGameEditor()
{
    delete ui;
}

void CoffeeGameEditor::setPrimaryView(QWidget *primary)
{
    m_viewSplit->addWidget(primary);
}

void CoffeeGameEditor::addInfoTab(QWidget *tab,const QString& name)
{
    m_infoWidget->addTab(tab,name);
}

void CoffeeGameEditor::addInspectorTab(QWidget *tab,const QString& name)
{
    m_inspectWidget->addTab(tab,name);
}

void CoffeeGameEditor::on_actionQuit_triggered()
{
    requestShutdown();
}

void CoffeeGameEditor::on_actionToolbar_toggled(bool arg1)
{
    ui->toolBar->setVisible(arg1);
}
