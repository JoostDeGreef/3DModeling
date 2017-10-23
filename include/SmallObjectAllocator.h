#ifndef SMALL_OBJECT_POOL_ALLOCATOR_H
#define SMALL_OBJECT_POOL_ALLOCATOR_H 1

namespace Geometry
{
    template<typename T>
    class SmallObjectAllocator
    {
    public:
        typedef T value_type;

        SmallObjectAllocator() = default;

        template <class U>
        constexpr SmallObjectAllocator(const SmallObjectAllocator<U>&) noexcept
        {}

        T* allocate(std::size_t n)
        {
            if (n > std::size_t(-1) / sizeof(T)) throw std::bad_alloc();
            if (1 == n)
            {
                return GetPool().allocate();
            }
            else
            {
                if (auto p = reinterpret_cast<T*>(new unsigned char[n * sizeof(T)])) return p;
                throw std::bad_alloc();
            }
        }

        void deallocate(T* p, std::size_t n) noexcept
        {
            if (1 == n)
            {
                GetPool().free(p);
            }
            else
            {
                delete [] reinterpret_cast<char*>(p);
            }
        }

    private:
        class GlobalPool
        {
        public:
            static const size_t block_size = 1024;
            typedef std::array<unsigned char, block_size * sizeof(T)> block;

            GlobalPool()
            {
                m_available.reserve(block_size * 2);
            }

            void allocate_new_block(std::vector<T*>& res)
            {
                std::lock_guard<std::mutex> lock(m_mutex);

                if (m_available.size()>block_size)
                {
                    res.resize(block_size, nullptr);
                    size_t offset = m_available.size() - block_size;
                    for (size_t i = 0; i<block_size; ++i)
                    {
                        res[i] = m_available[i + offset];
                    }
                    m_available.resize(offset);
                }
                else if (!m_available.empty())
                {
                    res.swap(m_available);
                }
                else
                {
                    m_blocks.emplace_back(block());
                    block& b = m_blocks.back();
                    T* p = (T*)&(b.data()[0]);
                    res.resize(block_size, nullptr);
                    for (size_t i = 0; i<block_size; ++i)
                    {
                        res[i] = &p[i];
                    }
                }
            }

            void return_for_reuse(std::vector<T*>& reuse)
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                if (m_available.empty())
                {
                    m_available.swap(reuse);
                }
                else
                {
                    for (T* p : reuse)
                    {
                        m_available.emplace_back(p);
                    }
                    std::sort(m_available.begin(), m_available.end());
                }
            }

        private:
            std::vector<T*> m_available;
            std::deque<block> m_blocks;
            std::mutex m_mutex;
        }; // GlobalPool

        class ThreadLocalPool
        {
        public:
            ThreadLocalPool()
            {
                m_available.reserve(GlobalPool::block_size * 2);
            }
            ~ThreadLocalPool()
            {
                GetPool().return_for_reuse(m_available);
            }

            T* allocate()
            {
                if (m_available.empty())
                {
                    GetPool().allocate_new_block(m_available);
                }
                T* p = m_available.back();
                m_available.pop_back();
                return p;
            }

            void free(T* p)
            {
                m_available.emplace_back(p);
            }
        private:
            std::vector<T*> m_available;

            GlobalPool& GetPool()
            {
                static GlobalPool pool;
                return pool;
            }
        }; // ThreadLocalPool

        static ThreadLocalPool& GetPool()
        {
            thread_local static ThreadLocalPool pool;
            return pool;
        }

    };

    template <class T, class U>
    bool operator==(const SmallObjectAllocator<T>&, const SmallObjectAllocator<U>&) { return true; }

    template <class T, class U>
    bool operator!=(const SmallObjectAllocator<T>&, const SmallObjectAllocator<U>&) { return false; }

    /* Wrapper to construct classes with protected constructors
    */
    template <class T>
    class SmallObjectPtr : public T
    {
    public:
        template <typename... Args>
        SmallObjectPtr(Args&&... args)
            : T(std::forward<Args>(args)...)
        {}

        template <typename... Args>
        static std::shared_ptr<T> Construct(Args&&... args)
        {
            return std::allocate_shared<SmallObjectPtr<T>>(SmallObjectAllocator<SmallObjectPtr<T>>(), std::forward<Args>(args)...);
        }
    };

    /* Construct a shared pointer using the SmallObjectAllocator
    */
    template <class T,typename... Args>
    std::shared_ptr<T> Construct(Args&&... args)
    {
        return SmallObjectPtr<T>::Construct(std::forward<Args>(args)...);
    }

    /* Wrapper class for unowned ptrs
    */
    template <class T>
    class raw_ptr
    {
    public:
        typedef T element_type;

        raw_ptr() : m_ptr(nullptr) {}
        raw_ptr(T* ptr) : m_ptr(ptr) {}
        raw_ptr(const raw_ptr<T>& other) : m_ptr(other.m_ptr) {}
        raw_ptr(raw_ptr<T>&& other) : m_ptr(std::move(other.m_ptr)) {}
        raw_ptr(const std::shared_ptr<T>& shared) : m_ptr(shared.get()) {}
        raw_ptr(const std::unique_ptr<T>& unique) : m_ptr(unique.get()) {}
        raw_ptr(const std::weak_ptr<T>& weak) : m_ptr(weak.lock().get()) {}
        ~raw_ptr() {}

        raw_ptr<T>& operator = (const raw_ptr<T>& other) { m_ptr = other.m_ptr; return *this; }
        raw_ptr<T>& operator = (raw_ptr<T>&& other) { std::swap(m_ptr,other.m_ptr); return *this; }
        raw_ptr<T>& operator = (const std::shared_ptr<T>& shared) { m_ptr = shared.get(); }
        raw_ptr<T>& operator = (const std::unique_ptr<T>& unique) { m_ptr = unique.get(); }
        raw_ptr<T>& operator = (const std::weak_ptr<T>& weak) { m_ptr = weak.lock().get(); }

        T* operator -> () const { return m_ptr; }
        T* get() const { return m_ptr; }

        T& operator * () const { return *m_ptr; }

        operator bool() const { return nullptr != m_ptr; }

        bool operator == (const raw_ptr<T>& other) const { return other.m_ptr == m_ptr; }
        bool operator != (const raw_ptr<T>& other) const { return other.m_ptr != m_ptr; }
        bool operator >= (const raw_ptr<T>& other) const { return other.m_ptr >= m_ptr; }
        bool operator <= (const raw_ptr<T>& other) const { return other.m_ptr <= m_ptr; }
        bool operator > (const raw_ptr<T>& other) const { return other.m_ptr > m_ptr; }
        bool operator < (const raw_ptr<T>& other) const { return other.m_ptr < m_ptr; }

        std::shared_ptr<T> lock() const { return m_ptr->shared_from_this(); };

    private:
        T* m_ptr;
    };
} // namespace Geometry

namespace std 
{
    template <typename T>
    struct hash<Geometry::raw_ptr<T>>
    {
        std::size_t operator()(const Geometry::raw_ptr<T>& ptr) const
        {
            return hash<T*>()(ptr.get());
        }
    };
} // namespace std

#endif // SMALL_OBJECT_POOL_ALLOCATOR_H
