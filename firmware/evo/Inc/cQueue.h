/*!\file cQueue.h
** \author SMFSW
** \copyright BSD 3-Clause License (c) 2017-2024, SMFSW
** \brief Queue handling library (written in plain c)
** \details Queue handling library (written in plain c)
**/
/****************************************************************/
#ifndef CQUEUE_H_
	#define CQUEUE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
/****************************************************************/


#define q_init_def(q, sz)	q_init(q, sz, 20, FIFO, false)	//!< Some kind of default behavior for queue initialization

#define q_pull				q_pop							//!< \deprecated q_pull was already used in cQueue lib, alias is made to keep compatibility with earlier versions
#define q_nbRecs			q_getCount						//!< \deprecated q_nbRecs was already used in cQueue lib, alias is made to keep compatibility with earlier versions
#define q_clean				q_flush							//!< \deprecated q_clean was already used in cQueue lib, alias is made to keep compatibility with earlier versions


/*!\enum enumQueueType
** \brief Queue behavior enumeration (FIFO, LIFO)
**/
typedef enum enumQueueType {
	FIFO = 0,	//!< First In First Out behavior
	LIFO = 1	//!< Last In First Out behavior
} QueueType;


/*!\struct Queue_t
** \brief Queue type structure holding all variables to handle the queue
**/
typedef struct Queue_t {
	QueueType	impl;		//!< Queue implementation: FIFO LIFO
	bool		ovw;		//!< Overwrite previous records when queue is full allowed
	bool		dynamic;	//!< Set to true when queue is dynamically allocated
	size_t		queue_sz;	//!< Size of the full queue
	size_t		rec_sz;		//!< Size of a record
	uint16_t	rec_nb;		//!< number of records in the queue
	uint8_t *	queue;		//!< Queue start pointer (when allocated)

	uint16_t	in;			//!< number of records pushed into the queue
	uint16_t	out;		//!< number of records pulled from the queue (only for FIFO)
	uint16_t	cnt;		//!< number of records not retrieved from the queue
	uint16_t	init;		//!< set to QUEUE_INITIALIZED after successful init of the queue and reset when killing queue
} Queue_t;


/*****************/
/*** FUNCTIONS ***/
/*****************/
/*!	\brief Queue initialization (using dynamic queue allocation)
**	\param [in,out] pQ - pointer of queue to handle
**	\param [in] size_rec - size of a record in the queue (in bytes)
**	\param [in] nb_recs - number of records in the queue
**	\param [in] type - Queue implementation type: FIFO, LIFO
**	\param [in] overwrite - Overwrite previous records when queue is full
**	\return NULL when allocation not possible, Queue tab address when successful
**/
void * __attribute__((nonnull)) q_init(	Queue_t * const pQ,
										const size_t size_rec, const uint16_t nb_recs,
										const QueueType type, const bool overwrite);

/*!	\brief Queue initialization (using static queue)
**	\param [in,out] pQ - pointer of queue to handle
**	\param [in] size_rec - size of a record in the queue (in bytes)
**	\param [in] nb_recs - number of records in the queue
**	\param [in] type - Queue implementation type: FIFO, LIFO
**	\param [in] overwrite - Overwrite previous records when queue is full
**	\param [in] pQDat - Pointer to static data queue
**	\param [in] lenQDat - Length of static data queue (in bytes) for static array size check against required size for queue
**	\return Queue tab address (to remain consistent with \ref q_init)
**/
void * __attribute__((nonnull(1))) q_init_static(	Queue_t * const pQ,
													const size_t size_rec, const uint16_t nb_recs,
													const QueueType type, const bool overwrite,
													void * const pQDat, const size_t lenQDat);

/*!	\brief Queue destructor: release dynamically allocated queue
**	\param [in,out] pQ - pointer of queue to handle
**/
void __attribute__((nonnull)) q_kill(Queue_t * const pQ);

/*!	\brief Flush queue, restarting from empty queue
**	\param [in,out] pQ - pointer of queue to handle
**/
void __attribute__((nonnull)) q_flush(Queue_t * const pQ);

/*!	\brief Push record to queue
**	\warning If using q_push, q_pop, q_peek, q_drop, q_peekItem and/or q_peekPrevious in both interrupts and main application,
**				you shall disable interrupts in main application when using these functions
**	\param [in,out] pQ - pointer of queue to handle
**	\param [in] record - pointer to record to be pushed into queue
**	\return Push status
**	\retval true if successfully pushed into queue
**	\retval false if queue is full
**/
bool __attribute__((nonnull)) q_push(Queue_t * const pQ, const void * const record);

