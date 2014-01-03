/*
** Copyright (C) 2014 Fargier Sylvain <fargier.sylvain@free.fr>
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
** RRDGraphItem.cpp
**
**        Created on: Jan 01, 2014
**   Original Author: Fargier Sylvain <fargier.sylvain@free.fr>
**
*/

#include <QSizeF>
#include <QGraphicsScale>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QCursor>
#include <QPen>
#include <QDebug>
#include <QPropertyAnimation>

#include "RRDGraphItem.hpp"

struct RRDGraphItemPrivate
{
    RRDGraphItemPrivate() :
        zoom(1.0, 1.0)
    {}

    QSizeF zoom;
    RRDPathGroup *group;
};

RRDGraphItem::RRDGraphItem(QGraphicsItem *parent) :
    QGraphicsWidget(parent),
    d_ptr(new RRDGraphItemPrivate)
{
    d_ptr->group = new RRDPathGroup(this);
    setCursor(Qt::CrossCursor);
    setFlags(QGraphicsItem::ItemClipsChildrenToShape |
             QGraphicsItem::ItemIsMovable);
}

RRDGraphItem::~RRDGraphItem()
{
}

QSizeF RRDGraphItem::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    switch (which) {
        case Qt::MinimumSize:
            return QSizeF(20, 20);
        case Qt::PreferredSize:
            return QSizeF(500, 200);
        case Qt::MaximumSize:
            return d_ptr->group->childrenBoundingRect().size();

        default:
            break;
    }
    return constraint;
}

void RRDGraphItem::setGeometry(const QRectF &rect)
{
    fit(rect);
    QGraphicsWidget::setGeometry(rect);
}

void RRDGraphItem::fit(const QRectF &g)
{
    QRectF geom(g);
    if (!geom.isValid())
        geom = geometry();
    if (!geom.isValid())
        geom = QRectF(QPointF(0, 0), sizeHint(Qt::MinimumSize, QSizeF()));

    d_ptr->group->setZoom(geom.width() / (d_ptr->group->childrenBoundingRect().width() * d_ptr->zoom.width()),
                          geom.height() / (d_ptr->group->childrenBoundingRect().height() * d_ptr->zoom.height()));
}

int RRDGraphItem::addPath(const QPainterPath &path)
{
    QGraphicsPathItem *item = new QGraphicsPathItem(path, d_ptr->group);
    item->setPen(QPen(Qt::black));
    item->setBrush(Qt::yellow);
    item->translate(0, -item->boundingRect().y());
    fit();
    return d_ptr->group->childItems().size() - 1;
}

void RRDGraphItem::removePath(int idx)
{
    if (idx >= d_ptr->group->childItems().size())
        return;

    QGraphicsItem *item = d_ptr->group->childItems().takeAt(idx);
    delete item;
}

RRDPathGroup *RRDGraphItem::pathGroup() const
{
    return d_ptr->group;
}

QRectF RRDGraphItem::pathGroupBoundingRect() const
{
    return d_ptr->group->mapFromParent(boundingRect()).boundingRect();
}

void RRDGraphItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * evt)
{
    QGraphicsItem::mouseDoubleClickEvent(evt);

    if (evt->button() != Qt::LeftButton && evt->button() != Qt::RightButton)
        return;

    d_ptr->group->stopAnimation();
    QPointF pos = d_ptr->group->mapFromParent(evt->pos());

    switch (evt->button())
    {
        case Qt::LeftButton:
            d_ptr->zoom /= 2;
            fit();
            break;
        case Qt::RightButton:
            d_ptr->zoom *= 2;
            if (d_ptr->zoom.height() > 1)
                d_ptr->zoom = QSizeF(1, 1);
            fit();
            break;
        default: break;
    }
    pos = boundingRect().center() - d_ptr->group->mapToParent(pos);
    d_ptr->group->moveBy(pos.x(), pos.y());
}

void RRDGraphItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        d_ptr->group->stopAnimation();
        d_ptr->group->setPos(d_ptr->group->pos() + event->pos()
                             - event->lastPos());

        event->accept();
    }
    else
        event->ignore();
}

void RRDGraphItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *evt)
{
    if (evt->button() != Qt::LeftButton && evt->button() != Qt::RightButton)
        return;

    d_ptr->group->fit();
}

RRDPathGroup::RRDPathGroup(QGraphicsItem *parent) :
    QGraphicsWidget(parent),
    m_anim(new QPropertyAnimation(this))
{
    m_anim->setTargetObject(this);
}

void RRDPathGroup::stopAnimation()
{
    if (m_anim->state() != QAbstractAnimation::Stopped)
    {
        QVariant endPos = m_anim->endValue();
        m_anim->stop();
        setProperty(m_anim->propertyName(), endPos);
    }
}

void RRDPathGroup::fit()
{
    stopAnimation();

    QRectF groupBound = mapToParent(childrenBoundingRect()).boundingRect();
    QRectF parentBound = parentItem()->boundingRect();
    QPointF p(pos());

    m_anim->setPropertyName("pos");
    m_anim->setDuration(1000);
    m_anim->setEasingCurve(QEasingCurve::OutBounce);

    if ((p.x() + groupBound.width()) < parentBound.right())
        p.setX(parentBound.right() - groupBound.width());
    else if (p.x() > parentBound.left())
        p.setX(parentBound.left());

    if ((p.y() + groupBound.height()) < parentBound.bottom())
        p.setY(parentBound.bottom() - groupBound.height());
    else if (p.y() > parentBound.top())
        p.setY(parentBound.top());
    if (p != pos())
    {
        m_anim->setEndValue(p);
        m_anim->start();
    }
}

void RRDPathGroup::setZoom(qreal x, qreal y)
{
    if (!m_zoom.isValid() || m_zoom.width() != x || m_zoom.height() != y)
    {
        QTransform scale;
        scale.scale(x, y);
        stopAnimation();
        setTransform(scale);
        m_zoom = QSizeF(x, y);
        emit zoomChanged();
    }
}

