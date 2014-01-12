/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2014 IMPOMEZIA <schat@impomezia.com>
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
#include <QTimer>
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
#include "ShareButton.h"
#include "ShareChatView.h"
#include "ShareMessages.h"
#include "SharePlugin.h"
#include "SharePlugin_p.h"
#include "text/TokenFilter.h"
#include "Tr.h"
#include "Translation.h"
#include "ui/SendWidget.h"
#include "ui/TabWidget.h"
#include "UploadData.h"

#define API_URL QLatin1String("https://upload.schat.me/1/image")

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
    else if (key == LS("img-desc"))     return tr("Image description");
    else if (key == LS("upload"))       return tr("Upload");

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

  QTimer::singleShot(0, this, SLOT(start()));
}


Share::~Share()
{
  qDeleteAll(m_data);
  delete m_tr;
}


bool Share::cancel(const QString &oid)
{
  m_id.init(oid.toLatin1());
  if (!m_data.contains(m_id))
    return false;

  UploadData *data = m_data.take(m_id);
  if (data->reply)
    data->reply->close();

  delete data;
  return true;
}


void Share::upload(const QString &oid, const QString &desc)
{
  m_id.init(oid.toLatin1());
  if (!m_data.contains(m_id))
    return;

  UploadData *data = m_data.value(m_id);
  data->setDesc(desc);
  const QList<QUrl> &urls = data->urls();

  if (data->type() == UploadData::Local) {
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    foreach (const QUrl &url, urls) {
      const QString file = url.toLocalFile();
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

    QNetworkRequest request(QUrl(API_URL));

    data->reply = m_net->post(request, multiPart);
    multiPart->setParent(data->reply);
  }
  else {
    QNetworkRequest request(QUrl(API_URL));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QVariantMap body;
    QVariantList u;
    u.reserve(urls.size());

    foreach (const QUrl &url, urls) {
      u.append(url.toString());
    }

    body.insert(LS("urls"), u);

    data->reply = m_net->post(request, JSON::generate(body));
  }

  data->reply->setProperty("room", data->roomId().toByteArray());
  data->reply->setProperty("id",   m_id.toByteArray());

  connect(data->reply, SIGNAL(finished()), SLOT(onFinished()));
  connect(data->reply, SIGNAL(uploadProgress(qint64,qint64)), SLOT(onUploadProgress(qint64,qint64)));

  emit uploadStarted(data->roomId().toString(), ChatId::toBase32(m_id.oid().byteArray()));
}


void Share::read(const MessagePacket &packet)
{
  Message message(packet->id(), Message::detectTab(packet->sender(), packet->dest()), LS("image"), LS("addImageMessage"));
  message.setAuthor(packet->sender());
  message.setDate(packet->date());
  message.data().insert(ChannelMessage::kStatus, packet->status());
  message.data().insert(ChannelMessage::kJSON,   packet->json());
  message.data().insert(ChannelMessage::kText,   TokenFilter::filter(LS("channel"), packet->text()));

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


bool Share::upload(const ChatId &roomId, const QList<QUrl> &urls, bool local)
{
  if (urls.isEmpty())
    return false;

  add(new UploadData(roomId, urls, local ? UploadData::Local : UploadData::Remote));
  return true;
}


void Share::onFinished()
{
  QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
  if (!reply)
    return;

  m_id.init(reply->property("id").toByteArray());
  m_roomId.init(reply->property("room").toByteArray());

  UploadData *data = 0;
  if (m_data.contains(m_id))
    data = m_data.take(m_id);

  const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
  m_id.init(reply->property("id").toByteArray());

  emit uploadStatus(m_roomId.toString(), ChatId::toBase32(m_id.oid().byteArray()), status);
  if (status == 200) {
    MessagePacket packet(new MessageNotice(ChatClient::id(), m_roomId.toByteArray(), data ? data->desc() : QString(), DateTime::utc(), m_id.toByteArray()));
    packet->setCommand(LS("image"));

    QVariantMap body = JSON::parse(reply->readAll()).toMap();
    body.insert(LS("format"), 1);
    packet->setData(body);

    ChatClient::io()->send(packet, true);
  }

  reply->deleteLater();

  if (data)
    delete data;
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


void Share::start()
{
  SendWidget::add(new ShareAction(this));
}


void Share::add(UploadData *data)
{
  if (!data)
    return;

  if (m_data.contains(data->id()))
    delete m_data.take(data->id());

  emit uploadAdded(data->data());
  m_data.insert(data->id(), data);
}


ChatPlugin *SharePlugin::create()
{
  m_plugin = new Share(this);
  return m_plugin;
}

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2(Share, SharePlugin);
#endif
