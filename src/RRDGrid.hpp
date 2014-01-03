#ifndef RRDGRID_HPP
#define RRDGRID_HPP

#include <QGraphicsWidget>
#include <QScopedPointer>

#include "RRDWidget.hpp"

class RRDGridPrivate;
class RRDPathGroup;

class RRDGrid : public QGraphicsWidget
{
    Q_OBJECT
public:
    explicit RRDGrid(RRDWidget *widget, QGraphicsItem *parent = 0);
    ~RRDGrid();

    void prepare();

protected slots:

    void onGeometryChanged();
    void onScaleChanged();

    void onVerticalPosChanged();
    void onHorizontalPosChanged();
    void onVerticalGeometryChanged();
    void onHorizontalGeometryChanged();

protected:
    RRDWidget *widget() const;
    RRDPathGroup *pathGroup() const;

    void updateHorizontalGrid(qreal step, const QRectF &view);
    void updateVerticalGrid(qreal step, const QRectF &view);

    QGraphicsLineItem *addVerticalLine();
    QGraphicsLineItem *addHorizontalLine();

    /**
     * @brief the grid's virtual bounding rect
     */
    QRectF gridBoundingRect();

    /**
     * @brief return the grid steps to use in graph's coordinates
     */
    QPointF gridStep() const;
    qreal timeGridNext();
    QRectF sceneView() const;

    QScopedPointer<RRDGridPrivate> d_ptr;
};

#endif // RRDGRID_HPP
