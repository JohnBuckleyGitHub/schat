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

#ifndef ABOUTDIALOG_H_
#define ABOUTDIALOG_H_

#include <QDialog>

class QLabel;
class QTabWidget;
class QVBoxLayout;

#ifndef SCHAT_NO_UPDATE_WIDGET
  class UpdateWidget;
#endif

/*!
 * \brief Диалог "О Программе"
 *
 * Базовый диалог для просмотра информации о программе, включает в себя классы MainTab, ChangeLogTab и LicenseTab.
 */
class AboutDialog : public QDialog
{
  Q_OBJECT

public:
  AboutDialog(QWidget *parent = 0);
  ~AboutDialog();

protected:
  #if defined(Q_WS_WIN)
  bool winEvent(MSG *message, long *result);
  #endif

private:
  void setStyleSheet();

  QPushButton *m_closeButton;
  QTabWidget *m_tabWidget;
  QVBoxLayout *m_mainLay;
  QWidget *m_bottom;
  #ifndef SCHAT_NO_UPDATE_WIDGET
    UpdateWidget *m_update;
  #endif
};


/*!
 * \brief Диалог "О Программе", Главная страница
 */
class AboutMain : public QWidget
{
  Q_OBJECT

public:
  AboutMain(QWidget *parent = 0);
};


/*!
 * \brief Диалог "О Программе", Страница участников.
 */
class AboutMembers : public QWidget
{
  Q_OBJECT

public:
  AboutMembers(QWidget *parent = 0);
};


/*!
 * \brief Диалог "О Программе", Страница истории версий.
 */
class AboutChangeLog : public QWidget
{
  Q_OBJECT

public:
  AboutChangeLog(QWidget *parent = 0);
};


/*!
 * \brief Диалог "О Программе", Лицензия.
 */
class AboutLicense : public QWidget
{
  Q_OBJECT

public:
  AboutLicense(QWidget *parent = 0);
};

#endif /*ABOUTDIALOG_H_*/
