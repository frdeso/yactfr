/*
 * Copyright (C) 2015-2018 Philippe Proulx <eepp.ca>
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

#include <yactfr/text-loc.hpp>

namespace yactfr {

TextLocation::TextLocation(const Index lineNumber, const Index colNumber) noexcept :
    _lineNumber {lineNumber},
    _colNumber {colNumber}
{
}

} // namespace yactfr