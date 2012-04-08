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

#include <QComboBox>
#include <QVBoxLayout>

#include "CountryField.h"
#include "sglobal.h"
#include "Tr.h"

CountryField::CountryField(QWidget *parent)
  : ProfileField(LS("country"), parent)
{
  m_box = new QComboBox(this);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->setMargin(0);
  mainLay->setSpacing(0);
  mainLay->addWidget(m_box);

  load();

  connect(m_box, SIGNAL(currentIndexChanged(int)), SLOT(indexChanged(int)));
}


void CountryField::setData(const QVariant &value)
{
  if (value.type() != QVariant::String)
    return;

  QString code = value.toString();
  if (code.size() != 2)
    return;

  int index = m_box->findData(code);
  if (index == -1)
    index = 0;

  m_box->setCurrentIndex(index);
}


void CountryField::indexChanged(int index)
{
  if (index == 0)
    apply(QString());
  else
    apply(m_box->itemData(index));
}


QIcon CountryField::icon(const QString &code, const QPixmap &layout)
{
  QPoint point = pos(code);
  if (point.isNull())
    return QIcon();

  return QIcon(layout.copy(point.x(), point.y(), 16, 11));
}


QPoint CountryField::pos(const QString &code)
{
  QPoint point;
  if (code.size() != 2)
    return point;

  char x = code.at(1).toLatin1();
  if (x < 'a' || x > 'z')
    return point;

  char y = code.at(0).toLatin1();
  if (y < 'a' || y > 'z')
    return point;

  point.setX(16 * (x - 'a') + 16);
  point.setY(11 * (y - 'a') + 11);
  return point;
}


void CountryField::load()
{
  QStringList countries; // ISO 3166-1 alpha-2
  countries
    << LS("ru")
    << LS("th")
    << LS("us");

  QPixmap layout(LS(":/images/flags.png"));

  foreach (QString code, countries) {
    m_box->addItem(icon(code, layout), Tr::value(LS("country-") + code), code);
  }

  m_box->insertItem(0, icon(LS("zz"), layout), tr("Not selected"));
  m_box->setCurrentIndex(0);
}
