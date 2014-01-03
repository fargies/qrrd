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
** RRDGraphItem.hpp
**
**        Created on: Jan 01, 2014
**   Original Author: Fargier Sylvain <fargier.sylvain@free.fr>
**
*/

#ifndef __RRD_GRAPH_ITEM_HPP__
#define __RRD_GRAPH_ITEM_HPP__

#include <QGraphicsItem>
#include <QGraphicsWidget>
#include <QGraphicsLayoutItem>

class RRDGraphItemPrivate;
class RRDPathGroup;
class QPropertyAnimation;

class RRDGraphItem : public QGraphicsWidget
{
public:
    explicit RRDGraphItem(QGraphicsItem *parent = 0);
    ~RRDGraphItem();

    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint) const;

    void setGeometry(const QRectF &rect);

    int addPath(const QPainterPath &path);
    void removePath(int idx);

    /**
     * @brief return the visible part of the associated paths
     * @details value is in path coordinate
     * @return the visible path bounding rect
     */
    QRectF pathGroupBoundingRect() const;

    RRDPathGroup *pathGroup() const;

protected:
    void fit(const QRectF &geometry = QRectF());

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *evt);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    QScopedPointer<RRDGraphItemPrivate> d_ptr;
};

class RRDPathGroup : public QGraphicsWidget
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

public:
    RRDPathGroup(QGraphicsItem *parent = 0);

    void stopAnimation();
    void fit();
    void setZoom(qreal x, qreal y);

signals:
    void zoomChanged();

protected:
    QPropertyAnimation *m_anim;
    QSizeF m_zoom;
};

#endif

