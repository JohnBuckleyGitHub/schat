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

#ifndef SHAREPLUGIN_P_H_
#define SHAREPLUGIN_P_H_

#include "plugins/ChatPlugin.h"

class ChatId;
class ChatView;
class QNetworkAccessManager;

class Share : public ChatPlugin
{
  Q_OBJECT

public:
  Share(QObject *parent);
  void upload(const ChatId &id, const QStringList &files);

private slots:
  void onFinished();
  void onUploadProgress(qint64 bytesSent, qint64 bytesTotal);

private:
  QNetworkAccessManager *m_net;
};

#endif /* SHAREPLUGIN_P_H_ */