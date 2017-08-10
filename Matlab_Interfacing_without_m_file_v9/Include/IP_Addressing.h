/************************************************************************************
 * Copyright (C) 2013                                                               *
 * TETCOS, Bangalore. India                                                         *
 *                                                                                  *
 * Tetcos owns the intellectual property rights in the Product and its content.     *
 * The copying, redistribution, reselling or publication of any or all of the       *
 * Product or its content without express prior written consent of Tetcos is        *
 * prohibited. Ownership and / or any other right relating to the software and all  *
 * intellectual property rights therein shall remain at all times with Tetcos.      *
 *                                                                                  *
 * Author:    Shashi Kant Suman                                                     *
 *                                                                                  *
 * ---------------------------------------------------------------------------------*/
#ifndef _NETSIM_IPADDRESSING_H_
#define _NETSIM_IPADDRESSING_H_
#ifdef  __cplusplus
extern "C" {
#endif
#ifdef _WIN32
#else
	#include  "Linux.h"
// #define _declspec(dllexport) extern
#endif
#define _NETSIM_IP_LEN 50
#define MAX_TTL 255
typedef struct stru_ip* NETSIM_IPAddress;
struct stru_ip
{
	int type;
	union
	{
		struct ipv4
		{
			unsigned int byte1;
			unsigned int byte2;
			unsigned int byte3;
			unsigned int byte4;
		}IPV4;
		struct ipv6
		{
			unsigned int byte[8];
		}IPV6;
	}IP;
};
_declspec(dllexport) NETSIM_IPAddress STR_TO_IP(char* ipStr,int type);
_declspec(dllexport) int IP_TO_STR(NETSIM_IPAddress ip,char* ipStr);
_declspec(dllexport) NETSIM_IPAddress IP_COPY(NETSIM_IPAddress ip);
_declspec(dllexport) void IP_FREE(NETSIM_IPAddress ip);
_declspec(dllexport) NETSIM_IPAddress IP_COPY_FORCE(NETSIM_IPAddress ip);
_declspec(dllexport) void IP_FREE_FORCE(NETSIM_IPAddress ip);
_declspec(dllexport) int IP_COMPARE(const NETSIM_IPAddress ip1,const NETSIM_IPAddress ip2);
_declspec(dllexport) void IP_TO_BINARY(NETSIM_IPAddress ip,char* binary);
_declspec(dllexport) unsigned int IP_ADD_TO_LIST(NETSIM_IPAddress** pipList,unsigned int* count,const NETSIM_IPAddress ip);
_declspec(dllexport) unsigned int IP_REMOVE_FROM_LIST(NETSIM_IPAddress* pipList,unsigned int* count,const NETSIM_IPAddress ip);
_declspec(dllexport) int IP_CHECK_IN_LIST(const NETSIM_IPAddress* ipList,unsigned int count,const NETSIM_IPAddress ip);
_declspec(dllexport) int IP_IS_IN_SAME_NETWORK(NETSIM_IPAddress ip1,NETSIM_IPAddress ip2,NETSIM_IPAddress subnet,unsigned int prefix_len);
_declspec(dllexport) NETSIM_IPAddress IP_NETWORK_ADDRESS(NETSIM_IPAddress ip,NETSIM_IPAddress subnet,unsigned int prefix_len);

_declspec(dllexport) int IP_FREE_ALL(); //Free all IP. Used only by NetworkStack.dll after simulation
//Validate all configured ip. return 1 if all ip is valid else return 0
/** Very costly function. Use only for debugging **/
_declspec(dllexport) int IP_VALIDATE_ALL_IP();

#define STR_TO_IP4(ipstr) STR_TO_IP(ipstr,4)
#define STR_TO_IP6(ipstr) STR_TO_IP(ipstr,6)

#define IP_IS_IN_SAME_NETWORK_IPV4(ip1,ip2,subnet)	IP_IS_IN_SAME_NETWORK(ip1,ip2,subnet,0)
#define IP_IS_IN_SAME_NETWORK_IPV6(ip1,ip2,prefix)	IP_IS_IN_SAME_NETWORK(ip1,ip2,NULL,prefix)

#define IP_NETWORK_ADDRESS_IPV4(ip,subnet)	IP_NETWORK_ADDRESS(ip,subnet,0)
#define IP_NETWORK_ADDRESS_IPV6(ip,prefix)	IP_NETWORK_ADDRESS(ip,NULL,prefix)

#ifdef  __cplusplus
}
#endif
#endif
