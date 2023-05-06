#include <stdio.h>
#include <stdlib.h>
#include "tool.h"
#include "event_manager.h"
#include "datagram.h"

#define LINKCHANGES 1 
#define  FROM_LAYER2     2
#define  LINK_CHANGE     0

#define INF 999
int TRACE = 1;             /* for my debugging */
int YES = 1;
int NO = 0;

void createRtpkt(Rtpkt *initRtpkt,int srcid,int destid,int *mincosts) {
	int i;
	initRtpkt->sourceid = srcid;
	initRtpkt->destid = destid;
	for (i=0; i<4; i++)
		initRtpkt->mincost[i] = mincosts[i];
}  

void init();
void rtinit0();
void rtinit1();
void rtinit2();
void rtinit3();
void rtupdate0(Rtpkt *rcvdpkt);
void rtupdate1(Rtpkt *rcvdpkt);
void rtupdate2(Rtpkt *rcvdpkt);
void rtupdate3(Rtpkt *rcvdpkt);
void printdt0();
void printdt1();
void printdt2();
void printdt3();
void linkhandler0(int linkid, int newcost);
void linkhandler1(int linkid, int newcost);
void linkhandler2(int linkid, int newcost);
void linkhandler3(int linkid, int newcost);

double clocktime = 0.000;
Event_manager *event_manager;
int connectcosts[4][4];

int main() {
	int cnt = 0;
	init();
	Event *eptr;
	while (1) {
		if(cnt > 200) break;
		cnt += 1;
		eptr = event_manager->get_event(event_manager);
		if (eptr==NULL)
			goto terminate;
		if (TRACE>1) {
			printf("MAIN: rcv event, t=%.3lf, at %d",
					eptr->time,eptr->entity);
			if (eptr->what == FROM_LAYER2 ) {
				Rtpkt *rtpktptr = (Rtpkt *)eptr->info;
				printf(" src:%2d,",rtpktptr->sourceid);
				printf(" dest:%2d,",rtpktptr->destid);
				printf(" contents: %3d %3d %3d %3d\n", 
						rtpktptr->mincost[0], rtpktptr->mincost[1],
						rtpktptr->mincost[2], rtpktptr->mincost[3]);
			}
		}
		clocktime = eptr->time;    /* update time to next event time */
		if (eptr->what == FROM_LAYER2 ) {
			Rtpkt *rtpktptr = (Rtpkt *)eptr->info;
			if (eptr->entity == 0) 
				rtupdate0(rtpktptr);
			else if (eptr->entity == 1) 
				rtupdate1(rtpktptr);
			else if (eptr->entity == 2) 
				rtupdate2(rtpktptr);
			else if (eptr->entity == 3) 
				rtupdate3(rtpktptr);
			else { printf("Panic: unknown event entity\n"); exit(0); }
		}
		else if (eptr->what == LINK_CHANGE ) {
			if (clocktime<10001.0) {
				linkhandler0(1,20);
				linkhandler1(0,20);
			}
			else   {
				linkhandler0(1,1);
				linkhandler1(0,1);
			}
		}
		else
		{ printf("Panic: unknown event type\n"); exit(0); }
		if (eptr->what == FROM_LAYER2 ) 
			free(eptr->info);        /* free memory for packet, if any */
		free(eptr);                    /* free memory for event   */
	}


terminate:
	printf("\nSimulator terminated at t=%lf, no packets in medium\n", clocktime);
	printdt0();
	printdt1();
	printdt2();
	printdt3();
}



