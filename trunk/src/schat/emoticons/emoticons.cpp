/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
 *
 * Class Emoticons
 * Copyright © 2008 by Carlo Segato <brandon.ml@gmail.com>
 * Copyright © 2008 Montel Laurent <montel@kde.org>
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

#include <QApplication>
#include <QDir>
#include <QFile>

#include "emoticons.h"
#include "emoticonsprovider.h"

class EmoticonsPrivate
{
public:
  EmoticonsPrivate(Emoticons *parent);
  ~EmoticonsPrivate();
  void loadServiceList();
//  EmoticonsProvider *loadProvider(const KService::Ptr &service);

//  QList<KService::Ptr> m_loaded;
  QHash<QString, EmoticonsTheme> m_themes;
//  KDirWatch *m_dirwatch;
  Emoticons *q;

private slots:
  void themeChanged(const QString &path);
};

EmoticonsPrivate::EmoticonsPrivate(Emoticons *parent)
  : q(parent)
{
}

EmoticonsPrivate::~EmoticonsPrivate()
{
//    delete m_dirwatch;
}

//bool priorityLessThan(const KService::Ptr &s1, const KService::Ptr &s2)
//{
//    return (s1->property("X-KDE-Priority").toInt() > s2->property("X-KDE-Priority").toInt());
//}

void EmoticonsPrivate::loadServiceList()
{
//    QString constraint("(exist Library)");
//    m_loaded = KServiceTypeTrader::self()->query("Emoticons", constraint);
//    qSort(m_loaded.begin(), m_loaded.end(), priorityLessThan);
}

//EmoticonsProvider *EmoticonsPrivate::loadProvider(const KService::Ptr &service)
//{
//    KPluginFactory *factory = KPluginLoader(service->library()).factory();
//    if (!factory) {
//        kWarning() << "Invalid plugin factory for" << service->library();
//        return 0;
//    }
//    EmoticonsProvider *provider = factory->create<EmoticonsProvider>(0);
//    return provider;
//}

void EmoticonsPrivate::themeChanged(const QString &path)
{
  QFileInfo info(path);
  QString name = info.dir().dirName();

  if (m_themes.contains(name)) {
    q->theme(name);
  }
}

/**
 * Default constructor
 */
Emoticons::Emoticons(QObject *parent)
  : QObject(parent), d(new EmoticonsPrivate(this))
{
//    d->loadServiceList();
//    d->m_dirwatch = new KDirWatch;
//    connect(d->m_dirwatch, SIGNAL(dirty(const QString&)), this, SLOT(themeChanged(const QString&)));
}


/**
 * Destruct the object
 */
Emoticons::~Emoticons()
{
  delete d;
}


/*!
 * Retrieve the current emoticons theme
 *
 * \return the current EmoticonsTheme
 */
EmoticonsTheme Emoticons::theme()
{
  return theme(currentThemeName());
}


/*!
 * Retrieve the theme with name \p name
 *
 * \param name name of the theme
 * \return the EmoticonsTheme \p name
 */
EmoticonsTheme Emoticons::theme(const QString &name)
{
//    if (d->m_themes.contains(name)) {
//        return d->m_themes.value(name);
//    }
//
//    for (int i = 0; i < d->m_loaded.size(); ++i) {
//        QString fName = d->m_loaded.at(i)->property("X-KDE-EmoticonsFileName").toString();
//        QString path = KGlobal::dirs()->findResource("emoticons", name + '/' + fName);
//
//        if (QFile::exists(path)) {
//            EmoticonsProvider *provider = d->loadProvider(d->m_loaded.at(i));
//            EmoticonsTheme theme(provider);
//            theme.loadTheme(path);
//            d->m_themes.insert(name, theme);
//
//            if (!d->m_dirwatch->contains(path)) {
//                d->m_dirwatch->addFile(path);
//            }
//            return theme;
//        }
//    }
  return EmoticonsTheme();
}


/**
 * Returns the current parse mode
 */
EmoticonsTheme::ParseMode Emoticons::parseMode()
{
//    KConfigGroup config(KSharedConfig::openConfig("kdeglobals"), "Emoticons");
//    return (EmoticonsTheme::ParseMode) config.readEntry("parseMode", int(EmoticonsTheme::RelaxedParse));
  return EmoticonsTheme::StrictParse;
}


/*!
 * Retrieve the current emoticon theme name
 */
QString Emoticons::currentThemeName()
{
//    KConfigGroup config(KSharedConfig::openConfig("kdeglobals"), "Emoticons");
//    QString name = config.readEntry("emoticonsTheme", "kde4");
  return "";
}


/*!
 * Returns a list of installed theme.
 *
 * \todo Невалидные темы не должны добавляться в список!
 */
QStringList Emoticons::themeList()
{
  QStringList ls;
  QStringList themeDirs; /// \todo Добавить другие пути к темам смайликов.
  themeDirs << (QApplication::applicationDirPath() + "/emoticons/");

  for (int i = 0; i < themeDirs.count(); ++i) {
    QDir themeQDir(themeDirs[i]);
    themeQDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    themeQDir.setSorting(QDir::Name);
    ls << themeQDir.entryList();
  }

  return ls;
}

/*!
 * Set \p theme as the current theme
 *
 * \param theme a pointer to a EmoticonsTheme object
 */
void Emoticons::setTheme(const EmoticonsTheme &theme)
{
  setTheme(theme.themeName());
}


/*!
 * Set \p theme as the current theme
 *
 * \param theme the name of a theme
 */
void Emoticons::setTheme(const QString &theme)
{
//    KConfigGroup config(KSharedConfig::openConfig("kdeglobals"), "Emoticons");
//    config.writeEntry("emoticonsTheme", theme);
//    config.sync();
}


/*!
 * Set the parse mode to \p mode
 */
void Emoticons::setParseMode(EmoticonsTheme::ParseMode mode)
{
//    KConfigGroup config(KSharedConfig::openConfig("kdeglobals"), "Emoticons");
//    config.writeEntry("parseMode", int(mode));
//    config.sync();
}
