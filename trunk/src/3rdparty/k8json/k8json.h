/* coded by Ketmar // Vampire Avalon (psyc://ketmar.no-ip.org/~Ketmar)
 * Understanding is not required. Only obedience.
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details.
 */
#ifndef K8JSON_H
#define K8JSON_H

#include <QHash>
#include <QList>
#include <QMap>
#include <QString>
#include <QVariant>

namespace K8JSON {

/*
 * quote string to JSON-friendly format, add '"'
 */
QString quote (const QString &str);

/*
 * check if given (const uchar *) represents valid UTF-8 sequence
 * NULL (or empty) s is not valid
 * sequence ends on '\0' if zeroInvalid==false
 */
bool isValidUtf8 (const uchar *s, int maxLen, bool zeroInvalid=false);


/*
 * skip blanks and comments
 * return ptr to first non-blank char or 0 on error
 * 'maxLen' will be changed
 */
const uchar *skipBlanks (const uchar *s, int *maxLength);

/*
 * skip one record
 * the 'record' is either one full field ( field: val)
 * or one list/object.
 * return ptr to the first non-blank char after the record (or 0)
 * 'maxLen' will be changed
 */
const uchar *skipRec (const uchar *s, int *maxLength);

/*
 * parse field value
 * return ptr to the first non-blank char after the value (or 0)
 * 'maxLen' will be changed
 */
const uchar *parseValue (QVariant &fvalue, const uchar *s, int *maxLength);


/*
 * parse one field (f-v pair)
 * return ptr to the first non-blank char after the record (or 0)
 * 'maxLen' will be changed
 */
const uchar *parseField (QString &fname, QVariant &fvalue, const uchar *s, int *maxLength);

/*
 * parse one record (list or object)
 * return ptr to the first non-blank char after the record (or 0)
 * 'maxLen' will be changed
 */
const uchar *parseRecord (QVariant &res, const uchar *s, int *maxLength);


/*
 * generate JSON text from variant
 */
bool generate(QByteArray &res, const QVariant &val, int indent = 0);

}


#endif
