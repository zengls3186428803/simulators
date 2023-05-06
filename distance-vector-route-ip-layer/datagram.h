#ifndef _DATAGRAM
#define DATAGRAM


struct Rtpkt {
	int sourceid;       /* id of sending router sending this pkt */
	int destid;         /* id of router to which pkt being sent 
						   (must be an immediate neighbor) */
	int mincost[4];    /* min cost to node 0 ... 3 */
};
typedef struct Rtpkt Rtpkt;

#endif
