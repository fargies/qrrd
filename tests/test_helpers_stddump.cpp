/*
** Copyright (C) 2013 Fargier Sylvain <fargier.sylvain@free.fr>
**
** This software is provided 'as-is', without any express or implied
** warranty.  In no event will the authors be held liable for any damages
** arising from the use of this software.
**
** Permission is granted to anyone to use this software for any purpose,
** including commercial applications, and to alter it and redistribute it
** freely, subject to the following restrictions:
**
** 1. The origin of this software must not be misrepresented; you must not
**    claim that you wrote the original software. If you use this software
**    in a product, an acknowledgment in the product documentation would be
**    appreciated but is not required.
** 2. Altered source versions must be plainly marked as such, and must not be
**    misrepresented as being the original software.
** 3. This notice may not be removed or altered from any source distribution.
**
** test_helpers_stddump.cpp
**
**        Created on: Nov 08, 2013
**   Original Author: Fargier Sylvain <fargier.sylvain@free.fr>
**
*/

#include <QString>
#include <QDebug>
#include "test_helpers_stddump.hpp"

namespace std {

ostream &operator << (ostream &oss, const QByteArray &array)
{
    oss << "QByteArray(" << array.toHex().toUpper().constData() << ")";
    return oss;
}

ostream &operator << (ostream &oss, const QString &str)
{
    oss << "QString(" << str.toAscii().constData() << ")";
    return oss;
}

ostream &operator << (ostream &oss, const QDateTime &date)
{
    if (date.isValid())
        oss << "QDateTime(" << qPrintable(date.toString()) << " ("
            << date.toTime_t() << "))";
    else
        oss << "QDateTime(Invalid)";
    return oss;
}

}
