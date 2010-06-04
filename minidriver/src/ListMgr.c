/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

**************************************************************************** */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "listmgr.h"

/****************************************************************************************************/

//unused
//static int Str_pos(char *p1, char *p2)
//{
//   int iReturn = 0;
//
//   if ( strstr(p1, p2) != NULL )
//   {
//      iReturn=p2 - p1;
//   }
//
//   return (iReturn);
//}

/****************************************************************************************************/

int Init_list (head_ptr)
head_type      *head_ptr ;
/*
+----------------------------------------------------------------------
| ABSTRACT: Initializes an (empty) list head.
|           There is no control to see if the list is empty or not !!
+----------------------------------------------------------------------
| RETURN: none
| INPUT : head_ptr : pointer to list head. (can contain any value)
| OUTPUT: *head_ptr : initialized list head
|                     next_ptr = head_ptr
|                     prev_ptr = head_ptr
|                     curr_ptr = head_ptr
|                     curr_pos = 0
|                     item_cnt = 0
+----------------------------------------------------------------------
*/

{
   head_ptr->prev_ptr   = head_ptr ;
   head_ptr->next_ptr   = head_ptr ;
   head_ptr->curr_ptr   = head_ptr ;
   head_ptr->curr_pos   = 0 ;
   head_ptr->item_count = 0 ;

   return (LM_OK);
}

/****************************************************************************************************/

int Prev_item_in_list (head_ptr)
head_type      *head_ptr ;

/*
+----------------------------------------------------------------------
| ABSTRACT: Moves current pointer to previous item in list.
|           Updates curr_ptr en curr_pos.
|           If on first item the current pointer is moved to the last
|           item of the list (circular list).
+----------------------------------------------------------------------
| RETURN: none
| INPUT : head_ptr : pointer to list head.
|         ->curr_ptr : points to current item
|         ->curr_pos : position in list of current item
| OUTPUT: head_ptr : pointer to list head.
|         ->curr_ptr : points to previous item
|         ->curr_pos : position in list of previous item
+----------------------------------------------------------------------
*/

{
   if ( head_ptr->curr_pos <= 0 )
   {
      return (LM_NOK);
   }

   head_ptr->curr_ptr = head_ptr->curr_ptr->prev_ptr ;
   head_ptr->curr_pos-- ;
   if ( head_ptr->curr_pos == 0 )
   {
      /* move to last item */
      head_ptr->curr_ptr = head_ptr->prev_ptr ;
      head_ptr->curr_pos = head_ptr->item_count ;
   }

   return (LM_OK);
}

/****************************************************************************************************/

int Next_item_in_list (head_ptr)
head_type      *head_ptr ;

/*
+----------------------------------------------------------------------
| ABSTRACT: Moves current pointer to next item in list.
|           Updates curr_ptr en curr_pos.
|           If on last item the current pointer is moved to the first
|           item of the list (circular list).
+----------------------------------------------------------------------
| RETURN: none
| INPUT : head_ptr : pointer to list head.
|         ->curr_ptr : points to current item
|         ->curr_pos : position in list of current item
| OUTPUT: head_ptr : pointer to list head.
|         ->curr_ptr : points to next item
|         ->curr_pos : position in list of next item
+----------------------------------------------------------------------
*/

{
   if ( head_ptr->item_count <= 0 )
   {
      return (LM_NOK);
   }

   head_ptr->curr_ptr = head_ptr->curr_ptr->next_ptr ;
   head_ptr->curr_pos++ ;
   if ( head_ptr->curr_pos > head_ptr->item_count )
   {
      /* move to first item */
      head_ptr->curr_ptr = head_ptr->next_ptr ;
      head_ptr->curr_pos = 1 ;
   }

   return (LM_OK);
}

/****************************************************************************************************/

int Goto_item_in_list (head_ptr, new_pos)
head_type      *head_ptr ;
int             new_pos ;

