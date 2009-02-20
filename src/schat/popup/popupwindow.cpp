/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
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

#include "popupwindow.h"
#include "qeffects_p.h"

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
    emit closeWindow();
  else if (event->button() == Qt::LeftButton)
    emit openChat();

  QTextBrowser::mousePressEvent(event);
}


/*!
 * \brief Приватный D-класс для класса PopupWindow.
 */
class PopupWindow::Private
{
public:
  Private();

  bool normal;
  int slot;
  QLabel *nick;
  QLabel *time;
  QString flashStyle;
  QString normalStyle;
  QTextBrowser *text;
};


/*!
 * Конструктор класса PopupWindow::Private.
 */
PopupWindow::Private::Private()
  : normal(true), slot(0)
{
  QFile file;
  file.setFileName(":/css/popupwindow-normal.css");
  if (file.open(QFile::ReadOnly)) {
    normalStyle = QLatin1String(file.readAll());
    file.close();
  }

  file.setFileName(":/css/popupwindow-flash.css");
  if (file.open(QFile::ReadOnly)) {
    flashStyle = QLatin1String(file.readAll());
    file.close();
  }
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
  d->nick->setObjectName("NickLabel");
  d->time = new QLabel(message.time, this);
  d->text = new PopupTextBrowser(this);
  d->text->setHtml(message.html);

  QGridLayout *mainLay = new QGridLayout(this);
  mainLay->addWidget(d->nick, 0, 0);
  mainLay->addWidget(d->time, 0, 1);
  mainLay->addWidget(d->text, 1, 0, 1, 2);
  mainLay->setMargin(0);
  mainLay->setSpacing(0);
  mainLay->setColumnStretch(0, 1);

  setStyleSheet(d->normalStyle);
  setFixedSize(QSize(Width, Height));

  QTimer *timer = new QTimer(this);
  timer->setInterval(1500);
  connect(timer, SIGNAL(timeout()), SLOT(flash()));
  QTimer::singleShot(0, timer, SLOT(start()));

  connect(d->text, SIGNAL(closeWindow()), SLOT(close()));
  connect(d->text, SIGNAL(openChat()), SLOT(openChat()));

//  QTimer::singleShot((20 + 1) * 1000, this, SLOT(close()));
  setWindowOpacity(0.9);
  #if defined(Q_OS_MAC)
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
  #else
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::ToolTip);
  #endif
}


PopupWindow::~PopupWindow()
{
  delete d;
}


/*!
 * Показ окна
 */
void PopupWindow::start(int slot)
{
  d->slot = slot;

  QRect geometry = QDesktopWidget().availableGeometry();
//  qDebug() << geometry.right() << geometry.bottom();

  move(geometry.right() - Width, geometry.bottom() - (slot == 1 ? Height : (Height + Space) * slot));

  show();
//  qScrollEffect(this, QEffects::LeftScroll, 200);
}


/*!
 * Закрытие окна.
 */
void PopupWindow::close()
{
  emit aboutToClose(d->nick->text());
  QWidget::close();
}


/*!
 * Обработка событий от мыши.
 */
void PopupWindow::mouseReleaseEvent(QMouseEvent *event)
{
  if (event->button() == Qt::RightButton)
    openChat();
  else if (event->button() == Qt::LeftButton)
    close();
}


/*!
 * Изменение стиля по таймеру для создания эффекта мигания окна.
 */
void PopupWindow::flash()
{
  if (d->normal)
    setStyleSheet(d->flashStyle);
  else
    setStyleSheet(d->normalStyle);

  d->normal = !d->normal;
}


void PopupWindow::openChat()
{
  emit openChat(d->nick->text());
  close();
}
