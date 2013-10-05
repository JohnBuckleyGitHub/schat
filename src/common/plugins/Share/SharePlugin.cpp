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

#include <QFileInfo>
#include <QHttpMultiPart>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QtPlugin>

#include "id/ChatId.h"
#include "sglobal.h"
#include "ShareChatView.h"
#include "SharePlugin.h"
#include "SharePlugin_p.h"

#include <QDebug>

Share::Share(QObject *parent)
  : ChatPlugin(parent)
{
  m_net = new QNetworkAccessManager(this);
  new ShareChatView(this);
}


void Share::upload(const ChatId &id, const QStringList &files)
{
  Q_UNUSED(id)

  if (files.isEmpty())
    return;

  QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

  foreach (const QString &file, files) {
    const QFileInfo fi(file);
    QString type = fi.suffix().toLower();
    if (type == LS("jpg"))
      type = LS("jpeg");

    QHttpPart part;
    part.setHeader(QNetworkRequest::ContentTypeHeader, "image/" + type.toLatin1());
    part.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=\"image\"; filename=\"" + fi.fileName().toUtf8() + "\"");

    QFile *f = new QFile(file);
    if (f->open(QFile::ReadOnly)) {
      part.setBodyDevice(f);
      multiPart->append(part);
    }
  }

  QNetworkRequest request(QUrl("https://upload.schat.me/1/image"));

  QNetworkReply *reply = m_net->post(request, multiPart);
  multiPart->setParent(reply);

  connect(reply, SIGNAL(finished()), SLOT(onFinished()));
  connect(reply, SIGNAL(uploadProgress(qint64,qint64)), SLOT(onUploadProgress(qint64,qint64)));
}


void Share::onFinished()
{
  QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
  if (!reply)
    return;

  qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
  qDebug() << reply->readAll();

  reply->deleteLater();
}


void Share::onUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
  qDebug() << bytesSent << bytesTotal;
}


ChatPlugin *SharePlugin::create()
{
  m_plugin = new Share(this);
  return m_plugin;
}

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2(Share, SharePlugin);
#endif