/*
+----------------------------------------------------------------------
| ABSTRACT: Moves current pointer to given position in list.
|           Updates curr_ptr en curr_pos.
|           The function always selects the fastest positioning method.
|           It always starts at the beginning or the end of the list or
|           from the current position.
+----------------------------------------------------------------------
| RETURN: none
| INPUT : head_ptr : pointer to list head.
| OUTPUT: head_ptr :
|         ->curr_ptr : new position
|         ->curr_pos :
+----------------------------------------------------------------------
*/

{
   int     i ;
   int     offset_from_begin ;
   int     offset_from_end ;
   int     offset_from_current ;

   if ( head_ptr->item_count <= 0 )
   {
      return (LM_NOK);
   }

   if ( new_pos == LAST )
   {
      new_pos = head_ptr->item_count ;
   }

   if ( (new_pos <= 0                  ) ||
        (new_pos > head_ptr->item_count)   )
   {
      return (LM_NOK);
   }

   /* determine best movement */
   offset_from_begin   = new_pos - 1 ;
   offset_from_end     = ITEM_CNT (head_ptr) - new_pos ;
   offset_from_current = new_pos - CURR_POS (head_ptr) ;

   if ( (abs (offset_from_current) <= offset_from_begin) &&
        (abs (offset_from_current) <= offset_from_end  )   )
   {
      /* start from current position */
      if ( offset_from_current >= 0 )
      {
         /* forward */
         for ( i = offset_from_current  ; i > 0 ; i-- )
         {
            Next_item_in_list (head_ptr) ;
         }
      }
      else
      {
         /* backward */
         for ( i = offset_from_current  ; i < 0 ; i++ )
         {
            Prev_item_in_list (head_ptr) ;
         }
      }
   }
   else
   {
      if ( offset_from_begin <= offset_from_end )
      {
         /* start at begin of list */
         head_ptr->curr_ptr = head_ptr->next_ptr ;
         head_ptr->curr_pos = 1 ;
         for ( i = offset_from_begin  ; i > 0 ; i-- )
         {
            Next_item_in_list (head_ptr) ;
         }
      }
      else
      {
         /* start at end of list */
         head_ptr->curr_ptr = head_ptr->prev_ptr ;
         head_ptr->curr_pos = head_ptr->item_count ;
         for ( i = offset_from_end  ; i > 0 ; i-- )
         {
            Prev_item_in_list (head_ptr) ;
         }
      }
   }

   return (LM_NOK);
}

/****************************************************************************************************/

//boolean_type Insert_item_in_list (head_ptr, new_ptr, insert_f)
//head_type       *head_ptr ;
//head_type       *new_ptr ;
//boolean_type    (*insert_f) () ;

/*
+----------------------------------------------------------------------
| ABSTRACT: Inserts item in list. Modifies necessary pointers in item
|           and head.
|           Current pointer and current position are moved to inserted
|           item.
|           This function scans through the list starting at the first
|           item. Every item_ptr is passed to the insert_f() function
|           when insert_f() returns true the given item is inserted in
|           the list before the last scanned item.
+----------------------------------------------------------------------
| RETURN: * true : item inserted succesfully
|         * false: item not inserted (insert_f always returned false)
|                  head remains unchanged.
| INPUT : - new_ptr : points to item to insert.
|         - head_ptr : list header as it is.
|         - insert_f : pointer to insert function.
|                      if NULL pointer -> the item is inserted at
|                                         current position in list
|
|           boolean_type insert_f (application_item_ptr)
|           application_item_type *application_item_ptr ;
|
|           insert_f() must return true if item has to be inserted
|           before application_item_ptr.
| OUTPUT: - head_ptr: curr_ptr points to inserted item.
|                     curr_pos contains position inserted item
|                     item_count nr of items in list
|         Head remains unchanged in case of error (return(FAIL))
+----------------------------------------------------------------------
*/

