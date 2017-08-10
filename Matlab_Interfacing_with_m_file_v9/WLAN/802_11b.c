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
This function is used to calculate the datarate for IEEE 802.11b
*/
int fn_NetSim_WLAN_802_11b_DataRate(NETSIM_ID nDeviceId, NETSIM_ID nInterfaceId, NETSIM_ID nReceiverId)
{
	MAC_VAR* pstrumac = WLAN_MAC(nDeviceId,nInterfaceId);
	PHY_VAR *pstruPhy = WLAN_PHY(nDeviceId,nInterfaceId);
	if(pstrumac->rate_adaptationflag)
	{

		unsigned int index = pstruPhy->currentPhyindex[nReceiverId];
		if(nReceiverId)
		{
			pstruPhy->dDataRate = struPhyParameters[index].dDataRate; // 11Mbps
			pstruPhy->dReceiverSensivity = struPhyParameters[index].dRxSensitivity;
			pstruPhy->Modulation = struPhyParameters[index].nModulation;
			switch(index)
			{
			case 4:
				pstruPhy->DsssRate = Rate_11Mbps;
				pstruPhy->dProcessingGain = 1;
				break;
			case 3:
				pstruPhy->DsssRate = Rate_5dot5Mbps;
				pstruPhy->dProcessingGain = 2;
				break;
			case 2:
				pstruPhy->DsssRate = Rate_2Mbps;
				pstruPhy->dProcessingGain = 5.5;
				break;
			case 1:
				pstruPhy->DsssRate = Rate_1Mbps;
				pstruPhy->dProcessingGain = 11;
				break;
			default:
				pstruPhy->dDataRate = 1; //0;			
				pstruPhy->dReceiverSensivity = -999;
				pstruPhy->DsssRate = Rate_1Mbps;//0;
				pstruPhy->dProcessingGain = 11;//0;
				break;
			}
		}
		else // Broadcast packets
		{
			pstruPhy->dDataRate = struPhyParameters[1].dDataRate; // 1Mbps
			pstruPhy->dReceiverSensivity = struPhyParameters[1].dRxSensitivity;
			pstruPhy->Modulation = struPhyParameters[1].nModulation;
			pstruPhy->DsssRate = Rate_1Mbps;
			pstruPhy->dProcessingGain = 11;		
		}
		pstruPhy->dMinReceiverSensitivity = struPhyParameters[1].dRxSensitivity; // Check out of range packets
	}
	else
	{
		PHY_VAR *pstruPhy;	
		pstruPhy = DEVICE_PHYVAR(nDeviceId,nInterfaceId);	
		if(nReceiverId)
		{
			//pstruPhy->dTotalReceivedPower = dCummulativeReceivedPower[nDeviceId][nReceiverId];
			if(pstruPhy->dTotalReceivedPower >= struPhyParameters[4].dRxSensitivity)
			{
				pstruPhy->dDataRate = struPhyParameters[4].dDataRate; // 11Mbps
				pstruPhy->dReceiverSensivity = struPhyParameters[4].dRxSensitivity;
				pstruPhy->Modulation = struPhyParameters[4].nModulation;
				pstruPhy->DsssRate = Rate_11Mbps;
				pstruPhy->dProcessingGain = 1;
			}
			else if( pstruPhy->dTotalReceivedPower < struPhyParameters[4].dRxSensitivity && pstruPhy->dTotalReceivedPower >= struPhyParameters[3].dRxSensitivity)
			{
				pstruPhy->dDataRate = struPhyParameters[3].dDataRate; // 5.5Mbps
				pstruPhy->dReceiverSensivity = struPhyParameters[3].dRxSensitivity;
				pstruPhy->Modulation = struPhyParameters[3].nModulation;
				pstruPhy->DsssRate = Rate_5dot5Mbps;
				pstruPhy->dProcessingGain = 2;
			}
			else if( pstruPhy->dTotalReceivedPower < struPhyParameters[3].dRxSensitivity && pstruPhy->dTotalReceivedPower >= struPhyParameters[2].dRxSensitivity)
			{
				pstruPhy->dDataRate = struPhyParameters[2].dDataRate; // 2Mbps
				pstruPhy->dReceiverSensivity = struPhyParameters[2].dRxSensitivity;
				pstruPhy->Modulation = struPhyParameters[2].nModulation;
				pstruPhy->DsssRate = Rate_2Mbps;
				pstruPhy->dProcessingGain = 5.5;
			}
			else if( pstruPhy->dTotalReceivedPower < struPhyParameters[2].dRxSensitivity && pstruPhy->dTotalReceivedPower >= struPhyParameters[1].dRxSensitivity)
			{
				pstruPhy->dDataRate = struPhyParameters[1].dDataRate; // 1Mbps
				pstruPhy->dReceiverSensivity = struPhyParameters[1].dRxSensitivity;
				pstruPhy->Modulation = struPhyParameters[1].nModulation;
				pstruPhy->DsssRate = Rate_1Mbps;
				pstruPhy->dProcessingGain = 11;
			}
			else if(pstruPhy->dTotalReceivedPower < struPhyParameters[1].dRxSensitivity)
			{
				// Device out of coverage area
				pstruPhy->dDataRate = 1; //0;			
				pstruPhy->dReceiverSensivity = -999;
				pstruPhy->DsssRate = Rate_1Mbps;//0;
				pstruPhy->dProcessingGain = 11;//0;
			}
		}
		else // Broadcast packets
		{
			pstruPhy->dDataRate = struPhyParameters[1].dDataRate; // 1Mbps
			pstruPhy->dReceiverSensivity = struPhyParameters[1].dRxSensitivity;
			pstruPhy->Modulation = struPhyParameters[1].nModulation;
			pstruPhy->DsssRate = Rate_1Mbps;
			pstruPhy->dProcessingGain = 11;		
		}
		pstruPhy->dMinReceiverSensitivity = struPhyParameters[1].dRxSensitivity; // Check out of range packets
	}
	return 0;
}


