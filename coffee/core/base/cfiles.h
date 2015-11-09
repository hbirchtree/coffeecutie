#ifndef CFILES_H
#define CFILES_H

#include "coffee/core/coffee_basetypes.h"

namespace Coffee{
namespace CResources{

struct CResourceUrl
{
    enum ResourceFlags{
        /*! Resource is in a remote location, internet and etc.*/
        Remote      = 0x1,
        /*! Resource is read-only*/
        ReadOnly    = 0x2,
        /*! Resource is read-write*/
        ReadWrite   = 0x4,
        /*! Resource is write-only*/
        WriteOnly   = 0x8,

        /*! Resource is mapped or streamed*/
        Streaming   = 0x10,
    };

    CResourceUrl();

    uint8 flags;
    cstring url;
};

/*!
 * \brief A data resource which location cannot be changed.
 */
struct CResource{
    CResource(cstring rsrc = nullptr);

    uint8 flags; /*!< Resource flags*/
    szptr size; /*!< Data size*/
    void* data; /*!< Data pointer*/

    cstring resource() const;

private:
    CString m_resource; /*!< URL for the resource*/
};

extern bool coffee_file_exists(const CResource* resc);

extern void coffee_file_open(CResource* resc);
extern void coffee_file_close(CResource* resc);

/*!
 * \brief Memory map file as buffer
 * \return True if success
 */
extern bool coffee_file_memmap(CResource* resc);
/*!
 * \brief Unmap file
 * \return True if success
 */
extern bool coffee_file_memunmap(CResource* resc);

/*!
 * \brief Free data pointer
 */
extern void coffee_file_free(CResource* resc);
extern bool coffee_file_pull(CResource* resc, bool textmode = false);
/*!
 * \brief Save data to file
 * \return
 */
extern bool coffee_file_commit(const CResource* resc, bool append = false);
/*!
 * \brief Save data to file, append null-terminator
 * \return
 */
extern void coffee_file_commit_textmode(const CResource* resc, bool append = false);

/*!
 * \brief Create a directory
 * \param dirname
 * \param recursive Whether or not to create non-existent parent directories. Equal to the "-p" option for mkdir.
 * \return True if process succeeded
 */
extern bool coffee_file_mkdir(cstring dirname, bool recursive);

}
}

#endif // CFILES_H
