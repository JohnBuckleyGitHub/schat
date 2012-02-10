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

#include <QContextMenuEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>

#include "statusmenu.h"

/*!
 * \brief Конструктор класса StatusMenu.
 */
StatusMenu::StatusMenu(bool male, QWidget *parent)
  : QMenu(parent),
    m_male(male),
    m_group(new QActionGroup(this))
{
  QAction *action = m_group->addAction(icon(StatusOnline), "");
  m_statuses.append(action);

  action = m_group->addAction(icon(StatusAway), "");
  m_statuses.append(action);

  action = m_group->addAction(icon(StatusDnD), "");
  m_statuses.append(action);

  action = m_group->addAction(icon(StatusOffline), "");
  m_statuses.append(action);

  for (int i = 0; i < m_statuses.size(); ++i) {
    m_statuses.at(i)->setData(i);
    m_statuses.at(i)->setCheckable(true);
  }

  action->setChecked(true);

  for (int i = 0; i < 3; ++i) {
    addAction(m_statuses.at(i));
  }

  addSeparator();
  addAction(m_statuses.at(StatusOffline));

  connect(m_group, SIGNAL(triggered(QAction *)), SLOT(statusChanged(QAction *)));
  retranslateUi();
}


/*!
 * Возвращает иконку ассоциированную с выбранным статусом.
 */
QIcon StatusMenu::icon(Status status) const
{
  QString file = ":/images/" + QString(m_male ? "male" : "female") + ".png";

  if (status == StatusOffline)
    return QIcon(QIcon(file).pixmap(16, 16, QIcon::Disabled));

  if (status == StatusOnline)
    return QIcon(file);

  QPixmap pixmap(file);
  QPainter painter(&pixmap);
  if (status == StatusAway)
    painter.drawPixmap(6, 6, QPixmap(":/images/status/small/away.png"));
  else if (status == StatusDnD)
    painter.drawPixmap(6, 6, QPixmap(":/images/status/small/dnd.png"));

  painter.end();
  return QIcon(pixmap);
}


QString StatusMenu::maxSizeText() const
{
  QString out;
  for (int i = 0; i < m_statuses.size(); ++i) {
    if (m_statuses.at(i)->text().size() > out.size())
      out = m_statuses.at(i)->text();
  }

  return out;
}


QString StatusMenu::text() const
{
  return m_group->checkedAction()->text();
}


void StatusMenu::setGender(bool male)
{
  m_male = male;

  for (int i = 0; i < m_statuses.size(); ++i) {
    m_statuses.at(i)->setIcon(icon(static_cast<StatusMenu::Status>(i)));
  }
}


/*!
 * Установка статуса.
 */
void StatusMenu::setStatus(Status status)
{
  m_status = status;
  m_statuses.at(status)->setChecked(true);
}



void StatusMenu::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QMenu::changeEvent(event);
}


/*!
 * Обработка выбора пользователем в меню нового статуса.
 */
void StatusMenu::statusChanged(QAction *action)
{
  emit statusChanged(action->data().toInt());
}


void StatusMenu::retranslateUi()
{
  setTitle(tr("Status"));
  m_statuses.at(StatusOnline)->setText(tr("Online"));
  m_statuses.at(StatusAway)->setText(tr("Away"));
  m_statuses.at(StatusDnD)->setText(tr("DND"));
  m_statuses.at(StatusOffline)->setText(tr("Offline"));
}


/*!
 * \brief Конструктор класса StatusWidget.
 */
StatusWidget::StatusWidget(StatusMenu *menu, QWidget *parent)
  : TranslateWidget(parent),
    m_actualSize(false),
    m_menu(menu)
{
  m_icon = new QLabel(this);
  setIcon(StatusMenu::StatusOffline);

  m_label = new QLabel(this);
  QHBoxLayout *lay = new QHBoxLayout(this);
  lay->setMargin(0);
  lay->setSpacing(3);
  lay->addWidget(m_icon);
  lay->addWidget(m_label);
  lay->addStretch();
}


void StatusWidget::setGender(bool male)
{
  m_menu->setGender(male);
  setIcon(m_menu->status());
}


void StatusWidget::setStatus(StatusMenu::Status status)
{
  m_menu->setStatus(status);
  setIcon(status);
  if (m_actualSize)
    m_label->setText(m_menu->text());
}


/*!
 * Обработка нажатий кнопок мыши для показа меню.
 */
void StatusWidget::mouseReleaseEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton) {
    m_menu->exec(event->globalPos());
  }
  else {
    QWidget::mouseReleaseEvent(event);
  }
}


void StatusWidget::showEvent(QShowEvent *event)
{
  if (!m_actualSize) {
    setMinimumSize(size());
    m_label->setText(m_menu->text());
    m_actualSize = true;
  }
  QWidget::showEvent(event);
}


/*!
 * При изменении языка скрываем и затем снова показываем виджет
 * для того что перерассчитать актуальный размер.
 */
void StatusWidget::retranslateUi()
{
  m_actualSize = false;
  setMinimumSize(0, 0);
  m_label->setText(m_menu->maxSizeText());
  setVisible(false);
  setVisible(true);
}


void StatusWidget::setIcon(StatusMenu::Status status)
{
  m_icon->setPixmap(m_menu->icon(status).pixmap(16));
}
