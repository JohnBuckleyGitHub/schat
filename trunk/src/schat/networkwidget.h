/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA <schat@impomezia.com>
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

#ifndef NETWORKWIDGET_H_
#define NETWORKWIDGET_H_

#include <QString>
#include <QWidget>

class QComboBox;
class QLabel;
class QSpinBox;
class QStandardItemModel;
class Settings;

/*!
 * \brief Виджет обеспечивающий выбор сети или одиночного сервера.
 */
class NetworkWidget : public QWidget {
  Q_OBJECT

public:
  NetworkWidget(Settings *settings, QWidget *parent = 0);
  bool save();
  void reset();

signals:
  void validServer(bool valid);

private slots:
  void activated(int index);
  void currentIndexChanged(int index);
  void editTextChanged(const QString &text);
  void setCurrentIndex(int index);

private:
  void init();

  int m_initPort;
  QComboBox *m_select;
  QLabel *m_infoLabel;
  QLabel *m_portLabel;
  QSpinBox *m_port;
  QString m_initText;
  QString m_networksPath;
  Settings *m_settings;
};

#endif /*NETWORKWIDGET_H_*/
