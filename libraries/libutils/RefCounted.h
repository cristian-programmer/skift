#pragma once

#include <libsystem/Assert.h>
#include <libsystem/Logger.h>

template <typename T>
class RefCounted
{
private:
    int _refcount = 1;

    __noncopyable(RefCounted);
    __nonmovable(RefCounted);

public:
    RefCounted() {}

    virtual ~RefCounted()
    {
        assert(_refcount == 0);
    }

    void ref()
    {
        int refcount = __atomic_add_fetch(&_refcount, 1, __ATOMIC_SEQ_CST);
        assert(refcount >= 0);
    }

    void deref()
    {
        int refcount = __atomic_sub_fetch(&_refcount, 1, __ATOMIC_SEQ_CST);
        assert(refcount >= 0);

        if (refcount == 1)
        {
            if constexpr (requires(const T &t) {
                              t.one_ref_left();
                          })
            {
                this->one_ref_left();
            }
        }

        if (refcount == 0)
        {
            delete static_cast<T *>(this);
        }
    }

    int refcount()
    {
        return _refcount;
    }
};

template <typename T>
void ref_if_not_null(T *ptr)
{
    if (ptr)
    {
        ptr->ref();
    }
}

template <typename T>
void deref_if_not_null(T *ptr)
{
    if (ptr)
    {
        ptr->deref();
    }
}
