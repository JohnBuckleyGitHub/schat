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

#include <QGridLayout>
#include <QListWidget>
#include <QPushButton>
#include <QStackedWidget>

#include "abstractsettingsdialog.h"

/*!
 * \brief Конструктор класса AbstractSettingsDialog.
 */
AbstractSettingsDialog::AbstractSettingsDialog(QWidget *parent)
  : QDialog(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);

  m_okButton       = new QPushButton(QIcon(":/images/dialog-ok.png"), tr("OK"), this);
  m_cancelButton   = new QPushButton(QIcon(":/images/dialog-cancel.png"), tr("Cancel"), this);
  m_resetButton    = new QPushButton(QIcon(":/images/undo.png"), "", this);
  m_resetButton->setToolTip(tr("Restore default settings"));
  m_contentsWidget = new QListWidget(this);
  m_contentsWidget->setSpacing(1);
  m_pagesWidget    = new QStackedWidget(this);

  QFrame *line = new QFrame(this);
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);

  connect(m_contentsWidget, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(changePage(QListWidgetItem *, QListWidgetItem*)));
  connect(m_okButton, SIGNAL(clicked()), SLOT(accept()));
  connect(m_cancelButton, SIGNAL(clicked()), SLOT(close()));
  connect(m_resetButton, SIGNAL(clicked()), SLOT(reset()));

  QHBoxLayout *buttonLayout = new QHBoxLayout;
  buttonLayout->addStretch();
  buttonLayout->addWidget(m_resetButton);
  buttonLayout->addWidget(m_okButton);
  buttonLayout->addWidget(m_cancelButton);
  buttonLayout->setSpacing(3);

  QGridLayout *mainLayout = new QGridLayout(this);
  mainLayout->setColumnStretch(0, 1);
  mainLayout->setColumnStretch(1, 4);
  mainLayout->addWidget(m_contentsWidget, 0, 0);
  mainLayout->addWidget(m_pagesWidget, 0, 1);
  mainLayout->addWidget(line, 1, 0, 1, 2);
  mainLayout->addLayout(buttonLayout, 2, 0, 1, 2);
  mainLayout->setMargin(3);
  mainLayout->setSpacing(3);
  m_badSize = true;

  setWindowTitle(tr("Settings"));
}


void AbstractSettingsDialog::setPage(int page)
{
  m_contentsWidget->setCurrentRow(page);
  m_pagesWidget->setCurrentIndex(page);
}


void AbstractSettingsDialog::showEvent(QShowEvent *event)
{
  if (m_badSize) {
    m_contentsWidget->setMinimumSize(m_contentsWidget->width() + 8, m_contentsWidget->height());
    resize(100, 100);
    m_contentsWidget->setCurrentRow(0);
    m_badSize = false;
  }

  QDialog::showEvent(event);
}


void AbstractSettingsDialog::accept()
{
  emit save();
  close();
}


void AbstractSettingsDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
  if (!current)
    current = previous;

  m_pagesWidget->setCurrentIndex(m_contentsWidget->row(current));
}


void AbstractSettingsDialog::reset() { emit reset(m_pagesWidget->currentIndex()); }


void AbstractSettingsDialog::createPage(const QIcon &icon, const QString &text, AbstractSettingsPage *page)
{
  m_pagesWidget->addWidget(page);
  new QListWidgetItem(icon, text, m_contentsWidget);
  connect(this, SIGNAL(save()), page, SLOT(save()));
  connect(this, SIGNAL(reset(int)), page, SLOT(reset(int)));
}


/*!
 * \brief Конструктор класса AbstractSettingsPage.
 */
AbstractSettingsPage::AbstractSettingsPage(int id, QWidget *parent)
  : QWidget(parent), m_id(id)
{
}
