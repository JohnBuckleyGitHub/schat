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

#include <QCoreApplication>

#include "ChatCore.h"
#include "ChatNotify.h"
#include "ChatUrls.h"
#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "client/ClientFeeds.h"
#include "JSON.h"
#include "net/SimpleID.h"
#include "Profile.h"
#include "sglobal.h"
#include "Tr.h"
#include "WebBridge.h"

WebBridge *WebBridge::m_self = 0;

class WebBridgeTr : public Tr
{
  Q_DECLARE_TR_FUNCTIONS(WebBridgeTr)

public:
  WebBridgeTr() : Tr() {}

protected:
  QString valueImpl(const QString &key) const
  {
    if (key == LS("my_computers"))           return tr("My Computers");
    else if (key == LS("my_computers_desc")) return tr("These are the computers currently linked to your account.");
    else if (key == LS("computer_name"))     return tr("Computer name");
    else if (key == LS("last_activity"))     return tr("Last Activity");
    else if (key == LS("actions"))           return tr("Actions");
    else if (key == LS("unlink"))            return tr("Unlink");
    else if (key == LS("version"))           return tr("<b>Version:</b>");
    else if (key == LS("last_ip"))           return tr("<b>Last IP Address:</b>");
    else if (key == LS("profile"))           return tr("Profile");
    else if (key == LS("connections"))       return tr("Connections");
    else if (key == LS("user_offline"))      return tr("User offline");
    else if (key == LS("chat_version"))      return tr("Version");
    else if (key == LS("os_name"))           return tr("OS");
    else if (key == LS("kB"))                return tr("kB");
    else if (key == LS("MB"))                return tr("MB");
    else if (key == LS("enabled"))           return tr("Enabled");
    else if (key == LS("enable"))            return tr("Enable");
    else if (key == LS("restart_now"))       return tr("Restart now");
    else if (key == LS("will_be_enabled"))   return tr("✔ Plugin will be enabled after you restart Simple Chat.");
    else if (key == LS("will_be_disabled"))  return tr("Plugin will be disabled after you restart Simple Chat.");
    else if (key == LS("sign_in_with"))      return tr("Sign in using your account with");
    else if (key == LS("or_anon_connect"))   return tr("Or connect anonymously");
    else if (key == LS("anonymous_user"))    return tr("anonymous user");
    else if (key == LS("status_Online"))     return tr("Online");
    else if (key == LS("status_Away"))       return tr("Away");
    else if (key == LS("status_DnD"))        return tr("Do not disturb");
    else if (key == LS("status_FreeForChat"))return tr("Free for Chat");
    else if (key == LS("status_Offline"))    return tr("Offline");
    return QString();
  }
};


WebBridge::WebBridge(QObject *parent)
  : QObject(parent)
{
  m_self = this;
  m_tr = new WebBridgeTr();
  retranslate();

  connect(ChatClient::channels(), SIGNAL(channel(ChannelInfo)), SLOT(channel(ChannelInfo)));
  connect(ChatClient::channels(), SIGNAL(quit(QByteArray)), SLOT(quit(QByteArray)));
  connect(ChatClient::i(), SIGNAL(online()), SIGNAL(online()));
  connect(ChatClient::i(), SIGNAL(offline()), SIGNAL(offline()));
}


WebBridge::~WebBridge()
{
  delete m_tr;
}


QString WebBridge::bytesToHuman(qint64 size, bool html)
{
  QString num;
  QString key;
  if (size < 1000000) {
    if (!size)
      num = LS("0");
    else if (size && size < 1024)
      num = LS("1");
    else
      num = QString::number((int) size / 1024);

    key = LS("kB");
  }
  else {
    num = QString::number((double) size / 1048576, 'f', 2);
    key = LS("MB");
  }

  if (html)
    num += LS(" <span data-tr=\"") + key + LS("\">") + Tr::value(key) + LS("</span>");
  else
    num += LC(' ') + Tr::value(key);

  return num;
}


QString WebBridge::channel(const QString &id) const
{
  return JSON::generate(channel(SimpleID::decode(id.toLatin1())));
}


