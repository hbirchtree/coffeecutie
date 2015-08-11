#include "coffeemessagelogger.h"
#include "ui_coffeemessagelogger.h"

#include <QTextEdit>
#include <QDateTime>

CoffeeMessageLogger::CoffeeMessageLogger(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CoffeeMessageLogger)
{
    ui->setupUi(this);
    ui->page->deleteLater();
    ui->page_2->deleteLater();
}

CoffeeMessageLogger::~CoffeeMessageLogger()
{
    delete ui;
}

int CoffeeMessageLogger::addMessageTab(const QString &title)
{
    int index = m_tabs.size();

    QTextEdit* ed = new QTextEdit(0);
    ed->setReadOnly(true);
    ed->setAcceptRichText(true);
    ed->document()->setMaximumBlockCount(128);
    ui->toolBox->addItem(ed,title);

    m_tabs.append(ed);

    return index;
}

void CoffeeMessageLogger::submitMessage(int cat, const QString &text)
{
    if(cat>=m_tabs.size()/*||m_nextMessage>=QDateTime::currentMSecsSinceEpoch()*/)
        return;
//    m_nextMessage = QDateTime::currentMSecsSinceEpoch()+10;
    m_tabs.at(cat)->append(text);
}
