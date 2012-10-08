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

#include <QLabel>
#include <QGridLayout>
#include <QTextBrowser>
#include <QMouseEvent>

#include "alerts/AlertType.h"
#include "ChatAlerts.h"
#include "PopupWindow.h"
#include "sglobal.h"
#include "DateTime.h"

PopupWindow::PopupWindow(const Alert &alert)
  : QFrame(0, Qt::ToolTip | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint)
{
  setAttribute(Qt::WA_DeleteOnClose, true);
  setWindowOpacity(0.9);

  m_icon = new QLabel(this);
  AlertType *type = ChatAlerts::type(alert);
  if (type)
    m_icon->setPixmap(type->icon().pixmap(16, 16));

  m_title = new QLabel(this);
  m_date = new QLabel(DateTime::toDateTime(alert.date()).toString(LS("hh:mm:ss")), this);

  m_text = new QTextBrowser(this);
  m_text->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_text->setContextMenuPolicy(Qt::NoContextMenu);
  m_text->setAttribute(Qt::WA_TransparentForMouseEvents);

  QVariantMap popup = alert.data().value(LS("popup")).toMap();
  m_text->setHtml(popup.value(LS("text")).toString());
  m_title->setText(popup.value(LS("title")).toString());

  QGridLayout *mainLay = new QGridLayout(this);
  mainLay->addWidget(m_icon, 0, 0);
  mainLay->addWidget(m_title, 0, 1);
  mainLay->addWidget(m_date, 0, 2);
  mainLay->addWidget(m_text, 1, 0, 1, 3);
  mainLay->setMargin(0);
  mainLay->setSpacing(0);
  mainLay->setColumnStretch(1, 1);

  setFixedSize(QSize(Width, Height));
}


void PopupWindow::mouseReleaseEvent(QMouseEvent *event)
{
  Q_UNUSED(event)
  close();
}
