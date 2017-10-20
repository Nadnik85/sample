#pragma once

#include <stdint.h>

#define MAX_BUF 32768
#define MAX_PACKET 32768

typedef unsigned char packet_t[MAX_PACKET];

void milisleep(uint64_t ms);
