#pragma once

#include <coffee/components/subsystem.h>

#include <atomic>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <vector>
#include <url/url.h>

namespace rq {
class runtime_queue;
}

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
        ~RangedFile();

        std::uint64_t size() const
        {
            return m_size;
        }
        /*! Fills dst from offset. Blocks while the IO queue reads */
        bool read(std::uint64_t offset, gsl::span<char> dst);

      private:
        platform::url::Url        m_url;
        std::uint64_t             m_size{0};
        std::shared_ptr<Resource> m_mapping;
        FileMapper*               m_owner{nullptr};
        int                       m_fd{-1}; /*!< web: the copy in the store */
    };

    /*! A file still mapped by an earlier fetch is handed out again */
    std::future<std::shared_ptr<Resource>> fetch(platform::url::Url const& src);

    /*! On the web the file is first brought into the local store (OPFS),
     * downloaded in chunks on the IO queue unless the copy there already has
     * the server's size, and read from there. Elsewhere a view of the mapping */
    std::future<std::shared_ptr<RangedFile>> open_ranged(
        platform::url::Url const& src);

    /*! Like fetch(), but through the local store on the web: the bytes are
     * read into the heap from the cached copy, not held by Fetch */
    std::future<std::shared_ptr<Resource>> fetch_cached(
        platform::url::Url const& src);

    enum class fetch_mode
    {
        memory, /*!< The whole file in the heap */
        ranged, /*!< Read by range; on the web from the local store */
    };

    struct fetch_entry
    {
        platform::url::Url url;
        fetch_mode         mode{fetch_mode::memory};
        bool               optional{false}; /*!< Missing is not a failure */
        bool               cache{true};     /*!< Web: kept in the local store for the next run */
    };

    /*!
     * \brief Files fetched one after another, with progress over their
     * bytes. Results are handed out once each file has landed.
     */
    class FetchSet
    {
        friend class FileMapper;

      public:
        /*! Bytes in over bytes expected, 0 to 1 */
        float progress() const;
        /*! Every file has landed or failed */
        bool done() const;
        /*! A file that is not optional could not be had */
        bool failed() const;
        /*! Stops after the file in flight; nothing more is handed out */
        void cancel();
        bool cancelled() const;

        std::size_t size() const
        {
            return m_entries.size();
        }
        fetch_entry const& entry(std::size_t i) const
        {
            return m_entries[i];
        }
        std::shared_ptr<Resource>   resource(std::size_t i) const;
        std::shared_ptr<RangedFile> ranged(std::size_t i) const;

      private:
        struct slot_t
        {
            std::atomic<std::uint64_t>  received{0};
            std::atomic<std::uint64_t>  expected{0};
            std::atomic<bool>           settled{false};
            std::shared_ptr<Resource>   resource;
            std::shared_ptr<RangedFile> ranged;
        };

        std::vector<fetch_entry>           m_entries;
        std::unique_ptr<slot_t[]>          m_slots;
        std::atomic<bool>                  m_cancelled{false};
        std::atomic<bool>                  m_failed{false};
    };

    /*! Called on the delivery queue as file `i` lands; its result is null if
     * it could not be had */
    using ready_fn = std::function<void(FetchSet&, std::size_t i)>;

    /*!
     * \brief Fetches `entries` in order on the download queue, one at a time.
     * `on_ready` runs on `deliver_on` for each file as it lands, in order;
     * without a queue, on the download queue itself. A file still held from
     * an earlier fetch of the same URL and mode is handed out again, neither
     * downloaded nor read again
     */
    std::shared_ptr<FetchSet> fetch_set(
        std::vector<fetch_entry> entries, rq::runtime_queue* deliver_on, ready_fn on_ready);

    /*! The first `bytes` of a file, enough to tell what it is; fewer if it is
     * shorter. Read by range on the web, out of the mapping elsewhere */
    std::future<std::vector<char>> read_head(platform::url::Url const& src, std::size_t bytes);

    /*! The queue range reads run on; they block the caller, so it must
     * never wait on the caller's thread. Without one, range reads fail */
    void set_io_queue(rq::runtime_queue* queue);

    /*! The queue fetch_set() downloads on; without one, the IO queue */
    void set_download_queue(rq::runtime_queue* queue);

  private:
    rq::runtime_queue* io_queue();
    rq::runtime_queue* download_queue();
    void               fetch_one(FetchSet& set, std::size_t i);
    static std::shared_ptr<Resource> map_local(platform::url::Url const& src);
    /* A file of this URL some earlier fetch handed out and that is still held */
    std::shared_ptr<Resource>   live_resource(platform::url::Url const& src);
    std::shared_ptr<RangedFile> live_ranged(platform::url::Url const& src);
    void                        remember(std::shared_ptr<Resource> const& res);
    void                        remember(std::shared_ptr<RangedFile> const& file);

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

    std::mutex                                       m_lock; /*!< m_resources, m_ranged */
    std::map<std::string, std::weak_ptr<Resource>>   m_resources;
    std::map<std::string, std::weak_ptr<RangedFile>> m_ranged;
    std::map<platform::url::Url, ResourceCleanupData>     m_resource_cleaners;
    rq::runtime_queue*                                    m_io_queue{nullptr};
    rq::runtime_queue*                                    m_download_queue{nullptr};
};

} // namespace comp_app