//{
//   boolean_type    successor_found  = false ;
//   head_type       save_head ;     /* dummy list head. Used for saving
//                                      original list head               */
//   head_type       *pred_ptr ;     /* pointer to predecessor in list   */
//   head_type       *succ_ptr ;     /* pointer to successor   in list   */
//
//   /* First we need to find the successor in the list */
//   if ( head_ptr->item_count == 0 )
//   {
//      /* first item in list */
//      succ_ptr = head_ptr ;
//   }
//   else
//   {
//      /* save original situation */
//      save_head = *head_ptr ;
//
//      if ( insert_f == NULL )
//      {
//    /* insert at current position */
//         succ_ptr = head_ptr->curr_ptr ;
//      }
//      else
//      {
//         /* scan through list and for each item
//            ask function insert_f () if it is the successor */
//         /* start scan at first item in list */
//         head_ptr->curr_pos = 1 ;
//         head_ptr->curr_ptr = head_ptr->next_ptr ;
//
//         while (! successor_found )
//         {
//            /* ask insert_f() if curr_ptr points to successor */
//            if ( (*insert_f) (head_ptr->curr_ptr) )
//            {
//               /* successor found */
//               succ_ptr = head_ptr->curr_ptr ;
//               successor_found = true ;
//            }
//            else
//            {
//               /* first check if end of list reached */
//               if ( head_ptr->curr_pos < head_ptr->item_count )
//               {
//                  /* try next item in list */
//                  Next_item_in_list (head_ptr) ;
//               }
//               else
//               {
//                  /* all items scanned but no successor found
//                     Restore original situation and return error */
//                  *head_ptr = save_head ;
//                  return (false) ;
//               }
//            } /* if insert_f == true */
//         }  /* while */
//      } /* if insert_f == NULL */
//   }
//
//   /*
//   +----------------------------------------------------------------
//   | successor found.
//   | succ_ptr points to successor
//   | new_ptr            new item
//   |
//   | head_ptr->curr_ptr and
//   | head_ptr->curr_pos point to insert position
//   |
//   | Insert new item before successor
//   +----------------------------------------------------------------
//   */
//
//   pred_ptr = succ_ptr->prev_ptr ;  /* predecessor  */
//
//   /* link predecessor with new item */
//   pred_ptr->next_ptr = new_ptr ;
//   new_ptr->prev_ptr  = pred_ptr ;
//
//   /* link successor with new item */
//   succ_ptr->prev_ptr = new_ptr ;
//   new_ptr->next_ptr  = succ_ptr ;
//
//   /* update current pointer and position */
//   head_ptr->curr_ptr = new_ptr ;
//   if ( head_ptr->item_count == 0 )
//   {
//      head_ptr->curr_pos = 1 ;
//   }
//
//   /* update item count */
//   head_ptr->item_count++ ;
//
//   return (true) ;
//}

/****************************************************************************************************/

int Append_item_in_list (head_ptr, new_ptr)
head_type  *head_ptr ;
head_type  *new_ptr ;

/*
+----------------------------------------------------------------------
| ABSTRACT: Appends item in list. Modifies necessary pointers in item
|           and head.
|           Current pointer and current position are moved to inserted
|           item (== last item).
+----------------------------------------------------------------------
| RETURN: none
| INPUT : - new_ptr : points to item to insert.
|         - head_ptr: as it is
| OUTPUT: - head_ptr: curr_ptr points to inserted item.
|                     curr_pos contains position inserted item
|                     item_count nr of items in list
+----------------------------------------------------------------------
*/

{
   head_type  *pred_ptr ;     /* pointer to predecessor in list */
   head_type  *succ_ptr ;     /* pointer to successor   in list */

   succ_ptr = head_ptr ;            /* successor   is list head */
   pred_ptr = succ_ptr->prev_ptr ;  /* predecessor is last item */

   /* link predecessor with new item */
   pred_ptr->next_ptr = new_ptr ;
   new_ptr->prev_ptr  = pred_ptr ;

   /* link successor with new item */
   succ_ptr->prev_ptr = new_ptr ;
   new_ptr->next_ptr  = succ_ptr ;

   /* update item_count */
   head_ptr->item_count++ ;

   /* update current pointer and position */
   head_ptr->curr_ptr = new_ptr ;
   head_ptr->curr_pos = head_ptr->item_count ;

   return (LM_OK);
}

