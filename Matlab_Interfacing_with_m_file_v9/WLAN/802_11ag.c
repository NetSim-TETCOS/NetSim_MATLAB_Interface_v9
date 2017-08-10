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
#include "math.h"
/**
This function is used to calculate the data rate for IEEE 802.11a and g.
*/
int fn_NetSim_WLAN_802_11a_11g_DataRate(NETSIM_ID nDeviceId, NETSIM_ID nInterfaceId, NETSIM_ID nReceiverId)
{
	MAC_VAR* pstrumac = WLAN_MAC(nDeviceId,nInterfaceId);
	PHY_VAR *pstruPhy = WLAN_PHY(nDeviceId,nInterfaceId);	
	NETSIM_ID nProtocol = DEVICE_MACLAYER(nDeviceId,nInterfaceId)->nMacProtocolId;
	unsigned int index = pstruPhy->currentPhyindex[nReceiverId];

	if(nReceiverId)
	{
		if(pstrumac->rate_adaptationflag)
		{
			pstruPhy->dDataRate = struPhyParameters[index].dDataRate; // 54Mbps
			pstruPhy->dReceiverSensivity = struPhyParameters[index].dRxSensitivity;
			pstruPhy->Modulation = struPhyParameters[index].nModulation;
			switch(index)
			{
			case 12:
				pstruPhy->OfdmRate = Rate_54mbps;
				break;
			case 11:
				pstruPhy->OfdmRate = Rate_48Mbps;
				break;
			case 10:
				pstruPhy->OfdmRate = Rate_36Mbps;
				break;
			case 9:
				pstruPhy->OfdmRate = Rate_24Mbps;
				break;
			case 8:
				pstruPhy->OfdmRate = Rate_18Mbps;
				break;
			case 7:
				pstruPhy->OfdmRate = Rate_12Mbps;
				break;
			case 6:
				pstruPhy->OfdmRate = Rate_9Mbps;
				break;
			case 5:
				pstruPhy->OfdmRate = Rate_6Mbps;
				break;
			default:
				pstruPhy->OfdmRate = 0;
				break;
			}
		}
		else
		{
			if(pstruPhy->dTotalReceivedPower >= struPhyParameters[12].dRxSensitivity)
			{
				pstruPhy->dDataRate = struPhyParameters[12].dDataRate; // 54Mbps
				pstruPhy->dReceiverSensivity = struPhyParameters[12].dRxSensitivity;
				pstruPhy->Modulation = struPhyParameters[12].nModulation;
				pstruPhy->OfdmRate = Rate_54mbps;

			}
			else if( pstruPhy->dTotalReceivedPower < struPhyParameters[12].dRxSensitivity && pstruPhy->dTotalReceivedPower >= struPhyParameters[11].dRxSensitivity)
			{
				pstruPhy->dDataRate = struPhyParameters[11].dDataRate; //48Mbps
				pstruPhy->dReceiverSensivity = struPhyParameters[11].dRxSensitivity;
				pstruPhy->Modulation = struPhyParameters[11].nModulation;
				pstruPhy->OfdmRate = Rate_48Mbps;

			}
			else if( pstruPhy->dTotalReceivedPower < struPhyParameters[11].dRxSensitivity && pstruPhy->dTotalReceivedPower >= struPhyParameters[10].dRxSensitivity)
			{
				pstruPhy->dDataRate = struPhyParameters[10].dDataRate; // 36Mbps
				pstruPhy->dReceiverSensivity = struPhyParameters[10].dRxSensitivity;
				pstruPhy->Modulation = struPhyParameters[10].nModulation;
				pstruPhy->OfdmRate = Rate_36Mbps;
			}
			else if( pstruPhy->dTotalReceivedPower < struPhyParameters[10].dRxSensitivity && pstruPhy->dTotalReceivedPower >= struPhyParameters[9].dRxSensitivity)
			{
				pstruPhy->dDataRate = struPhyParameters[9].dDataRate; // 24Mbps
				pstruPhy->dReceiverSensivity = struPhyParameters[9].dRxSensitivity;
				pstruPhy->Modulation = struPhyParameters[9].nModulation;
				pstruPhy->OfdmRate = Rate_24Mbps;
			}
			else if(pstruPhy->dTotalReceivedPower < struPhyParameters[9].dRxSensitivity && pstruPhy->dTotalReceivedPower >= struPhyParameters[8].dRxSensitivity)
			{
				pstruPhy->dDataRate = struPhyParameters[8].dDataRate; // 18Mbps
				pstruPhy->dReceiverSensivity = struPhyParameters[8].dRxSensitivity;
				pstruPhy->Modulation = struPhyParameters[8].nModulation;
				pstruPhy->OfdmRate = Rate_18Mbps;
			}
			else if(pstruPhy->dTotalReceivedPower < struPhyParameters[8].dRxSensitivity && pstruPhy->dTotalReceivedPower >= struPhyParameters[7].dRxSensitivity)
			{
				pstruPhy->dDataRate = struPhyParameters[7].dDataRate; // 12Mbps
				pstruPhy->dReceiverSensivity = struPhyParameters[7].dRxSensitivity;
				pstruPhy->Modulation = struPhyParameters[7].nModulation;
				pstruPhy->OfdmRate = Rate_12Mbps;
			}
			else if(pstruPhy->dTotalReceivedPower < struPhyParameters[7].dRxSensitivity && pstruPhy->dTotalReceivedPower >= struPhyParameters[6].dRxSensitivity)
			{
				pstruPhy->dDataRate = struPhyParameters[6].dDataRate; // 9Mbps
				pstruPhy->dReceiverSensivity = struPhyParameters[6].dRxSensitivity;
				pstruPhy->Modulation = struPhyParameters[6].nModulation;
				pstruPhy->OfdmRate = Rate_9Mbps;
			}
			else if(pstruPhy->dTotalReceivedPower < struPhyParameters[6].dRxSensitivity && pstruPhy->dTotalReceivedPower >= struPhyParameters[5].dRxSensitivity)
			{
				pstruPhy->dDataRate = struPhyParameters[5].dDataRate; // 6Mbps
				pstruPhy->dReceiverSensivity = struPhyParameters[5].dRxSensitivity;
				pstruPhy->Modulation = struPhyParameters[5].nModulation;
				pstruPhy->OfdmRate = Rate_6Mbps;
			}		
			else if(pstruPhy->dTotalReceivedPower < struPhyParameters[5].dRxSensitivity)
			{
				// Device out of coverage area
				pstruPhy->dDataRate = 6;	// instead of 0		
				pstruPhy->dReceiverSensivity = -999;
				pstruPhy->OfdmRate = 0;				
			}
		}
	}
	else // Broadcast packets
	{
		pstruPhy->dDataRate = struPhyParameters[5].dDataRate; // 6Mbps
		pstruPhy->dReceiverSensivity = struPhyParameters[5].dRxSensitivity;
		pstruPhy->Modulation = struPhyParameters[5].nModulation;
		pstruPhy->OfdmRate = Rate_6Mbps;	
	}
	pstruPhy->dMinReceiverSensitivity = struPhyParameters[5].dRxSensitivity; // Check out of range packets
	return 0;
}

