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
** DS.hpp
**
**        Created on: Dec 22, 2013
**   Original Author: Fargier Sylvain <fargier.sylvain@free.fr>
**
*/

#ifndef __DS_HPP__
#define __DS_HPP__

#include <QSharedDataPointer>
#include <QString>

class DSPrivate;

class DataSource
{
public:
    DataSource();
    ~DataSource();
    DataSource(const DataSource &other);

    DataSource &operator = (const DataSource &other);

    enum Type
    {
        COUNTER,
        ABSOLUTE,
        GAUGE,
        DERIVE,
        CDEF
    };

    bool isValid() const;

    void setName(const QString &name);
    void setIndex(uint idx);
    void setType(Type type);
    void setMinHB(uint count);
    void setMin(double min);
    void setMax(double max);

    uint index() const;
    Type type() const;
    uint minHB() const;
    double min() const;
    double max() const;
    QString name() const;

    /**
     * @brief toString convert a type to string
     *
     * @param[in] type the type to convert
     * @return string representation of the given type
     */
    static QString toString(Type type);

    /**
     * @brief fromString convert a type from string
     *
     * @param[in] str the string to parse
     * @param[out] type the parsed type
     * @return true on success
     */
    static bool fromString(const QString &str, Type &type);

private:
    QSharedDataPointer<DSPrivate> d_ptr;
};

#endif

