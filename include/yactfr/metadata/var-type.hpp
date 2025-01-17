/*
 * Copyright (C) 2015-2018 Philippe Proulx <eepp.ca>
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

#ifndef _YACTFR_METADATA_VAR_TYPE_HPP
#define _YACTFR_METADATA_VAR_TYPE_HPP

#include <algorithm>
#include <string>
#include <memory>
#include <utility>

#include "data-loc.hpp"
#include "var-type-opt.hpp"
#include "dt.hpp"
#include "dt-visitor.hpp"
#include "compound-dt.hpp"
#include "../aliases.hpp"

namespace yactfr {
namespace internal {

class TraceTypeImpl;

} // namespace internal

/*!
@brief
    Variant type.

@ingroup metadata_dt

An variant type describes data stream variants.
*/
class VariantType :
    public CompoundDataType
{
    friend class internal::TraceTypeImpl;

protected:
    explicit VariantType(_Kind kind, unsigned int minAlign, DataLocation&& selLoc,
                         MapItem::UP userAttrs);

public:
    /// Location of selectors of data stream variants described by this
    /// type.
    const DataLocation& selectorLocation() const noexcept
    {
        return _selLoc;
    }

    /// Types of selectors of data stream variants described by this
    /// type, or an empty set if this type is not part of a trace type
    /// yet.
    const DataTypeSet& selectorTypes() const noexcept
    {
        return _theSelTypes;
    }

protected:
    bool _isEqual(const DataType& other) const noexcept override
    {
        const auto& otherVariantType = static_cast<const VariantType&>(other);

        return CompoundDataType::_isEqual(other) && _selLoc == otherVariantType._selLoc;
    }

private:
    DataTypeSet& _selTypes() const noexcept
    {
        return _theSelTypes;
    }

private:
    const DataLocation _selLoc;
    mutable DataTypeSet _theSelTypes;
};

/*!
@brief
    Type of data stream optionals with an integer selector.

@ingroup metadata_dt
*/
template <typename SelectorValueT>
class VariantWithIntegerSelectorType :
    public VariantType
{
    friend class internal::TraceTypeImpl;

public:
    /// Type of options of this variant type.
    using Option = VariantTypeOption<SelectorValueT>;

    /// Vector of options of this variant type.
    using Options = std::vector<std::unique_ptr<const Option>>;

protected:
    explicit VariantWithIntegerSelectorType(const _Kind kind, const unsigned int minAlign,
                                            Options&& opts, DataLocation&& selLoc,
                                            MapItem::UP userAttrs) :
        VariantType {kind, minAlign, std::move(selLoc), std::move(userAttrs)},
        _opts {std::move(opts)}
    {
    }

public:
    /// Options offered by this type.
    const Options& options() const noexcept
    {
        return _opts;
    }

    /// Constant option iterator set at the first option of this type.
    typename Options::const_iterator begin() const noexcept
    {
        return _opts.begin();
    }

    /// Constant option iterator set \em after the last option of this
    /// type.
    typename Options::const_iterator end() const noexcept
    {
        return _opts.end();
    }

    /// Number of options this type has.
    Size size() const noexcept
    {
        return _opts.size();
    }

    /// Whether or not this type is empty.
    bool isEmpty() const noexcept
    {
        return _opts.empty();
    }

    /*!
    @brief
        Returns the option at the index \p index.

    @param[in] index
        Index of the option to return.

    @returns
        Option at the index \p index.

    @pre
        \p index < <code>size()</code>
    */
    const Option& operator[](const Index index) const noexcept
    {
        return *_opts[index];
    }

    /*!
    @brief
        Returns the option named \p name, or \c nullptr if not found.

    @param[in] name
        Name of the option to find.

    @returns
        Option named \p name, or \c nullptr if not found.
    */
    const Option *operator[](const std::string& name) const noexcept
    {
        const auto it = std::find_if(_opts.begin(), _opts.end(), [&name](const auto& opt) {
            return opt->name() && *opt->name() == name;
        });

        if (it == _opts.end()) {
            return nullptr;
        }

        return it->get();
    }

protected:
    Options _cloneOpts() const
    {
        Options opts;

        for (const auto& opt : _opts) {
            opts.push_back(opt->clone());
        }

        return opts;
    }

private:
    bool _isEqual(const DataType& other) const noexcept override
    {
        const auto& otherVariantType = static_cast<const VariantWithIntegerSelectorType&>(other);

        return VariantType::_isEqual(other) && this->_optsAreEqual(otherVariantType);
    }

    bool _optsAreEqual(const VariantWithIntegerSelectorType& other) const noexcept
    {
        if (_opts.size() != other.size()) {
            return false;
        }

        for (Index i = 0; i < _opts.size(); ++i) {
            if (*_opts[i] != *other._opts[i]) {
                return false;
            }
        }

        return true;
    }

private:
    Options _opts;
};

