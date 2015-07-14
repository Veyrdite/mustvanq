#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "udpInterface.hpp"
#include "serverList.hpp"

#define PORT 27950
#define MOTD "Mustvanq experimental master server"

// Message decoding and response
void handleMessage( message_s * mess );
// Replaces nonprintable and extended chars with '.'
void cleanString( char * string );
// Like strncmp(a, b, len), but assumes len = length of a
int strnacmp( char * a, char * b );

/*
// Extract one word from a string, similiar to 'cut' in sh
char * getWord( int targetWordPos, char * data, char seperator );
// Search for a word in a string
int findWordPos( char * targetWord, char * data, char seperator );
*/


udpInterface_c udp;
serverList_c serverList;

int main()
{
	udp.setup( PORT );
	printf("Listening for UDP on port %d\n", PORT);

	while ( true )
	{
		message_s * mess = udp.getMessage();
		if ( mess != NULL ) handleMessage( mess );
		free( mess );
	}
	return 0;
}

void handleMessage( message_s * mess )
{
	// We duplicate the incoming mess so that the port and ip of the
	// sender are now used as the recipient
	message_s reply = *mess;

	// Decode and respond to requests
	char * explanation = NULL;
	{
		if ( strnacmp( "debug", mess->data ) == 0 )
		{
			explanation = "DEBUG";
			serverList.printServers();
		}

		else if ( strnacmp( "heartbeat Unvanquished-dead", mess->data ) == 0 )
		{
			explanation = "server death";
			serverList.removeServer( mess->ipversion, mess->ip, mess->port );
		}

		else if ( strnacmp( "heartbeat Unvanquished" , mess->data ) == 0 )
		{
			explanation = "server heartbeat";
			// TODO These 12 bytes are supposed to be a unique challenge id
			// Does daemon even care?
			strcpy( reply.data, "getinfo xxxxxxxxxxxx" );
			udp.sendMessage( &reply );
		}

		else if ( strnacmp( "infoResponse", mess->data ) == 0 )
		{
			explanation = "server challenge reply";

		}

		else if ( strnacmp( "getserversExt", mess->data ) == 0 )
		{
			explanation = "get server list, extended";
		}

		else if ( strnacmp( "getmotd", mess->data ) == 0 )
		{
			explanation = "master motd request";
			strcpy( reply.data, MOTD );
			udp.sendMessage( &reply );
		}

		else
		{
			explanation = "unknown";
		}
	}

	// Pretty printing
	{
		char * cleanedData = strdup( mess->data );
		char * prettifiedIP = prettyIP(mess->ipversion, mess->ip);
		assert( cleanedData != NULL );
		assert( prettifiedIP != NULL );
		cleanString( cleanedData );
		printf("%s %d (%s) %s\n", prettifiedIP, mess->port, explanation, cleanedData );
		free( prettifiedIP );
		free( cleanedData );
	}
}

void cleanString( char * string )
{
	int pos = 0;
	while (true)
	{
		char a = string[pos];
		if ( a == '\0' ) break;
		if ( a < 0x20 || a > 0x7E ) string[pos] = '.';
		pos++;
	}
}

int strnacmp( char * a, char * b )
{
	return strncmp( a, b, strlen( a ) );
}

/*
char * getWord( int targetWordPos, char * data, char seperator )
{
	if ( targetWordPos < 0 ) return NULL;

	static char outputWord[MAX_MESSAGE_LENGTH];
	int curWord = 0;       // current word in data
	int outputWordPos = 0; // current character position in outputWord[]

	for ( int dataPos=0; dataPos < MAX_MESSAGE_LENGTH; dataPos++ )
	{
		char a = data[dataPos];

		if ( curWord < targetWordPos )
		{
			// Hunting for start of word
			if ( a == seperator ) curWord++;
		}
		else
		{
			// Copying out current word
			if ( a == seperator || a == '\0' )
			{
				outputWord[outputWordPos] = '\0';
				return outputWord;
			}
			else
			{
				outputWord[outputWordPos] = a;
			}
			outputWordPos++;
		}

		if ( a == '\0' ) break;
	}

	// We have exhausted all characters and not
	// gained a desirable result
	return NULL;
}

int findWordPos( char * targetWord, char * data, char seperator )
{
	int pos = 0;
	while ( true )
	{
		char * word = getWord( pos, data, seperator );
		if ( word == NULL )
		{
			// We have exhaused the data and not found our word, or some
			// other error has been encountered by getWord()
			return -1;
		}
		else
		{
			if ( strcmp(targetWord, word) == 0 ) return pos;
		}
		pos++;
	}
}
*/
