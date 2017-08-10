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
This function is called to initialize the MIMO and OFDM parameters relevant to 
IEEE802.11n. 
This function is called from the "fn_NetSim_WLAN_PHY_Configuration()" for HT PHY.
Initialize time parameters as per the Table 20-5—Timing-related constants 802.11n-2009 pdf 
also initialize 802.11n PHY header relevant parameters.
*/
int fn_NetSim_WLAN_802_11n_OFDM_MIMO_ParameterIntialization(NETSIM_ID nDeviceId,NETSIM_ID nInterfaceId)
{
	int nNSTS_MAX = 4;
	int nNSS_MAX = 4;
	int nTxAntCount_MAX = 4;
	int nRxAntCount_MAX = 4;

	PHY_VAR *pstruPhy = NETWORK->ppstruDeviceList[nDeviceId-1]->ppstruInterfaceList[nInterfaceId-1]->pstruPhysicalLayer->phyVar;

	pstruPhy->pstruOfdmMimoParameter = fnpAllocateMemory(1,sizeof(struct stru_802_11n_OFDM_MIMO_Parameters ));
	// Intialize OFDM timing parameters According Table 20-5—Timing-related constants 802.11n-2009 pdf

	if(pstruPhy->nChannelBandwidth == 20)
	{
		pstruPhy->pstruOfdmMimoParameter->nCH_BANDWIDTH = HT_CBW20;
		pstruPhy->pstruOfdmMimoParameter->nNSD = 52; // NSD Number of complex data numbers	
		pstruPhy->pstruOfdmMimoParameter->nNSP = 4;	 // NSP Number of pilot values
		pstruPhy->pstruOfdmMimoParameter->nNST = 56; // 52 + 4
		pstruPhy->pstruOfdmMimoParameter->nNSR = 28; // NSR Highest data subcarrier index
		pstruPhy->pstruOfdmMimoParameter->nFFT = 64; // FFT length	
	}
	else
	{
		pstruPhy->pstruOfdmMimoParameter->nCH_BANDWIDTH = HT_CBW40;
		pstruPhy->pstruOfdmMimoParameter->nNSD = 108; // NSD Number of complex data numbers	
		pstruPhy->pstruOfdmMimoParameter->nNSP = 6;	  // NSP Number of pilot values
		pstruPhy->pstruOfdmMimoParameter->nNST = 114; // 108 + 6
		pstruPhy->pstruOfdmMimoParameter->nNSR = 58;  // NSR Highest data subcarrier index
		pstruPhy->pstruOfdmMimoParameter->nFFT = 128; // FFT length
	}

	// Assign all the timing relevent parameters
	pstruPhy->pstruOfdmMimoParameter->dDeltaF = pstruPhy->nChannelBandwidth / (pstruPhy->pstruOfdmMimoParameter->nFFT * 1.0);//SubcarrierFrequencySpacing 312.5KHz (20 MHz/64) or (40 MHz/128)
	pstruPhy->pstruOfdmMimoParameter->dTDFT = 3.2;  // IDFT/DFT period 3.2 MicroSecs
	pstruPhy->pstruOfdmMimoParameter->dTGI = pstruPhy->pstruOfdmMimoParameter->dTDFT/4; // 3.2/4 = 0.8 MicroSecs  Guard interval duration 0.8 = TDFT/4  
	pstruPhy->pstruOfdmMimoParameter->dTGI2 = pstruPhy->pstruOfdmMimoParameter->dTGI * 2; // 0.8 * 2 = 1.6 MicroSecs Double guard interval 1.6 
	pstruPhy->pstruOfdmMimoParameter->dTGIS = pstruPhy->pstruOfdmMimoParameter->dTDFT/8; // Short guard interval duration   dDFT/8 = 0.4 
	pstruPhy->pstruOfdmMimoParameter->nTL_STF = 8;     // Non-HT short training sequence duration 8 = 10*TDFT/4 
	pstruPhy->pstruOfdmMimoParameter->nTHT_GF_STF = 8; // HT-greenfield short training field duration 8 =10* TDFT/4 
	pstruPhy->pstruOfdmMimoParameter->nTL_LTF = 8;     // Non-HT long training field duration 8 =2 * TDFT + TGI2 
	pstruPhy->pstruOfdmMimoParameter->dTSYM  = 4.0;   //TSYM: Symbol interval 4 =  TDFT + TGI       
	pstruPhy->pstruOfdmMimoParameter->dTSYMS = 3.6;  // TSYMS: Short GI symbol interval = TDFT+TGIS 3.6 
	pstruPhy->pstruOfdmMimoParameter->nTL_SIG = 4;   // Non-HT SIGNAL field duration 4  
	pstruPhy->pstruOfdmMimoParameter->nTHT_SIG = 8;  // HT SIGNAL field duration 8 = 2TSYM 8 
	pstruPhy->pstruOfdmMimoParameter->nTHT_STF = 4;  // HT short training field duration  4 
	pstruPhy->pstruOfdmMimoParameter->nTHT_LTF1 = 8; // First HT long training field duration 4 in HT-mixed format, 8 in HTgreenfield format
	pstruPhy->pstruOfdmMimoParameter->nTHT_LTFs = 4; // Second, and subsequent, HT long training fields

	if(pstruPhy->nGuardInterval == 400)
		pstruPhy->pstruOfdmMimoParameter->nGI_TYPE = SHORT_GI;
	else
		pstruPhy->pstruOfdmMimoParameter->nGI_TYPE = LONG_GI;

	if(pstruPhy->nNTX <= 0)
	{
		fnNetSimError("WLAN--11n Transmitting Antenna Count is <= 0, so assign the value to 1");
		pstruPhy->nNTX = 1; // Assign minimum value
	}
	if(pstruPhy->nNTX > nTxAntCount_MAX)
	{
		fnNetSimError("WLAN--11n Transmitting Antenna Count is > Maximum value, so assign the value to Max count");
		pstruPhy->nNTX = nTxAntCount_MAX; // Assign Maximum value
	}
	if(pstruPhy->nNRX <= 0)
	{
		fnNetSimError("WLAN--11n Receiving Antenna Count is <= 0, so assign the value to 1");
		pstruPhy->nNRX = 1; // Assign minimum value
	}
	if(pstruPhy->nNRX > nRxAntCount_MAX)
	{
		fnNetSimError("WLAN--11n Receiving Antenna Count is > Maximum value, so assign the value to Max count");
		pstruPhy->nNRX = nRxAntCount_MAX; // Assign Maximum value
	}

	// Set the STBC coding
	pstruPhy->nSTBC = 0;  // 0 or 1 or 2 
	// Check and Set NSS value, it should be minimum of transmit and received antenna count
	pstruPhy->nNSS = (pstruPhy->nNTX < pstruPhy->nNRX ? pstruPhy->nNTX: pstruPhy->nNRX);
	if(pstruPhy->nNSS > nNSS_MAX) //  nNSS_MAX = 4
		pstruPhy->nNSS = nNSS_MAX;
	// Set NSTS value 
	pstruPhy->nNSTS = pstruPhy->nNSS  + pstruPhy->nSTBC;
	if(pstruPhy->nNSTS > nNSTS_MAX ) // nNSTS_MAX = 4
		pstruPhy->nNSTS = nNSTS_MAX;
	// Set  HT_DLTFs  Table 20-12—Number of HT-DLTFs required for data space-time streams
	pstruPhy->nN_HT_DLTF = pstruPhy->nNSTS;
	if(pstruPhy->nNSTS == 3) 
		pstruPhy->nN_HT_DLTF = 4;
	// Set NESS   NESS + NSTS <= 4
	pstruPhy->nNESS =  nNSTS_MAX - pstruPhy->nNSTS;
	// Set HT_ELTFs Table 20-13—Number of HT-ELTFs required for extension spatial streams
	pstruPhy->nN_HT_ELTF = pstruPhy->nNESS;
	if(pstruPhy->nNESS == 3)
		pstruPhy->nN_HT_ELTF = 4;
	// Set total HT_LTF = HT_DLTF + HT_ELTF
	pstruPhy->nN_HT_LTF = pstruPhy->nN_HT_DLTF + pstruPhy->nN_HT_ELTF;
	if(pstruPhy->nN_HT_LTF > 5) // Max is 5
		pstruPhy->nN_HT_LTF = 5;

	return 0;
}


