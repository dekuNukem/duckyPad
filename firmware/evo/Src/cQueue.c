/*!\file cQueue.c
** \author SMFSW
** \copyright BSD 3-Clause License (c) 2017-2024, SMFSW
** \brief Queue handling library (written in plain c)
** \details Queue handling library (written in plain c)
**/
/****************************************************************/
#include <string.h>
#include <stdlib.h>

#include "cQueue.h"
/****************************************************************/


#define QUEUE_INITIALIZED	0x5AA5U		//!< Initialized queue control value


/**************************/
/*** INTERNAL FUNCTIONS ***/
/**************************/
/*!	\brief Increment index
**	\details Increment buffer index \b pIdx rolling back to \b start when limit \b end is reached
**	\param [in,out] pIdx - pointer to index value
**	\param [in] end - counter upper limit value
**	\param [in] start - counter lower limit value
**/
static inline void __attribute__((nonnull, always_inline)) _inc_idx(uint16_t * const pIdx, const uint16_t end, const uint16_t start)
{
	if (*pIdx < (end - 1U))		{ (*pIdx)++; }
	else						{ *pIdx = start; }
}

/*!	\brief Decrement index
**	\details Decrement buffer index \b pIdx rolling back to \b end when limit \b start is reached
**	\param [in,out] pIdx - pointer to index value
**	\param [in] end - counter upper limit value
**	\param [in] start - counter lower limit value
**/
static inline void __attribute__((nonnull, always_inline)) _dec_idx(uint16_t * const pIdx, const uint16_t end, const uint16_t start)
{
	if (*pIdx > start)		{ (*pIdx)--; }
	else					{ *pIdx = end - 1U; }
}


/*!	\brief get initialization state of the queue
**	\param [in] pQ - pointer of queue to handle
**	\return Queue initialization status
**	\retval true if queue is allocated
**	\retval false is queue is not allocated
**/
static inline bool __attribute__((nonnull, always_inline)) _q_isInitialized(const Queue_t * const pQ) {
	return (pQ->init == QUEUE_INITIALIZED) ? true : false; }

/*!	\brief get emptiness state of the queue
**	\param [in] pQ - pointer of queue to handle
**	\return Queue emptiness status
**	\retval true if queue is empty
**	\retval false is not empty
**/
static inline bool __attribute__((nonnull, always_inline)) _q_isEmpty(const Queue_t * const pQ) {
	return (pQ->cnt == 0U) ? true : false; }

/*!	\brief get fullness state of the queue
**	\param [in] pQ - pointer of queue to handle
**	\return Queue fullness status
**	\retval true if queue is full
**	\retval false is not full
**/
static inline bool __attribute__((nonnull, always_inline)) _q_isFull(const Queue_t * const pQ) {
	return (pQ->cnt == pQ->rec_nb) ? true : false; }


/*!	\brief get number of records in the queue
**	\param [in] pQ - pointer of queue to handle
**	\return Number of records stored in the queue
**/
static inline uint16_t __attribute__((nonnull, always_inline)) _q_getCount(const Queue_t * const pQ) {
	return pQ->cnt; }


/************************/
/*** PUBLIC FUNCTIONS ***/
/************************/
void * __attribute__((nonnull)) q_init(	Queue_t * const pQ,
										const size_t size_rec, const uint16_t nb_recs,
										const QueueType type, const bool overwrite)
{
	q_kill(pQ);	// De-Init queue (if previously initialized) and set structure to 0 to ensure proper functions behavior when queue is not allocated

	const size_t size = nb_recs * size_rec;

	pQ->queue = (uint8_t *) malloc(size);

	if (pQ->queue != NULL)
	{
		pQ->dynamic = true;

		pQ->queue_sz = size;
		pQ->rec_sz = size_rec;
		pQ->rec_nb = nb_recs;
		pQ->impl = type;
		pQ->ovw = overwrite;

		pQ->init = QUEUE_INITIALIZED;
	}

	return pQ->queue;	// return NULL when queue not properly allocated, Queue data address otherwise
}

void * __attribute__((nonnull(1))) q_init_static(	Queue_t * const pQ,
													const size_t size_rec, const uint16_t nb_recs,
													const QueueType type, const bool overwrite,
													void * const pQDat, const size_t lenQDat)
{
	q_kill(pQ);	// De-Init queue (if previously initialized) and set structure to 0 to ensure proper functions behavior when queue is not allocated

	const size_t size = nb_recs * size_rec;

	if ((pQDat != NULL) && (lenQDat >= size))	// Check static Queue pointer and data size
	{
		pQ->queue = (uint8_t *) pQDat;
		pQ->queue_sz = size;
		pQ->rec_sz = size_rec;
		pQ->rec_nb = nb_recs;
		pQ->impl = type;
		pQ->ovw = overwrite;

		pQ->init = QUEUE_INITIALIZED;
	}

	return pQ->queue;	// return NULL when queue not properly allocated, Queue data address otherwise
}

void __attribute__((nonnull)) q_kill(Queue_t * const pQ)
{
	if (_q_isInitialized(pQ) && pQ->dynamic && (pQ->queue != NULL))	{ free(pQ->queue); }	// Free existing data (if already dynamically initialized)
	memset(pQ, 0, sizeof(Queue_t));
}


