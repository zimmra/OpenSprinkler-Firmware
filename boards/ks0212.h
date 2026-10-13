#pragma once

#if defined(OSPI_KS0212) && !defined(OSPI)
#error "OSPI_KS0212 requires the OSPI target"
#endif

#include "platform/gpio.h"
#include <stdexcept>

namespace ks0212 {

// BCM numbering, in station order. The KS0212 relays are active HIGH.
constexpr unsigned char pins[] = {4, 22, 6, 26};

inline void all_off(unsigned char count = sizeof(pins)) {
	// Best effort only: a failed GPIO cannot be guaranteed off in software.
	for (unsigned char i = 0; i < count; ++i) gpio_write_checked(pins[i], LOW);
}

inline void begin() {
	for (unsigned char i = 0; i < sizeof(pins); ++i) {
		if (!gpio_claim_output_checked(pins[i], LOW)) {
			all_off(i);
			throw std::runtime_error("KS0212: failed to claim relay GPIO; controller stopped");
		}
		if (!gpio_write_checked(pins[i], LOW)) {
			all_off(i + 1);
			throw std::runtime_error("KS0212: failed to initialize relay LOW; controller stopped");
		}
	}
}

inline void write(unsigned char stations, bool enabled) {
	for (unsigned char i = 0; i < sizeof(pins); ++i) {
		if (!gpio_write_checked(pins[i], enabled && (stations & (1U << i)) ? HIGH : LOW)) {
			all_off();
			throw std::runtime_error("KS0212: relay GPIO write failed; controller stopped");
		}
	}
}

} // namespace ks0212
