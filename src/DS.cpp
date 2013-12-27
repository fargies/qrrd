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
** DS.cpp
**
**        Created on: Dec 22, 2013
**   Original Author: Fargier Sylvain <fargier.sylvain@free.fr>
**
*/

#include <QHash>
#include "DS.hpp"

struct DSPrivate : public QSharedData
{
    DSPrivate() :
        QSharedData(),
        index(-1)
    {}

    QString name;
    uint index;
    DataSource::Type type;
    uint minHB;
    double max;
    double min;
};

static QHash<QString, DataSource::Type> mapInit()
{
    QHash<QString, DataSource::Type> map;
    map["ABSOLUTE"] = DataSource::ABSOLUTE;
    map["CDEF"] = DataSource::CDEF;
    map["COUNTER"] = DataSource::COUNTER;
    map["DERIVE"] = DataSource::DERIVE;
    map["GAUGE"] = DataSource::GAUGE;
    return map;
}

static QHash<QString, DataSource::Type> s_map = mapInit();

DataSource::DataSource() :
    d_ptr(new DSPrivate)
{
}

DataSource::~DataSource()
{
}

DataSource::DataSource(const DataSource &other) :
    d_ptr(other.d_ptr)
{
}

DataSource &DataSource::operator =(const DataSource &other)
{
    if (this != &other)
        this->d_ptr = other.d_ptr;
    return *this;
}

bool DataSource::isValid() const
{
    return !d_ptr->name.isEmpty()
            && (!d_ptr->index < 0);
}

void DataSource::setName(const QString &name)
{
    d_ptr->name = name;
}

void DataSource::setIndex(uint idx)
{
    d_ptr->index = idx;
}

void DataSource::setType(DataSource::Type type)
{
    d_ptr->type = type;
}

void DataSource::setMinHB(uint count)
{
    d_ptr->minHB = count;
}

void DataSource::setMin(double min)
{
    d_ptr->min = min;
}

void DataSource::setMax(double max)
{
    d_ptr->max = max;
}

uint DataSource::index() const
{
    return d_ptr->index;
}

DataSource::Type DataSource::type() const
{
    return d_ptr->type;
}

uint DataSource::minHB() const
{
    return d_ptr->minHB;
}

double DataSource::min() const
{
    return d_ptr->min;
}

double DataSource::max() const
{
    return d_ptr->max;
}

QString DataSource::name() const
{
    return d_ptr->name;
}

QString DataSource::toString(DataSource::Type type)
{
    switch (type)
    {
    case DataSource::ABSOLUTE:
        return "ABSOLUTE";
    case DataSource::CDEF:
        return "CDEF";
    case DataSource::COUNTER:
        return "COUNTER";
    case DataSource::DERIVE:
        return "DERIVE";
    case DataSource::GAUGE:
        return "GAUGE";
    }
    return QString();
}

bool DataSource::fromString(const QString &str, DataSource::Type &type)
{
    type = s_map.value(str.toUpper(), static_cast<Type>(-1));
    return type != static_cast<Type>(-1);
}
