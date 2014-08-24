#ifndef _LIST_H_
#define _LIST_H_
/******************************************************************************
 * MODULE NAME:     list.h
 * PROJECT CODE:    __RT7681__
 * DESCRIPTION:
 * DESIGNER:        Charles Su
 * DATE:            Oct 2011
 *
 * SOURCE CONTROL:
 *
 * LICENSE:
 *     This source code is copyright (c) 2011 Ralink Tech. Inc.
 *     All rights reserved.
 *
 * REVISION     HISTORY:
 *   V1.0.0     Oct 2011    - Initial Version V1.0
 *
 *
 * SOURCE:
 * ISSUES:
 *    First Implementation.
 * NOTES TO USERS:
 *
 ******************************************************************************/
#include "types.h"



/******************************************************************************
 * TYPE DEFINITION
 ******************************************************************************/
/* The data structure of LIST ENTRY */
typedef struct LIST_ENTRY_s {

    struct LIST_ENTRY_s * Next;
    struct LIST_ENTRY_s * Prev;

} LIST_ENTRY_t, *pLIST_ENTRY_t;



/******************************************************************************
 * PROTOTYPE
 ******************************************************************************/
VOID apiInitListEntry (IN pLIST_ENTRY_t pEntry);
OUT BOOL apiListIsEmpty (IN pLIST_ENTRY_t pEntry);
VOID apiInsertListEntry (IN pLIST_ENTRY_t pEntry,IN pLIST_ENTRY_t pPrevEntry,IN pLIST_ENTRY_t pNextEntry);
VOID apiInsertHeadEntry (IN pLIST_ENTRY_t pHead,IN pLIST_ENTRY_t pEntry);
VOID apiInsertTailEntry (IN pLIST_ENTRY_t pHead,IN pLIST_ENTRY_t pEntry);
VOID apiRemoveListEntry (IN pLIST_ENTRY_t pEntry);
OUT pLIST_ENTRY_t apiRemoveHeadEntry (IN pLIST_ENTRY_t pHead);
OUT pLIST_ENTRY_t apiRemoveTailEntry (IN pLIST_ENTRY_t pHead);


/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/
/* Simple Doubly Linked List Structures - Entry Part */
typedef struct _LINK_ENTRY_T {
    struct _LINK_ENTRY_T *prNext, *prPrev;
} LINK_ENTRY_T, *P_LINK_ENTRY_T;

/* Simple Doubly Linked List Structures - List Part */
typedef struct _LINK_T {
    P_LINK_ENTRY_T  prNext;
    P_LINK_ENTRY_T  prPrev;
    UINT_32 u4NumElem;
} LINK_T, *P_LINK_T;

/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/

#define LITE_ASSERT ASSERT
#define __KAL_INLINE__          static inline __attribute__((__always_inline__))


#define ENTRY_OF(_addrOfField, _type, _field) \
        ((_type *)((PINT_8)(_addrOfField) - (PINT_8)OFFSET_OF(_type, _field)))


/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/

#define LINK_INITIALIZE(_prLink) \
    do { \
        ((P_LINK_T)(_prLink))->prNext = (P_LINK_ENTRY_T)(_prLink); \
        ((P_LINK_T)(_prLink))->prPrev = (P_LINK_ENTRY_T)(_prLink); \
        ((P_LINK_T)(_prLink))->u4NumElem = 0; \
    } while (0)

#define LINK_ENTRY_INITIALIZE(_prEntry) \
    do { \
        ((P_LINK_ENTRY_T)(_prEntry))->prNext = (P_LINK_ENTRY_T)NULL; \
        ((P_LINK_ENTRY_T)(_prEntry))->prPrev = (P_LINK_ENTRY_T)NULL; \
    } while (0)

#if 0
#define LINK_ENTRY_INVALID(_prEntry) \
    do { \
        ((P_LINK_ENTRY_T)(_prEntry))->prNext = (P_LINK_ENTRY_T)INVALID_LINK_POISON1; \
        ((P_LINK_ENTRY_T)(_prEntry))->prPrev = (P_LINK_ENTRY_T)INVALID_LINK_POISON2; \
    } while (0)
#endif

#define LINK_IS_EMPTY(_prLink)          (((P_LINK_T)(_prLink))->prNext == (P_LINK_ENTRY_T)(_prLink))

/* NOTE: We should do memory zero before any LINK been initiated, so we can check
 * if it is valid before parsing the LINK.
 */
#define LINK_IS_INVALID(_prLink)        (((P_LINK_T)(_prLink))->prNext == (P_LINK_ENTRY_T)NULL)

