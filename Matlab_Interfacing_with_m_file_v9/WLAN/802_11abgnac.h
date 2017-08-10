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

// Control frame data rate RTS and CTS
#define CONTRL_FRAME_RATE_11B 1			///< Control frame data rate for IEEE 802.11b in Mbps
#define CONTRL_FRAME_RATE_11A_AND_G 6	///< Control frame data rate for IEEE 802.11b in Mbps


typedef struct stru_802_11_PHY_PLME_characteristics PLME_CHARACTERISTICS;
typedef struct stru_802_11_DSSS_PLCP_Frame DSSS_PPDU;
typedef struct stru_802_11_OFDM_PLCP_Frame OFDM_PPDU;

typedef enum enum_802_11_Modulation MODULATION;
typedef enum enum_802_11_CodingRate CODING_RATE;

// typedefs for 802.11n 
typedef enum enum_802_11n_FORMAT FORMAT;
typedef enum enum_802_11n_CH_BANDWIDTH CH_BANDWIDTH;
typedef enum enum_802_11n_SMOOTHING SMOOTHING_11N;
typedef enum enum_802_11n_SOUNDING SOUNDING_11N;
typedef enum enum_802_11n_AGGREGATION AGGREGATION_11N;
typedef enum enum_802_11n_FEC_CODING FEC_CODING_11N;
typedef enum enum_802_11n_GI_TYPE GI_TYPE;
typedef enum enum_802_11n_CHAN_MAT_TYPE CHAN_MAT_TYPE;
//typedefs for 802.11n
typedef struct stru_802_11n_HT_PLCP_Frame HT_PPDU;
typedef struct stru_802_11_QoS_Control QOS_CONTROL;
typedef struct stru_802_11_HT_Control HT_CONTROL;
typedef struct stru_802_11n_OFDM_MIMO_Parameters OFDM_MIMO_PARAMETERS;

// typedefs for 802.11ac 
typedef enum enum_802_11ac_FORMAT FORMATac;
typedef enum enum_802_11ac_CH_BANDWIDTH CH_BANDWIDTHac;
typedef enum enum_802_11ac_SMOOTHING SMOOTHING_11ac;
typedef enum enum_802_11ac_SOUNDING SOUNDING_11ac;
typedef enum enum_802_11ac_AGGREGATION AGGREGATION_11ac;
typedef enum enum_802_11ac_FEC_CODING FEC_CODING_11ac;
typedef enum enum_802_11ac_GI_TYPE GI_TYPEac;
typedef enum enum_802_11ac_CHAN_MAT_TYPE CHAN_MAT_TYPEac;
//typedefs for 802.11ac
typedef struct stru_802_11ac_HT_PLCP_Frame VHT_PPDU;
//typedef struct stru_802_11_QoS_Control QOS_CONTROL;
typedef struct stru_802_11_HT_Control VHT_CONTROL;
typedef struct stru_802_11ac_OFDM_MIMO_Parameters ac_OFDM_MIMO_PARAMETERS;

