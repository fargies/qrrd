

#ifndef RRDPLOTTER_HPP
#define RRDPLOTTER_HPP

#include <QObject>
#include <QPainter>
#include <QString>
#include <QSharedDataPointer>

#include "RRA.hpp"

class RRDFile;
class RRDPlotterPrivate;

class RRDPlotter
{
public:
    RRDPlotter();
    explicit RRDPlotter(const RRDFile &rrd);
    RRDPlotter(const RRDPlotter &other);
    ~RRDPlotter();
    RRDPlotter &operator = (const RRDPlotter &other);

    bool isValid() const;

    RRA::ConsFunc consFunc() const;

    RRDFile rrd() const;

    void draw(const QDateTime &start, const QDateTime &end, QPainter &painter);

public:
    /**
     * @brief select consolidation function to display
     * @param[in] function to display
     * @return true if this function exists for the given RRD file
     */
    bool setConsFunc(RRA::ConsFunc func);

protected:
    QSharedDataPointer<RRDPlotterPrivate> d_ptr;
};

#endif // RRDPLOTTER_HPP
