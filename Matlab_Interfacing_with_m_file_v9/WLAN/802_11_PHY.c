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
This Function is used to calculate the Fading Loss(Rayleigh Fading)
*/
int fnCalculateFadingLoss(unsigned long *ulSeed1, unsigned long *ulSeed2,double *dFadingPower,int fm1)
{
	double n_sign;

	if(fm1==1)// if rayleigh fading is set
	{
		n_sign = fn_NetSim_Utilities_GenerateRandomNo(ulSeed1,ulSeed2)/NETSIM_RAND_MAX;
		*dFadingPower = n_sign;
		if(*dFadingPower<0 ||*dFadingPower>1) // Sanity check -- this condition should never occur
		{
			*dFadingPower=0;
			fnNetSimError("Random Number generated while calculating fading loss is not in the range [0,1]");
			return 0;
		}
		else
		{
			//*dFadingPower = log(*dFadingPower);
			//*dFadingPower = 10*log10(-*dFadingPower); //Rayleigh Fading is an exponential distribution with mean 1. So, we take an inverse exponential funtion to calculate Fading loss.
			
			*dFadingPower = fn_netsim_matlab();
			n_sign = fn_NetSim_Utilities_GenerateRandomNo(ulSeed1,ulSeed2)/NETSIM_RAND_MAX;
			if(n_sign<0.5)
			{
				*dFadingPower = -*dFadingPower; //this is done to ensure constructive/destructive fading
			}

		}

	}
	else
	{
		*dFadingPower = 0;
	}
	return 0;
}


/**
This function invoke the received power calculation and 
selects the path loss model based on the channel characteristics value. 
*/
_declspec(dllexport) int fn_NetSim_WLAN_CalculateReceivedPower(NETSIM_ID nDeviceId, NETSIM_ID nInterfaceId,double nTxPower,double dDistance,double dFrequency,double dPathLossExponent,double *dReceivedPower)
{
	double fpi=3.1417f;		
	double nGainTxr=0;		
	double nGainRver=0;	
	int nDefaultDistance=1; 
	double fA1,fWaveLength=0.0; 
	double fA1dB, fA2dB;
	double fTXPowerdBm; 

	PHY_VAR *pstruPhyVar;
	pstruPhyVar = DEVICE_PHYVAR(nDeviceId,nInterfaceId);
	if(pstruPhyVar->ChannelModel == NO_PATHLOSS || dDistance == 0)
	{
		*dReceivedPower = 10 * log10( nTxPower );
		return 0;
	}
	else
	{
		// get the gain of the Transmitter
		nGainTxr=0;
		// get the gain of the receiver
		nGainRver=0;
		// Calculate Lambda
		fWaveLength=(double)(300.0/(dFrequency * 1.0));//pathloss
		// Calculate  (4*3.14*do)
		fA1=fWaveLength/(4*(double) fpi * nDefaultDistance );
		//Calculate  20log10[ Lambda/(4*3.14*do) ]
		fA1dB =  20 * log10(fA1);
		//Calculate  10 * n *log10 (do/d)
		fA2dB =  10 * dPathLossExponent * log10(nDefaultDistance /dDistance);
		fTXPowerdBm = 10 * log10(nTxPower);
		//Calculate  [Pt]  +  [Gt]  +  [Gr]  +  20log10[ Lemda/(4*3.14*do) ] + ( 10 * n *log10 (do/d) )
		*dReceivedPower = fTXPowerdBm + nGainTxr + nGainRver + fA1dB + fA2dB;
	}
	return 0;
}
/**
This function is used to calculate shadow loss
*/
_declspec (dllexport) int fn_NetSim_WLAN_CalculateShadowLoss(unsigned long ulSeed1, unsigned long ulSeed2, double* dShadowReceivedPower,double dStandardDeviation)
{
	double ldGaussianDeviate=0.0;
	double ldRandomNumber = 0.0;
	static int nIset = 0;
	static double fGset=0;
	double fFac,fRsq,fV1,fV2;	
	double d_sign1;
	if(nIset==0)
	{
		do
		{		
			// call the random number generator function
			ldRandomNumber = fn_NetSim_Utilities_GenerateRandomNo(&ulSeed1,&ulSeed2)/NETSIM_RAND_MAX;		
			fV1=(double)(2.0*ldRandomNumber-1.0);
			//calculate the Random number from this function
			ldRandomNumber = fn_NetSim_Utilities_GenerateRandomNo(&ulSeed1,&ulSeed2)/NETSIM_RAND_MAX;		
			fV2=(double)(2.0*ldRandomNumber-1.0);
			fRsq=fV1*fV1+fV2*fV2;
		}while(fRsq>=1.0 || fRsq==0.0);
		fFac=(double)(sqrt(-2.0*log(fRsq)/fRsq));
		fGset=fV1*fFac;	
		nIset=1;
		ldGaussianDeviate = fV2*fFac;
	}
	else
	{
		nIset=0;
		ldGaussianDeviate = fGset;
	}
	d_sign1 = fn_NetSim_Utilities_GenerateRandomNo(&ulSeed1,&ulSeed2)/NETSIM_RAND_MAX;
	if(d_sign1 <= 0.5) 
	{
		// Assign the Received power due to shadow loss.	
		*dShadowReceivedPower = -ldGaussianDeviate * sqrt(dStandardDeviation); 
		// This is done to ensure there is constructive and destructive shadowing
	}
	else
	{	
		// Assign the Received power due to shadow loss.	
		*dShadowReceivedPower = ldGaussianDeviate * sqrt(dStandardDeviation);
	}

	return 0;
}

