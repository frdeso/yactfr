/*
 * Copyright (C) 2022 Philippe Proulx <eepp.ca>
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

#include <cassert>

#include <yactfr/metadata/non-nt-str-type.hpp>

namespace yactfr {

NonNullTerminatedStringType::NonNullTerminatedStringType(const int kind,
                                                         const unsigned int align) :
    ScalarDataType {kind | _KIND_NON_NT_STRING, align}
{
}

} // namespace yactfr