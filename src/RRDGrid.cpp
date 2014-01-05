/* TODO:
 *   - add small solid line at beggining/end of each line
 *   - add intermediate lines with only a small solid beggning/end line
 */

#include <QDebug>
#include <QtCore/qmath.h>
/* FIXME: will only work on posix like systems (for log10 and fmod) */
#include <math.h>

#include "RRDGrid.hpp"
#include "RRDGraphItem.hpp"

#define GRID_MAX 100
#define GRID_PSZ 40
#define GRID_LABEL_MARGIN 2

struct RRDGrid::Step
{
    uint timeStep; /* for x axis */
    RRDGrid::TimeStep timeType;
    qreal valueStep; /* for y axis */

    void timeStepRound();
};

class RRDGridPrivate
{
public:
    RRDGridPrivate(RRDWidget *rrd) :
        rrd(rrd)
    {}
    ~RRDGridPrivate()
    {
    }

    QList<RRDGridLine *> horizontal;
    QList<RRDGridLine *> vertical;
    RRDWidget *rrd;
    QRectF view; /* visible part of the graph in it's local coordinates */
    QGraphicsRectItem *border;
    QGraphicsSimpleTextItem *vertOrigin;
    QGraphicsSimpleTextItem *horizOrigin;
    RRDGrid::Step step;
};

typedef QList<RRDGridLine *> LineList;

RRDGrid::RRDGrid(RRDWidget *w, QGraphicsWidget *parent) :
    QGraphicsWidget(parent),
    d_ptr(new RRDGridPrivate(w))
{
    QGraphicsWidget *graph = w->graphItem()->pathGroup();

    d_ptr->border = new QGraphicsRectItem(this);
    d_ptr->vertOrigin = new QGraphicsSimpleTextItem(this);
    d_ptr->horizOrigin = new QGraphicsSimpleTextItem(this);

    connect(graph, SIGNAL(zoomChanged()),
            this, SLOT(onScaleChanged()));
    connect(graph, SIGNAL(xChanged()),
            this, SLOT(onHorizontalPosChanged()));
    connect(graph, SIGNAL(yChanged()),
            this, SLOT(onVerticalPosChanged()));
    connect(w->graphItem(), SIGNAL(geometryChanged()),
            this, SLOT(onGeometryChanged()));

    onGeometryChanged();
    QFontMetrics metrics(this->font());
    QSize marg(metrics.width("W") * 8,
               metrics.height() * 3);
    parent->setContentsMargins(marg.width(), marg.height(),
                               marg.width(), marg.height());
}

RRDGrid::~RRDGrid()
{
}

void RRDGrid::Step::timeStepRound()
{
    if (timeStep >= 30)
        timeStep = 30;
    else if (timeStep >= 15)
        timeStep = 15;
    else if (timeStep >= 10)
        timeStep = 10;
    else if (timeStep >= 5)
        timeStep = 5;
    else
        timeStep = (timeStep >= 2) ? 2 : 1;

    if (timeType == RRDGrid::MonthStep && timeStep > 2)
        timeStep = 6; /* but 1 2 6 is better for months */
}

void RRDGrid::gridStep(RRDGrid::Step &step) const
{
    /* compute a GRID_PSZxGRID_PSZ rect in Graph coordinates */
    QRectF rect = pathGroup()->mapFromParent(0, 0, GRID_PSZ, GRID_PSZ).boundingRect();

    int exp = log10(rect.height());
    step.valueStep = qPow(10, exp);
    qreal y = rect.height() / step.valueStep;
    if (y >= 5)
        step.valueStep *= 5;
    else if (y >= 2)
        step.valueStep *= 2;

    step.timeStep = rect.width();
    if (step.timeStep < 60) /* less than a minute -> secs grid */
        step.timeType = SecStep;
    else if (step.timeStep < 60 * 60)
    {
        step.timeStep /= 60;
        step.timeType = MinStep;
    }
    else if (step.timeStep < 60 * 60 * 24)
    {
        step.timeStep /= 60 * 60;
        step.timeType = HourStep;
    }
    else if (step.timeStep < 60 * 60 * 24 * 7)
    {
        /* will be bound to 1 2 5 by timeStepRound (< 7)*/
        step.timeStep /= 60 * 60 * 24;
        step.timeType = DayStep;
    }
    else if (step.timeStep < 60 * 60 * 24 * 30)
    {
        /* will be bound to 1 2 by timeStepRound (< 4) */
        step.timeStep /= 60 * 60 * 24 * 7;
        step.timeType = WeekStep;
    }
    else if (step.timeStep < 60 * 60 * 24 * 365)
    {
        step.timeType = MonthStep;
        /* will be bound to 1 2 6 by timeStepRound (< 4) */
        step.timeStep /= 60 * 60 * 24 * 30;
    }
    else
    {
        step.timeType = YearStep;
        step.timeStep /= 60 * 60 * 24 * 365;
    }
    step.timeStepRound();
}