/**
This function used to compute the data rate with respect to the total received power
set the received power value using received power range table for modulation(dbm)
*/
int fn_NetSim_WLAN_SetDataRate(NETSIM_ID nDeviceId, NETSIM_ID nInterfaceId, NETSIM_ID nReceiverId)
{
	PHY_VAR *pstruPhy=WLAN_PHY(nDeviceId,nInterfaceId);;	

	switch(pstruPhy->nPHYprotocol)
	{
	case IEEE_802_11b:
		fn_NetSim_WLAN_802_11b_DataRate(nDeviceId,nInterfaceId,nReceiverId);	
		return 0;
		break;
	case IEEE_802_11a:
	case IEEE_802_11g:
		fn_NetSim_WLAN_802_11a_11g_DataRate(nDeviceId,nInterfaceId,nReceiverId);		
		return 0;
		break;
	case IEEE_802_11n:
		fn_NetSim_WLAN_802_11n_DataRate(nDeviceId, nInterfaceId, nReceiverId);
		return 0;
		break;
	case IEEE_802_11ac:
		fn_NetSim_WLAN_802_11ac_DataRate(nDeviceId, nInterfaceId, nReceiverId);
		return 0;
		break;

	default:
		printf("WLAN--- Unknown protocol SetDatarate function\n ");
		break;
	}
	return 0;
}


/**
This function is used to calculate the Bit Error Rate (BER). The BER calculation 
depends on the Channel characteristics, and received power. So compare the Total 
received power with the Receiver sensitivity for the particular data rate. Then 
calculate the BER with respect to particular modulation.		 
*/
int fn_NetSim_WLAN_CalculateBER(NETSIM_ID nDeviceId, NETSIM_ID nInterfaceId, double dReceivedPower,NetSim_PACKET* pstruPacket)
{
	PHY_VAR *pstruPhy = WLAN_PHY(nDeviceId,nInterfaceId);
	if(pstruPhy->ChannelModel == NO_PATHLOSS)
	{
		pstruPhy->dBER = 0; // Assign BER value 				
		return 0;
	}
	switch(pstruPhy->PhyType)
	{
	case DSSS:	
		fn_NetSim_WLAN_802_11b_BER(nDeviceId,nInterfaceId,dReceivedPower,pstruPacket);
		return 0;
		break;
	case OFDM:	
		fn_NetSim_WLAN_802_11a_11g_BER(nDeviceId,nInterfaceId,dReceivedPower,pstruPacket);
		return 0;
		break; 
	case HT:
		fn_NetSim_WLAN_802_11n_BER(nDeviceId,nInterfaceId,dReceivedPower,pstruPacket);
		return 0;
		break;	
	case VHT:
		fn_NetSim_WLAN_802_11ac_BER(nDeviceId,nInterfaceId,dReceivedPower,pstruPacket);
		return 0;
		break;
	default:
		printf("WLAN-- Unknown PHY TYPE in CalculationBER function \n");
		pstruPhy->dBER =0; // Assign BER value 				
		break;
	}	
	return 0;
}
/**
This function used to calculate the received from any wireless node other wireless
nodes in the network. It check is there any interference between the adjacent channel.
If any interference then it consider that effect to calculate the received power.
*/
int fn_NetSim_WLAN_CalculateAndSetReceivedPower(NETSIM_ID nodeId)
{
	static int i=0;
	static int nMemoryAllocationFlag = 0;
	NetSim_LINKS* pstruLink;
	int nCheckInterference = 0;
	NETSIM_ID ni,nj, njInterface, niInterface;
	double dDistance;
	double dReceivedPower;
	double dShadowReceivedPower;
	double dTotalRxPower;
	double dFrequencyi = 0, dFrequencyj = 0;	
	unsigned long ulSeed1, ulSeed2;
	NETSIM_ID nDeviceCount;	
	NETSIM_ID nProticolId;
	/* nFlag is used to control the calculation. This function is called for every
	* packet. but it will calculate only for the first packet. This is done for
	* saving the computing time. As, currently mobility is not implemented in the
	* IEEE802.11 so, by approximation it can be assumed that
	* there is no significant change in the received power. If mobility is
	* implemented this if loop must be removed.
	* If the user want to change or modify this function for every packet then they
	* can easily do this by removing the if loop.
	*/
	PHY_VAR *pstruPhyi,*pstruPhyj;
	//if(nCSRPFlag == 0)
	{
		nDeviceCount = NETWORK->nDeviceCount;
		if(!nMemoryAllocationFlag)
		{
			dCummulativeReceivedPower  = fnpAllocateMemory(nDeviceCount+1, sizeof(double*));
			if(i==0)
				dCummulativeReceivedPower2  = fnpAllocateMemory(nDeviceCount+1, sizeof(double*));
			for(ni=0;ni<=nDeviceCount;ni++)
			{
				dCummulativeReceivedPower[ni] = fnpAllocateMemory(nDeviceCount+1, sizeof(double));
				if(i==0)
					dCummulativeReceivedPower2[ni]  = fnpAllocateMemory(nDeviceCount+1, sizeof(double));
			}
			nMemoryAllocationFlag = 1;
		}
		nCSRPFlag = 1;
		//Set between wireless devices
		for(ni=1; ni<=nDeviceCount; ni++)
		{
			for(niInterface=1; niInterface <= NETWORK->ppstruDeviceList[ni-1]->nNumOfInterface; niInterface++)
			{
				if(!NETWORK->ppstruDeviceList[ni-1]->ppstruInterfaceList[niInterface-1]->pstruMACLayer)
					continue;
				nProticolId = NETWORK->ppstruDeviceList[ni-1]->ppstruInterfaceList[niInterface-1]->pstruMACLayer->nMacProtocolId;
				if(nProticolId != MAC_PROTOCOL_IEEE802_11)//MAC_PROTOCOL_IEEE802_11a && nProticolId != MAC_PROTOCOL_IEEE802_11b && nProticolId != MAC_PROTOCOL_IEEE802_11g && nProticolId != MAC_PROTOCOL_IEEE802_11n ) 
					continue;
				switch(DEVICE_PHYMEDIUM(ni,niInterface))
				{
				case PHY_MEDIUM_WIRELESS:
					pstruLink = NETWORK->ppstruDeviceList[ni-1]->ppstruInterfaceList[niInterface-1]->pstruPhysicalLayer->pstruNetSimLinks;
					dFrequencyi = pstruLink->puniMedProp.pstruWirelessLink.dFrequency;
					for(nj=1;nj<=nDeviceCount;nj++)
					{					
						for(njInterface = 1; njInterface <= NETWORK->ppstruDeviceList[nj-1]->nNumOfInterface; njInterface++)
						{
							if(NETWORK->ppstruDeviceList[nj-1]->ppstruInterfaceList[njInterface-1]->pstruMACLayer)
								switch(DEVICE_PHYMEDIUM(nj,njInterface))
							{
								case PHY_MEDIUM_WIRELESS:
									if(ni==nj)
									{
										dCummulativeReceivedPower[ni][nj] = -999;
										if(i==0)
											dCummulativeReceivedPower2[ni][nj] = -999;
										continue;
									}
									else
									{
										pstruLink = NETWORK->ppstruDeviceList[nj-1]->ppstruInterfaceList[njInterface-1]->pstruPhysicalLayer->pstruNetSimLinks;
										if(!pstruLink)
											fnNetSimError("Device %d Interface %d is not connected via any link",nj,njInterface);
										dFrequencyj = pstruLink->puniMedProp.pstruWirelessLink.dFrequency;

										nCheckInterference = fn_NetSim_WLAN_CheckFrequencyInterfrence(dFrequencyi,dFrequencyj);
										if(!nCheckInterference)
										{
											dCummulativeReceivedPower[ni][nj] = -999;
											if(i==0)
												dCummulativeReceivedPower2[ni][nj] = -999;
											continue;
										}
										pstruPhyi = DEVICE_PHYVAR(ni,niInterface);
										pstruPhyj = DEVICE_PHYVAR(nj,njInterface);							
										dDistance = fn_NetSim_Utilities_CalculateDistance(DEVICE_POSITION(ni),DEVICE_POSITION(nj));
										pstruPhyi->dDistance = dDistance;							
										if(pstruPhyi->ChannelModel == NO_PATHLOSS)
										{
											dCummulativeReceivedPower[ni][nj] = 10*log10(pstruPhyi->nTxPower);
											if(i==0)
												dCummulativeReceivedPower2[ni][nj] = 10*log10(pstruPhyi->nTxPower);
										}
										else// pathloss 
										{						
											fn_NetSim_WLAN_CalculateReceivedPower(ni,niInterface,pstruPhyi->nTxPower,dDistance,dFrequencyi,pstruPhyi->dPathLossExponent,&dReceivedPower);
											dShadowReceivedPower=0.0;										
											if(pstruPhyi->dStandardDeviation != 0)
											{
												ulSeed1 = NETWORK->ppstruDeviceList[ni-1]->ulSeed[0];
												ulSeed2 = NETWORK->ppstruDeviceList[ni-1]->ulSeed[1];									
												fn_NetSim_WLAN_CalculateShadowLoss(ulSeed1,ulSeed2,&dShadowReceivedPower,pstruPhyi->dStandardDeviation);
											}								
											dTotalRxPower = dReceivedPower + dShadowReceivedPower;
											dCummulativeReceivedPower[ni][nj] = dTotalRxPower; //pstruPhyi->dTotalReceivedPower;
											if(i==0)
												dCummulativeReceivedPower2[ni][nj] = dTotalRxPower; //pstruPhyi->dTotalReceivedPower;
										}
									}
									break;
								default:
									//Other protocol interface
									//Ignore
									break;
							}
						}//jInterface				
					}
					break; //Medium wireless
				default:
					break;
				}			
			}//iInterface
		}

	}// end flag
	i++;
	return 0;
}
/**
Function used to check the interference between two wireless devices.
*/
int fn_NetSim_WLAN_CheckFrequencyInterfrence(double dFrequency1,double dFrequency2)
{
	if(dFrequency1 > dFrequency2)
	{
		if( (dFrequency1 - dFrequency2) >= 20 )
			return 0; // no interference
		else 
			return 1; // interference
	}
	else
	{
		if( (dFrequency2 - dFrequency1) >= 20 )
			return 0; // no interference
		else 
			return 1; // interference
	}
	//return 0;
}


