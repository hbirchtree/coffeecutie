#ifndef CFILES_H
#define CFILES_H

#include "coffee.h"
#include "coffee/cregex.h"
#include "coffee/cdebug.h"
#include "plat\plat_core.h"

namespace Coffee{
namespace CResources{
namespace CFiles{

extern FILE* coffee_file_open(cstring fname, cstring mode);
extern szptr coffee_file_size(FILE* file);
extern cstring_w coffee_file_read(FILE* file, void* ptr, szptr *size, bool textmode = false);
extern bool coffee_file_write(FILE* file, const void* data, szptr size);
extern int coffee_file_close(FILE* file);

}

/*!
 * \brief A data resource which location cannot be changed.
 */
struct CResource{
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

    /*!
     * \brief Creates resource with location
     * \param resource Resource to use
     */
    CResource(cstring resource);

    /*!
     * \brief Get location of resource
     * \return
     */
    cstring resource();

    uint8 flags     = 0; /*! Resource flags*/
    szptr size      = 0; /*! Data size*/
    void* data      = nullptr; /*! Data pointer*/

    /*!
     * \brief Whether resource exists or not
     * \return True if exists
     */
    bool exists();
    /*!
     * \brief Read data from file if it exists
     * \param textmode Read as text, null-terminate data
     * \return True if success
     */
    bool read_data(bool textmode = false);
    /*!
     * \brief Append text to data with null-termination
     * \param text Text to append
     * \return True if success
     */
    bool append_text(cstring text);
    /*!
     * \brief Save data to file, overwrites previous contents
     * \return
     */
    bool save_data();
    /*!
     * \brief Free data pointer
     */
    void free_data();

    /*!
     * \brief Memory map file as buffer
     * \return True if success
     */
    bool memory_map();
    /*!
     * \brief Unmap file
     * \return True if success
     */
    bool memory_unmap();

private:
    void identify_resource();

    CString m_resource;
};

}
}

#endif // CFILES_H
