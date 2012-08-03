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

#include <QTime>
#include <QDebug>

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
  qDeleteAll(FHunSpellMap);
  FHunSpellMap.clear();
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
  QTime t;
  t.start();
  QMap<QString, QByteArray>::const_iterator it = FDictionaryMap.begin();
  while (it != FDictionaryMap.end()) {
    QTextCodec *codec = QTextCodec::codecForName(it.value());
    QByteArray encodedString;
    encodedString = codec->fromUnicode(word);
    sum += FHunSpellMap[it.key()]->spell(encodedString.data());
    ++it;
  }

  qDebug() << "HunspellChecker::isCorrect" << t.elapsed() << "ms";
  return sum > 0 ? true : false;
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
  QTime t;
  t.start();
  QList<QString> words;
  QMap<QString, QByteArray>::const_iterator it = FDictionaryMap.begin();
  while (it != FDictionaryMap.end()) {
    char **sugglist = NULL;
    QTextCodec *codec = QTextCodec::codecForName(it.value());
    QByteArray encodedString;
    encodedString = codec->fromUnicode(word);
    int count =
        FHunSpellMap[it.key()]->suggest(&sugglist, encodedString.data());
    for (int i = 0; i < count; ++i)
      words << codec->toUnicode(sugglist[i]);
    FHunSpellMap[it.key()]->free_list(&sugglist, count);
    ++it;
  }

  qDebug() << "HunspellChecker::suggestions" << t.elapsed() << "ms";
  return words;
}


void HunspellChecker::setLangs(const QStringList &dicts)
{
  QTime t;
  t.start();
  qDeleteAll(FHunSpellMap);
  FHunSpellMap.clear();
  loadHunspell(dicts);

  qDebug() << "HunspellChecker::setLangs" << t.elapsed() << "ms";
}


void HunspellChecker::loadHunspell(const QStringList &dicts)
{
  QList<QString>::const_iterator i;
  for (i = dicts.begin(); i != dicts.end(); ++i) {
    QString dic = QString("%1/%2.dic").arg(dictPath).arg(*i);
    if (QFileInfo(dic).exists()) {
      FHunSpellMap.insert(*i, new Hunspell(QString("%1/%2.aff").arg(dictPath).arg(*i).toUtf8().constData(), dic.toUtf8().constData()));
      FDictionaryMap.insert(*i, FHunSpellMap[*i]->get_dic_encoding());
    }
  }
}
