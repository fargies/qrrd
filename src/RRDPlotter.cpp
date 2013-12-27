
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

        if (funcs.isEmpty())
            func = static_cast<RRA::ConsFunc>(-1);
        else
            func = *funcs.constBegin();

        paths.resize(rrd.ds().size());
    }

    RRDFile rrd;
    RRA::ConsFunc func;
    QVector<QPainterPath> paths;
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

void RRDPlotter::draw(
        const QDateTime &start,
        const QDateTime &end,
        QPainter &painter)
{
    if (!isValid() || !start.isValid() || !end.isValid())
        return;

    // FIXME: handle resolution ?
    if (d_ptr->rrd.function() != d_ptr->func
            || d_ptr->rrd.first() > start
            || d_ptr->rrd.last() < end)
        d_ptr->rrd.fetch(d_ptr->func, start, end);

    //FIXME: display all the graphs ?
    uint step(d_ptr->rrd.step());
    uint cur = 0;
    const QByteArray data(d_ptr->rrd.items());
    QDataStream stream(data);
    stream.setByteOrder(QDataStream::LittleEndian);

    for (int i = 0; i < d_ptr->paths.size(); ++i)
    {
        rrd_value_t v;
        stream >> v;

        d_ptr->paths[i] = QPainterPath(QPointF(cur, v));
    }

    while (!stream.atEnd())
    {
        for (int i = 0; i < d_ptr->paths.size(); ++i)
        {
            rrd_value_t v;
            stream >> v;

            d_ptr->paths[i].lineTo(cur, v);
        }
        cur += step;
    }

    for (int i = 0; i < d_ptr->paths.size(); ++i)
    {
        painter.setPen(Qt::black);
        uint delta = end.toTime_t() - start.toTime_t();
        if (delta == 0)
            continue;
        painter.scale((qreal) painter.window().width() / delta, 1);
        painter.setBrush(QBrush(Qt::yellow));
        painter.drawPath(d_ptr->paths[i]);
    }
}
