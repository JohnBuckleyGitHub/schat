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

#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include <QMenu>
#include <QTextEdit>
#include <QDebug>

#include "SpellHighlighter.h"
#include "SpellBackend.h"
#include "SpellCheckerPlugin.h"

#include "plugins/ChatPlugin.h"

class QTextDocument;

class SpellChecker : public ChatPlugin
{
  Q_OBJECT

public:
  SpellChecker(QObject *parent = 0);
  inline static SpellChecker* instance() { return m_self; }

protected:
  void appendHL(QTextDocument *ADocument);

  
public slots:
  void showContextMenu(const QPoint &pt);
  void repairWord();
  void setEnabledDicts(QList<QString> &dicts);
  void addWordToDict();

private slots:
  void start();

private:
  static SpellChecker *m_self;

  QTextEdit *m_textEdit;
  int FCurrentCursorPosition;
  SpellHighlighter *FSH;
  QMenu *suggestMenu(const QString &word);
  
};

#endif // SPELLCHECKER_H