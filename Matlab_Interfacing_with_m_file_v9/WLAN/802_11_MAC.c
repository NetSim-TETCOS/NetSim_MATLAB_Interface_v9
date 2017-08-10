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
#include "802_11n_ac.h"
/**
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
This function is called when MAC_OUT_EVENT trigged when a packet buffer 
to process in the MAC layer carrier sense. 
It check the Medium, if Medium is BUSY, CS failed return 0.
If Medium is IDLE, packets in the buffer, update arrival time,
add CHECK NAV sub event, return 1 to indicate CS success.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
int fn_NetSim_WLAN_CarrierSense()
{
	int nDestPresentFlag;
	NETSIM_ID nDeviceId = pstruEventDetails->nDeviceId;
	NETSIM_ID nInterfaceId = pstruEventDetails->nInterfaceId;
	MAC_VAR *pstruMac = DEVICE_MACVAR(nDeviceId,nInterfaceId);
	PHY_VAR *pstruPhy = DEVICE_PHYVAR(nDeviceId,nInterfaceId);
	NetSim_PACKET* pstruPacket;
	if(pstruMac->Medium != IDLE && pstruMac->CurrentState != MAC_IDLE)
		return 0;	//CS failed. Medium is busy.
	if(pstruMac->pstruTempData)
		pstruPacket = pstruMac->pstruTempData;
	else
	{
		pstruPacket = fn_NetSim_WLAN_Packet_GetPacketFromBuffer(pstruMac->pstruQosList,0);
		if(!pstruPacket)	
			return 0; //CS failed. No packet left for transmission.
	 }
	//Check the destination is with in the BSS if device is an AP for unicast frames
	if(nDeviceId == pstruPhy->nBSSID && pstruPhy->BssType == INFRASTRUCTURE)
	{
		while(1)
		{
			if(pstruPacket->nDestinationId)
			{
				nDestPresentFlag = fn_NetSim_WLAN_CheckDestDevicePresentWithinAP(nDeviceId,nInterfaceId,pstruPacket->nDestinationId);
				if(!nDestPresentFlag)
				{
					// Get packet from buffer and move to next packet
					pstruPacket = fn_NetSim_WLAN_Packet_GetPacketFromBuffer(pstruMac->pstruQosList,1);				
					// Drop the packet
					fn_NetSim_Packet_FreePacket(pstruPacket);
					pstruPacket = NULL;				
					// If buffer is not NULL add MAC OUT event to process the next packet
					pstruPacket = fn_NetSim_WLAN_Packet_GetPacketFromBuffer(pstruMac->pstruQosList,0);
					if(!pstruPacket)	
						return 0; //CS failed. No packet left for transmission.
					pstruEventDetails->nPacketId = pstruPacket->nPacketId;
					if(pstruPacket->pstruAppData)
						pstruEventDetails->nSegmentId = pstruPacket->pstruAppData->nSegmentId;
					else
						pstruEventDetails->nSegmentId = 0;					
				}
				else
					break; //Packet is for same BSS
			}
			else
				break; //Broadcast packet
		}
	}
	// Set the Start and Arrival time
	if(pstruMac->nRetryCount == 0)
	{
		if(nDeviceId == pstruPhy->nBSSID && pstruPhy->BssType == INFRASTRUCTURE) // AP
		{	
			pstruPacket->pstruMacData->dArrivalTime = pstruPacket->pstruMacData->dArrivalTime;
		}
		else // Wireless Nodes
		{	
			pstruPacket->pstruMacData->dArrivalTime = pstruPacket->pstruNetworkData->dEndTime;
		}
		pstruPacket->pstruMacData->dStartTime = pstruEventDetails->dEventTime;
	}
	if(pstruMac->nCSFlag == 0)
	{
		// Add CHECK NAV sub event
		pstruEventDetails->nPacketId = pstruPacket->nPacketId;
		pstruEventDetails->nSubEventType =  CHECK_NAV;
		pstruMac->nCSFlag = 1;
		return 1; // CS successfull
	}	
	return 0;
}
/**
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
This function called when CHECK_NAV subevent triggered. It is called  if the Medium 
is IDLE and a packet in the Accessbuffer to transmit.
If NAV <=0 then change the state to Wait_DIFS and add DIFS_END subevent.
If NAV > 0 then change the state to WF_NAV and add NAV_END subevent.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
int fn_NetSim_WLAN_CheckNAV()
{
	NETSIM_ID nDeviceId = pstruEventDetails->nDeviceId;
	NETSIM_ID nInterfaceId = pstruEventDetails->nInterfaceId;
	NETSIM_ID nProtocolId = pstruEventDetails->nProtocolId;
	MAC_VAR *pstruMacVar = WLAN_MAC(nDeviceId,nInterfaceId);
	PHY_VAR *pstruPhyVar = WLAN_PHY(nDeviceId,nInterfaceId);

	if(pstruEventDetails->dEventTime >= pstruMacVar->dNAV)
	{
		// Change the State
		fn_NetSim_WLAN_ChangeMacState(pstruMacVar,Wait_DIFS);		
		// Add DIFS_END subevent
		pstruEventDetails->dEventTime = pstruEventDetails->dEventTime + pstruPhyVar->nDIFS;
		pstruEventDetails->nSubEventType =  DIFS_END;
		pstruEventDetails->pPacket = NULL;
		fnpAddEvent(pstruEventDetails);	
	}
	else
	{	// Change the State
		fn_NetSim_WLAN_ChangeMacState(pstruMacVar,MAC_IDLE);
		pstruMacVar->nCSFlag = 0; // Reset the CS flag
	}	
	return 0;
}
/**
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
When the buffer has packet to transmit, the MAC LAYER sense the Medium. If it is IDLE,
then it will wait for DCF Inter Frame Space (DIFS) time before start transmission. 
At the end of DIFS time check the Medium. If Medium is IDLE, then change the State 
to BACKING_OFF, call the function to start backoff. If Medium is BUSY, then change 
the state to IDLE. 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
int fn_NetSim_WLAN_DIFS()
{
	NETSIM_ID nDeviceId = pstruEventDetails->nDeviceId;
	NETSIM_ID nInterfaceId = pstruEventDetails->nInterfaceId;	
	MAC_VAR *pstruMac = DEVICE_MACVAR(nDeviceId, nInterfaceId);	
	// check the medium 
	if(pstruMac->Medium == IDLE)
	{
		// Change the state
		fn_NetSim_WLAN_ChangeMacState(pstruMac,BACKING_OFF);
		// Start Backoff
		//pstruMac->BackOffFreezeFlag = 0; 
		fn_NetSim_WLAN_StartBackOff();
	}
	else
	{
		fn_NetSim_WLAN_ChangeMacState(pstruMac,MAC_IDLE);		
	}
	pstruMac->nCSFlag = 0; // Reset cs flag
	return 0;
}

/**
This function start the Backoff process. If backoff counter != 0,it adds the backoff event at (event 
time + backoff time) else it generate the random backoff time and add the backoff event. 
*/
int fn_NetSim_WLAN_StartBackOff()
{
	NETSIM_ID nDeviceId = pstruEventDetails->nDeviceId;
	NETSIM_ID nInterfaceId = pstruEventDetails->nInterfaceId;	
	NETSIM_ID nProtocolId = pstruEventDetails->nProtocolId;
	MAC_VAR *pstruMac = DEVICE_MACVAR(nDeviceId, nInterfaceId);	
	pstruMac->dBackOffStartTime = pstruEventDetails->dEventTime;
	if(pstruMac->dBackOffCounter == 0)
	{
		if(pstruMac->nRetryCount == 0)
			fn_NetSim_WLAN_IncreaseCW();// Expand the Contention Window 			
		fn_NetSim_WLAN_RandomBackOffTimeCalculation();			
	}
	pstruMac->dBackOffCounter = pstruMac->dBackOffTime;
	pstruMac->BackOffFreezeFlag = 0; //Set BackOffFreezFlag as Zero
	pstruEventDetails->dEventTime = pstruEventDetails->dEventTime + pstruMac->dBackOffTime;
	
	pstruEventDetails->nEventType = MAC_OUT_EVENT;
	pstruEventDetails->nSubEventType = BACKOFF;
	pstruEventDetails->pPacket = NULL;
	fnpAddEvent(pstruEventDetails);		
	return 0;
}
/**
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
This function is used to expand the CW. 
If the RetryCount = 0, then CWcurrent is set to CWmin the minimum CW.
else if CWcurrent >= CWmax, then CWcurrent is set to CWmax the Maximum CW.
else CWcurrent = (nCWcurrent * 2) + 1.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
int fn_NetSim_WLAN_IncreaseCW()
{
	NETSIM_ID nDeviceId = pstruEventDetails->nDeviceId;
	NETSIM_ID nInterfaceId = pstruEventDetails->nInterfaceId;
	MAC_VAR *pstruMac = DEVICE_MACVAR(nDeviceId,nInterfaceId);	
	PHY_VAR *pstruPhy = DEVICE_PHYVAR(nDeviceId,nInterfaceId);

	if(pstruMac->nRetryCount == 0)	
		pstruMac->nCWcurrent = pstruPhy->pstruPhyChar->naCWmin;	
	else
	{
		if(pstruMac->nCWcurrent >= pstruPhy->pstruPhyChar->naCWmax) 
			pstruMac->nCWcurrent = pstruPhy->pstruPhyChar->naCWmax; 
		else
			pstruMac->nCWcurrent = (pstruMac->nCWcurrent * 2) + 1;
	}
	return 0;	
}
/**
This function invokes the Back off time calculation.
BackoffTime = RandomNumber(Between CW) * SlotTime.
*/
int fn_NetSim_WLAN_RandomBackOffTimeCalculation()
{
	double dRandomNumber = 0.0;
	MAC_VAR *pstruMacVar;	
	PHY_VAR *pstruPhyVar;	
	NETSIM_ID nDeviceId = pstruEventDetails->nDeviceId;
	pstruMacVar = DEVICE_MACVAR(pstruEventDetails->nDeviceId,pstruEventDetails->nInterfaceId);
	pstruPhyVar = DEVICE_PHYVAR(pstruEventDetails->nDeviceId,pstruEventDetails->nInterfaceId); 
	//Get the seed values
	//dRandomNumber = fn_NetSim_Utilities_GenerateRandomNo(&pstruMacVar->ulWlanBackoffSeed1,&pstruMacVar->ulWlanBackoffSeed2);	
	dRandomNumber = fn_NetSim_Utilities_GenerateRandomNo(&NETWORK->ppstruDeviceList[nDeviceId-1]->ulSeed[0],&NETWORK->ppstruDeviceList[nDeviceId-1]->ulSeed[1]);
	// If you enable the below printfs, all these values are writing to Logfile.txt. User can analyze the backoff by comparing with packet trace and event trace.
	//printf("\nSeed1 = %ld seed2 = %ld",pstruMacVar->ulWlanBackoffSeed1,pstruMacVar->ulWlanBackoffSeed1);
	dRandomNumber = ceil(fmod(dRandomNumber, (pstruMacVar->nCWcurrent + 1)));
	//printf("EventID = %d and EventTime = %f",pstruEventDetails->nEventId,pstruEventDetails->dEventTime);
	//printf("Device ID = %d and RandNum = %f",pstruEventDetails->nDeviceId,dRandomNumber);
	pstruMacVar->dBackOffTime = dRandomNumber * pstruPhyVar->pstruPhyChar->naSlotTime;
	//printf("BOTime = %f\n",pstruMacVar->dBackOffTime);
	return 0;
}
/**
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
It check Medium if the backoff counter <= 0. If Medium IDLE then it start frame 
transmission. If Medium BUSY or backoff counter > 0, then change the State to IDLE 
to go for backoff for the residual time.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
int fn_NetSim_WLAN_BackOff()
{
	double dTime = pstruEventDetails->dEventTime;
	NETSIM_ID nDeviceId = pstruEventDetails->nDeviceId;
	NETSIM_ID nInterfaceId = pstruEventDetails->nInterfaceId;	
	MAC_VAR *pstruMac = DEVICE_MACVAR(nDeviceId, nInterfaceId);
	
	if((pstruMac->BackOffFreezeFlag == 1))// || (pstruMac->PrevState != Wait_DIFS)) // Set the flag when backoff is paused
	{
		pstruMac->BackOffFreezeFlag = 0;	
		return 0;
	}
	pstruMac->dBackOffCounter = pstruMac->dBackOffTime - (dTime - pstruMac->dBackOffStartTime);	
	if(pstruMac->dBackOffCounter < 1) //<= 0.0)//1)//0)
	{	// check the medium 		
		if(pstruMac->Medium == IDLE) //Backoff done. Start transmission
		{	
			fn_NetSim_WLAN_StartTransmission();
			pstruMac->dBackOffCounter = 0;		
			pstruMac->BackOffFreezeFlag = 0; 
			return 1;  // Backoff successfull
		}
		else
		{
			pstruMac->BackOffFreezeFlag = 0; 
			fn_NetSim_WLAN_ChangeMacState(pstruMac,MAC_IDLE);					
			pstruMac->pstruMetrics->nBackoffFailedCount++;
		}
	}
	else // Goto IDLE
	{
		pstruMac->BackOffFreezeFlag = 0; 
		fn_NetSim_WLAN_ChangeMacState(pstruMac,MAC_IDLE);
	}	
	return 0;
}

/**
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
It pause the backoff by setting the residual / remaining backofftime.
i.e., event time - BackoffStartTime. It set the BackOffFreezeFlag. Change the mac state to IDLE.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
int fn_NetSim_WLAN_PauseBackOff(NETSIM_ID nDeviceId, NETSIM_ID nInterfaceId, double dEventTime)
{
	MAC_VAR *pstruMac = DEVICE_MACVAR(nDeviceId, nInterfaceId);	
	double t=pstruMac->dBackOffTime;
	// Set Freeze Flag 
	pstruMac->BackOffFreezeFlag = 1;
	pstruMac->dMediumBusyTime = dEventTime;
	pstruMac->dBackOffTime = pstruMac->dBackOffTime - (dEventTime - pstruMac->dBackOffStartTime);
	pstruMac->dBackOffStartTime = dEventTime;
	if(pstruMac->dBackOffTime < 0.0)
		fnNetSimError("Backoff time less than 0");
	// change the state
	fn_NetSim_WLAN_ChangeMacState(pstruMac,MAC_IDLE);
	return 0;
}
/**
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
This function is called when BachOffCounter reaches to 0 and Medium is IDLE.
It pause the backoff by setting the residual / remaining backofftime 
i.e., event time - BackoffStartTime. It set the BackOffFreezeFlag. Change the mac state to IDLE.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
int fn_NetSim_WLAN_StartTransmission()
{
	NETSIM_ID nDeviceId = pstruEventDetails->nDeviceId;
	NETSIM_ID nInterfaceId = pstruEventDetails->nInterfaceId;
	NETSIM_ID nPortocolId = pstruEventDetails->nProtocolId;
	MAC_VAR *pstruMacVar = DEVICE_MACVAR(nDeviceId,nInterfaceId);	
	PHY_VAR *pstruPhyVar = DEVICE_PHYVAR(nDeviceId,nInterfaceId);
	NetSim_PACKET *pstruPacket;	
	int dPacketSize=0;
	if(pstruMacVar->pstruTempData)
		pstruPacket = pstruMacVar->pstruTempData;
	else
	{	
		if(pstruPhyVar->nPHYprotocol==IEEE_802_11n||pstruPhyVar->nPHYprotocol==IEEE_802_11ac)
		{
			pstruPacket = fn_NetSim_WLAN_Packet_GetAggregatedPacketFromBuffer(pstruMacVar->pstruQosList,pstruEventDetails->nDeviceId,pstruEventDetails->nInterfaceId,1);
			pstruMacVar->pstruTempData = pstruPacket;
			while(pstruPacket)
			{
				dPacketSize = dPacketSize + (int)pstruPacket->pstruNetworkData->dPacketSize + MAC_OVERHEAD_802_11n;
				pstruPacket=pstruPacket->pstruNextPacket;
			}
			pstruPacket = pstruMacVar->pstruTempData ;
		}
		else
			pstruPacket = fn_NetSim_WLAN_Packet_GetPacketFromBuffer(pstruMacVar->pstruQosList,0);
		if(!pstruPacket)	
			return 0;
	}
	if(pstruPacket->nDestinationId) // Unicast packets
	{
		if(pstruPhyVar->nPHYprotocol==IEEE_802_11n||pstruPhyVar->nPHYprotocol==IEEE_802_11ac)
		{
			fn_NetSim_WLAN_CheckRTSThreshold(nDeviceId,nInterfaceId,dPacketSize);
		}
		else
		{
			fn_NetSim_WLAN_CheckRTSThreshold(nDeviceId,nInterfaceId,pstruPacket->pstruNetworkData->dPacketSize);
		}
		// Check Transmission Type, Change sate and update subevent
		switch(pstruMacVar->nRtsCtsFlag)
		{
		case 1:// RTS-CTS Mechanism			
			fn_NetSim_WLAN_ChangeMacState(pstruMacVar,TXing_RTS);		
			pstruEventDetails->nSubEventType = SEND_RTS;		
			break;
		default: // Basic Mechanism			
			fn_NetSim_WLAN_ChangeMacState(pstruMacVar,TXing_UNICAST);			
			pstruEventDetails->nSubEventType = SEND_MPDU;
			break;
		}
	}
	else // For Broadcast packets
	{
		fn_NetSim_WLAN_ChangeMacState(pstruMacVar,TXing_BROADCAST);		
		pstruEventDetails->nSubEventType = SEND_MPDU;		
	}	
	return 0;
}

/**
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
This function invoke the rts threshold calculation.If threshold less than frame size
then set flag as one to select rts/cts mechanism, 
otherwise, set as zero to select basic access mechanism
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
int fn_NetSim_WLAN_CheckRTSThreshold(NETSIM_ID nDeviceId,NETSIM_ID nInterfaceId, double dPacketSize)
{
	MAC_VAR *pstruMacVar = DEVICE_MACVAR(nDeviceId,nInterfaceId);
	if(dPacketSize > pstruMacVar->nRTSThreshold)//pstruMacVar->pstruTempData->pstruMacData->dPacketSize)
	{		
		pstruMacVar->nRtsCtsFlag = 1; // Select the rts/cts mechanism		
	}
	else
	{		
		pstruMacVar->nRtsCtsFlag = 0; // Select the basic access mechanism		
	}
	return 0;
}
/**
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
This function is called to send MPDU. Sender send the data by adding 
PHYSICAL_OUT event. Also add the AckTimeOut sub event for unicast packets.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/

int fn_NetSim_WLAN_SendMPDU(double** dCummulativeReceivedPower)
{
	double dTime,dAckTimeout, dAckTxtime;
	double dByteTime, dPacketSize;	
	NETSIM_ID nProtocolId = pstruEventDetails->nProtocolId;
	NETSIM_ID nDeviceId = pstruEventDetails->nDeviceId;
	NETSIM_ID nInterfaceId = pstruEventDetails->nInterfaceId;
	MAC_VAR *pstruMacVar = DEVICE_MACVAR(nDeviceId,nInterfaceId);
	PHY_VAR *pstruPhyVar = DEVICE_PHYVAR(nDeviceId,nInterfaceId);	
	NetSim_PACKET *pstruPacket;	
	dTime = pstruEventDetails->dEventTime;
	dPacketSize=0;
	//Get the packet from Accessbuffer and move it into next packet	
	//pstruPacket = fn_NetSim_Packet_GetPacketFromBuffer(NETWORK->ppstruDeviceList[nDeviceId-1]->ppstruInterfaceList[nInterfaceId-1]->pstruAccessInterface->pstruAccessBuffer,1);
	if(pstruMacVar->pstruTempData)
		pstruPacket = pstruMacVar->pstruTempData;
	else
	{

		if(pstruPhyVar->nPHYprotocol == IEEE_802_11n||pstruPhyVar->nPHYprotocol == IEEE_802_11ac)
		{
			//Get the Aggregated Packets. Packets with same receiver ID are aggregated and not 
			//according to the destination id.
			//Broadcast Packets are not aggregated.
			pstruPacket = fn_NetSim_WLAN_Packet_GetAggregatedPacketFromBuffer(pstruMacVar->pstruQosList,pstruEventDetails->nDeviceId,pstruEventDetails->nInterfaceId,1);

		}
		else
			pstruPacket = fn_NetSim_WLAN_Packet_GetPacketFromBuffer(pstruMacVar->pstruQosList,1);
		
		if(!pstruPacket)
			return 0;	
	}
	pstruEventDetails->pPacket = pstruPacket;	

	// Add the MAC header

	while(pstruPacket)
	{

		fn_NetSim_WLAN_Add_MAC_Header(nDeviceId,nInterfaceId,pstruPacket);
		pstruPacket=pstruPacket->pstruNextPacket;
	}
	pstruPacket = pstruEventDetails->pPacket;
	//Set the receiver sensitivity  and call Set data rate function
	pstruPhyVar->dTotalReceivedPower = dCummulativeReceivedPower[nDeviceId][pstruPacket->nReceiverId];
	fn_NetSim_WLAN_SetDataRate(nDeviceId, nInterfaceId,pstruPacket->nReceiverId);	 
	// Add PHYSICAL OUT event to send data	
	pstruEventDetails->dEventTime = dTime;
	pstruEventDetails->dPacketSize = pstruPacket->pstruMacData->dPacketSize;

	if((pstruPhyVar->nPHYprotocol != IEEE_802_11n && pstruPhyVar->nPHYprotocol != IEEE_802_11ac) || pstruEventDetails->pPacket->nDestinationId==0)
		pstruEventDetails->nSubEventType = 0;
	else
		pstruEventDetails->nSubEventType = AMPDU_FRAME;
	
	pstruEventDetails->nEventType = PHYSICAL_OUT_EVENT;	
	pstruEventDetails->nPacketId = pstruPacket->nPacketId;
	if(pstruPacket->pstruAppData)
		pstruEventDetails->nSegmentId = pstruPacket->pstruAppData->nSegmentId;
	else
		pstruEventDetails->nSegmentId = 0;
	fnpAddEvent(pstruEventDetails);

	dByteTime = 8/pstruPhyVar->dDataRate; 	
	dPacketSize=0;
	pstruPacket=pstruEventDetails->pPacket;
	while(pstruPacket)
	{
		dPacketSize +=pstruPacket->pstruMacData->dPacketSize;
		pstruPacket=pstruPacket->pstruNextPacket;
	}

	pstruMacVar->dTransmissionTime = pstruPhyVar->pstruPhyChar->naPreambleLength\
		+ pstruPhyVar->pstruPhyChar->naPLCPHeaderLength\
		+ dPacketSize * dByteTime;

	pstruPacket=pstruEventDetails->pPacket;

	//If Condition to add Ack Timeout for Unicast Packets
	if(pstruPacket->nDestinationId != 0) // Unicast packets
	{
		if(pstruPhyVar->nPHYprotocol != IEEE_802_11n && pstruPhyVar->nPHYprotocol != IEEE_802_11ac)
		{
			dAckTimeout = ceil( pstruPhyVar->pstruPhyChar->naPreambleLength\
				+ pstruPhyVar->pstruPhyChar->naPLCPHeaderLength\
				+ (pstruPacket->pstruMacData->dPacketSize * dByteTime) \
				+ pstruPhyVar->pstruPhyChar->naSIFSTime\
				+ pstruPhyVar->pstruPhyChar->naPreambleLength\
				+ pstruPhyVar->pstruPhyChar->naPLCPHeaderLength\
				+ ((ACK_SIZE * 8)/pstruPhyVar->dControlFrameDataRate)) + 2;	//+ 1; // 1 for truncate to next value	
			pstruEventDetails->nSubEventType = ACK_TIMEOUT;
		}
		else
		{
			pstruPacket=pstruEventDetails->pPacket;
			if(pstruPhyVar->nPHYprotocol==IEEE_802_11n)
			{
				pstruMacVar->dTransmissionTime = fn_NetSim_WLAN_802_11n_TxTimeCalculation(dPacketSize,1,nDeviceId,nInterfaceId,1);
				dPacketSize = 32;
				dAckTxtime = fn_NetSim_WLAN_802_11n_TxTimeCalculation(dPacketSize,0,nDeviceId,nInterfaceId,1);
			}
			else if(pstruPhyVar->nPHYprotocol==IEEE_802_11ac)
			{
				pstruMacVar->dTransmissionTime = fn_NetSim_WLAN_802_11ac_TxTimeCalculation(dPacketSize,1,nDeviceId,nInterfaceId,1);
				dPacketSize = 152;
				dAckTxtime = fn_NetSim_WLAN_802_11ac_TxTimeCalculation(dPacketSize,0,nDeviceId,nInterfaceId,1);
			}
			else
			{
				dAckTxtime =0;
			}
			dAckTimeout = pstruMacVar->dTransmissionTime \
				+ pstruPhyVar->pstruPhyChar->naSIFSTime \
				+ dAckTxtime + 1;	
			pstruEventDetails->nSubEventType = BLOCK_ACK_TIMEOUT;
		}
		// Change the state
		pstruPacket=pstruEventDetails->pPacket;
		fn_NetSim_WLAN_ChangeMacState(pstruMacVar,Wait_BlockACK);
		// Add ACK Timeout event 
		pstruEventDetails->dEventTime = dTime + dAckTimeout;	
		
		pstruEventDetails->nEventType = TIMER_EVENT;
		pstruEventDetails->pPacket = NULL;
		fnpAddEvent(pstruEventDetails);				
		// Keep copy of packet for retransmission if transmission failure
		pstruMacVar->pstruTempData = fn_NetSim_Packet_CopyAggregatedPacket(pstruPacket);		
		pstruMacVar->nRetryCount++;				
		pstruMacVar->pstruMetrics->nTransmittedFrameCountUnicast++;
		pstruMacVar->nAckReceivedFlag = 0;
	}
	else // broadcast packet	
	{
		if(pstruMacVar->pstruTempData)
		{
			pstruMacVar->pstruTempData=NULL;
			//A broadcast packet should not be stored in retransmit buffer
		}
		fn_NetSim_WLAN_ChangeMacState(pstruMacVar,MAC_IDLE);
		pstruMacVar->pstruMetrics->nTransmittedFrameCountBroadcast++;
	}
	// Change Radio state of Transmitter
	WLAN_CHANGERADIOSTATE(nDeviceId,nInterfaceId,TRX_ON_BUSY);// nTxFlag = 1 for Transmitter
	return 0;
}

/**
This function adds MAC header to the packet before sending to Physical layer.
*/
int fn_NetSim_WLAN_Add_MAC_Header(NETSIM_ID nDeviceId,NETSIM_ID nInterfaceId, NetSim_PACKET *pstruPacket)
{
	PHY_VAR *pstruPhyVar = DEVICE_PHYVAR(nDeviceId,nInterfaceId);
	MAC_VAR *pstruMacVar = DEVICE_MACVAR(nDeviceId,nInterfaceId);
	MAC_HEADER *pstruMacHeader;
	pstruMacHeader = fnpAllocateMemory(1, sizeof *pstruMacHeader);
	pstruMacHeader->pstruFrameControl = fnpAllocateMemory(1,sizeof(FRAME_CONTROL));	
	pstruMacHeader->pstruFrameControl->FrameType = DATA;
	pstruMacHeader->pstruFrameControl->usnSubType = Data;
	switch(NETWORK->ppstruDeviceList[nDeviceId-1]->ppstruInterfaceList[nInterfaceId-1]->pstruPhysicalLayer->pstruNetSimLinks->nLinkType)
	{
	case LinkType_P2MP:
		if(nDeviceId != pstruPhyVar->nBSSID )// && pstruPhyVar->BssType == INFRASTRUCTURE)
		{	
			pstruMacHeader->pstruFrameControl->bToDS = 1;
			pstruMacHeader->pstruFrameControl->bFromDS = 0;// Data is to AP(infra = 0 and ToDS = 1)
			pstruMacHeader->Address1 = pstruPhyVar->szBSSID;
			pstruMacHeader->Address2 = pstruPacket->pstruMacData->szSourceMac;
			if(pstruPacket->nDestinationId != 0) // unicast packets
				pstruMacHeader->Address3 = (pstruPacket->pstruMacData->szDestMac);
			else
				pstruMacHeader->Address3 = NULL;
			pstruPacket->nReceiverId = pstruPhyVar->nBSSID;	
			pstruPacket->nSourceId = nDeviceId;
			pstruPacket->nTransmitterId = nDeviceId;		
			pstruPacket->nDestinationId = pstruPacket->nDestinationId;
		}
		else
		{
			pstruMacHeader->pstruFrameControl->bToDS = 0;
			pstruMacHeader->pstruFrameControl->bFromDS = 1;// Data from the AP
			if(pstruPacket->nDestinationId != 0)
				pstruMacHeader->Address1 = (pstruPacket->pstruMacData->szDestMac);
			else
				pstruMacHeader->Address1  = NULL;
			pstruMacHeader->Address2 = pstruPhyVar->szBSSID;
			pstruMacHeader->Address3 = pstruPacket->pstruMacData->szSourceMac;
			pstruPacket->nReceiverId  = pstruPacket->nDestinationId; //of Data packet
			pstruPacket->nSourceId = pstruPacket->nSourceId;
			pstruPacket->nTransmitterId = pstruPhyVar->nBSSID;		
			pstruPacket->nDestinationId = pstruPacket->nDestinationId;
		}
		break;
	case LinkType_MP2MP:
			pstruMacHeader->pstruFrameControl->bToDS = 0;// Both ToDS and FromDS bits 0 for IBSS
			pstruMacHeader->pstruFrameControl->bFromDS = 0;
			if(pstruPacket->nDestinationId != 0) // Unicast Frames;
				pstruMacHeader->Address1 = (pstruPacket->pstruMacData->szDestMac);
			else
				pstruMacHeader->Address1  = NULL;
			pstruMacHeader->Address2 = pstruPacket->pstruMacData->szSourceMac;
			pstruPacket->nReceiverId  = pstruPacket->nReceiverId;
			pstruPacket->nSourceId = pstruPacket->nSourceId;
			pstruPacket->nTransmitterId = pstruPacket->nTransmitterId;
			pstruPacket->nDestinationId = pstruPacket->nDestinationId;
		break;
	default:
		break;
	}	
	pstruMacHeader->Address4 =  NULL;
	pstruMacHeader->dBOTime = pstruMacVar->dBackOffTime;
	if(pstruPhyVar->nPHYprotocol == IEEE_802_11n||pstruPhyVar->nPHYprotocol == IEEE_802_11ac)
	{
		pstruPacket->pstruMacData->dOverhead = MAC_OVERHEAD_802_11n;
		//This is for the 4bytes of padding
		pstruPacket->pstruMacData->dOverhead += (4-((int)pstruPacket->pstruNetworkData->dPacketSize % 4))%4; 
	}
	else
		pstruPacket->pstruMacData->dOverhead = MAC_OVERHEAD;
	pstruPacket->pstruMacData->dPayload = pstruPacket->pstruNetworkData->dPacketSize;
	pstruPacket->pstruMacData->dPacketSize = pstruPacket->pstruMacData->dPayload \
		+ pstruPacket->pstruMacData->dOverhead ;	
	pstruPacket->pstruMacData->dEndTime = pstruEventDetails->dEventTime;
	pstruPacket->pstruMacData->nMACProtocol = pstruEventDetails->nProtocolId;
	pstruPacket->pstruMacData->Packet_MACProtocol = pstruMacHeader;	
	return 0;
}

