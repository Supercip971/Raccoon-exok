#ifndef LOCK_H
#define LOCK_H

#include <stdatomic.h>

typedef struct {
    atomic_char16_t lock;
} lock;

#define WAIT_LOCK(x) wait_lock(&x);
#define GRAB_LOCK_f(x)
#define GRAB_LOCK(x) WAIT_LOCK(x); GRAB_LOCK_f(x)
#define RELEASE_LOCK_f(x) atomic_thread_fence(memory_order_seq_cst); ((x).lock--)
#define RELEASE_LOCK(x) RELEASE_LOCK_f(x)

#define BEGIN_BOTTLENECK(name) GRAB_LOCK(_lock_##name);
#define END_BOTTLENECK(name) RELEASE_LOCK(_lock_##name);

#define CREATE_LOCK(name) static lock _lock_##name;
#define LOCK_NAME(x) _lock_##x

#define CREATE_GET(name) void get_lock_##name () {  GRAB_LOCK(_lock_ ##name); }
#define CREATE_SET_SYNC(name) void set_lock_##name () { RELEASE_LOCK(_lock_ ##name); }
#define CREATE_COMMON(name) CREATE_LOCK(name) CREATE_GET(name) CREATE_SET_SYNC(name)
#define CREATE_GET_PROTO(name) void get_lock_##name ();
#define CREATE_SET_PROTO(name) void set_lock_##name ();
#define CREATE_PROTOS(name) CREATE_GET_PROTO(name) CREATE_SET_PROTO(name)

static inline void wait_lock(lock *l)
{
    unsigned short expected = 0;

    while (atomic_compare_exchange_strong(&l->lock, &expected, 1));
    atomic_thread_fence(memory_order_seq_cst);
}

#endif