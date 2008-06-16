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
  inline void error() { error(404); }
  void saved(const QString &filename);
  
private:
  bool createQueue(const QString &filename);
  bool verifyFile(const FileInfo &fileInfo);
  inline bool verifyFile() { return verifyFile(currentFile); };
  void downloadNext();
  void error(int err);
  void finished();
  void writeSettings(bool err = false) const;
  
  Download *m_download;
  FileInfo currentFile;
  QQueue<FileInfo> m_queue;
  QString m_appPath;
  QString m_targetPath;
  QString m_urlPath;
  QStringList m_files;
  QUrl m_url;
  State m_state;
  UpdateXmlReader m_reader;
};

#endif /*UPDATE_H_*/
