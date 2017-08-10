
/***********************************************************************************/
/***********************************************************************************/
#include "main.h"
#include "WLAN.h"
#include "802_11n_ac.h"
/**
Packets with same Receiver ID are aggregated. Broadcast packets are not aggregated
*/
NetSim_PACKET* fn_NetSim_WLAN_Packet_GetAggregatedPacketFromBuffer(struct stru_802_11e_Packetlist* pstruQosList,NETSIM_ID nDeviceId,NETSIM_ID nInterfaceId,int nFlag)
{
	 
	NetSim_PACKET* pstruPacketHead, *pstruPacket, *pstruAggListPrevPacket;
	NetSim_PACKET *pstruList0,*pstruListPrevPacket ;
	MAC_VAR *pstruMacVar ; 
	int n=0;
	int i=0;

	pstruMacVar=(MAC_VAR*)WLAN_MAC(nDeviceId,nInterfaceId);
	n=pstruMacVar->nNumberOfAggregatedPackets;
	if(n<=0 && n>1024)
	{
		fnNetSimError("nNumberOfAggregatedPackets must be in range of [0,1024]");
		n=1;
	}
	
	for(i=0;i<MAX_UP;i++)
	{
		if(pstruQosList->pstruList[i])
		{
			pstruPacketHead = pstruQosList->pstruList[i];		
			if(nFlag)
			{
				n--;
				pstruAggListPrevPacket=pstruPacketHead;
				pstruQosList->pstruList[i] = pstruQosList->pstruList[i]->pstruNextPacket;
				pstruAggListPrevPacket->pstruNextPacket=NULL;
				pstruList0=pstruQosList->pstruList[i];


				while(n>0 && pstruList0!=NULL && pstruPacketHead->nDestinationId!=0)
				{
					if(pstruList0->nDestinationId==0||pstruList0->nReceiverId != pstruPacketHead->nReceiverId)
					{
						pstruListPrevPacket=pstruList0;
						pstruList0=pstruList0->pstruNextPacket;
						continue;
					}
					n--;
					if(pstruQosList->pstruList[i]==pstruList0)	//deleting packet from front
					{
						pstruPacket=pstruList0;
						pstruList0=pstruList0->pstruNextPacket;
						pstruQosList->pstruList[i]=pstruQosList->pstruList[i]->pstruNextPacket;
						pstruPacket->pstruNextPacket=NULL;
					}
					else//deleting packet from middle
					{
						pstruPacket=pstruList0;
						pstruListPrevPacket->pstruNextPacket=pstruList0->pstruNextPacket;
						pstruPacket->pstruNextPacket=NULL;
						pstruList0=pstruListPrevPacket->pstruNextPacket;
					}
					pstruAggListPrevPacket->pstruNextPacket=pstruPacket;
					pstruAggListPrevPacket=pstruAggListPrevPacket->pstruNextPacket;
				}
			
			}
			return pstruPacketHead;
		}
	}
	return NULL;
}
/// This function is used to free the aggregated Packet
void Aggregated_Packet_FreePacket(NetSim_PACKET *pstruPacketHead)
{
	NetSim_PACKET *pstruPacket;
	while(pstruPacketHead)
	{
		pstruPacket=pstruPacketHead;
		pstruPacketHead=pstruPacketHead->pstruNextPacket;
		pstruPacket->pstruNextPacket=NULL;
		fn_NetSim_Packet_FreePacket(pstruPacket);
		pstruPacket=NULL;
	}
	pstruPacketHead=NULL;
}
/** 
This function returns 1 if entire AMPDU collide i.e. header frame gets collided
else return 0
*/
int Check_AMPDU_Collide()
{
	NetSim_PACKET *pstruPacketHead;
	NetSim_PACKET *pstruPacket=pstruEventDetails->pPacket;
	pstruPacketHead=pstruPacket;
	
	if(pstruPacketHead->nPacketStatus!=PacketStatus_NoError)
	{
		Aggregated_Packet_FreePacket(pstruPacketHead);
		return 1;
	}
	else 
		return 0;
}
/**
This function is used to generate the cummulative acknowledgement packet.
*/
NetSim_PACKET *Generate_Block_Ack_Packet()
{
	
	NetSim_PACKET *pstruBlockAckPkt,*pstruPacket;
	BlockACK_FRAME *pstruBlockAck;
	int i;
	PHY_VAR *pstruPhy = DEVICE_PHYVAR(pstruEventDetails->nDeviceId,pstruEventDetails->nInterfaceId);
	pstruPacket=pstruEventDetails->pPacket;

	//Generate a BLOCK ACK Packet Format
	pstruBlockAckPkt = fn_NetSim_Packet_CreatePacket(MAC_LAYER);
	pstruBlockAckPkt->nPacketType = PacketType_Control;
	pstruBlockAckPkt->nPacketPriority = Priority_High;
	pstruBlockAckPkt->nControlDataType = WLAN_CNTRL_PKT(WLAN_BlockACK,pstruEventDetails->nProtocolId);
	pstruBlockAck =  fnpAllocateMemory(1, sizeof(BlockACK_FRAME));
	pstruBlockAck->pstruFrameControl = fnpAllocateMemory(1,sizeof(FRAME_CONTROL));	
	pstruBlockAck->pstruFrameControl->FrameType = CONTROL;
	pstruBlockAck->pstruFrameControl->usnSubType = BlockAck;	
	pstruBlockAck->RA =((MAC_HEADER*)(pstruPacket->pstruMacData->Packet_MACProtocol))->Address2;
	pstruBlockAck->TA =((MAC_HEADER*)(pstruPacket->pstruMacData->Packet_MACProtocol))->Address1;
	if(pstruPhy->nPHYprotocol==IEEE_802_11n)
	{
		for(i=0;i<MAX_802_11n_AMPDU_SIZE;i++)
		{
			pstruBlockAck->BitMap[i]=0;
		}
	}
	else if(pstruPhy->nPHYprotocol==IEEE_802_11ac)
		for(i=0;i<MAX_802_11ac_AMPDU_SIZE;i++)
		{
			pstruBlockAck->BitMap[i]=0;
		}

	pstruBlockAckPkt->nReceiverId  = pstruPacket->nReceiverId; 
	pstruBlockAckPkt->nDestinationId = pstruPacket->nDestinationId;
	pstruBlockAckPkt->nTransmitterId=pstruPacket->nTransmitterId;
	pstruBlockAckPkt->nSourceId=pstruPacket->nSourceId;
	pstruBlockAckPkt->pstruMacData->Packet_MACProtocol = pstruBlockAck;

	return pstruBlockAckPkt;
}

/// This fuction is used to copy the Aggregated Packet
NetSim_PACKET* fn_NetSim_Packet_CopyAggregatedPacket(NetSim_PACKET *packetList)//NULL;
{
	NetSim_PACKET *packetHead=NULL;
	NetSim_PACKET *packetHeadList=NULL;
	NetSim_PACKET *packet=NULL;

	while(packetList)
	{
		packet=fn_NetSim_Packet_CopyPacket(packetList);
		packet->pstruNextPacket=NULL;

		if(packetHead)
		{
			packetHeadList->pstruNextPacket=packet;
			packetHeadList=packetHeadList->pstruNextPacket;
			packet=NULL;
			
		}
		else
		{
			packetHead=packet;
			packetHead->pstruNextPacket=NULL;
			packetHeadList=packetHead;
			packet=NULL;

		}
		
		packetList=packetList->pstruNextPacket;

	}
	return packetHead;
}