/**
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
This function is used to calculate the transmission time. If flag=1, then preamble time is
also taken into consideration. else only the time required to transmit the packetsize 
specified will be returned.
For Short GI
TXTIME = TGF_HT_PREAMBLE + THT_SIG + TSYMS * NSYM + SignalExtension
For Regular/ Long GI
TXTIME = TGF_HT_PREAMBLE + THT_SIG + TSYM * NSYM + SignalExtension
TGF_HT_PREAMBLE    is the duration of the preamble in HT_greenfield format, given by
TGF_HT_PREAMBLE = THT_GF_STF +  THT_ LTF1  +  (NLTF – 1)THT_ LTFs
TSYM, TSYMS, THT-SIG, TL-STF, THT-STF, THT-GF-STF, TL-LTF, THT-LTF1 and THT-LTFs defined in Table20-5 Timing-related constants
SignalExtension   is 0 microsecs when TXVECTOR parameter NO_SIG_EXTN is TRUE and is the duration of signal extension as defined by aSignalExtension in
Table 20- 4 when TXVECTOR parameter NO_SIG_EXTN is FALSE NLTF is defined in Equation (20-22)
NSYM is the total number of data symbols in the data portion, which may be calculated according to Equation (20-95)
For BCC coding 
NSYM = mSTBC * ceil ( (8*length + 16 + 6*NES)/(mSTBC*NDBPS) )
For LDPC
NSYM = Navbits / NCBPS 
length 				is the number of octets in the data portion of the PPDU
mSTBC 				is equal to 2 when STBC is used, and otherwise 1
NES and NCBPS 		are defined in Table 20-6
NDBPS				 is defined in Table 20-28
Navbits   			 is defined in Equation (20-39)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
double fn_NetSim_WLAN_802_11n_TxTimeCalculation(double dPacketLength,int nPacketType, NETSIM_ID nDevId, NETSIM_ID nInterfaceId, int flag)
{
	double dTxTime = 0.0;
	double dTGF_HT_PREAMBLE;
	double dNSYM, dTempNum = 0.0; 
	int mSTBC = 1, nNDBPS, nNCBPS;
	int nSignalExtension = 0;
	int nNavbits = 1;
	PHY_VAR *pstruPhy = DEVICE_PHYVAR(nDevId,nInterfaceId);

	if(nPacketType == 1) // Other than WLAN control packets
	{
		nNDBPS = pstruPhy->nNDBPS;
		nNCBPS = pstruPhy->nNCBPS;
	}
	else  
	{
		nNDBPS = pstruPhy->nNDBPS/pstruPhy->nNSS;
		nNCBPS = pstruPhy->nNCBPS/pstruPhy->nNSS;
	}
	dTGF_HT_PREAMBLE = pstruPhy->pstruOfdmMimoParameter->nTHT_GF_STF \
		+ pstruPhy->pstruOfdmMimoParameter->nTHT_LTF1 \
		+ ((pstruPhy->nN_HT_LTF - 1) * pstruPhy->pstruOfdmMimoParameter->nTHT_LTFs);

	if(pstruPhy->pstruOfdmMimoParameter->nFEC_CODING == BCC_CODING)
	{
		dTempNum =  (8 * dPacketLength) + 16 + ( 6 * pstruPhy->nNES) ;		
		if(pstruPhy->pstruOfdmMimoParameter->nSTBC) // nSTBC != 0 
			mSTBC = 2;
		if(pstruPhy->nPHYprotocol!=IEEE_802_11n)
		{
				dNSYM = mSTBC * ceil( dTempNum /(mSTBC * nNDBPS));
		}
		else
		{
			if(flag)
			{	
				dNSYM = mSTBC * ( dTempNum /(mSTBC * nNDBPS));
				//This is to calculate time for tranmission of packet with a PHY Header
			}
			else
			{
				//This is to calculate time for transmission of a SUBframe. Note that 
				//here ceil function is not used.
				dTempNum = 8*dPacketLength;
				dNSYM = mSTBC* ( dTempNum/(mSTBC * nNDBPS));
				if(pstruPhy->nGuardInterval == 800)
				{
					dTxTime = pstruPhy->pstruOfdmMimoParameter->dTSYM *dNSYM;
				}  
				else
				{
					dTxTime = pstruPhy->pstruOfdmMimoParameter->dTSYMS * dNSYM ;
				}
				return dTxTime;	

			}
		}
	}
	else
	{
		dNSYM = nNavbits * nNCBPS;
		if(!flag)
		{
			dTxTime = pstruPhy->pstruOfdmMimoParameter->dTSYMS * dNSYM ;
			return dTxTime;

		}
	}

	if(pstruPhy->nGuardInterval == 800)
	{
		dTxTime = dTGF_HT_PREAMBLE + pstruPhy->pstruOfdmMimoParameter->nTHT_SIG \
			+ pstruPhy->pstruOfdmMimoParameter->dTSYM * dNSYM + nSignalExtension;
	} 
	else
	{
		dTxTime = dTGF_HT_PREAMBLE + pstruPhy->pstruOfdmMimoParameter->nTHT_SIG \
			+ pstruPhy->pstruOfdmMimoParameter->dTSYMS * dNSYM + nSignalExtension;
	}
	return dTxTime;	
}
/**
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
This function is used to set the datarate for a frame. Using this we are 
calculating the transmission time.
Datarate set based on the received power.
Select the receiver sensitivity as per below table assigned 
in PMD_RX_Specification_802_11abgn.c
page 1745 Table 20-23—Receiver minimum input level sensitivity  802.11-2012.pdf
Page 319 Table 20-22—Receiver minimum input level sensitivity 802.11n-2009.pdf 
These Datarates are as per table
page 345 Table 20-29 to  Table 20-36 802.11n-2009.pdf 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
int fn_NetSim_WLAN_802_11n_DataRate(NETSIM_ID nDeviceId, NETSIM_ID nInterfaceId, NETSIM_ID nReceiverId)
{
	int nNBPSC; // Number of bits per sub carrier it depend on modulation. 
	// 1 bit for BPSK, 2-QPSK, 4 - 16QAM, 6 - 64QAM
	double fCodingRate;
	float fChannelBandwidthFactor = 1.0;
	float fGuardIntervalFactor = 1.0;
	int nMCSIndex = 0; 
	int nRxSenIndex = 0; // 0 for 20 MHz (to access table from 13 to 20) and 8 for 40 MHz (to access from 21 to 28),
	PHY_VAR *pstruPhy = WLAN_PHY(nDeviceId,nInterfaceId);
	MAC_VAR* pstrumac = WLAN_MAC(nDeviceId,nInterfaceId);
	unsigned int index = pstruPhy->currentPhyindex[nReceiverId];
	
	if(pstruPhy->nChannelBandwidth == 40)
		nRxSenIndex = 8;
	if(nReceiverId)
	{
		if(pstrumac->rate_adaptationflag)
		{

			pstruPhy->dReceiverSensivity = struPhyParameters[index].dRxSensitivity;
			pstruPhy->Modulation = struPhyParameters[index].nModulation;
			pstruPhy->CodingRate = struPhyParameters[index].nCodingRate;
			nNBPSC = struPhyParameters[index].nNBPSC;
			switch(index)
			{
			case 20:
			case 28:
				nMCSIndex = 7;
				break;
			case 19:
			case 27:
				nMCSIndex = 6;
				break;
			case 18:
			case 26:
				nMCSIndex = 5;
				break;
			case 17:
			case 25:
				nMCSIndex = 4;
				break;
			case 16:
			case 24:
				nMCSIndex = 3;
				break;
			case 15:
			case 23:
				nMCSIndex = 2;
				break;
			case 14:
			case 22:
				nMCSIndex = 1;
				break;
			case 13:
			case 21:
				nMCSIndex = 0;
				break;
			default:
				nMCSIndex = 0;
				break;
			}
		}
		else
		{
				if(pstruPhy->dTotalReceivedPower >= struPhyParameters[20+nRxSenIndex].dRxSensitivity)
			{
				pstruPhy->dReceiverSensivity = struPhyParameters[20+nRxSenIndex].dRxSensitivity;
				pstruPhy->Modulation = struPhyParameters[20+nRxSenIndex].nModulation;
				pstruPhy->CodingRate = struPhyParameters[20+nRxSenIndex].nCodingRate;
				nNBPSC = struPhyParameters[20+nRxSenIndex].nNBPSC;
				nMCSIndex = 7;
			}
			else if( pstruPhy->dTotalReceivedPower < struPhyParameters[20+nRxSenIndex].dRxSensitivity && pstruPhy->dTotalReceivedPower >= struPhyParameters[19+nRxSenIndex].dRxSensitivity)
			{
				pstruPhy->dReceiverSensivity = struPhyParameters[19+nRxSenIndex].dRxSensitivity;
				pstruPhy->Modulation = struPhyParameters[19+nRxSenIndex].nModulation;
				pstruPhy->CodingRate = struPhyParameters[19+nRxSenIndex].nCodingRate;
				nNBPSC = struPhyParameters[19+nRxSenIndex].nNBPSC;
				nMCSIndex = 6;
			}
			else if( pstruPhy->dTotalReceivedPower < struPhyParameters[19+nRxSenIndex].dRxSensitivity && pstruPhy->dTotalReceivedPower >= struPhyParameters[18+nRxSenIndex].dRxSensitivity)
			{
				pstruPhy->dReceiverSensivity = struPhyParameters[18+nRxSenIndex].dRxSensitivity;
				pstruPhy->Modulation = struPhyParameters[18+nRxSenIndex].nModulation;
				pstruPhy->CodingRate = struPhyParameters[18+nRxSenIndex].nCodingRate;
				nNBPSC = struPhyParameters[18+nRxSenIndex].nNBPSC;
				nMCSIndex = 5;
			}
			else if( pstruPhy->dTotalReceivedPower < struPhyParameters[18+nRxSenIndex].dRxSensitivity && pstruPhy->dTotalReceivedPower >= struPhyParameters[17+nRxSenIndex].dRxSensitivity)
			{
				pstruPhy->dReceiverSensivity = struPhyParameters[17+nRxSenIndex].dRxSensitivity;
				pstruPhy->Modulation = struPhyParameters[17+nRxSenIndex].nModulation;
				pstruPhy->CodingRate = struPhyParameters[17+nRxSenIndex].nCodingRate;
				nNBPSC = struPhyParameters[17+nRxSenIndex].nNBPSC;
				nMCSIndex = 4;
			}
			else if(pstruPhy->dTotalReceivedPower < struPhyParameters[17+nRxSenIndex].dRxSensitivity && pstruPhy->dTotalReceivedPower >= struPhyParameters[16+nRxSenIndex].dRxSensitivity)
			{
				pstruPhy->dReceiverSensivity = struPhyParameters[16+nRxSenIndex].dRxSensitivity;
				pstruPhy->Modulation = struPhyParameters[16+nRxSenIndex].nModulation;
				pstruPhy->CodingRate = struPhyParameters[16+nRxSenIndex].nCodingRate;
				nNBPSC = struPhyParameters[16+nRxSenIndex].nNBPSC;
				nMCSIndex = 3;
			}
			else if(pstruPhy->dTotalReceivedPower < struPhyParameters[16+nRxSenIndex].dRxSensitivity && pstruPhy->dTotalReceivedPower >= struPhyParameters[15+nRxSenIndex].dRxSensitivity)
			{
				pstruPhy->dReceiverSensivity = struPhyParameters[15+nRxSenIndex].dRxSensitivity;
				pstruPhy->Modulation = struPhyParameters[15+nRxSenIndex].nModulation;
				pstruPhy->CodingRate = struPhyParameters[15+nRxSenIndex].nCodingRate;
				nNBPSC = struPhyParameters[15+nRxSenIndex].nNBPSC;
				nMCSIndex = 2;
			}
			else if(pstruPhy->dTotalReceivedPower < struPhyParameters[15+nRxSenIndex].dRxSensitivity && pstruPhy->dTotalReceivedPower >= struPhyParameters[14+nRxSenIndex].dRxSensitivity)
			{
				pstruPhy->dReceiverSensivity = struPhyParameters[14+nRxSenIndex].dRxSensitivity;
				pstruPhy->Modulation = struPhyParameters[14+nRxSenIndex].nModulation;
				pstruPhy->CodingRate = struPhyParameters[14+nRxSenIndex].nCodingRate;
				nNBPSC = struPhyParameters[14+nRxSenIndex].nNBPSC;
				nMCSIndex = 1;
			}
			else if(pstruPhy->dTotalReceivedPower < struPhyParameters[14+nRxSenIndex].dRxSensitivity && pstruPhy->dTotalReceivedPower >= struPhyParameters[13+nRxSenIndex].dRxSensitivity)
			{
				pstruPhy->dReceiverSensivity = struPhyParameters[13+nRxSenIndex].dRxSensitivity;
				pstruPhy->Modulation = struPhyParameters[13+nRxSenIndex].nModulation;
				pstruPhy->CodingRate = struPhyParameters[13+nRxSenIndex].nCodingRate;
				nNBPSC = struPhyParameters[13+nRxSenIndex].nNBPSC;
				nMCSIndex = 0;
			}		
			else if(pstruPhy->dTotalReceivedPower < struPhyParameters[13+nRxSenIndex].dRxSensitivity)
			{
				// Device out of coverage area
				pstruPhy->dReceiverSensivity = -999;
				/*nNBPSC = 0;*/
				pstruPhy->dReceiverSensivity = struPhyParameters[13+nRxSenIndex].dRxSensitivity;
				pstruPhy->Modulation = struPhyParameters[13+nRxSenIndex].nModulation;
				pstruPhy->CodingRate =struPhyParameters[13+nRxSenIndex].nCodingRate;
				nNBPSC = struPhyParameters[13+nRxSenIndex].nNBPSC;
				nMCSIndex = 0;
			}
		}
	}
	else // Broadcast packets
	{
		pstruPhy->dReceiverSensivity = struPhyParameters[13+nRxSenIndex].dRxSensitivity;
		pstruPhy->Modulation = struPhyParameters[13+nRxSenIndex].nModulation;
		pstruPhy->CodingRate = struPhyParameters[13+nRxSenIndex].nCodingRate;
		nNBPSC = struPhyParameters[13+nRxSenIndex].nNBPSC;
		nMCSIndex = 0;
	}
	pstruPhy->dMinReceiverSensitivity = struPhyParameters[13+nRxSenIndex].dRxSensitivity; // Check out of range packets

	// Assign Coding rate value
	switch(pstruPhy->CodingRate)
	{
	case Coding_1_2:
		fCodingRate = 1.0/2.0;
		break;
	case Coding_2_3:
		fCodingRate = 2.0/3.0;
		break;
	case Coding_3_4:
		fCodingRate = 3.0/4.0;
		break;
	case Coding_5_6:
		fCodingRate = 5.0/6.0;
		break;
	default:
		fCodingRate = 1.0/2.0;
		break;
	}
	pstruPhy->dR = fCodingRate;
	// Calculate MCS parameter details for Nss = 1, This is the basic rate
	pstruPhy->nNBPSC = nNBPSC;
	pstruPhy->nNCBPS = pstruPhy->nNBPSC * pstruPhy->pstruOfdmMimoParameter->nNSD; 
	pstruPhy->nNDBPS =  (unsigned short)ceil((pstruPhy->nNCBPS * fCodingRate));
	if(pstruPhy->nGuardInterval == 800)
	{
		pstruPhy->dDataRate = pstruPhy->nNDBPS / pstruPhy->pstruOfdmMimoParameter->dTSYM;
		pstruPhy->dControlFrameDataRate = (pstruPhy->pstruOfdmMimoParameter->nNSD / 2) / pstruPhy->pstruOfdmMimoParameter->dTSYM;
		pstruPhy->dBroadCastFrameDataRate = pstruPhy->dControlFrameDataRate;
	}
	else
	{
		pstruPhy->dDataRate = pstruPhy->nNDBPS / pstruPhy->pstruOfdmMimoParameter->dTSYMS;
		pstruPhy->dControlFrameDataRate = (pstruPhy->pstruOfdmMimoParameter->nNSD / 2) / pstruPhy->pstruOfdmMimoParameter->dTSYMS;
		pstruPhy->dBroadCastFrameDataRate = pstruPhy->dControlFrameDataRate;
	}
	if(pstruPhy->nNSS > 1) // Multiply by Nss
	{
		pstruPhy->dDataRate *= pstruPhy->nNSS;	
		pstruPhy->nNCBPS *= pstruPhy->nNSS;	
		pstruPhy->nNDBPS *= pstruPhy->nNSS;	
		pstruPhy->dControlFrameDataRate *= pstruPhy->nNSS;
		pstruPhy->dBroadCastFrameDataRate = pstruPhy->dControlFrameDataRate;
	}

	//pstruPhy->dDataRate *=0;
	// Set MSC index 
	switch(pstruPhy->nNSS)
	{
	default:
	case 1: // Datarate between MCS 0 to 7
		pstruPhy->nMCS = nMCSIndex;
		break;
	case 2:// MCS 8 - 15
		pstruPhy->nMCS = nMCSIndex + 8;
		break;
	case 3:// MCS 16 - 24
		pstruPhy->nMCS = nMCSIndex + 16;
		break;
	case 4: // MCS 25 - 31
		pstruPhy->nMCS = nMCSIndex + 24;
		break;
	}
	return 0;
}

