/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2010 IMPOMEZIA <schat@impomezia.com>
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

#include <QtGui>

#include "abstractprofile.h"
#include "popupwindow.h"
#include "protocol.h"
#include "settings.h"

/*!
 * Конструктор класса PopupTextBrowser.
 */
PopupTextBrowser::PopupTextBrowser(QWidget *parent)
  : QTextBrowser(parent)
{
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setContextMenuPolicy(Qt::NoContextMenu);
}


void PopupTextBrowser::mouseReleaseEvent(QMouseEvent * event)
{
  if (event->button() == Qt::RightButton)
    emit openChat(false);
  else if (event->button() == Qt::LeftButton)
    emit openChat(true);

  QTextBrowser::mousePressEvent(event);
}


/*!
 * \brief Приватный D-класс для класса PopupWindow.
 */
class PopupWindow::Private
{
public:
  Private();

  bool pub;            ///< Сообщение является обращением по имени из основного канала.
  int slot;            ///< Занятый окном слот, используется для расчёта позиции на экране.
  QLabel *nick;        ///< Обеспечивает отображение и хранение ника отправителя.
  QLabel *time;        ///< Обеспечивает отображение времени сообщения.
  QTextBrowser *text;  ///< Обеспечивает отображение текста сообщения.
  QTimer *closeTimer;  ///< Таймер автоматического закрытия.
};


/*!
 * Конструктор класса PopupWindow::Private.
 */
PopupWindow::Private::Private()
  : slot(0), closeTimer(0)
{
}


/*!
 * Конструктор класса PopupWindow.
 */
PopupWindow::PopupWindow(const Message &message, QWidget *parent)
  : QFrame(parent), d(new Private)
{
  setObjectName("PopupWindow");
  setAttribute(Qt::WA_DeleteOnClose, true);

  d->nick = new QLabel(message.nick, this);
  d->nick->setTextFormat(Qt::PlainText);
  d->nick->setObjectName("NickLabel");

  d->time = new QLabel(this);
  d->time->setTextFormat(Qt::PlainText);

  d->text = new PopupTextBrowser(this);
  d->text->document()->setDefaultStyleSheet(SimpleSettings->richTextCSS());

  setMessage(message);

  QGridLayout *mainLay = new QGridLayout(this);
  mainLay->addWidget(d->nick, 0, 0);
  mainLay->addWidget(d->time, 0, 1);
  mainLay->addWidget(d->text, 1, 0, 1, 2);
  mainLay->setMargin(0);
  mainLay->setSpacing(0);
  mainLay->setColumnStretch(0, 1);

  setFixedSize(QSize(Width, Height));

  connect(d->text, SIGNAL(openChat(bool)), SLOT(openChat(bool)));

  setWindowOpacity(0.9);
  #if defined(Q_OS_MAC)
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
  #else
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::ToolTip);
  #endif

  if (!SimpleSettings->getBool("PopupAutoClose"))
    return;

  if (SimpleSettings->profile()->status() == schat::StatusAutoAway && SimpleSettings->getBool("NoPopupAutoCloseInAway"))
    return;

  d->closeTimer = new QTimer(this);
  int delay = SimpleSettings->getInt("PopupAutoCloseTime");
  d->closeTimer->setInterval((delay > 1 ? delay : 1) * 1000);
  d->closeTimer->start();
  connect(d->closeTimer, SIGNAL(timeout()), SLOT(close()));
}


PopupWindow::~PopupWindow()
{
  delete d;
}


void PopupWindow::setMessage(const Message &message)
{
  d->time->setText(message.time);

  QString html = message.html;
  html.remove(QRegExp("<span style='display:inline-block;width:1px;height:1px;overflow:hidden;'>[^<]*</span>"));
  d->text->setHtml(html);

  d->pub = message.pub;

  if (d->closeTimer)
    d->closeTimer->start();
}


/*!
 * Показ окна
 */
void PopupWindow::start(int slot)
{
  moveToSlot(slot);

  show();
}


/*!
 * Закрытие окна.
 */
void PopupWindow::close()
{
  emit aboutToClose(d->nick->text(), d->slot);
  QWidget::close();
}


void PopupWindow::flash(const QString &style)
{
  setStyleSheet(style);
}


/*!
 * Обработка освобождения слота.
 * Если освободившийся слот меньше, то смещается на один слот вниз.
 *
 * \param slot Освободившийся слот.
 */
void PopupWindow::freeSlot(int slot)
{
  if (slot < d->slot)
    moveToSlot(--d->slot);
}


/*!
 * Обработка событий от мыши.
 */
void PopupWindow::mouseReleaseEvent(QMouseEvent *event)
{
  if (event->button() == Qt::RightButton)
    openChat(false);
  else if (event->button() == Qt::LeftButton)
    openChat(true);
}


void PopupWindow::openChat(bool open)
{
  emit openChat(d->nick->text(), d->pub, open);
  close();
}


/*!
 * Перемещение окна в заданный слот.
 * Определяются координаты слота и окно перемещается туда.
 */
void PopupWindow::moveToSlot(int slot)
{
  d->slot = slot;
  QRect geometry = QDesktopWidget().availableGeometry();
  move(geometry.right() - Width, geometry.bottom() - (slot == 1 ? Height : (Height + Space) * slot));
}
