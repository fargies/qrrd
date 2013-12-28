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
** RRDFile.cpp
**
**        Created on: Dec 21, 2013
**   Original Author: Fargier Sylvain <fargier.sylvain@free.fr>
**
*/

#include <QHash>
#include <QDebug>

#include <rrd.h>

#include "RRDFile.hpp"

struct RRDFilePrivate : public QSharedData
{
    RRDFilePrivate(const QString &fileName) :
        fileName(fileName),
        version(),
        items(0)
    {}

    ~RRDFilePrivate()
    {
        resetData();
    }

    const QString fileName;
    QString version;
    QDateTime lastUpdate;
    uint pdpStep;

    QStringList dsNames;
    QHash<QString, DataSource> ds;

    QSet<RRA::ConsFunc> consFuncs;
    QList<RRA> rra;
    QHash<RRA::ConsFunc, QDateTime> oldest;
    QHash<RRA::ConsFunc, QDateTime> latest;

    /* fetched data related */
    RRA current;
    rrd_value_t *items;

    void resetData();

    rrd_info_t *parseDS(rrd_info_t *ptr);
    rrd_info_t *parseRRA(rrd_info_t *ptr);
    bool parse(rrd_info_t *ptr);

};

rrd_info_t *RRDFilePrivate::parseDS(rrd_info_t *ptr)
{
    this->ds.clear();
    this->dsNames.clear();

    while (ptr && qstrncmp(ptr->key, "ds[", 3) == 0)
    {
        QString key(&ptr->key[3]);
        int idx = key.indexOf(']');

        if (idx < 0)
            return ptr;

        QString dsName(key.left(idx));

        if (key[idx + 1] != '.')
            return ptr;

        QString item(key.mid(idx + 2));

        if (item == "index")
        {
            if (ptr->type != RD_I_CNT)
                return ptr;
            this->ds[dsName].setIndex(ptr->value.u_cnt);
        }
        else if (item == "type")
        {
            DataSource::Type type;
            if (ptr->type != RD_I_STR
                    || !DataSource::fromString(QString(ptr->value.u_str), type))
                return ptr;
            this->ds[dsName].setType(type);
        }
        else if (item == "minimal_heartbeat")
        {
            if (ptr->type != RD_I_CNT)
                return ptr;
            this->ds[dsName].setMinHB(ptr->value.u_cnt);
        }
        else if (item == "min")
        {
            if (ptr->type != RD_I_VAL)
                return ptr;
            this->ds[dsName].setMin(ptr->value.u_val);
        }
        else if (item == "max")
        {
            if (ptr->type != RD_I_VAL)
                return ptr;
            this->ds[dsName].setMax(ptr->value.u_val);
        }

        ptr = ptr->next;
    }

    QHash<QString, DataSource>::iterator it = this->ds.begin();
    for (; it != this->ds.end(); ++it)
    {
        it.value().setName(it.key());
        this->dsNames.insert(it.value().index(), it.key());
    }

    return ptr;
}

rrd_info_t *RRDFilePrivate::parseRRA(rrd_info_t *ptr)
{
    this->rra.clear();
    this->oldest.clear();
    this->latest.clear();
    this->consFuncs.clear();

    int rraIdx = -1;
    QString rraName;
    while (ptr && qstrncmp(ptr->key, "rra[", 4) == 0)
    {
        QString key(&ptr->key[4]);
        int idx = key.indexOf(']');

        if (idx < 0)
            return ptr;

        QString newName(key.left(idx));
        if (rraIdx < 0 || newName != rraName)
        {
            ++rraIdx;
            this->rra.push_back(RRA(RRDFile(*this)));
            rraName = newName;
        }

        if (key[idx + 1] != '.')
            return ptr;

        QString item(key.mid(idx + 2));

        if (item == "cf")
        {
            RRA::ConsFunc f;
            if (ptr->type != RD_I_STR || !RRA::fromString(ptr->value.u_str, f))
                return ptr;
            this->rra[rraIdx].setFunction(f);
        }
        else if (item == "rows")
        {
            if (ptr->type != RD_I_CNT)
                return ptr;
            this->rra[rraIdx].setRowCount(ptr->value.u_cnt);
        }
        else if (item == "pdp_per_row")
        {
            if (ptr->type != RD_I_CNT)
                return ptr;
            this->rra[rraIdx].setPdpPerRow(ptr->value.u_cnt);
        }
        else if (item == "xff")
        {
            if (ptr->type != RD_I_VAL)
                return ptr;
            this->rra[rraIdx].setXff(ptr->value.u_val);
        }

        ptr = ptr->next;
    }

    QList<RRA>::const_iterator it = rra.constBegin();
    for (; it != rra.constEnd(); ++it)
    {
        consFuncs.insert(it->function());

        const QDateTime last = it->last();
        const QDateTime first = it->first();
        QHash<RRA::ConsFunc, QDateTime>::iterator itOld =
                oldest.find(it->function());
        if (itOld == oldest.end())
            oldest.insert(it->function(), first);
        else if (first < itOld.value())
            itOld.value() = first;

        itOld = latest.find(it->function());
        if (itOld == latest.end())
            latest.insert(it->function(), last);
        else if (last > itOld.value())
            itOld.value() = last;
    }

    return ptr;
}

