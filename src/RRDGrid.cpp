#include <QScrollBar>
#include <QDebug>
#include <QtCore/qmath.h>
/* FIXME: will only work on posix like systems (for log10 and fmod) */
#include <math.h>

#include "RRDGrid.hpp"

class RRDGridPrivate
{
public:
    RRDGridPrivate()
    {}
    ~RRDGridPrivate()
    {
        if (!horizontal.isEmpty())
            delete horizontal[0];
        if (!vertical.isEmpty())
            delete vertical[0];
    }

    QList<QGraphicsLineItem *> horizontal;
    QList<QGraphicsLineItem *> vertical;
    QRectF maxBound;
    QRectF view;
    QPointF step;
};

typedef QList<QGraphicsLineItem *> LineList;

RRDGrid::RRDGrid(RRDWidget *parent) :
    QObject(parent),
    d_ptr(new RRDGridPrivate)
{
    connect(parent->horizontalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(horizontalPosChanged()));
    connect(parent->verticalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(verticalPosChanged()));
    connect(parent->horizontalScrollBar(), SIGNAL(rangeChanged(int,int)),
            this, SLOT(horizontalRangeChanged()));
    connect(parent->verticalScrollBar(), SIGNAL(rangeChanged(int,int)),
            this, SLOT(verticalRangeChanged()));

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

QPointF RRDGrid::gridStep(const QRectF &view) const
{
    QRectF rect = QRectF(view.topLeft(), widget()->mapToScene(GRID_PSZ, GRID_PSZ)).normalized();
    QPointF step;
    qDebug() << rect.height() << log10(rect.height());
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
    qDebug() << step;
    return step;
}

QRectF RRDGrid::sceneView() const
{
    RRDWidget *w = widget();
    return QRectF(w->mapToScene(0, 0),
                  w->mapToScene(w->viewport()->rect().bottomRight())).normalized();
}

void RRDGrid::prepare()
{
    QRectF view = sceneView();

    if (view == d_ptr->view)
        return;

    QPointF step = gridStep(view);

    updateHorizontalGrid(step.y(), view);
    updateVerticalGrid(step.x(), view);
    d_ptr->step = step;
    d_ptr->view = view;
}


void RRDGrid::updateVerticalGrid(qreal step, const QRectF &view)
{
    qreal x = view.left();
    x -= (fmod(x, step));

    if (d_ptr->vertical.isEmpty())
        d_ptr->vertical.append(
                    new QGraphicsLineItem(0, d_ptr->maxBound.top(),
                                          0, d_ptr->maxBound.bottom(), 0,
                                          widget()->scene()));
    d_ptr->vertical[0]->setX(x);
    d_ptr->vertical[0]->setPen(QPen(Qt::green)); /* TODO: REMOVE */

    x = step;
    int idx = 1;
    for (; x < view.width(); x += step)
    {
        if (idx >= d_ptr->vertical.size())
            addVerticalLine();
        d_ptr->vertical[idx++]->setX(x);
    }

    while (idx < d_ptr->vertical.size())
        delete d_ptr->vertical.takeLast();
}

void RRDGrid::updateHorizontalGrid(qreal step, const QRectF &view)
{
    qreal y = view.top();
    y -= (fmod(y, step));

    if (d_ptr->horizontal.isEmpty())
        d_ptr->horizontal.append(
                    new QGraphicsLineItem(d_ptr->maxBound.left(), 0,
                                          d_ptr->maxBound.right(), 0, 0,
                                          widget()->scene()));
    d_ptr->horizontal[0]->setY(y);
    d_ptr->horizontal[0]->setPen(QPen(Qt::green)); /* TODO: REMOVE */

    y = step;
    int idx = 1;
    for (; y < view.height(); y += step)
    {
        if (idx >= d_ptr->horizontal.size())
            addHorizontalLine();
        d_ptr->horizontal[idx++]->setY(y);
    }

    while (idx < d_ptr->horizontal.size())
        delete d_ptr->horizontal.takeLast();
}

QGraphicsLineItem *RRDGrid::addHorizontalLine()
{
    d_ptr->horizontal.append(
                new QGraphicsLineItem(d_ptr->maxBound.left(), 0,
                                      d_ptr->maxBound.right(), 0,
                                      d_ptr->horizontal[0],
                                      widget()->scene()));
    QGraphicsLineItem *item(d_ptr->horizontal.last());
    item->setPen(QPen(Qt::DotLine));
    return item;
}

QGraphicsLineItem *RRDGrid::addVerticalLine()
{
    d_ptr->vertical.append(
                new QGraphicsLineItem(0, d_ptr->maxBound.top(),
                                      0, d_ptr->maxBound.bottom(),
                                      d_ptr->vertical[0],
                                      widget()->scene()));
    QGraphicsLineItem *item(d_ptr->vertical.last());
    item->setPen(QPen(Qt::DotLine));
    return item;
}

void RRDGrid::verticalRangeChanged()
{
    QRectF bound = widget()->sceneRect();
    if (bound.top() != d_ptr->maxBound.top() ||
            bound.bottom() != d_ptr->maxBound.bottom())
    {
        d_ptr->maxBound.setTop(bound.top());
        d_ptr->maxBound.setBottom(bound.bottom());

        LineList::iterator it = d_ptr->vertical.begin();
        for (; it != d_ptr->vertical.end(); ++it)
            (*it)->setLine(0, bound.top(), 0, bound.bottom());
    }
    prepare();
}

void RRDGrid::horizontalRangeChanged()
{
    QRectF bound = widget()->sceneRect();
    if (bound.left() != d_ptr->maxBound.left() ||
            bound.right() != d_ptr->maxBound.right())
    {
        d_ptr->maxBound.setLeft(bound.left());
        d_ptr->maxBound.setRight(bound.right());

        LineList::iterator it = d_ptr->horizontal.begin();
        for (; it != d_ptr->horizontal.end(); ++it)
            (*it)->setLine(bound.left(), 0, bound.right(), 0);
    }
    prepare();
}

void RRDGrid::verticalPosChanged()
{
    d_ptr->view =sceneView();

    qreal y = d_ptr->view.top();
    y -= (fmod(y, d_ptr->step.y()));

    if (!d_ptr->horizontal.isEmpty())
        d_ptr->horizontal[0]->setY(y);
}

void RRDGrid::horizontalPosChanged()
{
    d_ptr->view = sceneView();

    qreal x = d_ptr->view.left();
    x -= (fmod(x, d_ptr->step.x()));

    if (!d_ptr->vertical.isEmpty())
        d_ptr->vertical[0]->setX(x);
}

RRDWidget *RRDGrid::widget() const
{
    return static_cast<RRDWidget *>(parent());
}
