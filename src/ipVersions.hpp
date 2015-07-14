#pragma once

#define IPV4_ADDRESS_BYTELENGTH 4
#define IPV6_ADDRESS_BYTELENGTH 16
#define MAX_IP_BYTELENGTH IPV6_ADDRESS_BYTELENGTH

enum ipversion_e { IPV4, IPV6 };

// IPV4 ips can be represented as IPV6 addresses by setting:
// - first twelve bytes to this header (below)
// - last four bytes to the IPV4 address
//
// This is called "IPV4-mapped IPV6 addressing"
// https://en.wikipedia.org/w/index.php?title=IPv6&oldid=670296313#IPv4-mapped_IPv6_addresses
const unsigned char IPV4_IN_IPV6_HEADER[] = {0,0,0,0,0,0,0,0,0,0,255,255};
#define IPV4_IN_IPV6_STARTPOS 12
//TODO: replace above with functions


