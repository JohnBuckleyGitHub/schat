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
  Download(QObject *parent = 0);  
  inline void setBasePath(const QString &path) { m_basePath = path; }
  void get(const QUrl &url);
  
signals:
  void saved(const QString &filename);

private slots:
  void downloadFinished(QNetworkReply *reply);
  
private:
  bool saveToDisk(const QString &filename, QIODevice *data);
  QString saveFileName(const QUrl &url);
  
  QNetworkAccessManager m_manager;
  QString m_basePath;
};

#endif /*DOWNLOAD_H_*/
