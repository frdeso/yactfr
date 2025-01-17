/*
 * Copyright (C) 2017-2022 Philippe Proulx <eepp.ca>
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

#include <algorithm>
#include <cassert>
#include <tuple>
#include <set>

#include <yactfr/text-loc.hpp>
#include <yactfr/metadata/sl-array-type.hpp>
#include <yactfr/metadata/dl-array-type.hpp>
#include <yactfr/metadata/sl-str-type.hpp>
#include <yactfr/metadata/dl-str-type.hpp>
#include <yactfr/metadata/struct-type.hpp>
#include <yactfr/metadata/var-type.hpp>
#include <yactfr/text-parse-error.hpp>

#include "data-loc-map.hpp"

namespace yactfr {
namespace internal {

DataLocMap::DataLocMap(const PseudoDt& pseudoDt, const Scope scope) :
    _scope {scope}
{
    this->_create(pseudoDt);
}

const DataLocation& DataLocMap::operator[](const PseudoDt& pseudoDt) const noexcept
{
    assert(_map.find(&pseudoDt) != _map.end());
    return _map.at(&pseudoDt);
}

DataLocation DataLocMap::_dataLocFromPseudoDataLoc(const PseudoDataLoc& pseudoDataLoc)
{
    /*
     * `isEnv` is a temporary property which leads to a
     * `PseudoSlArrayType` instance in
     * TsdlParser::_parseArraySubscripts().
     */
    assert(!pseudoDataLoc.isEnv());

    if (pseudoDataLoc.isAbs()) {
        // already absolute
        return DataLocation {pseudoDataLoc.scope(), pseudoDataLoc.pathElems()};
    }

    /*
     * Relative location: start the search with the last entry of
     * `_memberNames`.
     */
    auto searchEntryIt = _memberNames.end() - 1;

    // this is the name we're looking for (first)
    const auto& firstPathElem = pseudoDataLoc.pathElems().front();

    // predicate for std::find_if() below
    const auto pred = [&firstPathElem](const auto namePtr) {
        return firstPathElem == *namePtr;
    };

    while (true) {
        if (std::find_if(searchEntryIt->begin(), searchEntryIt->end(), pred) !=
                searchEntryIt->end()) {
            // found it
            break;
        }

        /*
         * We can assert this because at this point the target member
         * name must exist. This is checked by
         * TsdlParserBase::_pseudoDataLocFromRelAllPathElems() (yes
         * those classes are very coupled) when also making sure that
         * the target doesn't cross a data type alias boundary.
         */
        assert(searchEntryIt != _memberNames.begin());
        --searchEntryIt;
    }

    /*
     * Now we go back from the first entry of `_memberNames` and append
     * the parent member names until we reach `searchEntryIt` (excluding
     * it).
     */
    DataLocation::PathElements pathElems;

    for (auto entryIt = _memberNames.begin(); entryIt != searchEntryIt; ++entryIt) {
        // last item of the entry is the parent name
        pathElems.push_back(*entryIt->back());
    }

    // append remaining, relative path elements
    std::copy(pseudoDataLoc.pathElems().begin(), pseudoDataLoc.pathElems().end(),
              std::back_inserter(pathElems));
    return DataLocation {_scope, std::move(pathElems)};
}

void DataLocMap::_create(const PseudoDt& pseudoDt)
{
    switch (pseudoDt.kind()) {
    case PseudoDt::Kind::SL_ARRAY:
    {
        auto& pseudoArrayType = static_cast<const PseudoSlArrayType&>(pseudoDt);

        this->_create(pseudoArrayType.pseudoElemType());
        break;
    }

    case PseudoDt::Kind::DL_ARRAY:
    {
        auto& pseudoArrayType = static_cast<const PseudoDlArrayType&>(pseudoDt);

        _map.emplace(std::make_pair(&pseudoArrayType,
                                    this->_dataLocFromPseudoDataLoc(pseudoArrayType.pseudoLenLoc())));
        this->_create(pseudoArrayType.pseudoElemType());
        break;
    }

    case PseudoDt::Kind::DL_BLOB:
    {
        auto& pseudoBlobType = static_cast<const PseudoDlBlobType&>(pseudoDt);

        _map.emplace(std::make_pair(&pseudoBlobType,
                                    this->_dataLocFromPseudoDataLoc(pseudoBlobType.pseudoLenLoc())));
        break;
    }

    case PseudoDt::Kind::STRUCT:
    {
        auto& pseudoStructType = static_cast<const PseudoStructType&>(pseudoDt);

        // new level
        _memberNames.push_back({});

        for (const auto& pseudoMemberType : pseudoStructType.pseudoMemberTypes()) {
            /*
             * Append this name before so that _createLocMap() can
             * access the ancestor names.
             */
            assert(pseudoMemberType->name());
            _memberNames.back().push_back(&(*pseudoMemberType->name()));
            this->_create(pseudoMemberType->pseudoDt());
        }

        _memberNames.pop_back();
        break;
    }

    case PseudoDt::Kind::VAR:
    case PseudoDt::Kind::VAR_WITH_INT_RANGES:
    {
        auto& pseudoVarType = static_cast<const PseudoVarType&>(pseudoDt);

        assert(pseudoVarType.pseudoSelLoc());
        _map.emplace(std::make_pair(&pseudoVarType,
                                    this->_dataLocFromPseudoDataLoc(*pseudoVarType.pseudoSelLoc())));

        for (const auto& pseudoOpt : pseudoVarType.pseudoOpts()) {
            this->_create(pseudoOpt->pseudoDt());
        }

        break;
    }

    case PseudoDt::Kind::OPT_WITH_BOOL_SEL:
    case PseudoDt::Kind::OPT_WITH_INT_SEL:
    {
        auto& pseudoOptType = static_cast<const PseudoOptType&>(pseudoDt);

        _map.emplace(std::make_pair(&pseudoOptType,
                                    this->_dataLocFromPseudoDataLoc(pseudoOptType.pseudoSelLoc())));
        this->_create(pseudoOptType.pseudoDt());
        break;
    }

    default:
        break;
    }
}

} // namespace internal
} // namespace yactfr
