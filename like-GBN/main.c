#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"event_manager.h"
#include"msg_and_seg.h"
#include"tool.h"
#include"transport_layer.h"


#define BIDIRECTIONAL 0
#define FROM_LAYER5 0
#define FROM_LAYER3 1
#define TIME_INTERRUPT 2
#define TRY_SEND 3
#define TRY_RECEIVE 4
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

unsigned int calculate_check_sum(Segment *seg_ptr);
double update_timeout_interval(Transport_layer_sender *sender);
void generate_future_try_send(Event_who entity);
void generate_future_try_receive(Event_who entity);
void send(Event_who entity ,Transport_layer_sender *sender);
void try_send(Event_who entity);

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
		} else if(eptr->what == TRY_SEND) {
			try_send(eptr->entity);
		} else if(eptr->what == TRY_RECEIVE) {
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
	eptr->time = time + jimsrand();



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
void generate_future_try_send(Event_who entity) {
	printf("generate_future_try_send\n");
	Event *eptr = (Event *)malloc(sizeof(Event));
	eptr->entity = entity;
	eptr->what = TRY_SEND;
	eptr->time = time + jimsrand();
	event_manager->insert(event_manager, eptr);
}

void generate_future_try_receive(Event_who entity) {
	printf("generate_future_receive\n");
	Event *eptr = (Event *)malloc(sizeof(Event));
	eptr->entity = entity;
	eptr->what = TRY_RECEIVE;
	eptr->time = time + jimsrand();
	event_manager->insert(event_manager, eptr);
}
/*************************************************************************************
 *                              transport layer code.                                 *
 *************************************************************************************
 */

Transport_layer_sender *senderA;
Transport_layer_sender *senderB;
typedef unsigned int ui;
typedef unsigned short int usi;

void try_send(Event_who entity) {
	printf("try-send\n");
	if(entity == A) {
		send(entity, senderA);
	} else if(entity == B) {
		send(entity, senderB);
	}
}

void send(Event_who entity ,Transport_layer_sender *sender) {
	printf("send\n");
	if(is_empty(sender)) return;
	if(!sender->is_timing) {
		starttimer(entity, sender->timeout_interval);
		sender->is_timing = 1;
	}
	sender->send_time = time;
	Segment seg;
	seg.seq_num = sender->expected_ack_num + (sender->next - sender->head);
	int i;
	for(i = 0; i < MSG_MAX; ++i) seg.payload[i] = 0;
	for(i = 0; i < MSG_MAX; ++i) {
		if(is_empty(sender) || sender->next == sender->tail) {//have sent all.
			break;
		}
		if(i == sender->rwnd || sender->next == sender->head + sender->max_swnd) {//rwnd is full or swnd is full
			printf("i:%d, other's rwnd:%d, next:%d, head+maxswnd:%d\n", i, sender->rwnd, sender->next,sender->head + sender->max_swnd);
			generate_future_try_send(entity);
			break;
		}
		seg.payload[i] = sender->buffer[senderA->next];
		sender->next += 1;
	}
	sender->rwnd -= i;
	seg.ack_num = sender->ack_num_sent;
	seg.check_sum = calculate_check_sum(&seg);
	tolayer3(A, seg);
}

void A_output(Message msg){
	printf("A-output.\n");
	if(isfull(senderA)) {
		printf("sender buffer is full, so drop this message\n");
		return;
	}
	int i = 0;
	for(i = 0; i < MSG_MAX; ++i) {
		if(isfull(senderA)) {
			printf("sender buffer is full, so drop all or part of this message .\n");
			break;
		} else {
			senderA->buffer[senderA->tail] = msg.data[i];
			senderA->tail += 1;
		}
	}
	send(A, senderA);
	printf("timeout_interval = %lf\n", senderA->timeout_interval);
}

void A_input(Segment seg){
	printf("A_input\n");
	senderA->update_timeout_interval(senderA);
	if(seg.check_sum + ~calculate_check_sum(&seg) != 0xffffffff) {
		printf("error:check sum=%d\n",seg.check_sum + ~calculate_check_sum(&seg));
	}
	if(seg.ack_num > senderA->expected_ack_num) {
		senderA->head += seg.ack_num - senderA->expected_ack_num;
		senderA->expected_ack_num = seg.ack_num;
		senderA->rwnd = seg.rwnd;
		senderA->ack_num_sent = seg.seq_num + MSG_MAX;
		stoptimer(A);
		senderA->is_timing = 0;
		if(senderA->head != senderA->next) {
			starttimer(A, senderA->timeout_interval);
			senderA->is_timing = 1;
		}
	} 
}

unsigned expected_seq;
unsigned seq;
void B_input(Segment seg){
	printf("B_input\n");
	if(seg.seq_num != expected_seq) {
		printf("expect:%u, get %u, seq is not expected, so drop it.\n",expected_seq, seg.seq_num);
		return;
	}
	if(seg.check_sum + ~calculate_check_sum(&seg) != 0xffffffff) {
		printf("check_sum is not correct, so drop it\n");
		return;
	}
	printf("expect:%u, get %u\n",expected_seq, seg.seq_num);
	Message msg;
	Segment echo_seg;
	for(int i = 0; i < MSG_MAX; ++i) {
		msg.data[i] = seg.payload[i];
		echo_seg.payload[i] = seg.payload[i] - 'a' + 'A';
	}
	echo_seg.seq_num = seq;
	echo_seg.rwnd = 2000;
	echo_seg.ack_num = expected_seq + MSG_MAX;
	echo_seg.check_sum = calculate_check_sum(&echo_seg);

	expected_seq = seg.seq_num + MSG_MAX;
	seq = seq + MSG_MAX;
	tolayer3(B, echo_seg);
}
void A_timerinterrupt(){
	printf("A_timerinterrupt\n");
	senderA->next = senderA->head;
	senderA->is_timing = 0;
	try_send(A);
}

void A_init() {
	senderA = Transport_layer_sender_Construct();
	senderA->expected_ack_num = 94;
}
void B_init() {
	expected_seq = 94;
	seq = 217;
}

void B_output(Message msg){
	printf("B-output.\n");
}
void B_timerinterrupt(){
	printf("B_timerinterrupt\n");
}
