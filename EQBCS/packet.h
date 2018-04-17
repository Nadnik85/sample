#pragma once

#include <stdint.h>

#define MAX_BUF 32768
#define MAX_PACKET 1048576

typedef unsigned char packet_t[MAX_BUF];

void milisleep(uint64_t ms);
