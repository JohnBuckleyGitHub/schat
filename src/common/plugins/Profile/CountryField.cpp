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


void CountryField::retranslateUi()
{
  ProfileField::retranslateUi();
  m_box->disconnect(this);

  QVariant data = m_box->itemData(m_box->currentIndex());
  m_box->removeItem(0);

  int size = m_box->count();
  for (int i = 0; i < size; ++i) {
    m_box->setItemText(i, Tr::value(LS("country-") + m_box->itemData(i).toString()));
  }

  m_box->model()->sort(0);
  QPixmap layout(LS(":/images/flags.png"));
  m_box->insertItem(0, icon(LS("zz"), layout), tr("Not selected"));
  m_box->setCurrentIndex(m_box->findData(data));

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
  QStringList countries; // ISO 3166-1 alpha-2, missed AQ, BL, BQ, CW, GG, IM, JR, MF, SS, SX
  countries
    << LS("ad")
    << LS("ae")
    << LS("af")
    << LS("ag")
    << LS("ai")
    << LS("al")
    << LS("am")
    << LS("ao")
//    << LS("aq")
    << LS("ar")
    << LS("as")
    << LS("at")
    << LS("aw")
    << LS("ax")
    << LS("az")

    << LS("ba")
    << LS("bb")
    << LS("bd")
    << LS("be")
    << LS("bf")
    << LS("bg")
    << LS("bh")
    << LS("bi")
    << LS("bj")
//    << LS("bl")
    << LS("bm")
    << LS("bn")
    << LS("bo")
//    << LS("bq")
    << LS("br")
    << LS("bs")
    << LS("bt")
    << LS("bv")
    << LS("bw")
    << LS("by")
    << LS("bz")

    << LS("ca")
    << LS("cd")
    << LS("cf")
    << LS("cg")
    << LS("ch")
    << LS("ci")
    << LS("ck")
    << LS("cl")
    << LS("cm")
    << LS("cn")
    << LS("co")
    << LS("cr")
    << LS("cu")
    << LS("cv")
//    << LS("cw")
    << LS("cx")
    << LS("cy")
    << LS("cz")

    << LS("de")
    << LS("dj")
    << LS("dk")
    << LS("dm")
    << LS("do")
    << LS("dz")

    << LS("ec")
    << LS("ee")
    << LS("eg")
    << LS("eh")
    << LS("er")
    << LS("es")
    << LS("et")

    << LS("fi")
    << LS("fj")
    << LS("fk")
    << LS("fm")
    << LS("fo")
    << LS("fr")

    << LS("ga")
    << LS("gb")
    << LS("gd")
    << LS("ge")
    << LS("gf")
//    << LS("gg")
    << LS("gh")
    << LS("gi")
    << LS("gl")
    << LS("gm")
    << LS("gn")
    << LS("gp")
    << LS("gq")
    << LS("gr")
    << LS("gs")
    << LS("gt")
    << LS("gu")
    << LS("gw")
    << LS("gy")

    << LS("hk")
    << LS("hm")
    << LS("hn")
    << LS("hr")
    << LS("ht")
    << LS("hu")

    << LS("id")
    << LS("ie")
    << LS("il")
//    << LS("im")
    << LS("in")
    << LS("io")
    << LS("iq")
    << LS("ir")
    << LS("is")
    << LS("it")

//    << LS("je")
    << LS("jm")
    << LS("jo")
    << LS("jp")

    << LS("ke")
    << LS("kg")
    << LS("kh")
    << LS("ki")
    << LS("km")
    << LS("kn")
    << LS("kp")
    << LS("kr")
    << LS("kw")
    << LS("ky")
    << LS("kz")

    << LS("la")
    << LS("lb")
    << LS("lc")
    << LS("li")
    << LS("lk")
    << LS("lr")
    << LS("ls")
    << LS("lt")
    << LS("lu")
    << LS("lv")
    << LS("ly")

    << LS("ma")
    << LS("mc")
    << LS("md")
    << LS("me")
//    << LS("mf")
    << LS("mg")
    << LS("mh")
    << LS("mk")
    << LS("ml")
    << LS("mm")
    << LS("mn")
    << LS("mo")
    << LS("mp")
    << LS("mq")
    << LS("mr")
    << LS("ms")
    << LS("mt")
    << LS("mu")
    << LS("mv")
    << LS("mw")
    << LS("mx")
    << LS("my")
    << LS("mz")

    << LS("na")
    << LS("nc")
    << LS("ne")
    << LS("nf")
    << LS("ng")
    << LS("ni")
    << LS("nl")
    << LS("no")
    << LS("np")
    << LS("nr")
    << LS("nu")
    << LS("nz")

    << LS("om")

    << LS("pa")
    << LS("pe")
    << LS("pf")
    << LS("pg")
    << LS("ph")
    << LS("pk")
    << LS("pl")
    << LS("pm")
    << LS("pn")
    << LS("pr")
    << LS("ps")
    << LS("pt")
    << LS("pw")
    << LS("py")

    << LS("qa")
    << LS("re")
    << LS("ro")
    << LS("rs")
    << LS("ru")
    << LS("rw")

    << LS("sa")
    << LS("sb")
    << LS("sc")
    << LS("sd")
    << LS("se")
    << LS("sg")
    << LS("sh")
    << LS("si")
    << LS("sj")
    << LS("sk")
    << LS("sl")
    << LS("sm")
    << LS("sn")
    << LS("so")
    << LS("sr")
//    << LS("ss")
    << LS("st")
    << LS("sv")
//    << LS("sx")
    << LS("sy")
    << LS("sz")

    << LS("tc")
    << LS("tf")
    << LS("th")
    << LS("tj")
    << LS("tk")
    << LS("tl")
    << LS("tm")
    << LS("tn")
    << LS("to")
    << LS("tr")
    << LS("tt")
    << LS("tv")
    << LS("tw")
    << LS("tz")

    << LS("ua")
    << LS("ug")
    << LS("um")
    << LS("us")
    << LS("uy")
    << LS("uz")

    << LS("va")
    << LS("vc")
    << LS("ve")
    << LS("vg")
    << LS("vi")
    << LS("vn")
    << LS("vu")

    << LS("wf")
    << LS("ws")

    << LS("ye")
    << LS("yt")

    << LS("za")
    << LS("zm")
    << LS("zw");

  QPixmap layout(LS(":/images/flags.png"));

  foreach (QString code, countries) {
    m_box->addItem(icon(code, layout), Tr::value(LS("country-") + code), code);
  }

  m_box->model()->sort(0);

  m_box->insertItem(0, icon(LS("zz"), layout), tr("Not selected"));
  m_box->setCurrentIndex(0);
}
