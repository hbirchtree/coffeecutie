#include <platforms/win32/detail.h>

#include <windows.h>

#include <hash-library/sha256.h>

namespace platform::file::win32 {

std::string detail::resource_hash(Url const& file)
{
    return "sha256/" + SHA256()(file.internUrl);
}

szptr detail::resource_length(win32_fd_t const& fd)
{
    return SizeofResource(nullptr, fd.resource);
}

std::optional<gsl::span<char>> detail::resource_data(win32_fd_t const& fd)
{
    auto resource = LoadResource(nullptr, fd.resource);
    auto ptr      = LockResource(resource);
    if(!ptr)
        return std::nullopt;
    return gsl::span<char>(static_cast<char*>(ptr), resource_length(fd));
}

result<detail::win32_fd_t, posix::posix_error> open_file(
    Url const& file, RSCA access, permissions_t perms)
{
    if(detail::is_resource(file, access))
    {
        auto hashed = detail::resource_hash(file);
        fprintf(stderr, "win32::open_file: %s -> %s: ",
            file.internUrl.c_str(), hashed.c_str());
        if(auto rsrc = FindResourceEx(nullptr, "CF_RES", hashed.c_str(), 1033))
        {
            fprintf(stderr, "FOUND\n");
            return detail::win32_fd_t{
                .resource = rsrc,
            };
        } else
            fprintf(stderr, "NOT FOUND\n");
    }
    if(auto res = posix::open_file(file, access, std::move(perms));
       res.has_value())
        return detail::win32_fd_t{
            .fd = std::move(res.value()),
        };
    else
        return res.error();
}

} // namespace platform::file::win32
