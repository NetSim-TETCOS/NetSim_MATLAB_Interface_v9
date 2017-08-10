/************************************************************************************
 * Copyright (C) 2013                                                               *
 * TETCOS, Bangalore. India                                                         *
 *                                                                                  *
 * Tetcos owns the intellectual property rights in the Product and its content.     *
 * The copying, redistribution, reselling or publication of any or all of the       *
 * Product or its content without express prior written consent of Tetcos is        *
 * prohibited. Ownership and / or any other right relating to the software and all *
 * intellectual property rights therein shall remain at all times with Tetcos.      *
 *                                                                                  *
 * Author:    Shashi Kant Suman                                                       *
 *                                                                                  *
 * ---------------------------------------------------------------------------------*/
#ifndef _NETSIM_GRAPH_H_
#define _NETSIM_GRAPH_H_
#ifndef WIN32
#include <stdio.h>
#include "Linux.h"
#endif
struct stru_GenericGraph
{
	char* heading;
	char* xLabel;
	char* yLabel;
	int realTimeFlag;
	char* txtFileName;
	char* bmpFileName;
	int labelcount;
	char** label;
	FILE* dataFile;
	#ifdef WIN32 
	CRITICAL_SECTION criticalSection;
	#else
	pthread_mutex_t criticalSection;
	#endif
	int fileupdateflag;
	char* command;
	int continueflag;
	void* plot;
	int (*fnCallreplot)(struct stru_GenericGraph*);
	struct stru_GenericGraph* next;
};

/* Graph API */
_declspec(dllexport) struct stru_GenericGraph* fn_NetSim_Install_Metrics_Graph(char* heading,
															char* xLabel,
															char* yLabel,
															bool realTimeFlag);
#endif
