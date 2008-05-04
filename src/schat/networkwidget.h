/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef NETWORKWIDGET_H_
#define NETWORKWIDGET_H_

#include <QString>
#include <QWidget>

class QComboBox;
class QLabel;
class QSpinBox;
class QStandardItemModel;

class NetworkWidget : public QWidget {
  Q_OBJECT

public:
  NetworkWidget(QStandardItemModel *model, QWidget *parent = 0);
  void init();

private slots:
  void activated(int index);
  void currentIndexChanged(int index);
 
private:
  QComboBox *m_selectCombo;
  QLabel *m_infoLabel;
  QLabel *m_portLabel;
  QSpinBox *m_portBox;
  QStandardItemModel *m_model;
  QString m_networkPath;  
};

#endif /*NETWORKWIDGET_H_*/
