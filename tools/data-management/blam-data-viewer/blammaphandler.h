#include <qnamespace.h>
#include <QImage>
#include <QVector>
#include <coffee/blam/cblam.h>
#include <coffee/core/types/edef/resenum.h>

using namespace Coffee;
using namespace Coffee::Blam;

class BlamMapHandler
{
    uchar* m_data;

    file_header_t* m_file;
    const tag_index_t* m_index;
    tag_index_t* m_index_magical;

public:
    BlamMapHandler(uchar* data);
    ~BlamMapHandler();

    bool valid();

    const char* mapName();

    bitm_texture_t texture(uchar* bitm, const index_item_t* ref);

    void allTextures(uchar* bitm, QVector<QImage>* images, QVector<const index_item_t*> *tags);

    const char* tagName(const index_item_t* item);
    QString tagType(const index_item_t* item);
};
