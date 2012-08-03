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
#include <QMenu>

#include "ChatCore.h"
#include "ChatSettings.h"
#include "sglobal.h"
#include "SpellChecker.h"
#include "SpellCheckerPage.h"
#include "SpellHighlighter.h"
#include "ui/InputWidget.h"
#include "ui/SendWidget.h"

SpellChecker* SpellChecker::m_self = 0;

SpellChecker::SpellChecker(QObject *parent)
  : ChatPlugin(parent)
  , m_textEdit(0)
  , m_position(0)
  , m_highlighter(0)
{
  m_self = this;

  QStringList defaultDicts;
  defaultDicts.append("en_US");
  defaultDicts.append(QLocale().name().toUtf8().constData());

  ChatCore::settings()->setLocalDefault(LS("SpellChecker/EnabledDicts"), defaultDicts);
  SettingsTabHook::add(new SpellCheckerPageCreator());

  QTimer::singleShot(0, this, SLOT(start()));
}


void SpellChecker::contextMenu(QMenu *menu, const QPoint &pos)
{
  menu->addSeparator();

  QTextCursor cursor = m_textEdit->cursorForPosition(pos);
  m_position = cursor.position();
  cursor.select(QTextCursor::WordUnderCursor);
  QString word = cursor.selectedText();

  if (word.isEmpty() || SpellBackend::instance()->isCorrect(word))
    return;

  suggestionsMenu(word, menu);

  QAction *action = menu->addAction(tr("Add to dictionary"), this, SLOT(addWordToDict()));
  action->setParent(menu);
}


/*!
 * Замена слова.
 */
void SpellChecker::repairWord()
{
  QAction *action = qobject_cast<QAction *>(sender());
  if (!action)
    return;

  QTextCursor cursor = m_textEdit->textCursor();

  cursor.beginEditBlock();
  cursor.setPosition(m_position, QTextCursor::MoveAnchor);
  cursor.select(QTextCursor::WordUnderCursor);
  cursor.removeSelectedText();
  cursor.insertText(action->text());
  cursor.endEditBlock();

  m_highlighter->rehighlightBlock(cursor.block());
}


void SpellChecker::setEnabledDicts(QList<QString> &dicts)
{
  if (m_highlighter)
    delete m_highlighter;

  SpellBackend::instance()->setLangs(dicts);
  m_highlighter = new SpellHighlighter(m_textEdit->document());
  m_highlighter->rehighlight();
}


void SpellChecker::addWordToDict()
{
  QAction *action = qobject_cast<QAction *>(sender());
  if (!action)
    return;

  QTextCursor cursor = m_textEdit->textCursor();
  cursor.setPosition(m_position, QTextCursor::MoveAnchor);
  cursor.select(QTextCursor::WordUnderCursor);
  const QString word = cursor.selectedText();

  SpellBackend::instance()->add(word);

  m_highlighter->rehighlightBlock(cursor.block());
}


void SpellChecker::start()
{
  m_textEdit = SendWidget::i()->input();
  connect(m_textEdit, SIGNAL(contextMenu(QMenu*,QPoint)), this, SLOT(contextMenu(QMenu*,QPoint)));

  SpellBackend::instance()->setLangs(ChatCore::settings()->value(LS("SpellChecker/EnabledDicts")).toStringList());
  m_highlighter = new SpellHighlighter(m_textEdit->document());
}


bool SpellChecker::suggestionsMenu(const QString &word, QMenu *parent)
{
  QStringList suggestions = SpellBackend::instance()->suggestions(word);
  if (suggestions.isEmpty())
    return false;

  QMenu *menu = parent->addMenu(tr("Suggestions"));
  foreach(const QString &word, suggestions) {
    menu->addAction(word, this, SLOT(repairWord()));
  }

  return true;
}

