#include <cassert>
#include <vector>
#include <utility>
#include "boards/ks0212.h"

static std::vector<std::pair<int, unsigned char>> writes;
static std::vector<int> claimed;
static int fail_claim = -1;
static int fail_write = -1;
bool gpio_claim_output_checked(int pin, unsigned char initial) {
    assert(initial == LOW);
    claimed.push_back(pin);
    return pin != fail_claim;
}
bool gpio_write_checked(int pin, unsigned char value) {
    writes.emplace_back(pin, value);
    return pin != fail_write;
}
int main() {
    ks0212::begin();
    assert((claimed == std::vector<int>{4, 22, 6, 26}));
    assert(writes.size() == 4);
    for (const auto& write : writes) assert(write.second == LOW);
    const unsigned char free_pins[] = PIN_FREE_LIST;
    for (auto pin : free_pins)
        for (auto relay : ks0212::pins) assert(pin != relay);
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
    for (auto pin : ks0212::pins) {
        fail_claim = pin;
        claimed.clear(); writes.clear();
        bool threw = false;
        try { ks0212::begin(); } catch (const std::runtime_error&) { threw = true; }
        assert(threw);
        assert(claimed.back() == pin);
        assert(writes.size() == 2 * (claimed.size() - 1));
        for (const auto& write : writes) assert(write.second == LOW && write.first != pin);
        fail_claim = -1;
        fail_write = pin;
        claimed.clear(); writes.clear(); threw = false;
        try { ks0212::begin(); } catch (const std::runtime_error&) { threw = true; }
        assert(threw && claimed.back() == pin);
        for (const auto& write : writes) assert(write.second == LOW);
        for (bool enabled : {false, true}) {
            writes.clear(); threw = false;
            try { ks0212::write(15, enabled); } catch (const std::runtime_error&) { threw = true; }
            assert(threw && writes.size() >= 5);
            for (unsigned int i = writes.size() - 4; i < writes.size(); ++i)
                assert(writes[i].second == LOW);
        }
        fail_write = -1;
    }
}
