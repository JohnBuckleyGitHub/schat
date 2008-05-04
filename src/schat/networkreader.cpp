/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtCore>

#include "networkreader.h"

NetworkReader::NetworkReader()
{

}


/** [public]
 * 
 */
bool NetworkReader::readFile(const QString &fileName)
{
  QFile file(fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
      qDebug()  << "Error: Cannot read file " << fileName << ": " << file.errorString();
      return false;
  }
  
  setDevice(&file);

  while (!atEnd()) {
    readNext();

    if (isStartElement()) {
      if (name() == "network" && attributes().value("version") == "1.0")
        readNetwork();
      else
        raiseError(QObject::tr("The file is not an NETWORK version 1.0 file."));
    }
  }

  return !error();
}


/** [private]
 * 
 */
void NetworkReader::readMeta()
{
  qDebug() << "void Network::readMeta()";
  
  while (!atEnd()) {
    readNext();

    if (isEndElement())
      break;

    if (isStartElement()) {
      if (name() == "name")
        m_name = readElementText();
      else if (name() == "site")
        m_site = readElementText();
      else if (name() == "description")
        m_description = readElementText();
      else
        readUnknownElement();
    }
  }
}


/** [private]
 * 
 */
void NetworkReader::readNetwork()
{
  qDebug() << "void Network::readNetwork()";
  
  while (!atEnd()) {
    readNext();

    if (isEndElement())
      break;

    if (isStartElement()) {
      if (name() == "meta")
        readMeta();
      else if (name() == "servers")
        readServers();
      else
        readUnknownElement();
    }
  }
}


/** [private]
 * 
 */
void NetworkReader::readServers()
{
  qDebug() << "void Network::readServers()";
  
  while (!atEnd()) {
    readNext();

    if (isEndElement())
      break;

    if (isStartElement()) {
      if (name() == "ip")
        m_servers << readElementText();
      else
        readUnknownElement();
    }
  }  
}


/** [private]
 * 
 */
void NetworkReader::readUnknownElement()
{
  qDebug() << "void Network::readUnknownElement()";
  
  while (!atEnd()) {
    readNext();

    if (isEndElement())
      break;

    if (isStartElement())
      readUnknownElement();
  }
}
