/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
 * Copyright © 2012 Alexey Ivanov <alexey.ivanes@gmail.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "SpellHighlighter.h"
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
