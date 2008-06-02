/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef UPDATE_H_
#define UPDATE_H_

#include <QObject>
#include <QQueue>
#include <QUrl>

#include "updatexmlreader.h"

class Download;

static const QString DefaultUpdateXmlUrl = "http://achim.net/schat/updates/update.xml";

class Update : public QObject {
  Q_OBJECT
  
public:
  enum State {
    GettingUpdateXml,
    GettingUpdates
  };
  
  Update(const QUrl &url, QObject *parent = 0);
  
public slots:
  void execute();
  
private slots:
  void error();
  void saved(const QString &filename);
  
private:
  void writeSettings();
  
  Download *m_download;
  QQueue<QString> m_queue;
  QString m_appPath;
  QString m_updatesPath;
  QStringList m_files;
  QUrl m_url;
  State m_state;
  UpdateXmlReader m_reader;
};

#endif /*UPDATE_H_*/
