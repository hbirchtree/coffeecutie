#include <atomic>
#include <stdint.h>

int main() {
    std::atomic<uint8_t> test_var;

    test_var.store(127);
    uint8_t exch = 127;
    test_var.compare_exchange_strong(exch, 0);

    return 0;
}
