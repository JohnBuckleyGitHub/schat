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

#ifndef STATUSMENU_H_
#define STATUSMENU_H_

#include <QMenu>

#include "translatewidget.h"

class QLabel;


/*!
 * \brief Обеспечивает функциональность меню выбора смайликов.
 */
class StatusMenu : public QMenu
{
  Q_OBJECT

public:
  enum Status {
    StatusOnline,
    StatusAway,
    StatusDnD,
    StatusOffline
  };

  StatusMenu(bool male, QWidget *parent = 0);
  inline Status status() const { return m_status; }
  QIcon icon(Status status) const;
  QString maxSizeText() const;
  QString text() const;
  void setGender(bool male);
  void setStatus(Status status);

signals:
  void statusChanged(int status);

protected:
  void changeEvent(QEvent *event);

private slots:
  void statusChanged(QAction *action);

private:
  void retranslateUi();

  bool m_male;
  QActionGroup *m_group;       ///< Группа для того чтобы можно было выбрать только один статус.
  QList<QAction *> m_statuses; ///< Список статусов.
  Status m_status;
};


/*!
 * \brief Виджет выбора статусов из строки состояния.
 */
class StatusWidget : public TranslateWidget
{
  Q_OBJECT

public:
  StatusWidget(StatusMenu *menu, QWidget *parent = 0);
  void setGender(bool male);
  void setStatus(StatusMenu::Status status);

protected:
  void mouseReleaseEvent(QMouseEvent *event);
  void showEvent(QShowEvent *event);

private:
  void retranslateUi();
  void setIcon(StatusMenu::Status status);

  bool m_actualSize;
  QLabel *m_icon;     ///< Видежт для отображения иконки статуса.
  QLabel *m_label;    ///< Виджет для показа текста статуса.
  StatusMenu *m_menu; ///< Меню для выбора статуса.
};

#endif /* STATUSMENU_H_ */
