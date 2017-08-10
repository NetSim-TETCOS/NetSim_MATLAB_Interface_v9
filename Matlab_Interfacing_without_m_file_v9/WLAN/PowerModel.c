/************************************************************************************
* Copyright (C) 2015                                                               *
* TETCOS, Bangalore. India                                                         *
*                                                                                  *
* Tetcos owns the intellectual property rights in the Product and its content.     *
* The copying, redistribution, reselling or publication of any or all of the       *
* Product or its content without express prior written consent of Tetcos is        *
* prohibited. Ownership and / or any other right relating to the software and all  *
* intellectual property rights therein shall remain at all times with Tetcos.      *
*                                                                                  *
* Author:    Shashi Kant Suman                                                     *
*                                                                                  *
* ---------------------------------------------------------------------------------*/
# include "main.h"
# include "WLAN.h"
/** 
	The Purpose of this function is to change the transceiver radio states  
*/
int fn_NetSim_WLAN_ChangeRadioState(NETSIM_ID nDeviceId, NETSIM_ID nInterfaceId, RADIO_STATE nOldState, RADIO_STATE nNewState,POWER** pstruDevicePower,NetSim_EVENTDETAILS* pstruEventDetails)
{
	double dPrevTime,dConsumedEnergy,dRechargeEnergy=0;
	int nStatus = 0;
	if(!pstruDevicePower)
		goto NO_POWER_MODEL;
	dPrevTime = pstruDevicePower[nDeviceId-1]->dPrevChangedStateTime;
	switch(nOldState)
	{
	case SLEEP:
		dConsumedEnergy = pstruDevicePower[nDeviceId-1]->dSleepModeCurrent_mA*pstruDevicePower[nDeviceId-1]->dVoltage_V*(ldEventTime-dPrevTime)/1000000;
		pstruDevicePower[nDeviceId-1]->dSleepModeEnergyConsumed += dConsumedEnergy;
		break;
	case TRX_ON_BUSY:
		dConsumedEnergy = pstruDevicePower[nDeviceId-1]->dTransmittingCurrent_mA*pstruDevicePower[nDeviceId-1]->dVoltage_V*(ldEventTime-dPrevTime)/1000000;
		pstruDevicePower[nDeviceId-1]->dTransmissionModeEnergyConsumed += dConsumedEnergy;
		break;
	case RX_ON_IDLE:
		dConsumedEnergy = pstruDevicePower[nDeviceId-1]->dIdleModeCurrent_mA*pstruDevicePower[nDeviceId-1]->dVoltage_V*(ldEventTime-dPrevTime)/1000000;
		pstruDevicePower[nDeviceId-1]->dIdleModeEnergyConsumed += dConsumedEnergy;
		break;
	case RX_ON_BUSY:
		dConsumedEnergy = pstruDevicePower[nDeviceId-1]->dReceivingCurrent_mA*pstruDevicePower[nDeviceId-1]->dVoltage_V*(ldEventTime-dPrevTime)/1000000;
		pstruDevicePower[nDeviceId-1]->dReceptionModeEnergyConsumed += dConsumedEnergy;
		break;
	case RX_OFF:
		dConsumedEnergy = pstruDevicePower[nDeviceId-1]->dIdleModeCurrent_mA*pstruDevicePower[nDeviceId-1]->dVoltage_V*(ldEventTime-dPrevTime)/1000000;
		pstruDevicePower[nDeviceId-1]->dIdleModeEnergyConsumed += dConsumedEnergy;
		break;
	default:
		dConsumedEnergy=0;
		//Unknown mode
		break;
	}
	//Change the state

	nStatus = 1;
	if(pstruDevicePower[nDeviceId-1]->nNodeStatus == OFF)
	{
		nStatus = 0;
		//	return nStatus;
	}
	dRechargeEnergy = pstruDevicePower[nDeviceId-1]->dRechargingCurrent_mA*pstruDevicePower[nDeviceId-1]->dVoltage_V*(ldEventTime-dPrevTime)/1000000;
	//Store the time
	pstruDevicePower[nDeviceId-1]->dPrevChangedStateTime = ldEventTime;
	pstruDevicePower[nDeviceId-1]->dRemainingPower -=dConsumedEnergy;
	pstruDevicePower[nDeviceId-1]->dRemainingPower += dRechargeEnergy;
	if(pstruDevicePower[nDeviceId-1]->dRemainingPower  <= 0 && pstruDevicePower[nDeviceId-1]->nPowerSource == BATTERY)
	{
		pstruDevicePower[nDeviceId-1]->nNodeStatus = OFF;
		pstruDevicePower[nDeviceId-1]->dRemainingPower = 0;
		nStatus = 0;
	}
	if(nStatus)
	{
NO_POWER_MODEL:
		nStatus=1;
		WLAN_PHY(nDeviceId,nInterfaceId)->RadioState = nOldState; 
		WLAN_PHY(nDeviceId,nInterfaceId)->RadioState = nNewState; 
		return nStatus;
	}
	else
	{
		WLAN_PHY(nDeviceId,nInterfaceId)->RadioState = RX_OFF;
		WLAN_MAC(nDeviceId,nInterfaceId)->CurrentState = OFF;
		return nStatus;
	}
}