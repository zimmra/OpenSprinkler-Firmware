#pragma once

#if defined(OSPI_KS0212) && !defined(OSPI)
#error "OSPI_KS0212 requires the OSPI target"
#endif

#include "platform/gpio.h"

namespace ks0212 {

// BCM numbering, in station order. The KS0212 relays are active HIGH.
constexpr unsigned char pins[] = {4, 22, 6, 26};

inline void begin() {
	for (auto pin : pins) {
		pinMode(pin, OUTPUT); // Linux GPIO claims outputs with initial value LOW.
		digitalWrite(pin, LOW);
	}
}

inline void write(unsigned char stations, bool enabled) {
	for (unsigned char i = 0; i < sizeof(pins); ++i) {
		digitalWrite(pins[i], enabled && (stations & (1U << i)) ? HIGH : LOW);
	}
}

} // namespace ks0212
