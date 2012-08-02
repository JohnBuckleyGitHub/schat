#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include <QObject>
#include <QMenu>
#include <QTextEdit>
#include <QDebug>

#include "SpellHighlighter.h"
#include "SpellBackend.h"
#include "SpellcheckerPlugin.h"

#include "ui/SendWidget.h"
#include "ui/InputWidget.h"

class Spellchecker : public QObject
{
  Q_OBJECT

public:
  static Spellchecker* instance();

signals:

protected:
  Spellchecker();
  ~Spellchecker();

  void appendHL(QTextDocument *ADocument);

  
public slots:
  void showContextMenu(const QPoint &pt);
  void repairWord();
  void setEnabledDicts(QList<QString> &dicts);
  void addWordToDict();

private:
  static Spellchecker *FInstance;

  QTextEdit* FCurrentTextEdit;
  int FCurrentCursorPosition;
  SpellHighlighter *FSH;
  QMenu *suggestMenu(const QString &word);
  
};

#endif // SPELLCHECKER_H
