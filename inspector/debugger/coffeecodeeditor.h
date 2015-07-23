#ifndef COFFEECODEEDITOR_H
#define COFFEECODEEDITOR_H

#include <QTextEdit>
#include <QPainter>
#include <QPaintEvent>
#include <QTextBlock>
#include "cutiesyntaxhighlighter.h"

class CoffeeCodeEditor;

class CoffeeLineNumberArea : public QWidget
{
public:
    CoffeeLineNumberArea(CoffeeCodeEditor* parent = 0);
    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent *ev);

private:
    CoffeeCodeEditor* m_editor;
};

class CoffeeCodeEditor : public QTextEdit
{
    Q_OBJECT
public:
    CoffeeCodeEditor(QWidget *parent = 0);

    void lineNumberPaint(QPaintEvent* ev);
    int getLineNumberAreaWidth() const;

public slots:
    void addErrorLine(int line);
    void addDebugLine(int line);

protected slots:
    void updateLineNumberWidth(int bcount);
    void highlightLine();

protected:
    void resizeEvent(QResizeEvent *ev);

    void updateLineSelections();

    void colorizeLine(const QColor &col, uint32_t line);
    uint64_t lineToOffset(uint32_t line);

private:
    QColor m_bgcolor;
    QTextEdit::ExtraSelection lineSelect;
    QList<QTextEdit::ExtraSelection> errorLines;
    QList<QTextEdit::ExtraSelection> debugLines;

    CoffeeLineNumberArea* m_lineNumberer;
    CutieSyntaxHighlighter* m_highlighter;
};

#endif // COFFEECODEEDITOR_H
