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
** RRDPlotter.hpp
**
**        Created on: Dec 27, 2013
**   Original Author: Fargier Sylvain <fargier.sylvain@free.fr>
**
*/

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

    /**
     * @brief direct access to rrd object.
     *
     * @details this operator is useful to retrieve information such as dsNames
     */
    const RRDFile *operator -> () const;
    RRDFile *operator -> ();

    void prepare(const QDateTime &start, const QDateTime &end);

    QStringList dsNames() const;

    /**
     * @brief retrieve the associated painterPath
     */
    QPainterPath path(uint index) const;

    /**
     * @brief retrieve a painting brush
     */
    QBrush brush(uint index) const;
    void setBrush(uint index, const QBrush &brush);

    /**
     * @brief retrieve a painting pen
     */
    QPen pen(uint index) const;
    void setPen(uint index, const QPen &pen);

    /**
     * @brief select consolidation function to display
     * @param[in] function to display
     * @return true if this function exists for the given RRD file
     */
    bool setConsFunc(RRA::ConsFunc func);

    QDateTime start() const;
    QDateTime end() const;

protected:
    QSharedDataPointer<RRDPlotterPrivate> d_ptr;
};

#endif // RRDPLOTTER_HPP
