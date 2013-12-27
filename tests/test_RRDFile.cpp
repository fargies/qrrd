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
** test_RRDFile.cpp
**
**        Created on: Dec 23, 2013
**   Original Author: Fargier Sylvain <fargier.sylvain@free.fr>
**
*/

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "test_helpers.hpp"
#include "RRDFile.hpp"

class RRDFileTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(RRDFileTest);
    CPPUNIT_TEST(load);
    CPPUNIT_TEST(fetch);
    CPPUNIT_TEST_SUITE_END();

protected:
    void load()
    {
        const RRDFile f(helpers::getDataPath() + "/sample.rrd");

        CPPUNIT_ASSERT(f.isValid());
        CPPUNIT_ASSERT_EQUAL((uint) 300, f.pdpStep());
        CPPUNIT_ASSERT_EQUAL(QString("0003"), f.version());
        CPPUNIT_ASSERT_EQUAL(QDateTime::fromTime_t(1387630200), f.lastUpdate());

        CPPUNIT_ASSERT_EQUAL(2, f.dsNames().size());

        CPPUNIT_ASSERT_EQUAL(10, f.rra().size());
        CPPUNIT_ASSERT(f.hasFunc(RRA::AVERAGE));
        CPPUNIT_ASSERT(f.hasFunc(RRA::MAX));
        CPPUNIT_ASSERT_EQUAL(2, f.consFuncs().size());

        CPPUNIT_ASSERT_EQUAL(QDateTime::fromTime_t(1318809600),
                             f.firstUpdate(RRA::AVERAGE));
        CPPUNIT_ASSERT_EQUAL(QDateTime::fromTime_t(1387630200),
                             f.lastUpdate(RRA::AVERAGE));
    }

    void fetch()
    {
        RRDFile f(helpers::getDataPath() + "/sample.rrd");

        CPPUNIT_ASSERT(f.fetch(RRA::AVERAGE,
                               f.firstUpdate(RRA::AVERAGE),
                               f.lastUpdate(RRA::AVERAGE)));
        CPPUNIT_ASSERT_EQUAL(f.firstUpdate(RRA::AVERAGE), f.first());
        CPPUNIT_ASSERT_EQUAL(QDateTime::fromTime_t(1387670400), f.last());
        CPPUNIT_ASSERT_EQUAL((uint) 798, f.itemCount());
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(RRDFileTest);
