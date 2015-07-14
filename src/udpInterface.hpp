#pragma once
#include "ipVersions.hpp"

#define MAX_MESSAGE_LENGTH 1024

struct message_s
{
	char data[MAX_MESSAGE_LENGTH];
	ipversion_e ipversion;
	unsigned char ip[MAX_IP_BYTELENGTH];
	unsigned int port;
};

class udpInterface_c
{
	public:
		void setup( int port );
		message_s * getMessage();
		void sendMessage( message_s * message );

	private:
		int mainSocket;
};

// Convert bytecode ip to nicely formatted string
char * prettyIP( ipversion_e ipversion, unsigned char * ip );