/**
This function is called to create and send an Ack after receiving the DATA 
from the destination or from the AP
*/
int fn_NetSim_WLAN_SendACK()
{
	double dTime;
	NETSIM_ID nDeviceId = pstruEventDetails->nDeviceId;
	NETSIM_ID nInterfaceId = pstruEventDetails->nInterfaceId;
	MAC_VAR *pstruMacVar = DEVICE_MACVAR(nDeviceId,nInterfaceId);
	PHY_VAR *pstruPhyVar = DEVICE_PHYVAR(nDeviceId,nInterfaceId);
	NetSim_PACKET *pstruPacket = pstruEventDetails->pPacket;
	NetSim_PACKET *pstruAckPkt;
	ACK_FRAME *pstruAck;
	dTime = pstruEventDetails->dEventTime;
	// Create ACK Frame	
	pstruAckPkt = fn_NetSim_Packet_CreatePacket(MAC_LAYER);
	pstruAckPkt->nPacketType = PacketType_Control;
	pstruAckPkt->nPacketPriority = Priority_High;
	pstruAckPkt->nControlDataType = WLAN_CNTRL_PKT(WLAN_ACK,pstruEventDetails->nProtocolId);
	pstruAck =  fnpAllocateMemory(1, sizeof(ACK_FRAME));
	pstruAck->pstruFrameControl = fnpAllocateMemory(1,sizeof(FRAME_CONTROL));	
	pstruAck->pstruFrameControl->FrameType = CONTROL;
	pstruAck->pstruFrameControl->usnSubType = ACK;	
	pstruAck->RA =((MAC_HEADER*)(pstruPacket->pstruMacData->Packet_MACProtocol))->Address2;
	switch(pstruPhyVar->BssType)
	{
	case INFRASTRUCTURE:
		if(nDeviceId != pstruPhyVar->nBSSID )// && pstruPhyVar->BssType == INFRASTRUCTURE)
		{	
			pstruAck->pstruFrameControl->bFromDS = 0;// Data is to AP(infra = 0 and ToDS = 1)
			pstruAck->pstruFrameControl->bToDS = 1;
			pstruAckPkt->nReceiverId = pstruPhyVar->nBSSID;		
		}
		else
		{
			pstruAck->pstruFrameControl->bFromDS = 1;// Data from AP
			pstruAck->pstruFrameControl->bToDS = 0;		
			pstruAckPkt->nReceiverId  = pstruPacket->nSourceId; //of Data packet
		}
		// Update packet fields
		pstruAckPkt->nSourceId = nDeviceId;
		pstruAckPkt->nTransmitterId = nDeviceId;
		pstruAckPkt->nDestinationId = pstruPacket->nSourceId; //of Data packet
		break;
	case INDEPENDENT:
		pstruAck->pstruFrameControl->bFromDS = 0;// Both 0 for IBSS
		pstruAck->pstruFrameControl->bToDS = 0;		
		pstruAckPkt->nReceiverId  = pstruPacket->nTransmitterId; //of Data packet
		// Update packet fields
		pstruAckPkt->nSourceId = nDeviceId;
		pstruAckPkt->nTransmitterId = nDeviceId;
		pstruAckPkt->nDestinationId = pstruPacket->nTransmitterId; //of Data packet
		break;
	default:
		break;
	}
	fn_NetSim_WLAN_SetDataRate(nDeviceId, nInterfaceId,pstruAckPkt->nReceiverId);
	pstruAckPkt->pstruMacData->Packet_MACProtocol = pstruAck;
	pstruAckPkt->pstruMacData->dArrivalTime = dTime;
	pstruAckPkt->pstruMacData->dStartTime = dTime;
	pstruAckPkt->pstruMacData->dEndTime = dTime; // need to check
	pstruAckPkt->pstruMacData->dOverhead  = ACK_SIZE;
	pstruAckPkt->pstruMacData->dPacketSize = pstruAckPkt->pstruMacData->dOverhead;
	pstruAckPkt->pstruMacData->nMACProtocol = MAC_PROTOCOL_IEEE802_11;
	pstruAckPkt->nPacketId = 0;
	// Add SEND ACK subevent
	pstruEventDetails->dEventTime = dTime;
	pstruEventDetails->dPacketSize = pstruAckPkt->pstruMacData->dPacketSize;
	pstruEventDetails->nSubEventType = 0;
	pstruEventDetails->nEventType = PHYSICAL_OUT_EVENT;
	pstruEventDetails->pPacket = pstruAckPkt;
	fnpAddEvent(pstruEventDetails);	
	// Change the state
	fn_NetSim_WLAN_ChangeMacState(pstruMacVar,MAC_IDLE);
	//Free the packet
	fn_NetSim_Packet_FreePacket(pstruPacket);
	pstruPacket = NULL;	
	return 0;
}
/**
This function is used to send the cummulative acknowledgement
*/
int fn_NetSim_WLAN_SendBlockACK()
{
	double dTime;
	int nReceiverId, nDestinationId, nSourceId, nTransmitterId;
	NETSIM_ID nDeviceId = pstruEventDetails->nDeviceId;
	NETSIM_ID nInterfaceId = pstruEventDetails->nInterfaceId;
	MAC_VAR *pstruMacVar = WLAN_MAC(nDeviceId,nInterfaceId);
	PHY_VAR *pstruPhyVar = WLAN_PHY(nDeviceId,nInterfaceId);
	NetSim_PACKET *pstruAckPkt = pstruEventDetails->pPacket;
	BlockACK_FRAME *pstruAck = (BlockACK_FRAME*)pstruEventDetails->pPacket->pstruMacData->Packet_MACProtocol;
	
	nReceiverId=pstruAckPkt->nReceiverId;
	nDestinationId=pstruAckPkt->nDestinationId;
	nTransmitterId=pstruAckPkt->nTransmitterId;
	nSourceId=pstruAckPkt->nSourceId;

	dTime = pstruEventDetails->dEventTime;
	
	switch(pstruPhyVar->BssType)
	{
	case INFRASTRUCTURE:
		if(nDeviceId != pstruPhyVar->nBSSID )// && pstruPhyVar->BssType == INFRASTRUCTURE)
		{	
			pstruAck->pstruFrameControl->bFromDS = 0;// Data is to AP(infra = 0 and ToDS = 1)
			pstruAck->pstruFrameControl->bToDS = 1;
			pstruAckPkt->nReceiverId = pstruPhyVar->nBSSID;		
		}
		else
		{
			pstruAck->pstruFrameControl->bFromDS = 1;// Data from AP
			pstruAck->pstruFrameControl->bToDS = 0;		
			pstruAckPkt->nReceiverId  =nSourceId; //of Data packet
		}
		// Update packet fields
		pstruAckPkt->nSourceId = nDeviceId;
		pstruAckPkt->nTransmitterId = nDeviceId;
		pstruAckPkt->nDestinationId = nSourceId; //of Data packet
		break;
	case INDEPENDENT:
		pstruAck->pstruFrameControl->bFromDS = 0;// Both 0 for IBSS
		pstruAck->pstruFrameControl->bToDS = 0;		
		pstruAckPkt->nReceiverId  = nTransmitterId; //of Data packet
		// Update packet fields
		pstruAckPkt->nSourceId = nDeviceId;
		pstruAckPkt->nTransmitterId = nDeviceId;
		pstruAckPkt->nDestinationId = nTransmitterId; //of Data packet
		break;
	default:
		break;
	}
	fn_NetSim_WLAN_SetDataRate(nDeviceId, nInterfaceId,pstruAckPkt->nReceiverId);
	pstruAckPkt->pstruMacData->dArrivalTime = dTime;
	pstruAckPkt->pstruMacData->dStartTime = dTime;
	pstruAckPkt->pstruMacData->dEndTime = dTime; // need to check
	if(pstruPhyVar->nPHYprotocol==IEEE_802_11n)
		pstruAckPkt->pstruMacData->dOverhead  = BlockACK_SIZE_802_11n;			//32 Bytes
	else if(pstruPhyVar->nPHYprotocol==IEEE_802_11ac)
		pstruAckPkt->pstruMacData->dOverhead  = BlockACK_SIZE_802_11ac;			//152 Bytes
	pstruAckPkt->pstruMacData->dPacketSize = pstruAckPkt->pstruMacData->dOverhead;
	pstruAckPkt->pstruMacData->nMACProtocol = MAC_PROTOCOL_IEEE802_11;
	pstruAckPkt->nPacketId = 0;
	// Add SEND ACK subevent
	pstruEventDetails->dEventTime = dTime;
	pstruEventDetails->dPacketSize = pstruAckPkt->pstruMacData->dPacketSize;
	pstruEventDetails->nSubEventType = AMPDU_FRAME;
	pstruEventDetails->nEventType = PHYSICAL_OUT_EVENT;
	pstruEventDetails->nPacketId = pstruAckPkt->nPacketId;
	pstruEventDetails->pPacket = pstruAckPkt;
	fnpAddEvent(pstruEventDetails);	
	// Change the state
	fn_NetSim_WLAN_ChangeMacState(pstruMacVar,MAC_IDLE);
	
	return 0;
}


