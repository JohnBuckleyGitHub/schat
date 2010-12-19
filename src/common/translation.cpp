/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2010 IMPOMEZIA <schat@impomezia.com>
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

#include <QCoreApplication>
#include <QTranslator>
#include <QDebug>

#include "translation.h"

/*!
 * Конструктор класса Translation.
 */
Translation::Translation(QObject *parent)
  : QObject(parent),
  m_empty(true)
{
  m_language = tr("English");
  m_core = new QTranslator(this);
}


void Translation::load(const QString &name)
{
//  qDebug() << this << "load" << name << m_search;

  if (!m_empty) {
    QCoreApplication::removeTranslator(m_core);
  }
  else
    m_empty = false;

  qDebug() << QLocale::system().name();
//  QString fileName;
//  if (name.endsWith(".qm")) {
//    m_name = detectName(name);
//    fileName = name;
//  }
}


void Translation::setSearch(const QStringList &search)
{
  m_search = search;
  m_search.append(":/translations");
}
