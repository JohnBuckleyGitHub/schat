/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
 * Copyright © 2012 Alexey Ivanov <alexey.ivanes@gmail.com>
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

#include <QLabel>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QHeaderView>

#include <QDebug>

#include "ChatCore.h"
#include "ChatSettings.h"
#include "SpellCheckerPage.h"
#include "sglobal.h"

SpellCheckerPage::SpellCheckerPage(QWidget *parent)
  : SettingsPage(QIcon(LS(":/images/SendFile/attach.png")), LS("spellchecker"), parent)
{
  m_label = new QLabel(this);

  m_table = new QTableWidget(this);
  m_table->setSelectionMode (QAbstractItemView::SingleSelection);
  m_table->setSelectionBehavior(QAbstractItemView::SelectRows);

  m_table->setColumnCount(1);
  m_table->setHorizontalHeaderLabels(QStringList() << tr("LANG"));

  //m_table->horizontalHeader()->setResizeMode(0,QHeaderView::ResizeToContents);
  m_table->horizontalHeader()->setResizeMode(0,QHeaderView::Stretch);
  m_table->horizontalHeader()->setSortIndicatorShown(false);
  m_table->horizontalHeader()->setHighlightSections(false);
  m_table->verticalHeader()->hide();

  const QList<QString> dicts = SpellBackend::instance()->dictionaries();

  QStringList enabledDicts = ChatCore::settings()->value(LS("Spellchecker/EnabledDicts")).toStringList();

  for (QList<QString>::const_iterator dict = dicts.begin(); dict != dicts.end(); ++dict)
    {
      QString lang = *dict;
      QLocale locale(lang.replace('-', '_'));
      if (locale.language() != QLocale::C)
      lang = QString("%1, %2 (%3)")
          .arg(QLocale::languageToString(locale.language()))
          .arg(QLocale::countryToString(locale.country()))
          .arg(lang);

      QTableWidgetItem *item = new QTableWidgetItem(lang);
      item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);
      item->setData(Qt::UserRole, *dict);
      if (!enabledDicts.empty())
      {
        for (QList<QString>::const_iterator u = enabledDicts.begin(); u != enabledDicts.end(); ++u)
          {
            if (*dict == *u)
              item->setCheckState(Qt::Checked);
            else
              item->setCheckState(Qt::Unchecked);
          }
      }
      else
      {
        item->setCheckState(Qt::Unchecked);
      }
      int curRow = m_table->rowCount();
      m_table->setRowCount(curRow+1);
      m_table->setItem(curRow,0,item);

    }

//  QHBoxLayout *portLay = new QHBoxLayout();
//  portLay->addWidget(m_portLabel);
//  portLay->addWidget(m_port);
//  portLay->addWidget(m_portLabel2, 1);
//  portLay->setContentsMargins(10, 0, 3, 0);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_label);
  mainLay->addWidget(m_table);

  connect(m_table,SIGNAL(itemChanged(QTableWidgetItem *)),SLOT(modified(QTableWidgetItem *)));

  retranslateUi();
}

void SpellCheckerPage::modified(QTableWidgetItem *item)
{
  Q_UNUSED(item);
  QStringList enabledDicts;
  for (int rows = 0; rows < m_table->rowCount(); ++rows)
    {
      if (m_table->item(rows,0)->checkState() == Qt::Checked)
        {
          enabledDicts.append(m_table->item(rows,0)->data(Qt::UserRole).toString());
        }
    }

    SpellChecker::instance()->setEnabledDicts(enabledDicts);

  ChatCore::settings()->setValue(LS("Spellchecker/EnabledDicts"), enabledDicts);
}

void SpellCheckerPage::retranslateUi()
{
  m_name = tr("Spell Checker");
  m_label->setText(LS("<b>") + m_name + LS("</b>"));
}

SettingsPage* SpellCheckerPageCreator::page(QWidget *parent)
{
  return new SpellCheckerPage(parent);
}
