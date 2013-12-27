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
** RRDFile.hpp
**
**        Created on: Dec 21, 2013
**   Original Author: Fargier Sylvain <fargier.sylvain@free.fr>
**
*/

#ifndef RRDFILE_HPP
#define RRDFILE_HPP

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QList>
#include <QSet>
#include <QVector>
#include <QSharedDataPointer>
#include <QStringList>

#include "DS.hpp"
#include "RRA.hpp"

class RRDFilePrivate;

class RRDFile
{
public:
    explicit RRDFile(const QString &fileName);
    RRDFile();
    RRDFile(RRDFilePrivate &priv);
    RRDFile(const RRDFile &other);

    ~RRDFile();

    RRDFile &operator = (const RRDFile &other);

    bool isValid() const;

    QString version() const;

    QString fileName() const;

    /**
     * @brief last update timestamp
     * @note last update is not obviously the last consolidated datapoint
     * timestamp.
     * @return last rrd file update.
     */
    QDateTime lastUpdate() const;

    QStringList dsNames() const;
    QString dsName(uint index) const;
    QList<DataSource> ds() const;

    /**
     * @brief pdp step information
     *
     * @return the number of seconds between two consolidated points.
     */
    uint pdpStep() const;

    /**
     * @brief list of available consolidation functions.
     */
    QSet<RRA::ConsFunc> consFuncs() const;
    bool hasFunc(RRA::ConsFunc func) const;

    QList<RRA> rra() const;

    /**
     * @brief return the latest value recorded for a given func
     * @param[in] function
     */
    QDateTime lastUpdate(RRA::ConsFunc function) const;

    /**
     * @brief return the oldest value the RRD can contain for a given func
     *
     * @note this value is not always available in the RRA database.
     */
    QDateTime firstUpdate(RRA::ConsFunc function) const;

    /**
     * @brief find the best matching RRA
     * @param[in] function
     * @param[in] first
     * @param[in] last
     * @return the best matching RRA
     */
    RRA bestMatch(
            RRA::ConsFunc function,
            const QDateTime &first,
            const QDateTime &last);

    /**
     * @brief fetch data from the RRD file.
     *
     * @details all the datastores are loaded at the same time.
     *
     * @note loaded start/end/resolution doesn't obviously match requested ones.
     */
    bool fetch(
            RRA::ConsFunc function,
            const QDateTime &first,
            const QDateTime &last,
            uint resolution = 0);

    /**
     * @brief starting date
     *
     * @details fetched data starting date.
     */
    QDateTime first() const;

    /**
     * @brief consolidation function
     *
     * @note this function can also be used to return the first function found
     * when no data has already been fetched.
     *
     * @details fetched data consolidation function.
     */
    RRA::ConsFunc function() const;

    /**
     * @brief end date
     *
     * @details fetched data end date.
     */
    QDateTime last() const;

    /**
     * @brief fetched data step
     *
     * @details in seconds.
     * @return fetched data step in seconds.
     */
    uint step() const;

    /**
     * @brief fetched data items
     *
     * @details
     * @return number of items fetched
     */
    uint itemCount() const;

    /**
     * @brief fetched data
     *
     * @details this data should be used with a QDataStream and deserialized in
     * rrd_value_t types.
     *
     * @return a byte array containing raw fetched data
     */
    QByteArray items() const;

    /**
     * @brief clear fetched data
     */
    void clear();

private:
    QSharedDataPointer<RRDFilePrivate> d_ptr;
};

#endif // RRDFILE_HPP