/****************************************************************************************************/

head_type *Remove_item_from_list (head_ptr)
head_type  *head_ptr ;

/*
+----------------------------------------------------------------------
| ABSTRACT: Unlinks current item from list.
|           Updates necessary pointers and counters in head.
|           Current pointer and current position are moved to
|           successor of removed item!
+----------------------------------------------------------------------
| RETURN: - pointer to removed item
| INPUT : - head_ptr : curr_ptr and curr_pos point to item to remove.
| OUTPUT: - head_ptr : curr_ptr points to successor of deleted item.
|                      curr_pos contains successor position
|                      item_count new nr of items in list
|         Head remains unchanged in case of error (abort)
+----------------------------------------------------------------------
*/

{
   head_type  *succ_ptr ;     /* pointer to successor in list */
   head_type  *pred_ptr ;     /* pointer to predecessor in list */
   head_type  *old_ptr ;      /* pointer to removed item        */

   /* check if items in list */
   if ( head_ptr->item_count <= 0 )
   {
      /* fatal error */
      // fatal_err_exit ("Remove_item_from_list () : list empty !") ;
      return (NULL) ; /* sure, only to make lint happy */
   }
   else
   {
      old_ptr  = head_ptr->curr_ptr ;
      succ_ptr = head_ptr->curr_ptr->next_ptr ;
      pred_ptr = head_ptr->curr_ptr->prev_ptr ;

      /* next_ptr in predecessor now points to successor */
      pred_ptr->next_ptr = succ_ptr ;

      /* prev_ptr in successor now points to predecessor */
      succ_ptr->prev_ptr = pred_ptr ;

      /* current pointer now points to successor in list (can be head!)
         current position remains unchanged */
      head_ptr->curr_ptr = succ_ptr ;

      /* item count - 1 */
      head_ptr->item_count-- ;

      /* if current ptr points to head -> skip head */
      if ( head_ptr->curr_ptr == head_ptr )
      {
         /* check for list empty ! (dirty side effect) */
         if ( head_ptr->item_count == 0 )
         {
            Init_list (head_ptr) ;
         }
         else
         {
            Next_item_in_list (head_ptr) ;
         }
      }

      /* return pointer to removed item */
      return (old_ptr) ;
   }
}

/****************************************************************************************************/

char *Allocate_memory_for_list(iMemSize)
int iMemSize;
{
   char *p_cPtr;

   if ( iMemSize < 0 )
   {
      p_cPtr=NULL;
   }

   p_cPtr=malloc(iMemSize * sizeof(char));

   return(p_cPtr);
}

/****************************************************************************************************/

int Destroy_list (head_ptr)
head_type *head_ptr ;

/*
+----------------------------------------------------------------------
| ABSTRACT: Removes all items of a linked list from memory.
|           This function can only be used when the items are allocated
|           dynamically with malloc() !!!!!!!!!!!!!
+----------------------------------------------------------------------
| RETURN  : none
| INPUT : - head_ptr : ptr to list head
| OUTPUT: - *head_ptr : empty list header
+----------------------------------------------------------------------
*/

{
   /* remove all items starting from first item */
   head_ptr->curr_ptr = head_ptr->next_ptr ;
   head_ptr->curr_pos = 1 ;
   while ( head_ptr->item_count > 0 )
   {
      /* unlink item and free allocated memory */
      free ((void *) Remove_item_from_list (head_ptr)) ;
   }

   Init_list (head_ptr) ;

   return (LM_OK);
}

/****************************************************************************************************/

