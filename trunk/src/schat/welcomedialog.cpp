/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtGui>

#include "welcomedialog.h"
#include "schatwindow.h"
#include "settings.h"
#include "profile.h"
#include "profilewidget.h"


/** [public]
 * Конструктор
 */
WelcomeDialog::WelcomeDialog(Settings *s, Profile *p, QWidget *parent)
  : QDialog(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);
  
  settings = s;
  profile  = p;
  
  profileWidget = new ProfileWidget(profile, this);
  askCheckBox = new QCheckBox(tr("Больше не показывать это окно"), this);
  okButton    = new QPushButton(QIcon(":/images/ok.png"), tr("ОК"), this);
  moreButton  = new QPushButton(QIcon(":/images/down.png"), tr(""), this);
  moreButton->setCheckable(true);
  serverLabel = new QLabel(tr("Адрес сервера:"), this);
  serverEdit = new QLineEdit(settings->server, this);
  
  QFrame *line = new QFrame(this);
  line->setObjectName(QString::fromUtf8("line"));
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  
  serverLabel->setVisible(false);
  serverEdit->setVisible(false);
  
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(profileWidget);
  mainLayout->addWidget(line);
  
  QHBoxLayout *buttonsLayout = new QHBoxLayout;
  buttonsLayout->addWidget(askCheckBox);
  buttonsLayout->addStretch();
  buttonsLayout->addWidget(okButton);
  buttonsLayout->addWidget(moreButton);
  buttonsLayout->setSpacing(3);
  
  QHBoxLayout *serverLayout = new QHBoxLayout;
  serverLayout->addWidget(serverLabel);
  serverLayout->addWidget(serverEdit);
  
  mainLayout->addLayout(buttonsLayout);
  mainLayout->addLayout(serverLayout);
  mainLayout->setMargin(6);
  mainLayout->setSpacing(6);
  mainLayout->setSizeConstraint(QLayout::SetFixedSize);
  
  if (!settings->firstRun)
    askCheckBox->setChecked(settings->hideWelcome);
  
  connect(moreButton, SIGNAL(toggled(bool)), this, SLOT(changeIcon(bool)));
  connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
  connect(profileWidget, SIGNAL(validNick(bool)), this, SLOT(validNick(bool)));
  
  setWindowTitle(tr("Simple Chat - Выбор ника"));
}


/** [public]
 *
 */
void WelcomeDialog::accept()
{
  profileWidget->save();
  settings->server = serverEdit->text();
  settings->hideWelcome = askCheckBox->isChecked();
  
  QDialog::accept();
}


/** [private slots]
 *
 */
void WelcomeDialog::changeIcon(bool s)
{
  if (s) {
    moreButton->setIcon(QIcon(":/images/up.png"));
    serverLabel->setVisible(true);
    serverEdit->setVisible(true);
  }
  else {
    moreButton->setIcon(QIcon(":/images/down.png"));
    serverLabel->setVisible(false);
    serverEdit->setVisible(false);
  }
}
