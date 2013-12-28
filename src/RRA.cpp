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
        function(RRA::INVALID),
        pdpStep(parent.pdpStep()),
        lastUpdate(parent.lastUpdate())
    {}

    bool canCompute() const
    {
        return (pdpPerRow && rowCount && pdpStep);
    }

    uint delta() const
    {
        return (pdpPerRow * (rowCount - 1) * pdpStep);
    }

    QDateTime computeLast()
    {
        return QDateTime::fromTime_t(firstUpdate.toTime_t() +
                delta());
    }

    QDateTime computeFirst()
    {
        return QDateTime::fromTime_t(lastUpdate.toTime_t() -
                delta());
    }

    RRA::ConsFunc function;
    uint pdpPerRow;
    uint rowCount;
    qreal xff;
    uint pdpStep;
    QDateTime lastUpdate;
    QDateTime firstUpdate;
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
    return d_ptr->function != INVALID;
}

RRA::ConsFunc RRA::function() const
{
    return d_ptr ? d_ptr->function : RRA::INVALID;
}

uint RRA::pdpPerRow() const
{
    return d_ptr ? d_ptr->pdpPerRow : 0;
}

uint RRA::rowCount() const
{
    return d_ptr ? d_ptr->rowCount : 0;
}

qreal RRA::xff() const
{
    return d_ptr ? d_ptr->xff : 0.0;
}

uint RRA::pdpStep() const
{
    return d_ptr ? d_ptr->pdpStep : 0;
}

uint RRA::step() const
{
    return d_ptr ? d_ptr->pdpStep * d_ptr->pdpPerRow : 0;
}

void RRA::setFunction(RRA::ConsFunc func)
{
    if (d_ptr)
        d_ptr->function = func;
}

void RRA::setPdpPerRow(uint pdpPerRow)
{
    if (!d_ptr)
        return;
    d_ptr->pdpPerRow = pdpPerRow;
    if (d_ptr->lastUpdate.isValid())
    {
        d_ptr->lastUpdate = prevRow(d_ptr->lastUpdate);
        d_ptr->firstUpdate = d_ptr->computeFirst();
    }
}

void RRA::setRowCount(uint rowCount)
{
    if (!d_ptr)
        return;
    d_ptr->rowCount = rowCount;
}

void RRA::setXff(qreal xff)
{
    if (!d_ptr)
        return;
    d_ptr->xff = xff;
}

void RRA::setFirst(const QDateTime &first)
{
    if (!d_ptr)
        return;
    d_ptr->firstUpdate = first;
    if (d_ptr->canCompute())
        d_ptr->lastUpdate = d_ptr->computeLast();
}

void RRA::setLast(const QDateTime &last)
{
    if (!d_ptr)
        return;
    d_ptr->lastUpdate = last;
    if (d_ptr->canCompute())
        d_ptr->firstUpdate = d_ptr->computeFirst();
}

QDateTime RRA::prevRow(const QDateTime &date) const
{
    if (!d_ptr || !d_ptr->pdpPerRow || !d_ptr->pdpStep)
        return QDateTime();

    uint stamp = date.toTime_t();
    return QDateTime::fromTime_t(stamp -
            (stamp % (d_ptr->pdpPerRow * d_ptr->pdpStep)));
}

QDateTime RRA::nextRow(const QDateTime &date) const
{
    if (!d_ptr || !d_ptr->pdpPerRow || !d_ptr->pdpStep)
        return QDateTime();

    uint stamp = date.toTime_t() + d_ptr->pdpPerRow * d_ptr->pdpStep;
    return QDateTime::fromTime_t(stamp -
            (stamp % (d_ptr->pdpPerRow * d_ptr->pdpStep)));
}

QDateTime RRA::first() const
{
    if (!d_ptr)
        return QDateTime();

    return d_ptr->firstUpdate;
}

QDateTime RRA::last() const
{
    if (!d_ptr)
        return QDateTime();

    return d_ptr->lastUpdate;
}

bool RRA::isInRange(const QDateTime &first, const QDateTime &last) const
{
    if (!d_ptr || !d_ptr->firstUpdate.isValid() ||
            !d_ptr->lastUpdate.isValid())
        return false;

    return (d_ptr->firstUpdate <= first) &&
            (d_ptr->lastUpdate >= last);
}

uint RRA::coveredRange(const QDateTime &first, const QDateTime &last) const
{
    if (!d_ptr)
        return 0;

    QDateTime start = d_ptr->firstUpdate;
    QDateTime end = d_ptr->lastUpdate;

    if (start < first)
        start = first;
    if (end > last)
        end = last;

    return start.secsTo(end);
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
    f = s_map.value(str.toUpper(), RRA::INVALID);
    return f != RRA::INVALID;
}

