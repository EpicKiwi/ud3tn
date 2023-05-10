// SPDX-License-Identifier: BSD-3-Clause OR Apache-2.0
#ifndef HAL_TYPES_H_INCLUDED
#define HAL_TYPES_H_INCLUDED

#include "platform/posix/simple_queue.h"

#include <sys/types.h>
#include <semaphore.h>
#include <fcntl.h>
#include <pthread.h>

#define QueueIdentifier_t Queue_t*
#define Semaphore_t sem_t*

#endif // HAL_TYPES_H_INCLUDED