/**
This function is used to calculate the BER for IEEE 802.11a and g.
*/
int fn_NetSim_WLAN_802_11a_11g_BER(NETSIM_ID nDeviceId,NETSIM_ID nInterfaceId,double dReceivedPower,NetSim_PACKET *pstruPacket)
{
	double dBer = 0.0;
	int nCount = 1;
	double k = 0.0;
	int nDataRate;
	int nModulation;
	int nCodingRate;
	int nNBPSC;
	int nNCBPS;
	int nNDBPS;
	double nx=0.0;
	double ldNpower = -101.0;//dbm //noise power for 20MHz channel.Refer help for more details.
	double ldLoss = -10;
	double ldEbByN0 = 0.0;//db
	double pb = 0.0;
	double fpi=3.1417f;
	double fp=0.0;	
	double fBer1=0.0;
	PHY_VAR *pstruPhy = DEVICE_PHYVAR(nDeviceId,nInterfaceId);
	nDataRate = pstruPhy->OfdmRate;
	switch(nDataRate)
	{
	case Rate_6Mbps:
		nModulation = struPhyParameters[5].nModulation;
		nCodingRate = struPhyParameters[5].nCodingRate;
		nNBPSC = struPhyParameters[5].nNBPSC;
		nNCBPS = struPhyParameters[5].nNCBPS;
		nNDBPS = struPhyParameters[5].nNDBPS;
		break;
	case Rate_9Mbps:
		nModulation = struPhyParameters[6].nModulation;
		nCodingRate = struPhyParameters[6].nCodingRate;
		nNBPSC = struPhyParameters[6].nNBPSC;
		nNCBPS = struPhyParameters[6].nNCBPS;
		nNDBPS = struPhyParameters[6].nNDBPS;
		break;
	case Rate_12Mbps:
		nModulation = struPhyParameters[7].nModulation;
		nCodingRate = struPhyParameters[7].nCodingRate;
		nNBPSC = struPhyParameters[7].nNBPSC;
		nNCBPS = struPhyParameters[7].nNCBPS;
		nNDBPS = struPhyParameters[7].nNDBPS;
		break;
	case Rate_18Mbps:
		nModulation = struPhyParameters[8].nModulation;
		nCodingRate = struPhyParameters[8].nCodingRate;
		nNBPSC = struPhyParameters[8].nNBPSC;
		nNCBPS = struPhyParameters[8].nNCBPS;
		nNDBPS = struPhyParameters[8].nNDBPS;
		break;
	case Rate_24Mbps:
		nModulation = struPhyParameters[9].nModulation;
		nCodingRate = struPhyParameters[9].nCodingRate;
		nNBPSC = struPhyParameters[9].nNBPSC;
		nNCBPS = struPhyParameters[9].nNCBPS;
		nNDBPS = struPhyParameters[9].nNDBPS;
		break;
	case Rate_36Mbps:
		nModulation = struPhyParameters[10].nModulation;
		nCodingRate = struPhyParameters[10].nCodingRate;
		nNBPSC = struPhyParameters[10].nNBPSC;
		nNCBPS = struPhyParameters[10].nNCBPS;
		nNDBPS = struPhyParameters[10].nNDBPS;
		break;
	case Rate_48Mbps:
		nModulation = struPhyParameters[11].nModulation;
		nCodingRate = struPhyParameters[11].nCodingRate;
		nNBPSC = struPhyParameters[11].nNBPSC;
		nNCBPS = struPhyParameters[11].nNCBPS;
		nNDBPS = struPhyParameters[11].nNDBPS;
		break;
	case Rate_54mbps:
		nModulation = struPhyParameters[12].nModulation;
		nCodingRate = struPhyParameters[12].nCodingRate;
		nNBPSC = struPhyParameters[12].nNBPSC;
		nNCBPS = struPhyParameters[12].nNCBPS;
		nNDBPS = struPhyParameters[12].nNDBPS;
		break;
	}

	//Calculate SNR in dB	
	ldEbByN0 = dReceivedPower +ldLoss - ldNpower-30;	

	switch(nModulation)
	{
	case Modulation_BPSK: // DBPSK modulation
		{
			extern BER bpsk_ber[];
			extern size_t bpsk_ber_len;
			pb = calculate_ber(ldEbByN0,bpsk_ber,bpsk_ber_len);
		}
		break;
	case Modulation_QPSK: //DQPSK modulation
		{
			extern BER qpsk_ber[];
			extern size_t qpsk_ber_len;
			pb = calculate_ber(ldEbByN0,qpsk_ber,qpsk_ber_len);
		}
		break;
	case Modulation_16_QAM: 
	case Modulation_64_QAM:
		{
			extern BER qam64_ber[];
			extern size_t qam64_ber_len;
			pb = calculate_ber(ldEbByN0,qam64_ber,qam64_ber_len);
		}
		break;
	default:
		fnNetSimError("WLAN--Unknown Modulation in OFDMA PHY in Calculate BER function");
		pb = 1; // Above the receiver sensitivity. So out of Coverage area.
		break;
	}				
	pstruPhy->dBER = pb; // Assign BER value 				
	return 0;
}




