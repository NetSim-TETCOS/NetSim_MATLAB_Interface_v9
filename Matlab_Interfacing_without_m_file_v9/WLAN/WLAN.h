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
#include"802_11e.h"
#include"802_11abgnac.h"

double fn_matlab_init();
double fn_netsim_matlab();
double fn_netsim_matlab_Finish();

#ifndef _NETSIM_802_11_H_
#define _NETSIM_802_11_H_
#ifdef  __cplusplus
extern "C" {
#endif

#include "ErrorModel.h"

#pragma comment(lib,"NetworkStack.lib")
#pragma comment(lib,"WLAN.lib")
#pragma comment(lib,"Metrics.lib")
#pragma comment(lib,"Mobility.lib")

#define WLAN_RATE_ADAPTATION_DEFAULT true

//2 bits binary number
#define B2_00	0
#define B2_01	1
#define B2_10	2
#define B2_11	3
//3 bits binary number
#define B3_000	0
#define B3_001	1
#define B3_010	2
#define B3_011	3
#define B3_100	4
#define B3_101	5
#define B3_110	6
#define B3_111	7
//4 bits binary number
#define B4_0000	0
#define B4_0001	1
#define B4_0010	2
#define B4_0011	3
#define B4_0100	4
#define B4_0101	5
#define B4_0110	6
#define B4_0111	7
#define B4_1000	8
#define B4_1001	9
#define B4_1010	10
#define B4_1011	11
#define B4_1100	12
#define B4_1101	13
#define B4_1110	14
#define B4_1111	15
// MAC frames
#define MAC_OVERHEAD_802_11n 44
#define MAC_OVERHEAD_802_11ac MAC_OVERHEAD_802_11n
#define MAC_OVERHEAD 40		///< 36 bytes header 4 bytes FCS
#define RTS_SIZE 20			///< 16 bytes header 4 bytes FCS
#define CTS_SIZE 14			///< 10 bytes header 4 bytes FCS
#define ACK_SIZE 14			///< 10 bytes header 4 bytes FCS
#define BlockACK_SIZE_802_11n 32    /**<16 bytes header,2 bytes BA Control, 2 bytes BA starting Sequence Control 
									8bytes BITMAP, 4bytes FCS - Total 32Bytes */
#define BlockACK_SIZE_802_11ac 152  /**< 16 bytes header,2 bytes BA Control, 2 bytes BA starting Sequence Control 
									     128bytes BITMAP, 4bytes FCS - Total 32Bytes */

#define WLAN_MAX_PACKET_TRACE_FIELD 5
#define MAX_UP 8 // Maximum user priority

//	*************** Typedef for enumarations ********************
typedef enum enum_802_11_ControlPacketType WLAN_CNTRL_PKT;
typedef enum enum_802_11_MAC_States MAC_STATE;
typedef enum enum_802_11_Subevent_Type SUBEVENT;
typedef enum enum_802_11_CCA_STATUS CCA_STATUS;
typedef enum enum_802_11_CCA_MODE CCA_MODE;
typedef enum enum_802_11_Radio_State RADIO_STATE;
typedef enum enum_802_11_PHY_TYPES PHY_TYPE;
typedef enum enum_802_11_ChannelCharacteristics CHANNEL_MODEL;
typedef enum enum_802_11_BSS_TYPE BSS_TYPE;
typedef enum enum_802_11_DSSS_PLCP_SIGNAL_Field DSSS_RATE;
typedef enum enum_802_11_OFDM_RATE_Field OFDM_RATE;
//typedef enum enum_802_11_Modulation MODULATION;
//typedef enum enum_802_11_CodingRate CODING_RATE;
typedef enum enum_802_11_FrameControl_Type FRAME_TYPE;
typedef enum enum_WLAN_PacketTraceStatus WLAN_PKT_TRACE_STATUS;
typedef enum enum_802_11_Protocol IEEE_802_11_PROTOCOL;

//	*************** Typedef for Structures ********************
typedef struct stru_802_11_MAC_Frame_Header MAC_HEADER;
typedef struct stru_802_11_FrameControl FRAME_CONTROL;
typedef struct stru_802_11_RTS_Frame RTS_FRAME;
typedef struct stru_802_11_CTS_Frame CTS_FRAME;
typedef struct stru_802_11_ACK_Frame ACK_FRAME;
typedef struct stru_802_11_BlockACK_Frame BlockACK_FRAME;
typedef struct stru_802_11_MacVariables MAC_VAR;
typedef struct stru_802_11_PhyVariables PHY_VAR;
typedef struct stru_802_11_Metrics WLAN_METRICS;

// ********************** WLAN ENUMERATION DECLARATION START **********************
/// Enumeration for WLAN PHY protocols
enum enum_802_11_Protocol
{
	IEEE_802_11a = 1,
	IEEE_802_11b,
	IEEE_802_11g,
	IEEE_802_11n,
	IEEE_802_11e,
	IEEE_802_11ac,
};
/// Enumeration for  WLAN Control packet type
enum enum_802_11_ControlPacketType		
{
	WLAN_ACK = 1,
	WLAN_RTS, 
	WLAN_CTS,
	WLAN_BlockACK,
};
#define WLAN_CNTRL_PKT(Control_Type,Protocol) Protocol*100+Control_Type
/// Enumeration for 802.11 MAC states
enum enum_802_11_MAC_States
{
	MAC_IDLE = 0,
	WF_NAV,			
	Wait_DIFS,
	BACKING_OFF,
	TXing_BROADCAST,
	TXing_UNICAST,
	TXing_ACK,
	TXing_RTS,	
	TXing_CTS,
	Wait_DATA,
	Wait_CTS,
	Wait_ACK,
	Wait_BlockACK,
	OFF,
};
/** Enumeration for Sub event Type*/
 enum enum_802_11_Subevent_Type
{
	CS = MAC_PROTOCOL_IEEE802_11*100+1,	// Carrier Sense
	CHECK_NAV,
	NAV_END, 
	DIFS_END,
	BACKOFF,
	SEND_ACK,
	SEND_BLOCK_ACK,
	SEND_RTS,
	SEND_CTS,
	SEND_MPDU, 
	CTS_TIMEOUT,
	ACK_TIMEOUT,
	BLOCK_ACK_TIMEOUT,
	RECEIVE_RTS, 
	RECEIVE_CTS,	
	RECEIVE_ACK,
	RECEIVE_BLOCK_ACK,
	RECEIVE_MPDU,
	UPDATE_DEVICE_STATUS,
	AMPDU_SUBFRAME,
	AMPDU_FRAME,
 };
/** Enumeration for PHY layer according to IEEE802.11 2012 standard*/
 enum enum_802_11_CCA_STATUS 		
{
	BUSY=0,
	IDLE,
};
 /// page-1534 16.4.8.5 CCA, and 17.4.8.5 CCA  IEEE802.11-2012 
enum enum_802_11_CCA_MODE
{
	ED_ONLY=1,			// CCA Mode 1: Energy above threshold.
	CS_ONLY,			// CCA Mode 2: CS only. 
	ED_and_CS,			// CCA Mode 3: CS with energy above threshold. 
	CS_WITH_TIMER,		// CCA Mode 4: CS with timer. 
	HR_CS_and_ED,		// CCA Mode 5: A combination of CS and energy above threshold. 
};

/// Enumeration for channel/ radio status
enum enum_802_11_Radio_State
{
	RX_OFF,
	RX_ON_IDLE,
	RX_ON_BUSY,
	TRX_ON_BUSY,
	SLEEP,
};

enum enum_802_11_PHY_TYPES
{
	FHSS_2_4_GHz = 01,
	DSSS_2_4_GHz = 02,
	IR_Baseband = 03,
	OFDM = 04,
	DSSS = 05, //HRDSSS = 05,
	ERP = 06,
	HT = 07,
	VHT = 8,
};	
/// Enueration for pathloss models
enum enum_802_11_ChannelCharacteristics
{
	NO_PATHLOSS = 0,
	LINE_OF_SIGHT,
	FADING,
	FADING_AND_SHADOWING,
};
/// Page 2341 Enumerated types used in Mac and MLME service primitives
enum enum_802_11_BSS_TYPE
{
	INFRASTRUCTURE,
	INDEPENDENT,
	MESH,
	ANY_BSS,
};
/// Enumeration to represent the DSSS rate	
enum enum_802_11_DSSS_PLCP_SIGNAL_Field	
{
	Rate_1Mbps = 0x0A,
	Rate_2Mbps = 0x14,
	Rate_5dot5Mbps = 0x37,
	Rate_11Mbps = 0x6E,
};
/// Enumeration to represent the OFDM rate	
enum enum_802_11_OFDM_RATE_Field
{
	Rate_6Mbps = B4_1101,
	Rate_9Mbps = B4_1111,
	Rate_12Mbps = B4_0101,
	Rate_18Mbps = B4_0111,
	Rate_24Mbps = B4_1001,
	Rate_36Mbps = B4_1011,
	Rate_48Mbps = B4_0001,
	Rate_54mbps = B4_0011,
};

enum enum_WLAN_PacketTraceStatus
{
	DISABLE=0,
	ENABLE=1,
};

// ********************** Enumeration for MAC layer ************************
/// Page-382 of IEEE Std 802.11-2012 Table 8-1—Valid type and subtype combinations
enum enum_802_11_FrameControl_Type		
{
	MANAGEMENT = B2_00,
	CONTROL = B2_01,
	DATA = B2_10,
	RESERVED = B2_11,
};
/// Page-382 of IEEE Std 802.11-2012 Table 8-1—Valid type and subtype combinations 
enum enum_802_11_Management_Frame_SubType
{
	AssociationResponse	= B4_0000,
	ReassociationRequest = B4_0010,
	ReassociationResponse = B4_0011,
	ProbeRequest = B4_0100,
	ProbeResponse = B4_0101,
	TimingAdvertisement = B4_0110,
	Reserved_1 = B4_0111,
	Beacon = B4_1000,
	ATIM = B4_1001,
	Disassociation = B4_1010,
	Authentication = B4_1011,
	Deauthentication = B4_1100,
	Action = B4_1101,
	ActionNoAck	= B4_1110,
	Reserved_2 = B4_1111,
};
/// Page-383 of IEEE Std 802.11-2012 Table 8-1—Valid type and subtype combinations
enum enum_802_11_Control_Frame_SubType
{
	Reserved = B4_0000,	
	ControlWrapper = B4_0111,
	BlockAckRequest = B4_1000,
	BlockAck = B4_1001,
	PS_Poll	= B4_1010,
	RTS = B4_1011,
	CTS = B4_1100,
	ACK = B4_1101,
	CF_End = B4_1110,
	CF_End_Plus_CF_Ack = B4_1111,
};

/// Page-383 of IEEE Std 802.11-2012 Table 8-1—Valid type and subtype combinations
enum enum_802_11_Data_Frame_SubType
{
	Data = B4_0000,
	Data_Plus_CFAck = B4_0001,
	Data_Plus_CFPoll = B4_0010,
	Data_Plus_CFAck_Plus_CFPoll = B4_0011,		
	CF_Ack = B4_0101,
	CF_Poll = B4_0110,
	CFAck_Plus_CFPoll = B4_0111,
	QoS_Data = B4_1000,
	QoSData_Plus_CFAck = B4_1001,
	QoSData_Plus_CFPoll = B4_1010,
	QoSData_Plus_CFAck_Plus_CFPoll = B4_1011,
	QoS_Null = B4_1100,
	Reserved_4 = B4_1101,
	QoS_CFPoll = B4_1110,
	QoS_CFAck_Plus_CFPoll = B4_1111,
};
// ********************** WLAN STRUCTURE DECLARATION START ************************
/** Structure for MAC layer.
 Page-381 IEEE Std 802.11-2012 Figure 8-1—MAC frame format */
struct stru_802_11_MAC_Frame_Header 
{
	FRAME_CONTROL *pstruFrameControl;		//	2 bytes	
	short int snDurationID;					//	2 bytes
	PNETSIM_MACADDRESS Address1;				//	6 bytes  
	PNETSIM_MACADDRESS Address2;				//	6 bytes  
	PNETSIM_MACADDRESS Address3;				//	6 bytes  
	//short int nSeqoenceControl;//	2 bytes
	// Below two fields for Sequence Control
	int usnFragmentNumber:4;	// 4 bits	
	int usnSequenceNumber;	// 12 bits
	PNETSIM_MACADDRESS Address4;				//	6 bytes  
	//short int snQoS_Control;				//	2 bytes
	QOS_CONTROL *pstruQoSControl;
	//int nHT_Control;						//	4 bytes
	HT_CONTROL *pstruHTControl;
	int nFCS;								//	4 bytes
	// Simulation parameters
	double dBOTime;	
};											// ----------
											//  40 bytes
											
/// Page-382 IEEE Std 802.11-2012 Figure 8-2—Frame Control field
struct stru_802_11_FrameControl
{
	unsigned short int nProtocolVersion;
	FRAME_TYPE FrameType;	
	unsigned int usnSubType;
	bool bToDS;
	bool bFromDS;
	bool bMoreFragments;
	bool bRetry;
	bool bPowerManagement;
	bool bMoreData;
	bool bProtectedFrame;
	bool bOrder;
};
/// Data structure for Quality of Service
struct stru_802_11_QoS_Control
{
	int TID:4;
	bool EOSP;
	int AckPolicy:2;
	bool AMSDUPresent;
	int TXOPLimit:8;
};

struct stru_802_11_HT_Control
{
	// Link Adaptation Control
	int Reserved1;
	bool TRQ; 
	/* int MAI;	 MCS request (MRQ) or ASEL indication. Set to 14 (indicating ASELI) to indicate that the MFB/ASELC subfield is
				 interpreted as ASELC. Otherwise, the MAI subfield is interpreted as shown
				in Figure 8-7, and the MFB/ASELC subfield is interpreted as MCS feedback (MFB).*/
	
	int MRQ;	/**< MCS request Set to 1 to indicate that MFB is requested.
				 Set to 0 to indicate that no MFB is requested.*/
	int MSI;	/**< MRQ sequence identifier When the MRQ subfield is equal to 1, the MSI subfield contains a
				 sequence number in the range 0 to 6 that identifies the specific request.
				 When the MRQ subfield is equal to 0, the MSI subfield is reserved.*/
	int MFSI;	/**< MCS feedback sequence identifier. Set to the received value of MSI 
				 contained in the frame to which the MFB information refers.
				 Set to 7 for unsolicited MFB.MFSI */
	/*int MFB/ASELC;  MCS feedback and antenna selection command/data When the MAI subfield 
				   is equal to the value ASELI, this subfield is interpreted as defined 
				   in Figure 8-8 and Table 8-9. Otherwise, this subfield contains 
				   recommended MFB. A value of 127 indicates that no feedback is present.*/	
	int ASELCommand; /**< 0 - Transmit Antenna Selection Sounding Indication (TXASSI)					
					1 - Transmit Antenna Selection Sounding Request (TXASSR) or Transmit ASEL Sounding Resumption
					2 Receive Antenna Selection Sounding Indication (RXASSI)
					3 Receive Antenna Selection Sounding Request (RXASSR)
					4 Sounding Label 
					5 No Feedback Due to ASEL Training Failure or Stale Feedback
					6 Transmit Antenna Selection Sounding Indication requesting feedback 
						of explicit CSI (TXASSI-CSI)
					7 Reserved Reserved*/
	int ASELData;
	int CalibrationPosition;  /**< Position in calibration sounding exchange sequence
								Set to 0 indicates this is not a calibration frame.
								Set to 1 indicates calibration start.
								Set to 2 indicates sounding response.
								Set to 3 indicates sounding complete.*/
	int CalibrationSequence; /**< Calibration sequence identifier The field is included in 
							 each frame within the calibration procedure and its value 
							 is unchanged for the frame exchanges during one calibration
							 procedure. See 9.29.2.4.3.*/
	int Reserved2;
	int CSI_Steering;	/**< CSI/Steering subfield values
							0 No feedback required
							1 CSI
							2 Noncompressed beamforming
							3 Compressed beamforming */
	int NDPAnnouncement; /**<	The NDP Announcement subfield of the HT Control field indicates
							that an NDP will be transmitted after the frame.
							is set to 1 to indicate that an NDP will follow; otherwise,
							it is set to 0.*/
	int Reserved3;
	bool ACConstraint; /**< The AC Constraint subfield of the HT Control field indicates 
					   whether the mapped AC of an RD data frame is constrained to a single AC,
					   0 The response to a reverse direction grant (RDG) may contain data
					   frames from any TID.
					   1 The response to an RDG may contain data frames only from the same
					   AC as the last data frame received from the RD initiator  */
	bool RDG_MorePPDU; /**< The RDG/More PPDU subfield of the HT Control field is
					   interpreted differently depending on whether it is transmitted
					   by an RD initiator or an RD responder.
					   0	RD initiator --		No reverse grant
							RD responder --    The PPDU carrying the frame is the last
							transmission by the RD responder
					   1   RD initiator --  An RDG is present, as defined by the Duration/ID field
						 RD responder --  The PPDU carrying the frame is followed by another PPDU */
};

/// Page-404 of IEEE Std 802.11-2012 Figure 8-13—RTS frame
struct stru_802_11_RTS_Frame
{
	FRAME_CONTROL *pstruFrameControl;				// 2 bytes
	short int nDuration;							// 2 bytes
	PNETSIM_MACADDRESS RA;							// 6 bytes
	PNETSIM_MACADDRESS TA;							// 6 bytes
	int nFCS;										// 4 bytes	
	// Simulation parameters
	double dBOTime;
};
/// Page-405 of IEEE Std 802.11-2012 Figure 8-14—CTS frame
struct stru_802_11_CTS_Frame
{
	FRAME_CONTROL *pstruFrameControl;				// 2 bytes			
	short int nDuration;							// 2 bytes
	PNETSIM_MACADDRESS RA;							// 6 bytes
	int nFCS;										// 4 bytes
};
/// Page-405 of IEEE Std 802.11-2012 Figure 8-15—ACK frame
struct stru_802_11_ACK_Frame	
{
	FRAME_CONTROL *pstruFrameControl;				// 2 bytes			
	short int nDuration;							// 2 bytes
	PNETSIM_MACADDRESS RA;							// 6 bytes
	int nFCS;										// 4 bytes
};
/// Data structure for cummulative acknowledgement.
struct stru_802_11_BlockACK_Frame
{
	FRAME_CONTROL *pstruFrameControl;				// 2 bytes			
	short int nDuration;							// 2 bytes
	PNETSIM_MACADDRESS RA;							// 6 bytes
	PNETSIM_MACADDRESS TA;							// 6 bytes
	short int BARControl;							// 2 bytes
	short int BAStartingSequenceControl;			// 2 bytes
	bool BitMap[1024];								// 8 bytes = 64 bits for n & 128 bytes = 1024 bits for ac
	int nFCS;										// 4 bytes
													// 32bytes total
};

/**
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
********** PHY layer ************
Page-1514 IEEE Std 802.11 2012 Table 16-2 DS PHY characteristics
Page-1623 IEEE Std 802.11-2012 Table 18-17—OFDM PHY characteristics
Page-361 6.5.4.2 Semantics of the service primitive
PLME-CHARACTERISTICS
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
struct stru_802_11_PHY_PLME_characteristics // or PLME_Characteristics	
{
	int naSlotTime;
	int naSIFSTime;
	int naSignalExtension;
	int naCCATime;
	int naPHY_RX_START_Delay;
	int naRxTxTurnaroundTime;
	int naTxPLCPDelay;
	int naRxPLCPDelay;
	int naRxTxSwitchTime;
	int naTxRampOnTime;
	int naTxRampOffTime;
	int naTxRFDelay;
	int naRxRFDelay;
	int naAirPropagationTime;
	int naMACProcessingDelay;
	int naPreambleLength;
	int naRIFSTime;
	int naSymbolLength;
	int naSTFOneLength;	
	int naSTFTwoLength;
	int naLTFOneLength;
	int naLTFTwoLength;
	int naPLCPHeaderLength;
	int naPLCPSigTwoLength;
	int naPLCPServiceLength;
	int naPLCPConvolutionalTailLength;
	int naMPDUDurationFactor;
	int naMPDUMaxLength;
	int naPSDUMaxLength;
	int naPPDUMaxTime;
	int naIUSTime;
	int naDTT2UTTTime;
	int naCWmin;
	int naCWmax;
	int naMaxCSIMatricesReportDelay;
	int naMaxTODError;
	int naMaxTOAError;
	int naTxPmdTxStartRFDelay;
	int naTxPmdTxStartRMS;
};

/** Structure for NetSim Simulation MAC and PHY layer device variable */
struct stru_802_11_MacVariables
{
	struct stru_802_11_Metrics *pstruMetrics;
	// Configuration Parameters
	int nRetryLimit;		
	int nRTSThreshold;				
	int nCWcurrent;			
	int nRetryCount;
	MAC_STATE PrevState;
	MAC_STATE CurrentState;		
	CCA_STATUS Medium;	
	bool bFlagNavExpired;
	double dNAV;
	int nRtsSent;	
	int nCtsSent;
	int nCTSTimeOutTriggered;
	int nCtsReceivedFlag;	
	int nAckReceivedFlag;
	int nAckExpected_PacketId;
	int nAckExpected_SegmentId;	
	double dBackOffCounter;	
	double dPasuedBackOffTime;
	bool dBackoffPauseFlag;
	double dBackOffTime;	
	double dBackOffStartTime;
	bool BackOffFreezeFlag;	
	bool BackOffEventTriggered;
	double dMediumBusyTime;	
	int nRetryFlag;		
	int nRtsCtsFlag;	
	int nNumberOfAggregatedPackets;
	NetSim_PACKET *pstruTempData;
	double dTransmissionTime; 	
	unsigned long ulWlanBackoffSeed1;
	unsigned long ulWlanBackoffSeed2;
	int nCSFlag;		
	QOS_STATUS nQosStatus;
	QOSLIST* pstruQosList;
	int n80211eEnableFlag;
	bool rate_adaptationflag;
};
/** Structure for Physical layer variables */
struct stru_802_11_PhyVariables
{
	IEEE_802_11_PROTOCOL nPHYprotocol;
	struct stru_802_11_PHY_PLME_characteristics *pstruPhyChar; 
	int nDIFS;
	int nEIFS;
	int nRIFS;
	int nPIFS;
	// Configuration Parameters
	double nTxPower;
	PHY_TYPE PhyType;
	int usnChannelNumber;	
	double dFrequency;	
	double dPathLossExponent;
	double dStandardDeviation;		
	double dFadingFigure;
	double dDistFromAp;
	double dDistance;	
	double dReceiverSensivity;
	double dReceivedPower;			
	double dShadowReceivedPower;	
	double dTotalReceivedPower;		
	double dTotalRxPower_mw;	
	double dMinReceiverSensitivity; // to check out of range packets
	double dBER;		
	CHANNEL_MODEL ChannelModel;	
	double dBroadCastFrameDataRate;
	double dControlFrameDataRate;
	double dDataRate; 	
	OFDM_RATE OfdmRate;	
	MODULATION Modulation;
	CODING_RATE CodingRate;
	DSSS_RATE DsssRate;
	double dProcessingGain;		
	RADIO_STATE RadioState;
	CCA_MODE CcaMode;
	double dCSThreshold;
	//Connected device details 
	PNETSIM_MACADDRESS szBSSID;
	NETSIM_ID nConDeviceId;
	NETSIM_ID nConInterfaceId;	
	// Details for the center device
	short int nConDevCount;	
	NETSIM_ID nBSSID;
	BSS_TYPE BssType;
	double dTransmissionTime;	
	// Additional variable added for 802.11n 	
	// Configuration parameters
	double fFrequencyBand;
	unsigned short int nChannelBandwidth;
	unsigned short int nGuardInterval;
	unsigned short int nNSS;		// Number of spatial streams
	unsigned short int nNTX;		// Number of transmit chains
	unsigned short int nNRX;		// Number of receive chains
	unsigned short int nNSTS;		// Number of space-time streams 
	// other parameters needed for MCS Modulation and coding and rate details
	unsigned short int nNCBPS;		// Number of coded bits per symbol
	unsigned short int nNCBPSS_i;	// Number of coded bits per symbol per the ith spatial stream
	unsigned short int nNDBPS;		// Number of data bits per symbol
	unsigned short int nNBPSC;		// Number of coded bits per single carrier
	unsigned short int nNBPSCS_i;	// Number of coded bits per single carrier for spatial stream i
	unsigned short int nNESS;		// Number of extension spatial streams
	unsigned short int nNES;		// Number of BCC encoders for the Data field
	unsigned short int nN_HT_LTF;		// Number of HT Long Training fields (see 20.3.9.4.6)
	unsigned short int nN_HT_DLTF;	// Number of Data HT Long Training fields
	unsigned short int nN_HT_ELTF;	// Number of Extension HT Long Training fields
	double dR;						// Coding rate
	unsigned short int nMCS;

	unsigned short int nSTBC;
	unsigned short int nSounding;
	unsigned short int nSmoothing;
	unsigned short int nAggrigation;
	struct stru_802_11n_OFDM_MIMO_Parameters *pstruOfdmMimoParameter;
	struct stru_802_11ac_OFDM_MIMO_Parameters *pstruOfdmACMimoParameter;

	unsigned int maxPhyindex;
	unsigned int minPhyindex;
	unsigned int* currentPhyindex;
	int* currentDroppedCount;
	int* currentReceivedCount;
};
/** Structure for metrics */
struct stru_802_11_Metrics
{
	// Ap Metrics
	double dTotalTransmissionTime;
	double dQueuingDelay;
	double dMediaAccessTime;
	double *adConDevDistance;
	double *adDataRate;
	double *adBER;
	int nTransmittedFrameCountUnicast;
	int nTransmittedFrameCountBroadcast;
	int nReceivedFrameCountUnicast;
	int nReceivedFrameCountBroadcast;	
	int nRTSSuccessCount;
	int nRTSFailureCount;
	int nACKFailureCount ;
	int nFailedCount ;	
	int nBackoffFailedCount;
	int nErroredFrameCount;
	int nCollidedFrameCount;	
	int nOutOffReachFrameCount;
	// Below are not considered in the WLAN metrics display
	int nRtsReceivedCount;
	int nCtsReceivedCount;
	int nRTSFailureDueToNav;
	int nAckReceivedCount;
};

#define BATTERY 1
#define MAINLINE 2
/// Structure for Power model.
	typedef struct struPower
	{
		int nDeviceId;
		int nPowerSource; /* Battery
						  MainLine */
		int nEnergyHarvesting;
		double dRechargingCurrent_mA;
		double dInitialEnergy_mW;
		double dVoltage_V;
		double dTransmittingCurrent_mA;
		double dReceivingCurrent_mA;
		double dIdleModeCurrent_mA;
		double dSleepModeCurrent_mA;
		int nNodeStatus;	// 1 for ON, 0 for OFF
		int nNodePrevStatus;
		double dPrevChangedStateTime;
		double dRemainingPower;
		double dTransmissionModeEnergyConsumed;
		double dReceptionModeEnergyConsumed;
		double dSleepModeEnergyConsumed;
		double dIdleModeEnergyConsumed;
	}POWER,*PPOWER;
	PPOWER* pstruDevicePower;

// Global variables
int gnWLANPacketTraceField[WLAN_MAX_PACKET_TRACE_FIELD]; 
// variable to calculate the received power.
double** dCummulativeReceivedPower;

double** dCummulativeReceivedPower2;
int nCSRPFlag; //Carrier sense received power flag
// Variable to store number of 802.11 protocol enabled in a network
int gn80211ProtocolEnabledCount;
int gn80211PktTraceFlag;

/****************** WLAN dll FUNCTION DECLARATION *********************************/
// ++++++++++++++ 802_11_MAC.c +++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function to do carrier sense when packet arrive to wireless station or AP.
int fn_NetSim_WLAN_CarrierSense();
// Function to start backoff at the end of DIFS period.
int fn_NetSim_WLAN_StartBackOff();
// Function to pause the backoff counter when medium found to be BUSY.
//int fn_NetSim_WLAN_PauseBackOff();
int fn_NetSim_WLAN_PauseBackOff(NETSIM_ID nDeviceId, NETSIM_ID nInterfaceId, double dEventTime);
// Function to start frame transmission after back off period.
int	fn_NetSim_WLAN_StartTransmission();
// Function to add MAC header while sending MPDU.
int fn_NetSim_WLAN_Add_MAC_Header(NETSIM_ID ,NETSIM_ID , NetSim_PACKET* );
// Function to calculate the NAV.
int fn_NetSim_WLAN_CalculateNAV_Duration(NETSIM_ID nDevId,NETSIM_ID nInterfaceId,double dPacketSize, int *dDuration, int nFrameType);

// ++++++++++++++ 802_11_PHY.c +++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function to add Physical layer over heads.
int fn_NetSim_WLAN_Add_Phy_Header();
// Function to calculate the transmission time required for the packet to reach the receiver.
double fn_NetSim_WLAN_CalculateTransmissionTime(NetSim_PACKET *pstruPacket, NETSIM_ID nDevId, NETSIM_ID nInterfaceId, int flag);
// Function used to calculate and set the received power from one wireless device to others.
int fn_NetSim_WLAN_CalculateAndSetReceivedPower(NETSIM_ID nNodeId);
// Function to check the interference between the two wireless channel with in the 20 MHz range.
// Return 1 - interference and 0 - no interference.
int fn_NetSim_WLAN_CheckFrequencyInterfrence(double dFrequency1,double dFrequency2);
// Function to Initialize the Phy variables.
int fn_NetSim_WLAN_PHY_Configuration(NETSIM_ID nDeviceId,NETSIM_ID nInterfaceId);
// Function to transmit the frame by generating the PHYSICAL_IN event.
int fn_NetSim_WLAN_TransmitFrame(NetSim_PACKET* pstruPacket, NETSIM_ID nDevId, NETSIM_ID nInterfaceId, NETSIM_ID nConDevId,NETSIM_ID nConInterface,int );

int fn_NetSim_WLAN_SetDataRate(NETSIM_ID nDeviceId, NETSIM_ID nInterfaceId, NETSIM_ID nReceiverId);
int fn_NetSim_WLAN_CalculateBER(NETSIM_ID nDeviceId, NETSIM_ID nInterfaceId,double dReceivedPower,NetSim_PACKET* pstruPacket);
int fn_NetSim_WLAN_BackOff();
int fn_NetSim_WLAN_SendRTS(double** dCummulativeReceivedPower);
int fn_NetSim_WLAN_SendMPDU(double** dCummulativeReceivedPower);
int fn_NetSim_WLAN_SendCTS();
int fn_NetSim_WLAN_SendACK();
int fn_NetSim_WLAN_CheckNAV();
int fn_NetSim_WLAN_ReceiveACK();
int fn_NetSim_WLAN_ReceiveRTS();
int fn_NetSim_WLAN_ReceiveCTS();
int fn_NetSim_WLAN_ReceiveMPDU();
int fn_NetSim_WLAN_ResetNAV();
int fn_NetSim_WLAN_AckTimeOut();
int fn_NetSim_WLAN_CtsTimeOut();
int fn_NetSim_WLAN_IncreaseCW();
int fn_NetSim_WLAN_RandomBackOffTimeCalculation();
int fn_NetSim_WLAN_CheckRTSThreshold(NETSIM_ID nDeviceId,NETSIM_ID nInterfaceId, double dPacketSize);
int fn_NetSim_WLAN_CheckRetryLimit();

// **********  WLAN lib FUNCTION DECLARATION  *********************
// Function to change the Mac Current State to Previous State and Current State to nState.
int fn_NetSim_WLAN_ChangeMacState(struct stru_802_11_MacVariables *pstruMacVar, int nState);
// Function to change the radio state to TRANSMITTING, RECEIVING or IDLE.
int fn_NetSim_WLAN_ChangeRadioState(NETSIM_ID nDeviceId, NETSIM_ID nInterfaceId, RADIO_STATE nOldState, RADIO_STATE nNewState,POWER** pstruDevicePower,NetSim_EVENTDETAILS* pstruEventDetails);

// Function to check the destination device is present in the Associated wireless nodes to AP.
int fn_NetSim_WLAN_CheckDestDevicePresentWithinAP(NETSIM_ID nDevId, NETSIM_ID nInterfaceId, NETSIM_ID nDestId);
// Function to forward the frame when AP receive MPDU. 
int fn_NetSim_WLAN_AP_ForwardFrame();
// Function to send the broadcast frame to all connected devices. 
int fn_NetSim_WLAN_TransmitBroadCastFrame(NetSim_PACKET* pstruPacket,NETSIM_ID nDevId,NETSIM_ID nInterface,int SubEventType);
// Function to send the unicast frame to the connected receiver. 
int fn_NetSim_WLAN_TransmitUnicastFrame(NetSim_PACKET* pstruPacket,NETSIM_ID nDevId,NETSIM_ID nInterface, int);
// Function to check the packets in the packet list of Access buffer.
int fn_NetSim_WLAN_CheckPacketsInPacketlist(NETSIM_ID nDevId, NETSIM_ID nInterfaceId);
// Function used to Assign the Receiver Sensitivity of a Receiver.
int fn_NetSim_Assign_RXSensitivityDataRateAtReceiver(NETSIM_ID nDeviceId, NETSIM_ID nInterfaceId, NetSim_PACKET *pstruPacket);


/****************** NetWorkStack DLL functions declarations *****************************************/
/* Function for configuring WLAN parameters*/
_declspec(dllexport) int fn_NetSim_WLAN_Configure(void** var);
int fn_NetSim_WLAN_Configure_F(void** var);
/* Function for Initializing WLAN protocol */
_declspec(dllexport) int fn_NetSim_WLAN_Init_F(struct stru_NetSim_Network *NETWORK_Formal, NetSim_EVENTDETAILS *pstruEventDetails_Formal,char *pszAppPath_Formal, char *pszWritePath_Formal,int nVersion_Type,void **fnPointer);
/* Function to run WLAN protocol */
_declspec (dllexport) int fn_NetSim_WLAN_Run(); 
//Function to free the WLAN protocol variable and Unload the primitives
_declspec(dllexport) int fn_NetSim_WLAN_Finish();
int fn_NetSim_WLAN_Finish_F();
//Return the sub event name with respect to the sub event number for writing event trace
_declspec (dllexport) char *fn_NetSim_WLAN_Trace(int nSubEvent);
char *fn_NetSim_WLAN_Trace_F(int nSubEvent);
// Function to free the allocated memory for the WLAN packet
_declspec(dllexport) int fn_NetSim_WLAN_FreePacket(NetSim_PACKET* );
int fn_NetSim_WLAN_FreePacket_F(NetSim_PACKET* );
// Function to copy the WLAN packet from source to destination
_declspec(dllexport) int fn_NetSim_WLAN_CopyPacket(NetSim_PACKET* ,NetSim_PACKET* );
int fn_NetSim_WLAN_CopyPacket_F(NetSim_PACKET* ,NetSim_PACKET* );
// Function to write WLAN Metrics into Metrics.txt
_declspec(dllexport) int fn_NetSim_WLAN_Metrics(char* );
int fn_NetSim_WLAN_Metrics_F(char* );
// Function to configure and write WLAN packet trace  
_declspec(dllexport) char* fn_NetSim_WLAN_ConfigPacketTrace(const void* xmlNetSimNode);
_declspec(dllexport) int fn_NetSim_WLAN_WritePacketTrace(NetSim_PACKET* pstruPacket, char** ppszTrace);
// Function to do tasks when PHYSICAL_OUT event is triggered.
int fn_NetSim_WLAN_PhysicalOut();
// Function to do tasks when PHYSICAL_IN event is triggered.
int fn_NetSim_WLAN_PhysicalIn();
// Function to do tasks when MAC_IN event is triggered.
int fn_NetSim_WLAN_MacIn();
// Function to update the device state. PHY OUT
int fn_NetSim_WLAN_Transmitter_UpdateMedium_UpdateDeviceStatus(NETSIM_ID nDeviceId,NETSIM_ID nInterfaceId); 
// Function to update the device state.UPDATE DEVICE STATE TIMER event at PHY IN EventTime.
int	fn_NetSim_WLAN_Receiver_UpdateMedium_UpdateDeviceStatus();
NetSim_PACKET* fn_NetSim_Packet_CopyAggregatedPacket(NetSim_PACKET*);

int fn_NetSim_WLAN_DIFS();
int fn_NetSim_WLAN_SendBlockACK();
int fn_NetSim_WLAN_ReceiveBlockACK();
int fnCalculateFadingLoss(unsigned long *ulSeed1, unsigned long *ulSeed2,double *dFadingPower,int fm1);
unsigned int get_phy_parameter_min_index(IEEE_802_11_PROTOCOL phystd,int bandwidth);
unsigned int get_phy_parameter_max_index(IEEE_802_11_PROTOCOL phystd,int bandwidth);
unsigned int get_phy_parameter_current_index(NETSIM_ID devid,NETSIM_ID ifid,NETSIM_ID rcvid);
void packet_drop_notify(NETSIM_ID devid,NETSIM_ID ifid,NETSIM_ID rcvid);
void packet_recv_notify(NETSIM_ID devid,NETSIM_ID ifid,NETSIM_ID rcvid);

#define WLAN_PHY(d,i) ((PHY_VAR*)DEVICE_PHYVAR(d,i))
#define WLAN_MAC(d,i) ((MAC_VAR*)DEVICE_MACVAR(d,i))
#define WLAN_CHANGERADIOSTATE(nDeviceId,ninterfaceId,nNewState) fn_NetSim_WLAN_ChangeRadioState(nDeviceId,ninterfaceId,WLAN_PHY(nDeviceId,ninterfaceId)->RadioState,nNewState,pstruDevicePower,pstruEventDetails)

#ifdef  __cplusplus
}
#endif

#endif

