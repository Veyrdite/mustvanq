#pragma once
#include <time.h>
#include "ipVersions.hpp"

#define SERVER_LIFE 600 // Seconds.  Daemon defaults to ~300


class serverList_c
{
	public:
		// Update or add the current server
		void updateServer( ipversion_e ipversion, unsigned char ip[MAX_IP_BYTELENGTH], unsigned int port, bool empty, bool full );
		void removeServer( ipversion_e ipversion, unsigned char ip[MAX_IP_BYTELENGTH], unsigned int port );
		// Remove servers that have not sent a heartbeat for a while
		void cullOldServers();
		// Format a serverlist for transmission
		void dumpServers( char * target );

		void printServers(); // for debugging

	private:
		struct gameserver_s
		{
			ipversion_e ipversion;
			unsigned char ip[MAX_IP_BYTELENGTH];
			unsigned int port;
			time_t lastHeartbeat;
			bool empty;
			bool full;

			gameserver_s * next;
			gameserver_s * prev;
		};
		gameserver_s * firstServer = NULL;
		void delFromList( gameserver_s * toRemove );
		bool areServersIdentical( gameserver_s * server, ipversion_e ipversion, unsigned char ip[MAX_IP_BYTELENGTH], unsigned int port );
};