QString WebBridge::randomId() const
{
  return SimpleID::encode(ChatCore::randomId());
}


QString WebBridge::status(const QString &id) const
{
  if (ChatClient::state() != ChatClient::Online)
    return LS("Offline");

  ClientChannel channel = ChatClient::channels()->get(SimpleID::decode(id));
  if (!channel)
    return LS("Offline");

  return channel->status().toString();
}


QString WebBridge::translate(const QString &key) const
{
  return Tr::value(key);
}


QStringList WebBridge::fields() const
{
  return Profile::fields();
}


/*!
 * Получение фида по идентификатору канала и имени.
 */
QVariantMap WebBridge::feed(const QString &id, const QString &name, bool cache) const
{
  ClientChannel channel = ChatClient::channels()->get(SimpleID::decode(id));
  if (!channel)
    return QVariantMap();

  return feed(channel, name, cache);
}


/*!
 * Получение собственного фида по имени.
 */
QVariantMap WebBridge::feed(const QString &name, bool cache) const
{
  return feed(ChatClient::channel(), name, cache);
}


void WebBridge::request(const QString &command, const QString &name, const QVariantMap &json)
{
  ChatClient::feeds()->request(ChatClient::id(), command, name, json);
}


void WebBridge::setTabPage(const QString &id, int page)
{
  ClientChannel channel = ChatClient::channels()->get(SimpleID::decode(id.toLatin1()));
  if (!channel)
    return;

  channel->data()[LS("page")] = page;
}


QVariantMap WebBridge::channel(const QByteArray &id)
{
  ClientChannel channel = ChatClient::channels()->get(id);
  if (!channel)
    return QVariantMap();

  QVariantMap data;
  data[LS("Id")]     = QString(SimpleID::encode(id));
  data[LS("Name")]   = channel->name();
  data[LS("Url")]    = ChatUrls::toUrl(channel, LS("insert")).toString();
  data[LS("Color")]  = channel->gender().toString();
  data[LS("Status")] = channel->status().toString();
  return data;
}


/*!
 * Базовая функция получения фида.
 *
 * \param channel Канал владелец фида.
 * \param name    Имя фида.
 * \param cache   \b false если необходимо игнорировать локальный кэш и получить фид с сервера.
 *
 * \return JSON тело фида или пустую таблицу, если фид не доступен локально.
 */
QVariantMap WebBridge::feed(ClientChannel channel, const QString &name, bool cache)
{
  FeedPtr feed = channel->feed(name, false);
  if (!cache || !feed)
    ChatClient::feeds()->request(channel->id(), LS("get"), name);

  if (!feed)
    return QVariantMap();

  return feed->data();
}


QVariantMap WebBridge::feed(const FeedNotify &notify)
{
  ClientChannel channel = ChatClient::channels()->get(notify.channel());
  if (!channel)
    return QVariantMap();

  int type = notify.type();
  QVariantMap out;

  if (type == Notify::FeedData) {
    FeedPtr feed = channel->feed(notify.name(), false);
    if (!feed)
      return out;

    out[LS("data")] = feed->data();
    out[LS("type")] = LS("body");
  }
  else
    out[LS("data")] = notify.json();

  if (type == Notify::FeedReply)
    out[LS("type")] = LS("reply");
  else if (type == Notify::QueryError)
    out[LS("type")] = LS("error");

  out[LS("status")] = notify.status();
  out[LS("name")]   = notify.name();
  out[LS("own")]    = notify.channel() == ChatClient::id();
  out[LS("id")]     = QString(SimpleID::encode(notify.channel()));
  return out;
}


void WebBridge::retranslate()
{
  emit retranslated();
}


void WebBridge::channel(const ChannelInfo &info)
{
  QVariantMap data = WebBridge::channel(info.id());
  if (info.option() == ChannelInfo::New || info.option() == ChannelInfo::Renamed)
    emit renamed(data);

  emit recolored(data);
  emit statusChanged(data.value(LS("Id")).toString(), data.value(LS("Status")).toString());
}


void WebBridge::quit(const QByteArray &user)
{
  emit statusChanged(SimpleID::encode(user), LS("Offline"));
}
