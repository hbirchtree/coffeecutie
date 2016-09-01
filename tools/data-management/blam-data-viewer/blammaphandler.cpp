#include <blammaphandler.h>
#include <QString>

BlamMapHandler::BlamMapHandler(uchar* data) :
    m_data(data),
    m_file(nullptr),
    m_index(nullptr),
    m_index_magical(nullptr)
{
    m_file = blam_file_header_get(data,version_t::pc);
    if(!m_file)
        return;
    m_index = blam_tag_index_ptr(m_file);
    m_index_magical = new tag_index_t(*m_index);
    uchar* d1 = (uchar*)m_file;
    uchar* d2 = (uchar*)m_index;
    blam_tag_index_magic(m_index_magical,d2-d1);
}

bool BlamMapHandler::valid()
{
    return m_index_magical;
}

const char* BlamMapHandler::mapName()
{
    return blam_file_header_full_mapname(m_file);
}

bitm_texture_t BlamMapHandler::texture(uchar *bitm, const index_item_t* ref)
{
    const bitm_image_t* item = bitm_get(ref,m_file,m_index_magical->index_magic,nullptr);
    return bitm_get_texture(item,bitm);
}

void BlamMapHandler::allTextures(uchar *bitm, QVector<QImage> *images)
{
    const index_item_t* base = blam_tag_index_get_items(m_file);
    const index_item_t* it;
    bitm_texture_t tex;

    size_t i = 0;
    while((i++) < m_index->tagCount)
    {
        it = &base[i];
        if(blam_tagref_match_class(it,0,blam_index_item_type_bitm))
        {
            tex = texture(bitm,it);
            if(tex.type == TexType::T2D)
            {
                QImage::Format fmt;
                switch(tex.cformat)
                {
                case PixFmt::RGBA8:
                    fmt = QImage::Format_RGBA8888;
                    break;
                default:
                    continue;
                    fmt = QImage::Format_Mono;
                    break;
                }

                images->append(QImage(
                                   (uchar*)tex.data,
                                   tex.resolution.width,
                                   tex.resolution.height,
                                   fmt));
            }
        }
    }
}
