/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtCore>

#include "updatexmlreader.h"
#include "version.h"


/** [public]
 * 
 */
UpdateXmlReader::UpdateXmlReader()
{
  m_coreLevel = -1;
  m_qtLevel = -1;
}


/** [public]
 * 
 */
bool UpdateXmlReader::isUpdateAvailable() const
{
  if (m_list.isEmpty())
    return false;
  
  if (m_qtLevel > UpdateLevelQt)
    return true;
  
  if (m_coreLevel > UpdateLevelCore)
    return true;
  
  return false;
}


/** [public]
 * 
 */
bool UpdateXmlReader::readFile(const QString &fileName)
{
  QFile file(fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text))
    return false;
  
  setDevice(&file);

  while (!atEnd()) {
    readNext();

    if (isStartElement()) {
      if (name() == "updates" && attributes().value("version") == "1.0")
        readUpdates();
      else
        raiseError(QObject::tr("BAD FILE FORMAT OR VERSION"));
    }
  }

  return !error();
}


/** [private]
 * 
 */
void UpdateXmlReader::readCumulative()
{
  while (!atEnd()) {
    readNext();

    if (isEndElement())
      break;

    if (isStartElement()) {
      if (name() == "file") {
        FileInfo fileInfo;
        fileInfo.type  = attributes().value("type").toString();
        fileInfo.level = attributes().value("level").toString().toInt();
        
        if ((fileInfo.type == "qt" && fileInfo.level == m_qtLevel) || (fileInfo.type == "core" && fileInfo.level == m_coreLevel)) {
          fileInfo.size = attributes().value("size").toString().toULongLong();
          fileInfo.md5  = attributes().value("md5").toString();
          fileInfo.name = readElementText();
          m_list << fileInfo;
        }         
      }
      else
        readUnknownElement();
    }
  } 
}


/** [private]
 * 
 */
void UpdateXmlReader::readFiles()
{
  while (!atEnd()) {
    readNext();

    if (isEndElement())
      break;

    if (isStartElement()) {
      if (name() == "cumulative")
        readCumulative();
      else
        readUnknownElement();
    }
  }
}


/** [private]
 * 
 */
void UpdateXmlReader::readMeta()
{
  while (!atEnd()) {
    readNext();

    if (isEndElement())
      break;

    if (isStartElement()) {
      if (name() == "core") {
        m_coreLevel = attributes().value("level").toString().toInt();
        m_core = readElementText();
      }
      else if (name() == "qt") {
        m_qtLevel = attributes().value("level").toString().toInt();
        m_qt = readElementText();
      }
      else
        readUnknownElement();
    }
  }
}


/** [private]
 * 
 */
void UpdateXmlReader::readUnknownElement()
{
  while (!atEnd()) {
    readNext();

    if (isEndElement())
      break;

    if (isStartElement())
      readUnknownElement();
  }
}


/** [private]
 * 
 */
void UpdateXmlReader::readUpdates()
{
  while (!atEnd()) {
    readNext();

    if (isEndElement())
      break;

    if (isStartElement()) {
      if (name() == "meta")
        readMeta();
      else if (name() == "files" && attributes().value("platform") == "win32") 
        readFiles();
      else
        readUnknownElement();
    }
  }  
}
