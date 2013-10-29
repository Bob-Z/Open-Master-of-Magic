/*
   Open MoM is a free implementation of classical game Mater of Magic.
   Copyright (C) 2013 bobz38@free.fr

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include <stdlib.h>
#include "llist.h"

/* Return the first element of the list */
llist_t * add_llist(llist_t * first,void * data)
{
	llist_t * last;

	if(first==NULL) {
		last=(llist_t*)malloc(sizeof(llist_t));
		last->next=NULL;
		last->data=data;
		return last;
	}

	last=first;

	while(last->next != NULL) {
		last=last->next;
	}

	last->next = (llist_t*)malloc(sizeof(llist_t));
	last = last->next;
	last->data=data;
	last->next=NULL;

	return first;
}

/* Return the first element of the list */
llist_t * del_llist(llist_t * first,void * to_delete)
{
	llist_t * current;
	llist_t * to_remove;

	if(first == NULL) {
		return NULL;
	}

	if( first->data == to_delete ) {
		if(to_delete) {
			free(first->data);
		}
		current = first->next;
		free(first);
		return current;
	}

	current = first;

	while( current->next ) {
		if( current->next->data == to_delete ) {
			to_remove = current->next;
			free(current->next->data);
			current->next = current->next->next;
			free(to_remove);
			return first;
		}

		current = current->next;
	}

	return(first);
}
