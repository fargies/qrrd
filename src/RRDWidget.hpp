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
** 1. The origin of this software must not be misrepresented; you must not
**    claim that you wrote the original software. If you use this software
**    in a product, an acknowledgment in the product documentation would be
**    appreciated but is not required.
** 2. Altered source versions must be plainly marked as such, and must not be
**    misrepresented as being the original software.
** 3. This notice may not be removed or altered from any source distribution.
**
** RRDWidget.hpp
**
**        Created on: Dec 24, 2013
**   Original Author: Fargier Sylvain <fargier.sylvain@free.fr>
**
*/

#ifndef RRDWIDGET_HPP
#define RRDWIDGET_HPP

#include <QScopedPointer>
#include <QGraphicsView>

#include "RRDFile.hpp"
#include "RRDPlotter.hpp"

class RRDWidgetPrivate;

class RRDWidget : public QGraphicsView
{
    Q_OBJECT
    Q_PROPERTY(QDateTime start READ start WRITE setStart NOTIFY startChanged)
    Q_PROPERTY(QDateTime end READ end WRITE setEnd NOTIFY endChanged)

public:
    explicit RRDWidget(QWidget *parent = 0);
    ~RRDWidget();

    QDateTime start() const;
    QDateTime end() const;
    uint range() const;

    int addSource(const RRDFile &rrd);
    RRDFile source(int idx);
    QList<RRDPlotter> sources() const;

public slots:
    void setStart(const QDateTime &start);
    void setEnd(const QDateTime &end);

signals:
    void startChanged(const QDateTime &start);
    void endChanged(const QDateTime &end);

protected:
    void updatePaths();
    void paintEvent(QPaintEvent *evt);
    void mouseDoubleClickEvent(QMouseEvent * evt);
    void mouseReleaseEvent(QMouseEvent *evt);
    void mouseMoveEvent(QMouseEvent *evt);

    void fit();

    QScopedPointer<RRDWidgetPrivate> d_ptr;
};

#endif // RRDWIDGET_HPP
