#include "coffeegameeditor.h"
#include "ui_coffeegameeditor.h"

#include <QSplitter>
#include <QTabWidget>
#include <QInputDialog>

CoffeeGameEditor::CoffeeGameEditor(QWidget *parent, QWidget *info, QWidget *inspector, QWidget *scripting) :
    QMainWindow(parent),
    ui(new Ui::CoffeeGameEditor)
{
    ui->setupUi(this);
    QSplitter* inspectSplit = new QSplitter(ui->centralwidget);
    QSplitter* viewSplit = new QSplitter(inspectSplit);

    QTabWidget* inspectionBar = new QTabWidget(inspectSplit);
    inspectionBar->addTab(inspector,"Object inspector");
    inspectionBar->addTab(info,"Renderer");

    inspectSplit->addWidget(scripting);
    inspectSplit->addWidget(inspectionBar);
    inspectSplit->setOrientation(Qt::Horizontal);
    viewSplit->addWidget(scripting);
    viewSplit->setOrientation(Qt::Vertical);
    ui->centralwidget->layout()->addWidget(inspectSplit);
    ui->centralwidget->layout()->setContentsMargins(0,0,0,0);

    ui->actionToolbar->setChecked(true);
}

CoffeeGameEditor::~CoffeeGameEditor()
{
    delete ui;
}

void CoffeeGameEditor::on_actionQuit_triggered()
{
    requestShutdown();
}

void CoffeeGameEditor::on_actionToolbar_toggled(bool arg1)
{
    ui->toolBar->setVisible(arg1);
}
