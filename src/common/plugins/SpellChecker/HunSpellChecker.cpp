/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
 * Copyright © 2012 Alexey Ivanov <alexey.ivanes@gmail.com>
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

#include <QDir>
#include <QLocale>
#include <QCoreApplication>
#include <QTextCodec>

#include "HunSpellChecker.h"
#include "hunspell/hunspell.hxx"

HunspellChecker::HunspellChecker(QObject *parent)
  : SpellBackend(parent)
{
#ifdef Q_WS_WIN
  dictPath = QString("%1/hunspell/dict").arg(QCoreApplication::applicationDirPath()).toUtf8().constData();
#else
  dictPath = "/usr/share/hunspell/";
#endif
}


HunspellChecker::~HunspellChecker()
{
  qDeleteAll(m_list);
  m_list.clear();
}


bool HunspellChecker::add(const QString &word)
{
  Q_UNUSED(word)
  bool result = false;
  //if (FHunSpell)
  {
    //FHunSpell->add(AWord.toUtf8().constData());
    result = true;
  }
  return result;
}


bool HunspellChecker::available() const
{
  return true;
}


bool HunspellChecker::isCorrect(const QString &word) const
{
  int sum = 0;

  foreach (Hunspell *dic, m_list) {
    QByteArray encoded = QTextCodec::codecForName(dic->get_dic_encoding())->fromUnicode(word);
    sum += dic->spell(encoded.constData());
  }

  return sum > 0;
}


QStringList HunspellChecker::dictionaries() const
{
  QStringList dict;
  QDir dir(dictPath);
  if (dir.exists()) {
    QStringList lstDic = dir.entryList(QStringList("*.dic"), QDir::Files);
    foreach(QString tmp, lstDic) {
      if (tmp.startsWith("hyph_"))
        continue;
      if (tmp.startsWith("th_"))
        continue;
      if (tmp.endsWith(".dic"))
        tmp = tmp.mid(0, tmp.length() - 4);
      dict << tmp;
    }
  }
  return dict;
}


QStringList HunspellChecker::suggestions(const QString &word) const
{
  QStringList words;

  foreach (Hunspell *dic, m_list) {
    char **sugglist    = 0;
    QTextCodec *codec  = QTextCodec::codecForName(dic->get_dic_encoding());
    QByteArray encoded = codec->fromUnicode(word);

    int count = dic->suggest(&sugglist, encoded.constData());
    for (int i = 0; i < count; ++i)
      words << codec->toUnicode(sugglist[i]);

    dic->free_list(&sugglist, count);
  }

  return words;
}


void HunspellChecker::setLangs(const QStringList &dicts)
{
  qDeleteAll(m_list);
  m_list.clear();
  loadHunspell(dicts);
}


void HunspellChecker::loadHunspell(const QStringList &dicts)
{
  foreach (const QString &name, dicts) {
    Hunspell *dic = new Hunspell(QString("%1/%2.aff").arg(dictPath).arg(name).toUtf8().constData(), QString("%1/%2.dic").arg(dictPath).arg(name).toUtf8().constData());
    if (QTextCodec::codecForName(dic->get_dic_encoding()))
      m_list.append(dic);
    else
      delete dic;
  }
}
