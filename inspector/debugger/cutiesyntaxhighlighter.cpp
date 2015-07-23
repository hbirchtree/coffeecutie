#include "cutiesyntaxhighlighter.h"

CutieSyntaxHighlighter::CutieSyntaxHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
    //Cobalt color scheme

    QColor cbStatement("#FFEF92");
    QColor cbFunction("#FF9D00");
    QColor cbNumber("#FF7C9D");
    QColor cbString("#3AD900");
    QColor cbComment("#009DFF");


    //Variable declarations
    varnameFormat.setForeground(cbStatement);
    varnameFormat.setFontWeight(QFont::Bold);

    QStringList vardeclare;
    vardeclare << "\\bvar\\b";

    genRules(vardeclare,varnameFormat);

    //Functions
    funcFormat.setForeground(cbFunction);
    funcFormat.setFontWeight(QFont::Bold);

    QStringList fundeclare;
    fundeclare << "\\bfunction\\b"<< "\\bfor\\b"
                  << "\\bif\\b" << "\\bwhile\\b"
                  << "\\bswitch\\b" << "\\bcase\\b";

    genRules(fundeclare,funcFormat);

    //Comments, single-line
    commentFormat.setForeground(cbComment);
    commentFormat.setFontWeight(QFont::Cursive);

    HighlightScope cmt_single;
    cmt_single.format = commentFormat;
    cmt_single.patt_b = QRegExp("//");
    cmt_single.patt_e = QRegExp("\n");

    HighlightScope cmt_multi;
    cmt_multi.format = commentFormat;
    cmt_multi.patt_b = QRegExp("/*");
    cmt_multi.patt_e = QRegExp("*/");

    rules_scoped.append(cmt_single);
    rules_scoped.append(cmt_multi);
}

void CutieSyntaxHighlighter::highlightBlock(const QString &text)
{
    //Normal rules
    for(const HighlightSet& set : rules){
        QRegExp e(set.patt);
        int idx = e.indexIn(text);
        while(idx>=0){
            int len = e.matchedLength();
            setFormat(idx,len,set.format);
            idx = e.indexIn(text,idx+len);
        }
    }
    //Scope pass
    for(const HighlightScope& set : rules_scoped){
        QRegExp b(set.patt_b);
        QRegExp e(set.patt_e);

        int idx = b.indexIn(text);
        int idx_e = 0;
        while(idx>=0){
            idx_e = e.indexIn(text,idx+b.matchedLength());
            if(idx_e<0)
                break;
            setFormat(idx,idx_e-idx,set.format);
            printf("marking");
            idx = b.indexIn(text,idx_e+e.matchedLength());
        }
    }
}

void CutieSyntaxHighlighter::genRules(const QStringList &src, const QTextCharFormat &fmt)
{
    for(QString s : src){
        HighlightSet set;
        set.format = QTextCharFormat(fmt);
        set.patt = QRegExp(s);
        rules.append(set);
    }
}