#define LINK_IS_VALID(_prLink)          (((P_LINK_T)(_prLink))->prNext != (P_LINK_ENTRY_T)NULL)


#define LINK_ENTRY(ptr, type, member)   ENTRY_OF(ptr, type, member)

/* Insert an entry into a link list's head */
#define LINK_INSERT_HEAD(_prLink, _prEntry) \
        { \
            linkAdd(_prEntry, _prLink); \
            ((_prLink)->u4NumElem)++; \
        }


/* Append an entry into a link list's tail */
#define LINK_INSERT_TAIL(_prLink, _prEntry) \
        { \
            linkAddTail(_prEntry, _prLink); \
            ((_prLink)->u4NumElem)++; \
        }

/* Peek head entry, but keep still in link list */
#define LINK_PEEK_HEAD(_prLink, _type, _member) \
        ( \
            LINK_IS_EMPTY(_prLink) ? \
            NULL : LINK_ENTRY((_prLink)->prNext, _type, _member) \
        )

/* Peek tail entry, but keep still in link list */
#define LINK_PEEK_TAIL(_prLink, _type, _member) \
        ( \
            LINK_IS_EMPTY(_prLink) ? \
            NULL : LINK_ENTRY((_prLink)->prPrev, _type, _member) \
        )


/* Get first entry from a link list */
/* NOTE: We assume the link entry located at the beginning of "prEntry Type",
 * so that we can cast the link entry to other data type without doubts.
 * And this macro also decrease the total entry count at the same time.
 */
#define LINK_REMOVE_HEAD(_prLink, _prEntry, _P_TYPE) \
        { \
            LITE_ASSERT(_prLink); \
            if (LINK_IS_EMPTY(_prLink)) { \
                _prEntry = (_P_TYPE)NULL; \
            } \
            else { \
                _prEntry = (_P_TYPE)(((P_LINK_T)(_prLink))->prNext); \
                linkDel((P_LINK_ENTRY_T)(_prEntry)); \
                ((_prLink)->u4NumElem)--; \
            } \
        }

/* Assume the link entry located at the beginning of prEntry Type.
 * And also decrease the total entry count.
 */
#define LINK_REMOVE_KNOWN_ENTRY(_prLink, _prEntry) \
        { \
            LITE_ASSERT(_prLink); \
            LITE_ASSERT(_prEntry); \
            linkDel((P_LINK_ENTRY_T)(_prEntry)); \
            ((_prLink)->u4NumElem)--; \
        }

/* Iterate over a link list */
#define LINK_FOR_EACH(_prEntry, _prLink) \
    for (_prEntry = (_prLink)->prNext; \
         _prEntry != (P_LINK_ENTRY_T)(_prLink); \
         _prEntry = ((P_LINK_ENTRY_T)(_prEntry))->prNext)

/* Iterate over a link list backwards */
#define LINK_FOR_EACH_PREV(_prEntry, _prLink) \
    for (_prEntry = (_prLink)->prPrev; \
         _prEntry != (P_LINK_ENTRY_T)(_prLink); \
         _prEntry = ((P_LINK_ENTRY_T)(_prEntry))->prPrev)

/* Iterate over a link list safe against removal of link entry */
#define LINK_FOR_EACH_SAFE(_prEntry, _prNextEntry, _prLink) \
    for (_prEntry = (_prLink)->prNext, _prNextEntry = (_prEntry)->prNext; \
         _prEntry != (P_LINK_ENTRY_T)(_prLink); \
         _prEntry = _prNextEntry, _prNextEntry = (_prEntry)->prNext)

/* Iterate over a link list of given type */
#define LINK_FOR_EACH_ENTRY(_prObj, _prLink, _rMember, _TYPE) \
    for (_prObj = LINK_ENTRY((_prLink)->prNext, _TYPE, _rMember); \
         &((_prObj)->_rMember) != (P_LINK_ENTRY_T)(_prLink); \
         _prObj = LINK_ENTRY((_prObj)->_rMember.prNext, _TYPE, _rMember))

/* Iterate backwards over a link list of given type */
#define LINK_FOR_EACH_ENTRY_PREV(_prObj, _prLink, _rMember, _TYPE) \
    for (_prObj = LINK_ENTRY((_prLink)->prPrev, _TYPE, _rMember);  \
         &((_prObj)->_rMember) != (P_LINK_ENTRY_T)(_prLink); \
         _prObj = LINK_ENTRY((_prObj)->_rMember.prPrev, _TYPE, _rMember))

