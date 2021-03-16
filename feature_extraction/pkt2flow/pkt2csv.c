/*
 * Author: Lucas Airam Castro de Souza
 * Laboratory: Grupo de Teleinformática e Automação
 * University: Universidade Federal do Rio de Janeiro
 *
 *
 *
 * Configuration: set output file and flow time window size in configuration.h
 *
 * Usage: ./pkt2flow [pcapfile]
 * 
 * Output: file
 *
 * 
 *
 * */



#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pcap.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "structures.h"
#include "functionHeader.h"
#include "configuration.h"



FILE 		*FLOWS_RESULTS;				/* file to wirte results */		
flowList_t 	*LIST_FIRST;				/* contains all active flows */ 
bool 		empty;					/* indicates if the list is empty */
flowList_t 	*LIST_LAST;			        /* contains the final address */ 
double		timer;					/* contains the time to remove the oldest flow */
 

/******************************** Flow list interaction functions **********************************/

flowList_t * 
findFlow(flowID_t flow, bool *find, char *direction)
{
	flowList_t *currentFlow;
	currentFlow = LIST_FIRST;
	

	while(currentFlow->flowHeader.ipDst)
	{

		if(	
			(!(strcmp(currentFlow->flowHeader.ipSrc,flow.ipSrc)) &&
			!(strcmp(currentFlow->flowHeader.ipDst,flow.ipDst)) &&	
			currentFlow->flowHeader.portSrc == flow.portSrc &&
			currentFlow->flowHeader.portDst == flow.portDst &&
			currentFlow->flowHeader.protocol == flow.protocol)
				)
		{
			*(find) = true;
			*(direction) = FORWARD;
			return currentFlow;
		}
		
		if(	
			(!(strcmp(currentFlow->flowHeader.ipSrc,flow.ipDst)) &&
			!(strcmp(currentFlow->flowHeader.ipDst,flow.ipSrc)) &&	
			currentFlow->flowHeader.portSrc == flow.portDst &&
			currentFlow->flowHeader.portDst == flow.portSrc &&
			currentFlow->flowHeader.protocol == flow.protocol)
				)
		{
			*(find) = true;
			*(direction) = BACKWARD;
			return currentFlow;
		}
		currentFlow = currentFlow->next;
	}
	*(find) = false;
	return NULL;
}

int 
flowAdd(flowID_t flow)
{
	flowList_t *newEntry;	     	/* if the list is not empty, this variable will allocate the new entry */


	if(empty)
	{
		/* get flow header */
		strcpy(LIST_FIRST->flowHeader.ipSrc,flow.ipSrc);
		strcpy(LIST_FIRST->flowHeader.ipDst,flow.ipDst);
		LIST_FIRST->flowHeader.portSrc 			= flow.portSrc;
		LIST_FIRST->flowHeader.portDst 			= flow.portDst;
		LIST_FIRST->flowHeader.protocol 		= flow.protocol;
		LIST_FIRST->flowHeader.time 			= flow.time;
		LIST_FIRST->flowFeatures.totalForwardPackets	= 0;	
		LIST_FIRST->flowFeatures.totalBackwardPackets	= 0;	
		LIST_LAST->flowFeatures.pkt_window		= NULL;	
		empty 						= false;
		
		/* set the timer */
		timer = flow.time;
	
		/* allocate the next element */
		newEntry = (flowList_t *)(malloc(sizeof(flowList_t)));
		newEntry->last 		= LIST_FIRST;
		newEntry->next 		= NULL;
		LIST_LAST 		= newEntry;
		LIST_FIRST->next 	= LIST_LAST;
		LIST_LAST->flowFeatures.pkt_window		= NULL;	
		return ok;
	}

	/* for the second flow or greater */

	/* get flow header */
	strcpy(LIST_LAST->flowHeader.ipSrc,flow.ipSrc);
	strcpy(LIST_LAST->flowHeader.ipDst,flow.ipDst);
	LIST_LAST->flowHeader.portSrc 			= flow.portSrc;
	LIST_LAST->flowHeader.portDst 			= flow.portDst;
	LIST_LAST->flowHeader.protocol 			= flow.protocol;
	LIST_LAST->flowHeader.time 			= flow.time;
	LIST_LAST->flowFeatures.totalForwardPackets	= 0;	
	LIST_LAST->flowFeatures.totalBackwardPackets	= 0;	
	LIST_LAST->flowFeatures.pkt_window		= NULL;	

	/* allocate the next element */
	newEntry = (flowList_t *)(malloc(sizeof(flowList_t)));
	newEntry->last 		= LIST_LAST;
	newEntry->next 		= NULL;
	newEntry->flowHeader.ipSrc[0] = '\0';
	newEntry->flowHeader.ipDst[0] = '\0';
	LIST_LAST->next 	= newEntry;
	LIST_LAST 		= newEntry;
	LIST_LAST->flowFeatures.pkt_window		= NULL;	
	return ok;
}

