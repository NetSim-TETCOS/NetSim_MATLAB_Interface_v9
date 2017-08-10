/************************************************************************************
 * Copyright (C) 2013    
 *
 * TETCOS, Bangalore. India                                                         *

 * Tetcos owns the intellectual property rights in the Product and its content.     *
 * The copying, redistribution, reselling or publication of any or all of the       *
 * Product or its content without express prior written consent of Tetcos is        *
 * prohibited. Ownership and / or any other right relating to the software and all  *
 * intellectual property rights therein shall remain at all times with Tetcos.      *

 * Author:   Basamma YB 
 *  
 ************************************************************************************/
#include "main.h"
#include "WLAN.h"
/**
This fuction is called from the fn_NetSim_WLAN_Init().
Intialize the parameters relevent to QOS list.
*/
int fn_NetSim_WLAN_80211e_InitQosParameters()
{
	NETSIM_ID nLoop, nPort, nNumOfPorts ;
	MAC_VAR* pstruMacVar;
	
	for(nLoop=1;nLoop <=NETWORK->nDeviceCount;nLoop++)	
	{
		nNumOfPorts= NETWORK->ppstruDeviceList[nLoop-1]->nNumOfInterface;
		for(nPort=1;nPort<=nNumOfPorts;nPort++)
		{
			if(NETWORK->ppstruDeviceList[nLoop-1]->ppstruInterfaceList[nPort-1]->pstruMACLayer)
				switch(DEVICE_MACLAYER(nLoop,nPort)->nMacProtocolId)
				{
				case MAC_PROTOCOL_IEEE802_11:				
						pstruMacVar = NETWORK->ppstruDeviceList[nLoop-1]->ppstruInterfaceList[nPort-1]->pstruMACLayer->MacVar;
						pstruMacVar->pstruQosList  = fnpAllocateMemory(1, sizeof(struct stru_802_11e_Packetlist));
						pstruMacVar->pstruQosList->pstruList = fnpAllocateMemory(MAX_UP,sizeof(NetSim_PACKET* ));								
						pstruMacVar->pstruQosList->dMaximumSize = NETWORK->ppstruDeviceList[nLoop-1]->ppstruInterfaceList[nPort-1]->pstruAccessInterface->pstruAccessBuffer->dMaxBufferSize * 1024 * 1024;
					break;
				default:				
					break;
				}			
		}
	}
	return 0;
}

