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
    QWidget(parent),
    d_ptr(new RRDWidgetPrivate)
{
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

int RRDWidget::addSource(const RRDFile &rrd)
{
    if (!d_ptr->start.isValid() || !d_ptr->end.isValid())
    {
        d_ptr->start = rrd.firstUpdate(rrd.function());
        d_ptr->end = rrd.lastUpdate(rrd.function());
    }
    d_ptr->rrd.push_back(RRDPlotter(rrd));
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
    }
}

void RRDWidget::setEnd(const QDateTime &end)
{
    if (d_ptr->end != end)
    {
        d_ptr->end = end;
        emit endChanged(end);
    }
}

void RRDWidget::paintEvent(QPaintEvent * /*evt*/)
{
    QPainter p(this);

    QList<RRDPlotter>::iterator it = d_ptr->rrd.begin();
    for (; it != d_ptr->rrd.end(); ++it)
    {
        it->draw(d_ptr->start,
                 d_ptr->end,
                 p);
    }
}