//int Move_list (source_head_ptr, target_head_ptr)
//head_type      *source_head_ptr ;
//head_type      *target_head_ptr ;
//
///*
//+----------------------------------------------------------------------
//| ABSTRACT: Moves all items of a linked list from source_head_ptr to
//|           target_head_ptr.
//|           The target_list must be empty!!!
//|           After the move, the source list is empty.
//+----------------------------------------------------------------------
//| RETURN  : none
//| INPUT : - source_head_ptr : ptr to source list head
//|         - target_head_ptr :        target
//| OUTPUT: - *source_head_ptr : empty list header
//|         - *target_head_ptr : copy of original *source_head_ptr
//+----------------------------------------------------------------------
//*/
//
//{
//   /* check if target list empty */
//   if ( ITEM_CNT (target_head_ptr) > 0 )
//   {
//      return (LM_NOK);
//   }
//
//   /* copy list header */
//   *target_head_ptr = *source_head_ptr ;
//
//   /* link first item with target head */
//   target_head_ptr->next_ptr->prev_ptr = target_head_ptr ;
//
//   /* link last item with target head */
//   target_head_ptr->prev_ptr->next_ptr = target_head_ptr ;
//
//   /* empty source list */
//   Init_list (source_head_ptr) ;
//
//   return (LM_OK);
//}

//Unused
///****************************************************************************************************/
//boolean_type Find_item_in_list (head_ptr, key_ptr, field_f)
//head_type  *head_ptr ;
//char       *key_ptr ;
//char       *(*field_f) () ;
///*
//+----------------------------------------------------------------------
//| ABSTRACT: Find list item with supplied key string.
//|           The search is circular, it starts at curr_ptr and ends
//|           at curr_ptr x.
//+----------------------------------------------------------------------
//| RETURN: * true if item found
//|         * false if not found or list empty.
//| INPUT : - head_ptr  = the pointer to the header of the list.
//|              curr_ptr : ptr to current item
//|              curr_pos : position current item
//|                         (start position for circular search)
//|         - key       = string, containing the search key
//|           The first character of the key can be one
//|           of the following special characters :
//|           '/'  = forward search
//|           '?'  = backward search
//|           '='  = exact match (key and item key must have same length)
//|           '\'  = escape special characters
//|
//|           If no special first character given, the item string
//|           must match the key from the first position of the string.
//|
//|           examples :
//|           key = "abc"  : forward search from current position.
//|                          Item key must start with "abc" and
//|                          followed by 0 or more characters
//|           key = "/abc" : forward search. Item key must contain "abc"
//|                          anywhere in the string.
//|           key = "?abc" : same but backward search.
//|           key = "=abc" : forward search. Item key must be "abc"
//|
//|         - field_f   = a pointer to function, wich returns the key
//|                       string of the list item.
//|
//|                       function syntax :
//|                       char (*item_key_f) (head_ptr)
//|                       head_type *head_ptr ;
//|
//| OUTPUT:   head_ptr = the pointer to the header of the list.
//|              curr_ptr : points to item found
//|              curr_pos : position of item found
//+----------------------------------------------------------------------
//*/
//
//{
//   enum
//      {
//         FORWARD,
//         BACKWARD
//      } direction ;  /* search direction */
//
//   enum
//      {
//         ANY,
//         EXACT,
//         BEGIN
//      } match ;  /* match method */
//
//
//   int            start_pos ; /* position of item where search is started   */
//   int            position ;  /* position of item string in key string      */
//   boolean_type   item_found  = false ;
//
//   head_type      save_head ; /* dummy list head to save original situation */
//
//   /* check if list not empty */
//   if ( head_ptr->item_count <= 0 )
//   {
//      /* no item found */
//      return (false) ;
//   }
//
//   /* check for a /,\ or ? */
//   switch (key_ptr [0])
//   {
//   case '/' :
//      /* forward search */
//      direction = FORWARD ;
//      match     = ANY ;
//      /* remove / from key */
//      key_ptr++ ;
//      break ;
//
//   case '?' :
//      /* backward search */
//      direction = BACKWARD ;
//      match     = ANY ;
//      /* remove ? from key */
//      key_ptr++ ;
//      break ;
//
//   case '=' :
//      /* exact search */
//      direction = FORWARD ;
//      match     = EXACT ;
//      /* remove = from key */
//      key_ptr++ ;
//      break ;
//
//   case '\\' :
//      direction = FORWARD ;
//      match     = BEGIN ;
//      /* skip escape char */
//      key_ptr++ ;
//      break ;
//
//   default :
//      /* the key must match the item_field from the beginning */
//      direction = FORWARD ;
//      match     = BEGIN ;
//      break ;
//   }
//
//   /* save list head original situation */
//   save_head = *head_ptr ;
//
//   /* save start position of search */
//   start_pos = head_ptr->curr_pos ;
//
//   /* start (circular) search from current position
//      continue until item found or at start position */
//   do
//   {
//      /* request item string and try if it fits in key */
//      switch ( match )
//      {
//      case EXACT :
//         if ( strcmp ((*field_f) (head_ptr), key_ptr) == 0 )
//         {
//            item_found = true ;
//         }
//         else
//         {
//            item_found = false ;
//         }
//         break ;
//
//      case BEGIN :
//      case ANY   :
//         /* match is BEGIN or ANY */
//         position = Str_pos ((*field_f) (head_ptr), key_ptr) ;
//         if ( position >= 0 )
//         {
//            item_found = true ;
//            if ( match == BEGIN )
//            {
//               item_found = (position == 0) ;
//            }
//         }
//         break ;
//
//      default :
//         break ;
//      }
//
//      if ( ! item_found )
//      {
//         if ( direction == FORWARD )
//         {
//            Next_item_in_list (head_ptr) ;
//         }
//         else
//         {
//            Prev_item_in_list (head_ptr) ;
//         }
//      }
//   } while ( (head_ptr->curr_pos != start_pos) &&
//             (! item_found)                      ) ;
//
//   if ( item_found )
//   {
//      /* curr_ptr and curr_pos now point to item found */
//      return (true) ;
//   }
//   else
//   {
//      /* nothing found, restore original situation (if possible -- pe) */
//      if (ITEM_CNT(head_ptr) == ITEM_CNT (&save_head))
//      {
//         *head_ptr = save_head ;
//      }
//      return (false) ;
//   }
//}

