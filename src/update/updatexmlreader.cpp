/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtCore>

#include "updatexmlreader.h"


/** [public]
 * 
 */
UpdateXmlReader::UpdateXmlReader()
{

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
        raiseError(QObject::tr("BAD FILE FORMAT"));
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
      if (name() == "file")
        qDebug() << readElementText();
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
      if (name() == "latest")
        qDebug() << readElementText();
      else if (name() == "level")
        qDebug() << readElementText();
      else if (name() == "order")
        qDebug() << readElementText();
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
      else if (name() == "files")
        readFiles();
      else
        readUnknownElement();
    }
  }  
}