/// PPDU format and Modulation-dependent parameters
enum enum_802_11_Modulation
{
	Modulation_DBPSK,
	Modulation_DQPSK,
	Modulation_CCK,
	Modulation_BPSK,
	Modulation_QPSK,
	Modulation_16_QAM,
	Modulation_64_QAM,
	Modulation_128_QAM,
	Modulation_256_QAM,
};
/// Modulation-dependent parameters
enum enum_802_11_CodingRate
{
	Coding_1_2,		
	Coding_2_3,
	Coding_3_4,
	Coding_5_6,
};
/** 
Enumarations for 802.11n 
Table 20-1—TXVECTOR and RXVECTOR parameters page 249- 802.11n-2009.pdf 
*/
enum enum_802_11n_FORMAT
{
	NON_HT, ///< For legacy non 802.11n
	HT_MF,	///< indicates HT-mixed format. legacy + 802.11n
	HT_GF,	///< indicates HT-greenfield format. Only 802.11n supported devices.
			///< We are considered this method only.
};
enum enum_802_11ac_FORMAT
{
	NON_VHT, ///< For legacy non 802.11ac
	VHT_MF,	///< indicates HT-mixed format. legacy + 802.11ac
	VHT_GF,	///< indicates HT-greenfield format. Only 802.11ac supported devices.
			// We are considered this method only.
};
enum enum_802_11n_CH_BANDWIDTH
{
	HT_CBW20,	   ///< 20 MHz and 40 MHz upper and 40 MHz lower modes
	HT_CBW40,	   ///< 40 MHz
	NON_HT_CBW40,  ///< for non-HT duplicate format
	NON_HT_CBW20,  ///< for all other non-HT formats
};
enum enum_802_11ac_CH_BANDWIDTH
{
	VHT_CBW20,	   ///< 20 MHz
	VHT_CBW40,	   ///< 40 MHz
	VHT_CBW80,	   ///< 80 MHz
	VHT_CBW160,	   ///< 160 MHz
	NON_VHT_CBW160,  ///< for non-HT duplicate format
	NON_VHT_CBW80,  ///< for all other non-HT formats
	NON_VHT_CBW40,   
	NON_VHT_CBW20,   
};
//Frequency-domain smoothing is recommended as part of channel estimation.
enum enum_802_11n_SMOOTHING 
{ 
	SMOOTHING_NOT_REC,	///< smoothing is not recommended.
	SMOOTHING_REC,		///< smoothing is recommended.
};
/// Frequency-domain smoothing is recommended as part of channel estimation.
enum enum_802_11ac_SMOOTHING 
{ 
	SMOOTHING_NOT_RECac,	///< smoothing is not recommended.
	SMOOTHING_RECac,		///< smoothing is recommended.
};
/// Indicates whether this packet is a sounding packet.
enum enum_802_11n_SOUNDING 
{	
	SOUNDING,		///< sounding packet.
	NOT_SOUNDING	///<  not a sounding packet
};
/// Indicates whether this packet is a sounding packet.
enum enum_802_11ac_SOUNDING 
{	
	SOUNDINGac,		///< sounding packet.
	NOT_SOUNDINGac	///<  not a sounding packet
};
/// Indicates whether the PSDU contains an A-MPDU.
enum enum_802_11n_AGGREGATION 
{
	NOT_AGGREGATED, ///< Packet does not have A-MPDU aggregation
	AGGREGATED,		///< Packet has A-MPDU aggregation	
};
/// Indicates whether the PSDU contains an A-MPDU.
enum enum_802_11ac_AGGREGATION 
{
	NOT_AGGREGATEDac, ///< Packet does not have A-MPDU aggregation
	AGGREGATEDac,		///< Packet has A-MPDU aggregation	
};
/// Indicates which FEC encoding is used.
enum enum_802_11n_FEC_CODING 
{
	BCC_CODING,		///< Binary convolutional code -BCC
	LDPC_CODING,	///< low-density parity check code - LDPC
};
/// Indicates which FEC encoding is used.
enum enum_802_11ac_FEC_CODING 
{
	BCC_CODINGac,		///< Binary convolutional code -BCC
	LDPC_CODINGac,	///< low-density parity check code - LDPC
};
/// Guard interval is used in the transmission of the packet.
enum enum_802_11n_GI_TYPE 
{	
	LONG_GI,	///< Long GI is not used in the packet 800ns
	SHORT_GI,	///< Short GI is used in the packet 400ns
};
/// Guard interval is used in the transmission of the packet.
enum enum_802_11ac_GI_TYPE 
{	
	LONG_GIac,	///< Long GI is not used in the packet 800ns
	SHORT_GIac,	///< Short GI is used in the packet 400ns
};
enum enum_802_11n_CHAN_MAT_TYPE
{
	COMPRESSED_SV,		///< CHAN_MAT is a set of compressed beamforming vector matrices.
	NON_COMPRESSED_SV,	///< CHAN_MAT is a set of noncompressed beamforming vector matrices.
	CSI_MATRICES,		///< CHAN_MAT is a set of channel state matrices.
};
enum enum_802_11ac_CHAN_MAT_TYPE
{
	COMPRESSED_SVac,		///< CHAN_MAT is a set of compressed beamforming vector matrices.
	NON_COMPRESSED_SVac,	///< CHAN_MAT is a set of noncompressed beamforming vector matrices.
	CSI_MATRICESac,		///< CHAN_MAT is a set of channel state matrices.
};

