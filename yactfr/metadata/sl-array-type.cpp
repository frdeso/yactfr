/*
 * Copyright (C) 2015-2022 Philippe Proulx <eepp.ca>
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

#include <yactfr/metadata/sl-array-type.hpp>

namespace yactfr {

StaticLengthArrayType::StaticLengthArrayType(const unsigned int minAlign, DataType::UP elemType,
                                             const Size len, const bool hasTraceTypeUuidRole) :
    ArrayType {_KIND_SL_ARRAY, minAlign, std::move(elemType)},
    _len {len},
    _hasTraceTypeUuidRole {hasTraceTypeUuidRole}
{
}

DataType::UP StaticLengthArrayType::_clone() const
{
    return std::make_unique<StaticLengthArrayType>(this->minimumAlignment(),
                                                   this->elementType().clone(), _len,
                                                   _hasTraceTypeUuidRole);
}

bool StaticLengthArrayType::_compare(const DataType& other) const noexcept
{
    auto& otherStaticLengthArrayType = static_cast<const StaticLengthArrayType&>(other);

    return ArrayType::_compare(other) && _len == otherStaticLengthArrayType._len &&
           otherStaticLengthArrayType._hasTraceTypeUuidRole == _hasTraceTypeUuidRole;
}

} // namespace yactfr