/*!
@brief
    Type of data stream variants with an unsigned integer selector.

@ingroup metadata_dt
*/
class VariantWithUnsignedIntegerSelectorType final :
    public VariantWithIntegerSelectorType<VariantWithUnsignedIntegerSelectorTypeOption::SelectorValue>
{
public:
    /// Unique pointer to constant type of data stream variants
    /// with an unsigned integer selector.
    using UP = std::unique_ptr<const VariantWithUnsignedIntegerSelectorType>;

public:
    /*!
    @brief
        Builds a type of data stream variants with an unsigned integer
        selector.

    @param[in] minimumAlignment
        Minimum alignment of data stream variants described by this
        type.
    @param[in] options
        Options of the variant type (moved to this type).
    @param[in] selectorLocation
        Location of selectors of data stream variants described by this
        type.
    @param[in] userAttributes
        @parblock
        User attributes of data stream variants described by this type.

        If set, each key of \p *userAttributes is a namespace.
        @endparblock

    @pre
        \p minimumAlignment > 0.
    @pre
        \p minimumAlignment is a power of two.
    @pre
        <code>!options.empty()</code>
    @pre
        \p selectorLocation only locates unsigned integers.
    @pre
        The options of \p options don't contain overlapping integer
        ranges between them.
    */
    explicit VariantWithUnsignedIntegerSelectorType(unsigned int minimumAlignment,
                                                    Options&& options,
                                                    DataLocation selectorLocation,
                                                    MapItem::UP userAttributes);

    /*!
    @brief
        Builds a type of data stream variants with an unsigned integer
        selector, having a default minimum alignment property (1).

    @param[in] options
        Options of the variant type (moved to this type).
    @param[in] selectorLocation
        Location of selectors of data stream variants described by this
        type.
    @param[in] userAttributes
        @parblock
        User attributes of data stream variants described by this type.

        If set, each key of \p *userAttributes is a namespace.
        @endparblock

    @pre
        <code>!options.empty()</code>
    @pre
        \p selectorLocation only locates unsigned integers.
    @pre
        The options of \p options don't contain overlapping integer
        ranges between them.
    */
    explicit VariantWithUnsignedIntegerSelectorType(Options&& options,
                                                    DataLocation selectorLocation,
                                                    MapItem::UP userAttributes);

    /*!
    @brief
        Creates a unique pointer to constant type of data stream
        variants with an unsigned integer selector, forwarding \p args
        to the constructor.

    @param[in] args
        Arguments to forward to the constructor of the type of data
        stream variants with an unsigned integer selector.

    @returns
        Created unique pointer to constant type of data stream variants
        with an unsigned integer selector.

    @pre
        See the preconditions of the constructor.
    */
    template <typename... ArgTs>
    static UP create(ArgTs&&... args)
    {
        return std::make_unique<UP::element_type>(std::forward<ArgTs>(args)...);
    }

private:
    DataType::UP _clone() const override;

    void _accept(DataTypeVisitor& visitor) const override
    {
        visitor.visit(*this);
    }
};

/*!
@brief
    Type of data stream variants with a signed integer selector.

@ingroup metadata_dt
*/
class VariantWithSignedIntegerSelectorType final :
    public VariantWithIntegerSelectorType<VariantWithSignedIntegerSelectorTypeOption::SelectorValue>
{
public:
    /// Unique pointer to constant type of data stream variants
    /// with a signed integer selector.
    using UP = std::unique_ptr<const VariantWithSignedIntegerSelectorType>;

public:
    /*!
    @brief
        Builds a type of data stream variants with a signed integer
        selector.

    @param[in] minimumAlignment
        Minimum alignment of data stream variants described by this
        type.
    @param[in] options
        Options of the variant type (moved to this type).
    @param[in] selectorLocation
        Location of selectors of data stream variants described by this
        type.
    @param[in] userAttributes
        @parblock
        User attributes of data stream variants described by this type.

        If set, each key of \p *userAttributes is a namespace.
        @endparblock

    @pre
        \p minimumAlignment > 0.
    @pre
        \p minimumAlignment is a power of two.
    @pre
        <code>!options.empty()</code>
    @pre
        \p selectorLocation only locates signed integers.
    @pre
        The options of \p options don't contain overlapping integer
        ranges between them.
    */
    explicit VariantWithSignedIntegerSelectorType(unsigned int minimumAlignment, Options&& options,
                                                  DataLocation selectorLocation,
                                                  MapItem::UP userAttributes);

    /*!
    @brief
        Builds a type of data stream variants with a signed integer
        selector, having a default minimum alignment property (1).

    @param[in] options
        Options of the variant type (moved to this type).
    @param[in] selectorLocation
        Location of selectors of data stream variants described by this
        type.
    @param[in] userAttributes
        @parblock
        User attributes of data stream variants described by this type.

        If set, each key of \p *userAttributes is a namespace.
        @endparblock

    @pre
        <code>!options.empty()</code>
    @pre
        \p selectorLocation only locates signed integers.
    @pre
        The options of \p options don't contain overlapping integer
        ranges between them.
    */
    explicit VariantWithSignedIntegerSelectorType(Options&& options, DataLocation selectorLocation,
                                                  MapItem::UP userAttributes);

    /*!
    @brief
        Creates a unique pointer to constant type of data stream
        variants with a signed integer selector, forwarding \p args
        to the constructor.

    @param[in] args
        Arguments to forward to the constructor of the type of data
        stream variants with a signed integer selector.

    @returns
        Created unique pointer to constant type of data stream variants
        with a signed integer selector.

    @pre
        See the preconditions of the constructor.
    */
    template <typename... ArgTs>
    static UP create(ArgTs&&... args)
    {
        return std::make_unique<UP::element_type>(std::forward<ArgTs>(args)...);
    }

private:
    DataType::UP _clone() const override;

    void _accept(DataTypeVisitor& visitor) const override
    {
        visitor.visit(*this);
    }
};

} // namespace yactfr

#endif // _YACTFR_METADATA_VAR_TYPE_HPP
