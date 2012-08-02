#include "Spellchecker.h"
#include <QCoreApplication>

Spellchecker* Spellchecker::FInstance = NULL;

Spellchecker* Spellchecker::instance()
{
	if (!FInstance)
	{
		FInstance = new Spellchecker();
	}
	return FInstance;
}

Spellchecker::Spellchecker() : QObject(QCoreApplication::instance()), FCurrentTextEdit(NULL), FCurrentCursorPosition(0)
{

    qDebug() << "TEST";

    InputWidget *textEdit = SendWidget::i()->input();
    textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(textEdit, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showContextMenu(const QPoint &)));

    FSH = new SpellHighlighter(textEdit->document());

}

Spellchecker::~Spellchecker()
{

}

QMenu* Spellchecker::suggestMenu(const QString &word)
{
    QList<QString> sgstions = SpellBackend::instance()->suggestions(word);
    QMenu *menu = new QMenu(tr("Suggestions"));

    for (QList<QString>::const_iterator sgstion = sgstions.begin(); sgstion != sgstions.end(); ++sgstion)
    {
        QAction *action = menu->addAction(*sgstion, this, SLOT(repairWord()));
        action->setProperty("word", *sgstion);
        action->setParent(menu);
    }

    return menu;
}

void Spellchecker::showContextMenu(const QPoint &pt)
{
    FCurrentTextEdit = qobject_cast<InputWidget *>(sender());
    Q_ASSERT(FCurrentTextEdit);

    QMenu *menu = FCurrentTextEdit->createStandardContextMenu();

    menu->addSeparator();

    QMenu *sugMenu = NULL;
    Q_ASSERT(!sugMenu);

    QTextCursor cursor = FCurrentTextEdit->cursorForPosition(pt);
    FCurrentCursorPosition = cursor.position();
    cursor.select(QTextCursor::WordUnderCursor);
    const QString word = cursor.selectedText();

    if (!word.isEmpty() && !SpellBackend::instance()->isCorrect(word)) {
        sugMenu = suggestMenu(word);

        if (!sugMenu->isEmpty()) {
            menu->addMenu(sugMenu);
        }

        QAction *action = menu->addAction(tr("Add to dictionary"), this, SLOT(addWordToDict()));
        action->setParent(menu);
    }

    menu->exec(FCurrentTextEdit->mapToGlobal(pt));

    if (sugMenu) {
        delete sugMenu;
    }
    delete menu;
}

void Spellchecker::repairWord()
{
    QAction *action = qobject_cast<QAction *>(sender());
    Q_ASSERT(action);
    if (!action)
    {
        return;
    }

    QTextCursor cursor = FCurrentTextEdit->textCursor();

    cursor.beginEditBlock();
    cursor.setPosition(FCurrentCursorPosition, QTextCursor::MoveAnchor);
    cursor.select(QTextCursor::WordUnderCursor);
    cursor.removeSelectedText();
    cursor.insertText(action->property("word").toString());
    cursor.endEditBlock();

#if QT_VERSION >= 0x040600 // Qt 4.5
    FSH->rehighlightBlock(cursor.block());
#else
    FSH->rehighlight();
#endif
}

void Spellchecker::setEnabledDicts(QList<QString> &dicts)
{
    delete FSH;
    SpellBackend::instance()->setLangs(dicts);
    InputWidget *textEdit = SendWidget::i()->input();
    FSH = new SpellHighlighter(textEdit->document());
    FSH->rehighlight();
}

void Spellchecker::addWordToDict()
{
    QAction *action = qobject_cast<QAction *>(sender());
    Q_ASSERT(action);
    if (!action)
    {
        return;
    }

    QTextCursor cursor = FCurrentTextEdit->textCursor();
    cursor.setPosition(FCurrentCursorPosition, QTextCursor::MoveAnchor);
    cursor.select(QTextCursor::WordUnderCursor);
    const QString word = cursor.selectedText();

    SpellBackend::instance()->add(word);

#if QT_VERSION >= 0x040600 // Qt 4.5
    FSH->rehighlightBlock(cursor.block());
#else
    FSH->rehighlight();
#endif
}

