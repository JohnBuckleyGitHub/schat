#include <QtGui>
#include <QtNetwork>

#include "schatwindow.h"
#include "protocol.h"

SChatWindow::SChatWindow(QWidget *parent)
  : QMainWindow(parent)
{
  setupUi(this);
  
  splitter->setStretchFactor(0, 4);
  splitter->setStretchFactor(1, 1);
  lineEdit_2->setText(defaultNick());
  listView->setModel(&model);
  
  connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(returnPressed()));
  connect(pushButton, SIGNAL(clicked(bool)), this, SLOT(newConnection()));
  
  clientSocket = new ClientSocket(this);
}


void SChatWindow::returnPressed()
{
  QString text = lineEdit->text();
  if (text.isEmpty())
    return;

  if (text.startsWith(QChar('/'))) {
    QColor color = textBrowser->textColor();
    textBrowser->setTextColor(Qt::red);
    textBrowser->append(tr("<div style='color:#da251d;'>! Неизвестная команда: %1</div>").arg(text.left(text.indexOf(' '))));
    textBrowser->setTextColor(color);
  }
  else {
    clientSocket->send(sChatOpcodeSendMessage, text);
  }
  
  scroll();
  lineEdit->clear();  
}


/** [private]
 * 
 */
void SChatWindow::scroll()
{
  QScrollBar *bar = textBrowser->verticalScrollBar(); 
  bar->setValue(bar->maximum());
}


/** [private]
 * 
 */
QString SChatWindow::currentTime()
{
  return QTime::currentTime().toString("hh:mm:ss");
}

void SChatWindow::newConnection()
{
  clientSocket->setNick(lineEdit_2->text());
  clientSocket->connectToHost("192.168.5.134", 7666);
}


/** [private]
 * 
 */
QString SChatWindow::defaultNick()
{
  QString nick;
  QStringList environment = QProcess::systemEnvironment();
  int index = environment.indexOf(QRegExp("USERNAME.*"));

  if (index != -1) {
    QStringList stringList = environment.at(index).split("=");
    if (stringList.size() == 2)
      nick = stringList.at(1).toUtf8();
  }

  if (nick.isEmpty())
    nick = "Newbie";
  
  return nick;
}


/** [public slots]
 * 
 */
void SChatWindow::newParticipant(const QString &p, bool echo)
{
  QStandardItem *item = new QStandardItem(p);
  model.appendRow(item);
  model.sort(0);
  
  if (echo)
    textBrowser->append(tr("<div style='color:#909090'>[%1] <i><b>%2</b> заходит в чат</i></div>").arg(currentTime()).arg(p));
  
  scroll();
  
  qDebug() << "SChatWindow::newParticipant(QString &p, bool echo)" << p;
}


/** [public slots]
 * 
 */
void SChatWindow::newMessage(const QString &nick, const QString &message)
{
  qDebug() << "SChatWindow::newMessage(const QString &nick, const QString &message)";
  
  textBrowser->append(tr("<div><span style='color:#909090'>[%1] &lt;<b>%2</b>&gt;</span> %3</div>").arg(currentTime()).arg(nick).arg(message));
  scroll();
}


/** [public slots]
 * 
 */
void SChatWindow::participantLeft(const QString &nick)
{
  qDebug() << "SChatWindow::participantLeft(const QString &nick)";
  
  QList<QStandardItem *> items;
  items = model.findItems(nick, Qt::MatchFixedString);
  if (items.size() == 1) {
    QModelIndex index = model.indexFromItem(items[0]);
    model.removeRow(index.row());
  }
  
  textBrowser->append(tr("<div style='color:#909090'>[%1] <i><b>%2</b> выходит из чата</i></div>").arg(currentTime()).arg(nick));
  scroll();
}