/**
This function is used to calculate the BER for IEEE 802.11n
*/
int fn_NetSim_WLAN_802_11n_BER(NETSIM_ID nDeviceId,NETSIM_ID nInterfaceId,double dReceivedPower,NetSim_PACKET* pstruPacket)
{
	double dBer = 0.0;
	int nCount = 1;
	double k = 0.0;
	int nModulation;	
	double ldNpower = -101.0;//dbm //noise power for 20MHz channel.Refer help for more details.
	double ldLoss = -1.70;//db
	double ldEbByN0 = 0.0;//db
	double pb = 0.0;
	double fpi=3.1417f;
	double fp=0.0;	
	double fBer1=0.0;
	int nMCS, nBandWidth,nGuardTime;
	HT_PPDU *pstruHtPPDU; 
	PHY_VAR *pstruPhy = DEVICE_PHYVAR(nDeviceId,nInterfaceId);

	ldEbByN0 = dReceivedPower + ldLoss - ldNpower-30;//bit energy calculation.30dB is subtracted from the energy to convert the value from dBm to dB

	if(pstruPhy->ChannelModel == NO_PATHLOSS)
	{
		pstruPhy->dBER = 0; // Assign BER value 				
		return 0;
	}
	if(!pstruPacket || !pstruPacket->pstruPhyData->Packet_PhyData)
	{
		nMCS = pstruPhy->nMCS;	
		nBandWidth = pstruPhy->nChannelBandwidth;
		nGuardTime = pstruPhy->nGuardInterval;
	}
	else
	{
		pstruHtPPDU = pstruPacket->pstruPhyData->Packet_PhyData;		
		nMCS = pstruHtPPDU->nMCS;
		if(pstruHtPPDU->bCBW_20_40)
		{
			nModulation = 40; 
			ldEbByN0 = ldEbByN0-3;//with change in bandwidth of the channel from 20MHz to 40MHz the noise power increases by a value of 3dB. Refer help for more details.
		}
		else
			nModulation = 20;
		if(pstruHtPPDU->bShort_GI)
			nGuardTime = 400;
		else
			nGuardTime = 800;
	}

	switch(nMCS % 8)
	{		
		//BPSK Modulation
	case 0:// MCS = 0 , 8 , 16 or 24
	case 1: // MCS = 1, 9, 17 , 25
		{
			extern BER bpsk_ber[];
			extern size_t bpsk_ber_len;
			pb = calculate_ber(ldEbByN0,bpsk_ber,bpsk_ber_len);
		}
		break;	

		//QPSK Modulation	
	case 2: // MCS = 2, 10, 18, 26
	case 3:  // MCS = 3, 11, 19, 27
		{
			extern BER qpsk_ber[];
			extern size_t qpsk_ber_len;
			pb = calculate_ber(ldEbByN0,qpsk_ber,qpsk_ber_len);
		}
		break;
		//16-QAM Modulation
	case 4:	// MCS = 4, 12, 20, 28
	case 5:	// MCS = 5, 13, 21, 29
		
		//64-QAM Modulation
	case 6:		//MCS 6, 14, 22, 30
	case 7:		// MCS = 7, 15, 23, 31
		{
			extern BER qam64_ber[];
			extern size_t qam64_ber_len;
			pb = calculate_ber(ldEbByN0,qam64_ber,qam64_ber_len);
		}
		break;
	default:
		fnNetSimError("WLAN--Unknown Modulation in OFDM PHY in Calculate BER function");
		pb = 1;				
		break;		
	}				
	pstruPhy->dBER = pb; // Assign BER value
	return 0;
}
