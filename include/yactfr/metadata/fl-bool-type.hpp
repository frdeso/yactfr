/*
 * Copyright (C) 2022 Philippe Proulx <eepp.ca>
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

#ifndef _YACTFR_METADATA_FL_BOOL_TYPE_HPP
#define _YACTFR_METADATA_FL_BOOL_TYPE_HPP

#include "fl-bit-array-type.hpp"
#include "bo.hpp"
#include "dt.hpp"
#include "dt-visitor.hpp"

namespace yactfr {

class DataTypeVisitor;

/*!
@brief
    Fixed-length boolean type.

@ingroup metadata_dt

A fixed-length boolean type describes data stream fixed-length booleans.
*/
class FixedLengthBooleanType final :
    public FixedLengthBitArrayType
{
public:
    /*!
    @brief
        Builds a fixed-length boolean type.

    @param[in] alignment
        Alignment of data stream fixed-length booleans described by this
        type.
    @param[in] length
        Length of data stream fixed-length booleans (bits) described by
        this type.
    @param[in] byteOrder
        Byte order of data stream fixed-length booleans described by
        this type.
    @param[in] userAttributes
        @parblock
        User attributes of data stream fixed-length booleans described
        by this type.

        If set, each key of \p *userAttributes is a namespace.
        @endparblock

    @pre
        \p alignment > 0.
    @pre
        \p alignment is a power of two.
    @pre
        \p length > 0.
    */
    explicit FixedLengthBooleanType(unsigned int alignment, unsigned int length,
                                    ByteOrder byteOrder, MapItem::UP userAttributes = nullptr);

    /*!
    @brief
        Builds a fixed-length boolean type having a default alignment
        property (1).

    @param[in] length
        Length of data stream fixed-length booleans (bits) described by
        this type.
    @param[in] byteOrder
        Byte order of data stream fixed-length booleans described by
        this type.
    @param[in] userAttributes
        @parblock
        User attributes of data stream fixed-length booleans described
        by this type.

        If set, each key of \p *userAttributes is a namespace.
        @endparblock

    @pre
        \p length > 0.
    */
    explicit FixedLengthBooleanType(unsigned int length, ByteOrder byteOrder,
                                    MapItem::UP userAttributes = nullptr);


    /*!
    @brief
        Copy constructor.

    @param[in] other
        Fixed-length boolean type to copy.
    */
    FixedLengthBooleanType(const FixedLengthBooleanType& other);

private:
    DataType::UP _clone() const override;

    void _accept(DataTypeVisitor& visitor) const override
    {
        visitor.visit(*this);
    }
};

} // namespace yactfr

#endif // _YACTFR_METADATA_FL_BOOL_TYPE_HPP
