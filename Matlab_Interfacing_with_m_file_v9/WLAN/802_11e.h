/************************************************************************************
 * Copyright (C) 2013     
 *
 * TETCOS, Bangalore. India                                                         *

 * Tetcos owns the intellectual property rights in the Product and its content.     *
 * The copying, redistribution, reselling or publication of any or all of the       *
 * Product or its content without express prior written consent of Tetcos is        *
 * prohibited. Ownership and / or any other right relating to the software and all  *
 * intellectual property rights therein shall remain at all times with Tetcos.      *

 * Author:    Basamma YB                                                      *
 * ---------------------------------------------------------------------------------*/

typedef struct stru_802_11e_Packetlist QOSLIST;
typedef enum enum_802_11e_Status QOS_STATUS;

int fn_NetSim_WLAN_802_11e_PacketArrive_MacLayer();
int fn_NetSim_WLAN_802_11e_AddPacketToList(NetSim_PACKET** list,NetSim_PACKET* packet, double dMaximumBuffSize,double dCurrentBuffSize);
NetSim_PACKET* fn_NetSim_WLAN_Packet_GetPacketFromBuffer(struct stru_802_11e_Packetlist* pstruQosList,int nFlag);
int fn_NetSim_WLAN_80211e_InitQosParameters();
int fn_NetSim_WLAN_802_11e_Free_PacketList(NETSIM_ID nDeviceId, NETSIM_ID nInterfaceId);
/// Enumeration to either enable or disable the IEEE 802.11e status
enum enum_802_11e_Status
{
	DISABLE_11E,
	ENABLE_11E,
};
/// Data structure for IEEE 802.11e packet lists.
struct stru_802_11e_Packetlist
{
	NetSim_PACKET** pstruList;
#define AC_VO pstruList[0]
#define AC_VI pstruList[1]
#define AC_BE pstruList[3]
#define AC_BK pstruList[2]
	double dMaximumSize;
	double dCurrentSize;
};

