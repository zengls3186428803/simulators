#include<stdio.h>
#include<stdlib.h>
#include"event_manager.h"



#define BIDIRECTIONAL 0
#define FROM_LAYER5 0
#define FROM_LAYER3 1
#define TIME_INTERRUPT 2
#define A 0
#define B 1

int TRACE = 1;             /* for my debugging */
int nsim = 0;              /* number of messages from 5 to 4 so far */ 
int nsimmax = 0;           /* number of msgs to generate, then stop */
double time = 0.000;
double lossprob;            /* probability that a packet is dropped  */
double corruptprob;         /* probability that one bit is packet is flipped */
double lambda;              /* arrival rate of messages from layer 5 */   
int   ntolayer3;           /* number sent into layer 3 */
int   nlost;               /* number lost in media */
int ncorrupt;              /* number corrupted by media*/


void init();
void A_input(Segment seg);
void A_output(Message msg);
void B_input(Segment seg);;
void B_output(Message msg);
void generate_next_arrival();
void A_init();
void B_init();
void A_timerinterrupt();
void B_timerinterrupt();
double jimsrand();

Event_manager *event_manager;
int main() {
	init();
	A_init();
	B_init();

	Event *eptr;
	Message  msg2give;
	Segment  pkt2give;

	int i,j;
	char c; 
	while(1) {
		printf("current time:%lf\n", time);
		event_manager->show(event_manager);
		Event *eptr = event_manager->get_event(event_manager);
		if(eptr == NULL) {
			goto terminate;
		}
		time = eptr->time;
		if(eptr->what == FROM_LAYER5) {
			generate_next_arrival();   /* set up future arrival */
			/* fill in msg to give with string of same letter */    
			j = nsim % 26; 
			for (i=0; i<MSG_MAX; i++)  
				msg2give.data[i] = 97 + j;
			if (TRACE>2) {
				printf("          MAINLOOP: data given to student: ");
				for (i=0; i<MSG_MAX; i++) 
					printf("%c", msg2give.data[i]);
				printf("\n");
			}
			nsim++;
			if (eptr->entity == A){	
				A_output(msg2give);  
			} else {
				B_output(msg2give);  
			}
			if(nsim >= nsimmax) break;
		} else if(eptr->what == FROM_LAYER3) {
			pkt2give.seq_num = eptr->seg_ptr->seq_num;
			pkt2give.ack_num = eptr->seg_ptr->ack_num;
			pkt2give.check_sum = eptr->seg_ptr->check_sum;
			for (i=0; i<20; i++)  
				pkt2give.payload[i] = eptr->seg_ptr->payload[i];
			if (eptr->entity == A)      /* deliver packet by calling */
				A_input(pkt2give);            /* appropriate entity */
			else
				B_input(pkt2give);
			free(eptr->seg_ptr);          /* free the memory for packet */
		} else if(eptr->what == TIME_INTERRUPT) {
			if (eptr->entity == A) {
				A_timerinterrupt();
			}
			else{
				B_timerinterrupt();
			}
		} else  {
			printf("INTERNAL PANIC: unknown event type \n");
		}
		free(eptr);
	}
terminate:
	printf("routine exit because there is no event.\n");
	return 0;
}



void init(){
	event_manager = Event_manager_Constructor();
	void generate_next_arrival();
	int i;
	double sum, avg;

	printf("-----  Stop and Wait Network Simulator Version 1.1 -------- \n\n");
	printf("Enter the number of messages to simulate: ");
	scanf("%d",&nsimmax);
	printf("Enter  packet loss probability [enter 0.0 for no loss]:");
	scanf("%lf",&lossprob);
	printf("Enter packet corruption probability [0.0 for no corruption]:");
	scanf("%lf",&corruptprob);
	printf("Enter average time between messages from sender's layer5 [ > 0.0]:");
	scanf("%lf",&lambda);
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

	ntolayer3 = 0;
	nlost = 0;
	ncorrupt = 0;

	time=0.0;                    /* initialize time to 0.0 */
	generate_next_arrival();     /* initialize event list */
}

double jimsrand() {
	double mmm = 2147483647;   /* largest int  - MACHINE DEPENDENT!!!!!!!!   */
	double x;                   /* individual students may need to change mmm */ 
	int y = rand();
	x = y/mmm;            /* x should be uniform in [0,1] */
	return x;
}  

void generate_next_arrival(){
	double x;
	if (TRACE>2)
		printf("          GENERATE NEXT ARRIVAL: creating new arrival\n");
	x = lambda*jimsrand()*2;  /* x is uniform on [0,2*lambda] */

	/* having mean of lambda        */
	Event *eptr = (Event *)malloc(sizeof(Event));
	eptr->time =  time + x;
	eptr->what =  FROM_LAYER5;
	if (BIDIRECTIONAL && (jimsrand()>0.5) )
		eptr->entity = B;
	else
		eptr->entity = A;
	event_manager->insert(event_manager, eptr);
}