/**
Calculate and return Transmission time for one packet.
*/
double fn_NetSim_WLAN_CalculateTransmissionTime(NetSim_PACKET *pstruPacket, NETSIM_ID nDevId, NETSIM_ID nInterfaceId,int flag)
{
	double dTxTime = 0.0, dPacketLength;	
	NETSIM_ID nProtocolId = DEVICE_MACLAYER(nDevId,nInterfaceId)->nMacProtocolId;
	PHY_VAR *pstruPhy = (PHY_VAR*)DEVICE_PHYVAR(nDevId,nInterfaceId);	
	switch(pstruPhy->nPHYprotocol)
	{
	case IEEE_802_11a:
	case IEEE_802_11b:
	case IEEE_802_11g:
		if(pstruPacket->nReceiverId == 0)
		{
			dTxTime = ceil( pstruPhy->pstruPhyChar->naPreambleLength
				+ pstruPhy->pstruPhyChar->naPLCPHeaderLength
				+ ( pstruPacket->pstruPhyData->dPacketSize - pstruPacket->pstruPhyData->dOverhead)  *(8/pstruPhy->dControlFrameDataRate));
			return dTxTime;
		}	
		switch(pstruPacket->nControlDataType%(nProtocolId *100))
		{			
		case WLAN_RTS:		
		case WLAN_CTS:		
		case WLAN_ACK: 
			dTxTime = ceil( pstruPhy->pstruPhyChar->naPreambleLength \
				+ pstruPhy->pstruPhyChar->naPLCPHeaderLength \
				+ ( pstruPacket->pstruPhyData->dPacketSize - pstruPacket->pstruPhyData->dOverhead) *(8/pstruPhy->dControlFrameDataRate));
			break;
		default:
			dTxTime = pstruPhy->pstruPhyChar->naPreambleLength;
			dTxTime += pstruPhy->pstruPhyChar->naPLCPHeaderLength;
			dTxTime += ( pstruPacket->pstruPhyData->dPacketSize - pstruPacket->pstruPhyData->dOverhead) *(8/pstruPhy->dDataRate);
			dTxTime = ceil(dTxTime);
			/*ceil( pstruPhy->pstruPhyChar->naPreambleLength \
			+ pstruPhy->pstruPhyChar->naPLCPHeaderLength \
			+ ( pstruPacket->pstruPhyData->dPacketSize - pstruPacket->pstruPhyData->dOverhead) *(8/pstruPhy->dDataRate));*/
			break;
		}
		break;	
	case IEEE_802_11n:
		if(pstruPacket->nReceiverId == 0)
		{
			dPacketLength = fnGetPacketSize(pstruPacket);
			if(flag)
				dTxTime = fn_NetSim_WLAN_802_11n_TxTimeCalculation(dPacketLength, 0, nDevId, nInterfaceId,1);
			else
				dTxTime = fn_NetSim_WLAN_802_11n_TxTimeCalculation(dPacketLength, 0, nDevId, nInterfaceId,0);

			return dTxTime;
		}
		switch(pstruPacket->nControlDataType%(nProtocolId *100))
		{			
		case WLAN_RTS:		
		case WLAN_CTS:		
		case WLAN_ACK: 
			//dTxTime = fn_NetSim_WLAN_802_11n_TxTimeCalculation(pstruPacket,nDevId, nInterfaceId,1);
			dPacketLength = fnGetPacketSize(pstruPacket);
			dTxTime = fn_NetSim_WLAN_802_11n_TxTimeCalculation(dPacketLength, 0, nDevId, nInterfaceId,1);
			break;
		case WLAN_BlockACK:
			//dTxTime = fn_NetSim_WLAN_802_11n_TxTimeCalculation(pstruPacket,nDevId, nInterfaceId,1);
			dPacketLength = fnGetPacketSize(pstruPacket);
			dTxTime = fn_NetSim_WLAN_802_11n_TxTimeCalculation(dPacketLength, 0, pstruPacket->nReceiverId, fn_NetSim_Stack_GetWirelessInterface(DEVICE_PHYLAYER(nDevId, nInterfaceId)->nLinkId,pstruPacket->nReceiverId),1);
			break;
		default:
			//dTxTime = fn_NetSim_WLAN_802_11n_TxTimeCalculation(pstruPacket,nDevId, nInterfaceId,1);
			dPacketLength = fnGetPacketSize(pstruPacket);
			if(flag)
				dTxTime = fn_NetSim_WLAN_802_11n_TxTimeCalculation(dPacketLength, 1, nDevId, nInterfaceId,1);
			else
				dTxTime = fn_NetSim_WLAN_802_11n_TxTimeCalculation(dPacketLength, 1, nDevId, nInterfaceId,0);
			break;
		}		
		break;	
	case IEEE_802_11ac:
		if(pstruPacket->nReceiverId == 0)
		{
			dPacketLength = fnGetPacketSize(pstruPacket);
			if(flag)
				dTxTime = fn_NetSim_WLAN_802_11ac_TxTimeCalculation(dPacketLength, 0, nDevId, nInterfaceId,1);
			else
				dTxTime = fn_NetSim_WLAN_802_11ac_TxTimeCalculation(dPacketLength, 0, nDevId, nInterfaceId,0);

			return dTxTime;
		}
		switch(pstruPacket->nControlDataType%(nProtocolId *100))
		{			
		case WLAN_RTS:		
		case WLAN_CTS:		
		case WLAN_ACK: 
			//dTxTime = fn_NetSim_WLAN_802_11n_TxTimeCalculation(pstruPacket,nDevId, nInterfaceId,1);
			dPacketLength = fnGetPacketSize(pstruPacket);
			dTxTime = fn_NetSim_WLAN_802_11ac_TxTimeCalculation(dPacketLength, 0, nDevId, nInterfaceId,1);
			break;
		case WLAN_BlockACK:
			//dTxTime = fn_NetSim_WLAN_802_11n_TxTimeCalculation(pstruPacket,nDevId, nInterfaceId,1);
			dPacketLength = fnGetPacketSize(pstruPacket);
			dTxTime = fn_NetSim_WLAN_802_11ac_TxTimeCalculation(dPacketLength, 0, pstruPacket->nReceiverId, fn_NetSim_Stack_GetWirelessInterface(DEVICE_PHYLAYER(nDevId, nInterfaceId)->nLinkId,pstruPacket->nReceiverId),1);
			break;
		default:
			//dTxTime = fn_NetSim_WLAN_802_11n_TxTimeCalculation(pstruPacket,nDevId, nInterfaceId,1);
			dPacketLength = fnGetPacketSize(pstruPacket);
			if(flag)
				dTxTime = fn_NetSim_WLAN_802_11ac_TxTimeCalculation(dPacketLength, 1, nDevId, nInterfaceId,1);
			else
				dTxTime = fn_NetSim_WLAN_802_11ac_TxTimeCalculation(dPacketLength, 1, nDevId, nInterfaceId,0);
			break;
		}		
		break;
	default:
		printf("WLAN--- Unknown Protocol type Calculate Transmission Time\n");
		break;
	}
	return dTxTime;	
}

