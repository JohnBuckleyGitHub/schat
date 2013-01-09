/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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

#include <QGridLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QTextBrowser>
#include <QTimer>

#include "alerts/AlertType.h"
#include "ChatAlerts.h"
#include "ChatNotify.h"
#include "DateTime.h"
#include "PopupWindow.h"
#include "sglobal.h"

PopupWindow::PopupWindow(const Alert &alert, int timeout, const QString &css)
# if defined(Q_OS_MAC)
  : QFrame(0, Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowStaysOnTopHint)
# else
  : QFrame(0, Qt::ToolTip | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint)
# endif
  , m_id(alert.id())
  , m_tab(alert.tab())
{
  setObjectName(LS("PopupWindow"));
  setAttribute(Qt::WA_DeleteOnClose, true);
  setAttribute(Qt::WA_ShowWithoutActivating, true);
  setWindowOpacity(0.9);

  m_icon = new QLabel(this);
  m_icon->setObjectName(LS("IconLabel"));
  AlertType *type = ChatAlerts::type(alert);
  if (type)
    m_icon->setPixmap(type->icon().pixmap(16, 16));

  m_title = new QLabel(this);
  m_title->setObjectName(LS("TitleLabel"));

  m_date = new QLabel(DateTime::toDateTime(alert.date()).toString(LS("hh:mm:ss")), this);
  m_date->setObjectName(LS("DateLabel"));

  m_text = new QTextBrowser(this);
  m_text->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_text->setContextMenuPolicy(Qt::NoContextMenu);
  m_text->setAttribute(Qt::WA_TransparentForMouseEvents);
  m_text->document()->setDefaultStyleSheet(css);

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

  connect(ChatAlerts::i(), SIGNAL(removed(QByteArray)), SLOT(removed(QByteArray)));

  if (timeout)
    QTimer::singleShot(timeout * 1000, this, SLOT(close()));
}


void PopupWindow::mouseReleaseEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton) {
    ChatNotify::start(Notify::OpenChannel, m_tab);
    ChatNotify::start(Notify::ShowChat);
    close();
  }
  else if (event->button() == Qt::RightButton) {
    ChatAlerts::remove(m_tab, m_id);
    close();
  }
}


void PopupWindow::removed(const QByteArray &alertId)
{
  if (m_id == alertId)
    close();
}
