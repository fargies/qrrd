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
** RRA.cpp
**
**        Created on: Dec 21, 2013
**   Original Author: Fargier Sylvain <fargier.sylvain@free.fr>
**
*/

#include <QSharedData>
#include <QHash>
#include <QDateTime>

#include "RRA.hpp"
#include "RRDFile.hpp"

struct RRAPrivate : public QSharedData
{
    RRAPrivate(const RRDFile &parent) :
        QSharedData(),
        function(static_cast<RRA::ConsFunc>(-1)),
        pdpStep(parent.pdpStep()),
        lastUpdate(parent.lastUpdate().toTime_t())
    {}

    uint first() const
    {
        return lastUpdate - (pdpPerRow * (rowCount - 1) * pdpStep);
    }

    RRA::ConsFunc function;
    uint pdpPerRow;
    uint rowCount;
    qreal xff;
    uint pdpStep;
    uint lastUpdate;
};

static QHash<QString, RRA::ConsFunc> mapInit()
{
    QHash<QString, RRA::ConsFunc> map;
    map["AVERAGE"] = RRA::AVERAGE;
    map["LAST"] = RRA::LAST;
    map["MAX"] = RRA::MAX;
    map["MIN"] = RRA::MIN;
    return map;
}

static QHash<QString, RRA::ConsFunc> s_map = mapInit();

RRA::RRA() :
    d_ptr()
{}

RRA::RRA(const RRDFile &parent) :
    d_ptr(new RRAPrivate(parent))
{}

RRA::~RRA()
{}

RRA::RRA(const RRA &other) :
    d_ptr(other.d_ptr)
{}

RRA &RRA::operator =(const RRA &other)
{
    if (this != &other)
        this->d_ptr = other.d_ptr;
    return *this;
}

bool RRA::isValid() const
{
    return d_ptr && d_ptr->function != static_cast<ConsFunc>(-1);
}

RRA::ConsFunc RRA::function() const
{
    return d_ptr->function;
}

uint RRA::pdpPerRow() const
{
    return d_ptr->pdpPerRow;
}

uint RRA::rowCount() const
{
    return d_ptr->rowCount;
}

qreal RRA::xff() const
{
    return d_ptr->xff;
}

uint RRA::pdpStep() const
{
    return d_ptr->pdpStep;
}

void RRA::setFunction(RRA::ConsFunc func)
{
    d_ptr->function = func;
}

void RRA::setPdpPerRow(uint pdpPerRow)
{
    d_ptr->pdpPerRow = pdpPerRow;
    d_ptr->lastUpdate = prevRow(d_ptr->lastUpdate);
}

void RRA::setRowCount(uint rowCount)
{
    d_ptr->rowCount = rowCount;
}

void RRA::setXff(qreal xff)
{
    d_ptr->xff = xff;
}

uint RRA::prevRow(uint stamp) const
{
    return stamp - (stamp % (d_ptr->pdpPerRow * d_ptr->pdpStep));
}

uint RRA::nextRow(uint stamp) const
{
    return prevRow(stamp + d_ptr->pdpPerRow * d_ptr->pdpStep);
}

uint RRA::first() const
{
    return d_ptr->first();
}

uint RRA::last() const
{
    return d_ptr->lastUpdate;
}

bool RRA::isInRange(const QDateTime &first, const QDateTime &last) const
{
    return (d_ptr->first() <= first.toTime_t()) &&
            (d_ptr->lastUpdate >= last.toTime_t());
}

uint RRA::coveredRange(const QDateTime &first, const QDateTime &last) const
{
    uint start = d_ptr->first();
    uint end = d_ptr->lastUpdate;

    if (start < first.toTime_t())
        start = first.toTime_t();
    if (end > last.toTime_t())
        end = last.toTime_t();

    return end - start;
}

QString RRA::toString(RRA::ConsFunc f)
{
    switch (f)
    {
    case RRA::AVERAGE:
        return "AVERAGE";
    case RRA::LAST:
        return "LAST";
    case RRA::MAX:
        return "MAX";
    case RRA::MIN:
        return "MIN";
    }
    return QString();
}

bool RRA::fromString(const QString &str, RRA::ConsFunc &f)
{
    f = s_map.value(str.toUpper(), static_cast<ConsFunc>(-1));
    return f != static_cast<ConsFunc>(-1);
}
