#include <coffee/asio/asio_data.h>

namespace Coffee{
namespace CASIO{

thread_local ASIO_Client::AsioContext ASIO_Client::t_context = nullptr;

}
}
