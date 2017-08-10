/************************************************************************************
 * Copyright (C) 2013                                                               *
 * TETCOS, Bangalore. India                                                         *
 *                                                                                  *
 * Tetcos owns the intellectual property rights in the Product and its content.     *
 * The copying, redistribution, reselling or publication of any or all of the       *
 * Product or its content without express prior written consent of Tetcos is        *
 * prohibited. Ownership and / or any other right relating to the software and all *
 * intellectual property rights therein shall remain at all times with Tetcos.      *
 *                                                                                  *
 * Author:    Shashi Kant Suman                                                       *
 *                                                                                  *
 * ---------------------------------------------------------------------------------*/
#ifndef _NETSIM_STACK_H_
#define _NETSIM_STACK_H_
#ifdef  __cplusplus
extern "C" {
#endif
#include "List.h"

#ifdef _WIN32
#ifdef _NETSIM_CODE_
#define EXPORTED _declspec(dllexport)
#else
#define EXPORTED _declspec(dllimport)
#endif
#ifdef _NETSIM_METRIC_CODE_
#undef EXPORTED
#define EXPORTED
#endif
#else
#ifdef _NETSIM_CODE_
#define EXPORTED
#else
#define EXPORTED extern
#endif
#ifdef _NETSIM_METRIC_CODE_
#undef EXPORTED
#define EXPORTED
#endif

#endif
EXPORTED int nContinueFlag;

//Time Definition
#define SECOND		1000000
#define MILLISECOND	1000
#define MICROSECOND	1
#define NANOSECOND	0.001


/* Enumeration for flag*/
enum enum_Flag
{
	ZERO,
	ONE,
};
#ifndef __cplusplus
enum enum_bool
{
	false,
	true,
};
#endif

/* Enumeration for Layer type*/
enum enum_LayerType
{
	LAYER_NULL=0,
	PHYSICAL_LAYER = 1, //Interface layer in linux
	MAC_LAYER,			//Protocol layer in linux
	NETWORK_LAYER,		//Socket layer in linux
	TRANSPORT_LAYER,	//Socket layer in linux
	APPLICATION_LAYER,	//Process layer in linux
};
/* Enumeration for event type*/
enum enum_EventType
{
	PHYSICAL_OUT_EVENT = PHYSICAL_LAYER*10+1,
	PHYSICAL_IN_EVENT = PHYSICAL_LAYER*10+2,
	MAC_OUT_EVENT = MAC_LAYER*10+1,
	MAC_IN_EVENT = MAC_LAYER*10+2,
	NETWORK_OUT_EVENT = NETWORK_LAYER*10+1,
	NETWORK_IN_EVENT = NETWORK_LAYER*10+2,
	TRANSPORT_OUT_EVENT	= TRANSPORT_LAYER*10+1,
	TRANSPORT_IN_EVENT = TRANSPORT_LAYER*10+2,
	APPLICATION_OUT_EVENT =	APPLICATION_LAYER*10+1,
	APPLICATION_IN_EVENT = APPLICATION_LAYER*10+2,
	TIMER_EVENT = 1,
	INTERNAL_NETSIM_EVENT=2,
};

/* Enumeration for Application layer protocol*/
enum enum_AppProtocol
{
	APP_PROTOCOL_RIP = APPLICATION_LAYER*100+1,//=501,
	APP_PROTOCOL_OSPF,
	APP_PROTOCOL_BGP,
};

/* Enumeration for transport layer protocol*/
enum enum_TrxProtocol
{
	TX_PROTOCOL_TCP = TRANSPORT_LAYER*100+1,
	TX_PROTOCOL_UDP,
};

/* Enumeration for network layer protocol*/
enum enum_NWProtocol
{
	/* IP protocol*/
	NW_PROTOCOL_IPV4 = NETWORK_LAYER*100+1,
	NW_PROTOCOL_IPV6,
	NW_PROTOCOL_ARP,
	NW_PROTOCOL_MPLS, //2.5 layer protocol

	/* Routing Protocol*/
	NW_PROTOCOL_DSR,
	NW_PROTOCOL_AODV,
	NW_PROTOCOL_ZRP,
	NW_PROTOCOL_OLSR,
};

/* Enumeration for MAC layer protocol*/
enum enum_MACProtocol
{
	/***WIRELESS***/
	MAC_PROTOCOL_IEEE802_11 = MAC_LAYER*100+1,	//WLAN
	MAC_PROTOCOL_IEEE802_15_4,		//Zigbee
	MAC_PROTOCOL_IEEE802_16,		//Wi-Max
	MAC_PROTOCOL_IEEE802_22,		//Cognitive Radio

	/***WIRED***/
	MAC_PROTOCOL_IEEE802_3,			//Ethernet
	MAC_PROTOCOL_ATM,				//ATM
	MAC_PROTOCOL_P2P,				//Point to point link

	/***Cellular Protocol***/
	MAC_PROTOCOL_CDMA,				//CDMA
	MAC_PROTOCOL_GSM,				//GSM
	MAC_PROTOCOL_LTE,				//LTE
	MAC_PROTOCOL_ADVANCE_LTE,		//Advance LTE

	/***/
	MAC_PROTOCOL_TDMA,				//TDMA
	MAC_PROTOCOL_DTDMA,				//Dynamic TDMA
};

///Internal module
#define PROTOCOL_METRICS		2
#define PROTOCOL_MOBILITY		3
#define PROTOCOL_APPLICATION	4

/*Enumeration for physical medium type. Must be compatible with MAC layer*/
enum enum_PhyMedium
{
	PHY_MEDIUM_NULL = 0,
	PHY_MEDIUM_WIRELESS = 1,
	PHY_MEDIUM_WIRED = 2,
};



/*Enumeration for interface type. Must be compatible with network layer, MAC layer and physical medium*/
enum enum_InterfaceType
{
	INTERFACE_NULL=0,
	INTERFACE_LAN_802_3=1,	//Ethernet
	INTERFACE_LAN_802_11,	//WLAN b	
	INTERFACE_LAN_802_15_4,	//Zigbee
	INTERFACE_LAN_802_22,	//Cognitive radio
	INTERFACE_WAN_ROUTER,	//WAN
	INTERFACE_WAN_ATM,		//ATM
	INTERFACE_VIRTUAL,		//Virtual interface
	INTERFACE_GSM,			//GSM Interface
	INTERFACE_CDMA,			//CDMA Interface
	INTERFACE_LTE,			//LTE Interface
	INTERFACE_ADVANCE_LTE,	//Advance LTE Interface
	INTERFACE_WIMAX,		//Wi-Max Interface
};

/* Enumeration for device type*/
enum enum_DeviceType
{
	Not_Device=0,
	/* Node 5-layer device */
	NODE=1,			//Wired or wireless
	CPE=1,			//Wired
	MOBILESTATION=1,//Wireless
	UE=1,			//Wireless LTE user equipment
	SUBSCRIBER=1,	//wireless
	SENSOR=1,		//Wireless
	SINKNODE=1,		//Wireless
	PANCOORDINATOR=1,	//Wireless

	/***Intermediate Device***/
	/* 2-layer device*/
	SWITCH=2,		//Wired
	FRSWITCH=2,		//Wired
	ATMSWITCH=2,	//Wired
	X25SWITCH=2,	//Wired
	HUB=3,			//Wired
	BASESTATION=4,	//Wireless
	eNB=4,			//Wireless LTE base station
	ACCESSPOINT=5,	//Wireless
	/*3-Layer device*/
	ROUTER=6,		//Wired and/or wireless
	MME=6,			//LTE Mobile management entity

	MSC=8,				//Mobile Switching center
};

/* Enumeration for link type */
enum enum_LinkType
{
	LinkType_NULL=0,
	LinkType_P2P=1,	//Point to point
	LinkType_P2MP=2,//Point to multipoint
	LinkType_MP2MP=3,//multipoint to multipoint
};

/* Enumerator for link mode */
enum enum_LinkMode
{
	LinkMode_NULL=0,
	LinkMode_HalfDuplex=1,
	LinkMode_FullDuplex=2,
};

/* Enumeration for Simulation exit type*/
enum enum_SimulationEndType
{
	SimEndType_TIME=1,			//End simulation after specified time
	SimEndType_NUMOFEVENT,		//End simulation after specified number of event execution
	SimEndType_NUMOFPACKET,	//End simulation after specified number of packet transfered
};


/*Enumeration for scheduling type*/
enum enum_SchedulingType
{
	SCHEDULING_NONE=0,
	SCHEDULING_FIFO=1,
	SCHEDULING_PRIORITY=2,
	SCHEDULING_ROUNDROBIN=3,
	SCHEDULING_WFQ=4,
};
/*Enumeration for traffic type*/
enum enum_ApplicationType
{
	TRAFFIC_NULL=0,
	TRAFFIC_DATA=1,
	TRAFFIC_CBR,
	TRAFFIC_VOICE,
	TRAFFIC_VIDEO,
	TRAFFIC_FTP,
	TRAFFIC_HTTP,
	TRAFFIC_DATABASE,
	TRAFFIC_CUSTOM,
	TRAFFIC_EMAIL,
	TRAFFIC_PEER_TO_PEER,
	TRAFFIC_ERLANG_CALL,
	TRAFFIC_EMULATION,
	TRAFFIC_SENSING,
};

/* enumeration for wireshark option */
enum emum_wireshark_option
{
	wireshark_disable=0,
	wireshark_online,
	wireshark_offline,
};

/*enumeration for Node action */
typedef enum enum_NodeAction
{
	NOT_CONNECTED,
	CONNECTED,
	JOIN,
	LEAVE,	
}NODE_ACTION;

/* Structure of mac address*/
typedef struct stru_macaddress
{
	unsigned long long int nmacaddress;
	char szmacaddress[13];
}NETSIM_MACADDRESS,*PNETSIM_MACADDRESS;

EXPORTED PNETSIM_MACADDRESS BROADCAST_MAC;

/*Coordinate of point in 3-d space*/
struct stru_NetSim_Coordinates
{
	bool ismap;
	double lat;
	double lon;
	double X;
	double Y;
	double Z;
};

/*Store the simulation parameter*/
struct stru_NetSim_SimulationParameter
{
	SIMULATION_END_TYPE nSimulationExitType;//Simulation exit type
	double dVal;	//Value at which simulation end occurs for a particular simulation end type.
					//For example Time = 100000 Sec.
};

/* Stores the application layer of device*/
struct stru_NetSim_ApplicationLayer
{
	LAYER_TYPE nLayerType; //Layer type
	APPLICATION_LAYER_PROTOCOL nAppRoutingProtocol; //Application layer Routing protocol like RIP
	void* routingVar;	//Routing variable
	int (*fnDeviceAppLayer)();
};

/* Stores the transport layer of device*/
struct stru_NetSim_TransportLayer
{
	unsigned int isTCP:1;	//Flag to enable or disable TCP
	unsigned int isUDP:1;	//Flag to enable or disable UDP;
	LAYER_TYPE nLayerType; //Layer type	
	double dMSS; //Updated by tx layer protocol
	void* TCPVar; //Pointer for TCP variable
	void* UDPVar;//Pointer for UDP variable
};

/*Stores the network layer of device*/
struct stru_NetSim_NetworkLayer
{
	LAYER_TYPE nLayerType; //Layer type
	NETWORK_LAYER_PROTOCOL nRoutingProtocolId; //Routing protocol id
	void* RoutingVar; //Routing variable
	int (*routerFunction)(); //Routing function used by app layer routing protocol
							 // like RIP, OSPF	
	void* ipRoutingTables;	//IP routing tables
	void* ipVar; //IP configuration variable
};

/*Stores the MAC layer of device*/
struct stru_NetSim_MacLayer
{
	LAYER_TYPE nLayerType;	//Layer type
	MAC_LAYER_PROTOCOL nMacProtocolId;	//Protocol id
	PNETSIM_MACADDRESS szMacAddress; //MAC address of the device
	void* MacVar;	//MAC protocol variable
};

/*Stores the physical layer of device*/
struct stru_NetSim_PhysicalLayer
{
	LAYER_TYPE nLayerType; //Layer type
	PHYSICAL_LAYER_MEDIUM nPhyMedium;	//Physical medium type. Must be compatible with mac layer
	NETSIM_ID nLinkId;	//Connected link id
	struct stru_NetSim_Links* pstruNetSimLinks; //Pointer to connected link
	void* phyVar; //Phy layer variable
	double dLastPacketEndTime; //Last packet end time
};

/* Stores the buffer information of device*/
struct stru_NetSim_Device_Buffer
{
	double dMaxBufferSize;	//Max buffer size in bytes
	double dCurrentBufferSize;	//Current buffer size in bytes
	SCHEDULING_TYPE nSchedulingType;	//Scheduling type
	unsigned int nQueuedPacket;
	unsigned int nDequeuedPacket;
	unsigned int nDroppedPacket;
	struct stru_NetSim_Packet* pstruPacketlist;	//Packet list
	struct stru_NetSim_Packet* last;
};
/*Store the socket interface of device*/
struct stru_NetSim_SocketInterface
{
	unsigned int nSocketCount;
	struct stru_NetSim_Device_Buffer** pstruSocketBuffer; //Socket buffer
};

/* Stores the MAC-NW interface of device*/
struct stru_NetSim_Access_Interface
{
	struct stru_NetSim_Device_Buffer* pstruAccessBuffer; //MAC-NW buffer
};

/* structure for node join profile */
typedef struct stru_NodeJoinProfile
{
	NETSIM_ID NodeId;
	double time;
	NODE_ACTION action;
	_ele* ele;
}NODE_JOIN_PROFILE,*PNODE_JOIN_PROFILE;
#define JOIN_PROFILE_ALLOC()				(PNODE_JOIN_PROFILE)list_alloc(sizeof(NODE_JOIN_PROFILE),offsetof(NODE_JOIN_PROFILE,ele))
#define JOIN_PROFILE_NEXT(profile)			profile=(PNODE_JOIN_PROFILE)LIST_NEXT(profile)
#define JOIN_PROFILE_ADD(lprofile,profile)	LIST_ADD_LAST((void**)lprofile,profile)

/* stores the links information*/
struct stru_NetSim_Links
{
	NETSIM_ID nConfigLinkId; //Link id setted in config file
	NETSIM_ID nLinkId;
	LINK_TYPE nLinkType;
	PHYSICAL_LAYER_MEDIUM nLinkMedium;
	LINK_MODE nLinkMode;
	union uni_NetSim_MediumProp
	{
		struct stru_NetSim_WiredLink
		{
			double dDistance;
			double dPropagationDelayUp;
			double dDataRateUp;
			double dErrorRateUp;
			double dPropagationDelayDown;
			double dDataRateDown;
			double dErrorRateDown;
		}pstruWiredLink;
		struct stru_NetSim_wirelessLink
		{
			double dDataRateUp;
			double dDataRateDown;
			double dFrequency;
			double dPathLossExponent;
			double dStandardDeviation;
			double dFadingFigure;
		}pstruWirelessLink;
	}puniMedProp;
	union uni_NetSim_Link_DeviceList
	{
		struct stru_NetSim_P2P
		{
			NETSIM_ID nFirstDeviceId;
			NETSIM_ID nFirstInterfaceId;
			NETSIM_ID nSecondDeviceId;
			NETSIM_ID nSecondInterfaceId;
		}pstruP2P;
		struct stru_NetSim_P2MP
		{
			int nConnectedDeviceCount;//Count of connected device through this wireless link
			NETSIM_ID nCenterDeviceId;
			NETSIM_ID nCenterInterfaceId;
			NETSIM_ID* anDevIds;	//List of all the device belong to this wireless medium
			NETSIM_ID* anDevInterfaceIds;
		}pstrup2MP;
		struct stru_NetSim_MP2MP
		{
			NETSIM_ID nConnectedDeviceCount;//Count of connected device through this wireless link
			NETSIM_ID* anDevIds;	//List of all the device belong to this shared medium
			NETSIM_ID* anDevInterfaceIds;
		}pstruMP2MP;
	}puniDevList;
	struct stru_NetSim_Links* pstruNextLink;
	struct stru_NetSim_Links* pstruPreviousLink;
};
/*Stores the interface information of device*/
struct stru_NetSim_Interface
{
	NETSIM_ID nInterfaceId; //Interface id. Unique for each interface for a device.
	NETSIM_ID nConfigInterfaceId;
	INTERFACE_TYPE nInterfaceType; //Interface type
	NETWORK_LAYER_PROTOCOL nProtocolId; //IP protocol id
	NETSIM_IPAddress szAddress; //IP address of the interface
	NETSIM_IPAddress szSubnetMask; //Subnet mask of the interface in case of IPV4
	unsigned int prefix_len; //Prefix length in case of IPV6
	NETSIM_IPAddress szDefaultGateWay; //Default gateway
	void* ipVar;
	void* interfaceVar;	//Variable used to store the common interface variable.
	NETWORK_LAYER_PROTOCOL nLocalNetworkProtocol;
	struct stru_NetSim_Access_Interface* pstruAccessInterface; //Network-MAC interface
	struct stru_NetSim_MacLayer* pstruMACLayer; //MAC layer of interface
	struct stru_NetSim_PhysicalLayer* pstruPhysicalLayer; //Physical layer of interface
	struct stru_NetSim_Interface* pstruNextInterface;
	struct stru_NetSim_Interface* pstruPrevInterface;
};

#define SEED_COUNT 2
/* Stores the device information in network*/
struct stru_NetSim_Device
{
	NETSIM_ID nConfigDeviceId; //Device id setted in the config file
	NETSIM_ID nDeviceId;		//Unique id for all type of device. Auto assigned by backend.This is mapped to device name from frontend
	netsimDEVICE_TYPE nDeviceType;	//Type of device
	NETSIM_Name type;
	NETSIM_Name szDeviceName;			//Name of the device in GUI
	unsigned int nNumOfInterface;	//Number of interface in device
	unsigned long ulSeed[SEED_COUNT];//Unique seed
	void* deviceVar;	//Variable used to store to device specific variable. Common for all interface
	struct stru_NetSim_Interface** ppstruInterfaceList; //Interface list
	struct stru_NetSim_ApplicationLayer* pstruApplicationLayer;//Application layer of device
	struct stru_NetSim_SocketInterface* pstruSocketInterface; //Socket interface of device. 
	struct stru_NetSim_TransportLayer* pstruTransportLayer;	//Transport layer of device
	struct stru_NetSim_NetworkLayer* pstruNetworkLayer; //Network layer of device
	struct stru_NetSim_Coordinates* pstruDevicePosition;	//Position of device in simulation workspace
	struct stru_NetSim_Mobility* pstruDeviceMobility;		//Mobility information of mobile device
	WIRESHARK_OPTION wireshark_option;
	PNODE_JOIN_PROFILE node_join_profile;
	NODE_ACTION node_status;
	struct stru_NetSim_Device* pstruNextDevice;
	struct stru_NetSim_Device* pstruPrevDevice;
};

/* Store the Network structure*/
struct stru_NetSim_Network
{
	NETSIM_ID nDeviceCount;	//Total number of device
	NETSIM_ID nLinkCount;	//Total number of links in network
	NETSIM_ID nApplicationCount; //Total number of application running
	struct stru_NetSim_Device** ppstruDeviceList;	//List of all device in network
	struct stru_NetSim_Links** ppstruNetSimLinks;
	void** appInfo;	//Information of application. Refer ../ApplicationDll/Application.h
	struct stru_NetSim_SimulationParameter* pstruSimulationParameter;//Simulation parameter for end type and value
};
EXPORTED struct stru_NetSim_Network* NETWORK;
struct stru_NetSim_EventDetails
{
	unsigned long long int nEventId; //Unique id of event
	double dEventTime; //Event time when packet is processed.
	EVENT_TYPE nEventType; //Type of event
	NETSIM_ID nProtocolId;
	netsimDEVICE_TYPE nDeviceType; //Type of device
	NETSIM_ID nDeviceId; //Unique id of device
	NETSIM_ID nInterfaceId; //Unique id of interface
	NETSIM_ID nApplicationId; //Unique id of application
	double dPacketSize; //Packet size of current event
	struct stru_NetSim_Packet *pPacket; //Packet for current event
	long long int nPacketId; //Unique id for packet
	int nSegmentId;
	NETSIM_ID nSubEventType; //Sub event type
	void* szOtherDetails; //Details specific to protocol.
	unsigned long long int nPrevEventId;
	int line;
	char* file;
};

/* Stack API */

/*MACROS*/
#define DEVICE(DeviceId)							NETWORK->ppstruDeviceList[DeviceId-1]
 #ifdef DEVICE_TYPE
 #undef DEVICE_TYPE
 #endif
#define DEVICE_TYPE(DeviceId)						DEVICE(DeviceId)->nDeviceType
#define DEVICE_NAME(DeviceId)						DEVICE(DeviceId)->szDeviceName
#define DEVICE_VAR(DeviceId)						DEVICE(DeviceId)->deviceVar
#define DEVICE_INTERFACE(DeviceId,InterfaceId)		DEVICE(DeviceId)->ppstruInterfaceList[InterfaceId-1]
#define DEVICE_APPLAYER(DeviceId)					DEVICE(DeviceId)->pstruApplicationLayer
#define DEVICE_TRXLayer(DeviceId)					DEVICE(DeviceId)->pstruTransportLayer
#define DEVICE_NWLAYER(DeviceId)					DEVICE(DeviceId)->pstruNetworkLayer
#define DEVICE_MACLAYER(DeviceId,InterfaceId)		DEVICE_INTERFACE(DeviceId,InterfaceId)->pstruMACLayer
#define DEVICE_PHYLAYER(DeviceId,InterfaceId)		DEVICE_INTERFACE(DeviceId,InterfaceId)->pstruPhysicalLayer
#define DEVICE_POSITION(DeviceId)					DEVICE(DeviceId)->pstruDevicePosition
#define DEVICE_MOBILITY(DeviceId)					DEVICE(DeviceId)->pstruDeviceMobility
#define DEVICE_SOCKETINTERFACE(DeviceId)			DEVICE(DeviceId)->pstruSocketInterface
#define DEVICE_MAC_NW_INTERFACE(DeviceId,InterfaceId) DEVICE_INTERFACE(DeviceId,InterfaceId)->pstruAccessInterface
#define DEVICE_ACCESSBUFFER(DeviceId,InterfaceId)	DEVICE_MAC_NW_INTERFACE(DeviceId,InterfaceId)->pstruAccessBuffer
#define DEVICE_PHYMEDIUM(DeviceId,InterfaceId)		DEVICE_PHYLAYER(DeviceId,InterfaceId)->nPhyMedium
#define	DEVICE_PHYVAR(DeviceId,InterfaceId)			DEVICE_PHYLAYER(DeviceId,InterfaceId)->phyVar
#define DEVICE_MACVAR(DeviceId,InterfaceId)			DEVICE_MACLAYER(DeviceId,InterfaceId)->MacVar
#define DEVICE_HWADDRESS(DeviceId,InterfaceId)		DEVICE_MACLAYER(DeviceId,InterfaceId)->szMacAddress
#define DEVICE_NWROUTINGVAR(DeviceId)				DEVICE_NWLAYER(DeviceId)->RoutingVar
#define DEVICE_NWPROTOCOL(DeviceId,InterfaceId)		DEVICE_INTERFACE(DeviceId,InterfaceId)->nProtocolId
#define DEVICE_NWADDRESS(DeviceId,InterfaceId)		DEVICE_INTERFACE(DeviceId,InterfaceId)->szAddress
#define DEVICE_TCPVAR(DeviceId)						DEVICE_TRXLayer(DeviceId)->TCPProtocol
#define DEVICE_UDPVAR(DeviceId)						DEVICE_TRXLayer(DeviceId)->UDPProtocol
#define DEVICE_APPINFO(DeviceId)					DEVICE_APPLAYER(DeviceId)->pstruApplicationInfo
#define DEVICE_APPROUTINGVAR(DeviceId)				DEVICE_APPLAYER(DeviceId)->routingVar
/* Stack API end*/

/*Event API*/
#define EVENT_TYPE(EventDetails)					(EventDetails)->nEventType
#define EVENT_TIME(EventDetails)					(EventDetails)->dEventTime
#define EVENT_DEVICETYPE(EventDetails)				(EventDetails)->nDeviceType
#define EVENT_DEVICEID(EventDetails)				(EventDetails)->nDeviceId
#define EVENT_INTERFACEID(EventDetails)				(EventDetails)->nInterfaceId
#define EVENT_APPID(EventDetails)					(EventDetails)->nApplicationId
#define EVENT_PACKETSIZE(EventDetails)				(EventDetails)->dPacketSize
#define EVENT_PACKET(EventDetails)					(EventDetails)->pPacket
#define EVENT_PACKETID(EventDetails)				(EventDetails)->nPacketId
#define EVENT_SUBEVENTTYPE(EventDetails)			(EventDetails)->nSubEventType
#define EVENT_OTHERDETAILS(EventDetails)			(EventDetails)->szOtherDetails
/*Event API End*/

//Simulation kernel api
typedef long long int (*fn_NetSim_Utilities_AddEvent)(struct stru_NetSim_EventDetails*,int,const char*); /* Add an event to event list.*/
fn_NetSim_Utilities_AddEvent fnpAddEvent;
_declspec(dllexport) int fnDeleteEvent(unsigned long long int nEventId);
/* Stack Variable declaration*/
EXPORTED struct stru_NetSim_EventDetails* pstruEventDetails; //Stores the event details. Only one time memory is allocated. Most used variable
EXPORTED double ldEventTime;		//Stores the current event time
/*EXPORTED*/ EVENT_TYPE nEventType;	//Stores the current event type
EXPORTED char* pszAppPath;		//Stores the application path

EXPORTED char* pszIOPath;		//Stores the I/O path
EXPORTED NETSIM_VERSIONTYPE nVersionType;		//Stores the version type of netsim
EXPORTED int nDbgFlag; //Stores the debug flag

EXPORTED int wireshark_flag;

#define NETSIM_RAND_MAX (double)10000000000

//Calculate distance
_declspec(dllexport) double fn_NetSim_Utilities_CalculateDistance(NetSim_COORDINATES* coordinate1,NetSim_COORDINATES* coordinates2);
//Random Number Generator
_declspec(dllexport) double fn_NetSim_Utilities_GenerateRandomNo(unsigned long *uSeed, unsigned long *uSeed1);

//Concat string
_declspec(dllexport) int fn_NetSim_Utilities_ConcatString(int num,char* arg1,const char* arg2,...);


//Used to generate number based on distribution.
_declspec(dllexport) int fn_NetSim_Utils_Distribution(int nDistributionType, double *fDistOut,unsigned long *uSeed, unsigned long *uSeed1,double fFirstArg,double fSecondArg);

//return the device id. This function take input as device name and return the device is of the device if found else return 0. 
_declspec(dllexport) NETSIM_ID fn_NetSim_Stack_GetDeviceId_asName(const char* name);

//Device Id by IP address. This function take input as IP address and return the device id and also update the interface id(passed as reference)
_declspec(dllexport) NETSIM_ID fn_NetSim_Stack_GetDeviceId_asIP(NETSIM_IPAddress ip,NETSIM_ID* nInterfaceId);

//Device Id by Mac address. This function take the MAC address as input and return the device id and also update the interface id (Passed as reference)
_declspec(dllexport) NETSIM_ID fn_NetSim_Stack_GetDeviceId_asMac(PNETSIM_MACADDRESS mac,NETSIM_ID* nInterfaceId);

//Wireless interface of a device. This function takes the link id and device id as input and return the interface id of wireless port if connected else return 0.
_declspec(dllexport) NETSIM_ID fn_NetSim_Stack_GetWirelessInterface(NETSIM_ID nLinkId,NETSIM_ID nDevId);

//Used to get interface id from IP address of a device.
_declspec(dllexport) NETSIM_ID fn_NetSim_Stack_GetInterfaceIdFromIP(NETSIM_ID nDeviceId,NETSIM_IPAddress ip);

/* Function used to get connection.
 * Take four parameter
 * 1. Device id
 * 2. Interface id
 * 3. pointer to connected device id
 * 4. pointer to connected interface id
 * return link id.
 * Update connected device id and connected interface id.
 * return 0 in case of no connection found.
 */
_declspec(dllexport) NETSIM_ID fn_NetSim_Stack_GetConnectedDevice(NETSIM_ID nDeviceId,NETSIM_ID nInterfaceId,NETSIM_ID* nConnectedDevId,NETSIM_ID* nConnectedInterfaceId);

//Used to remove a device from link. Call only for P2MP or MP2MP link
_declspec(dllexport) NetSim_LINKS* fn_NetSim_Stack_RemoveDeviceFromlink(NETSIM_ID nDeviceId,NETSIM_ID nInterfaceId,NETSIM_ID nLinkId);

//Used to add a device to link. Call only for P2MP or MP2MP link
_declspec(dllexport) NetSim_LINKS* fn_NetSim_Stack_AddDeviceTolink(NETSIM_ID nDeviceId,NETSIM_ID nInterfaceId,NETSIM_ID nLinkId);

//Used to get the MAC protocol running on interface of a device 
_declspec(dllexport) MAC_LAYER_PROTOCOL fn_NetSim_Stack_GetMacProtocol(NETSIM_ID nDeviceId,NETSIM_ID nInterfaceId);
//Used to get Network layer protocol running on the device
_declspec(dllexport) NETWORK_LAYER_PROTOCOL fn_NetSim_Stack_GetNWProtocol(NETSIM_ID nDeviceId);
//Used to get network layer routing protocol running on device
_declspec(dllexport) NETWORK_LAYER_PROTOCOL fn_NetSim_Stack_GetNWRoutingProtocol(NETSIM_ID nDeviceId);
//Used to get APP layer routing protocol running on device
_declspec(dllexport) APPLICATION_LAYER_PROTOCOL fn_NetSim_Stack_GetAppRoutingProtocol(NETSIM_ID nDeviceId);
//used to get Transport layer protocol based on the packet type.
_declspec(dllexport) TRANSPORT_LAYER_PROTOCOL fn_NetSim_Stack_GetTrnspProtocol(NETSIM_ID nDeviceId, const struct stru_NetSim_Packet*);

/* Used to get the out interface in case of static routing is enable.
   This function is called in Network In event in case of 3/4/5 layer device(Node, router)
   or Mac in in case of 2 layer device (Switch, AP).
   This function takes four parameter - 
   1. Source Id 
   2. Destination Id
   3. Current device id
   4. Pointer to out interface
   5. Pointer to next hop.
   This function return the out interface id.
   It will also update the out interface and next hop.
*/
_declspec(dllexport) NETSIM_ID fn_NetSim_StaticRoute_GetOutInterface(NETSIM_ID nSourceId,NETSIM_ID nDestinationId,NETSIM_ID nDeviceId,NETSIM_ID* nOutInterface,NETSIM_ID* nextHop);
/* Used to get the next hop in case of static routing is enable.
   This function is called in Network out event in case of 3/4/5 layer device(Node, router)
   or Mac out in case of 2 layer device (Switch, AP).
   This function takes four parameter - 
   1. Source Id 
   2. Destination Id
   3. Current device id
   4. Pointer to out interface
   5. Pointer to next hop.
   This function return the out interface id.
   It will also update the out interface and next hop.
*/
_declspec(dllexport) NETSIM_ID fn_NetSim_StaticRoute_GetNextHop(NETSIM_ID nSourceId,NETSIM_ID nDestinationId,NETSIM_ID nDeviceId,NETSIM_ID* nOutInterface,NETSIM_ID* nNextHop);


/* Configuration API*/
//Used to get an attributes value from an xml tag
_declspec (dllexport) char* fn_NetSim_xmlConfig_GetVal(void* xmlNetSimNode,const char* szName,int flag);
//Used to configure packet trace field
_declspec (dllexport) char* fn_NetSim_xmlConfigPacketTraceField(const void* xmlNetSimNode,const char* name);
//Used to write error log in config log file
_declspec (dllexport) int xmlErrorRedefine(const char* msg,char* val,const void* xmlNetSimNode);
//Used to get child node of any node
_declspec(dllexport) void* fn_NetSim_xmlGetChildElement(void* xmlNetSimNode,const char* childName,int count);
//convert string link type to enum
_declspec(dllexport) LINK_TYPE fn_NetSim_Config_GetLinkType(const char* value);
//Covert string suppression model to enum
_declspec(dllexport) SUPPRESSION_MODEL fn_NetSim_Config_GetSuppressionModel(char* model);
//Convert string service type to enum
_declspec(dllexport) SERVICE_TYPE fn_NetSim_Config_GetServiceType(char* type);
//FConvert string device type to enum
_declspec(dllexport) netsimDEVICE_TYPE fn_NetSim_Config_GetDeviceType(const char* type);
//convert the string video model to enum
_declspec(dllexport) VIDEO_MODEL fn_NetSim_Config_GetVideoModel(const char* value);
//convert the string application type to enum
_declspec(dllexport) APPLICATION_TYPE fn_NetSim_Config_GetPacketType(const char* value);
//Conver the string distribution to enum
_declspec(dllexport) DISTRIBUTION fn_NetSim_Config_GetDistribution(const char* value);
//Convert the string interface type to enum
_declspec(dllexport) INTERFACE_TYPE fn_NetSim_Config_GetInterfaceType(const char* value);
//convert string protocol to enum
_declspec(dllexport) int fn_NetSim_Config_GetProtocol(const char* value);
//Used to get the IP address from name and interface id
_declspec(dllexport) NETSIM_IPAddress fn_NetSim_Stack_GetIPAddressByName(const NETSIM_Name szDeviceName,NETSIM_ID nInterfaceId);
//Used to get the mac address from IP address
_declspec(dllexport) PNETSIM_MACADDRESS fn_NetSim_Stack_GetMacAddressFromIP(const NETSIM_IPAddress szIPAddress);
//used to get the IP address from MAC address
_declspec(dllexport) NETSIM_IPAddress fn_NetSim_Stack_GetIPAddressFromMac(const PNETSIM_MACADDRESS szMacAddress);
//Used to get the first configure interface ip address of a device
_declspec(dllexport) NETSIM_IPAddress fn_NetSim_Stack_GetFirstIPAddressAsId(NETSIM_ID nDeviceId,unsigned int type);
//used to get the ip address from device id and interface id
_declspec(dllexport) NETSIM_IPAddress fn_NetSim_Stack_GetIPAddressAsId(NETSIM_ID nDeviceId,NETSIM_ID nInterfaceId);
//Used to get the first configured interface ip address of a device
_declspec(dllexport) NETSIM_IPAddress fn_NetSim_Stack_GetFirstIPAddressByName(const NETSIM_Name szDeviceName);
//Used to get config id of link by backend id of link
_declspec(dllexport) NETSIM_ID fn_NetSim_Stack_GetConfigIdOfLinkById(NETSIM_ID nLinkId);
//Used to get config id of device by backend id of device
_declspec(dllexport) NETSIM_ID fn_NetSim_Stack_GetConfigIdOfDeviceById(NETSIM_ID nDeviceId);
// Used to get the device id from config id
_declspec (dllexport) NETSIM_ID fn_NetSim_GetDeviceIdByConfigId(NETSIM_ID id);

//Used to get Application by config id
_declspec(dllexport) NETSIM_ID fn_NetSim_Stack_GetApplicationIdByConfigId(NETSIM_ID id);
//Used to get interface id by config id
_declspec(dllexport) NETSIM_ID fn_NetSim_GetInterfaceIdByConfigId(NETSIM_ID devId,NETSIM_ID id);
//Convert Connection medium string to enum
_declspec(dllexport) PHYSICAL_LAYER_MEDIUM fn_NetSim_Cofig_GetConnectionMedium(char* medium);

/* Traffic Generator */
//Used to create a packet for video traffic
_declspec(dllexport) int fn_NetSim_App_VideoTraffic(NETSIM_ID nDeviceId,NETSIM_ID nAppId,double dPacketSize,double dArrivalTime);
//Used to create a packet for data traffic
_declspec(dllexport) int fn_NetSim_App_DataTraffic(NETSIM_ID nDeviceId,NETSIM_ID nAppId,double dPacketSize,double dArrivalTime);
//used to create the packet for voice packet
_declspec(dllexport) int fn_NetSim_App_VoiceTraffic(NETSIM_ID nDeviceId,NETSIM_ID nAppId,double dPacketSize,double dArrivalTime);

/* Metrics */
_declspec(dllexport) int fn_NetSim_Metrics_Add(const NetSim_PACKET* pPacket);

_declspec(dllexport) double fn_NetSim_Stack_GetMSS(const NetSim_PACKET* pPacket);

/*Bit wise Operation*/
//Used to check if a bit is set or not
_declspec(dllexport) int isBitSet(unsigned int num,int index);
//Used to set a bit in number
_declspec(dllexport) unsigned int setBit(unsigned int num,int index);
//used to unset a bit in number
_declspec(dllexport) unsigned int unsetBit(unsigned int num,int index);

//Interlayer communication function
//Register call back back when an ICMP error arrive
_declspec(dllexport) int fn_NetSim_Stack_RegisterICMPCallback(int (*fnCallBack)(NetSim_PACKET* errPacket,NETSIM_ID nDeviceId,unsigned int nICMPErr));
//Called by IP layer when an ICMP error arrives
_declspec(dllexport) int fn_NetSim_Stack_CallICMPErrorFun(NetSim_PACKET* packet,NETSIM_ID nDeviceId,unsigned int nICMPErr);

//Function pointer called to resolve id to ip
typedef NETSIM_IPAddress (*fndns_query)(NETSIM_ID nDeviceId,NETSIM_ID id);
EXPORTED fndns_query fnDNS;
_declspec(dllexport) NETSIM_IPAddress DNS_QUERY(NETSIM_ID nDeviceId,NETSIM_ID id);

//Callback function for mobility. Must include Mobility.lib
typedef int (*_fnMobilityCallBack)(NETSIM_ID nDeviceId);
_fnMobilityCallBack* fnMobilityCallBack;
_declspec(dllexport) int fnMobilityRegisterCallBackFunction(_fnMobilityCallBack fnCallBack);

//Callback function for node join. Must include Mobility.lib
typedef int (*_fnNodeJoinCallBack)(NETSIM_ID nDeviceId,double time,NODE_ACTION);
_fnNodeJoinCallBack* fnNodeJoinCallBack;
_declspec(dllexport) int fnNodeJoinRegisterCallBackFunction(_fnNodeJoinCallBack fnCallBack);

//Xml configuration automatic reading
#define INT 1
#define DOUBLE 2
#define FLOAT 2
#define STRING 3
#define _BOOL 4
_declspec(dllexport) int GetXmlVal(void* var,char* name,void* xmlNode,int flag,int type);
#define getXmlVar(var,name,xmlNode,flag,type,protocol) if(!GetXmlVal(var,#name,xmlNode,flag,type)) *(var)= protocol##_##name##_DEFAULT

//Mac address API
_declspec(dllexport) PNETSIM_MACADDRESS STR_TO_MAC(char* mac);
_declspec(dllexport) char* MAC_TO_STR(PNETSIM_MACADDRESS mac);
_declspec(dllexport) PNETSIM_MACADDRESS MAC_COPY(PNETSIM_MACADDRESS mac);
_declspec(dllexport) int MAC_COMPARE(PNETSIM_MACADDRESS mac1, PNETSIM_MACADDRESS mac2);
_declspec(dllexport) void MAC_FREE(PNETSIM_MACADDRESS mac);
_declspec(dllexport) void MAC_FREE_ALL();

#ifdef  __cplusplus
}
#endif
#endif //_NETSIM_STACK_H_
