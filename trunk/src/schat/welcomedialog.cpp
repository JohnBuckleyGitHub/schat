/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtGui>

#include "networkwidget.h"
#include "profile.h"
#include "profilewidget.h"
#include "schatwindow.h"
#include "settings.h"
#include "welcomedialog.h"


/** [public]
 * Конструктор
 */
WelcomeDialog::WelcomeDialog(Settings *settings, Profile *profile, QWidget *parent)
  : QDialog(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);
  
  m_settings = settings;
  
  m_profileWidget = new ProfileWidget(profile, this);
  m_askCheckBox = new QCheckBox(tr("Больше не показывать это окно"), this);
  m_okButton    = new QPushButton(QIcon(":/images/ok.png"), tr("ОК"), this);
  m_moreButton  = new QPushButton(QIcon(":/images/down.png"), tr(""), this);
  m_moreButton->setCheckable(true);
  m_networkWidget = new NetworkWidget(m_settings, this);
  m_networkWidget->setVisible(false);
  
  QFrame *line = new QFrame(this);
  line->setObjectName(QString::fromUtf8("line"));
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(m_profileWidget);
  mainLayout->addWidget(line);
  
  QHBoxLayout *buttonsLayout = new QHBoxLayout;
  buttonsLayout->addWidget(m_askCheckBox);
  buttonsLayout->addStretch();
  buttonsLayout->addWidget(m_okButton);
  buttonsLayout->addWidget(m_moreButton);
  buttonsLayout->setSpacing(3);
  
  mainLayout->addLayout(buttonsLayout);
  mainLayout->addWidget(m_networkWidget);
  mainLayout->setMargin(6);
  mainLayout->setSpacing(6);
  mainLayout->setSizeConstraint(QLayout::SetFixedSize);
  
  m_profileWidget->setMinimumSize(m_networkWidget->minimumSizeHint().width(), m_profileWidget->minimumSizeHint().height());
  
  if (m_settings->firstRun)
    m_askCheckBox->setChecked(true);
  
  connect(m_moreButton, SIGNAL(toggled(bool)), this, SLOT(changeIcon(bool)));
  connect(m_okButton, SIGNAL(clicked()), this, SLOT(accept()));
  connect(m_profileWidget, SIGNAL(validNick(bool)), this, SLOT(validNick(bool)));
  
  setWindowTitle(tr("Simple Chat - Выбор ника"));
}


/** [public slots]
 *
 */
void WelcomeDialog::accept()
{
  qDebug() << "void WelcomeDialog::accept()";
  
  m_profileWidget->save();
  m_settings->hideWelcome = m_askCheckBox->isChecked();
  
  QDialog::accept();
}


/** [private slots]
 *
 */
void WelcomeDialog::changeIcon(bool s)
{
  if (s) {
    m_moreButton->setIcon(QIcon(":/images/up.png"));
    m_networkWidget->setVisible(true);
  }
  else {
    m_moreButton->setIcon(QIcon(":/images/down.png"));
    m_networkWidget->setVisible(false);
  }
}