/*!	\brief Pop record from queue
**	\warning If using q_push, q_pop, q_peek, q_drop, q_peekItem and/or q_peekPrevious in both interrupts and main application,
**				you shall disable interrupts in main application when using these functions
**	\param [in] pQ - pointer of queue to handle
**	\param [in,out] record - pointer to record to be popped from queue
**	\return Pop status
**	\retval true if successfully pulled from queue
**	\retval false if queue is empty
**/
bool __attribute__((nonnull)) q_pop(Queue_t * const pQ, void * const record);

/*!	\brief Peek record from queue
**	\warning If using q_push, q_pop, q_peek, q_drop, q_peekItem and/or q_peekPrevious in both interrupts and main application,
**				you shall disable interrupts in main application when using these functions
**	\note This function is most likely to be used in conjunction with q_drop
**	\param [in] pQ - pointer of queue to handle
**	\param [in,out] record - pointer to record to be peeked from queue
**	\return Peek status
**	\retval true if successfully peeked from queue
**	\retval false if queue is empty
**/
bool __attribute__((nonnull)) q_peek(const Queue_t * const pQ, void * const record);

/*!	\brief Drop current record from queue
**	\warning If using q_push, q_pop, q_peek, q_drop, q_peekItem and/or q_peekPrevious in both interrupts and main application,
**				you shall disable interrupts in main application when using these functions
**	\note This function is most likely to be used in conjunction with q_peek
**	\param [in,out] pQ - pointer of queue to handle
**	\return drop status
**	\retval true if successfully dropped from queue
**	\retval false if queue is empty
**/
bool __attribute__((nonnull)) q_drop(Queue_t * const pQ);

/*!	\brief Peek record at index from queue
**	\warning If using q_push, q_pop, q_peek, q_drop, q_peekItem and/or q_peekPrevious in both interrupts and main application,
**				you shall disable interrupts in main application when using these functions
**	\note This function is only useful if searching for a duplicate record and shouldn't be used in conjunction with q_drop
**	\param [in] pQ - pointer of queue to handle
**	\param [in,out] record - pointer to record to be peeked from queue
**	\param [in] idx - index of the record to pick
**	\return Peek status
**	\retval true if successfully peeked from queue
**	\retval false if index is out of range
**/
bool __attribute__((nonnull)) q_peekIdx(const Queue_t * const pQ, void * const record, const uint16_t idx);

/*!	\brief Peek previous record from queue
**	\warning If using q_push, q_pop, q_peek, q_drop, q_peekItem and/or q_peekPrevious in both interrupts and main application,
**				you shall disable interrupts in main application when using these functions
**	\note This inline is only useful with FIFO implementation, use q_peek instead with a LIFO (will lead to the same result)
**	\param [in] pQ - pointer of queue to handle
**	\param [in,out] record - pointer to record to be peeked from queue
**	\return Peek status
**	\retval true if successfully peeked from queue
**	\retval false if queue is empty
**/
bool __attribute__((nonnull)) q_peekPrevious(const Queue_t * const pQ, void * const record);


/***************/
/*** GETTERS ***/
/***************/
/*!	\brief get initialization state of the queue
**	\param [in] pQ - pointer of queue to handle
**	\return Queue initialization status
**	\retval true if queue is allocated
**	\retval false is queue is not allocated
**/
bool __attribute__((nonnull)) q_isInitialized(const Queue_t * const pQ);

/*!	\brief get size of queue
**	\remark Size in bytes (like sizeof)
**	\param [in] pQ - pointer of queue to handle
**	\return Size of queue in bytes
**/
uint32_t __attribute__((nonnull)) q_sizeof(const Queue_t * const pQ);

/*!	\brief get emptiness state of the queue
**	\param [in] pQ - pointer of queue to handle
**	\return Queue emptiness status
**	\retval true if queue is empty
**	\retval false is not empty
**/
bool __attribute__((nonnull)) q_isEmpty(const Queue_t * const pQ);

/*!	\brief get fullness state of the queue
**	\param [in] pQ - pointer of queue to handle
**	\return Queue fullness status
**	\retval true if queue is full
**	\retval false is not full
**/
bool __attribute__((nonnull)) q_isFull(const Queue_t * const pQ);

/*!	\brief get number of records in the queue
**	\param [in] pQ - pointer of queue to handle
**	\return Number of records stored in the queue
**/
uint16_t __attribute__((nonnull)) q_getCount(const Queue_t * const pQ);

/*!	\brief get number of records left in the queue
**	\param [in] pQ - pointer of queue to handle
**	\return Number of records left in the queue
**/
uint16_t __attribute__((nonnull)) q_getRemainingCount(const Queue_t * const pQ);


/****************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* CQUEUE_H_ */
/****************************************************************/
