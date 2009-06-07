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

#ifndef NETWORKWIDGET_H_
#define NETWORKWIDGET_H_

#include <QString>
#include <QWidget>

#include "networkreader.h"

class QComboBox;
class QLabel;
class QSpinBox;
class QStandardItemModel;
class Settings;

/*!
 * \brief Виджет обеспечивающий выбор сети или одиночного сервера.
 */
class NetworkWidget : public QWidget
{
  Q_OBJECT

public:
  /// Опции создания виджета.
  enum OptionsFlag {
    NoOptions    = 0x0, ///< Нет опций.
    NetworkLabel = 0x1, ///< Добавить надпись \b Сеть.
  };

  Q_DECLARE_FLAGS(Options, OptionsFlag)

  NetworkWidget(QWidget *parent = 0, Options options = NoOptions);
  void reset();

signals:
  void validServer(bool valid);

public slots:
  int save(bool notify = true);

private slots:
  void currentIndexChanged(int index);
  void editTextChanged(const QString &text);
  void setCurrentIndex(int index);

private:
  inline int addSingleServer(const ServerInfo &info, bool current = true) { return addSingleServer(info.address, info.port, current); }
  int addSingleServer(const QString &address, quint16 port, bool current = true);
  int findSingleServer(const QString &address, quint16 port) const;
  ServerInfo singleServer(const QString &url);
  void init();

  QComboBox *m_select;
  QString m_initText;
  Settings *m_settings;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(NetworkWidget::Options)

#endif /*NETWORKWIDGET_H_*/