/**
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
This function is used to configure the PhySical Layrer parameters of device.
Configure the Preamble and PLCP header length.
Configure Control and broadcast frame data rate.
Calculate DIFS, EIFS and RIFS values.
Assign CS Threshold Value.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
int fn_NetSim_WLAN_PHY_Configuration(NETSIM_ID nDeviceId,NETSIM_ID nInterfaceId)
{
	PHY_VAR *pstruPhyVar;	
	pstruPhyVar = (PHY_VAR*)DEVICE_INTERFACE(nDeviceId,nInterfaceId)->pstruPhysicalLayer->phyVar;    
	//NETWORK->ppstruDeviceList[nDeviceId]->ppstruInterfaceList[nInterfaceId]->pstruPhysicalLayer->phyVar;
	switch(pstruPhyVar->PhyType)
	{
	case OFDM:
		pstruPhyVar->pstruPhyChar->naPLCPHeaderLength = 4 ;	//MicroSecs
		pstruPhyVar->pstruPhyChar->naPreambleLength = 16;	//MicroSecs
		pstruPhyVar->pstruPhyChar->naCCATime = 4;			//MicroSecs
		pstruPhyVar->pstruPhyChar->naMACProcessingDelay = 2;//MicroSecs
		pstruPhyVar->pstruPhyChar->naMPDUDurationFactor = 0;
		pstruPhyVar->pstruPhyChar->naMPDUMaxLength = 4095;
		pstruPhyVar->pstruPhyChar->naPHY_RX_START_Delay = 25;//MicroSecs
		pstruPhyVar->pstruPhyChar->naRxPLCPDelay = 1;		//MicroSecs
		pstruPhyVar->pstruPhyChar->naRxRFDelay = 1;			//MicroSecs
		pstruPhyVar->pstruPhyChar->naRxTxSwitchTime = 1;	// MicroSecs
		pstruPhyVar->pstruPhyChar->naRxTxTurnaroundTime = 2;//MicroSecs
		pstruPhyVar->pstruPhyChar->naTxPLCPDelay = 1;		//MicroSecs
		pstruPhyVar->pstruPhyChar->naTxRampOnTime = 1;		//MicroSecs				
		//Assign Control frame and broadcast frame data rate
		pstruPhyVar->dControlFrameDataRate = CONTRL_FRAME_RATE_11A_AND_G;	
		pstruPhyVar->dBroadCastFrameDataRate = CONTRL_FRAME_RATE_11A_AND_G;	
		pstruPhyVar->dReceiverSensivity = -82; //dBm for 802.11a and g // Check	
		pstruPhyVar->dCSThreshold = -85;
		break;
	case HT:
		fn_NetSim_WLAN_802_11n_OFDM_MIMO_ParameterIntialization(nDeviceId,nInterfaceId);				
		pstruPhyVar->dReceiverSensivity = -82; //dBm for 802.11a and g // Check	
		pstruPhyVar->dCSThreshold = -85;
		break;
	case VHT:
		fn_NetSim_WLAN_802_11ac_OFDM_MIMO_ParameterIntialization(nDeviceId,nInterfaceId);				
		pstruPhyVar->dReceiverSensivity = -82; //dBm for 802.11ac
		pstruPhyVar->dCSThreshold = -85;
		break;
	case DSSS:
	default:
		pstruPhyVar->pstruPhyChar->naPLCPHeaderLength = 48;	//MicroSecs 
		pstruPhyVar->pstruPhyChar->naPreambleLength = 144;	//MicroSecs 
		pstruPhyVar->pstruPhyChar->naCCATime = 15;			//MicroSecs
		pstruPhyVar->pstruPhyChar->naMACProcessingDelay = 2;//MicroSecs
		pstruPhyVar->pstruPhyChar->naMPDUDurationFactor = 0;
		pstruPhyVar->pstruPhyChar->naMPDUMaxLength = 8192;
		pstruPhyVar->pstruPhyChar->naPHY_RX_START_Delay = 192;//MicroSecs
		pstruPhyVar->pstruPhyChar->naRxPLCPDelay = 1;		//MicroSecs
		pstruPhyVar->pstruPhyChar->naRxRFDelay = 1;			//MicroSecs
		pstruPhyVar->pstruPhyChar->naRxTxSwitchTime = 4;	// MicroSecs
		pstruPhyVar->pstruPhyChar->naRxTxTurnaroundTime = 5;//MicroSecs
		pstruPhyVar->pstruPhyChar->naTxPLCPDelay = 1;		//MicroSecs
		pstruPhyVar->pstruPhyChar->naTxRampOnTime = 1;		//MicroSecs	
		pstruPhyVar->dControlFrameDataRate = CONTRL_FRAME_RATE_11B;	//2
		pstruPhyVar->dBroadCastFrameDataRate = CONTRL_FRAME_RATE_11B;//2	
		pstruPhyVar->dReceiverSensivity = -80; //dBm for 802.11b	// Check
		pstruPhyVar->dCSThreshold = -100; //-80;//-97
		break;
	}

	pstruPhyVar->CcaMode = CS_ONLY;	
	// calculate DIFS
	pstruPhyVar->nDIFS = pstruPhyVar->pstruPhyChar->naSIFSTime + (2 * pstruPhyVar->pstruPhyChar->naSlotTime);
	pstruPhyVar->nEIFS = pstruPhyVar->pstruPhyChar->naSIFSTime + pstruPhyVar->nDIFS + (int) (ACK_SIZE * 8.0 / pstruPhyVar->dControlFrameDataRate) + 1;
	pstruPhyVar->nPIFS =  pstruPhyVar->pstruPhyChar->naSIFSTime +  pstruPhyVar->pstruPhyChar->naSlotTime;
	return 0;
}

/**
This function is called in the fn_NetSim_WLAN_PhysicalOut() function.
Add the PHY header pstruPacket->pstruPhyData->Packet_PhyData to the packet. 
*/
int fn_NetSim_WLAN_Add_Phy_Header()
{
	NETSIM_ID nDeviceId = pstruEventDetails->nDeviceId;
	NETSIM_ID nInterfaceId = pstruEventDetails->nInterfaceId;
	PHY_VAR *pstruPhyVar = (PHY_VAR*)DEVICE_PHYVAR(nDeviceId,nInterfaceId);
	NetSim_PACKET *pstruPacket = pstruEventDetails->pPacket;
	DSSS_PPDU *pstruDsss;
	OFDM_PPDU *pstruOfdm;
	HT_PPDU *pstruHT;
	VHT_PPDU *pstruVHT;
	switch(pstruPhyVar->nPHYprotocol)
	{
	case IEEE_802_11b:
		pstruDsss = (DSSS_PPDU*)fnpAllocateMemory(1, sizeof *pstruDsss);
		switch(pstruPacket->nControlDataType%(pstruEventDetails->nProtocolId *100))
		{			
		case WLAN_RTS:		
		case WLAN_CTS:		
		case WLAN_ACK: 
		case WLAN_BlockACK:
			pstruDsss->nSIGNAL = Rate_1Mbps;
			break;
		default:
			pstruDsss->nSIGNAL = pstruPhyVar->DsssRate;
			break;
		}		
		pstruPacket->pstruPhyData->Packet_PhyData = pstruDsss;
		break;
	case IEEE_802_11a:
	case IEEE_802_11g:
		pstruOfdm = (OFDM_PPDU*)fnpAllocateMemory(1, sizeof *pstruOfdm);
		switch(pstruPacket->nControlDataType%(pstruEventDetails->nProtocolId *100))
		{			
		case WLAN_RTS:		
		case WLAN_CTS:		
		case WLAN_ACK: 
		case WLAN_BlockACK:
			pstruOfdm->nRATE = Rate_6Mbps;
			break;
		default:
			pstruOfdm->nRATE = pstruPhyVar->OfdmRate;
			break;
		}		
		pstruPacket->pstruPhyData->Packet_PhyData = pstruOfdm;
		break;
	case IEEE_802_11n:
		pstruHT = (HT_PPDU*)fnpAllocateMemory(1,WLAN_PHY_DATA_SIZE);		
		// Assign the relevent values of sig field
		pstruHT->nMCS = pstruPhyVar->nMCS;
		if(pstruPhyVar->pstruOfdmMimoParameter->nCH_BANDWIDTH == HT_CBW40)
			pstruHT->bCBW_20_40 = 1; //0-20MHz/40MHz upper/lower, 1-40MHz	
		if(pstruPhyVar->pstruOfdmMimoParameter->nAGGREGATION == AGGREGATED)
			pstruHT->bAggregation = 1;		
		if(pstruPhyVar->pstruOfdmMimoParameter->nSOUNDING == NOT_SOUNDING)
			pstruHT->bNot_Sounding = 1;		
		if(pstruPhyVar->pstruOfdmMimoParameter->nSMOOTHING == SMOOTHING_REC)
			pstruHT->bSmoothing = 1;
		//if(pstruPhyVar->pstruOfdmMimoParameter->nGI_TYPE == SHORT_GI)
		if(pstruPhyVar->nGuardInterval == 400)
			pstruHT->bShort_GI = 1;
		if(pstruPhyVar->pstruOfdmMimoParameter->nFEC_CODING == LDPC_CODING)
			pstruHT->bFEC_coding = 1;
		pstruPacket->pstruPhyData->Packet_PhyData  = pstruHT;		
		break;
	case IEEE_802_11ac:
		pstruVHT = (VHT_PPDU*)fnpAllocateMemory(1,WLAN_PHY_DATA_SIZE);		
		// Assign the relevent values of sig field
		pstruVHT->nMCS = pstruPhyVar->nMCS;
		if(pstruPhyVar->pstruOfdmACMimoParameter->nCH_BANDWIDTH == HT_CBW40)
			pstruVHT->bCBW_20_40 = 1; //0-20MHz/40MHz upper/lower, 1-40MHz	
		if(pstruPhyVar->pstruOfdmACMimoParameter->nAGGREGATION == AGGREGATED)
			pstruVHT->bAggregation = 1;		
		if(pstruPhyVar->pstruOfdmACMimoParameter->nSOUNDING == NOT_SOUNDING)
			pstruVHT->bNot_Sounding = 1;		
		if(pstruPhyVar->pstruOfdmACMimoParameter->nSMOOTHING == SMOOTHING_REC)
			pstruVHT->bSmoothing = 1;
		//if(pstruPhyVar->pstruOfdmMimoParameter->nGI_TYPE == SHORT_GI)
		if(pstruPhyVar->nGuardInterval == 400)
			pstruVHT->bShort_GI = 1;
		if(pstruPhyVar->pstruOfdmACMimoParameter->nFEC_CODING == LDPC_CODING)
			pstruVHT->bFEC_coding = 1;
		pstruPacket->pstruPhyData->Packet_PhyData  = pstruVHT;		
		break;
	default:
		break;
	}
	return 0;
}