/****************************************************************************************************/

//static int Quick_sort (head_ptr, sort_f, l, l_ptr, r, r_ptr)
//head_type   *head_ptr;
//int          (*sort_f) ();
//int          l;
//head_type   *l_ptr;
//int          r;
//head_type   *r_ptr;

/*
+----------------------------------------------------------------------
| ABSTRACT:
+----------------------------------------------------------------------
| RETURN  :
| INPUT   :
| OUTPUT  :
+----------------------------------------------------------------------
*/
//{
//   int            i;
//   head_type     *i_ptr;
//   int            j;
//   head_type     *j_ptr;
//   int            m;
//   head_type      tmp;
//
//   i     = l     ;
//   i_ptr = l_ptr ;
//   j     = r     ;
//   j_ptr = r_ptr ;
//   m     = (l + r) / 2 ;  /* The middle element */
//   Goto_item_in_list (head_ptr, m) ;
//
//   do
//   {
//      while ( (*sort_f) (i_ptr, head_ptr->curr_ptr) < 0 )
//      {
//    i     = i + 1 ;
//    i_ptr = i_ptr->next_ptr ;
//      }
//      while ( (*sort_f) (head_ptr->curr_ptr, j_ptr) < 0 )
//      {
//    j     = j - 1 ;
//    j_ptr = j_ptr->prev_ptr ;
//      }
//
//      if ( i < j )
//      {
//    /* swap i_ptr and j_ptr */
//    if ( (j - i) == 1 )
//    {
//       /* Two neighbours have to be exchanged */
//       tmp.prev_ptr = i_ptr->prev_ptr ;
//       tmp.next_ptr = j_ptr->next_ptr ;
//
//       tmp.prev_ptr->next_ptr    = j_ptr ;
//       tmp.next_ptr->prev_ptr    = i_ptr ;
//       i_ptr->prev_ptr           = j_ptr ;
//       i_ptr->next_ptr           = tmp.next_ptr ;
//       j_ptr->prev_ptr           = tmp.prev_ptr ;
//       j_ptr->next_ptr           = i_ptr ;
//    }
//    else
//    {
//       tmp.prev_ptr = i_ptr->prev_ptr ;
//       tmp.next_ptr = i_ptr->next_ptr ;
//
//       tmp.prev_ptr->next_ptr    = j_ptr ;
//       tmp.next_ptr->prev_ptr    = j_ptr ;
//       j_ptr->prev_ptr->next_ptr = i_ptr ;
//       j_ptr->next_ptr->prev_ptr = i_ptr ;
//
//       i_ptr->prev_ptr           = j_ptr->prev_ptr ;
//       i_ptr->next_ptr           = j_ptr->next_ptr ;
//       j_ptr->prev_ptr           = tmp.prev_ptr ;
//       j_ptr->next_ptr           = tmp.next_ptr ;
//    }
//
//         tmp.prev_ptr = j_ptr        ;
//    j_ptr        = i_ptr        ;
//    i_ptr        = tmp.prev_ptr ;
//
//    i     = i + 1           ;
//    i_ptr = i_ptr->next_ptr ;
//    j     = j - 1           ;
//    j_ptr = j_ptr->prev_ptr ;
//      }
//      else if ( i == j )
//      {
//    i     = i + 1           ;
//    i_ptr = i_ptr->next_ptr ;
//    j     = j - 1           ;
//    j_ptr = j_ptr->prev_ptr ;
//      }
//
//   } while ( i <= j ) ;
//
//
//   if ( l < j )
//   {
//      /* Make sure that the header of the linked list is consistent*/
//      head_ptr->curr_ptr = head_ptr->next_ptr ;
//      head_ptr->curr_pos = 1                  ;
//      /*
//      +------------------------------------------------------------
//      | Item "l" may have been swapped, make sure that l_ptr does
//      | point at the l-th element.
//      +------------------------------------------------------------
//      */
//      Goto_item_in_list (head_ptr, l) ;
//      l_ptr = head_ptr->curr_ptr ;
//      Quick_sort (head_ptr, sort_f, l, l_ptr, j, j_ptr) ;
//   }
//
//   if ( i < r )
//   {
//      /* Make sure that the header of the linked list is consistent*/
//      head_ptr->curr_ptr = head_ptr->next_ptr ;
//      head_ptr->curr_pos = 1                  ;
//      /*
//      +------------------------------------------------------------
//      | Item "r" may have been swapped, make sure that r_ptr does
//      | point at the r-th element.
//      +------------------------------------------------------------
//      */
//      Goto_item_in_list (head_ptr, r) ;
//      r_ptr = head_ptr->curr_ptr ;
//      Quick_sort (head_ptr, sort_f, i, i_ptr, r, r_ptr) ;
//   }
//   /* Make sure that the header of the linked list is consistent*/
//   head_ptr->curr_ptr = head_ptr->next_ptr ;
//   head_ptr->curr_pos = 1                  ;
//
//   return (LM_OK);
//}

