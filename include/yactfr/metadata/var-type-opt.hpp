/*
 * Copyright (C) 2015-2018 Philippe Proulx <eepp.ca>
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

#ifndef _YACTFR_METADATA_VAR_TYPE_OPT_HPP
#define _YACTFR_METADATA_VAR_TYPE_OPT_HPP

#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>

#include <yactfr/internal/metadata/utils.hpp>

#include "dt.hpp"
#include "int-range-set.hpp"
#include "item.hpp"

namespace yactfr {
namespace internal {

class TraceTypeImpl;

} // namespace internal

/*!
@brief
    Generic \link VariantType variant type\endlink option.

@ingroup metadata_dt
*/
template <typename SelectorValueT>
class VariantTypeOption final :
    boost::noncopyable
{
    friend class internal::TraceTypeImpl;

public:
    /// Unique pointer to constant variant type option.
    using UP = std::unique_ptr<const VariantTypeOption>;

    /// Type of the value of a selector.
    using SelectorValue = SelectorValueT;

    /// Type of the selector range set.
    using SelectorRangeSet = IntegerRangeSet<SelectorValueT>;

public:
    /*!
    @brief
        Builds a variant type option named \p name having the data type
        \p dataType.

    @param[in] name
        Name of the variant type option.
    @param[in] dataType
        Data type of the variant type option.
    @param[in] selectorRanges
        Selector values for which this option is selected.
    @param[in] userAttributes
        @parblock
        User attributes of this variant type option.

        If set, each key of \p *userAttributes is a namespace.
        @endparblock

    @pre
        \p dataType is set.
    @pre
        \p selectorRanges is not empty.
    */
    explicit VariantTypeOption(boost::optional<std::string> name, DataType::UP dataType,
                               SelectorRangeSet selectorRanges,
                               MapItem::UP userAttributes = nullptr) :
        _name {std::move(name)},
        _dt {std::move(dataType)},
        _selRanges {std::move(selectorRanges)},
        _userAttrs {std::move(userAttributes)}
    {
    }

    /*!
    @brief
        Builds an unnamed variant type option having the type
        \p dataType.

    @param[in] dataType
        Data type of the variant type option.
    @param[in] selectorRanges
        Selector values for which this option is selected.
    @param[in] userAttributes
        @parblock
        User attributes of this variant type option.

        If set, each key of \p *userAttributes is a namespace.
        @endparblock

    @pre
        \p dataType is set.
    @pre
        \p selectorRanges is not empty.
    */
    explicit VariantTypeOption(DataType::UP dataType, SelectorRangeSet selectorRanges,
                               MapItem::UP userAttributes = nullptr) :
        VariantTypeOption {
            boost::none, std::move(dataType), std::move(selectorRanges), std::move(userAttributes)
        }
    {
    }

    /*!
    @brief
        Creates a constant variant type option unique pointer,
        forwarding \p args to the constructor.

    @param[in] args
        Arguments to forward to the variant type option constructor.

    @returns
        Created constant variant type option unique pointer.

    @pre
        See the preconditions of the constructor.
    */
    template <typename... ArgTs>
    static UP create(ArgTs&&... args)
    {
        return std::make_unique<typename UP::element_type>(std::forward<ArgTs>(args)...);
    }

    /// Name of this variant type option.
    const boost::optional<std::string>& name() const noexcept
    {
        return _name;
    }

    /*!
     * @brief
     *     Display name, or \c boost::none if missing or if this type is
     *     not part of a \link TraceType trace type\endlink yet.
     *
     * When the returned value is set, it's, depending on the
     * \link TraceType::majorVersion() major version\endlink of
     * the containing trace type:
     *
     * <dl>
     *   <dt>1
     *   <dd>name() with the first underscore removed, if any.
     *
     *   <dt>2
     *   <dd>name()
     * </dl>
     */
    const boost::optional<std::string>& displayName() const noexcept
    {
        return _dispName;
    }

    /// Data type of this variant type option.
    const DataType& dataType() const noexcept
    {
        return *_dt;
    }

    /// Selector values for which this option is selected.
    const SelectorRangeSet& selectorRanges() const noexcept
    {
        return _selRanges;
    }

    /*!
    @brief
        User attributes.

    If set, each key of \p *userAttributes is a namespace.

    @note
        Even if the return value isn't \c nullptr, the returned map
        item may still be empty (which also means no user attributes).
    */
    const MapItem *userAttributes() const noexcept
    {
        return _userAttrs.get();
    }

    /// Clone (deep copy) of this variant type option.
    UP clone() const
    {
        return VariantTypeOption::create(_name, _dt->clone(), _selRanges,
                                         internal::tryCloneUserAttrs(this->userAttributes()));
    }

    /*!
    @brief
        Equality operator.

    @param[in] other
        Other variant type option to compare to.

    @returns
        \c true if \p other is equal to this variant type option.
    */
    bool operator==(const VariantTypeOption<SelectorValueT>& other) const
    {
        return _name == other._name && *_dt == *other._dt && _selRanges == other._selRanges;
    }

    /*!
    @brief
        Non-equality operator.

    @param[in] other
        Other variant type option to compare to.

    @returns
        \c true if \p other is not equal to this variant type option.
    */
    bool operator!=(const VariantTypeOption<SelectorValueT>& other) const
    {
        return !(*this == other);
    }

private:
    const boost::optional<std::string> _name;
    mutable boost::optional<std::string> _dispName;
    const DataType::UP _dt;
    const SelectorRangeSet _selRanges;
    const MapItem::UP _userAttrs;
};

/*!
@brief
    Option of the type of data stream variants with an unsigned selector
    (VariantWithUnsignedIntegerSelectorType).

@ingroup metadata_dt
*/
using VariantWithUnsignedIntegerSelectorTypeOption = VariantTypeOption<unsigned long long>;

/*!
@brief
    Option of the type of data stream variants with a signed selector
    (VariantWithSignedIntegerSelectorType).

@ingroup metadata_dt
*/
using VariantWithSignedIntegerSelectorTypeOption = VariantTypeOption<long long>;

} // namespace yactfr

#endif // _YACTFR_METADATA_VAR_TYPE_OPT_HPP