QDateTime RRDGrid::timeGridMod(
        const QDateTime &dateTime,
        const RRDGrid::Step &step) const
{
    if (!dateTime.isValid())
        return QDateTime();

    QDate date(dateTime.date());
    QTime time(dateTime.time());

    switch (step.timeType)
    {
    case YearStep:
        return QDateTime(QDate(date.year() - date.year() % step.timeStep, 1, 1));
    case MonthStep:
        return QDateTime(QDate(date.year(), date.month()
                               - (date.month() - 1) % step.timeStep, 1));
    case WeekStep:
    {
        date = date.addDays(1 - date.dayOfWeek());
        date = date.addDays(-(date.weekNumber() - 1) % step.timeStep);
        return QDateTime(date);
    }
    case DayStep:
        date = date.addDays(-date.dayOfYear() % step.timeStep);
        return QDateTime(date);
    case HourStep:
        return QDateTime(date, QTime(time.hour()
                                     - time.hour() % step.timeStep, 0),
                         dateTime.timeSpec());
    case MinStep:
        return QDateTime(date, QTime(time.hour(), time.minute()
                                     - time.minute() % step.timeStep),
                         dateTime.timeSpec());
    case SecStep:
        return QDateTime(date, QTime(time.hour(), time.minute(),
                                     time.second()
                                     - time.second() % step.timeStep),
                         dateTime.timeSpec());
    default:
        return QDateTime();
    }
}

QDateTime RRDGrid::timeGridNext(
        const QDateTime &date,
        const RRDGrid::Step &step) const
{
    if (!date.isValid())
        return QDateTime();

    switch (step.timeType)
    {
    case SecStep:
        return date.addSecs(step.timeStep);
    case MinStep:
        return date.addSecs(step.timeStep * 60);
    case HourStep:
        return date.addSecs(step.timeStep * 60 * 60);
    case DayStep:
        return date.addDays(step.timeStep);
    case WeekStep:
        return date.addDays(step.timeStep * 7);
    case MonthStep:
        return date.addMonths(step.timeStep);
    case YearStep:
        return date.addYears(step.timeStep);
    default:
        return date;
    }
}

void RRDGrid::prepare()
{
    QRectF view = widget()->graphItem()->pathGroupBoundingRect();

    if (!view.isValid())// || view == d_ptr->view)
        return;

    Step step;
    gridStep(step);

    updateHorizontalGrid(step, view);
    updateVerticalGrid(step, view);
    d_ptr->step = step;
    d_ptr->view = view;
}

/*
 * TODO: here's one of the most expensive functions, we should detect wether
 * the move a forward/backward move and remove timeGridNext calls.
 */
void RRDGrid::updateVerticalGrid(const RRDGrid::Step &step, const QRectF &view)
{
    RRDPathGroup *group = pathGroup();
    QDateTime origin(QDateTime::fromTime_t((uint) view.left()));
    QDateTime x = timeGridMod(origin, step);

    int idx = 0;
    for (x = timeGridNext(x, step);
         x.toTime_t() < view.right() && idx < GRID_MAX;
         x = timeGridNext(x, step))
    {
        RRDGridLine *line = (idx >= d_ptr->vertical.size()) ?
            addVerticalLine() :
            d_ptr->vertical[idx];
        line->setX(group->mapToParent(x.toTime_t(), 0).x());
        ++idx;
    }

    while (idx < d_ptr->vertical.size())
        delete d_ptr->vertical.takeLast();

    setOriginLabel(Qt::Vertical, origin.toString(Qt::ISODate));
}

void RRDGrid::updateHorizontalGrid(
        const RRDGrid::Step &step,
        const QRectF &view)
{
    RRDPathGroup *group = pathGroup();
    qreal y = view.top();
    y -= (fmod(y, step.valueStep)) - step.valueStep;

    if (d_ptr->horizontal.isEmpty())
        addHorizontalLine();

    int idx = 0;
    for (; y < view.bottom() && idx < GRID_MAX; y += step.valueStep)
    {
        RRDGridLine *line = (idx >= d_ptr->horizontal.size()) ?
            addHorizontalLine() :
            d_ptr->horizontal[idx];
        line->setY(group->mapToParent(0, y).y());
        line->setLegendText(QString::number(-y));
        ++idx;
    }

    while (idx < d_ptr->horizontal.size())
        delete d_ptr->horizontal.takeLast();

    setOriginLabel(Qt::Horizontal, QString::number(-view.bottom()));
}

