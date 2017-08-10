#include "main.h"
#include "WLAN.h"

/**
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
********************* 802.11a/b/g ************************
Index,ReceiverSensitivity,Modulation,DataRate,CodingRate,NBPSC,NCBPS,NDBPS
NBPSC - Coded bits per subcarrier
NCBPS - Coded bits per OFDM symbol
NDBPS - Data bits per OFDM symbol
page-1590 Table 18-4—Modulation-dependent parameters
page-1612 Table 18-14—Receiver performance requirements

*********************** 802.11n ***************************
Index,ReceiverSensitivity,Modulation,DataRate,CodingRate,NBPSC,NCBPS,NDBPS
Note: Using formula calculating data rate, NBPSC and NCBPS.
NDBPS - Data bits per OFDM symbol
page 1745 Table 20-23—Receiver minimum input level sensitivity  802.11-2012.pdf
Page 319 Table 20-22—Receiver minimum input level sensitivity 802.11n-2009.pdf 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/

struct stru_802_11_Phy_Parameters struPhyParameters[69] =
{
	  //IEEE802.11b
	{0,0,0,0,0,0,0,0},	
	{1,-97,Modulation_DBPSK,1,0,0,0,0},
	{2,-66,Modulation_DQPSK,2,0,0,0,0},
	{3,-57,Modulation_CCK,5.5,0,0,0,0},
	{4,-53,Modulation_CCK,11,0,0,0,0},

	 //IEEE802.11a and IEEE802.11g
	{5,-82,Modulation_BPSK,6,Coding_1_2,1,48,24},
	{6,-79,Modulation_BPSK,9,Coding_3_4,1,48,36},
	{7,-77,Modulation_QPSK,12,Coding_1_2,2,96,48},
	{8,-74,Modulation_QPSK,18,Coding_3_4,2,96,72},
	{9,-70,Modulation_16_QAM,24,Coding_1_2,4,192,96},
	{10,-66,Modulation_16_QAM,36,Coding_3_4,4,192,144},
	{11,-65,Modulation_64_QAM,48,Coding_2_3,6,288,192},
	{12,-64,Modulation_64_QAM,54,Coding_3_4,6,288,216},

	//IEEE802.11n 20MHz Bandwidth
	{13,-82,Modulation_BPSK,0,Coding_1_2,1,0,0},
	{14,-79,Modulation_QPSK,0,Coding_1_2,2,0,0},
	{15,-77,Modulation_QPSK,0,Coding_3_4,2,0,0},
	{16,-74,Modulation_16_QAM,0,Coding_1_2,4,0,0},
	{17,-70,Modulation_16_QAM,0,Coding_3_4,4,0,0},
	{18,-66,Modulation_64_QAM,0,Coding_2_3,6,0,0},
	{19,-65,Modulation_64_QAM,0,Coding_3_4,6,0,0},
	{20,-64,Modulation_64_QAM,0,Coding_5_6,6,0,0},

	//IEEE802.11n 40MHz Bandwidth  
	{21,-79,Modulation_BPSK,0,Coding_1_2,1,0,0},
	{22,-76,Modulation_QPSK,0,Coding_1_2,2,0,0},
	{23,-74,Modulation_QPSK,0,Coding_3_4,2,0,0},
	{24,-71,Modulation_16_QAM,0,Coding_1_2,4,0,0},
	{25,-67,Modulation_16_QAM,0,Coding_3_4,4,0,0},
	{26,-63,Modulation_64_QAM,0,Coding_2_3,6,0,0},
	{27,-62,Modulation_64_QAM,0,Coding_3_4,6,0,0},
	{28,-61,Modulation_64_QAM,0,Coding_5_6,6,0,0},	
	
	//IEEE802.11ac 20MHz Bandwidth
	{29,-82,Modulation_BPSK,0,Coding_1_2,1,0,0},
	{30,-79,Modulation_QPSK,0,Coding_1_2,2,0,0},
	{31,-77,Modulation_QPSK,0,Coding_3_4,2,0,0},
	{32,-74,Modulation_16_QAM,0,Coding_1_2,4,0,0},
	{33,-70,Modulation_16_QAM,0,Coding_3_4,4,0,0},
	{34,-66,Modulation_64_QAM,0,Coding_2_3,6,0,0},
	{35,-65,Modulation_64_QAM,0,Coding_3_4,6,0,0},
	{36,-64,Modulation_64_QAM,0,Coding_5_6,6,0,0},
	{37,-59,Modulation_256_QAM,0,Coding_3_4,8,0,0},
	{38,-57,Modulation_256_QAM,0,Coding_5_6,8,0,0},
	
	//IEEE802.11ac 40MHz Bandwidth  
	{39,-79,Modulation_BPSK,0,Coding_1_2,1,0,0},
	{40,-76,Modulation_QPSK,0,Coding_1_2,2,0,0},
	{41,-74,Modulation_QPSK,0,Coding_3_4,2,0,0},
	{42,-71,Modulation_16_QAM,0,Coding_1_2,4,0,0},
	{43,-67,Modulation_16_QAM,0,Coding_3_4,4,0,0},
	{44,-63,Modulation_64_QAM,0,Coding_2_3,6,0,0},
	{45,-62,Modulation_64_QAM,0,Coding_3_4,6,0,0},
	{46,-61,Modulation_64_QAM,0,Coding_5_6,6,0,0},
	{47,-56,Modulation_256_QAM,0,Coding_3_4,8,0,0},
	{48,-54,Modulation_256_QAM,0,Coding_5_6,8,0,0},
	
	//IEEE802.11ac 80MHz Bandwidth
	{49,-76,Modulation_BPSK,0,Coding_1_2,1,0,0},
	{50,-73,Modulation_QPSK,0,Coding_1_2,2,0,0},
	{51,-71,Modulation_QPSK,0,Coding_3_4,2,0,0},
	{52,-68,Modulation_16_QAM,0,Coding_1_2,4,0,0},
	{53,-64,Modulation_16_QAM,0,Coding_3_4,4,0,0},
	{54,-60,Modulation_64_QAM,0,Coding_2_3,6,0,0},
	{55,-59,Modulation_64_QAM,0,Coding_3_4,6,0,0},
	{56,-58,Modulation_64_QAM,0,Coding_5_6,6,0,0},
	{57,-53,Modulation_256_QAM,0,Coding_3_4,8,0,0},
	{58,-51,Modulation_256_QAM,0,Coding_5_6,8,0,0},
	
	//IEEE802.11ac 160MHz Bandwidth  
	{59,-73,Modulation_BPSK,0,Coding_1_2,1,0,0},
	{60,-70,Modulation_QPSK,0,Coding_1_2,2,0,0},
	{61,-68,Modulation_QPSK,0,Coding_3_4,2,0,0},
	{62,-65,Modulation_16_QAM,0,Coding_1_2,4,0,0},
	{63,-61,Modulation_16_QAM,0,Coding_3_4,4,0,0},
	{64,-57,Modulation_64_QAM,0,Coding_2_3,6,0,0},
	{65,-56,Modulation_64_QAM,0,Coding_3_4,6,0,0},
	{66,-55,Modulation_64_QAM,0,Coding_5_6,6,0,0},
	{67,-50,Modulation_256_QAM,0,Coding_3_4,8,0,0},
	{68,-48,Modulation_256_QAM,0,Coding_5_6,8,0,0}
	
};

#define WLAN_11b_min		1
#define WLAN_11b_max		4

#define WLAN_11a_min		5
#define WLAN_11a_max		12

#define WLAN_11g_min		5
#define WLAN_11g_max		12

#define WLAN_11n_20_min		13
#define WLAN_11n_20_max		20

#define WLAN_11n_40_min		21
#define WLAN_11n_40_max		28

#define WLAN_11ac_20_min	29
#define WLAN_11ac_20_max	38

#define WLAN_11ac_40_min	39
#define WLAN_11ac_40_max	48

#define WLAN_11ac_80_min	49
#define WLAN_11ac_80_max	58

#define WLAN_11ac_160_min	59
#define WLAN_11ac_160_max	68


#define INDEX_DROP_COUNT	3
#define INDEX_CREDIT_COUNT	19

unsigned int get_phy_parameter_min_index(IEEE_802_11_PROTOCOL phystd,int bandwidth)
{
	switch(phystd)
	{
	case IEEE_802_11b:
		return WLAN_11b_min;
	case IEEE_802_11a:
		return WLAN_11a_min;
	case IEEE_802_11g:
		return WLAN_11g_min;
	case IEEE_802_11n:
		{
			if(bandwidth == 20)
				return WLAN_11n_20_min;
			else if(bandwidth == 40)
				return WLAN_11n_40_min;
			else
				return 0;
		}
	case IEEE_802_11ac:
		{
			if(bandwidth == 20)
				return WLAN_11ac_20_min;
			else if(bandwidth == 40)
				return WLAN_11ac_40_min;
			else if(bandwidth == 80)
				return WLAN_11ac_80_min;
			else if(bandwidth == 160)
				return WLAN_11ac_160_min;
			else
				return 0;
		}
	}
	return 0;
}

unsigned int get_phy_parameter_max_index(IEEE_802_11_PROTOCOL phystd,int bandwidth)
{
	switch(phystd)
	{
	case IEEE_802_11b:
		return WLAN_11b_max;
	case IEEE_802_11a:
		return WLAN_11a_max;
	case IEEE_802_11g:
		return WLAN_11g_max;
	case IEEE_802_11n:
		{
			if(bandwidth == 20)
				return WLAN_11n_20_max;
			else if(bandwidth == 40)
				return WLAN_11n_40_max;
			else
				return 0;
		}
	case IEEE_802_11ac:
		{
			if(bandwidth == 20)
				return WLAN_11ac_20_max;
			else if(bandwidth == 40)
				return WLAN_11ac_40_max;
			else if(bandwidth == 80)
				return WLAN_11ac_80_max;
			else if(bandwidth == 160)
				return WLAN_11ac_160_max;
			else
				return 0;
		}
	}
	return 0;
}

unsigned int get_phy_parameter_current_index(NETSIM_ID devid,NETSIM_ID ifid,NETSIM_ID rcvid)
{
	PHY_VAR* phy=WLAN_PHY(devid,ifid);
	return phy->currentPhyindex[rcvid];
}

void packet_drop_notify(NETSIM_ID devid,NETSIM_ID ifid,NETSIM_ID rcvid)
{
	PHY_VAR* phy=WLAN_PHY(devid,ifid);
	phy->currentDroppedCount[rcvid]++;
	//printf("\nDrop count -- %d\n",phy->currentDroppedCount[rcvid]);
	if(phy->currentDroppedCount[rcvid] > INDEX_DROP_COUNT)
	{
		if(phy->currentPhyindex[rcvid] > phy->minPhyindex)
			phy->currentPhyindex[rcvid]--;
		phy->currentDroppedCount[rcvid]=0;
		//printf("\nIndex for %d to %d is %d\n",devid,rcvid,phy->currentPhyindex[rcvid]);
	}
	phy->currentReceivedCount[rcvid] = 0;
}

void packet_recv_notify(NETSIM_ID devid,NETSIM_ID ifid,NETSIM_ID rcvid)
{
	PHY_VAR* phy=WLAN_PHY(devid,ifid);
	phy->currentReceivedCount[rcvid]++;
	//printf("\nReceive count -- %d\n",phy->currentReceivedCount[rcvid]);
	if(phy->currentReceivedCount[rcvid] > INDEX_CREDIT_COUNT)
	{
		if(phy->currentPhyindex[rcvid] < phy->maxPhyindex)
			phy->currentPhyindex[rcvid]++;
		phy->currentReceivedCount[rcvid]=0;
		//printf("\nIndex for %d to %d is %d\n",devid,rcvid,phy->currentPhyindex[rcvid]);
	}
	phy->currentDroppedCount[rcvid] = 0;
}
