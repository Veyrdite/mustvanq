/*
 * POSIX sockets are interesting things.  The manpages on my system are
 * mostly useless for beginners as they skip a lot of important detail.
 * There is also a million ways you can use and setup the sockets.
 *
 * This guide is a ridiculously useful resource:
 * http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html
 *
 * (Hales/Veyrdite 2015-07-09)
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include "udpInterface.hpp"

void udpInterface_c::setup( int port )
{
	// Traditonally you setup a sockaddr_in by hand to determine what
	// type of socket to use.  getaddrinfo() can do this for you
	addrinfo * netInfo;
	addrinfo hints;
	char portString[20];

	memset(&hints, 0, sizeof(hints) ); // blank hints.  Yes this is neccesary!
	hints.ai_family = AF_INET6;        // use IPV6 (IPv4 works as a subset, see ipVersions.hpp)
	hints.ai_socktype = SOCK_DGRAM;    // UDP
	hints.ai_flags = AI_PASSIVE;       // We don't need/want to know our own ip
	sprintf( portString, "%d", port);

	getaddrinfo( NULL, portString, &hints, &netInfo );
	if ( netInfo == NULL )
	{
		fprintf(stderr, "Error: couldn't find suitable address info.  Do we have network support?\n");
		exit(1);
	}

	mainSocket = socket( netInfo->ai_family, netInfo->ai_socktype, netInfo->ai_protocol);
	if ( mainSocket == -1 )
	{
		fprintf(stderr, "Error: can't open socket.  Is some other program already using it?\n");
		exit(1);
	}

	int b = bind( mainSocket, netInfo->ai_addr, netInfo->ai_addrlen);
	if ( b == -1 )
	{
		fprintf(stderr, "Error: can't bind socket.  Is some other program already using it?\n");
		exit(1);
	}
}

message_s * udpInterface_c::getMessage()
{
	char data[MAX_MESSAGE_LENGTH];
	sockaddr_in6 sender;

	// Recvfrom is a strange beast that needs a pointer to this length.
	unsigned int sockAddrLen = sizeof(sender);

	int len = recvfrom( mainSocket, data, MAX_MESSAGE_LENGTH, 0, (sockaddr*)&sender, &sockAddrLen );
	// recvfrom returns an invalid length upon an error
	if ( len > 0 )
	{
		// Manually terminate data string
		if ( data[len-1] == '\n' ) data[len-1] = '\0';
		else data[len] = '\0';

		message_s * result = (message_s *)malloc( sizeof( message_s ));
		assert( result != NULL );

		result->port = sender.sin6_port;
		memcpy( result->ip, sender.sin6_addr.s6_addr, IPV6_ADDRESS_BYTELENGTH );

		/*
		 * As our recieving socket has been set up as IPV6 all packets
		 * coming in will be marked as IPV6, even if they are actually
		 * IPV4.  We can detect the actual protocol by looking at the
		 * first twelve bytes of the IP address: see ipVersions.hpp
		 */
		if ( memcmp( sender.sin6_addr.s6_addr, IPV4_IN_IPV6_HEADER, sizeof(IPV4_IN_IPV6_HEADER)) == 0 )
			result->ipversion = IPV4;
		else
			result->ipversion = IPV6;

		// If the message came with a header, don't copy that
		if ( data[0] == '\xFF' && strlen( data ) >= 4 )
			strcpy( result->data, &(data[4]) );
		else
			strcpy( result->data, data );

		return result;
	}
	else
	{
		return NULL;
	}
}

void udpInterface_c::sendMessage( message_s * message )
{
	// Prepend header
	char textWithHeader[MAX_MESSAGE_LENGTH + 4];
	textWithHeader[0] = '\xFF';
	textWithHeader[1] = '\xFF';
	textWithHeader[2] = '\xFF';
	textWithHeader[3] = '\xFF';
	strcpy( &textWithHeader[4], message->data );

	// Manufacture destination socket
	sockaddr * destination;
	size_t destinationLength;

	switch ( message->ipversion )
	{
		case IPV4:
		{
			sockaddr_in temp;
			temp.sin_family = AF_INET;
			temp.sin_port = message->port;
			memcpy( &(temp.sin_addr.s_addr), &(message->ip[IPV4_IN_IPV6_STARTPOS]), IPV4_ADDRESS_BYTELENGTH );
			destination = (sockaddr *)&temp;
			destinationLength = sizeof( temp );
			break;
		}
		case IPV6:
		{
			sockaddr_in6 temp;
			temp.sin6_family = AF_INET6;
			temp.sin6_port = message->port;
			memcpy( temp.sin6_addr.s6_addr, message->ip, IPV6_ADDRESS_BYTELENGTH );
			//temp.sin6_scope_id = 0xe; // TODO test 0xe == global scope
			destination = (sockaddr *)&temp;
			destinationLength = sizeof( temp );
			break;
		}
	}

	int len = strlen( textWithHeader );
	sendto( mainSocket, textWithHeader, len, 0, destination, destinationLength );
}

char * prettyIP( ipversion_e ipversion, unsigned char * ip )
{
	char * result = (char *)malloc( INET6_ADDRSTRLEN );
	assert( result != NULL );

	switch ( ipversion )
	{
		case IPV4:
			inet_ntop( AF_INET, &(ip[IPV4_IN_IPV6_STARTPOS]), result, INET6_ADDRSTRLEN);
			break;
		case IPV6:
			inet_ntop( AF_INET6, ip, result, INET6_ADDRSTRLEN );
			break;
	}

	return result;
}

