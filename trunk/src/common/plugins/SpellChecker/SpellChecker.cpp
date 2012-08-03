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

#include <QTimer>

#include "ChatCore.h"
#include "ChatSettings.h"
#include "sglobal.h"
#include "SpellChecker.h"
#include "SpellCheckerPage.h"
#include "ui/InputWidget.h"
#include "ui/SendWidget.h"

SpellChecker* SpellChecker::m_self = 0;

SpellChecker::SpellChecker(QObject *parent)
  : ChatPlugin(parent)
  , m_textEdit(0)
  , FCurrentCursorPosition(0)
{
  m_self = this;

  QStringList defaultDicts;
  defaultDicts.append("en_US");
  defaultDicts.append(QLocale().name().toUtf8().constData());

  ChatCore::settings()->setLocalDefault(LS("SpellChecker/EnabledDicts"), defaultDicts);
  SettingsTabHook::add(new SpellCheckerPageCreator());

  QTimer::singleShot(0, this, SLOT(start()));
}


QMenu* SpellChecker::suggestMenu(const QString &word)
{
    QList<QString> sgstions = SpellBackend::instance()->suggestions(word);
    QMenu *menu = new QMenu(tr("Suggestions"));

    for (QList<QString>::const_iterator sgstion = sgstions.begin(); sgstion != sgstions.end(); ++sgstion)
    {
        QAction *action = menu->addAction(*sgstion, this, SLOT(repairWord()));
        action->setProperty("word", *sgstion);
        action->setParent(menu);
    }

    return menu;
}

void SpellChecker::showContextMenu(const QPoint &pt)
{
  m_textEdit = qobject_cast<InputWidget *>(sender());
    Q_ASSERT(m_textEdit);

    QMenu *menu = m_textEdit->createStandardContextMenu();

    menu->addSeparator();

    QMenu *sugMenu = NULL;
    Q_ASSERT(!sugMenu);

    QTextCursor cursor = m_textEdit->cursorForPosition(pt);
    FCurrentCursorPosition = cursor.position();
    cursor.select(QTextCursor::WordUnderCursor);
    const QString word = cursor.selectedText();

    if (!word.isEmpty() && !SpellBackend::instance()->isCorrect(word)) {
        sugMenu = suggestMenu(word);

        if (!sugMenu->isEmpty()) {
            menu->addMenu(sugMenu);
        }

        QAction *action = menu->addAction(tr("Add to dictionary"), this, SLOT(addWordToDict()));
        action->setParent(menu);
    }

    menu->exec(m_textEdit->mapToGlobal(pt));

    if (sugMenu) {
        delete sugMenu;
    }
    delete menu;
}

void SpellChecker::repairWord()
{
    QAction *action = qobject_cast<QAction *>(sender());
    Q_ASSERT(action);
    if (!action)
    {
        return;
    }

    QTextCursor cursor = m_textEdit->textCursor();

    cursor.beginEditBlock();
    cursor.setPosition(FCurrentCursorPosition, QTextCursor::MoveAnchor);
    cursor.select(QTextCursor::WordUnderCursor);
    cursor.removeSelectedText();
    cursor.insertText(action->property("word").toString());
    cursor.endEditBlock();

#if QT_VERSION >= 0x040600 // Qt 4.5
    FSH->rehighlightBlock(cursor.block());
#else
    FSH->rehighlight();
#endif
}

void SpellChecker::setEnabledDicts(QList<QString> &dicts)
{
    delete FSH;
    SpellBackend::instance()->setLangs(dicts);
    InputWidget *textEdit = SendWidget::i()->input();
    FSH = new SpellHighlighter(textEdit->document());
    FSH->rehighlight();
}

void SpellChecker::addWordToDict()
{
    QAction *action = qobject_cast<QAction *>(sender());
    Q_ASSERT(action);
    if (!action)
    {
        return;
    }

    QTextCursor cursor = m_textEdit->textCursor();
    cursor.setPosition(FCurrentCursorPosition, QTextCursor::MoveAnchor);
    cursor.select(QTextCursor::WordUnderCursor);
    const QString word = cursor.selectedText();

    SpellBackend::instance()->add(word);

#if QT_VERSION >= 0x040600 // Qt 4.5
    FSH->rehighlightBlock(cursor.block());
#else
    FSH->rehighlight();
#endif
}


void SpellChecker::start()
{
  InputWidget *textEdit = SendWidget::i()->input();
  textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(textEdit, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showContextMenu(const QPoint &)));

  FSH = new SpellHighlighter(textEdit->document());
}