/**
This function used to calculate probability density function.
*/
double fn_WLAN_pp(double x,double p)
{
	double x1,d,i;
	double first=0.0;
	double second=0.0;
	double ncr=0.0;
	double ncr1=0.0;

	d=x;
	//For Find ODD or Even
	x1=(double)x/2.0;
	//If it's EVEN
	if(floor(x1)-(x/2) >= 0)
	{
		i= (double)(d/2)+1;
		for(;i<=d;i++)
		{
			ncr=(double)fn_WLAN_PHY_ncr ((int)i,(int)d);
			second = second + (double)( ncr * ( pow(p,i)* pow( (1-p),(d-i) ) ) );
		}
		i= (double)d/2;
		ncr1=(double)fn_WLAN_PHY_ncr ((int)i,(int)d);
		i= (double)d/2;
		first = (double) ( (1/2.0) * ncr1 * pow(p,i) * pow( (1-p),i) );	
		first=first+second;

	}
	//If it,s ODD
	else
	{
		i= (double)(d+1)/2;
		for(;i<=d;i++)
		{
			ncr=(double)fn_WLAN_PHY_ncr ((int)i,(int)d);
			first = first + (double)( ncr * ( pow(p,i)* pow( (1-p),(d-i) ) ) );
		}
	}

	return first;
}
/**
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
This function is used to Compute erf(x)
Implemented Q(x) using erf function = erfc(1/sqrt(x)) = 1/2 - ( ( 1/2) * erf(x)/sqrt(2) ) )
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
double fn_WLAN_PHY_erf(double x)
{
	double t,y;
	double a1 =  0.254829592;
	double a2 = -0.284496736;
	double a3 =  1.421413741;
	double a4 = -1.453152027;
	double a5 =  1.061405429;
	double p  =  0.3275911;
	//#define a1 0.254829592
	//#define a2 -0.284496736
	//#define a3 1.421413741
	//#define a4 -1.453152027
	//#define a5 1.061405429
	//#define p 0.3275911
	int sign;    
	sign = 1;
	if( x < 0)
		sign = -1;
	x = (double)abs((int)x);  
	t = 1.0/(1.0 + p*x);
	y = 1.0 - (((((a5*t + a4)*t) + a3)*t + a2)*t + a1)*t*exp(-x*x);
	return (sign*y);
}

/**
This function is used to calculate gamma(x) function.
*/
double gamma(double x)
{
	double M_pi = 3.14; 
	int i,k,m;
	double ga,gr,r=0.0,z;
	static double g[] = {
		1.0,
		0.5772156649015329,
		-0.6558780715202538,
		-0.420026350340952e-1,
		0.1665386113822915,
		-0.421977345555443e-1,
		-0.9621971527877e-2,
		0.7218943246663e-2,
		-0.11651675918591e-2,
		-0.2152416741149e-3,
		0.1280502823882e-3,
		-0.201348547807e-4,
		-0.12504934821e-5,
		0.1133027232e-5,
		-0.2056338417e-6,
		0.6116095e-8,
		0.50020075e-8,
		-0.11812746e-8,
		0.1043427e-9,
		0.77823e-11,
		-0.36968e-11,
		0.51e-12,
		-0.206e-13,
		-0.54e-14,
		0.14e-14};

		if (x > 171.0) 
			return 1e308;    // This value is an overflow flag.
		if (x == (int)x) 
		{
			if (x > 0.0) 
			{
				ga = 1.0;               // use factorial
				for (i=2;i<x;i++) 
				{
					ga *= i;
				}
			}
			else
				ga = 1e308;
		}
		else 
		{
			if (fabs(x) > 1.0) 
			{
				z = fabs(x);
				m = (int)z;
				r = 1.0;
				for (k=1;k<=m;k++) 
				{
					r *= (z-k);
				}
				z -= m;
			}
			else
				z = x;
			gr = g[24];
			for (k=23;k>=0;k--) 
			{
				gr = gr*z+g[k];
			}
			ga = 1.0/(gr*z);
			if (fabs(x) > 1.0) 
			{
				ga *= r;
				if (x < 0.0) 
				{
					ga = -M_pi/(x*ga*sin(M_pi*x));
				}
			}
		}
		return ga;
}


