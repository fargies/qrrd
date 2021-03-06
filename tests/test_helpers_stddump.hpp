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
** test_helpers_stddump.hpp
**
**        Created on: Nov 08, 2013
**   Original Author: Fargier Sylvain <fargier.sylvain@free.fr>
**
*/

#ifndef __TEST_HELPERS_STDDUMP_HPP__
#define __TEST_HELPERS_STDDUMP_HPP__

#include <iostream>
#include <QByteArray>
#include <QString>
#include <QDateTime>

namespace std {

ostream &operator << (ostream &oss, const QByteArray &array);
ostream &operator << (ostream &oss, const QString &str);
ostream &operator << (ostream &oss, const QDateTime &str);

}

#endif // TEST_HELPERS_STDDUMP_HPP
