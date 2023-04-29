#ifndef _MSG_AND_SEG
#define _MSG_AND_SEG _NSG_AND_SEG


#define MSG_MAX 20
struct Message{//application layer message
	int len;
	char data[MSG_MAX];
};
typedef struct Message Message;
struct Segment{//transport layer segment
	unsigned int seq_num;
	unsigned int ack_num;
	unsigned int check_sum;
	char payload[MSG_MAX];
};
typedef struct Segment Segment;


#endif