bool RRDFilePrivate::parse(rrd_info_t *ptr)
{
    resetData();

    if (!ptr || qstrcmp(ptr->key, "filename") != 0)
        return false;

    ptr = ptr->next;
    if (!ptr
            || qstrcmp(ptr->key, "rrd_version") != 0
            || ptr->type != RD_I_STR)
        return false;
    QString version(ptr->value.u_str);

    ptr = ptr->next;
    if (!ptr
            || qstrcmp(ptr->key, "step") != 0
            || ptr->type != RD_I_CNT)
        return false;
    this->pdpStep = ptr->value.u_cnt;

    ptr = ptr->next;
    if (!ptr
            || qstrcmp(ptr->key, "last_update") != 0
            || ptr->type != RD_I_CNT)
        return false;
    this->lastUpdate = QDateTime::fromTime_t(ptr->value.u_cnt);

    ptr = ptr->next;
    if (!ptr)
        return false;

    ptr = parseDS(ptr->next);
    if (!ptr)
        return false;

    if (parseRRA(ptr) != NULL)
        return false;

    this->version = version;
    return true;
}

void RRDFilePrivate::resetData()
{
    current.setFirst(QDateTime());
    current.setRowCount(0);
    current.setFunction(RRA::INVALID);
    current.setPdpPerRow(0);

    if (items)
    {
        free(items);
        items = 0;
    }
}

RRDFile::RRDFile() :
    d_ptr()
{}

RRDFile::RRDFile(RRDFilePrivate &priv) :
    d_ptr(&priv)
{}

RRDFile::RRDFile(const QString &fileName) :
    d_ptr(new RRDFilePrivate(fileName))
{
    rrd_info_t *info = rrd_info_r(d_ptr->fileName.toLocal8Bit().data());
    if (!info)
        return;

    if (d_ptr->parse(info))
    {
        d_ptr->current = RRA(*this);
        if (!d_ptr->consFuncs.isEmpty())
            d_ptr->current.setFunction(*d_ptr->consFuncs.begin());
    }
    else
        d_ptr->current = RRA(*this);

    rrd_info_free(info);
}

RRDFile::RRDFile(const RRDFile &other) :
    d_ptr(other.d_ptr)
{}

RRDFile::~RRDFile()
{}

RRDFile &RRDFile::operator =(const RRDFile &other)
{
    if (this != &other)
        d_ptr = other.d_ptr;
    return *this;
}

bool RRDFile::isValid() const
{
    return d_ptr && !d_ptr->version.isEmpty();
}

QString RRDFile::version() const
{
    return d_ptr->version;
}

QString RRDFile::fileName() const
{
    return d_ptr->fileName;
}

QDateTime RRDFile::lastUpdate() const
{
    return d_ptr->lastUpdate;
}

QStringList RRDFile::dsNames() const
{
    return d_ptr->dsNames;
}

QString RRDFile::dsName(uint index) const
{
    return d_ptr->dsNames.value(index);
}

QList<DataSource> RRDFile::ds() const
{
    return d_ptr->ds.values();
}

uint RRDFile::pdpStep() const
{
    return d_ptr->pdpStep;
}

