/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef DOWNLOAD_H_
#define DOWNLOAD_H_

#include <QNetworkAccessManager>
#include <QNetworkReply>

class Download: public QObject
{
  Q_OBJECT

public:
  Download(const QString &targetPath, QObject *parent = 0);  
  void get(const QUrl &url);
  
signals:
  void error();
  void saved(const QString &filename);

private slots:
  void downloadFinished(QNetworkReply *reply);
  
private:
  bool saveToDisk(const QString &filename, QIODevice *data);
  QString saveFileName(const QUrl &url);
  
  QNetworkAccessManager m_manager;
  QString m_targetPath;
};

#endif /*DOWNLOAD_H_*/