/**
This function is used to Compute the Q function using Approximation Q_5.
*/
double fn_WLAN_Q_5(double x)
{
	double x2,x3,x4,x5,x6;
	x2 = x*x;
	x3 = x2*x;
	x4 = x3*x;
	x5 = x4*x;
	x6 = x5*x;
	return (exp(-x2/2) * (x4+9*x2+8) /(x5+10*x3+15*x) / sqrt(2*(22/7)));
}
/**
This function is used to calculate BER for 11 Mbps data rate using CCK modulation.
*/
double fu_NetSim_WLAN_PHY_BER_CCK11(int k, int M, float fTotal_pr)
{
	float No,Chip_Rate,Rxpower,EcbyNodB, EbByNodB;
	double f_gama, f_consatnt, f_Num, f_Den;
	int m;
	double fBER;
	double EbByNo;

	No = -174; // dBm
	Chip_Rate = (float)70.4; // dB = 11 Mchpis/ Sec
	Rxpower = fTotal_pr;
	EcbyNodB = Rxpower  - (No + Chip_Rate);
	EbByNodB = 	EcbyNodB;
	EbByNo = pow(10,EbByNodB /10);
	f_consatnt = pow(2,k-1) / ((pow(2,k)) - 1);
	f_gama = sqrt(2 * EbByNo);
	fBER = 0;
	for(m = 1; m <= M-1; m++)
	{
		f_Num = pow(-1,m+1) * fn_WLAN_PHY_ncr(m, M-1);
		f_Den = 1 + m + ( m * EbByNo);
		fBER = fBER  +  (f_Num / f_Den);
	}
	fBER = fBER * f_consatnt;
	return(fBER);
}
/**
This function is used to calculate BER for 5.5 Mbps data rate using CCK modulation.
*/
double fu_NetSim_WLAN_PHY_BER_CCK55(int k, int M, float fTotal_pr)
{	
	float No, Chip_Rate, Rxpower,EcbyNodB, EbByNodB;
	float f_gama, f_consatnt, f_Num, f_Den;
	int m;
	double EbByNo;
	double fBER;
	No = -174; // dBm
	Chip_Rate = (float)70.4; // dB = 11 Mchpis/ Sec
	Rxpower = fTotal_pr;
	EcbyNodB = Rxpower  - (No + Chip_Rate);
	EbByNodB = 	EcbyNodB+ 3;
	EbByNo = pow(10,(EbByNodB /10));
	f_consatnt = (float)(pow(2,k-1) / ((pow(2,k)) - 1));
	f_gama = (float)sqrt(2 * EbByNo);
	fBER = 0;
	for(m = 1; m <= M-1; m++)
	{
		f_Num = (float)(pow(-1,m+1) * fn_WLAN_PHY_ncr(m, M-1));
		f_Den = 1 + m + ( m * (float)EbByNo);
		fBER = fBER  +  (f_Num / f_Den);
	}
	fBER = fBER * f_consatnt;
	return(fBER);
}
/**
This function is used to calculate BER for 2 Mbps data rate using DQPSK modulation.
*/
double fn_NetSim_WLAN_PHY_BER_DQPSK(float fRxPower)
{
	float No,  PG, Chip_Rate, EcbyNodB, EbByNodB;
	double EbByNo;
	double dtemp, dtemp1, dtemp2;
	double fBER;
	No = -174; // dB
	Chip_Rate = (float)70.4; // dB = 11 Mchpis/ Sec
	PG = (float)7.4; // dB
	EcbyNodB = fRxPower  - (No + Chip_Rate);
	EbByNodB = 	EcbyNodB + PG;
	EbByNo = pow(10,(EbByNodB /10));
	dtemp = sqrt(EbByNo / (EbByNo + 1));
	dtemp2 = 2 - pow(dtemp,2);
	dtemp1 = sqrt(dtemp2);
	fBER = (double) (1 - (dtemp /dtemp2))/ 2;
	return(fBER);
}