/* Iterate over a link list of given type safe against removal of link entry */
#define LINK_FOR_EACH_ENTRY_SAFE(_prObj, _prNextObj, _prLink, _rMember, _TYPE) \
    for (_prObj = LINK_ENTRY((_prLink)->prNext, _TYPE, _rMember),  \
         _prNextObj = LINK_ENTRY((_prObj)->rMember.prNext, _TYPE, _rMember); \
         &((_prObj)->_rMember) != (P_LINK_ENTRY_T)(_prLink); \
         _prObj = _prNextObj, \
         _prNextObj = LINK_ENTRY((_prNextObj)->_rMember.prNext,_TYPE, _rMember))

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/
/*----------------------------------------------------------------------------*/
/*!
* \brief This function is only for internal link list manipulation.
*
* \param[in] prNew  Pointer of new link head
* \param[in] prPrev Pointer of previous link head
* \param[in] prNext Pointer of next link head
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
__KAL_INLINE__ VOID
__linkAdd (
    IN P_LINK_ENTRY_T prNew,
    IN P_LINK_ENTRY_T prPrev,
    IN P_LINK_ENTRY_T prNext
    )
{
    prNext->prPrev = prNew;
    prNew->prNext = prNext;
    prNew->prPrev = prPrev;
    prPrev->prNext = prNew;

    return;
} /* end of __linkAdd() */


/*----------------------------------------------------------------------------*/
/*!
* \brief This function will add a new entry after the specified link head.
*
* \param[in] prNew  New entry to be added
* \param[in] prHead Specified link head to add it after
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
__KAL_INLINE__ VOID
linkAdd (
    IN P_LINK_ENTRY_T prNew,
    IN P_LINK_T prLink
    )
{
    __linkAdd(prNew, (P_LINK_ENTRY_T)prLink, prLink->prNext);

    return;
} /* end of linkAdd() */


/*----------------------------------------------------------------------------*/
/*!
* \brief This function will add a new entry before the specified link head.
*
* \param[in] prNew  New entry to be added
* \param[in] prHead Specified link head to add it before
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
__KAL_INLINE__ VOID
linkAddTail (
    IN P_LINK_ENTRY_T prNew,
    IN P_LINK_T prLink
    )
{
    __linkAdd(prNew, prLink->prPrev, (P_LINK_ENTRY_T)prLink);

    return;
} /* end of linkAddTail() */


/*----------------------------------------------------------------------------*/
/*!
* \brief This function is only for internal link list manipulation.
*
* \param[in] prPrev Pointer of previous link head
* \param[in] prNext Pointer of next link head
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
__KAL_INLINE__ VOID
__linkDel (
    IN P_LINK_ENTRY_T prPrev,
    IN P_LINK_ENTRY_T prNext
    )
{
    prNext->prPrev = prPrev;
    prPrev->prNext = prNext;

    return;
} /* end of __linkDel() */


/*----------------------------------------------------------------------------*/
/*!
* \brief This function will delete a specified entry from link list.
*        NOTE: the entry is in an initial state.
*
* \param prEntry    Specified link head(entry)
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
__KAL_INLINE__ VOID
linkDel (
    IN P_LINK_ENTRY_T prEntry
    )
{
    __linkDel(prEntry->prPrev, prEntry->prNext);

    LINK_ENTRY_INITIALIZE(prEntry);

    return;
} /* end of linkDel() */


/*----------------------------------------------------------------------------*/
/*!
* \brief This function will delete a specified entry from link list and then add it
*        after the specified link head.
*
* \param[in] prEntry        Specified link head(entry)
* \param[in] prOtherHead    Another link head to add it after
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
__KAL_INLINE__ VOID
linkMove (
    IN P_LINK_ENTRY_T prEntry,
    IN P_LINK_T prLink
    )
{
    __linkDel(prEntry->prPrev, prEntry->prNext);
    linkAdd(prEntry, prLink);

    return;
} /* end of linkMove() */


/*----------------------------------------------------------------------------*/
/*!
* \brief This function will delete a specified entry from link list and then add it
*        before the specified link head.
*
* \param[in] prEntry        Specified link head(entry)
* \param[in] prOtherHead    Another link head to add it before
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
__KAL_INLINE__ VOID
linkMoveTail (
    IN P_LINK_ENTRY_T prEntry,
    IN P_LINK_T prLink
    )
{
    __linkDel(prEntry->prPrev, prEntry->prNext);
    linkAddTail(prEntry, prLink);

    return;
} /* end of linkMoveTail() */

#endif /* _LIST_H_ */

