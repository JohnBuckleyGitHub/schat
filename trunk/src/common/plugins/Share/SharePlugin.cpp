/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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
#include <QFileInfo>
#include <QHttpMultiPart>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QtPlugin>

#include "alerts/AlertType.h"
#include "ChatAlerts.h"
#include "ChatCore.h"
#include "client/ChatClient.h"
#include "client/SimpleClient.h"
#include "DateTime.h"
#include "JSON.h"
#include "messages/ChannelMessage.h"
#include "sglobal.h"
#include "ShareChatView.h"
#include "ShareMessages.h"
#include "SharePlugin.h"
#include "SharePlugin_p.h"
#include "Tr.h"
#include "Translation.h"
#include "ui/TabWidget.h"

class SharePluginTr : public Tr
{
  Q_DECLARE_TR_FUNCTIONS(SharePluginTr)

public:
  SharePluginTr() : Tr() { m_prefix = LS("share-"); }

protected:
  QString valueImpl(const QString &key) const
  {
    if (key == LS("upload-images"))     return tr("Upload images");
    else if (key == LS("close"))        return tr("Close");
    else if (key == LS("upload-error")) return tr("An error occurred while uploading");
    else if (key == LS("please-wait"))  return tr("Please Wait...");

    return QString();
  }
};


class ImageAlertType : public AlertType
{
public:
  ImageAlertType(int weight)
  : AlertType(LS("image"), weight)
  {
    m_defaults[ALERT_TRAY_KEY]  = true;
    m_defaults[ALERT_SOUND_KEY] = true;
    m_defaults[ALERT_FILE_KEY]  = LS("Received.wav");

    m_icon = QIcon(LS(":/images/Share/picture.png"));
  }

  QString name() const { return QObject::tr("Image"); }
};


Share::Share(QObject *parent)
  : ChatPlugin(parent)
{
  m_tr  = new SharePluginTr();
  m_net = new QNetworkAccessManager(this);
  new ShareChatView(this);
  new ShareMessages(this);

  ChatCore::translation()->addOther(LS("share"));
  ChatAlerts::add(new ImageAlertType(390));
}


Share::~Share()
{
  delete m_tr;
}


bool Share::cancel(const QString &oid)
{
  m_id.init(oid.toLatin1());
  if (!m_replies.contains(m_id))
    return false;

  m_replies.value(m_id)->close();
  return true;
}


void Share::read(const MessagePacket &packet)
{
  Message message(packet->id(), Message::detectTab(packet->sender(), packet->dest()), LS("image"), LS("addImageMessage"));
  message.setAuthor(packet->sender());
  message.setDate(packet->date());
  message.data().insert(ChannelMessage::kStatus, packet->status());
  message.data().insert(ChannelMessage::kJSON,   packet->json());

  if (!packet->oid.isNull()) {
    message.data().insert(ChannelMessage::kOID, QString(ChatId::toBase32(packet->oid.byteArray())));
    message.data().insert(ChannelMessage::kMDate, packet->mdate);
  }

  TabWidget::add(message);

  if (packet->status() != Notice::OK || packet->sender() == ChatClient::id())
    return;

  Alert alert = Alert(LS("image"), packet->id(), packet->date());
  alert.setTab(packet->sender(), packet->dest());
  ChatAlerts::start(alert);
}


/**
 * \todo Получать адрес сервера изображений от сервера.
 */
bool Share::upload(const ChatId &roomId, const QList<QUrl> &urls)
{
  if (urls.isEmpty())
    return false;

  m_id.init(ObjectId::gen());

  QNetworkRequest request(QUrl("https://upload.schat.me/1/image"));
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  QVariantMap data;
  QVariantList u;
  u.reserve(urls.size());

  foreach (const QUrl &url, urls) {
    u.append(url.toString());
  }

  data.insert(LS("urls"), u);

  QNetworkReply *reply = m_net->post(request, JSON::generate(data));
  reply->setProperty("room", roomId.toByteArray());
  reply->setProperty("id",   m_id.toByteArray());

  connect(reply, SIGNAL(finished()), SLOT(onFinished()));
  connect(reply, SIGNAL(uploadProgress(qint64,qint64)), SLOT(onUploadProgress(qint64,qint64)));

  emit uploadStarted(roomId.toString(), ChatId::toBase32(m_id.oid().byteArray()));
  return true;
}


/**
 * \todo Получать адрес сервера изображений от сервера.
 */
bool Share::upload(const ChatId &roomId, const QStringList &files)
{
  if (files.isEmpty())
    return false;

  m_id.init(ObjectId::gen());
  QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

  foreach (const QString &file, files) {
    const QFileInfo fi(file);
    QString type = fi.suffix().toLower();
    if (type == LS("jpg"))
      type = LS("jpeg");

    QHttpPart part;
    part.setHeader(QNetworkRequest::ContentTypeHeader, "image/" + type.toLatin1());
    part.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=\"image\"; filename=\"" + fi.fileName().toUtf8() + "\"");

    QFile *f = new QFile(file, multiPart);
    if (f->open(QFile::ReadOnly)) {
      part.setBodyDevice(f);
      multiPart->append(part);
    }
  }

  QNetworkRequest request(QUrl("https://upload.schat.me/1/image"));

  QNetworkReply *reply = m_net->post(request, multiPart);
  multiPart->setParent(reply);

  reply->setProperty("room", roomId.toByteArray());
  reply->setProperty("id",   m_id.toByteArray());
  m_replies.insert(m_id, reply);

  connect(reply, SIGNAL(finished()), SLOT(onFinished()));
  connect(reply, SIGNAL(uploadProgress(qint64,qint64)), SLOT(onUploadProgress(qint64,qint64)));

  emit uploadStarted(roomId.toString(), ChatId::toBase32(m_id.oid().byteArray()));
  return true;
}


void Share::onFinished()
{
  QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
  if (!reply)
    return;

  m_id.init(reply->property("id").toByteArray());
  m_roomId.init(reply->property("room").toByteArray());
  m_replies.remove(m_id);

  const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
  m_id.init(reply->property("id").toByteArray());

  emit uploadStatus(m_roomId.toString(), ChatId::toBase32(m_id.oid().byteArray()), status);
  if (status != 200) {
    reply->deleteLater();
    return;
  }

  MessagePacket packet(new MessageNotice(ChatClient::id(), m_roomId.toByteArray(), QString(), DateTime::utc(), m_id.toByteArray()));
  packet->setCommand(LS("image"));

  QVariantMap data = JSON::parse(reply->readAll()).toMap();
  data.insert(LS("format"), 1);
  packet->setData(data);

  ChatClient::io()->send(packet, true);

  reply->deleteLater();
}


void Share::onUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
  QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
  if (!reply)
    return;

  m_id.init(reply->property("id").toByteArray());
  m_roomId.init(reply->property("room").toByteArray());

  emit uploadProgress(m_roomId.toString(), ChatId::toBase32(m_id.oid().byteArray()), bytesSent, bytesTotal);
}


ChatPlugin *SharePlugin::create()
{
  m_plugin = new Share(this);
  return m_plugin;
}

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2(Share, SharePlugin);
#endif