/****************************************************************************************************/

//int Merge_lists (first_list_head_ptr, second_list_head_ptr)
//head_type    *first_list_head_ptr ;
//head_type    *second_list_head_ptr ;

/*
+----------------------------------------------------------------------
| ABSTRACT: This function merges a second list to a first list AFTER the
|           current position of the first list.
|           If the second list is empty, no action is taken.
|           After the merge, the second header is intialized.
|           For the first header, the current pointer points to the first
|           element of the list (curr_pos = 1)
+----------------------------------------------------------------------
| RETURN  : undefined
| INPUT   : first_list_head_ptr + second_list_head_ptr
| OUTPUT  : first_list_head_ptr  --> second linked list merged
|           second_list_head_ptr --> initialized
+----------------------------------------------------------------------
*/

//{
//   if ( LIST_EMPTY (second_list_head_ptr) )
//   {
//      return(LM_OK) ;
//   }
//
//   second_list_head_ptr->prev_ptr->next_ptr =
//          first_list_head_ptr->curr_ptr->next_ptr ;
//   first_list_head_ptr->curr_ptr->next_ptr->prev_ptr =
//          second_list_head_ptr->prev_ptr ;
//   first_list_head_ptr->curr_ptr->next_ptr =
//          second_list_head_ptr->next_ptr ;
//   second_list_head_ptr->next_ptr->prev_ptr =
//          first_list_head_ptr->curr_ptr ;
//
//   first_list_head_ptr->curr_ptr    = first_list_head_ptr->next_ptr ;
//   first_list_head_ptr->curr_pos    = 1 ;
//   first_list_head_ptr->item_count += second_list_head_ptr->item_count ;
//
//   second_list_head_ptr->prev_ptr   = second_list_head_ptr ;
//   second_list_head_ptr->next_ptr   = second_list_head_ptr ;
//   second_list_head_ptr->curr_ptr   = second_list_head_ptr ;
//   second_list_head_ptr->curr_pos   = 0 ;
//   second_list_head_ptr->item_count = 0 ;
//
//   return (LM_OK);
//}

