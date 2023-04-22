#pragma once

#include <coffee/core/CFiles>
#include <coffee/core/datastorage/text/json/json.h>
#include <peripherals/semantic/chunk.h>

namespace Coffee {

struct settings_visitor
{
    virtual ~settings_visitor()
    {
    }

    /*!
     * \brief If called with <dirname>/<filename>, the following paths
     *  will be searched:
     *  - <dirname>/ALL.<type>.json
     *  - <dirname>/<filename>.<type>.json
     * \param basepath
     */
    void parse(Path const& basepath)
    {
        m_dirname  = basepath.dirname();
        m_basename = basepath.fileBasename();

        auto directoryDesc =
            (m_dirname + "ALL").addExtension(type()).addExtension("json");
        auto fileDesc = basepath.addExtension(type()).addExtension("json");

        auto directoryRsc = Resource(MkUrl(directoryDesc, RSCA::AssetFile));
        auto fileRsc      = Resource(MkUrl(fileDesc, RSCA::AssetFile));

        if(FileExists(directoryRsc))
            parse(C_OCAST<Bytes>(directoryRsc));
        if(FileExists(fileRsc))
            parse(C_OCAST<Bytes>(fileRsc));
    }

    void parse(Bytes&& data)
    {
        if(!data.data)
            return;

        auto doc = json::Read(data);

        for(auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it)
            visit(it->name.GetString(), it->value);
    }

    Path dirname()
    {
        return m_dirname;
    }
    Path fileBasename()
    {
        return m_basename;
    }

    virtual std::string type() = 0;

    virtual void visit(std::string const& name, json::Value const& value) = 0;

  private:
    Path m_dirname;
    Path m_basename;
};

} // namespace Coffee
