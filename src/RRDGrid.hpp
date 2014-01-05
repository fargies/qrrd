#ifndef RRDGRID_HPP
#define RRDGRID_HPP

#include <QGraphicsWidget>
#include <QScopedPointer>

#include "RRDWidget.hpp"

class RRDGridPrivate;
class RRDPathGroup;
class RRDGridLine;

class QGraphicsAnchorLayout;

class RRDGrid : public QGraphicsWidget
{
    Q_OBJECT
public:
    explicit RRDGrid(RRDWidget *widget, QGraphicsWidget *parent = 0);
    ~RRDGrid();

    //    void setTitle(const QString &title);
    //    void setLabel(Qt::Orientation orientation, const QString &label);

    void prepare();

    /**
     * @brief The TimeStep enum
     * @details used to define the grid step on the horizontal axis
     * (vertical grid).
     */
    enum TimeStep
    {
        SecStep,
        MinStep,
        HourStep,
        DayStep,
        WeekStep,
        MonthStep,
        YearStep
    };

    class Step;

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

    void updateHorizontalGrid(const Step &step, const QRectF &view);
    void updateVerticalGrid(const Step &step, const QRectF &view);

    RRDGridLine *addVerticalLine();
    RRDGridLine *addHorizontalLine();

    void setOriginLabel(Qt::Orientation orientation, const QString &label);
    void updateOriginLabelPos();

    /**
     * @brief compute the grid steps to use in graph's coordinates
     */
    void gridStep(Step &step) const;
    QDateTime timeGridMod(const QDateTime &date, const Step &step) const;
    QDateTime timeGridNext(const QDateTime &date, const Step &step) const;

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
