
#include <QSharedData>
#include <QDebug>

#include <rrd.h>
#include <math.h>

#include "RRDPlotter.hpp"
#include "RRDFile.hpp"

struct RRDPlotterPrivate : public QSharedData
{
    RRDPlotterPrivate(const RRDFile &r) :
        rrd(r)
    {
        rrd.clear();
        const QSet<RRA::ConsFunc> funcs(rrd.consFuncs());

        func = funcs.isEmpty() ? RRA::INVALID : *funcs.constBegin();

        paths.resize(rrd.ds().size());
    }

    struct Paths
    {
        QPainterPath path;
        QBrush brush;
        QPen pen;
    };

    RRDFile rrd;
    RRA::ConsFunc func;
    QVector<Paths> paths;
};

RRDPlotter::RRDPlotter() :
    d_ptr()
{}

RRDPlotter::RRDPlotter(const RRDFile &rrd) :
    d_ptr(new RRDPlotterPrivate(rrd))
{
}

RRDPlotter::RRDPlotter(const RRDPlotter &other) :
    d_ptr(other.d_ptr)
{}

RRDPlotter::~RRDPlotter()
{}

RRDPlotter &RRDPlotter::operator =(const RRDPlotter &other)
{
    if (this != &other)
        d_ptr = other.d_ptr;
    return *this;
}

bool RRDPlotter::isValid() const
{
    return d_ptr && d_ptr->func != static_cast<RRA::ConsFunc>(-1);
}

RRA::ConsFunc RRDPlotter::consFunc() const
{
    return d_ptr->func;
}

RRDFile RRDPlotter::rrd() const
{
    return d_ptr->rrd;
}

QStringList RRDPlotter::dsNames() const
{
    return this->rrd().dsNames();
}

const RRDFile *RRDPlotter::operator -> () const
{
    return &d_ptr->rrd;
}

RRDFile *RRDPlotter::operator -> ()
{
    return &d_ptr->rrd;
}

bool RRDPlotter::setConsFunc(RRA::ConsFunc func)
{
    if (func == d_ptr->func)
        return true;
    else if (!d_ptr->rrd.hasFunc(func))
        return false;
    else
    {
        d_ptr->func = func;
        return true;
    }
}

void RRDPlotter::prepare(
        const QDateTime &start,
        const QDateTime &end)
{
    RRDPlotterPrivate *d = d_ptr.data();
    if (!isValid() || !start.isValid() || !end.isValid() ||
            start == end)
        return;

    RRA rra(d->rrd.current());
    // FIXME: handle resolution ?
    if (!rra.isValid()
            || rra.function() != d->func
            || !rra.isInRange(start, end))
    {
        d->rrd.fetch(d->func, start, end);
        rra = d->rrd.current();
    }

    uint step(rra.step());
    uint cur = rra.first().toTime_t();
    const QByteArray data(d->rrd.items());
    QDataStream stream(data);
    stream.setByteOrder(QDataStream::LittleEndian);

    for (int i = 0; i < d->paths.size(); ++i)
    {
        d->paths[i].path = QPainterPath();
        d->paths[i].path.moveTo(cur, 0);
    }

    while (!stream.atEnd())
    {
        for (int i = 0; i < d->paths.size(); ++i)
        {
            rrd_value_t v;
            stream >> v;

            d->paths[i].path.lineTo(cur, -v);
        }
        cur += step;
    }

    for (int i = 0; i < d->paths.size(); ++i)
        d->paths[i].path.lineTo(cur, 0);
}

QPainterPath RRDPlotter::path(uint index) const
{
    return d_ptr->paths.value(index).path;
}

QBrush RRDPlotter::brush(uint index) const
{
    return d_ptr->paths.value(index).brush;
}

void RRDPlotter::setBrush(uint index, const QBrush &brush)
{
    if (index >= d_ptr->paths.size())
        return;
    d_ptr->paths[index].brush = brush;
}

QPen RRDPlotter::pen(uint index) const
{
    return d_ptr->paths.value(index).pen;
}

void RRDPlotter::setPen(uint index, const QPen &pen)
{
    if (index >= d_ptr->paths.size())
        return;
    d_ptr->paths[index].pen = pen;
}

QDateTime RRDPlotter::start() const
{
    if (!d_ptr || d_ptr->paths.isEmpty()
            || d_ptr->paths[0].path.isEmpty()
            || !d_ptr->rrd.current().isValid())
        return QDateTime();
    else
        return QDateTime::fromTime_t(d_ptr->rrd.current().first().toTime_t() +
                d_ptr->paths[0].path.elementAt(0).x);
}

QDateTime RRDPlotter::end() const
{
    if (!d_ptr || d_ptr->paths.isEmpty()
            || d_ptr->paths[0].path.isEmpty()
            || !d_ptr->rrd.current().isValid())
        return QDateTime();
    else
        return QDateTime::fromTime_t(d_ptr->rrd.current().last().toTime_t() +
                d_ptr->paths[0].path.elementAt(0).x);
}

