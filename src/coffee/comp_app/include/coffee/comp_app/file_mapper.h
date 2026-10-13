#pragma once

#include <coffee/components/subsystem.h>

#include <condition_variable>
#include <cstdint>
#include <deque>
#include <functional>
#include <future>
#include <mutex>
#include <thread>
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

    /*!
     * \brief A file read by range instead of mapped. On the web the bytes
     * never enter the heap until read; elsewhere it is a view of the mapping.
     */
    class RangedFile
    {
        friend class FileMapper;

      public:
        std::uint64_t size() const
        {
            return m_size;
        }
        /*! Fills dst from offset. Blocks while the IO thread reads */
        bool read(std::uint64_t offset, gsl::span<char> dst);

      private:
        platform::url::Url        m_url;
        std::uint64_t             m_size{0};
        std::shared_ptr<Resource> m_mapping;
        FileMapper*               m_owner{nullptr};
    };

    ~FileMapper();

    /*! A file still mapped by an earlier fetch is handed out again */
    std::future<std::shared_ptr<Resource>> fetch(platform::url::Url const& src);

    std::future<std::shared_ptr<RangedFile>> open_ranged(
        platform::url::Url const& src);

  private:
    /*! Runs fn on the IO thread, started on first use */
    void run_io(std::function<void()>&& fn);

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

    std::map<std::string, std::weak_ptr<Resource>> m_resources;
    std::map<platform::url::Url, ResourceCleanupData>     m_resource_cleaners;

    std::thread                       m_io;
    std::mutex                        m_io_lock;
    std::condition_variable           m_io_wake;
    std::deque<std::function<void()>> m_io_jobs;
    bool                              m_io_quit{false};
};

} // namespace comp_app
