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
** 1. The start of this software must not be misrepresented; you must not
**    claim that you wrote the startal software. If you use this software
**    in a product, an acknowledgment in the product documentation would be
**    appreciated but is not required.
** 2. Altered source versions must be plainly marked as such, and must not be
**    misrepresented as being the startal software.
** 3. This notice may not be removed or altered from any source distribution.
**
** RRDWidget.cpp
**
**        Created on: Dec 24, 2013
**   startal Author: Fargier Sylvain <fargier.sylvain@free.fr>
**
*/

#include <QPainter>
#include <QPoint>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QGraphicsSimpleTextItem>
#include <QDebug>

#include "RRDWidget.hpp"
#include "RRDGrid.hpp"

struct RRDWidgetPrivate
{
    RRDWidgetPrivate() :
        grid(0),
        overlay(0)
    {}

    QDateTime start;
    QDateTime end;
    QList<RRDPlotter> rrd;
    RRDGrid *grid;
    QGraphicsView *overlay;
};

RRDWidget::RRDWidget(QWidget *parent) :
    QGraphicsView(parent),
    d_ptr(new RRDWidgetPrivate)
{
    setDragMode(QGraphicsView::ScrollHandDrag);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setScene(new QGraphicsScene(this));
    viewport()->setCursor(Qt::CrossCursor);
    setViewportMargins(50,50,50,50);

    d_ptr->grid = new RRDGrid(this);
    d_ptr->overlay = new QGraphicsView(this);
    d_ptr->overlay->setScene(new QGraphicsScene(d_ptr->overlay));
    d_ptr->overlay->setAcceptDrops();
}

RRDWidget::~RRDWidget()
{
    delete d_ptr->grid;
}

QDateTime RRDWidget::start() const
{
    return d_ptr->start;
}

QDateTime RRDWidget::end() const
{
    return d_ptr->end;
}

uint RRDWidget::range() const
{
    if (!d_ptr->end.isValid() || !d_ptr->start.isValid())
        return 0;
    else
        return d_ptr->end.toTime_t() - d_ptr->start.toTime_t();
}

int RRDWidget::addSource(const RRDFile &rrd)
{
    if (!d_ptr->start.isValid() || !d_ptr->end.isValid())
    {
        d_ptr->start = rrd.firstUpdate(rrd.current().function());
        d_ptr->end = rrd.lastUpdate(rrd.current().function());
    }
    d_ptr->rrd.push_back(RRDPlotter(rrd));
    updatePaths();
}

RRDFile RRDWidget::source(int idx)
{
    return d_ptr->rrd.value(idx).rrd();
}

QList<RRDPlotter> RRDWidget::sources() const
{
    return d_ptr->rrd;
}

void RRDWidget::setStart(const QDateTime &start)
{
    if (d_ptr->start != start)
    {
        d_ptr->start = start;
        emit startChanged(start);
        update();
    }
}

void RRDWidget::setEnd(const QDateTime &end)
{
    if (d_ptr->end != end)
    {
        d_ptr->end = end;
        emit endChanged(end);
        update();
    }
}

void RRDWidget::updatePaths()
{
    QList<RRDPlotter>::iterator it = d_ptr->rrd.begin();
    for (; it != d_ptr->rrd.end(); ++it)
    {
        it->prepare(d_ptr->start, d_ptr->end);
        for (int i = 0; i < it->dsNames().size(); ++i)
        {
            /* FIXME: do something with the pointer ? */
            scene()->addPath(it->path(i), QPen(Qt::black), QBrush(Qt::yellow));//it->pen(i), it->brush(i));
        }
    }
    fitInView(0, 0, range(), height());
    update();
    overlay()->scene()->addRect(rect(), QPen(Qt::yellow), QBrush(Qt::yellow, Qt::SolidPattern));
}

void RRDWidget::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    QGraphicsView::paintEvent(event);
    if (d_ptr->overlay)
        d_ptr->overlay->render(&p, rect());
}

QGraphicsView *RRDWidget::overlay() const
{
    return d_ptr->overlay;
}

void RRDWidget::mouseDoubleClickEvent(QMouseEvent *evt)
{
    QGraphicsView::mouseDoubleClickEvent(evt);
    uint delta;
    QPointF pos = mapToScene(evt->pos());
    QRectF view(mapToScene(0, 0), mapToScene(width(), height()));

    switch (evt->button())
    {
        case Qt::LeftButton:
            {
                view.setWidth(view.width() / 2);
                view.setHeight(view.height() / 2);
                view.moveCenter(pos);
                fitInView(view);
            }
            break;
        case Qt::RightButton:
            {
                view.setWidth(view.width() * 2);
                view.setHeight(view.height() * 2);
                view.moveCenter(pos);
                fitInView(view);
            }
            break;
        default: break;
    }
}

void RRDWidget::mouseReleaseEvent(QMouseEvent *evt)
{
    QGraphicsView::mouseReleaseEvent(evt);
    viewport()->setCursor(Qt::CrossCursor);
}