void __attribute__((nonnull)) q_flush(Queue_t * const pQ)
{
	pQ->in = 0;
	pQ->out = 0;
	pQ->cnt = 0;
}


bool __attribute__((nonnull)) q_push(Queue_t * const pQ, const void * const record)
{
	bool ret = true;

	if (_q_isFull(pQ))		// No more records available
	{
		if (pQ->ovw)		// Queue is full, overwrite is allowed
		{
			if (pQ->impl == FIFO)
			{
				_inc_idx(&pQ->out, pQ->rec_nb, 0);	// as oldest record is overwritten, increment out
			}
			//else if (pQ->impl == LIFO)	{}		// Nothing to do in this case
		}
		else
		{
			ret = false;
		}
	}
	else
	{
		pQ->cnt++;	// Increase records count
	}

	if (ret)
	{
		uint8_t * const pStart = pQ->queue + (pQ->rec_sz * pQ->in);
		memcpy(pStart, record, pQ->rec_sz);
		_inc_idx(&pQ->in, pQ->rec_nb, 0);
	}

	return ret;
}

bool __attribute__((nonnull)) q_pop(Queue_t * const pQ, void * const record)
{
	bool ret = true;

	if (_q_isEmpty(pQ))		// No records
	{
		ret = false;
	}
	else
	{
		const uint8_t * pStart;

		if (pQ->impl == FIFO)
		{
			pStart = pQ->queue + (pQ->rec_sz * pQ->out);
			_inc_idx(&pQ->out, pQ->rec_nb, 0);
		}
		else /* if (pQ->impl == LIFO) */
		{
			_dec_idx(&pQ->in, pQ->rec_nb, 0);
			pStart = pQ->queue + (pQ->rec_sz * pQ->in);
		}

		memcpy(record, pStart, pQ->rec_sz);
		pQ->cnt--;	// Decrease records count
	}

	return ret;
}

bool __attribute__((nonnull)) q_peek(const Queue_t * const pQ, void * const record)
{
	bool ret = true;

	if (_q_isEmpty(pQ))		// No records
	{
		ret = false;
	}
	else
	{
		const uint8_t * pStart;

		if (pQ->impl == FIFO)
		{
			pStart = pQ->queue + (pQ->rec_sz * pQ->out);
			// No change on out var as it's just a peek
		}
		else /* if (pQ->impl == LIFO) */
		{
			uint16_t rec = pQ->in;	// Temporary var for peek (no change on pQ->in with dec_idx)
			_dec_idx(&rec, pQ->rec_nb, 0);
			pStart = pQ->queue + (pQ->rec_sz * rec);
		}

		memcpy(record, pStart, pQ->rec_sz);
	}

	return ret;
}

bool __attribute__((nonnull)) q_drop(Queue_t * const pQ)
{
	bool ret = true;

	if (_q_isEmpty(pQ))		// No records
	{
		ret = false;
	}
	else
	{
		if (pQ->impl == FIFO)
		{
			_inc_idx(&pQ->out, pQ->rec_nb, 0);
		}
		else /*if (pQ->impl == LIFO)*/
		{
			_dec_idx(&pQ->in, pQ->rec_nb, 0);
		}

		pQ->cnt--;	// Decrease records count
	}

	return ret;
}

bool __attribute__((nonnull)) q_peekIdx(const Queue_t * const pQ, void * const record, const uint16_t idx)
{
	bool ret = true;

	if ((idx + 1U) > _q_getCount(pQ))	// Index out of range
	{
		ret = false;
	}
	else
	{
		const uint8_t * pStart;

		if (pQ->impl == FIFO)
		{
			pStart = pQ->queue + (pQ->rec_sz * ((pQ->out + idx) % pQ->rec_nb));
		}
		else /* if (pQ->impl == LIFO) */
		{
			pStart = pQ->queue + (pQ->rec_sz * idx);
		}

		memcpy(record, pStart, pQ->rec_sz);
	}

	return ret;
}

bool __attribute__((nonnull)) q_peekPrevious(const Queue_t * const pQ, void * const record)
{
	const uint16_t idx = _q_getCount(pQ) - 1U;	// No worry about count -1 when queue is empty, test is done by q_peekIdx
	return q_peekIdx(pQ, record, idx);
}


/**********************/
/*** PUBLIC GETTERS ***/
/**********************/
bool __attribute__((nonnull)) q_isInitialized(const Queue_t * const pQ) {
	return _q_isInitialized(pQ); }

uint32_t __attribute__((nonnull)) q_sizeof(const Queue_t * const pQ) {
	return pQ->queue_sz; }

bool __attribute__((nonnull)) q_isEmpty(const Queue_t * const pQ) {
	return _q_isEmpty(pQ); }

bool __attribute__((nonnull)) q_isFull(const Queue_t * const pQ) {
	return _q_isFull(pQ); }

uint16_t __attribute__((nonnull)) q_getCount(const Queue_t * const pQ) {
	return _q_getCount(pQ); }

uint16_t __attribute__((nonnull)) q_getRemainingCount(const Queue_t * const pQ) {
	return pQ->rec_nb - pQ->cnt; }

