#include <QScrollBar>
#include <QDebug>
#include <QtCore/qmath.h>
/* FIXME: will only work on posix like systems (for log10 and fmod) */
#include <math.h>

#include "RRDGrid.hpp"
#include "RRDGraphItem.hpp"

#define GRID_MAX 100

class RRDGridPrivate
{
public:
    RRDGridPrivate(RRDWidget *rrd) :
        rrd(rrd)
    {}
    ~RRDGridPrivate()
    {
    }

    QList<QGraphicsLineItem *> horizontal;
    QList<QGraphicsLineItem *> vertical;
    RRDWidget *rrd;
    QRectF view; /* visible part of the graph in it's local coordinates */
    QPointF step;
};

typedef QList<QGraphicsLineItem *> LineList;

RRDGrid::RRDGrid(RRDWidget *w, QGraphicsItem *parent) :
    QGraphicsWidget(parent),
    d_ptr(new RRDGridPrivate(w))
{
    setGeometry(w->graphItem()->geometry());
    QGraphicsWidget *graph = w->graphItem()->pathGroup();

    connect(graph, SIGNAL(zoomChanged()),
            this, SLOT(onScaleChanged()));
    connect(graph, SIGNAL(xChanged()),
            this, SLOT(onHorizontalPosChanged()));
    connect(graph, SIGNAL(yChanged()),
            this, SLOT(onVerticalPosChanged()));
    connect(w->graphItem(), SIGNAL(geometryChanged()),
            this, SLOT(onGeometryChanged()));

    prepare();
}

RRDGrid::~RRDGrid()
{
}

/* FIXME implement timegrid
 *
 *    enum GridType
    {
        GRID_SEC,
        GRID_MIN,
        GRID_HOUR,
        GRID_DAY,
        GRID_MONTH,
        GRID_YEAR
    };
uint RRDWidget::gridNext(GridType type, uint step, uint pos)
{
    switch (type)
    {
    case GRID_DAY: step *= 24;
    case GRID_HOUR: step *= 60;
    case GRID_MIN: step *= 60;
    case GRID_SEC:
        return  pos - (pos % step) + step;
    case GRID_MONTH:
        {
            QDate curr(QDateTime::fromTime_t(pos).date());
            if (curr.day() != 1)
                curr = curr.addDays(- curr.day() + 1);
            curr = curr.addMonths(step);
            return QDateTime(curr).toTime_t();
        }
    }
}
*/
/*
void RRDWidget::updateGridSpacing()
{
    QPointF current = mapToScene(0, 0);

    uint secs = mapToScene(50, 0).x() - current.x();
    qDebug() << secs << " for 20 px";

    if (secs < 60) * less than a minute -> secs grid *
    {
        type = GRID_SEC;
    }
    else if (secs < 60 * 60)
    {
        secs /= 60;
        type = GRID_MIN;
    }
    else if (secs < 60 * 60 * 24)
    {
        secs /= 60 * 60;
        type = GRID_HOUR;
    }
    else if (secs < 60 * 60 * 24 * 5)
    {
        secs /= 60 * 60 * 24;
        type = GRID_DAY;
    }
    else
    {
        type = GRID_MONTH;
        return 1;
    }

    if (secs >= 5)
        return 5;
    else
        return (secs > 1) ? 2 : 1;
}*/

#define GRID_PSZ 20

QPointF RRDGrid::gridStep() const
{
    /* compute a GRID_PSZxGRID_PSZ rect in Graph coordinates */
    QRectF rect = pathGroup()->mapFromParent(0, 0, GRID_PSZ, GRID_PSZ).boundingRect();
    QPointF step;

    int exp = log10(rect.height());
    step.setY(qPow(10, exp));
    qreal y = rect.height() / step.y();
    if (y >= 5)
        step.setY(step.y() * 5);
    else if (y >= 2)
        step.setY(step.y() * 2);

    exp = log10(rect.width());
    step.setX(pow(10, exp));
    qreal x = rect.width() / step.x();
    if (x >= 5)
        step.setX(step.x() * 5);
    else if (x >= 2)
        step.setX(step.x() * 2);

    return step;
}

void RRDGrid::prepare()
{
    QRectF view = widget()->graphItem()->pathGroupBoundingRect();

    if (!view.isValid())// || view == d_ptr->view)
        return;

    QPointF step = gridStep();

    updateHorizontalGrid(step.y(), view);
    updateVerticalGrid(step.x(), view);
    d_ptr->step = step;
    d_ptr->view = view;
}