/**
This function is called to create send RTS after receiving the RTS.
If the packetsize>RTSthreshold then sender send the RTS. Add CTSTimeout event.
*/
int fn_NetSim_WLAN_SendRTS(double** dCummulativeReceivedPower)
{
	double dTime,dCtsTimeout,dCtsTxTime;
	int nNAV;
	double dPacketSize = 0;
	NETSIM_ID nDeviceId = pstruEventDetails->nDeviceId;
	NETSIM_ID nInterfaceId = pstruEventDetails->nInterfaceId;
	MAC_VAR *pstruMacVar = WLAN_MAC(nDeviceId,nInterfaceId);
	PHY_VAR *pstruPhyVar = WLAN_PHY(nDeviceId,nInterfaceId);
	NetSim_PACKET *pstruPacket;
	NetSim_PACKET *pstruRtsPkt, *pstruCopyPkt;
	RTS_FRAME *pstruRts;
	dTime = pstruEventDetails->dEventTime;
	//Get the packet from 	
	if(pstruMacVar->pstruTempData)
		pstruPacket = pstruMacVar->pstruTempData;
	else
	{
		pstruPacket = fn_NetSim_WLAN_Packet_GetPacketFromBuffer(pstruMacVar->pstruQosList,0);
		if(!pstruPacket)
			return 0;	
	}
	// Create RTS Frame	
	pstruRtsPkt = fn_NetSim_Packet_CreatePacket(MAC_LAYER);
	pstruRtsPkt->nPacketType = PacketType_Control;
	pstruRtsPkt->nPacketPriority = Priority_High;
	pstruRtsPkt->nControlDataType= WLAN_CNTRL_PKT(WLAN_RTS,pstruEventDetails->nProtocolId);
	pstruRts =  fnpAllocateMemory(1, sizeof(RTS_FRAME));
	pstruRts->pstruFrameControl = fnpAllocateMemory(1,sizeof(FRAME_CONTROL));	
	pstruRts->pstruFrameControl->FrameType = CONTROL;
	pstruRts->pstruFrameControl->usnSubType = RTS;
	pstruRts->TA = NETWORK->ppstruDeviceList[nDeviceId-1]->ppstruInterfaceList[nInterfaceId-1]->pstruMACLayer->szMacAddress;
	if(nDeviceId != pstruPhyVar->nBSSID ) 
	{
		pstruRts->RA = pstruPhyVar->szBSSID;
		pstruRts->pstruFrameControl->bFromDS = 0;// Data from AP
		pstruRts->pstruFrameControl->bToDS = 1;
		pstruRtsPkt->nReceiverId = pstruPhyVar->nBSSID;		
	}
	else
	{
		pstruRts->RA = pstruPacket->pstruMacData->szDestMac;
		pstruRts->pstruFrameControl->bFromDS = 1;// Data is to AP
		pstruRts->pstruFrameControl->bToDS = 0;		
		pstruRtsPkt->nReceiverId  = pstruPacket->nDestinationId;
	}
	//Set the data rate and receiver sensitivity 
	pstruPhyVar->dTotalReceivedPower = dCummulativeReceivedPower[nDeviceId][pstruRtsPkt->nReceiverId];
	fn_NetSim_WLAN_SetDataRate(nDeviceId, nInterfaceId,pstruRtsPkt->nReceiverId);
	// calculate the NAV value and assign to Duration field.
	if(pstruPhyVar->nPHYprotocol == IEEE_802_11n)
	{
		while(pstruPacket)
		{
			dPacketSize =dPacketSize + pstruPacket->pstruNetworkData->dPacketSize + MAC_OVERHEAD_802_11n;
			pstruPacket=pstruPacket->pstruNextPacket;
		}
		pstruPacket=pstruMacVar->pstruTempData;
	}
	else if(pstruPhyVar->nPHYprotocol == IEEE_802_11ac)
	{
		while(pstruPacket)
		{
			dPacketSize =dPacketSize + pstruPacket->pstruNetworkData->dPacketSize + MAC_OVERHEAD_802_11ac;
			pstruPacket=pstruPacket->pstruNextPacket;
		}
		pstruPacket=pstruMacVar->pstruTempData;
	}
	else
	{
		dPacketSize =pstruPacket->pstruNetworkData->dPacketSize + MAC_OVERHEAD;
	}
	
	fn_NetSim_WLAN_CalculateNAV_Duration(nDeviceId,nInterfaceId,dPacketSize,&nNAV,WLAN_RTS);
	pstruRts->nDuration = (short int) nNAV;
	pstruRts->dBOTime = pstruMacVar->dBackOffTime;
	// Update Packet fields.
	pstruRtsPkt->nSourceId = nDeviceId;
	pstruRtsPkt->nTransmitterId = nDeviceId;
	pstruRtsPkt->nDestinationId = pstruPacket->nDestinationId;//0;  // Broadcast RTS   /
	pstruRtsPkt->pstruMacData->Packet_MACProtocol = pstruRts;
	pstruRtsPkt->pstruMacData->dArrivalTime = dTime;
	pstruRtsPkt->pstruMacData->dStartTime = dTime;
	pstruRtsPkt->pstruMacData->dEndTime = dTime; // need to check
	pstruRtsPkt->pstruMacData->dOverhead  = RTS_SIZE;
	pstruRtsPkt->pstruMacData->dPacketSize = pstruRtsPkt->pstruMacData->dOverhead;
	pstruRtsPkt->pstruMacData->nMACProtocol = pstruEventDetails->nProtocolId;
	pstruRtsPkt->nPacketId = 0;
	// Add event to send RTS
	pstruEventDetails->dEventTime = dTime;	
	pstruEventDetails->nApplicationId = 0;
	pstruEventDetails->nPacketId = pstruEventDetails->nPacketId;
	pstruEventDetails->dPacketSize = pstruRtsPkt->pstruMacData->dPacketSize;
	pstruEventDetails->nSubEventType = AMPDU_FRAME;
	pstruEventDetails->nEventType = PHYSICAL_OUT_EVENT;
	pstruEventDetails->pPacket = pstruRtsPkt;
	fnpAddEvent(pstruEventDetails);	
	if(pstruPhyVar->nPHYprotocol != IEEE_802_11n && pstruPhyVar->nPHYprotocol != IEEE_802_11ac)
	{
	// calculate the time for CTS timeout
	dCtsTimeout = ((RTS_SIZE * 8)/pstruPhyVar->dControlFrameDataRate \
		+ pstruPhyVar->pstruPhyChar->naPreambleLength \
		+ pstruPhyVar->pstruPhyChar->naPLCPHeaderLength \
		+ pstruPhyVar->pstruPhyChar->naSIFSTime\
		+ (CTS_SIZE * 8)/pstruPhyVar->dControlFrameDataRate \
		+ pstruPhyVar->pstruPhyChar->naPreambleLength \
		+ pstruPhyVar->pstruPhyChar->naPLCPHeaderLength \
		+ pstruPhyVar->pstruPhyChar->naSIFSTime ) + 1;
	}
	else if(pstruPhyVar->nPHYprotocol == IEEE_802_11n)
	{
		dPacketSize = pstruRtsPkt->pstruMacData->dPacketSize; //RTS size
		pstruMacVar->dTransmissionTime = fn_NetSim_WLAN_802_11n_TxTimeCalculation(dPacketSize,0,nDeviceId,nInterfaceId,1);
		dPacketSize = 14;
		dCtsTxTime = fn_NetSim_WLAN_802_11n_TxTimeCalculation(dPacketSize,0,nDeviceId,nInterfaceId,1);
		dCtsTimeout = pstruMacVar->dTransmissionTime \
				+ pstruPhyVar->pstruPhyChar->naSIFSTime \
				+ dCtsTxTime + 1;
	}
	else if(pstruPhyVar->nPHYprotocol == IEEE_802_11ac)
	{
		dPacketSize = pstruRtsPkt->pstruMacData->dPacketSize; //RTS size
		pstruMacVar->dTransmissionTime = fn_NetSim_WLAN_802_11ac_TxTimeCalculation(dPacketSize,0,nDeviceId,nInterfaceId,1);
		dPacketSize = 14;
		dCtsTxTime = fn_NetSim_WLAN_802_11ac_TxTimeCalculation(dPacketSize,0,nDeviceId,nInterfaceId,1);
		dCtsTimeout = pstruMacVar->dTransmissionTime \
				+ pstruPhyVar->pstruPhyChar->naSIFSTime \
				+ dCtsTxTime + 1;
	}
	else
	{
		fnNetSimError("Unknown %d phy protocol for WLAN\n",pstruPhyVar->nPHYprotocol);
		dCtsTimeout = 0;
	}

	// Add CTS timeout evnt 			nApplicationId	0	unsigned short

	pstruCopyPkt = fn_NetSim_Packet_CopyPacket(pstruRtsPkt);
	pstruEventDetails->dEventTime = dTime + dCtsTimeout;	
	pstruEventDetails->nSubEventType = CTS_TIMEOUT;	
	pstruEventDetails->nEventType = TIMER_EVENT;
	pstruEventDetails->pPacket = pstruCopyPkt;	
	fnpAddEvent(pstruEventDetails);	
	// set or reset respective flags
	pstruMacVar->nRtsSent = 1;
	pstruMacVar->nCTSTimeOutTriggered = 1;	
	pstruMacVar->nCtsReceivedFlag = 0;	
	pstruMacVar->nRetryCount++;	
	fn_NetSim_WLAN_ChangeMacState(pstruMacVar,Wait_CTS);

	WLAN_CHANGERADIOSTATE(nDeviceId,nInterfaceId,TRX_ON_BUSY);// nTxFlag = 1 for Transmitter
	return 0;
}

