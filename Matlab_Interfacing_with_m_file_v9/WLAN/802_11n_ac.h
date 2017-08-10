/*************************************************************************************
*************************************************************************************/
#define MAX_802_11n_AMPDU_SIZE 64
#define MAX_802_11ac_AMPDU_SIZE 1024

NetSim_PACKET* fn_NetSim_WLAN_Packet_GetAggregatedPacketFromBuffer(struct stru_802_11e_Packetlist* pstruQosList,NETSIM_ID nDeviceId,NETSIM_ID nInterfaceId, int nFlag);
NetSim_PACKET *Generate_Block_Ack_Packet();
int Check_AMPDU_Collide();
void Aggregated_Packet_FreePacket(NetSim_PACKET *pstruPacketHead);
NetSim_PACKET* fn_NetSim_Packet_CopyAggregatedPacket(NetSim_PACKET *packetList);//NULL;