void RRDGrid::updateVerticalGrid(qreal step, const QRectF &view)
{
    RRDPathGroup *group = pathGroup();
    qreal x = view.left();
    x -= (fmod(x, step));

    if (d_ptr->vertical.isEmpty())
        addVerticalLine();

    d_ptr->vertical[0]->setX(group->mapToParent(x, 0).x());
    d_ptr->vertical[0]->setPen(QPen(Qt::green)); /* TODO: REMOVE */

    int idx = 1;
    for (x += step; x < view.right() && idx < GRID_MAX; x += step)
    {
        if (idx >= d_ptr->vertical.size())
            addVerticalLine();
        d_ptr->vertical[idx++]->setX(group->mapToParent(x, 0).x());
    }

    while (idx < d_ptr->vertical.size())
        delete d_ptr->vertical.takeLast();
}

void RRDGrid::updateHorizontalGrid(qreal step, const QRectF &view)
{
    RRDPathGroup *group = pathGroup();
    qreal y = view.top();
    y -= (fmod(y, step));

    if (d_ptr->horizontal.isEmpty())
        addHorizontalLine();

    d_ptr->horizontal[0]->setY(group->mapToParent(0, y).y());
    d_ptr->horizontal[0]->setPen(QPen(Qt::green)); /* TODO: REMOVE */

    int idx = 1;
    for (y += step; y < view.bottom() && idx < GRID_MAX; y += step)
    {
        if (idx >= d_ptr->horizontal.size())
            addHorizontalLine();
        d_ptr->horizontal[idx++]->setY(group->mapToParent(0, y).y());
    }

    while (idx < d_ptr->horizontal.size())
        delete d_ptr->horizontal.takeLast();
}

QGraphicsLineItem *RRDGrid::addHorizontalLine()
{
    QRectF gridBound(gridBoundingRect());
    QGraphicsItem *parent = d_ptr->horizontal.isEmpty() ?
                static_cast<QGraphicsItem*>(this) :
                static_cast<QGraphicsItem*>(d_ptr->horizontal[0]);
    d_ptr->horizontal.append(
                new QGraphicsLineItem(gridBound.left(), 0,
                                      gridBound.right(), 0,
                                      this,
                                      widget()->scene()));
    QGraphicsLineItem *item(d_ptr->horizontal.last());
    item->setPen(QPen(Qt::DotLine));
    return item;
}

QGraphicsLineItem *RRDGrid::addVerticalLine()
{
    QRectF gridBound(gridBoundingRect());
    QGraphicsItem *parent = d_ptr->vertical.isEmpty() ?
                static_cast<QGraphicsItem*>(this) :
                static_cast<QGraphicsItem*>(d_ptr->vertical[0]);
    d_ptr->vertical.append(
                new QGraphicsLineItem(0, gridBound.top(),
                                      0, gridBound.bottom(),
                                      this,
                                      widget()->scene()));
    QGraphicsLineItem *item(d_ptr->vertical.last());
    item->setPen(QPen(Qt::DotLine));
    return item;
}

QRectF RRDGrid::gridBoundingRect()
{
    return boundingRect().adjusted(-10, -10, 10, 10);
}

void RRDGrid::onVerticalGeometryChanged()
{
    QRectF bound(gridBoundingRect());


    LineList::iterator it = d_ptr->vertical.begin();
    for (; it != d_ptr->vertical.end(); ++it)
        (*it)->setLine(0, bound.top(), 0, bound.bottom());
}

void RRDGrid::onHorizontalGeometryChanged()
{
    QRectF bound(gridBoundingRect());

    LineList::iterator it = d_ptr->horizontal.begin();
    for (; it != d_ptr->horizontal.end(); ++it)
        (*it)->setLine(bound.left(), 0, bound.right(), 0);

}

void RRDGrid::onVerticalPosChanged()
{
    QRectF view = widget()->graphItem()->pathGroupBoundingRect();

    updateHorizontalGrid(d_ptr->step.y(), view);
}

void RRDGrid::onHorizontalPosChanged()
{
    QRectF view = widget()->graphItem()->pathGroupBoundingRect();

    updateVerticalGrid(d_ptr->step.x(), view);
}

void RRDGrid::onGeometryChanged()
{
    QRectF oldGeom(geometry());
    QRectF geom(widget()->graphItem()->geometry());

    setGeometry(geom);

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
