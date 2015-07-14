#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "serverList.hpp"
#include "udpInterface.hpp"

void serverList_c::cullOldServers()
{
	time_t now = time(NULL);

	gameserver_s * current = firstServer;
	while ( current != NULL )
	{
		if ( current->lastHeartbeat + SERVER_LIFE < now )
		{
			gameserver_s * toRemove = current;
			current = current->next;
			delFromList( toRemove );
		}
		else
		{
			current = current->next;
		}
	}
}

void serverList_c::removeServer(  ipversion_e ipversion, unsigned char ip[MAX_IP_BYTELENGTH], unsigned int port )
{
	gameserver_s * current = firstServer;
	while ( current != NULL )
	{
		if ( areServersIdentical( current, ipversion, ip, port) )
		{
			gameserver_s * toRemove = current;
			current = current->next;
			delFromList( toRemove );
		}
		else
		{
			current = current->next;
		}
	}
}

void serverList_c::delFromList( gameserver_s * toRemove )
{
	if ( toRemove->prev == NULL )
		firstServer = toRemove->next;
	else
		toRemove->prev->next = toRemove->next;

	if ( toRemove->next != NULL )
		toRemove->next->prev = toRemove->prev;

	free( toRemove );
}

void serverList_c::printServers()
{
	gameserver_s * current = firstServer;
	while ( current != NULL )
	{
		char * prettifiedIP = prettyIP(current->ipversion, current->ip);
		printf("Server: ip %s port %d time %ld\n", prettifiedIP, current->port, current->lastHeartbeat );
		free( prettifiedIP);
		current = current->next;
	}
}

bool serverList_c::areServersIdentical( gameserver_s * server, ipversion_e ipversion, unsigned char ip[MAX_IP_BYTELENGTH], unsigned int port )
{
	if
	(
		port == server->port
		&& ipversion == server->ipversion
		&& memcmp( ip, server->ip, MAX_IP_BYTELENGTH ) == 0
	)
	return true;
	else return false;
}

void serverList_c::updateServer( ipversion_e ipversion, unsigned char ip[MAX_IP_BYTELENGTH], unsigned int port, bool empty, bool full )
{
	// Search the list for the current server
	gameserver_s * current = firstServer;
	while ( current != NULL )
	{
		if ( areServersIdentical( current, ipversion, ip, port) )
		{
			current->lastHeartbeat = time(NULL);
			return;
		}
		current = current->next;
	}

	// If we get this far then the server is not already in our list
	gameserver_s * newserver = (gameserver_s *)malloc(sizeof(gameserver_s));
	assert( newserver != NULL );
	newserver->port = port;
	newserver->ipversion = ipversion;
	newserver->empty = empty;
	newserver->full = full;
	memcpy( newserver->ip, ip, MAX_IP_BYTELENGTH );
	newserver->lastHeartbeat = time( NULL );

	newserver->next = firstServer;
	newserver->prev = NULL;
	firstServer = newserver;
}