/**
This function is called to create and send CTS frame after receiving the 
RTS frame from device which address matches to pstruRts->RA.
*/
int fn_NetSim_WLAN_SendCTS()
{
	double dTime;
	int nNAV;
	NETSIM_ID nDeviceId = pstruEventDetails->nDeviceId;
	NETSIM_ID nInterfaceId = pstruEventDetails->nInterfaceId;
	MAC_VAR *pstruMacVar = WLAN_MAC(nDeviceId,nInterfaceId);
	PHY_VAR *pstruPhyVar = WLAN_PHY(nDeviceId,nInterfaceId);
	NetSim_PACKET *pstruPacket = pstruEventDetails->pPacket;
	NetSim_PACKET *pstruCtsPkt;
	CTS_FRAME *pstruCts;
	dTime = pstruEventDetails->dEventTime;
	// Create CTS Frame	
	pstruCtsPkt = fn_NetSim_Packet_CreatePacket(MAC_LAYER);
	pstruCtsPkt->nPacketType = PacketType_Control;
	pstruCtsPkt->nPacketPriority = Priority_High;
	pstruCtsPkt->nControlDataType = WLAN_CNTRL_PKT(WLAN_CTS,pstruEventDetails->nProtocolId);
	pstruCts = (CTS_FRAME*)calloc(1, sizeof(CTS_FRAME));
	pstruCts->pstruFrameControl = (FRAME_CONTROL*)calloc(1,sizeof(FRAME_CONTROL));	
	pstruCts->pstruFrameControl->FrameType = CONTROL;
	pstruCts->pstruFrameControl->usnSubType = CTS;
	pstruCts->RA =((RTS_FRAME*)(pstruPacket->pstruMacData->Packet_MACProtocol))->TA;
	if(nDeviceId != pstruPhyVar->nBSSID )
	{		
		pstruCts->pstruFrameControl->bFromDS = 0;// Data is to AP(infra = 0 and ToDS = 1)
		pstruCts->pstruFrameControl->bToDS = 1;
		pstruCtsPkt->nReceiverId = pstruPhyVar->nBSSID;				
	}
	else
	{
		pstruCts->pstruFrameControl->bFromDS = 1;// Data from AP
		pstruCts->pstruFrameControl->bToDS = 0;		
		pstruCtsPkt->nReceiverId  = pstruPacket->nSourceId; //of RTS packet
	}
	//Set Data RAte
	fn_NetSim_WLAN_SetDataRate(nDeviceId, nInterfaceId,pstruCtsPkt->nReceiverId);
	// calculate the NAV value and assign to Duration field.	
	nNAV = ((RTS_FRAME*)(pstruPacket->pstruMacData->Packet_MACProtocol))->nDuration;
	fn_NetSim_WLAN_CalculateNAV_Duration(nDeviceId,nInterfaceId,0,&nNAV,WLAN_CTS);
	pstruCts->nDuration = (short int)nNAV;
	// Update Packet fields.
	pstruCtsPkt->nSourceId = nDeviceId;
	pstruCtsPkt->nTransmitterId = nDeviceId;
	pstruCtsPkt->nDestinationId = 0;// Broadcast CTS
	pstruCtsPkt->pstruMacData->Packet_MACProtocol = pstruCts;
	pstruCtsPkt->pstruMacData->dArrivalTime = dTime;
	pstruCtsPkt->pstruMacData->dStartTime = dTime;
	pstruCtsPkt->pstruMacData->dEndTime = dTime; // need to check
	pstruCtsPkt->pstruMacData->dOverhead  = CTS_SIZE;
	pstruCtsPkt->pstruMacData->dPacketSize = pstruCtsPkt->pstruMacData->dOverhead;
	pstruCtsPkt->pstruMacData->nMACProtocol = pstruEventDetails->nProtocolId;
	pstruCtsPkt->nPacketId = 0;
	// Add event to send CTS
	pstruEventDetails->dEventTime = dTime;
	pstruEventDetails->dPacketSize = pstruCtsPkt->pstruMacData->dPacketSize;
	pstruEventDetails->nSubEventType = AMPDU_FRAME;
	pstruEventDetails->nEventType = PHYSICAL_OUT_EVENT;
	pstruEventDetails->pPacket = pstruCtsPkt;
	fnpAddEvent(pstruEventDetails);	
	// Set Cts sent 
	pstruMacVar->nCtsSent = 1;
	fn_NetSim_WLAN_ChangeMacState(pstruMacVar,Wait_DATA);
	// Free RTS packet
	fn_NetSim_Packet_FreePacket(pstruPacket);
	pstruPacket = NULL;
	return 0;
}

