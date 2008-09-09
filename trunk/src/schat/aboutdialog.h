/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.com)
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ABOUTDIALOG_H_
#define ABOUTDIALOG_H_

#include <QDialog>

class QLabel;
class QTabWidget;

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

private:
  QPushButton *m_closeButton;
  QTabWidget *m_tabWidget;
};


/*!
 * \brief Диалог "О Программе", Главная страница
 */
class MainTab : public QWidget
{
  Q_OBJECT

public:
  MainTab(QWidget *parent = 0); 
};


/*!
 * \brief Диалог "О Программе", Страница участников.
 */
class MembersTab : public QWidget
{
  Q_OBJECT

public:
  MembersTab(QWidget *parent = 0); 
};


/*!
 * \brief Диалог "О Программе", Страница истории версий.
 */
class ChangeLogTab : public QWidget
{
  Q_OBJECT

public:
  ChangeLogTab(QWidget *parent = 0); 
};


/*!
 * \brief Диалог "О Программе", Лицензия.
 */
class LicenseTab : public QWidget
{
  Q_OBJECT

public:
  LicenseTab(QWidget *parent = 0);
};

#endif /*ABOUTDIALOG_H_*/
