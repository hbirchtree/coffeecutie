#pragma once

#include <peripherals/identify/compiler/function_inlining.h>
#include <peripherals/stl/type_safety.h>

#include <string>
#include <system_error>
#include <type_traits>

namespace stl_types {

using error_category = std::error_category;

struct error_code : std::error_code
{
    std::string error_message;

    error_code& operator=(std::string error_msg)
    {
        this->error_message = error_msg;
        return *this;
    }

    void reassign(int error_code)
    {
        assign(error_code, category());
    }

    FORCEDINLINE std::string message() const
    {
        auto msg = std::error_code::message();
        return error_message.size() ? msg + ": " + error_message : msg;
    }
};

struct sentinel_error_code
{
};

template<typename DomainError, typename ErrorCategory>
struct domain_error_code : error_code
{
    using error_type    = DomainError;
    using category_type = ErrorCategory;

    ErrorCategory m_category;

    FORCEDINLINE domain_error_code()
    {
        assign(0, m_category);
    }

    using error_code::operator=;

    FORCEDINLINE domain_error_code& operator=(domain_error_code&& other)
    {
        error_message = std::move(other.error_message);
        return *this;
    }

    FORCEDINLINE domain_error_code& operator=(DomainError error_code)
    {
        assign(C_CAST<int>(error_code), m_category);
        return *this;
    }

    FORCEDINLINE bool operator!=(DomainError error)
    {
        return C_CAST<int>(error) != value();
    }

    FORCEDINLINE bool operator==(DomainError error)
    {
        return C_CAST<int>(error) == value();
    }

    template<
        typename TDomain,
        typename TErrorCategory,
        typename std::enable_if<
            std::is_same<TDomain, DomainError>::value &&
                std::is_same<TErrorCategory, ErrorCategory>::value,
            TDomain>::type* = nullptr>
    FORCEDINLINE domain_error_code& to_domain_deref()
    {
        return *this;
    }
};

template<typename DomainError, typename ErrorCategory, typename NestedError>
struct nested_domain_error_code : error_code
{
    using nested_error_type = NestedError;
    using error_type        = DomainError;
    using category_type     = ErrorCategory;

    ErrorCategory m_category;
    NestedError   m_internalError;

    FORCEDINLINE nested_domain_error_code()
    {
        assign(0, m_category);
    }

    FORCEDINLINE nested_domain_error_code(nested_domain_error_code&& other)
        : m_category(std::move(other.m_category))
        , m_internalError(std::move(other.m_internalError))
    {
        error_message = std::move(other.error_message);
    }

    template<
        typename Dummy = int,
        typename std::enable_if<
            !std::is_same<NestedError, sentinel_error_code>::value,
            Dummy>::type* = nullptr>
    FORCEDINLINE std::string message() const
    {
        if(m_internalError)
            return error_code::message() + ": " + m_internalError.message();
        else
            return error_code::message();
    }

    using error_code::operator=;

    FORCEDINLINE nested_domain_error_code& operator=(
        nested_domain_error_code&& other)
    {
        m_category      = std::move(other.m_category);
        m_internalError = std::move(other.m_internalError);
        error_message   = std::move(other.error_message);
        return *this;
    }

    FORCEDINLINE nested_domain_error_code& operator=(DomainError error_code)
    {
        assign(C_CAST<int>(error_code), m_category);
        return *this;
    }

    FORCEDINLINE nested_domain_error_code& operator=(NestedError&& error_code)
    {
        m_internalError = std::move(error_code);
        return *this;
    }

    FORCEDINLINE bool operator!=(DomainError error)
    {
        return C_CAST<int>(error) != value();
    }

    FORCEDINLINE bool operator==(DomainError error)
    {
        return C_CAST<int>(error) == value();
    }

    template<typename TargetError>
    requires type_safety::is_specialized<TargetError, domain_error_code>::value
    FORCEDINLINE TargetError& as()
    {
        return to_domain_deref<
            typename TargetError::error_type,
            typename TargetError::category_type>();
    }

    template<
        typename TDomain,
        typename TErrorCategory,
        typename std::enable_if<
            std::is_same<TDomain, error_type>::value,
            TDomain>::type* = nullptr,
        typename std::enable_if<
            std::is_same<TErrorCategory, category_type>::value,
            TErrorCategory>::type* = nullptr>
    /*!
     * \brief Casting to domain_error_code of similar type, no nesting
     */
    FORCEDINLINE domain_error_code<TDomain, TErrorCategory>& to_domain_deref()
    {
        using out_type = domain_error_code<TDomain, TErrorCategory>;
        return *C_RCAST<out_type*>(this);
    }

    template<
        typename TDomain,
        typename TErrorCategory,
        typename std::enable_if<
            !std::is_same<TDomain, error_type>::value ||
                !std::is_same<TErrorCategory, category_type>::value,
            TDomain>::type* = nullptr>
    /*!
     * \brief If casting from the current type doesn't work, try the nested type
     * This works recursively
     */
    FORCEDINLINE domain_error_code<TDomain, TErrorCategory>& to_domain_deref()
    {
        return m_internalError
            .template to_domain_deref<TDomain, TErrorCategory>();
    }
};

template<typename NestedError>
struct nested_empty_error_code
{
    using nested_error_type = NestedError;
};

} // namespace stl_types
