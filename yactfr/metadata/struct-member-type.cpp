/*
 * Copyright (C) 2015-2018 Philippe Proulx <eepp.ca>
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

#include <utility>

#include <yactfr/metadata/struct-member-type.hpp>
#include <yactfr/internal/metadata/utils.hpp>

namespace yactfr {

static std::string dispNameFromName(const std::string& name)
{
    if (name[0] == '_') {
        return name.substr(1);
    } else {
        return name;
    }
}

StructureMemberType::StructureMemberType(std::string name, DataType::UP dt, MapItem::UP userAttrs) :
    _dispName {dispNameFromName(name)},
    _name {std::move(name)},
    _dt {std::move(dt)},
    _userAttrs {std::move(userAttrs)}
{
}

std::unique_ptr<const StructureMemberType> StructureMemberType::clone() const
{
    return std::make_unique<StructureMemberType>(_name, _dt->clone(),
                                                 internal::tryCloneUserAttrs(this->userAttributes()));
}

bool StructureMemberType::operator==(const StructureMemberType& other) const noexcept
{
    return _name == other._name && *_dt == *other._dt;
}

} // namespace yactfr
