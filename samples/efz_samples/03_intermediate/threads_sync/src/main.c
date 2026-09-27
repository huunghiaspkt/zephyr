/*
 * Copyright (c) 2026 EmbeddedFun
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * threads_sync — the three primitives every Zephyr app eventually needs.
 *
 *   k_mutex  -> shared state, mutual exclusion. Use when N threads
 *               touch the same memory and must take turns.
 *   k_sem    -> signalling. Use when one thread (or ISR) wants to wake
 *               another. Counting form models a token pool.
 *   k_msgq   -> bounded queue of fixed-size items. Use when threads
 *               want to *send data*, not just signal.
 *
 * This sample wires up all three with one producer and one consumer:
 *
 *   producer
 *     | -- k_msgq_put(sample) -> consumer
 *     | -- k_sem_give(work)   -> consumer wakes
 *     | <-- k_mutex_lock(stats) --> shared counter
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(threads_sync, LOG_LEVEL_INF);

#define STACK_SIZE 1024
#define PRIO       5

/* (1) MSGQ — a 4-deep queue of 32-bit samples. */
K_MSGQ_DEFINE(sample_q, sizeof(uint32_t), 4, 4);

/* (2) SEM — counts 0..1, used as a binary wake signal. */
K_SEM_DEFINE(work_sem, 0, 1);

/* (3) MUTEX — guards `stats` below. */
K_MUTEX_DEFINE(stats_lock);

static struct {
	uint32_t produced;
	uint32_t consumed;
} stats;

static void producer(void *a, void *b, void *c)
{
	ARG_UNUSED(a); ARG_UNUSED(b); ARG_UNUSED(c);

	uint32_t sample = 0;

	while (1) {
		sample++;

		/* Queue the value. K_FOREVER blocks until space is free —
		 * here it never blocks because the consumer keeps up. */
		k_msgq_put(&sample_q, &sample, K_FOREVER);

		/* Wake the consumer. Many producers can give; one consumer
		 * takes. */
		k_sem_give(&work_sem);

		k_mutex_lock(&stats_lock, K_FOREVER);
		stats.produced++;
		k_mutex_unlock(&stats_lock);

		k_msleep(500);
	}
}

static void consumer(void *a, void *b, void *c)
{
	ARG_UNUSED(a); ARG_UNUSED(b); ARG_UNUSED(c);

	while (1) {
		/* Wait for a sample to arrive. */
		k_sem_take(&work_sem, K_FOREVER);

		uint32_t sample;
		while (k_msgq_get(&sample_q, &sample, K_NO_WAIT) == 0) {

			k_mutex_lock(&stats_lock, K_FOREVER);
			stats.consumed++;
			uint32_t p = stats.produced;
			uint32_t c = stats.consumed;
			k_mutex_unlock(&stats_lock);

			LOG_INF("consumed sample=%u  (produced=%u consumed=%u)",
				sample, p, c);
		}
	}
}

K_THREAD_DEFINE(producer_tid, STACK_SIZE, producer, NULL, NULL, NULL,
		PRIO, 0, 0);
K_THREAD_DEFINE(consumer_tid, STACK_SIZE, consumer, NULL, NULL, NULL,
		PRIO, 0, 0);

int main(void)
{
	k_thread_name_set(producer_tid, "producer");
	k_thread_name_set(consumer_tid, "consumer");
	LOG_INF("Mutex + Sem + MsgQ demo running. Watch the counters tick.");
	return 0;
}
