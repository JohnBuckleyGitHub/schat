#ifndef SPELLCHECKERPLUGIN_H_
#define SPELLCHECKERPLUGIN_H_

#include "ChatApi.h"
#include "CoreApi.h"

class SpellcheckerPlugin : public QObject, CoreApi, ChatApi
{
  Q_OBJECT
  Q_INTERFACES(CoreApi ChatApi)

public:
  QVariantMap header() const
  {
    QVariantMap out = CoreApi::header();
    out["Id"]       = "SpellChecker";
    out["Name"]     = "Spell Checker";
    out["Version"]  = "0.1.0";
    out["Site"]     = "http://wiki.schat.me/Plugin/SpellChecker";
    out["Desc"]     = "Spell Checker";
    out["Desc/ru"]  = "Автоматическая проверка орфографии";
    out["Required"] = "1.99.37";

    return out;
  }

  ChatPlugin *create();
};

#endif /* SPELLCHECKERPLUGIN_H_ */
