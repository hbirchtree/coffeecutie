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

    map_container m_map;

public:
    BlamMapHandler(uchar* data);
    ~BlamMapHandler();

    bool valid();

    const char* mapName();
    map_container &container();

    bitm_texture_t texture(uchar* bitm, const index_item_t* ref);

    void allTextures(uchar* bitm, QVector<QImage>* images);

    const char* tagName(const index_item_t* item);
    QString tagType(const index_item_t* item);
};