QSet<RRA::ConsFunc> RRDFile::consFuncs() const
{
    return d_ptr->consFuncs;
}

bool RRDFile::hasFunc(RRA::ConsFunc func) const
{
    return d_ptr->consFuncs.contains(func);
}

QList<RRA> RRDFile::rra() const
{
    return d_ptr->rra;
}

QDateTime RRDFile::lastUpdate(RRA::ConsFunc function) const
{
    return d_ptr->latest.value(function);
}

QDateTime RRDFile::firstUpdate(RRA::ConsFunc function) const
{
    return d_ptr->oldest.value(function);
}

RRA RRDFile::bestMatch(RRA::ConsFunc function, const QDateTime &first, const QDateTime &last)
{
    RRA ret;

    QList<RRA>::const_iterator it = d_ptr->rra.constBegin();
    for (; it != d_ptr->rra.constEnd(); ++it)
    {
        if (it->function() != function)
            continue;
        else if (it->isInRange(first, last))
        {
            /* good candidate */
            if (!ret.isValid() || !ret.isInRange(first, last) ||
                    (it->pdpPerRow() < ret.pdpPerRow()))
                ret = *it;
        }
        else if (!ret.isValid())
            ret = *it;
        else if (ret.coveredRange(first, last) < it->coveredRange(first, last))
            ret = *it;
    }
    return ret;
}

bool RRDFile::fetch(
        RRA::ConsFunc func,
        const QDateTime &start,
        const QDateTime &end,
        uint resolution)
{
    QByteArray startstr(QByteArray::number(start.toTime_t() - 1));
    QByteArray endstr(QByteArray::number(end.toTime_t() - 1)); // librrd is weird
    QByteArray resstr(QByteArray::number(resolution));
    QByteArray funcstr(RRA::toString(func).toLocal8Bit());

    clear();

    int idx = 0;
    QVector<const char *> argv(10);
    argv[idx++] = "fetch";
    argv[idx++] = d_ptr->fileName.toLocal8Bit().constData();
    argv[idx++] = funcstr.constData();
    argv[idx++] = "-s";
    argv[idx++] = startstr.constData();
    argv[idx++] = "-e";
    argv[idx++] = endstr.constData();
    if (resolution != 0)
    {
        argv[idx++] = "-r";
        argv[idx++] = resstr.constData();
    }
    argv[idx] = 0;

    time_t realStart, realEnd;
    unsigned long step, dsCount;
    char **dsNames;
    if (rrd_fetch(idx, const_cast<char **>(argv.data()),
                  &realStart, &realEnd, &step, &dsCount, &dsNames, &d_ptr->items) != 0)
    {
        qWarning() << "[RRDFile] failed to fetch data from " << d_ptr->fileName;
        return false;
    }

    for (int i = 0; i < dsCount; ++i)
        free(dsNames[i]);
    free(dsNames);

    if (!step)
    {
        qWarning() << "[RRDFile] corrupted data retrieved " << "(step == 0)";
    }
    else if (dsCount != d_ptr->ds.size())
    {
        qWarning() << "[RRDFile] corrupted data retrieved " << "(dsCount " <<
                      dsCount << " != " << d_ptr->ds.size();
    }
    else
    {
        if (d_ptr->current.pdpStep() == 0)
            d_ptr->current = RRA(*this);
        d_ptr->current.setRowCount((realEnd - realStart) / step);
        d_ptr->current.setFunction(func);
        d_ptr->current.setPdpPerRow(step / d_ptr->pdpStep);
        d_ptr->current.setFirst(QDateTime::fromTime_t(realStart + step));
    }

    return d_ptr->current.rowCount() != 0;
}

RRA RRDFile::current() const
{
    return d_ptr->current;
}

uint RRDFile::itemCount() const
{
    return d_ptr->current.rowCount();
}

QByteArray RRDFile::items() const
{
    QByteArray ret;

    ret.setRawData(reinterpret_cast<const char *>(d_ptr->items),
                   d_ptr->current.rowCount() * d_ptr->ds.size() *
                   sizeof (rrd_value_t));

    return ret;
}

void RRDFile::clear()
{
    if (d_ptr->items)
        d_ptr->resetData();
}

