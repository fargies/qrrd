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
** test_helpers.cpp
**
**        Created on: Nov 08, 2013
**   Original Author: Fargier Sylvain <fargier.sylvain@free.fr>
**
*/

#include <QCoreApplication>
#include <QFile>

#include "test_helpers.hpp"
#include "config.h"


namespace helpers
{

#ifndef SRC_DATA_DIR
#warning SRC_DATA_DIR undefined
#define SRC_DATA_DIR "."
#endif

#ifndef DATA_DIR
#warning DATA_DIR undefined
#define DATA_DIR "."
#endif

QString getDataPath()
{
    static QString data_dir;

    if (data_dir.isEmpty())
    {
        QString dir;

        dir = QCoreApplication::applicationDirPath() + "/data";
        if (QFile::exists(dir))
            data_dir = dir;
        else if (QFile::exists(SRC_DATA_DIR))
            data_dir = SRC_DATA_DIR;
        else if (QFile::exists(DATA_DIR))
            data_dir = DATA_DIR;
    }
    return data_dir;
}

}