RRDGridLine *RRDGrid::addHorizontalLine()
{
    QRectF gridBound(boundingRect());
    d_ptr->horizontal.append(
                new RRDGridLine(Qt::Horizontal,
                                gridBound.left(), gridBound.right(),
                                this, widget()->scene()));
    return d_ptr->horizontal.last();
}

void RRDGrid::setOriginLabel(Qt::Orientation orientation, const QString &label)
{
    switch (orientation)
    {
    case Qt::Vertical:
        d_ptr->vertOrigin->setText(label); break;
    case Qt::Horizontal:
        d_ptr->horizOrigin->setText(label); break;
    }
    updateOriginLabelPos();
}

void RRDGrid::updateOriginLabelPos()
{
    QRectF bound(d_ptr->horizOrigin->boundingRect());
    d_ptr->horizOrigin->setPos(-bound.width() - GRID_LABEL_MARGIN, geometry().height() -
                               bound.center().y());

    bound = d_ptr->vertOrigin->boundingRect();
    d_ptr->vertOrigin->setPos(-bound.center().x(), geometry().height() +
                              GRID_LABEL_MARGIN);
}

RRDGridLine *RRDGrid::addVerticalLine()
{
    QRectF gridBound(boundingRect());
    d_ptr->vertical.append(
                new RRDGridLine(Qt::Vertical,
                                gridBound.top(), gridBound.bottom(),
                                this, widget()->scene()));
    return d_ptr->vertical.last();
}

void RRDGrid::onVerticalGeometryChanged()
{
    QRectF bound(boundingRect());

    LineList::iterator it = d_ptr->vertical.begin();
    for (; it != d_ptr->vertical.end(); ++it)
        (*it)->setLine(0, bound.top(), 0, bound.bottom());

    updateOriginLabelPos();
}

void RRDGrid::onHorizontalGeometryChanged()
{
    QRectF bound(boundingRect());

    LineList::iterator it = d_ptr->horizontal.begin();
    for (; it != d_ptr->horizontal.end(); ++it)
        (*it)->setLine(bound.left(), 0, bound.right(), 0);
}

void RRDGrid::onVerticalPosChanged()
{
    QRectF view = widget()->graphItem()->pathGroupBoundingRect();

    updateHorizontalGrid(d_ptr->step, view);
}

void RRDGrid::onHorizontalPosChanged()
{
    QRectF view = widget()->graphItem()->pathGroupBoundingRect();

    updateVerticalGrid(d_ptr->step, view);
}

void RRDGrid::onGeometryChanged()
{
    QRectF oldGeom(geometry());
    QRectF geom(widget()->graphItem()->geometry());

    setGeometry(geom);
    d_ptr->border->setRect(0, 0, geom.width(), geom.height());

    if (oldGeom.top() != geom.top() ||
            oldGeom.bottom() != geom.bottom())
        onVerticalGeometryChanged();

    if (oldGeom.left() != geom.left() ||
            oldGeom.right() != geom.right())
        onHorizontalGeometryChanged();

    prepare();
}

void RRDGrid::onScaleChanged()
{
    prepare();
}

RRDWidget *RRDGrid::widget() const
{
    return d_ptr->rrd;
}

RRDPathGroup *RRDGrid::pathGroup() const
{
    return widget()->graphItem()->pathGroup();
}

RRDGridLine::RRDGridLine(
        Qt::Orientation orientation,
        qreal p1, qreal p2,
        QGraphicsItem *parent,
        QGraphicsScene *scene) :
    QGraphicsLineItem(parent, scene),
    m_orientation(orientation)
{
    if (m_orientation == Qt::Vertical)
        setLine(0, p1, 0, p2);
    else
        setLine(p1, 0, p2, 0);
    setPen(QPen(Qt::DotLine));
    m_legend = new QGraphicsSimpleTextItem(this);
}

void RRDGridLine::setLegendText(const QString &text)
{
    m_legend->setText(text);
    QRectF bound(m_legend->boundingRect());
    if (m_orientation == Qt::Vertical)
        m_legend->setPos(boundingRect().bottomRight() -
                         QPointF(bound.center().x(), 2));
    else
        m_legend->setPos(-bound.right() - 2, -bound.center().y());
}
