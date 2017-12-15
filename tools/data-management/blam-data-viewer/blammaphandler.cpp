#include <blammaphandler.h>
#include <QString>

BlamMapHandler::BlamMapHandler(uchar* data) :
    m_map(data, version_t::pc)
{
}

bool BlamMapHandler::valid()
{
    return m_map.tags.index_magic != 0;
}

const char* BlamMapHandler::mapName()
{
    return file_header_full_mapname(m_map.map);
}

map_container &BlamMapHandler::container()
{
    return m_map;
}

bitm_texture_t BlamMapHandler::texture(uchar *bitm, const index_item_t* ref)
{
    const bitm_image_t* item = bitm_get(ref,m_map.map,
                                        m_map.tags.index_magic,nullptr);

    if(!item)
        return {};

    return bitm_get_texture(item,bitm);
}

void BlamMapHandler::allTextures(uchar *bitm, QVector<QImage> *images)
{
    bitm_texture_t tex;

    auto texture_processor = [&](index_item_t const* it)
    {
        tex = texture(bitm,it);

        if(!tex.data)
            return;

        if(tex.type == TexType::T2D)
        {
            QImage::Format fmt;
            switch(tex.format)
            {
            case PixCmp::RGBA:
                switch(tex.dformat)
                {
                case BitFormat::UIntR:
                    fmt = QImage::Format_RGBA8888;
                    break;
                default:
                    return;
                }
                break;
            default:
                return;
            }

            if(fmt != QImage::Format_RGBA8888)
                Q_ASSERT(false);

            images->append(QImage(
                               (uchar*)tex.data,
                               tex.resolution.width,
                               tex.resolution.height,
                               fmt));
        }
    };

    auto matcher = [](index_item_t const* it)
    {
        return tag_class_cmp(it->tagclass_e[0], tag_class_t::bitm);
    };

    tag_index_view index_view(m_map);

    auto it = index_view.begin();
    auto end = index_view.end();
    while(it != end)
    {
        it = std::find_if(it, end, matcher);
        if(it == end)
            break;

        texture_processor(*it);
        ++it;
    }
}

const char *BlamMapHandler::tagName(const index_item_t *item)
{
    return m_map.get_name(item);
}

QString BlamMapHandler::tagType(const index_item_t *item)
{
    std::string out;
    const char* str = item->tagclass[0];
    out.push_back(str[3]);
    out.push_back(str[2]);
    out.push_back(str[1]);
    out.push_back(str[0]);
    return QString::fromStdString(out);
}
