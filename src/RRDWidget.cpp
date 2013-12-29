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
#include <QDebug>

#include "RRDWidget.hpp"

struct RRDWidgetPrivate
{
    RRDWidgetPrivate()
    {}

    QDateTime start;
    QDateTime end;
    QList<RRDPlotter> rrd;
};

RRDWidget::RRDWidget(QWidget *parent) :
    QGraphicsView(parent),
    d_ptr(new RRDWidgetPrivate)
{
    setDragMode(QGraphicsView::ScrollHandDrag);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setBackgroundBrush(QBrush(Qt::red, Qt::CrossPattern));
    setScene(new QGraphicsScene(this));
}

RRDWidget::~RRDWidget()
{}

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
}

void RRDWidget::paintEvent(QPaintEvent *evt)
{
    fit();
    QGraphicsView::paintEvent(evt);
}

void RRDWidget::mouseDoubleClickEvent(QMouseEvent *evt)
{
    uint delta;
    QPointF pos = mapToScene(evt->pos());
    QRectF view(mapToScene(0, 0), mapToScene(width(), height()));

    switch (evt->button())
    {
        case Qt::LeftButton:
            {
                qDebug() << "before" << view;
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

void RRDWidget::fit()
{
//    fitInView(-verticalScrollBar()->, 0, range(), height());
    /*resetTransform();
    scale((qreal) width() / (d_ptr->end.toTime_t() - d_ptr->start.toTime_t()),
            1);*/
}