/**
This function is called when device receives the MPDU frame. 
*/
int fn_NetSim_WLAN_ReceiveMPDU()
{
	NetSim_PACKET *pstruCopyPacket;
	NetSim_PACKET *pstruPacketHead=NULL;
	NetSim_PACKET *pstruBlockAckPkt;
	BlockACK_FRAME *pstruBlockAck;
	NetSim_EVENTDETAILS pevent;
	int i;
	int numOfPacketsAggregated;
	double dTime = pstruEventDetails->dEventTime;

	NETSIM_ID nDeviceId = pstruEventDetails->nDeviceId;
	NETSIM_ID nInterfaceId = pstruEventDetails->nInterfaceId;	
	MAC_VAR *pstruMacVar = WLAN_MAC(nDeviceId,nInterfaceId);
	PHY_VAR *pstruPhyVar = WLAN_PHY(nDeviceId,nInterfaceId); 
	NetSim_PACKET *pstruPacket = pstruEventDetails->pPacket;
	
	if(pstruPhyVar->nPHYprotocol==IEEE_802_11n)
	{
		numOfPacketsAggregated = MAX_802_11n_AMPDU_SIZE;
	}
	else if(pstruPhyVar->nPHYprotocol==IEEE_802_11ac)
	{
		numOfPacketsAggregated = MAX_802_11ac_AMPDU_SIZE;
	}
	while(pstruPacket)
	{
		pstruPacket->pstruMacData->dArrivalTime = dTime;
		pstruPacket->pstruMacData->dStartTime = dTime;
		pstruPacket->pstruMacData->dEndTime = dTime;
		if(pstruPhyVar->nPHYprotocol == IEEE_802_11n||pstruPhyVar->nPHYprotocol == IEEE_802_11ac) 
		{
			pstruPacket->pstruMacData->dPayload=pstruPacket->pstruPhyData->dPacketSize-pstruPacket->pstruMacData->dOverhead;
		}
		else
			pstruPacket->pstruMacData->dPayload=pstruPacket->pstruPhyData->dPacketSize-MAC_OVERHEAD;
		pstruPacket->pstruMacData->dPacketSize = pstruPacket->pstruMacData->dPayload;
		pstruPacket->pstruMacData->dOverhead= 0;
		pstruPacket=pstruPacket->pstruNextPacket;
	}
	pstruPacket=pstruEventDetails->pPacket;
	switch(pstruEventDetails->nProtocolId)
	{
	case MAC_PROTOCOL_IEEE802_11:	
		switch(pstruPhyVar->BssType)
		{
		case INFRASTRUCTURE:
			switch(pstruPacket->nDestinationId)
			{
			case 0: // broadcast packet			
				pstruMacVar->pstruMetrics->nReceivedFrameCountBroadcast++;
				if(nDeviceId != pstruPhyVar->nBSSID) // W/L station
				{	//Free the MAC Data			
					fn_NetSim_WLAN_FreePacket(pstruPacket);
					pstruPacket->pstruMacData->nMACProtocol = fn_NetSim_Stack_GetMacProtocol(nDeviceId,nInterfaceId);
					pstruEventDetails->dEventTime = pstruEventDetails->dEventTime;				
					pstruEventDetails->nProtocolId = fn_NetSim_Stack_GetNWProtocol(nDeviceId);
					pstruEventDetails->dPacketSize = pstruPacket->pstruMacData->dPacketSize;						
					pstruEventDetails->pPacket = pstruPacket;
					pstruEventDetails->nSubEventType = 0;
					pstruEventDetails->nEventType = NETWORK_IN_EVENT;
					fnpAddEvent(pstruEventDetails);
					// Change the state. Add MAC OUT event if medium free
					fn_NetSim_WLAN_ChangeMacState(pstruMacVar,MAC_IDLE);				
					if(pstruMacVar->Medium)
						fn_NetSim_WLAN_CheckPacketsInPacketlist(nDeviceId, nInterfaceId);

				}
				else // For AP
				{
					fn_NetSim_WLAN_ChangeMacState(pstruMacVar,MAC_IDLE);
					fn_NetSim_WLAN_AP_ForwardFrame();					
				
				}			
				break;
			default: // Unicast packet
				if(nDeviceId == pstruPacket->nDestinationId) // W/L station
				{		
					if(pstruPhyVar->nPHYprotocol!=IEEE_802_11n && pstruPhyVar->nPHYprotocol!=IEEE_802_11ac)
					{
						pstruMacVar->pstruMetrics->nReceivedFrameCountUnicast++;
						pstruCopyPacket = fn_NetSim_Packet_CopyAggregatedPacket(pstruPacket);
						//Add MAC_OUT event to send ACK			
						pstruEventDetails->dEventTime = dTime + pstruPhyVar->pstruPhyChar->naSIFSTime;			
						pstruEventDetails->pPacket = pstruPacket;			
						pstruEventDetails->nSubEventType = SEND_ACK;
						pstruEventDetails->nEventType = MAC_OUT_EVENT;
						fnpAddEvent(pstruEventDetails);	
						fn_NetSim_WLAN_ChangeMacState(pstruMacVar,TXing_ACK);
						//Free the MAC Data			
						fn_NetSim_WLAN_FreePacket(pstruCopyPacket);
						// Add Network IN event		
						pstruCopyPacket->pstruMacData->nMACProtocol = fn_NetSim_Stack_GetMacProtocol(nDeviceId,nInterfaceId);				
						pstruEventDetails->dPacketSize = pstruCopyPacket->pstruMacData->dPacketSize;
						pstruEventDetails->nProtocolId = fn_NetSim_Stack_GetNWProtocol(nDeviceId);
						pstruEventDetails->pPacket = pstruCopyPacket;			
						pstruEventDetails->nSubEventType = 0;
						pstruEventDetails->nEventType = NETWORK_IN_EVENT;
						fnpAddEvent(pstruEventDetails);				
					}
					else if(pstruPhyVar->nPHYprotocol==IEEE_802_11n || pstruPhyVar->nPHYprotocol==IEEE_802_11ac)
					{
						memcpy(&pevent,pstruEventDetails,sizeof* pstruEventDetails);
						//If first Packet is collide, delete all packets
						if(Check_AMPDU_Collide())
							return 0;
						pstruPacketHead=pstruPacket;
						
								
						pstruBlockAckPkt=Generate_Block_Ack_Packet();
						pstruBlockAck = pstruBlockAckPkt->pstruMacData->Packet_MACProtocol;

						i=0;
						while((pstruPacketHead) && (i<numOfPacketsAggregated))
						{

							pstruPacket=pstruPacketHead;
							pstruPacketHead=pstruPacketHead->pstruNextPacket;
							pstruPacket->pstruNextPacket=NULL;
							if(pstruPacket->nPacketStatus==PacketStatus_NoError)
							{
								pstruMacVar->pstruMetrics->nReceivedFrameCountUnicast++;
								pstruBlockAck->BitMap[i]=1;
								// Add Network IN event	
								pstruCopyPacket=fn_NetSim_Packet_CopyAggregatedPacket(pstruPacket);
								pstruCopyPacket->pstruMacData->nMACProtocol = fn_NetSim_Stack_GetMacProtocol(nDeviceId,nInterfaceId);				
								pevent.dPacketSize = pstruCopyPacket->pstruMacData->dPacketSize;
								pevent.nPacketId=pstruCopyPacket->nPacketId;
								
								pevent.nProtocolId = fn_NetSim_Stack_GetNWProtocol(nDeviceId);
								pevent.pPacket = pstruCopyPacket;			
								pevent.nSubEventType = 0;
								// Adding one micro second extra for each packet for deaggregation
								pstruEventDetails->dEventTime = pstruEventDetails->dEventTime + 1;	
								pevent.nEventType = NETWORK_IN_EVENT;
								fnpAddEvent(&pevent);
							}
							//free the packet;
							fn_NetSim_Packet_FreePacket(pstruPacket);				
							pstruPacket = NULL;
							i++;
						}
						//Add MAC_OUT event to send ACK	
						pstruEventDetails->nProtocolId = fn_NetSim_Stack_GetMacProtocol(nDeviceId,nInterfaceId);
						pstruEventDetails->dEventTime = dTime + pstruPhyVar->pstruPhyChar->naSIFSTime;			
						pstruEventDetails->pPacket = pstruBlockAckPkt;			
						pstruEventDetails->nSubEventType = SEND_BLOCK_ACK;
						pstruEventDetails->nEventType = MAC_OUT_EVENT;
						fnpAddEvent(pstruEventDetails);	
						fn_NetSim_WLAN_ChangeMacState(pstruMacVar,TXing_ACK);
					
					}
				}
				else if(nDeviceId == pstruPacket->nReceiverId)// AP
				{		
					if(pstruPhyVar->nPHYprotocol!=IEEE_802_11n && pstruPhyVar->nPHYprotocol!=IEEE_802_11ac)
					{

						pstruMacVar->pstruMetrics->nReceivedFrameCountUnicast++;
						pstruCopyPacket = fn_NetSim_Packet_CopyPacket(pstruPacket);				
						//Add MAC_OUT event to send ACK	
						pstruEventDetails->dEventTime = dTime+pstruPhyVar->pstruPhyChar->naSIFSTime;			
						pstruEventDetails->dPacketSize = pstruCopyPacket->pstruMacData->dPacketSize;
						pstruEventDetails->pPacket = pstruCopyPacket;			
						pstruEventDetails->nSubEventType = SEND_ACK;
						pstruEventDetails->nEventType = MAC_OUT_EVENT;
						fnpAddEvent(pstruEventDetails);	
						fn_NetSim_WLAN_ChangeMacState(pstruMacVar,TXing_ACK);
						// Forward the packet
						pstruEventDetails->pPacket = pstruPacket;	
						fn_NetSim_WLAN_AP_ForwardFrame();				
					}
					else if(pstruPhyVar->nPHYprotocol==IEEE_802_11n||pstruPhyVar->nPHYprotocol==IEEE_802_11ac)
					{
						if(Check_AMPDU_Collide())
							return 0;
						memcpy(&pevent,pstruEventDetails,sizeof* pstruEventDetails);
						pstruBlockAckPkt=Generate_Block_Ack_Packet();
						pstruBlockAck = pstruBlockAckPkt->pstruMacData->Packet_MACProtocol;
						
						pstruPacket = pstruEventDetails->pPacket;
						i=0;
						while(pstruPacket && i<numOfPacketsAggregated)
						{
							if(pstruPacket->nPacketStatus==PacketStatus_NoError)
							{
								pstruBlockAck->BitMap[i]=1;
								pstruMacVar->pstruMetrics->nReceivedFrameCountUnicast++;
							}
							pstruPacket=pstruPacket->pstruNextPacket;
							i++;
						}
						pstruPacket = pstruEventDetails->pPacket;
						//Add MAC_OUT event to send ACK			
						pstruEventDetails->dEventTime = dTime + pstruPhyVar->pstruPhyChar->naSIFSTime;			
						pstruEventDetails->pPacket = pstruBlockAckPkt;			
						pstruEventDetails->nSubEventType = SEND_BLOCK_ACK;
						pstruEventDetails->nEventType = MAC_OUT_EVENT;
						fnpAddEvent(pstruEventDetails);	
						fn_NetSim_WLAN_ChangeMacState(pstruMacVar,TXing_ACK);
						//Forward the Packet
						pstruEventDetails->pPacket = pstruPacket;	
						fn_NetSim_WLAN_AP_ForwardFrame();
					}
				}
				break;
			}	
			break;
		case INDEPENDENT:
			if(!pstruPacket->nDestinationId)// broadcast packet	
			{
				pstruMacVar->pstruMetrics->nReceivedFrameCountBroadcast++;				
				//Free the MAC Data			
				fn_NetSim_WLAN_FreePacket(pstruPacket);
				pstruPacket->pstruMacData->nMACProtocol = fn_NetSim_Stack_GetMacProtocol(nDeviceId,nInterfaceId);
				pstruEventDetails->dEventTime = pstruEventDetails->dEventTime;				
				pstruEventDetails->nProtocolId = fn_NetSim_Stack_GetNWProtocol(nDeviceId);
				pstruEventDetails->dPacketSize = pstruPacket->pstruMacData->dPacketSize;						
				pstruEventDetails->pPacket = pstruPacket;
				pstruEventDetails->nSubEventType = 0;
				pstruEventDetails->nEventType = NETWORK_IN_EVENT;
				fnpAddEvent(pstruEventDetails);
				// Change the state. Add MAC OUT event if medium free
				fn_NetSim_WLAN_ChangeMacState(pstruMacVar,MAC_IDLE);				
				if(pstruMacVar->Medium)
					fn_NetSim_WLAN_CheckPacketsInPacketlist(nDeviceId, nInterfaceId);
			}
			else
			{
				if(pstruPhyVar->nPHYprotocol!=IEEE_802_11n && pstruPhyVar->nPHYprotocol!=IEEE_802_11ac)
				{
					pstruMacVar->pstruMetrics->nReceivedFrameCountUnicast++;
					pstruCopyPacket = fn_NetSim_Packet_CopyAggregatedPacket(pstruPacket);
					//Add MAC_OUT event to send ACK			
					pstruEventDetails->dEventTime = dTime + pstruPhyVar->pstruPhyChar->naSIFSTime;			
					pstruEventDetails->pPacket = pstruPacket;			
					pstruEventDetails->nSubEventType = SEND_ACK;
					pstruEventDetails->nEventType = MAC_OUT_EVENT;
					fnpAddEvent(pstruEventDetails);	
					fn_NetSim_WLAN_ChangeMacState(pstruMacVar,TXing_ACK);
					//Free the MAC Data			
					fn_NetSim_WLAN_FreePacket(pstruCopyPacket);
					// Add Network IN event		
					pstruCopyPacket->pstruMacData->nMACProtocol = fn_NetSim_Stack_GetMacProtocol(nDeviceId,nInterfaceId);				
					pstruEventDetails->dPacketSize = pstruCopyPacket->pstruMacData->dPacketSize;
					pstruEventDetails->nProtocolId = fn_NetSim_Stack_GetNWProtocol(nDeviceId);
					pstruEventDetails->pPacket = pstruCopyPacket;			
					pstruEventDetails->nSubEventType = 0;
					pstruEventDetails->nEventType = NETWORK_IN_EVENT;
					fnpAddEvent(pstruEventDetails);	
				}
				else if(pstruPhyVar->nPHYprotocol==IEEE_802_11n||pstruPhyVar->nPHYprotocol==IEEE_802_11ac)
				{
					memcpy(&pevent,pstruEventDetails,sizeof* pstruEventDetails);
					if(Check_AMPDU_Collide())
						return 0;
					pstruPacketHead=pstruPacket; 
					pstruBlockAckPkt=Generate_Block_Ack_Packet();
					pstruBlockAck = pstruBlockAckPkt->pstruMacData->Packet_MACProtocol;
					i=0;
					while((pstruPacketHead) && (i<numOfPacketsAggregated))
					{
						pstruPacket=pstruPacketHead;
						pstruPacketHead=pstruPacketHead->pstruNextPacket;
						pstruPacket->pstruNextPacket=NULL;

						if(pstruPacket->nPacketStatus==PacketStatus_NoError)
						{
							pstruMacVar->pstruMetrics->nReceivedFrameCountUnicast++;
							pstruBlockAck->BitMap[i]=1;
							// Add Network IN event		
							pstruCopyPacket = fn_NetSim_Packet_CopyAggregatedPacket(pstruPacket);
							pstruCopyPacket->pstruMacData->nMACProtocol = fn_NetSim_Stack_GetMacProtocol(nDeviceId,nInterfaceId);				
							pevent.dPacketSize = pstruCopyPacket->pstruMacData->dPacketSize;
							pevent.nPacketId=pstruCopyPacket->nPacketId;
							
							pevent.nProtocolId = fn_NetSim_Stack_GetNWProtocol(nDeviceId);
							pevent.pPacket = pstruCopyPacket;			
							pevent.nSubEventType = 0;
							
							//Adding one micro second extra for deaggregation
						    pstruEventDetails->dEventTime = pstruEventDetails->dEventTime + 1;	
							pevent.nEventType = NETWORK_IN_EVENT;
							fnpAddEvent(&pevent);
						}
							
						//free the packet;
						fn_NetSim_Packet_FreePacket(pstruPacket);				
						pstruPacket = NULL;
						i++;
					}
					//Add MAC_OUT event to send ACK			
					pstruEventDetails->dEventTime = dTime + pstruPhyVar->pstruPhyChar->naSIFSTime;			
					pstruEventDetails->pPacket = pstruBlockAckPkt;			
					pstruEventDetails->nProtocolId = fn_NetSim_Stack_GetMacProtocol(nDeviceId,nInterfaceId);
					pstruEventDetails->nSubEventType = SEND_BLOCK_ACK;
					pstruEventDetails->nEventType = MAC_OUT_EVENT;
					fnpAddEvent(pstruEventDetails);	
					fn_NetSim_WLAN_ChangeMacState(pstruMacVar,TXing_ACK);
						
				}
			}
			break;
		}
		break;
	default:
		printf("WLAN--- MAC IN EVENT unknown protocol\n");
		break;
	}
	return 0;
}
/**
This function is called when device receives the ACK frame. It checks the expected 
ack number is matched with temp packet or not. If yes then free the temp packet, 
increase the AckreceivedCount, set the CWcurrent to CWmin.Finally free the ACK frame.
*/
int fn_NetSim_WLAN_ReceiveACK()
{
	NETSIM_ID nDeviceId = pstruEventDetails->nDeviceId;
	NETSIM_ID nInterfaceId = pstruEventDetails->nInterfaceId;	
	MAC_VAR *pstruMacVar = DEVICE_MACVAR(nDeviceId,nInterfaceId); 
	PHY_VAR *pstruPhyVar = DEVICE_PHYVAR(nDeviceId,nInterfaceId); 
	NetSim_PACKET *pstruPacket = pstruEventDetails->pPacket;
	ACK_FRAME *pstruAck = pstruPacket->pstruMacData->Packet_MACProtocol;

	if(pstruAck->RA==DEVICE_HWADDRESS(nDeviceId,nInterfaceId))
	{	
		pstruMacVar->pstruMetrics->nAckReceivedCount++;
		pstruMacVar->nAckReceivedFlag = 1;
		pstruMacVar->nRetryCount = 0; //Set RetryCount = 0
		pstruMacVar->nCWcurrent = pstruPhyVar->pstruPhyChar->naCWmin; //Set to CWmin
		if(pstruMacVar->pstruTempData && pstruMacVar->pstruTempData->ReceiveAckNotification)
			pstruMacVar->pstruTempData->ReceiveAckNotification(pstruPacket);
		// Free temp packet
		fn_NetSim_Packet_FreePacket(pstruMacVar->pstruTempData);	
		pstruMacVar->pstruTempData = NULL;
		// Change the state
		fn_NetSim_WLAN_ChangeMacState(pstruMacVar,MAC_IDLE);

		//Call function to notify received packet
		packet_recv_notify(nDeviceId,nInterfaceId,pstruPacket->nTransmitterId);
	}
	fn_NetSim_Packet_FreePacket(pstruPacket); // Free ACK packet
	pstruPacket = NULL;
	if(pstruMacVar->Medium)
		fn_NetSim_WLAN_CheckPacketsInPacketlist(nDeviceId,nInterfaceId);		
	return 0;
}
/**
This function is used to receive the cummulative acknowledgement
*/
int fn_NetSim_WLAN_ReceiveBlockACK()
{
	NetSim_PACKET *pstruPacket;
	int i, numOfPackesAggregated ;
	NETSIM_ID nDeviceId = pstruEventDetails->nDeviceId;
	NETSIM_ID nInterfaceId = pstruEventDetails->nInterfaceId;	
	MAC_VAR *pstruMacVar = DEVICE_MACVAR(nDeviceId,nInterfaceId); 
	PHY_VAR *pstruPhyVar = DEVICE_PHYVAR(nDeviceId,nInterfaceId);

	NetSim_PACKET *pstruAckPacket = pstruEventDetails->pPacket;
	BlockACK_FRAME *pstruAck = pstruAckPacket->pstruMacData->Packet_MACProtocol;

	NetSim_PACKET *pstruPacketHead=NULL;
	NetSim_PACKET *pstruPacketHeadList=NULL;

	NetSim_PACKET *pstruList = pstruMacVar->pstruTempData;
	pstruMacVar->pstruTempData=NULL;

	if(pstruPhyVar->nPHYprotocol == IEEE_802_11n)
	{
		numOfPackesAggregated=MAX_802_11n_AMPDU_SIZE;
	}
	else if(pstruPhyVar->nPHYprotocol == IEEE_802_11ac)
	{
		numOfPackesAggregated=MAX_802_11ac_AMPDU_SIZE;
	}
	
	if(pstruAck->RA==DEVICE_HWADDRESS(nDeviceId,nInterfaceId))
	{
		i=0;
		while((pstruList!=NULL)&& (i<numOfPackesAggregated))
		{
			pstruPacket = pstruList;
			pstruList=pstruList->pstruNextPacket;
			pstruPacket->pstruNextPacket=NULL;
			if(pstruAck->BitMap[i]==0) //if Packet is errored, Add packet for retransmission
			{
				if(pstruPacketHead==NULL)
				{
					pstruPacketHead=pstruPacket;
					pstruPacketHeadList=pstruPacketHead;
				}
				else
				{
					pstruPacketHeadList->pstruNextPacket = pstruPacket;
					pstruPacketHeadList=pstruPacketHeadList->pstruNextPacket;
				}
			}
			else		//Packet received succesfully...free the packet
			{
				if(pstruPacket->ReceiveAckNotification)
					pstruPacket->ReceiveAckNotification(pstruAckPacket);
				fn_NetSim_Packet_FreePacket(pstruPacket);
				pstruPacket=NULL;
			}
			i++;
			
		}
	}
	pstruMacVar->pstruTempData=pstruPacketHead;
	pstruMacVar->pstruMetrics->nAckReceivedCount++;
	pstruMacVar->nAckReceivedFlag = 1;
	fn_NetSim_WLAN_ChangeMacState(pstruMacVar,MAC_IDLE);
	if(pstruMacVar->pstruTempData == NULL)
	{
		//All Packets transmitted Successfully
		pstruMacVar->nRetryCount = 0; //Set RetryCount = 0
		pstruMacVar->nCWcurrent = pstruPhyVar->pstruPhyChar->naCWmin; //Set to CWmin
		if(pstruMacVar->pstruTempData && pstruMacVar->pstruTempData->ReceiveAckNotification)
			pstruMacVar->pstruTempData->ReceiveAckNotification(pstruPacket);
		
		if(pstruMacVar->Medium)
		fn_NetSim_WLAN_CheckPacketsInPacketlist(nDeviceId,nInterfaceId);		
		
		//Call function to notify received packet
		packet_recv_notify(nDeviceId,nInterfaceId,pstruAckPacket->nTransmitterId);

	}
	else
	{
		//Retransmit the packets by adding MAC out event
		fn_NetSim_WLAN_CheckRetryLimit();
		packet_drop_notify(nDeviceId,nInterfaceId,pstruAckPacket->nTransmitterId);
		switch(pstruMacVar->nRetryFlag)	
		{
		case 1: //Retransmit the frame	
			fn_NetSim_WLAN_IncreaseCW();
			if(pstruMacVar->pstruTempData)
			{
				
				
				pstruPacket = pstruMacVar->pstruTempData;
				while(pstruPacket)
				{
					fn_NetSim_WLAN_FreePacket(pstruPacket);					
					pstruPacket=pstruPacket->pstruNextPacket;
				}
				
			}
			// Change the State
			fn_NetSim_WLAN_ChangeMacState(pstruMacVar,MAC_IDLE);
			// Send The Packets 
			//Question...do i do carrier sense again?? or directly send it??
			
			if(pstruMacVar->Medium) // IDLE
				fn_NetSim_WLAN_CheckPacketsInPacketlist(nDeviceId, nInterfaceId);
			break;
		case 0: // Drop the frame	
			pstruPacketHead=pstruMacVar->pstruTempData;
			pstruMacVar->pstruTempData = NULL;
			while(pstruPacketHead)
			{
				pstruMacVar->pstruMetrics->nFailedCount++;
				pstruPacket=pstruPacketHead;
				pstruPacketHead=pstruPacketHead->pstruNextPacket;
				pstruPacket->pstruNextPacket=NULL;
				

								
				if(pstruPacket->DropNotification)
					pstruPacket->DropNotification(pstruPacket);
				
				fn_NetSim_Packet_FreePacket(pstruPacket);
				pstruPacket = NULL;
					
			}
			pstruMacVar->nCWcurrent = pstruPhyVar->pstruPhyChar->naCWmin;	//set CWcurrent to CWmin 			
			pstruMacVar->nRetryCount = 0; //Set RetryCount = 0
			// Change the State
			fn_NetSim_WLAN_ChangeMacState(pstruMacVar,MAC_IDLE);
			if(pstruMacVar->Medium) // IDLE
				fn_NetSim_WLAN_CheckPacketsInPacketlist(nDeviceId, nInterfaceId);
			break;
		default:
			break;
		
		}
	}
	fn_NetSim_Packet_FreePacket(pstruAckPacket);
	return 0;
	
}

