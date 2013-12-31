#ifndef RRDGRID_HPP
#define RRDGRID_HPP

#include <QGraphicsObject>
#include <QScopedPointer>

#include "RRDWidget.hpp"

class RRDGridPrivate;

class RRDGrid : public QObject
{
    Q_OBJECT
public:
    explicit RRDGrid(RRDWidget *parent);
    ~RRDGrid();

    void prepare();

protected slots:
    void verticalRangeChanged();
    void horizontalRangeChanged();
    void verticalPosChanged();
    void horizontalPosChanged();

    RRDWidget *widget() const;

protected:
    void updateHorizontalGrid(qreal step, const QRectF &view);
    void updateVerticalGrid(qreal step, const QRectF &view);
    QGraphicsLineItem *addVerticalLine();
    QGraphicsLineItem *addHorizontalLine();

    QPointF gridStep(const QRectF &view) const;
    qreal timeGridNext();
    QRectF sceneView() const;

    QScopedPointer<RRDGridPrivate> d_ptr;
};

#endif // RRDGRID_HPP