void init() {
	event_manager = Event_manager_Constructor();

	/* initialize by hand since not all compilers allow array initilization */
	connectcosts[0][0]=0;  connectcosts[0][1]=1;  connectcosts[0][2]=3;
	connectcosts[0][3]=7;
	connectcosts[1][0]=1;  connectcosts[1][1]=0;  connectcosts[1][2]=1;
	connectcosts[1][3]=999;
	connectcosts[2][0]=3;  connectcosts[2][1]=1;  connectcosts[2][2]=0;
	connectcosts[2][3]=2;
	connectcosts[3][0]=7;  connectcosts[3][1]=999;  connectcosts[3][2]=2;
	connectcosts[3][3]=0;

	int i;
	double sum, avg;

	printf("Enter TRACE:");
	scanf("%d",&TRACE);

	srand(9999);              /* init random number generator */
	sum = 0.0;                /* test random number generator for students */
	for (i=0; i<1000; i++)
		sum=sum+jimsrand();    /* jimsrand() should be uniform in [0,1] */
	avg = sum/1000.0;
	if (avg < 0.25 || avg > 0.75) {
		printf("It is likely that random number generation on your machine\n" ); 
		printf("is different from what this emulator expects.  Please take\n");
		printf("a look at the routine jimsrand() in the emulator code. Sorry. \n");
		exit(0);
	}

	clocktime=0.0;                /* initialize time to 0.0 */
	rtinit0();
	rtinit1();
	rtinit2();
	rtinit3();

	/* initialize future link changes */
	if (LINKCHANGES==1)   {
		Event *eptr;  
		eptr = (Event *)malloc(sizeof(Event));
		eptr->time =  10000.0;
		eptr->what =  LINK_CHANGE;
		eptr->entity =  -1;
		eptr->info =  NULL;
		event_manager->insert(event_manager, eptr);
		eptr = (Event *)malloc(sizeof(Event));
		eptr->what =  LINK_CHANGE;
		eptr->time =  20000.0;
		eptr->info =  NULL;
		event_manager->insert(event_manager, eptr);
	}
}


/************************** TOLAYER2 ***************/
void tolayer2(Rtpkt packet) {
	Event *eptr, *q;
	double lastime;
	int i;

	/* be nice: check if source and destination id's are reasonable */
	if (packet.sourceid<0 || packet.sourceid >3) {
		printf("WARNING: illegal source id in your packet, ignoring packet!\n");
		return;
	}
	if (packet.destid<0 || packet.destid >3) {
		printf("WARNING: illegal dest id in your packet, ignoring packet!\n");
		return;
	}
	if (packet.sourceid == packet.destid)  {
		printf("WARNING: source and destination id's the same, ignoring packet!\n");
		return;
	}
	if (connectcosts[packet.sourceid][packet.destid] == 999)  {
		printf("WARNING: source and destination not connected, ignoring packet!\n");
		return;
	}

	/* make a copy of the packet student just gave me since he/she may decide */
	/* to do something with the packet after we return back to him/her */ 
	Rtpkt *mypktptr;
	mypktptr = (Rtpkt *) malloc(sizeof(Rtpkt));
	mypktptr->sourceid = packet.sourceid;
	mypktptr->destid = packet.destid;
	for (i=0; i<4; i++)
		mypktptr->mincost[i] = packet.mincost[i];
	if (TRACE>2)  {
		printf("    TOLAYER2: source: %d, dest: %d\n              costs:", 
				mypktptr->sourceid, mypktptr->destid);
		for (i=0; i<4; i++)
			printf("%d  ",mypktptr->mincost[i]);
		printf("\n");
	}

	/* create future event for arrival of packet at the other side */
	eptr = (Event *)malloc(sizeof(Event));
	eptr->what =  FROM_LAYER2;   /* packet will pop out from layer3 */
	eptr->entity = packet.destid; /* event occurs at other entity */
	eptr->info = mypktptr;       /* save ptr to my copy of packet */

	/* finally, compute the arrival time of packet at the other end.
	   medium can not reorder, so make sure packet arrives between 1 and 10
	   time units after the latest arrival time of packets
	   currently in the medium on their way to the destination */
	lastime = clocktime;
	for (q=event_manager->head; q!=NULL ; q = q->next) 
		if ( (q->what==FROM_LAYER2  && q->entity==eptr->entity) ) 
			lastime = q->time;
	eptr->time =  lastime + 2.*jimsrand();
	if (TRACE>2)  
		printf("    TOLAYER2: scheduling arrival on other side\n");
	event_manager->insert(event_manager, eptr);
} 
