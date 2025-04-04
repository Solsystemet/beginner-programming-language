class ArenaAllocater
{
public:
    explicit ArenaAllocater(size_t bytes)
        : m_size(bytes)
    {
        m_buffer = malloc(m_size);
        m_offset = m_buffer;
    }

    // Move constructor
    ArenaAllocater(ArenaAllocater&& other) noexcept
        : m_size(other.m_size), m_buffer(other.m_buffer), m_offset(other.m_offset)
    {
        other.m_buffer = nullptr;
        other.m_offset = nullptr;
        other.m_size = 0;
    }

    // Move assignment
    ArenaAllocater& operator=(ArenaAllocater&& other) noexcept
    {
        if (this != &other) {
            free(m_buffer); // free current buffer

            m_size = other.m_size;
            m_buffer = other.m_buffer;
            m_offset = other.m_offset;

            other.m_buffer = nullptr;
            other.m_offset = nullptr;
            other.m_size = 0;
        }
        return *this;
    }

    // Prevent copying
    ArenaAllocater(const ArenaAllocater& other) = delete;
    ArenaAllocater& operator=(const ArenaAllocater& other) = delete;

    ~ArenaAllocater()
    {
        free(m_buffer);
    }

    template<typename T>
    T* alloc()
    {
        void* offset = m_offset;
        m_offset = static_cast<char*>(m_offset) + sizeof(T);
        return static_cast<T*>(offset);
    }

private:
    size_t m_size;
    void* m_buffer;
    void* m_offset;
};