/// Page-1505 IEEE Std 802.11-2012 Figure 16-1—PLCP frame format
struct stru_802_11_DSSS_PLCP_Frame 
{
	// PLCP preamble 144 bits
	long long int nSYNC;			///<128 bits 
	unsigned short int nSFD;		///<16 bits = 1111 0011 1010 0000	
	// PLCP Header 48 bits
	unsigned short int nSIGNAL;		///<8 bits
	unsigned short int nSERVICE;	///<8 bits
	unsigned short int nLENGTH;		///<16 bits
	unsigned short int nCRC;		///<16 bits	
	// For Netsim simulation added below details
	double dTotalReceivedPower;
	double dRxSensitivity;
};

/// Page-1588 IEEE Std 802.11-2012 Figure 18-1—PPDU frame format
struct stru_802_11_OFDM_PLCP_Frame			
{
	// OFDM_PLCP_Preamble;				///< 12 Symbols
	// PLCP Header 
	int nRATE:4;						///< 4 bits
	bool bReserved;						///< 1 bit
	int bLENGTH:12;						///< 12 bits
	bool bParity;						///< 1 bit
	int nTail_1:6;						///< 6 bits
	int nSERVICE:16;					///< 16 bits													
	int nTail_2:6;						///< 6 bits
	int nPadBits;
	// For Netsim simulation added below details
	double dTotalReceivedPower;
	double dRxSensitivity;
};
/// Table 20-5—Timing-related constants and Table 20-6—Frequently used parameters
struct stru_802_11n_OFDM_MIMO_Parameters
{
	int nChannelBandwidth;				// 20MHz or 40MHz
	int nFFT;			///< FFT length
	int	nNSD;			///< NSD Number of complex data numbers
	int nNSP;			///< NSP Number of pilot values
	int nNST;			///< NST Total number of subcarriers
	int nNSR;			///< NSR Highest data subcarrier index
	double dDeltaF;		///<dSubcarrierFrequencySpacing; // 312.5KHz (20 MHz/64) or (40 MHz/128)
	double dTDFT;		///< IDFT/DFT period 3.2  
	double dTGI;		///< Guard interval duration 0.8 = TDFT/4   
    double dTGI2;		///< Double guard interval 1.6 
	double dTGIS;		///< Short guard interval duration   dDFT/8 0.4 
	int nTL_STF;		///< Non-HT short training sequence duration 8 = 10*TDFT/4 
	int nTHT_GF_STF;	///< HT-greenfield short training field duration 8 =10* TDFT/4 
	int nTL_LTF;		///< Non-HT long training field duration 8 =2 * TDFT + TGI2  
	double dTSYM;		///< TSYM: Symbol interval 4 =  TDFT + TGI  
	double dTSYMS;		///< dShortGIsymbolInterval;      // TSYMS: Short GI symbol interval N/A 3.6  = TDFT+TGIS 3.6 
	int nTL_SIG;		///< Non-HT SIGNAL field duration 4   
	int nTHT_SIG;		///< HT SIGNAL field duration 8 = 2TSYM 8 
	int nTHT_STF;		///< HT short training field duration  4 
	int nTHT_LTF1;		///< First HT long training field duration 4 in HT-mixed format, 8 in HTgreenfield format
	int nTHT_LTFs;		///< Second, and subsequent, HT long training fields duration 4 	

