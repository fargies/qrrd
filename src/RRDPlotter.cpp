
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
    if (!isValid() || !start.isValid() || !end.isValid() ||
            start == end)
        return;

    RRA current(d_ptr->rrd.current());
    // FIXME: handle resolution ?
    if (!current.isValid()
            || current.function() != d_ptr->func
            || !current.isInRange(start, end))
    {
        d_ptr->rrd.fetch(d_ptr->func, start, end);
        current = d_ptr->rrd.current();
    }

    //FIXME: display all the graphs ?
    uint step(current.step());
    uint cur = 0;
    const QByteArray data(d_ptr->rrd.items());
    QDataStream stream(data);
    stream.setByteOrder(QDataStream::LittleEndian);

    for (int i = 0; i < d_ptr->paths.size(); ++i)
    {
        d_ptr->paths[i].path = QPainterPath();
        d_ptr->paths[i].path.moveTo(cur, 0);
    }

    while (!stream.atEnd())
    {
        for (int i = 0; i < d_ptr->paths.size(); ++i)
        {
            rrd_value_t v;
            stream >> v;

            d_ptr->paths[i].path.lineTo(cur, -v);
        }
        cur += step;
    }

    uint delta = end.toTime_t() - start.toTime_t();
    uint startTrans = current.first().toTime_t() - start.toTime_t();

    for (int i = 0; i < d_ptr->paths.size(); ++i)
    {
        d_ptr->paths[i].path.lineTo(cur, 0);
        if (startTrans)
            d_ptr->paths[i].path.translate(startTrans, 0);

/*        painter.setPen(Qt::black);

        painter.scale((qreal) painter.window().width() / delta, 1);
        painter.setBrush(QBrush(Qt::yellow));
        painter.drawPath(d_ptr->paths[i]);*/
    }
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

