#ifndef SPELLHIGHLIGHTER_H
#define SPELLHIGHLIGHTER_H

#include <QString>
#include <QSyntaxHighlighter>

class SpellHighlighter : public QSyntaxHighlighter
{
public:
    SpellHighlighter(QTextDocument *ADocument);
    virtual void highlightBlock(const QString &AText);
private:
    QTextCharFormat FCharFormat;
};

#endif //SPELLHIGHLIGHTER_H