	// Table 20-1—TXVECTOR and RXVECTOR parameters page 249- 802.11n-2009.pdf
	FORMAT nFormat;
	int nNonHtModulation;
	int nL_LENGTH;
	int nL_DATARATE;
	int nLSIGVALID;
	int nSERVICE;
	int nTXPWR_LEVEL;
	int nRSSI;
	int nPREAMBLE_TYPE;
	int nMCS;
	int nREC_MCS;
	CH_BANDWIDTH nCH_BANDWIDTH;
	int nCH_OFFSET;
	int nLENGTH;
	SMOOTHING_11N nSMOOTHING;
	SOUNDING_11N nSOUNDING;
	AGGREGATION_11N nAGGREGATION;
	int nSTBC;
	FEC_CODING_11N nFEC_CODING;
	GI_TYPE nGI_TYPE;
	int NUM_EXTEN_SS;
	int ANTENNA_SET;
	int nN_TX;		
	int nEXPANSION_MAT;
	int nEXPANSION_MAT_TYPE;
	int nCHAN_MAT;
	CHAN_MAT_TYPE nCHAN_MAT_TYPE;
	int	nRCPI;
	int nSNR;
	int nNO_SIG_EXTN;	
};
struct stru_802_11ac_OFDM_MIMO_Parameters
{
	int nChannelBandwidth;				// 20MHz or 40MHz or 80MHz or 160MHz
	int nFFT;			///< FFT length
	int	nNSD;			///< NSD Number of complex data numbers
	int nNSP;			///< NSP Number of pilot values
	int nNST;			///< NST Total number of subcarriers
	int nNSR;			///< NSR Highest data subcarrier index
	double dDeltaF;		///< dSubcarrierFrequencySpacing; // 312.5KHz (20 MHz/64) or (40 MHz/128)
	double dTDFT;		///< IDFT/DFT period 3.2  
	double dTGI;		///< Guard interval duration 0.8 = TDFT/4   
    double dTGI2;		///< Double guard interval 1.6 
	double dTGIS;		///< Short guard interval duration   dDFT/8 0.4 
	int nTL_STF;		///< Non-HT short training sequence duration 8 = 10*TDFT/4 
	int nTVHT_GF_STF;	///< HT-greenfield short training field duration 8 =10* TDFT/4 
	int nTL_LTF;		///< Non-HT long training field duration 8 =2 * TDFT + TGI2  
	double dTSYM;		///< TSYM: Symbol interval 4 =  TDFT + TGI  
	double dTSYMS;		///< dShortGIsymbolInterval;      // TSYMS: Short GI symbol interval N/A 3.6  = TDFT+TGIS 3.6 
	int nTL_SIG;		///< Non-HT SIGNAL field duration 4   
	int nTVHT_SIGA;		///< HT SIGNAL field duration = 2TSYM =  8 
	int nTVHT_SIGB;		///< HT SIGNAL field duration = TSYM = 4
	int nTVHT_STF;		///< HT short training field duration  4 
	int nTVHT_LTF;		///< HT long training field duration 8
	

	// Table 20-1—TXVECTOR and RXVECTOR parameters page 249- 802.11n-2009.pdf
	FORMAT nFormat;
	int nNonHtModulation;
	int nL_LENGTH;
	int nL_DATARATE;
	int nLSIGVALID;
	int nSERVICE;
	int nTXPWR_LEVEL;
	int nRSSI;
	int nPREAMBLE_TYPE;
	int nMCS;
	int nREC_MCS;
	CH_BANDWIDTH nCH_BANDWIDTH;
	int nCH_OFFSET;
	int nLENGTH;
	SMOOTHING_11N nSMOOTHING;
	SOUNDING_11N nSOUNDING;
	AGGREGATION_11N nAGGREGATION;
	int nSTBC;
	FEC_CODING_11N nFEC_CODING;
	GI_TYPE nGI_TYPE;
	int NUM_EXTEN_SS;
	int ANTENNA_SET;
	int nN_TX;		
	int nEXPANSION_MAT;
	int nEXPANSION_MAT_TYPE;
	int nCHAN_MAT;
	CHAN_MAT_TYPE nCHAN_MAT_TYPE;
	int	nRCPI;
	int nSNR;
	int nNO_SIG_EXTN;	
};
/// Table 20-4—Elements of the HT PLCP packet  and Figure 20-1—PPDU format
struct stru_802_11n_HT_PLCP_Frame 
{
	int L_STF;			///< 8 Microsecs Non-HT Short Training field to support Legacy g and a
	int L_LTF;			///< 8 Microsecs Non-HT Long Training field to support Legacy g and a
	int L_SIG;			///< 4 Microsecs Non-HT SIGNAL field to support Legacy g and a
	int HT_STF;			///< 4 Microsecs HT Short Training field Mixed mode 
	int HT_GF_STF;		///< 8 Microsecs HT-Greenfield Short Training field	
	int HT_LTF1;		///< 8 Microsecs First HT Long Training field (Data)
	//Table 20-10—HT-SIG fields page 275 802.11n-2009.pdf	
	int nMCS:8;					///< 7 bits Modulation and Coding Scheme 7 Index into the MCS table.
	int bCBW_20_40:1;			///< 1 bit Set to 0 for 20 MHz or 40 MHz upper/lower.
								///< Set to 1 for 40 MHz.
	unsigned short int nHT_Length;		///< 16 bits  The number of octets of data in the PSDU in the range of 0 to 65 535.
	int bSmoothing:1;	///< 1 bit  Set to 1 indicates that channel estimate smoothing is recommended.
						///< Set to 0 indicates that only per-carrier independent (unsmoothed) channel estimate is recommended. See 20.3.11.10.1.
	int bNot_Sounding:1;  /**< 1 bit  Set to 0 indicates that PPDU is a sounding PPDU.
						      Set to 1 indicates that the PPDU is not a sounding PPDU. */
	int bReserved:1;		///< 1 bit Set to 1.
	int bAggregation:1;	/**< 1 bit Set to 1 to indicate that the PPDU in the data portion of the packet contains an AMPDU;
						     otherwise, set to 0. */
	int nSTBC:2;		/**< 2 bits Set to a nonzero number, to indicate the difference between the number of spacetime
						    streams ( ) and the number of spatial streams ( ) indicated by the MCS. 
						    Set to 00 to indicate no STBC ( ). */
	int bFEC_coding:1;    ///< 1 bit  Set to 1 for LDPC. Set to 0 for BCC.
	int bShort_GI:1;	  /**< 1 bit  Set to 1 to indicate that the short GI is used after the HT training.
						       Set to 0 otherwise. */
	int Number_Ess:2;		/**< 2 bits  Indicates the number of extension spatial streams ( ).
							    Set to 0 for no extension spatial stream.
							    Set to 1 for 1 extension spatial stream.
						        Set to 2 for 2 extension spatial streams.
						        Set to 3 for 3 extension spatial streams. */
	int CRC:8;			/**< 8 bits  CRC of bits 0–23 in HT-SIG1 and bits 0–9 in HT-SIG2. See 20.3.9.4.4. The first
						     bit to be transmitted is bit C7 as explained in 20.3.9.4.4. */
	int Tail_Bits:7;	///< 6 Used to terminate the trellis of the convolution coder. Set to 0.
	// End of HT SIG

