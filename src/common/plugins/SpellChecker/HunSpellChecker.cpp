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
#include <QTextCodec>
#include <QThreadPool>

#include "hunspell/hunspell.hxx"
#include "HunSpellChecker.h"
#include "sglobal.h"
#include "SpellChecker.h"

HunspellChecker::HunspellChecker(QObject *parent)
  : SpellBackend(parent)
{
  m_pool = new QThreadPool(this);
  m_pool->setMaxThreadCount(1);

  m_path = SpellChecker::path();
}


HunspellChecker::~HunspellChecker()
{
  clear();
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

  if (!m_mutex.tryLock())
    return false;

  foreach (Hunspell *dic, m_list) {
    QByteArray encoded = QTextCodec::codecForName(dic->get_dic_encoding())->fromUnicode(word);
    sum += dic->spell(encoded.constData());
  }
  m_mutex.unlock();

  return sum > 0;
}


QStringList HunspellChecker::dictionaries() const
{
  QStringList dict;
  QDir dir(m_path);
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

  if (!m_mutex.tryLock())
    return words;

  foreach (Hunspell *dic, m_list) {
    char **sugglist    = 0;
    QTextCodec *codec  = QTextCodec::codecForName(dic->get_dic_encoding());
    QByteArray encoded = codec->fromUnicode(word);

    int count = dic->suggest(&sugglist, encoded.constData());
    for (int i = 0; i < count; ++i)
      words << codec->toUnicode(sugglist[i]);

    dic->free_list(&sugglist, count);
  }

  m_mutex.unlock();
  return words;
}


void HunspellChecker::queuedSuggestions(const QString &word) const
{
  HunspellSuggestions *task = new HunspellSuggestions(this, word);
  connect(task, SIGNAL(ready(QString,QStringList)), SIGNAL(suggestionsReady(QString,QStringList)));
  m_pool->start(task);
}


void HunspellChecker::setLangs(const QStringList &dicts)
{
  QStringList files;
  foreach (const QString &name, dicts) {
    files.append(m_path + LC('/') + name);
  }

  m_pool->start(new HunspellLoader(this, files));
}


/*!
 * Очистка загруженных словарей.
 */
void HunspellChecker::clear()
{
  m_mutex.lock();
  qDeleteAll(m_list);
  m_list.clear();
  m_mutex.unlock();
}


/*!
 * Загрузка словарей.
 */
void HunspellChecker::load(const QStringList &dicts)
{
  if (dicts.isEmpty())
    return;

  m_mutex.lock();
  foreach (const QString &name, dicts) {
    if (QFile::exists(name + LS(".dic"))) {
      Hunspell *dic = new Hunspell(QString(name + LS(".aff")).toUtf8().constData(), QString(name + LS(".dic")).toUtf8().constData());
      if (QTextCodec::codecForName(dic->get_dic_encoding()))
        m_list.append(dic);
      else
        delete dic;
    }
  }
  m_mutex.unlock();
}


HunspellLoader::HunspellLoader(HunspellChecker *hunspell, const QStringList &dicts)
  : QRunnable()
  , m_hunspell(hunspell)
  , m_dicts(dicts)
{
}


void HunspellLoader::run()
{
  m_hunspell->clear();
  m_hunspell->load(m_dicts);
}


HunspellSuggestions::HunspellSuggestions(const HunspellChecker *hunspell, const QString &word)
  : QObject()
  , QRunnable()
  , m_hunspell(hunspell)
  , m_word(word)
{
}


void HunspellSuggestions::run()
{
  QStringList words = m_hunspell->suggestions(m_word);
  if (!words.isEmpty())
    emit ready(m_word, words);
}
