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
** RRA.hpp
**
**        Created on: Dec 21, 2013
**   Original Author: Fargier Sylvain <fargier.sylvain@free.fr>
**
*/

#ifndef __RRA_HPP__
#define __RRA_HPP__

#include <QSharedDataPointer>

class RRAPrivate;

class RRA
{
public:
    RRA();
    ~RRA();
    RRA(const RRA &other);

    RRA &operator = (const RRA &other);

    enum ConsFunc
    {
        AVERAGE,
        MIN,
        MAX,
        LAST
    };

    bool isValid() const;

    ConsFunc function() const;
    uint pdpPerRow() const;
    uint rowCount() const;
    qreal xff() const;

    void setFunction(ConsFunc func);
    void setPdpPerRow(uint pdpPerRow);
    void setRowCount(uint rowCount);
    void setXff(qreal xff);

    /**
     * @brief previous consolidated point (row)
     */
    uint prevRow(uint stamp, uint step) const;

    /**
     * @brief next consolidated point (row)
     */
    uint nextRow(uint stamp, uint step) const;

    /**
     * @brief convert a ConsFunc toString
     *
     * @param[in] f function to convert
     * @return a string representation of the consolidation function.
     */
    static QString toString(ConsFunc f);

    /**
     * @brief string to ConsFunc conversion function
     *
     * @param[in] str string to parse
     * @param[out] f function
     * @return true if the string was successfully parsed
     */
    static bool fromString(const QString &str, ConsFunc &f);

private:
    QSharedDataPointer<RRAPrivate> d_ptr;
};

#endif