	int HT_LTFs;		///< Additional HT Long Training fields (Data and Extension)	
};

struct stru_802_11ac_HT_PLCP_Frame 
{
	int L_STF;			///< 8 Microsecs Non-HT Short Training field to support Legacy g and a
	int L_LTF;			///< 8 Microsecs Non-HT Long Training field to support Legacy g and a
	int L_SIG;			///< 4 Microsecs Non-HT SIGNAL field to support Legacy g and a
	int VHT_STF;			///< 4 Microsecs HT Short Training field Mixed mode 
	int VHT_GF_STF;		///< 8 Microsecs HT-Greenfield Short Training field	
	int VHT_LTF1;		///< 8 Microsecs First HT Long Training field (Data)
	//Table 20-10—HT-SIG fields page 275 802.11n-2009.pdf	
	int nMCS:8;					///< 7 bits Modulation and Coding Scheme 7 Index into the MCS table.
	int bCBW_20_40:1;			/**< 1 bit Set to 0 for 20 MHz or 40 MHz upper/lower.
								     Set to 1 for 40 MHz.*/
	unsigned short int nHT_Length;		///< 16 bits  The number of octets of data in the PSDU in the range of 0 to 65 535.
	int bSmoothing:1;	/**< 1 bit  Set to 1 indicates that channel estimate smoothing is recommended.
						     Set to 0 indicates that only per-carrier independent (unsmoothed) channel estimate is recommended. See 20.3.11.10.1.*/
	int bNot_Sounding:1;  /**< 1 bit  Set to 0 indicates that PPDU is a sounding PPDU.
						      Set to 1 indicates that the PPDU is not a sounding PPDU.*/
	int bReserved:1;		///< 1 bit Set to 1.
	int bAggregation:1;	/**< 1 bit Set to 1 to indicate that the PPDU in the data portion of the packet contains an AMPDU;
						     otherwise, set to 0. */
	int nSTBC:2;		/**< 2 bits Set to a nonzero number, to indicate the difference between the number of spacetime
						     streams ( ) and the number of spatial streams ( ) indicated by the MCS.
						     Set to 00 to indicate no STBC ( ). */
	int bFEC_coding:1;    ///< 1 bit  Set to 1 for LDPC. Set to 0 for BCC.
	int bShort_GI:1;		/**< 1 bit  Set to 1 to indicate that the short GI is used after the HT training.
							    Set to 0 otherwise. */
	int Number_Ess:2;	/**< 2 bits  Indicates the number of extension spatial streams ( ).
						     Set to 0 for no extension spatial stream.
						     Set to 1 for 1 extension spatial stream.
						     Set to 2 for 2 extension spatial streams.
						     Set to 3 for 3 extension spatial streams. */
	int CRC:8;			/**< 8 bits  CRC of bits 0–23 in HT-SIG1 and bits 0–9 in HT-SIG2. See 20.3.9.4.4. The first
						     bit to be transmitted is bit C7 as explained in 20.3.9.4.4. */
	int Tail_Bits:7;	///< 6 Used to terminate the trellis of the convolution coder. Set to 0.
	// End of HT SIG