/**
This function is called when device receives the RTS frame. If device HWaddress !=
the pstruRts->RA. Then it updates NAV and drop RTS frame. If address matches, then 
if NAV <= 0, then add SEND CTS sub event. Increase the RtsReceivedCount. Else if 
NAV > 0increase nRTSFailureDueToNav. 
*/
int fn_NetSim_WLAN_ReceiveRTS()
{
	double dTime = pstruEventDetails->dEventTime;
	NETSIM_ID nDeviceId = pstruEventDetails->nDeviceId;
	NETSIM_ID nInterfaceId = pstruEventDetails->nInterfaceId;
	NETSIM_ID nProtocolId = pstruEventDetails->nProtocolId;
	MAC_VAR *pstruMacVar = DEVICE_INTERFACE(nDeviceId,nInterfaceId)->pstruMACLayer->MacVar;
	PHY_VAR *pstruPhyVar = DEVICE_INTERFACE(nDeviceId,nInterfaceId)->pstruPhysicalLayer->phyVar;
	NetSim_PACKET *pstruPacket = pstruEventDetails->pPacket;
	RTS_FRAME *pstruRts = pstruPacket->pstruMacData->Packet_MACProtocol;

	if(pstruRts->RA,DEVICE_HWADDRESS(nDeviceId,nInterfaceId))
	{	
		pstruMacVar->pstruMetrics->nRtsReceivedCount++;
		//Add the SEND CTS sub event
		pstruEventDetails->dEventTime = dTime + pstruPhyVar->pstruPhyChar->naSIFSTime;		
		pstruEventDetails->nEventType = MAC_OUT_EVENT;
		pstruEventDetails->nSubEventType = SEND_CTS;
		fnpAddEvent(pstruEventDetails);	
		// Change the State
		fn_NetSim_WLAN_ChangeMacState(pstruMacVar,TXing_CTS);
	}
	else
	{	
		if(pstruMacVar->dNAV > pstruEventDetails->dEventTime)
		{
			pstruMacVar->dNAV = pstruMacVar->dNAV -  pstruEventDetails->dEventTime + pstruRts->nDuration;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
		}
		else
		{
			pstruMacVar->dNAV = pstruEventDetails->dEventTime + pstruRts->nDuration;                                                                                              

		}
		fn_NetSim_Packet_FreePacket(pstruPacket); // Free RTS packet
		pstruPacket = NULL;
	}	
	return 0;
}