/*************************************************************************************************************/

/************************************ update features functions ***************************************************************/

int 
updateFlowFeaturesOTHER(flowID_t flow, const struct pcap_pkthdr *packet)
{
return ok;

}


int 
updateFlowFeaturesUDP(flowID_t flow, const struct pcap_pkthdr *packet, const struct sniff_udp *udp)
{

return ok;

}
			


int 
updateFlowFeaturesTCP(flowID_t flow, const struct pcap_pkthdr *packet, const struct sniff_tcp *tcp)
{

	flowList_t *flowEntry;
	bool find;
	char direction;
	char size_char[10];
	char comma[5] = ",";
	size_t data_len = sizeof(flowEntry->flowFeatures.pkt_window);
	
	flowEntry = findFlow(flow, &find, &direction);
	
	if(!find)
		return erroEmptyPointer;

	data_len = data_len + sizeof(comma);

	sprintf(size_char,"%i",packet->len);

	flowEntry->flowFeatures.pkt_window = realloc(flowEntry->flowFeatures.pkt_window,data_len);
	
	memcpy((char *)flowEntry->flowFeatures.pkt_window + data_len, comma, data_len);
	data_len = data_len + sizeof(size_char);

	if(direction == FORWARD)
	{
		memcpy((char *)flowEntry->flowFeatures.pkt_window + data_len, size_char, data_len);
		return ok;
	}
		
	
	
	memcpy((char *)flowEntry->flowFeatures.pkt_window + data_len, size_char, data_len);

	return ok;		

}


/*************************************************************************************************************/


/****************************************** save flows functions *********************************************/

void
saveAllFlows(void)
{
	while(!empty)
	{
		saveExpiredFlow(LIST_FIRST);
		flowRemove();
	}
	return;
}

/**  rever ordem das características ***/
void 
saveExpiredFlow(flowList_t *flow)
{
	if(!flow->flowHeader.ipSrc[0])
		return;

	/* save features */
	fprintf(FLOWS_RESULTS,"%s,%u,%s,%u,%u,%s\n",

			/********************************** flow header ******************************/

			flow->flowHeader.ipSrc,						       /*  1 */
			flow->flowHeader.portSrc,                                              /*  2 */
			flow->flowHeader.ipDst,						       /*  3 */
			flow->flowHeader.portDst,                                              /*  4 */
			flow->flowHeader.protocol,                                             /*  5 */ 


			/******************************* forward features *****************************/


			flow->flowFeatures.pkt_window);                                                              

	return;
}

/*****************************************************************************************************************/


int 
flowRemove(void)
{
 	flowList_t *aux;
	
	if(!empty)						/* if the FIFO is not empty  the list */
	{

			if(LIST_FIRST->next)			/* the flow is the first element, but have more elements */
			{
				/* remove the flow and bring the second to the top */
				aux = LIST_FIRST;
				LIST_FIRST = LIST_FIRST->next;
				timer = LIST_FIRST->flowHeader.time;
				free(aux);
				return ok;
				
			}
			
			/* list is empty now */	
			LIST_FIRST = LIST_LAST;
			empty = true;				
			timer = BLOCK_TIMER;			/* disable the alarm */
			return ok;


	}
	return erroFlowDontExist;

}


void 
checkExpiredFlows(double currentTime)
{
	while(timer <= currentTime && timer > 0)
	{
		saveExpiredFlow(LIST_FIRST); 					/* save flow features and remove the entry */
		flowRemove();							/* remove flow entry */
	}
	return;
}