	int VHT_LTFs;		///< Additional HT Long Training fields (Data and Extension)	
};
/// Data structure for physical layer parameters
struct stru_802_11_Phy_Parameters
{
	int nIndex;				
	double dRxSensitivity;
	MODULATION nModulation;
	double dDataRate;
	CODING_RATE nCodingRate;
	int nNBPSC;				
	int nNCBPS;							
	int nNDBPS;						
};
struct stru_802_11_Phy_Parameters struPhyParameters[69];

// 802.11n
int fn_NetSim_WLAN_802_11n_OFDM_MIMO_ParameterIntialization(NETSIM_ID nDeviceId,NETSIM_ID nInterfaceId);
double fn_NetSim_WLAN_802_11n_TxTimeCalculation(double dPacketLength,int nPacketType, NETSIM_ID nDevId, NETSIM_ID nInterfaceId, int flag);
int fn_NetSim_WLAN_802_11n_DataRate(NETSIM_ID nDeviceId, NETSIM_ID nInterfaceId, NETSIM_ID nReceiverId);
int fn_NetSim_WLAN_802_11n_BER(NETSIM_ID nDeviceId,NETSIM_ID nInterfaceId,double dReceivedPower,NetSim_PACKET* pstruPacket);
// 802.11ac
int fn_NetSim_WLAN_802_11ac_OFDM_MIMO_ParameterIntialization(NETSIM_ID nDeviceId,NETSIM_ID nInterfaceId);
double fn_NetSim_WLAN_802_11ac_TxTimeCalculation(double dPacketLength,int nPacketType, NETSIM_ID nDevId, NETSIM_ID nInterfaceId, int flag);
int fn_NetSim_WLAN_802_11ac_DataRate(NETSIM_ID nDeviceId, NETSIM_ID nInterfaceId, NETSIM_ID nReceiverId);
int fn_NetSim_WLAN_802_11ac_BER(NETSIM_ID nDeviceId,NETSIM_ID nInterfaceId,double dReceivedPower,NetSim_PACKET *pstruPacket);

// 802.11a/b/g
int fn_NetSim_WLAN_802_11b_DataRate(NETSIM_ID nDeviceId, NETSIM_ID nInterfaceId, NETSIM_ID nReceiverId);
int fn_NetSim_WLAN_802_11a_11g_DataRate(NETSIM_ID nDeviceId, NETSIM_ID nInterfaceId, NETSIM_ID nReceiverId);
int fn_NetSim_WLAN_802_11b_BER(NETSIM_ID nDeviceId,NETSIM_ID nInterfaceId,double dReceivedPower,NetSim_PACKET *pstruPacket);
int fn_NetSim_WLAN_802_11a_11g_BER(NETSIM_ID nDeviceId,NETSIM_ID nInterfaceId,double dReceivedPower,NetSim_PACKET *pstruPacket);


double fn_WLAN_Q_5(double x);
double fu_NetSim_WLAN_PHY_BER_CCK11(int k, int M, float fTotal_pr);
double fu_NetSim_WLAN_PHY_BER_CCK55(int k, int M, float fTotal_pr);
double fn_NetSim_WLAN_PHY_BER_DQPSK(float fRxPower);
double fu_NetSim_WLAN_PHY_BER_BDPSK(float fRxPower);

double fn_WLAN_PHY_ncr (int i, int n);
double fn_WLAN_PHY_erf(double x);
double fn_WLAN_pp(double x,double p);
double gamma(double x);
#define WLAN_PHY_DATA_SIZE (max((max(sizeof(DSSS_PPDU),sizeof(OFDM_PPDU))),sizeof(HT_PPDU)))
