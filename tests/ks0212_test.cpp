#include <cassert>
#include <vector>
#include <utility>
#include "boards/ks0212.h"

static std::vector<std::pair<int, unsigned char>> writes;
static std::vector<int> claimed;
void pinMode(int pin, unsigned char mode) {
    assert(mode == OUTPUT);
    claimed.push_back(pin);
}
void digitalWrite(int pin, unsigned char value) {
    writes.emplace_back(pin, value);
}
int main() {
    ks0212::begin();
    assert((claimed == std::vector<int>{4, 22, 6, 26}));
    assert(writes.size() == 4);
    for (const auto& write : writes) assert(write.second == LOW);
    // Every bit pattern, including unused stations, and disabled controller.
    for (unsigned int mask = 0; mask < 256; ++mask) {
        for (bool enabled : {false, true}) {
            writes.clear();
            ks0212::write(static_cast<unsigned char>(mask), enabled);
            assert(writes.size() == 4);
            for (unsigned int i = 0; i < 4; ++i) {
                assert(writes[i].first == claimed[i]);
                assert(writes[i].second == (enabled && (mask & (1U << i)) ? HIGH : LOW));
            }
        }
    }
}
