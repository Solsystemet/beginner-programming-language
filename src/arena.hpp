#pragma once
#include <cstddef>
#include <iostream>
#include <cstdlib> // For malloc and free

class ArenaAllocater
{
public:
    ArenaAllocater() = default;
    explicit ArenaAllocater(size_t bytes)
        : m_size(bytes)
    {
        m_buffer = static_cast<unsigned char*>(malloc(m_size));
        m_offset = m_buffer;
    }

    template<typename T>
    T* alloc()
    {
        void* offset = m_offset;
        m_offset += sizeof(T);
        return static_cast<T*>(offset);
    }


    ~ArenaAllocater()
    {
        free(m_buffer);
    }

private:
    size_t m_size;
    unsigned char* m_buffer;
    unsigned char* m_offset;
};
