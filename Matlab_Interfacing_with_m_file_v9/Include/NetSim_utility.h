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



#ifndef _NETSIM_UTILITY_H_
#define _NETSIM_UTILITY_H_
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include "stdarg.h"
#include "Linux.h"
#endif
#include <time.h>
#include <stdint.h>


#ifdef  __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#ifndef _NETSIM_UTILITY_CODE_
#pragma comment(lib,"NetSim_utility.lib")
#define NETSIM_UTILITY_EXPORT _declspec(dllimport)
#else
#define NETSIM_UTILITY_EXPORT _declspec(dllexport)
#endif
#else
#ifndef _NETSIM_UTILITY_CODE_
#define NETSIM_UTILITY_EXPORT extern
#else
#define NETSIM_UTILITY_EXPORT 
#endif
#endif

	typedef struct stru_pcap_writer
	{
		FILE* pFile;
		time_t start_time;
		HANDLE hPipe;
	}PCAP_WRITER,*PPCAP_WRITER;

	typedef struct stru_wireshark_trace
	{
		PPCAP_WRITER* pcapWriterlist;
		void (*convert_sim_to_real_packet)(void* netsim_packet,void* pcap_handle,double dTime);
	}WIRESHARKTRACE,*PWIRESHARKTRACE;

	NETSIM_UTILITY_EXPORT WIRESHARKTRACE wireshark_trace;

	//Function prototype

	//Used to get path for any file
	_declspec(dllexport) void fnGetFilePath(__in char* name,__out char* path);

	//Used to get options index form command
	_declspec(dllexport) int fnParseOption(int count,char* arg[],char* opt);

	//Used to get current time in string
	_declspec(dllexport) void GetTimeStr(__out char* date,__in int len);

	//Used to print the log file with thread id and time
	_declspec(dllexport) FILE* open_log_file(char* filename,char* mode);
	_declspec(dllexport) int close_log_file(FILE* fp);
	_declspec(dllexport) int ReportLog(__in FILE* fp,__in const char* msg,...);

	/* Strip whitespace chars off end of given string, in place. Return s. */
	_declspec(dllexport) char* rstrip(char* s);

	/* Return pointer to first non-whitespace char in given string. */
	_declspec(dllexport) char* lskip(const char* s);

	/* Return pointer to first char c or ';' comment in given string, or pointer to
   null at end of string if neither found. ';' must be prefixed by a whitespace
   character to register as a comment. */
	_declspec(dllexport) char* find_char_or_comment(const char* s, char c);

	/* Version of strncpy that ensures dest (size bytes) is null-terminated. */
	_declspec(dllexport) char* strncpy0(char* dest, const char* src, size_t size);

	/* Used to init the start_time. Must be called once */
	_declspec(dllexport) void init_time();

	/* Used to get current time in microsecond */
	_declspec(dllexport) uint64_t em_current_time();

	/* Used to initiallize the start time*/
	_declspec(dllexport) void em_init_time(void);

	/* Used to get start time */
	_declspec(dllexport) struct tm get_start_time();

	/* used to get file name without extension */
	_declspec(dllexport) char* get_filename_without_extension(char* filename);

	/* Used to prepare filter string for wireshark */
	_declspec(dllexport) void create_wireshark_filter(__out char* filter,__in int len,__in char* src_ip,__in char* dst_ip);

	/* Call wireshark */
	_declspec(dllexport) void call_wireshark(int duration,char* filter_string);

	/* open pcap file for writing */
	_declspec(dllexport) void* open_pcap_file(const char *filename,char* pipename);

	/* write packet to pcap file */
	_declspec(dllexport) void write_to_pcap(void* handle,unsigned char* packet,unsigned int len,double* dTime);

	/* close the pcap write */
	_declspec(dllexport) void close_pcap_file(void* pcap_handle);



#ifdef  __cplusplus
}
#endif
#endif //_NETSIM_UTILITY_H_