/**
This function is used to calculate BER for 1 Mbps data rate using DBPSK modulation.
*/
double  fu_NetSim_WLAN_PHY_BER_BDPSK(float fRxPower)
{
	double dBER;
	double dEbByNo;
	float fEbByNodB;
	float fEcByNodB;
	float fNodB;
	float fChip_RatedB;
	float fPG;
	fNodB = -174; // dB
	fChip_RatedB = (float)70.4; // dB = 11 Mchpis/ Sec
	fPG = (float)10.4; //dB for 1Mbps, Eb = 11 Ec
	fEcByNodB = fRxPower - (fNodB + fChip_RatedB);
	fEbByNodB = fEcByNodB + fPG ;
	dEbByNo = pow(10,(fEbByNodB/10));
	dBER = 1 / (2 * (1 + dEbByNo));
	return(dBER);
}

/**
This function is used for nCr calculation 
*/
double fn_WLAN_PHY_ncr (int i, int n)
{
	double ncr=0,ncrup=1,ncrden=1;
	int l;
	for(l=n;l>n-i;l--)
	{
		ncrup= ncrup*l;
	}
	for(l=i;l>=1;l--)
	{
		ncrden=ncrden*l;
	}
	ncr=ncrup/ncrden;
	return(ncr);
}


/**
Transmit the packet in the Medium, i.e from Physical out to Physical In.
While transmitting check whether the Receiver radio state is CHANNEL_IDLE and also 
is the receiver is reachable, that is not an out off reach. If both condition 
satisfied then add the PHY IN EVENT, else drop the frame.
*/
int fn_NetSim_WLAN_TransmitFrame(NetSim_PACKET* pstruPacket, NETSIM_ID nDevId, NETSIM_ID nInterfaceId, NETSIM_ID nConDevId,NETSIM_ID nConInterface, int SubEventType)
{
	PHY_VAR *pstruPhy = WLAN_PHY(nConDevId,nConInterface);
	PHY_VAR *pstruTxPhy = WLAN_PHY(nDevId,nInterfaceId);
	MAC_VAR *pstruTxMac;	
	NetSim_PACKET *pstruPacketList=pstruPacket;
	NetSim_EVENTDETAILS pevent;
	//Update the transmitter and receiver
	while(pstruPacketList)
	{
		pstruPacketList->nReceiverId = nConDevId;
		pstruPacketList->nTransmitterId = nDevId;
		pstruPacketList=pstruPacketList->pstruNextPacket;
	}

	if(pstruPhy->RadioState == RX_ON_IDLE && dCummulativeReceivedPower[nDevId][nConDevId] > pstruTxPhy->dMinReceiverSensitivity)
	{
		// Change the Receiver state
		if(!WLAN_CHANGERADIOSTATE(nConDevId,nConInterface,RX_ON_BUSY))
			return -1; //Node is off

		pstruPacketList=pstruPacket;
		while(pstruPacketList)
		{
			memcpy(&pevent,pstruEventDetails,sizeof* pstruEventDetails);
			pevent.dEventTime = pstruPacketList->pstruPhyData->dEndTime;
			pevent.dPacketSize = pstruPacketList->pstruPhyData->dPacketSize;
			if(pstruPacketList->pstruAppData)
			{
				pevent.nApplicationId = pstruPacketList->pstruAppData->nApplicationId;
				pevent.nSegmentId = pstruPacketList->pstruAppData->nSegmentId;
			}
			else
			{
				pevent.nApplicationId = 0;
				pevent.nSegmentId = 0;
			}
			pevent.nDeviceId = nConDevId;
			pevent.nDeviceType = NETWORK->ppstruDeviceList[nConDevId-1]->nDeviceType;
			pevent.nEventType = PHYSICAL_IN_EVENT;
			pevent.nInterfaceId = nConInterface;
			pevent.nPacketId = pstruPacketList->nPacketId;		
			pevent.nProtocolId = fn_NetSim_Stack_GetMacProtocol(nConDevId,nConInterface);
			pevent.pPacket=pstruPacketList;
			
			//Set the subevent
			if((pstruPhy->nPHYprotocol==IEEE_802_11n||pstruPhy->nPHYprotocol==IEEE_802_11ac)
				&& pstruPacket->nDestinationId!=0				)
			{				
				pevent.nSubEventType = SubEventType;
				SubEventType = AMPDU_SUBFRAME;
			}
			else
			{
				pevent.nSubEventType = 0;
			}
			fnpAddEvent(&pevent);
			pstruPacketList = pstruPacketList->pstruNextPacket;
		}
		return 1;
	}
	else if(pstruPhy->RadioState != RX_OFF)
	{
		pstruPacketList=pstruPacket;
		pstruTxMac = WLAN_MAC(nDevId,nInterfaceId);
		if(dCummulativeReceivedPower[nDevId][nConDevId] < pstruTxPhy->dMinReceiverSensitivity)
		{
			// Not able to decode at the receiver, so make it as error packet
			while(pstruPacketList)
			{
				pstruPacketList->nPacketStatus = PacketStatus_Error;
				// Increament out off reach packet count
				pstruTxMac->pstruMetrics->nOutOffReachFrameCount++;
				pstruPacketList=pstruPacketList->pstruNextPacket;
			}

		}
		if(pstruPhy->RadioState != RX_ON_IDLE)
		{
			pstruPacketList=pstruPacket;
			while(pstruPacketList)
			{
				pstruPacketList->nPacketStatus = PacketStatus_Collided;			
				pstruTxMac->pstruMetrics->nCollidedFrameCount++;
				pstruPacketList=pstruPacketList->pstruNextPacket;
			}
		}
		// Change the radio state of the TRANSMITTER back to CHANNEL_IDLE
		WLAN_CHANGERADIOSTATE(nDevId,nInterfaceId,RX_ON_IDLE);
		// Write packet trace
		pstruPacketList=pstruPacket;
		while(pstruPacketList)
		{
			pstruPacket = pstruPacketList;
			pstruPacketList = pstruPacketList->pstruNextPacket;
			fn_NetSim_WritePacketTrace(pstruPacket);	 		
			fn_NetSim_Metrics_Add(pstruPacket);
			fn_NetSim_Packet_FreePacket(pstruPacket);
			pstruPacket = NULL;
			pstruEventDetails->pPacket=NULL;
		}
		return 0;
	}
	return 1;
}