/**
Function to calculate the BER for IEEE 802.11b
*/
int fn_NetSim_WLAN_802_11b_BER(NETSIM_ID nDeviceId,NETSIM_ID nInterfaceId,double dReceivedPower,NetSim_PACKET* pstruPacket)
{
	double dBer = 0.0;
	int nCount = 1;
	double k = 0.0;
	int nDataRate;
	double ldNpower = -101.0;//dbm //noise power for 20MHz channel.Refer help for more details.
	double ldLoss = -1.70;//db
	double ldEsByN0 = 0.0;//db
	double ldEbByN0 = 0.0;	
	double fpi=3.1417f;
	double pb=0.0;

	PHY_VAR *pstruPhy = WLAN_PHY(nDeviceId,nInterfaceId);
	nDataRate = pstruPhy->DsssRate;
	ldEbByN0 =dReceivedPower + ldLoss - ldNpower-30;
	switch(nDataRate)
	{
	case Rate_1Mbps: // DBPSK modulation
		{
			extern BER bpsk_ber[];
			extern size_t bpsk_ber_len;
			pb = calculate_ber(ldEbByN0,bpsk_ber,bpsk_ber_len);
		}
		break;
	case Rate_2Mbps: //DQPSK modulation
		{
			extern BER qpsk_ber[];
			extern size_t qpsk_ber_len;
			pb = calculate_ber(ldEbByN0,qpsk_ber,qpsk_ber_len);
		}
		break;
	case Rate_5dot5Mbps: //DataRate = 5.5 Mbps, CCK modulation
		{
			extern BER cck5_5_ber[];
			extern size_t cck5_5_ber_len;
			pb = calculate_ber(ldEbByN0,cck5_5_ber,cck5_5_ber_len);
		}
		break;
	case Rate_11Mbps: // DataRate =  11 Mbps, So  CCK11 modulation
		{
			extern BER cck11_ber[];
			extern size_t cck11_ber_len;
			pb = calculate_ber(ldEbByN0,cck11_ber,cck11_ber_len);
		}
		break;
	default:
		fnNetSimError("WLAN--Unknown Modulation in DSSS PHY in Calculate BER function");
		dBer = 1; // Above the receiver sensitivity. So out of Coverage area.
		break;
	}
	pstruPhy->dBER = pb; // Assign BER value 				
	return 0;
}