/****************************************************************************************************/

/*int Sort_list (head_ptr, sort_f)
head_type   *head_ptr ;
int          (*sort_f) () ;
*/
/*
+----------------------------------------------------------------------
| ABSTRACT: Sorts a linked list in memory.
|           The sort-algorithm is the Quicksort.
|
|           The application has to write a comparison function,
|           which is called with two arguments pointing to the
|           elements being compared.  The function must return an
|           integer less than, equal to, or greater than zero as a
|           consequence of whether its first argument is to be
|           considered less than, equal to, or greater than the second.
|           This is the same return convention strcmp uses.
+----------------------------------------------------------------------
| RETURN : none
|
| INPUT  : - head_ptr : ptr to list header
|          - sort_f  : ptr to function that determines if two
|                      elements of the list have to be swapped.
|
| OUTPUT : - head_ptr : after sorting, the current pointer points
|                       to the first element in the list.
+----------------------------------------------------------------------
*/
//
//{
//   head_type     *i_ptr ;
//   head_type     *j_ptr ;
//   int nr_of_items = ITEM_CNT (head_ptr) ;
//
//   if ( nr_of_items > 2 )
//   {
//      Quick_sort (head_ptr, sort_f, 1,
//                  head_ptr->next_ptr,
//                  nr_of_items,
//                  head_ptr->prev_ptr) ;
//
//      return(LM_OK) ;
//   }
//   else if ( nr_of_items == 2 )
//   {
//      Goto_item_in_list (head_ptr, 1) ;
//      i_ptr = head_ptr->next_ptr ;
//      j_ptr = head_ptr->prev_ptr ;
//
//      /* call application-dependent compare-function */
//      if ( (*sort_f) (i_ptr, j_ptr) > 0 )
//      {
//         /* swap i_ptr and j_ptr */
//         j_ptr->next_ptr           = i_ptr ;
//         i_ptr->next_ptr           = i_ptr->prev_ptr ;
//         i_ptr->prev_ptr->next_ptr = j_ptr ;
//
//         j_ptr->prev_ptr           = i_ptr->prev_ptr ;
//         i_ptr->prev_ptr->prev_ptr = i_ptr ;
//         i_ptr->prev_ptr           = j_ptr ;
//
//         head_ptr->curr_ptr        = head_ptr->next_ptr ;
//         head_ptr->curr_pos        = 1 ;
//      }
//      return(LM_OK) ;
//   }
//   else if ( nr_of_items == 1 )
//   {
//      return(LM_OK) ;
//   }
//   else if ( nr_of_items == 0 )
//   {
//      return(LM_OK) ;
//   }
//
//   return (LM_OK);
//}
//
/****************************************************************************************************/

