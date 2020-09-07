
#include "samd51_port.h"

PortGroup* port_get_port_group(uint32_t pin) {
    EPortType port = g_APinDescription[pin].ulPort;
    return &PORT->Group[port];
}
