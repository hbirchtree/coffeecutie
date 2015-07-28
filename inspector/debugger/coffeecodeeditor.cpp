#include "coffeecodeeditor.h"

#include <QDebug>
#include <QScrollBar>
#include <QAbstractTextDocumentLayout>

CoffeeCodeEditor::CoffeeCodeEditor(QWidget *parent) : QTextEdit(parent)
{
    QString bgcol = "#002240";
    m_bgcolor = QColor(bgcol);
    m_highlighter = new CutieSyntaxHighlighter(this->document());
    m_lineNumberer = new CoffeeLineNumberArea(this);

    setTextColor(QColor("#FFFFFF"));
    setStyleSheet("QTextEdit{background-color:"+bgcol+";color:white}");

    connect(this->document(),SIGNAL(blockCountChanged(int)),SLOT(updateLineNumberWidth(int)));
    connect(this,SIGNAL(cursorPositionChanged()),SLOT(highlightLine()));
    connect(this,&CoffeeCodeEditor::textChanged,[=](){
        updateLineNumberWidth(0);
        m_lineNumberer->repaint();
    });
    connect(this->verticalScrollBar(),&QScrollBar::valueChanged,[=](int val){
        Q_UNUSED(val)
        m_lineNumberer->repaint();
    });
}

void CoffeeCodeEditor::lineNumberPaint(QPaintEvent *ev)
{
    QPainter p(m_lineNumberer);
    p.fillRect(ev->rect(),m_bgcolor);

    p.setPen(QColor(Qt::white).darker(140));

    QTextBlock block = document()->firstBlock();

    int lineNum = 0;

    QRectF r1 = QRectF(this->viewport()->geometry());

    while(block.isValid()){
        QRectF r2 = this->document()->documentLayout()->blockBoundingRect(block)
                .translated(r1.x(),
                            r1.y()-this->verticalScrollBar()->sliderPosition()).toRect();
        lineNum++;
        if(r1.contains(r2)){
            QRectF dr(0.0,r2.top(),(qreal)getLineNumberAreaWidth(),(qreal)fontMetrics().height());
            p.drawText(dr,Qt::AlignRight,QString::number(lineNum));
        }
        block = block.next();
    }
}

int CoffeeCodeEditor::getLineNumberAreaWidth() const
{
    int digs = 1;
    int max = qMax(1,document()->blockCount());
    while(max >= 10){
        max /= 10;
        digs++;
    }
    return 3 + fontMetrics().width(QLatin1Char('9')) * digs;
}

void CoffeeCodeEditor::addErrorLine(int line)
{
    colorizeLine(QColor("#D25252"),line);
}

void CoffeeCodeEditor::addDebugLine(int line)
{
    colorizeLine(QColor("#FFEE80"),line);
}

void CoffeeCodeEditor::updateLineNumberWidth(int bcount)
{
    Q_UNUSED(bcount)
    setViewportMargins(getLineNumberAreaWidth(),0,0,0);
}

void CoffeeCodeEditor::highlightLine()
{
    if(!isReadOnly()){
        QColor lineCol = m_bgcolor.lighter(145);

        lineSelect.format.setBackground(lineCol);
        lineSelect.format.setProperty(QTextFormat::FullWidthSelection,true);
        lineSelect.cursor = textCursor();
        lineSelect.cursor.clearSelection();
    }
    updateLineSelections();
}

void CoffeeCodeEditor::resizeEvent(QResizeEvent *ev)
{
    QTextEdit::resizeEvent(ev);

    QRect cr = contentsRect();
    m_lineNumberer->setGeometry(QRect(cr.left(),cr.top(),getLineNumberAreaWidth(),cr.height()));
}

void CoffeeCodeEditor::updateLineSelections()
{
    setExtraSelections(QList<ExtraSelection>() << lineSelect << debugLines << errorLines);
}

void CoffeeCodeEditor::colorizeLine(const QColor &col, uint32_t line)
{
    if(!isReadOnly()){
        ExtraSelection lineSelect;
        lineSelect.format.setBackground(col);
        lineSelect.format.setProperty(QTextFormat::FullWidthSelection,true);
        lineSelect.cursor = textCursor();
        lineSelect.cursor.setPosition(lineToOffset(line));
        lineSelect.cursor.clearSelection();
        debugLines.append(lineSelect);
    }
    updateLineSelections();
}

uint64_t CoffeeCodeEditor::lineToOffset(uint32_t line)
{
    uint64_t sum = 0;
    uint32_t i = 0;
    QStringList lines = this->document()->toPlainText().split("\n");
    if(line>(uint32_t)lines.size())
        return this->document()->toPlainText().size();
    while(i<line&&i<(uint32_t)lines.size()){
        sum+=lines[i].size()+1; //+1 for newline character which is omitted in this case
        i++;
    }
    return sum-1;
}

CoffeeLineNumberArea::CoffeeLineNumberArea(CoffeeCodeEditor *parent) : QWidget(parent)
{
    m_editor = parent;
}

QSize CoffeeLineNumberArea::sizeHint() const
{
    return QSize(m_editor->getLineNumberAreaWidth(),0);
}

void CoffeeLineNumberArea::paintEvent(QPaintEvent *ev)
{
    m_editor->lineNumberPaint(ev);
}