void
processPacket (u_char *arg, const struct pcap_pkthdr *pkthdr, const u_char *packet)
{
	
	/* packet headers */
	const struct sniff_ethernet *ethernet; 						/* ETHERNET header */
	const struct sniff_ip *ip; 							/* IP header */
	const struct sniff_tcp *tcp; 							/* TCP header */
	const struct sniff_udp *udp; 							/* UDP header */

	/* current time in microseconds based on the packet header */
	time_t currentTime = pkthdr->ts.tv_sec*FIT_USEC + pkthdr->ts.tv_usec;

	
	
	char direction;
	flowID_t newFlow;

	/* variable to check if the header is valid */	
	int size_ip;
	int size_tcp;
	bool find = false;
	u_short size_udp;

	unsigned short int protocol_check;

	
	ethernet = (struct sniff_ethernet *)(packet);

	/* verifying if the packet is an IP packet */
	if(ethernet->ether_type != IP_TYPE)
		return;

	ip = (struct sniff_ip *)(packet + SIZE_ETHERNET);

	size_ip = IP_HL(ip)*4;

	if(size_ip < 20)
		return;						/* Invalid IP header length */
	


	

	/* determine protocol */
	switch(ip->ip_p)
	{
		case IPPROTO_TCP:
			protocol_check = 1;
			break;
		case IPPROTO_UDP:
			protocol_check = 0;
			break;

		/* not implemented cases */

		case IPPROTO_ICMP:
			return;
		case IPPROTO_IP:
			return;
		default:
			return;

	}
	

	/* set flow IP source and destination */
	strcpy(newFlow.ipSrc,inet_ntoa(ip->ip_src));
	strcpy(newFlow.ipDst,inet_ntoa(ip->ip_dst));


	/* tcp protocol */
	if(protocol_check)
	{
		
		tcp = (struct sniff_tcp *)(packet + SIZE_ETHERNET + size_ip);
		size_tcp = TH_OFF(tcp)*4;
		

		if(size_tcp < 20)
		{
			return;					/* Invalid TCP header length */
		}
		/* mount the flow header */
		newFlow.portDst = ntohs(tcp->th_dport);
		newFlow.portSrc = ntohs(tcp->th_sport);
		newFlow.protocol = 6;
		
		
	
		/* verifyes if the flows already exists */
		findFlow(newFlow, &find, &direction);
		
		/* try to add the flow to the list */
		if(!find)	
		{
			newFlow.time = pkthdr->ts.tv_sec*FIT_USEC + pkthdr->ts.tv_usec + WINDOW_SIZE*FIT_USEC;
			flowAdd(newFlow);
		}
		
		/* search for expired flows */
		checkExpiredFlows((double)currentTime);

		/* update features */
		updateFlowFeaturesTCP(newFlow,pkthdr,tcp);
	
		
		return;
	}

	/* udp protocol */
	if(!protocol_check)
	{
		udp = (struct sniff_udp *)(packet + SIZE_ETHERNET + size_ip);
		size_udp = udp->uh_ulen;
		
		if(size_udp < 8)
			return;				/* Invalid UDP header length */
		
		/* mount the flow header */
		newFlow.portDst = ntohs(udp->uh_dport);
		newFlow.portSrc = ntohs(udp->uh_sport);
		newFlow.protocol = 17;
			

		findFlow(newFlow, &find, &direction);
		
		/* try to add the flow to the list */
		if(!find)	
		{
			newFlow.time = pkthdr->ts.tv_sec*FIT_USEC + pkthdr->ts.tv_usec + WINDOW_SIZE*FIT_USEC;
			flowAdd(newFlow);
		}

		/* update features */
		updateFlowFeaturesUDP(newFlow,pkthdr,udp);
		
		
		/* search for expired flows */	
		checkExpiredFlows((double)currentTime);

		return;
	}

	/* for other cases, only ip features are extracted */
	
	/* mount the flow header */
	newFlow.portDst = 0;
	newFlow.portSrc = 0;
	newFlow.protocol = 1;
		

	findFlow(newFlow, &find, &direction);
	
	/* try to add the flow to the list */
	if(!find)	
	{
		newFlow.time = pkthdr->ts.tv_sec*FIT_USEC + pkthdr->ts.tv_usec + WINDOW_SIZE*FIT_USEC;
		flowAdd(newFlow);
	}

	/* update features */
	updateFlowFeaturesOTHER(newFlow,pkthdr);
	
	
	/* search for expired flows */	
	checkExpiredFlows((double)currentTime);

	return;

}
	


int 
main(int argc, char **argv){
	//int count=0;
	pcap_t *descr = NULL;
    	char errbuf[PCAP_ERRBUF_SIZE];// *device=NULL;
	memset(errbuf,0,PCAP_ERRBUF_SIZE);
	
	FLOWS_RESULTS = fopen(FILE_RESULTS,"a");	
	LIST_LAST = (flowList_t *)(malloc(sizeof(flowList_t))); 
	LIST_LAST->next = NULL;
	LIST_LAST->last = NULL;
	LIST_FIRST = LIST_LAST;

	empty = true;	
	
	/* read from a file */
	if(argc > 1)
	{
		/* open file */
		descr = pcap_open_offline(argv[1],errbuf);

		if(pcap_loop(descr, 0, processPacket, NULL) < 0)
			return true;
		
		saveAllFlows();
		fclose(FLOWS_RESULTS);
		return ok;
	}

	

//	/* Get the name of the first device suitable for capture */
//    	device = pcap_lookupdev(errbuf);
//    	printf("Opening device %s\n", device);
//
//    	/* Open device in promiscuous mode */
//    	descr = pcap_open_live(device, MAXBYTES2CAPTURE,1, 512, errbuf);
//
//    	/* Loop forever &  call processPacket() for every received packet */
//    	pcap_loop(descr, -1, processPacket, (u_char *)&count);    
//	fclose(FLOWS_RESULTS);
	
	return ok;
}




