#include <stdio.h>
#include <stdlib.h>
#include "datagram.h"

#ifndef INF
#define INF 999
#endif

extern int TRACE;
extern int YES;
extern int NO;
extern int connectcosts[4][4];

static int dv[4][4][4];

void tolayer2(Rtpkt rtpkt);
void createRtpkt(Rtpkt *initRtpkt,int srcid,int destid,int *mincosts);
void rtinit(int flag_entity);
void rtupdate(int flag_entity ,Rtpkt *rcvdpkt);
void printdt(int flag_entity);
void linkhandler(int flag_entity,int linkid, int newcost);


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

int min(int a, int b) {
	if(a < b) return a;
	return b;
}

void rtinit(int flag_entity) {
	printf("rtinit%d\n",flag_entity);
	int i,j;
	for(i = 0; i < 4; ++i) {
		if(i == flag_entity) {
			for(j = 0; j < 4; ++j) {
				dv[flag_entity][i][j] = connectcosts[i][j];
			}
		} else {
			for(j = 0; j < 4; ++j) {
				dv[flag_entity][i][j] = INF;
			}
		}
	}
	for(i = 0; i < 4; ++i) {
		if(connectcosts[flag_entity][i] != INF && i != flag_entity) {
			Rtpkt *rtpkt = (Rtpkt*)malloc(sizeof(Rtpkt));
			createRtpkt(rtpkt, flag_entity, i, dv[flag_entity][flag_entity]);
			tolayer2(*rtpkt);
			free(rtpkt);
		}
	}
}


void rtupdate(int flag_entity ,Rtpkt *rcvdpkt) {
	int i, j;
	i = rcvdpkt->sourceid;
	for(j = 0; j < 4; ++j) {
		dv[flag_entity][i][j] = rcvdpkt->mincost[j];
	}
	int flag = 0;
	for(int j = 0; j < 4; ++j) {
		int Min = INF;
		for(i = 0; i < 4; ++i) {
			Min = min(Min, dv[flag_entity][i][j] + connectcosts[flag_entity][i]);
		}
		if(dv[flag_entity][flag_entity][j] != Min) {
			flag = 1;
		}
		dv[flag_entity][flag_entity][j] = Min;
	}
	if(flag == 1) {
		for(i = 0; i < 4; ++i) {
			if(connectcosts[flag_entity][i] != INF && i != flag_entity) {
				Rtpkt *rtpkt = (Rtpkt*)malloc(sizeof(Rtpkt));
				createRtpkt(rtpkt, flag_entity, i, dv[flag_entity][flag_entity]);
				tolayer2(*rtpkt);
				free(rtpkt);
			}
		}
	}
}


void printdt(int flag_entity) {
	int i,j;
	printf("<%d>DV-table:\n",flag_entity);
	for(i = 0; i < 4; ++i) {
		for(j = 0; j < 4; ++j) {
			printf("%d ",dv[flag_entity][i][j]);
		}
		printf("\n");
	}
}


/* called when cost from 0 to linkid changes from current value to newcost*/
/* You can leave this routine empty if you're an undergrad. If you want */
/* to use this routine, you'll need to change the value of the LINKCHANGE */
/* constant definition in prog3.c from 0 to 1 */
void linkhandler(int flag_entity,int linkid, int newcost) {

}


void rtinit0() {
	rtinit(0);
}
void rtinit1() {
	rtinit(1);
}
void rtinit2() {
	rtinit(2);
}
void rtinit3() {
	rtinit(3);
}

void rtupdate0(Rtpkt *rcvdpkt) {
	rtupdate(0, rcvdpkt);
}
void rtupdate1(Rtpkt *rcvdpkt) {
	rtupdate(1, rcvdpkt);
}
void rtupdate2(Rtpkt *rcvdpkt) {
	rtupdate(2, rcvdpkt);
}
void rtupdate3(Rtpkt *rcvdpkt) {
	rtupdate(3, rcvdpkt);
}

void printdt0() {
	printdt(0);
}
void printdt1() {
	printdt(1);
}
void printdt2() {
	printdt(2);
}
void printdt3() {
	printdt(3);
}

void linkhandler0(int linkid, int newcost) {
	linkhandler(0, linkid, newcost);	
}
void linkhandler1(int linkid, int newcost) {
	linkhandler(1, linkid, newcost);	
}
void linkhandler2(int linkid, int newcost) {
	linkhandler(2, linkid, newcost);	
}
void linkhandler3(int linkid, int newcost) {
	linkhandler(3, linkid, newcost);	
}
