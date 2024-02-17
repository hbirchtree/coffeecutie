#pragma once

#include <coffee/components/subsystem.h>

#include <url/url.h>

namespace comp_app {

/*!
 * \brief FileMapper is recommended for the following cases:
 *  - A large file that needs to be loaded ad-hoc
 *  - A file that is potentially large
 *  - In environments where mmap() isn't necessarily available
 * So actually just Emscripten
 */
class FileMapper : public compo::SubsystemBase
{
  public:
    using type = FileMapper;

    class Resource
    {
        friend class FileMapper;

      public:
        ~Resource();
        operator semantic::BytesConst() const;

        template<typename T>
        explicit operator gsl::span<const T>() const
        {
            return static_cast<semantic::BytesConst>(*this).as<const T>().view;
        }

      protected:
        platform::url::Url         m_url;
        platform::file::map_handle m_mapping;
#if defined(COFFEE_EMSCRIPTEN)
        /* Emscripten needs to store a vector */
        std::vector<char> m_alloc;
#endif
    };

    void start_frame(compo::ContainerProxy&, const compo::time_point&);
    std::future<std::shared_ptr<Resource>> fetch(platform::url::Url const& src);

  private:
    struct ResourceCleanupData
    {
        /* Stash things that we need to potentially clean up a resource */
        struct
        {
            int handle{-1};
        } emscripten;

        struct
        {
            int fd{-1};
        } posix;
    };

    std::map<platform::url::Url, std::weak_ptr<Resource>> m_resources;
    std::map<platform::url::Url, ResourceCleanupData>     m_resource_cleaners;
};

} // namespace comp_app
