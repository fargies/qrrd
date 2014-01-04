#ifndef RRDGRID_HPP
#define RRDGRID_HPP

#include <QGraphicsWidget>
#include <QScopedPointer>

#include "RRDWidget.hpp"

class RRDGridPrivate;
class RRDPathGroup;
class RRDGridLine;

class RRDGrid : public QGraphicsWidget
{
    Q_OBJECT
public:
    explicit RRDGrid(RRDWidget *widget, QGraphicsWidget *parent = 0);
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

    RRDGridLine *addVerticalLine();
    RRDGridLine *addHorizontalLine();

    /**
     * @brief return the grid steps to use in graph's coordinates
     */
    QPointF gridStep() const;
    qreal timeGridNext();
    QRectF sceneView() const;

    QScopedPointer<RRDGridPrivate> d_ptr;
};

class RRDGridLine : public QGraphicsLineItem
{
public:
    RRDGridLine(Qt::Orientation orientation,
                qreal p1, qreal p2,
                QGraphicsItem *parent = 0,
                QGraphicsScene *scene = 0);

    inline Qt::Orientation orientation() const
    { return m_orientation; }

    inline QGraphicsSimpleTextItem *legend() const
    { return m_legend; }

    void setLegendText(const QString &text);

protected:
    Qt::Orientation m_orientation;
    QGraphicsSimpleTextItem *m_legend;
};

#endif // RRDGRID_HPP
