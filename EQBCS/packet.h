#pragma once

#include <stdint.h>

#define MAX_BUF 32768
#define MAX_PACKET 1048576

typedef unsigned char packet_data_t[MAX_BUF];
typedef unsigned char packet_t[MAX_BUF];

void milisleep(uint64_t ms);
/*
typedef struct header_t {
	unsigned char packet_start;
	unsigned char packet_type;
	unsigned int packet_length;
};

typedef struct footer_t {
	unsigned char footer_data;
};

typedef struct packet_login {
	union {
		packet_data_t packet_data;
		struct
		{
			header_t header;
			unsigned char *username;
			unsigned char *password;
			footer_t footer;
		};
	};
};

typedef struct packet_message {
	union {
		packet_data_t packet_data;
		struct
		{
			header_t header;

			unsigned char username[64];
			unsigned char password[64];
			footer_t footer;
		};
	};
};
*/