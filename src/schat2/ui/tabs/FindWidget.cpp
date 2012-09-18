/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
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

#include <QHBoxLayout>
#include <QLabel>
#include <QToolBar>

#include "arora/lineedit.h"
#include "ui/tabs/FindWidget.h"
#include "sglobal.h"

FindWidget::FindWidget(QWidget *parent)
  : QFrame(parent)
{
  m_toolBar = new QToolBar(this);

  QLabel *label = new QLabel(this);
  label->setPixmap(QPixmap(LS(":/images/search.png")));

  m_editFind = new LineEdit(this);
  m_editFind->addWidget(label, LineEdit::LeftSide);
  m_editFind->setWidgetSpacing(3);

  m_toolBar->addWidget(m_editFind);

  QHBoxLayout *mainLay = new QHBoxLayout(this);
  mainLay->addWidget(m_toolBar);
  mainLay->setMargin(4);

# if defined(Q_OS_WIN32)
  setObjectName(LS("FindWidget"));
  setStyleSheet(QString("FindWidget { background-color:%1; }").arg(palette().color(QPalette::Window).name()));
# endif
}
