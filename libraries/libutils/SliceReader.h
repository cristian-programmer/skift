#pragma once
#include <libutils/Slice.h>
#include <libutils/String.h>

class SliceReader
{
public:
    SliceReader(Slice &slice) : _slice(slice)
    {
    }

    template<typename T>
    inline T peek()
    {
        auto size = sizeof(T);
        auto sub_slice = _slice.slice(_index,size);
        return *(T*)sub_slice.start();
    }
    
    template<typename T>
    inline T get()
    {
        //TODO: check size
        const auto& result = peek<T>();
        _index += sizeof(T);
        return result; 
    }

    inline String get_fixed_len_string(unsigned int len)
    {
        auto sub_slice = _slice.slice(_index,len);
        const char* cstr = reinterpret_cast<const char*>(sub_slice.start());
        _index += len;
        return String(make<StringStorage>(cstr,len));
    }

    inline void skip(unsigned int num_bytes)
    {
        _index += num_bytes;
    }

    inline unsigned int index()
    {
        return _index;
    }

    inline bool good()
    {
        return _index < _slice.size();
    }

private:
    const Slice &_slice;
    unsigned int _index = 0;
};