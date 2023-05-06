#include"transport_layer.h"
#include"msg_and_seg.h"
#include<stdlib.h>

extern double time;
double update_timeout_interval(Transport_layer_sender *sender) {
	double cur_rrt = time - sender->send_time;
	double cur_dev = cur_rrt - sender->estimated_rrt;
	if(cur_dev < 0) cur_dev = -cur_dev;
	sender->deviate_rrt = (1 - sender->beta) * sender->deviate_rrt + sender->beta * cur_dev;
	sender->estimated_rrt = (1 - sender->alpha) * sender->estimated_rrt + sender->alpha * cur_rrt;
	double timeout_interval = sender->estimated_rrt +  4 * sender->deviate_rrt;
	sender->timeout_interval = timeout_interval;
}


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


Bool isfull(Transport_layer_sender *sender) {
	if((sender->tail + 1) % sender->buffer_size == sender->head) return True;
	return False;
}
Bool is_empty(Transport_layer_sender *sender) {
	if(sender->tail == sender->head) return True;
	return False;
}
Transport_layer_sender *Transport_layer_sender_Construct() {
	Transport_layer_sender *sender = (Transport_layer_sender *)malloc(sizeof(Transport_layer_sender));
	sender->alpha = 0.125;
	sender->beta = 0.25;
	sender->estimated_rrt = 1;
	sender->deviate_rrt = 0;
	sender->buffer_size = BUFFER_SIZE;
	sender->timeout_interval = 1.5;
	sender->rwnd = BUFFER_SIZE/3;
	sender->is_timing = False;
	sender->next = sender->head = sender->tail = 0;
	sender->max_swnd = BUFFER_SIZE/3;
	//sender->calculate_check_sum = calculate_check_sum;
	sender->update_timeout_interval = update_timeout_interval;
	sender->isfull = isfull;
	sender->is_empty = is_empty;
	return sender;
}