/**
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
This fuction is called when MAC_OUT_EVENT triggered with sub event type = 0;
Check the pstruPacket->nQOS, based on that move the packets into different list.
Check IEEE802.11e enabled or not.
if not enabled then check the QOS class and move packet in to different list
	if pstruPacket->nQOS = QOS_ertPS or QOS_rtPS
		Voice packets move to pstruList[0]
		Vedio packets move to pstruList[1]
	if pstruPacket->nQOS = QOS_nrtPS and QOS_BE
		Move packets into pstruList[3]
	if pstruPacket->nQOS = QOS_UGS
		Move packets into pstruList[0]	
if IEEE802.11e not enabled 
	Move all packets into pstruList[3]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
int fn_NetSim_WLAN_802_11e_PacketArrive_MacLayer()
{
	NetSim_PACKET* pstruPacket;
	NetSim_PACKET* pstruTempPacket;
	int nDevId = pstruEventDetails->nDeviceId;
	int nInterface = pstruEventDetails->nInterfaceId;
	MAC_VAR* pstruMacVar = DEVICE_MACVAR(nDevId,nInterface);
	PHY_VAR* pstruPhy = DEVICE_PHYVAR(nDevId,nInterface);
	int nDeviceId=nDevId;
	int nInterfaceId=nInterface;
	PHY_VAR *pstruPhyVar=pstruPhy;

	//The packets that arrive at the access buffer are sorted according to the
	//802.11e priority into one of the 4 QOS buffers.
	NetSim_BUFFER* pstruBuffer = DEVICE_MAC_NW_INTERFACE(nDevId,nInterface)->pstruAccessBuffer;	
TAKENEXTPACKET:
	pstruPacket = fn_NetSim_Packet_GetPacketFromBuffer(pstruBuffer,1);	
	pstruTempPacket = pstruPacket;

	//Update the Receiver and transmitter id of Packets and then put them in the QOS buffer
	//If 802.11e is Disabled, then put the packets in list 3, i.e. there is just one list
	
	while(pstruPacket)
	{
		NetSim_PACKET* pstruTempPacket;          
		pstruTempPacket = pstruPacket->pstruNextPacket;
		pstruPacket->pstruNextPacket = NULL;
		//Start and Arrival Time
		
		if(pstruEventDetails->nDeviceType == ACCESSPOINT && pstruPhy->BssType == INFRASTRUCTURE) // AP
		{	
			pstruPacket->pstruMacData->dArrivalTime = pstruEventDetails->dEventTime	;
		}
		else // Wireless Nodes
		{	
			pstruPacket->pstruMacData->dArrivalTime = pstruPacket->pstruNetworkData->dEndTime;
		}
		//pstruPacket->pstruMacData->dStartTime = pstruEventDetails->dEventTime;
		//Updating Receiver and transmitter ID
		switch(NETWORK->ppstruDeviceList[nDeviceId-1]->ppstruInterfaceList[nInterfaceId-1]->pstruPhysicalLayer->pstruNetSimLinks->nLinkType)
		{
		case LinkType_P2MP:
			if(nDeviceId != pstruPhyVar->nBSSID )// && pstruPhyVar->BssType == INFRASTRUCTURE)
			{	
				pstruPacket->nReceiverId = pstruPhyVar->nBSSID;	
				pstruPacket->nSourceId = nDeviceId;
				pstruPacket->nTransmitterId = nDeviceId;		
				pstruPacket->nDestinationId = pstruPacket->nDestinationId;
			}
			else
			{		
				pstruPacket->nReceiverId  = pstruPacket->nDestinationId; //of Data packet
				pstruPacket->nSourceId = pstruPacket->nSourceId;
				pstruPacket->nTransmitterId = pstruPhyVar->nBSSID;		
				pstruPacket->nDestinationId = pstruPacket->nDestinationId;
			}
			break;
		case LinkType_MP2MP:
				
				pstruPacket->nReceiverId  = pstruPacket->nReceiverId;//pstruPacket->nDestinationId; //of Data packet
				pstruPacket->nSourceId = pstruPacket->nSourceId;
				pstruPacket->nTransmitterId = pstruPacket->nTransmitterId;//pstruPacket->nSourceId;	
				pstruPacket->nDestinationId = pstruPacket->nDestinationId;
			break;
		default:
			break;
		}	

		//If 802.11e is enabled, then put the packet in respective list. 		
		if(pstruMacVar->n80211eEnableFlag)
		{
			switch(pstruPacket->nQOS)
			{
			case QOS_ertPS:					
			case QOS_rtPS:
				if(pstruPacket->nPacketType == PacketType_Voice)
					fn_NetSim_WLAN_802_11e_AddPacketToList(&(pstruMacVar->pstruQosList->pstruList[0]),pstruPacket,pstruMacVar->pstruQosList->dMaximumSize,pstruMacVar->pstruQosList->dCurrentSize);
				else				
					fn_NetSim_WLAN_802_11e_AddPacketToList(&(pstruMacVar->pstruQosList->pstruList[1]),pstruPacket,pstruMacVar->pstruQosList->dMaximumSize,pstruMacVar->pstruQosList->dCurrentSize);
				break;
			case QOS_UGS:			
				fn_NetSim_WLAN_802_11e_AddPacketToList(&(pstruMacVar->pstruQosList->pstruList[0]),pstruPacket,pstruMacVar->pstruQosList->dMaximumSize,pstruMacVar->pstruQosList->dCurrentSize);
				break;
			case QOS_nrtPS:
			case QOS_BE:			
				fn_NetSim_WLAN_802_11e_AddPacketToList(&(pstruMacVar->pstruQosList->pstruList[2]),pstruPacket,pstruMacVar->pstruQosList->dMaximumSize,pstruMacVar->pstruQosList->dCurrentSize);
				break;
			default:			
				fn_NetSim_WLAN_802_11e_AddPacketToList(&(pstruMacVar->pstruQosList->pstruList[2]),pstruPacket,pstruMacVar->pstruQosList->dMaximumSize,pstruMacVar->pstruQosList->dCurrentSize);
				break;
			}
		}
		else // IEEE11eflag DIABLED
		{
			fn_NetSim_WLAN_802_11e_AddPacketToList(&(pstruMacVar->pstruQosList->pstruList[3]),pstruPacket,pstruMacVar->pstruQosList->dMaximumSize,pstruMacVar->pstruQosList->dCurrentSize);
		}
		pstruPacket = pstruTempPacket;
	}	
	if(fn_NetSim_GetBufferStatus(pstruBuffer))
		goto TAKENEXTPACKET;
	//Function to check if Packets are there in QOS buffer and call a 
	//MAC out CS event if there.
	fn_NetSim_WLAN_CheckPacketsInPacketlist(nDevId,nInterface);
	return 0;
}
/**
	This fuction is used to add packets in to list if buffer has space, 
	else free the packet.
	After adding the packlet increase the current buffer size.	
*/
int fn_NetSim_WLAN_802_11e_AddPacketToList(NetSim_PACKET** pstruList,NetSim_PACKET* packet, double dMaximumBuffSize,double dCurrentBuffSize)
{
	if(dMaximumBuffSize != 0 && dCurrentBuffSize > dMaximumBuffSize)
	{
		fn_NetSim_Packet_FreePacket(packet);
		packet = NULL;
		return 0;
	}
	else
	{
		if(dMaximumBuffSize)
			dCurrentBuffSize += fnGetPacketSize(packet);

		if(!*pstruList)
		{
			*pstruList = packet;
			return 0;
		}
		else
		{
			NetSim_PACKET* temp;
			temp = *pstruList;
			while(temp->pstruNextPacket)
				temp = temp->pstruNextPacket;
			temp->pstruNextPacket = packet;
			return 0;
		}
	}
}
/**
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
This fuction is used to Get the packet from the pstruList.
The pstruList has priority from 0, 1, 2, 3.
Packets in pstruList[0] has highest priority and pstruList[3] has lerast priority.
So, here first we check any packets in the list[0], is so we get the packet from 
this list, else we go for next list and so on.
nFlag = 0 Just access the packet, but not move the packet pointer to next packet.
nFlag = 1 Taket the packet and move the packet pointer to next packet.
Decrease the current buffer size.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
NetSim_PACKET* fn_NetSim_WLAN_Packet_GetPacketFromBuffer(struct stru_802_11e_Packetlist* pstruQosList,int nFlag)
{
	NetSim_PACKET* pstruPacket;
	if(pstruQosList->pstruList[0])
	{
		pstruPacket = pstruQosList->pstruList[0];
		if(nFlag)
		{
			pstruQosList->pstruList[0] = pstruQosList->pstruList[0]->pstruNextPacket;
			pstruPacket->pstruNextPacket = NULL;
		}
		return pstruPacket;
	}
	else if(pstruQosList->pstruList[1])
	{
		pstruPacket = pstruQosList->pstruList[1];
		if(nFlag)
		{
			pstruQosList->pstruList[1] = pstruQosList->pstruList[1]->pstruNextPacket;
			pstruPacket->pstruNextPacket = NULL;
		}
		return pstruPacket;
	}
	else if(pstruQosList->pstruList[2])
	{
		pstruPacket = pstruQosList->pstruList[2];
		if(nFlag)
		{
			pstruQosList->pstruList[2] = pstruQosList->pstruList[2]->pstruNextPacket;
			pstruPacket->pstruNextPacket = NULL;
		}
		return pstruPacket;
	}
	else if(pstruQosList->pstruList[3])
	{
		pstruPacket = pstruQosList->pstruList[3];
		if(nFlag)
		{
			pstruQosList->pstruList[3] = pstruQosList->pstruList[3]->pstruNextPacket;
			pstruPacket->pstruNextPacket = NULL;
		}
		return pstruPacket;
	}
	return NULL;
}



/**
This fuction is called from the fn_NetSim_WLAN_Finish().
It first free any unprocessed packets in the pstruList s.
Free the parameters relevent to QOS list assigned during the fn_NetSim_WLAN_Init().
*/
int fn_NetSim_WLAN_802_11e_Free_PacketList(NETSIM_ID nDeviceId, NETSIM_ID nInterfaceId)
{
	int i;
	NetSim_PACKET* pstruPacket;
	MAC_VAR *pstruMacVar = NETWORK->ppstruDeviceList[nDeviceId-1]->ppstruInterfaceList[nInterfaceId-1]->pstruMACLayer->MacVar;
	for(i = 0; i < MAX_UP; i++)
	{
		if(pstruMacVar->pstruQosList->pstruList[i])
		{
			while(pstruMacVar->pstruQosList->pstruList[i])
			{
				pstruPacket = pstruMacVar->pstruQosList->pstruList[i];		
				pstruMacVar->pstruQosList->pstruList[i] = pstruMacVar->pstruQosList->pstruList[i]->pstruNextPacket;
				//pstruPacket->pstruNextPacket = NULL;
				fn_NetSim_Packet_FreePacket(pstruPacket);
				pstruPacket = NULL;
			}			
		}
		fnpFreeMemory(pstruMacVar->pstruQosList->pstruList[i]);
		pstruMacVar->pstruQosList->pstruList[i] = NULL;
	}
	fnpFreeMemory(pstruMacVar->pstruQosList->pstruList);
	return 0;
}