/**
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
This function is called when device receives the CTS frame. 
If device HWaddress != the pstruCts->RA. Then it  updates NAV and drop CTS frame. 
If address matches, then add SEND MPDU sub event. Increase the CtsReceivedCount and nRTSSuccessCount.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
int fn_NetSim_WLAN_ReceiveCTS()
{
	double dTime = pstruEventDetails->dEventTime;
	NETSIM_ID nDeviceId = pstruEventDetails->nDeviceId;
	NETSIM_ID nInterfaceId = pstruEventDetails->nInterfaceId;
	MAC_VAR *pstruMacVar = WLAN_MAC(nDeviceId,nInterfaceId);
	PHY_VAR *pstruPhyVar = WLAN_PHY(nDeviceId,nInterfaceId); 
	NetSim_PACKET *pstruPacket = pstruEventDetails->pPacket;
	CTS_FRAME *pstruCts = (CTS_FRAME*)pstruPacket->pstruMacData->Packet_MACProtocol;

	if(pstruCts->RA==DEVICE_HWADDRESS(nDeviceId,nInterfaceId))
	{
		
		pstruMacVar->nCtsReceivedFlag = 1;		
		pstruMacVar->pstruMetrics->nRTSSuccessCount++;		
		//Add the SEND DATA sub event
		pstruEventDetails->dEventTime = dTime + pstruPhyVar->pstruPhyChar->naSIFSTime;		
		pstruEventDetails->nEventType = MAC_OUT_EVENT;
		pstruEventDetails->nSubEventType = SEND_MPDU;
		pstruEventDetails->pPacket = NULL;
		fnpAddEvent(pstruEventDetails);
		// Change the State
		fn_NetSim_WLAN_ChangeMacState(pstruMacVar,TXing_UNICAST);
		fn_NetSim_Packet_FreePacket(pstruPacket); // Free CTS packet
		pstruPacket = NULL;
	}
	else
	{
		if(pstruMacVar->dNAV > pstruEventDetails->dEventTime)
		{
			pstruMacVar->dNAV = pstruMacVar->dNAV -  pstruEventDetails->dEventTime + pstruCts->nDuration;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
		}
		else
		{
			pstruMacVar->dNAV = pstruEventDetails->dEventTime + pstruCts->nDuration;                                                                                              

		}
		fn_NetSim_Packet_FreePacket(pstruPacket); // Free CTS packet
		pstruPacket = NULL;
	}
	pstruMacVar->pstruMetrics->nCtsReceivedCount++;

	return 0;
}

/**
 This function performs AckTimeOut operation. The AckTimeOut event added when we send
 the Unicast data. The AckTimeout time is the expected time, before that the sender 
 expecting the ACK. If sender does not receive the ACK before timeout, then sender 
 retransmit if RetryCount is with in the Retry limit, else sender drop the frame. 
*/
int fn_NetSim_WLAN_AckTimeOut()
{
	NETSIM_ID nDeviceId = pstruEventDetails->nDeviceId;
	NETSIM_ID nInterfaceId = pstruEventDetails->nInterfaceId;
	MAC_VAR *pstruMacVar = DEVICE_MACVAR(nDeviceId,nInterfaceId); 
	PHY_VAR *pstruPhyVar = DEVICE_PHYVAR(nDeviceId,nInterfaceId);	
	NetSim_PACKET *pstruPacket;
	NetSim_BUFFER* pstruBuffer;
	pstruBuffer = DEVICE_MAC_NW_INTERFACE(nDeviceId,nInterfaceId)->pstruAccessBuffer;
	if(pstruMacVar->nAckReceivedFlag)
	{
		pstruMacVar->nAckReceivedFlag = 0;			
		return 0;
	}	
	pstruMacVar->pstruMetrics->nACKFailureCount++;

	//Notify packet drop
	if(pstruMacVar->pstruTempData)
		packet_drop_notify(nDeviceId,nInterfaceId,pstruMacVar->pstruTempData->nReceiverId);

	//check the  retry attempts reached the limit
	fn_NetSim_WLAN_CheckRetryLimit();
	switch(pstruMacVar->nRetryFlag)	
	{
	case 1: //Retransmit the frame	
		fn_NetSim_WLAN_IncreaseCW();
		if(pstruMacVar->pstruTempData)
		{
			pstruPacket = pstruMacVar->pstruTempData;
			while(pstruPacket)
			{
				fn_NetSim_WLAN_FreePacket(pstruPacket);					
				pstruPacket=pstruPacket->pstruNextPacket;
			}
			pstruPacket = pstruMacVar->pstruTempData;
		}
		// Change the State
		fn_NetSim_WLAN_ChangeMacState(pstruMacVar,MAC_IDLE);
		// Check the Medium 
		if(pstruMacVar->Medium) // IDLE
			fn_NetSim_WLAN_CheckPacketsInPacketlist(nDeviceId, nInterfaceId);
		break;
	case 0: // Drop the frame			
		pstruMacVar->pstruMetrics->nFailedCount++;
		if(pstruMacVar->pstruTempData)
		{
			pstruPacket = pstruMacVar->pstruTempData;

			if(pstruPacket->DropNotification)
				pstruPacket->DropNotification(pstruPacket);
			Aggregated_Packet_FreePacket(pstruMacVar->pstruTempData);
			pstruMacVar->pstruTempData = NULL;
				
		}
		pstruMacVar->nCWcurrent = pstruPhyVar->pstruPhyChar->naCWmin;	//set CWcurrent to CWmin 			
		pstruMacVar->nRetryCount = 0; //Set RetryCount = 0
		// Change the State
		fn_NetSim_WLAN_ChangeMacState(pstruMacVar,MAC_IDLE);
		if(pstruMacVar->Medium) // IDLE
			fn_NetSim_WLAN_CheckPacketsInPacketlist(nDeviceId, nInterfaceId);
		break;
	default:
		break;
	}	
	return 0;
}

/**
 This function invoke the retry limit checking. If active frame retry count is less than
 active frame retry limit, then set move flag as one to allow the frame retransmission.
 Otherwise, set move flag as zero to drop the active frame 
 */
int fn_NetSim_WLAN_CheckRetryLimit()
{
	NetSIm_DEVICEINTERFACE *pstruInterface;
	MAC_VAR *pstruMacVar;	
	pstruInterface=NETWORK->ppstruDeviceList[pstruEventDetails->nDeviceId-1]->ppstruInterfaceList[pstruEventDetails->nInterfaceId-1];
	pstruMacVar = pstruInterface->pstruMACLayer->MacVar;	
	if(pstruMacVar->nRetryCount < pstruMacVar->nRetryLimit )
	{	
		pstruMacVar->nRetryFlag = 1;		
	}
	else
	{
		pstruMacVar->nRetryFlag = 0;		
	}
	return 0;
}


/**
 This function perform CTS TimeOut operation. The CTS TimeOut event added when we send
 the RTS. The CTS Timeout time is the expected time, before that the sender 
 expecting the CTS. If sender won't receive the CTS before timeout, and if RetryCount is with in the Retrylimt then sender 
 retransmit, else sender drop the frame. 
*/
int fn_NetSim_WLAN_CtsTimeOut()
{
	NETSIM_ID nDeviceId = pstruEventDetails->nDeviceId;
	NETSIM_ID nInterfaceId = pstruEventDetails->nInterfaceId;
	MAC_VAR *pstruMacVar = DEVICE_MACVAR(nDeviceId,nInterfaceId); 
	PHY_VAR *pstruPhyVar = DEVICE_PHYVAR(nDeviceId,nInterfaceId); 
	NetSim_PACKET *pstruTempPacket = pstruEventDetails->pPacket;
	NetSim_PACKET *pstruPacket;
	if(pstruMacVar->nCtsReceivedFlag)
	{
		pstruMacVar->nCtsReceivedFlag = 0;	//Set CTS received as Zero
		// free the packet
		fn_NetSim_Packet_FreePacket(pstruTempPacket);
		pstruTempPacket = NULL;		
		return 0;
	}		
	pstruMacVar->pstruMetrics->nRTSFailureCount++;
	//check the attempts reached the limit	
	fn_NetSim_WLAN_CheckRetryLimit();
	switch(pstruMacVar->nRetryFlag)
	{
	case 1:
		//Expand the contention window			
		fn_NetSim_WLAN_IncreaseCW();	
		// Change the State
		fn_NetSim_WLAN_ChangeMacState(pstruMacVar,MAC_IDLE);
		if(pstruMacVar->Medium)
		{
			fn_NetSim_WLAN_CheckPacketsInPacketlist(nDeviceId, nInterfaceId);
		}
		break;
	case 0:
		//Get the packet from Accessbuffer and move it into next packet
		pstruPacket = fn_NetSim_Packet_GetPacketFromBuffer(DEVICE_MAC_NW_INTERFACE(nDeviceId,nInterfaceId)->pstruAccessBuffer,1);
		//delete the first data from buffer list and move to next
		fn_NetSim_Packet_FreePacket(pstruPacket);
		pstruPacket = NULL;		
		pstruMacVar->pstruMetrics->nFailedCount++;
		pstruMacVar->nCWcurrent = pstruPhyVar->pstruPhyChar->naCWmin;
		pstruMacVar->nRetryCount = 0;//Set RetryCount = 0
		// Change the State
		fn_NetSim_WLAN_ChangeMacState(pstruMacVar,MAC_IDLE);
		break;
	default:
		break;
	}	
	// free the CTS packet
	fn_NetSim_Packet_FreePacket(pstruTempPacket);
	pstruTempPacket = NULL;	
	return 0;
}

