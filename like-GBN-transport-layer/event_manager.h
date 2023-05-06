#ifndef EVENT_MANAGER
#define _EVENT_MANAGER

#include"msg_and_seg.h"


typedef int Event_who;
typedef double Event_time;
typedef int Event_what;
struct Event{//event
	Event_who entity;
	Event_time time;
	Event_what what;
	struct Segment *seg_ptr;
	struct Event *next;
	struct Event *prev;
};
typedef struct Event Event;

struct Event_list{
	Event *head;
	void (*insert)(struct Event_list *el,Event *e);
	void (*delete)(struct Event_list *el, Event *e);
	Event *(*get_event)(struct Event_list *el);
	Event *(*search_by_who_what)(struct Event_list *el, Event_who entity, Event_what event_what);
	void (*show)(struct Event_list *el);
};
typedef struct Event_list Event_list;
typedef struct Event_list Event_manager;

void insert(Event_list *el,Event *e);
void delete(Event_list *el, Event *e);
Event *search_by_who_what(Event_list *el, Event_who entity, Event_what what);
Event *get_event(Event_list *el);
void show(Event_list *el);
Event_manager *Event_manager_Constructor();
Event_list * Event_list_Constructor();

#endif