/********************** Student-callable ROUTINES ***********************/

/* called by students routine to cancel a previously-started timer */
void stoptimer(int AorB) {
	if (TRACE>2)
		printf("          STOP TIMER: stopping timer at %lf\n",time);
	Event *eptr = event_manager->search_by_who_what(event_manager, AorB, TIME_INTERRUPT);
	event_manager->delete(event_manager, eptr);
}


void starttimer(int AorB,double increment){
	if (TRACE>2)
		printf("          START TIMER: starting timer at %lf\n",time);
	/* be nice: check to see if timer is already started, if so, then  warn */
	Event *eptr = event_manager->search_by_who_what(event_manager, AorB, TIME_INTERRUPT);
	if(eptr != NULL) {
		printf("Warning: attempt to start a timer that is already started\n");
		return;
	}
	/* create future event for when timer goes off */
	eptr = (Event *)malloc(sizeof(Event));
	eptr->time =  time + increment;
	eptr->what =  TIME_INTERRUPT;
	eptr->entity = AorB;
	event_manager->insert(event_manager, eptr);
} 


/************************** TOLAYER3 ***************/
void tolayer3(int AorB, Segment packet) {
	Segment *mypktptr;
	double lastime, x, jimsrand();
	int i;


	ntolayer3++;

	/* simulate losses: */
	if (jimsrand() < lossprob)  {
		nlost++;
		if (TRACE>0)    
			printf("          TOLAYER3: packet being lost\n");
		return;
	}  

	/* make a copy of the packet student just gave me since he/she may decide */
	/* to do something with the packet after we return back to him/her */ 
	mypktptr = (Segment *)malloc(sizeof(Segment));
	mypktptr->seq_num = packet.seq_num;
	mypktptr->ack_num = packet.ack_num;
	mypktptr->check_sum = packet.check_sum;
	for (i=0; i<MSG_MAX; i++)
		mypktptr->payload[i] = packet.payload[i];
	if (TRACE>2)  {
		printf("          TOLAYER3: seq: %d, ack %d, check: %d, data: ", mypktptr->seq_num,
				mypktptr->ack_num,  mypktptr->check_sum);
		for (i=0; i<MSG_MAX; i++)
			printf("%c",mypktptr->payload[i]);
		printf("\n");
	}

	Event *eptr,*q;
	/* create future event for arrival of packet at the other side */
	eptr = (Event *)malloc(sizeof(Event));
	eptr->what =  FROM_LAYER3;   /* packet will pop out from layer3 */
	eptr->entity = (AorB+1) % 2; /* event occurs at other entity */
	eptr->seg_ptr = mypktptr;       /* save ptr to my copy of packet */
	/* finally, compute the arrival time of packet at the other end.
	   medium can not reorder, so make sure packet arrives between 1 and 10
	   time units after the latest arrival time of packets
	   currently in the medium on their way to the destination */
	lastime = time;
	/* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next) */
	Event *evlist = event_manager->head;
	for (q=evlist; q!=NULL ; q = q->next) 
		if ( (q->what == FROM_LAYER3  && q->entity == eptr->entity) ) 
			lastime = q->time;
	//eptr->time =  lastime + 1 + 9*jimsrand();
	eptr->time = lastime + jimsrand();



	/* simulate corruption: */
	if (jimsrand() < corruptprob)  {
		ncorrupt++;
		if ( (x = jimsrand()) < .75)
			mypktptr->payload[0]='Z';   /* corrupt payload */
		else if (x < 0.875)
			mypktptr->seq_num = 999999;
		else
			mypktptr->ack_num = 999999;
		if (TRACE>0)    
			printf("          TOLAYER3: packet being corrupted\n");
	}  

	if (TRACE>2)  
		printf("          TOLAYER3: scheduling arrival on other side\n");
	event_manager->insert(event_manager, eptr);
} 

void tolayer5(int AorB,Message msg) {
	int i;  
	if (TRACE>2) {
		printf("          TOLAYER5: data received: ");
		for (i=0; i<20; i++)  
			printf("%c",msg.data[i]);
		printf("\n");
	}
}
/*************************************************************************************
 *                              transport layer code.                                 *
 *************************************************************************************
 interfaces that trsanport layer can user as follow:

 starttimer(calling_entity,increment)
 stoptimer(calling_entity)
 tolayer3(calling_entity,packet)
 tolayer5(calling_entity,message)
 */
struct Transport_layer_sender{
#define BUFFER_SIZE 10000
	Message buffer;
	int wait;
	double estimated_rrt;
	double deviate_rrt;
	double alpha;
	double beta;
	double send_time;
	double timeout_interval;
};
typedef struct Transport_layer_sender Transport_layer_sender;
unsigned int calculate_check_sum(Segment *seg_ptr) {
	unsigned int ans = 0;
	unsigned short x = 0;
	x = x|seg_ptr->seq_num;
	ans += x;
	if(ans>0xffff) ans = ans - 0xffff;//= ans - (0xffff + 1) + 1
	x = 0;
	x = x|(seg_ptr->seq_num >> 16);
	ans += x;
	if(ans>0xffff) ans = ans - 0xffff;
	x = 0;
	for(int i = 0; i < MSG_MAX; ++i) {
		if(i&1) {
			x = (x<<8)|(unsigned short int)seg_ptr->payload[i];
			ans += x;
			if(ans>0xffff) ans = ans - 0xffff;
			x = 0;
		} else {
			x = (unsigned short int)seg_ptr->payload[i];
		}
	}
	return ~ans;
}

