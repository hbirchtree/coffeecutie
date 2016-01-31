#ifndef COFFEE_TYPES_TRANSLATABLE_STRING_H
#define COFFEE_TYPES_TRANSLATABLE_STRING_H

#include <plat/plat_memory.h>
#include "basetypes.h"

namespace Coffee{

template<typename T1, typename T2> struct _cbasic_translation_locale;

template<typename T1,typename T2>
class _cbasic_translation_table
{
public:
    /*!
     * \brief Check if a specific locale is part of the table
     * \return
     */
    virtual bool is_language(_cbasic_translation_locale<T1,T2> const&) const = 0;

    /*!
     * \brief Return an array of all locales available.
     * \return
     */
    virtual _cbasic_translation_locale<T1,T2> const* get_locales() const = 0;

    /*!
     * \brief The default locale if we don't know what to do
     * \return
     */
    virtual _cbasic_translation_locale<T1,T2> const& get_default() const = 0;
    /*!
     * \brief If a locale has been chosen somehow, return it. If not, return default.
     * \return
     */
    virtual _cbasic_translation_locale<T1,T2> const& get_selected() const = 0;

    /*!
     * \brief Maps from an arbitrary/chosen identifier to a string that will be displayed. Should fall back to a default string if nothing is found.
     * \param src
     * \return
     */
    virtual CString const& lookup_string(T2 const& src) const = 0;
};

template<typename T1, typename T2>
struct _cbasic_translation_locale
{
    /*!
     * \brief Identifier for this specific locale
     */
    T1 identifier;
    /*!
     * \brief This locales parent table
     */
    _cbasic_translation_table<T1,T2> const* table;
};

template<typename T1, typename T2>
class _cbasic_translation_string
{
public:
    _cbasic_translation_string(T2 translatable):
        m_translateString(translatable)
    {
    }

    CString get_string(_cbasic_translation_locale<T1,T2> const& locale)
    {
        return locale.table->lookup_string(m_translateString);
    }

private:
    /*!
     * \brief The identifier for this specific translatable string
     */
    T2 m_translateString;
};

}

#endif
