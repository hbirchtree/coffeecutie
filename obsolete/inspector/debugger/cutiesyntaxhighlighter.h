#ifndef CUTIESYNTAXHIGHLIGHTER_H
#define CUTIESYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegExp>
#include <QTextCharFormat>

class CutieSyntaxHighlighter : public QSyntaxHighlighter
{
public:
    CutieSyntaxHighlighter(QTextDocument* parent);

    // QSyntaxHighlighter interface
protected:
    void highlightBlock(const QString &text);

private:
    struct HighlightSet {
        QRegExp patt;
        QTextCharFormat format;
    };
    struct HighlightScope {
        QRegExp patt_b;
        QRegExp patt_e;
        QTextCharFormat format;
    };

    QVector<HighlightScope> rules_scoped;
    QVector<HighlightSet> rules;

    void genRules(const QStringList& src, const QTextCharFormat& fmt);

    QTextCharFormat commentFormat;
    QTextCharFormat varnameFormat;
    QTextCharFormat funcFormat;
    QTextCharFormat classFormat;
    QTextCharFormat constructFormat;
    QTextCharFormat stringFormat;
    QTextCharFormat numberFormat;
};

#endif // CUTIESYNTAXHIGHLIGHTER_H