double update_timeout_interval(Transport_layer_sender *sender) {
	double cur_rrt = time - sender->send_time;
	double cur_dev = cur_rrt - sender->estimated_rrt;
	if(cur_dev < 0) cur_dev = -cur_dev;
	sender->deviate_rrt = (1 - sender->beta) * sender->deviate_rrt + sender->beta * cur_dev;
	sender->estimated_rrt = (1 - sender->alpha) * sender->estimated_rrt + sender->alpha * cur_rrt;
	double timeout_interval = sender->estimated_rrt +  4 * sender->deviate_rrt;
	sender->timeout_interval = timeout_interval;
}

unsigned int calculate_check_sum(Segment *seg_ptr);
Transport_layer_sender senderA;
Transport_layer_sender senderB;

unsigned int A_seq_num;
unsigned int B_seq_num;
typedef unsigned int ui;
typedef unsigned short int usi;
void A_init() {
	senderA.alpha = 0.125;
	senderA.beta = 0.25;
	senderA.estimated_rrt = 1;
	senderA.deviate_rrt = 0;
	senderA.timeout_interval = 1.5;
	A_seq_num = 0;
}
void B_init() {
	senderB.alpha = 0.125;
	senderB.beta = 0.25;
	senderB.estimated_rrt = 1;
	senderB.deviate_rrt = 0;
	senderB.timeout_interval = 1.5;
	B_seq_num = 0;
}
void A_output(Message msg){
	if(senderA.wait == 1) {
		printf("wait echo, so drop this message.\n");
		return;
	}
	senderA.buffer = msg;
	senderA.wait = 1;
	printf("A-output.\n");
	Segment seg;
	int i = 0;
	for(i=0; i < MSG_MAX; ++i) seg.payload[i] = msg.data[i];
	seg.seq_num = A_seq_num;
	seg.ack_num = B_seq_num;
	seg.check_sum = calculate_check_sum(&seg);
	tolayer3(A, seg);
	starttimer(A, senderA.timeout_interval);
	senderA.send_time = time;
	printf("timeout_interval = %lf\n", senderA.timeout_interval);
}
void B_output(Message msg){
	printf("B-output.\n");
	Segment seg;
	int i = 0;
	for(i=0; i < MSG_MAX; ++i) seg.payload[i] = msg.data[i];
	seg.seq_num = B_seq_num;
	seg.ack_num = A_seq_num;
	seg.check_sum = calculate_check_sum(&seg);
	tolayer3(B, seg);
	starttimer(B, senderB.timeout_interval);
	senderB.send_time = time;
}
void A_input(Segment seg){
	printf("A_input\n");
	update_timeout_interval(&senderA);
	ui check_sum = calculate_check_sum(&seg);
	if(~check_sum + seg.check_sum != 0xffffffff) {
		printf("error:check sum = %X\nso, drop this segment\n",check_sum + seg.check_sum);
		return;
	}
	printf("check sum is corect.\n");
	senderA.wait = 0;
	stoptimer(A);
	Message msg;
	for(int i = 0; i < MSG_MAX; ++i) msg.data[i] = seg.payload[i];
	tolayer5(A, msg);
}
void B_input(Segment seg){
	printf("B_input\n");
	update_timeout_interval(&senderB);
	ui check_sum = calculate_check_sum(&seg);
	if(~check_sum + seg.check_sum != 0xffffffff) {
		printf("error:check sum = %X\nso, drop this segment\n",check_sum + seg.check_sum);
		return;
	}
	printf("check sum is corect.\n");
	Message msg;
	int i;
	for(i = 0; i < MSG_MAX; ++i) msg.data[i] = seg.payload[i];
	tolayer5(B, msg);

	Segment echo_seg;
	echo_seg.ack_num;
	B_seq_num = (B_seq_num + 1) % 2;
	echo_seg.seq_num = B_seq_num;
	for(i = 0; i < MSG_MAX; ++i) echo_seg.payload[i] = seg.payload[i] - 'a' + 'A';
	echo_seg.check_sum = calculate_check_sum(&echo_seg);
	tolayer3(B, echo_seg);
}
void A_timerinterrupt(){
	printf("A_timerinterrupt\n");
	senderA.wait = 0;
	A_output(senderA.buffer);
}
void B_timerinterrupt(){
	printf("B_timerinterrupt\n");
	senderB.wait = 0;
	B_output(senderB.buffer);
}
