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

#include <QApplication>
#include <QtPlugin>

#include "ChatCore.h"
#include "Profile.h"
#include "ProfileChatView.h"
#include "ProfilePlugin.h"
#include "ProfilePlugin_p.h"
#include "sglobal.h"
#include "Translation.h"
#include "Tr.h"

class ProfilePluginTr : public Tr
{
  Q_DECLARE_TR_FUNCTIONS(ProfilePluginTr)

public:
  ProfilePluginTr() : Tr() { m_prefix = LS("field-"); }

protected:
  QString valueImpl(const QString &key) const
  {
    if (key == LS("city"))       return tr("City");
    else if (key == LS("site"))  return tr("Site");
    else if (key == LS("email")) return tr("E-Mail");
    return QString();
  }
};


ProfilePluginImpl::ProfilePluginImpl(QObject *parent)
  : ChatPlugin(parent)
{
  m_tr = new ProfilePluginTr();
  new ProfileChatView(this);

  Profile::addField(LS("city"), 2000);
  Profile::addField(LS("site"), 4000);
  Profile::addField(LS("email"), 4050);

  ChatCore::translation()->addOther(LS("profile"));
}


ProfilePluginImpl::~ProfilePluginImpl()
{
  delete m_tr;
}


ChatPlugin *ProfilePlugin::create()
{
  m_plugin = new ProfilePluginImpl(this);
  return m_plugin;
}

Q_EXPORT_PLUGIN2(Profile, ProfilePlugin);
