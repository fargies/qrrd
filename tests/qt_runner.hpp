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
** qt_runner.hpp
**
**        Created on: Dec 23, 2013
**   Original Author: Fargier Sylvain <fargier.sylvain@free.fr>
**
*/

#ifndef __QT_RUNNER_HH__
#define __QT_RUNNER_HH__

#include <string>
#include <QObject>

namespace CppUnit
{
class TestRunner;
class TestResult;


class QtRunner : public QObject
{
    Q_OBJECT

public:
    QtRunner(TestRunner &runner);

    void run(TestResult &controller, const std::string &testPath="");

protected Q_SLOTS:
    void run();

protected:
    TestRunner &m_runner;
    TestResult *m_controller;
    std::string m_path;
};

}

#endif