/**
This function is called to Calculate the NAV while sending RTS and CTS frame.
*/
int fn_NetSim_WLAN_CalculateNAV_Duration(NETSIM_ID nDeviceId,NETSIM_ID nInterfaceId,double dPacketSize, int *nDuration, int nFrameType)
{
	double dDataTxTime, dAckTxTime, dCtsTxTime;
	PHY_VAR *pstruPhy = DEVICE_PHYVAR(nDeviceId,nInterfaceId);
	switch(nFrameType)
	{
	case WLAN_RTS:
		if(pstruPhy->nPHYprotocol != IEEE_802_11n && pstruPhy->nPHYprotocol != IEEE_802_11ac)
		{
			// Duration field = Frame transmit time + CTS transmit time + ACK tranmit time
			*nDuration = (int)ceil(pstruPhy->pstruPhyChar->naPreambleLength\
				+ pstruPhy->pstruPhyChar->naPLCPHeaderLength\
				+ (dPacketSize*(8.0/pstruPhy->dDataRate)) + pstruPhy->pstruPhyChar->naSIFSTime\
				+ pstruPhy->pstruPhyChar->naPreambleLength 
				+ pstruPhy->pstruPhyChar->naPLCPHeaderLength\
				+ ((CTS_SIZE * 8.0)/pstruPhy->dControlFrameDataRate)\
				+ pstruPhy->pstruPhyChar->naSIFSTime\
				+ pstruPhy->pstruPhyChar->naPreambleLength\
				+ pstruPhy->pstruPhyChar->naPLCPHeaderLength\
				+ (ACK_SIZE * 8 / pstruPhy->dControlFrameDataRate) );
		}
		else if(pstruPhy->nPHYprotocol == IEEE_802_11n )
		{
			dDataTxTime = fn_NetSim_WLAN_802_11n_TxTimeCalculation(dPacketSize,1,nDeviceId,nInterfaceId,1);
			dPacketSize = 14; 
			dCtsTxTime = fn_NetSim_WLAN_802_11n_TxTimeCalculation(dPacketSize,0,nDeviceId,nInterfaceId,1);
			dPacketSize = 32; 
			dAckTxTime = fn_NetSim_WLAN_802_11n_TxTimeCalculation(dPacketSize,0,nDeviceId,nInterfaceId,1);;
			*nDuration = (int)ceil( dDataTxTime + pstruPhy->pstruPhyChar->naSIFSTime\
				+ dCtsTxTime + pstruPhy->pstruPhyChar->naSIFSTime\
				+ dAckTxTime );
		}
		else if(pstruPhy->nPHYprotocol == IEEE_802_11ac )
		{
			dDataTxTime = fn_NetSim_WLAN_802_11ac_TxTimeCalculation(dPacketSize,1,nDeviceId,nInterfaceId,1);
			dPacketSize = 14; 
			dCtsTxTime = fn_NetSim_WLAN_802_11ac_TxTimeCalculation(dPacketSize,0,nDeviceId,nInterfaceId,1);
			dPacketSize = 32; 
			dAckTxTime = fn_NetSim_WLAN_802_11ac_TxTimeCalculation(dPacketSize,0,nDeviceId,nInterfaceId,1);;
			*nDuration = (int)ceil( dDataTxTime + pstruPhy->pstruPhyChar->naSIFSTime\
				+ dCtsTxTime + pstruPhy->pstruPhyChar->naSIFSTime\
				+ dAckTxTime );
		}
		break;
	case WLAN_CTS: // Decreament the CTS transmition time
		if(pstruPhy->nPHYprotocol != IEEE_802_11n && pstruPhy->nPHYprotocol != IEEE_802_11ac)
		{
			*nDuration = (int)ceil(*nDuration -  pstruPhy->pstruPhyChar->naSIFSTime\
				- pstruPhy->pstruPhyChar->naPreambleLength\
				- pstruPhy->pstruPhyChar->naPLCPHeaderLength\
				- ( CTS_SIZE * 8 / pstruPhy->dControlFrameDataRate ) );
		}
		else if(pstruPhy->nPHYprotocol == IEEE_802_11n)
		{
			dPacketSize = 14; 
			dCtsTxTime = fn_NetSim_WLAN_802_11n_TxTimeCalculation(dPacketSize,0,nDeviceId,nInterfaceId,1);
			*nDuration = (int)ceil(*nDuration -  dCtsTxTime);
		}
		else if(pstruPhy->nPHYprotocol == IEEE_802_11ac)
		{
			dPacketSize = 14; 
			dCtsTxTime = fn_NetSim_WLAN_802_11ac_TxTimeCalculation(dPacketSize,0,nDeviceId,nInterfaceId,1);
			*nDuration = (int)ceil(*nDuration -  dCtsTxTime);
		}
		break;
	default:
		break;
	}	
	return 0;
}

/**
Function is used to forward the frames from AP to different Interface.
*/
int fn_NetSim_WLAN_AP_ForwardFrame()
{
	NETSIM_ID nLoop;		
	NetSim_BUFFER *pstruBuffer;
	NetSim_LINKS* pstruLink;	
	NetSim_PACKET *pstruPacketCopy,*pstruPacketHead;
	NETSIM_ID nDeviceId = pstruEventDetails->nDeviceId;
	NETSIM_ID nInterfaceId = pstruEventDetails->nInterfaceId;	
	MAC_VAR *pstruMac = DEVICE_MACVAR(nDeviceId, nInterfaceId); 	
	NetSim_PACKET *pstruPacket = pstruEventDetails->pPacket;	
	pstruPacket->pstruMacData->dArrivalTime = pstruEventDetails->dEventTime;
	switch(pstruPacket->nDestinationId) //Check the destination
	{
	case 0: //Broadcast packet		
		for(nLoop = 0;nLoop<NETWORK->ppstruDeviceList[nDeviceId-1]->nNumOfInterface;nLoop++)
		{	
			//Add the packet to access interface
			if(!NETWORK->ppstruDeviceList[nDeviceId-1]->ppstruInterfaceList[nLoop]->pstruAccessInterface)
				continue;
			pstruPacketCopy=fn_NetSim_Packet_CopyAggregatedPacket(pstruPacket);
			fn_NetSim_WLAN_FreePacket(pstruPacketCopy); //Free the MAC Data	
			pstruBuffer = DEVICE_MAC_NW_INTERFACE(nDeviceId,nLoop+1)->pstruAccessBuffer;
			if(!fn_NetSim_GetBufferStatus(pstruBuffer))
			{
				//Add the MAC out event
				pstruEventDetails->nInterfaceId = nLoop+1;				
				pstruEventDetails->nProtocolId = fn_NetSim_Stack_GetMacProtocol(nDeviceId,nLoop+1);				
				pstruEventDetails->pPacket = NULL;
				pstruEventDetails->nSubEventType = 0;
				pstruEventDetails->nEventType = MAC_OUT_EVENT;
				fnpAddEvent(pstruEventDetails);
				fn_NetSim_Packet_AddPacketToList(pstruBuffer,pstruPacketCopy,0);
			}
			else
			{
				fn_NetSim_Packet_AddPacketToList(pstruBuffer,pstruPacketCopy,0);
				pstruLink = NETWORK->ppstruDeviceList[nDeviceId-1]->ppstruInterfaceList[nLoop]->pstruPhysicalLayer->pstruNetSimLinks;
				if(!pstruLink)
					fnNetSimError("No link is connected to device %d interface %d.",nDeviceId,nLoop+1);
				if(pstruLink->nLinkType == LinkType_P2MP)
				{
					if(pstruMac->Medium)
						fn_NetSim_WLAN_CheckPacketsInPacketlist(nDeviceId, nInterfaceId);
				}
			}
		}
		fn_NetSim_Packet_FreePacket(pstruPacket);
		pstruPacket = NULL;
		break;
	default: // unicast packets
		if(fn_NetSim_WLAN_CheckDestDevicePresentWithinAP(nDeviceId,nInterfaceId,pstruPacket->nDestinationId))
		{			
			pstruPacketHead=pstruPacket;
			// Packet is for Wireless network
			//Add the packet to access interface
			pstruBuffer = DEVICE_MAC_NW_INTERFACE(nDeviceId,nInterfaceId)->pstruAccessBuffer;	
			//Add the MAC out event
			pstruEventDetails->nInterfaceId = nInterfaceId;				
			pstruEventDetails->nProtocolId = fn_NetSim_Stack_GetMacProtocol(nDeviceId,nInterfaceId);				
			pstruEventDetails->pPacket = NULL;
			pstruEventDetails->nSubEventType = 0;//CS;
			pstruEventDetails->nEventType = MAC_OUT_EVENT;
			fnpAddEvent(pstruEventDetails);
			while(pstruPacketHead)
			{
				pstruPacket=pstruPacketHead;
				pstruPacketHead=pstruPacketHead->pstruNextPacket;
				pstruPacket->pstruNextPacket=NULL;
				if(pstruPacket->nPacketStatus==PacketStatus_Collided||pstruPacket->nPacketStatus==PacketStatus_Error)
				{
					//free packet
					fn_NetSim_Packet_FreePacket(pstruPacket);	
					pstruPacket=NULL;
					continue;
				}
				fn_NetSim_WLAN_FreePacket(pstruPacket);
				fn_NetSim_Packet_AddPacketToList(pstruBuffer,pstruPacket,0);//pstruPacketCopy,0);					
			}
		}
		else
		{		
			pstruPacketHead=pstruPacket;
			//The packet is not for wireless interface. Broadcasting to other port
			for(nLoop = 1;nLoop<=NETWORK->ppstruDeviceList[nDeviceId-1]->nNumOfInterface;nLoop++)
			{
				int f=0;
				if(nLoop == nInterfaceId)
					continue;
				//Place the packet to access interface
				pstruBuffer = DEVICE_MAC_NW_INTERFACE(nDeviceId,nLoop)->pstruAccessBuffer;

				if(!fn_NetSim_GetBufferStatus(pstruBuffer))
					f=1;
				while(pstruPacketHead)
				{	
					pstruPacket=pstruPacketHead;
					pstruPacketHead=pstruPacketHead->pstruNextPacket;
					pstruPacket->pstruNextPacket=NULL;
					if(pstruPacket->nPacketStatus==PacketStatus_Collided||pstruPacket->nPacketStatus==PacketStatus_Error)
					{
						continue;
					}
					fn_NetSim_WLAN_FreePacket(pstruPacket);
					fn_NetSim_Packet_AddPacketToList(pstruBuffer,pstruPacket,0);
					if(f)
						f=2;
				}
				if(f==2)
				{
					//Add the MAC out event
					pstruEventDetails->nEventType = MAC_OUT_EVENT;
					pstruEventDetails->nInterfaceId = nLoop;
					pstruEventDetails->nProtocolId = fn_NetSim_Stack_GetMacProtocol(nDeviceId,nLoop);
					pstruEventDetails->nSubEventType = 0;
					pstruEventDetails->pPacket = NULL;
					fnpAddEvent(pstruEventDetails);
				}
				
			}
		}
		break;
	}	
	return 0;
}

/**
Function used to check any packets in the QOS buffer packet list to call 
MAC Out event for transmission.
*/
int fn_NetSim_WLAN_CheckPacketsInPacketlist(NETSIM_ID nDevId, NETSIM_ID nInterfaceId)
{
	MAC_VAR *pstruMac = DEVICE_MACVAR(nDevId, nInterfaceId);
	PHY_VAR *pstruPhy = DEVICE_PHYVAR(nDevId, nInterfaceId);
	NetSim_PACKET* pstruPacket;	
	
	// Check the medium
	if(pstruMac->Medium == BUSY)
		return 0;
	if((pstruMac->CurrentState != MAC_IDLE) || (pstruPhy->RadioState != RX_ON_IDLE))
		return 0;
	//Check buffer has packets to send	
	if(pstruMac->pstruTempData)
		pstruPacket = pstruMac->pstruTempData;
	else
	{
		// Get packet from buffer
		pstruPacket = fn_NetSim_WLAN_Packet_GetPacketFromBuffer(pstruMac->pstruQosList,0);
		if(!pstruPacket)
			return 0;
	}
	// Add MAC_OUT_EVENT 
	pstruEventDetails->nDeviceId = nDevId;
	pstruEventDetails->nDeviceType = NETWORK->ppstruDeviceList[nDevId-1]->nDeviceType; //DEVICE_TYPE(nDevId);
	pstruEventDetails->nInterfaceId = nInterfaceId;	
	pstruEventDetails->nPacketId = pstruPacket->nPacketId;
	if(pstruPacket->pstruAppData)
		pstruEventDetails->nSegmentId = pstruPacket->pstruAppData->nSegmentId;
	else
		pstruEventDetails->nSegmentId = 0;
	if(pstruPacket->pstruNetworkData)
		pstruEventDetails->dPacketSize = pstruPacket->pstruNetworkData->dPacketSize;
	else
		pstruEventDetails->dPacketSize = pstruPacket->pstruMacData->dPacketSize;
	pstruEventDetails->pPacket = NULL;
	pstruEventDetails->nProtocolId = fn_NetSim_Stack_GetMacProtocol(nDevId, nInterfaceId);
	pstruEventDetails->nSubEventType = CS;
	pstruEventDetails->nEventType = MAC_OUT_EVENT;
	fnpAddEvent(pstruEventDetails);
	return 0;
}

/**
This function called when NAV_END subevent triggered. Reset the NAV and Navflag,
if Navflag is set.
*/
int fn_NetSim_WLAN_ResetNAV()
{
	NETSIM_ID nDeviceId = pstruEventDetails->nDeviceId;
	NETSIM_ID nInterfaceId = pstruEventDetails->nInterfaceId;
	MAC_VAR *pstruMac = DEVICE_MACVAR(nDeviceId,nInterfaceId);
	if(pstruMac->bFlagNavExpired)
	{
		pstruMac->bFlagNavExpired = 0;
		pstruMac->dNAV = 0;
	}
	// Change the state
		fn_NetSim_WLAN_ChangeMacState(pstruMac,MAC_IDLE);
	return 0;
}



