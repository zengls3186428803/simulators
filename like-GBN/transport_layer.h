#ifndef _TRANSPORT_LAYER
#define _TRANSPORT_LAYER
#include"msg_and_seg.h"

typedef char Byte;
typedef int Bool;
#define True 1
#define False 0
#define BUFFER_SIZE 1999999
struct Transport_layer_sender{
	Byte buffer[BUFFER_SIZE];
	int buffer_size;
	int head;
	int tail;
	int next;

	int rwnd;//receive window
	int max_swnd;//max value of sender receive.
	unsigned int expected_ack_num;
	unsigned int next_send_num;
	unsigned int ack_num_sent;

	int is_timing;

	double estimated_rrt;
	double deviate_rrt;
	double alpha;
	double beta;
	double send_time;
	double timeout_interval;

	Bool (*isfull)(struct Transport_layer_sender *sender);
	Bool (*is_empty)(struct Transport_layer_sender *sender);
	//unsigned int (*calculate_check_sum)(Segment *seg_ptr);
	double (*update_timeout_interval)(struct Transport_layer_sender *sender);
};
typedef struct Transport_layer_sender Transport_layer_sender;


Transport_layer_sender *Transport_layer_sender_Construct();
Bool isfull(Transport_layer_sender *sender);
Bool is_empty(Transport_layer_sender *sender);
unsigned int calculate_check_sum(Segment *seg_ptr);
double update_timeout_interval(Transport_layer_sender *sender);



#endif
