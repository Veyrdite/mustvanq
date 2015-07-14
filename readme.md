Master server replacement for Unvanquished.

Still under heavy development!  Only responds to a small subset of commands (see main.cpp) and probably suffers hardening issues for erroneous input.

# General
## Compiling and Running
Linux, no other dependancies.  Should work on other *nix systems if they allow [IPV6 sockets to also accept IPV4 data](https://en.wikipedia.org/w/index.php?title=IPv6&oldid=670296313#IPv4-mapped_IPv6_addresses).

 * make
 * ./mustvanq

To change the port number edit main.cpp.  Logging of all recieved data is made to stdout so you may want to redirect it to a file.

## Testing and Playing
By hand:
	netcat -u  <ip_of_master> <port_of_master>  # IPV4
	netcat -6u <ip_of_master> <port_of_master>  # IPV6

Remember that "localhost" is 127.0.0.1 in IPV4 and ::1 in IPV6.  Messages don't need a four byte header (see below) to be recognised.

In the game:
	/sv_master <ip_of_master>

## Why was this written?
The author wanted to make some small changes to the [existing master server](https://github.com/Unvanquished/unvanquished-master) but didn't feel confident working with the code.

## Errata
IPv6 support is not fully tested.  It may set fire to things or not work at all.


# Unvanquished Master Server protocol
The "master server" lives at a known ip address and keeps track of gameservers.  Clients use the master server to discover gameservers and gameservers send "heartbeats" to the master server to advertise themselves.

 * Much of this information applies to other quake-based games
 * Everything is UDP
 * All packets start with four FF characters (in C: -1 or "\xFF") as a header

The header is signify that the packets are out of band (OOB) to normal client-server communication.  [Daemon](https://wiki.unvanquished.net/index.php?title=Engine) uses and expects this for both incoming and outgoing messages to the master server.  For the rest of this guide this header is omitted for clarity.

Packet headers in daemon: comments at top of[unvanquished/src/engine/qcommon/net_chan.cpp](https://github.com/Unvanquished/Unvanquished/blob/master/src/engine/qcommon/net_chan.cpp)

OOB packet handling in daemon: void CL_ConnectionlessPacket() in [unvanquished/src/engine/client/cl_main.cpp](https://github.com/Unvanquished/Unvanquished/blob/master/src/engine/client/cl_main.cpp)




