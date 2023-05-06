#include"event_manager.h"
#include<stdlib.h>
#include<stdio.h>

Event_manager *Event_manager_Constructor() {
	return Event_list_Constructor();
}

Event_list * Event_list_Constructor() {
	Event_list *event_list_ptr = (Event_list *)malloc(sizeof(Event_list));
	event_list_ptr->head = NULL;
	event_list_ptr->insert = insert;
	event_list_ptr->get_event = get_event;
	event_list_ptr->search_by_who_what = search_by_who_what;
	event_list_ptr->show = show;
	event_list_ptr->delete = delete;
	return event_list_ptr;
}


void insert(Event_list *el,Event *e){
	if(el == NULL) {
		return;
	}
	if(e == NULL) {
		return;
	}
	Event *eptr, *prev_eptr;
	for(eptr = el->head; eptr != NULL; eptr = eptr->next) {
		prev_eptr = eptr;//for condition 4 in insert.
		if(eptr->time > e->time) {
			break;
		}
	}
	if(eptr == el->head && eptr != NULL) {
		el->head = e;
		e->prev = NULL;
		e->next = eptr;
		eptr->prev = e;
	} else if(eptr == el->head && eptr == NULL) {
		el->head = e;
		e->prev = NULL;
		e->next = NULL;
	} else if(eptr != el->head && eptr != NULL) {
		eptr->prev->next = e;
		e->prev = eptr->prev;
		eptr->prev = e;
		e->next = eptr;
	} else if(eptr != el->head && eptr == NULL) {
		prev_eptr->next = e;
		e->prev = prev_eptr;
		e->next = NULL;
	}
}
void delete(Event_list *el, Event *e){
	if(el == NULL || e == NULL) return;
	Event *eptr = e;
	if(eptr->prev != NULL && eptr->next != NULL) {
		eptr->prev->next = eptr->next;
		eptr->next->prev = eptr->prev;
	} else if(eptr->prev != NULL && eptr->next == NULL) {
		eptr->prev->next = NULL;
	} else if(eptr->prev == NULL && eptr->next != NULL) {
		eptr->next->prev = NULL;
		el->head = eptr->next;
	} else if(eptr->prev == NULL && eptr->next == NULL) {
		el->head = NULL;
	}
	free(eptr);
	return;
}
Event *search_by_who_what(Event_list *el, Event_who entity, Event_what what) {
	if(el == NULL) return NULL;
	Event *eptr;
	for(eptr = el->head; eptr!=NULL; eptr = eptr->next) {
		if(eptr->what == what && eptr->entity == entity) {
			return eptr;
		}
	}
	return NULL;
}
Event *get_event(Event_list *el) {
	if(el == NULL) return NULL;
	if(el->head == NULL) return NULL;
	Event *eptr = el->head;
	if(eptr->next != NULL) {
		eptr->next->prev = NULL;
	}	
	el->head = eptr->next;
	return eptr;
}
void show(Event_list *el) {
	if(el == NULL || el->head == NULL) return;
	Event *q;
	printf("--------------\nEvent List Follows:\n");
	for(q = el->head; q!=NULL; q=q->next) {
		printf("Event time: %f, type: %d entity: %d\n",q->time,q->what,q->entity);
	}
	printf("--------------\n");
}
