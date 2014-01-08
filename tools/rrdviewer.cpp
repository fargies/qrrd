/*
** Copyright (C) 2014 Fargier Sylvain <fargier.sylvain@free.fr>
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
** rrdviewer.cpp
**
**        Created on: Dec 27, 2013
**   Original Author: Sylvain Fargier <fargier.sylvain@free.fr>
**
*/

#include <QApplication>

#include "RRDFile.hpp"
#include "RRDWidget.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QStringList args = app.arguments();
    RRDWidget *widget = new RRDWidget();

    args.pop_front();
    QStringList::const_iterator it = args.constBegin();
    for (; it != args.constEnd(); ++it)
    {
        widget->addSource(RRDFile(*it));
    }
    //FIXME: drag & drop ?

    widget->show();
    return app.exec();
}

