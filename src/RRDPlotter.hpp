/*
** This document and/or file is SOMFY's property. All information it
** contains is strictly confidential. This document and/or file shall
** not be used, reproduced or passed on in any way, in full or in part
** without SOMFY's prior written approval. All rights reserved.
** Ce document et/ou fichier est la propritye SOMFY. Les informations
** quil contient sont strictement confidentielles. Toute reproduction,
** utilisation, transmission de ce document et/ou fichier, partielle ou
** intégrale, non autorisée préalablement par SOMFY par écrit est
** interdite. Tous droits réservés.
** 
** Copyright © (2009-2012), Somfy SAS. All rights reserved.
** All reproduction, use or distribution of this software, in whole or
** in part, by any means, without Somfy SAS prior written approval, is
** strictly forbidden.
**
** RRDPlotter.hpp
**
**        Created on: Dec 27, 2013
**   Original Author: Sylvain Fargier <sylvain.fargier@somfy.com>
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
