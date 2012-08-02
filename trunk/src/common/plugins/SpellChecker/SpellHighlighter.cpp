#include "SpellHighlighter.h"
#include "SpellcheckerPlugin_p.h"
#include "SpellBackend.h"

SpellHighlighter::SpellHighlighter(QTextDocument *ADocument) : QSyntaxHighlighter(ADocument)
{
    FCharFormat.setUnderlineColor(Qt::red);
    FCharFormat.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
}

void SpellHighlighter::highlightBlock(const QString &AText)
{
    // Match words (minimally) excluding digits within a word
    static const QRegExp expression("\\b[^\\s\\d]+\\b");

    int index = 0;
    while ((index = expression.indexIn(AText, index)) != -1)
    {
        int length = expression.matchedLength();
        if (!SpellBackend::instance()->isCorrect(expression.cap()))
        {
            setFormat(index, length, FCharFormat);
        }

        index += length;
    }
}
