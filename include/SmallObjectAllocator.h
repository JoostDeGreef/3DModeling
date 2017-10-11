#ifndef SMALL_OBJECT_POOL_ALLOCATOR_H
#define SMALL_OBJECT_POOL_ALLOCATOR_H 1

#include <deque>
#include <vector>
#include <mutex>

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
            }
            throw std::bad_alloc();
        }

        void deallocate(T* p, std::size_t n) noexcept
        {
            if (1 == n)
            {
                GetPool().free(p);
            }
            else
            {
                delete[] reinterpret_cast<char*>(p);
            }
        }

        class Object
        {
        public:
            template<typename... Args>
            static std::shared_ptr<T> Construct(Args&&... args)
            {
                SmallObjectAllocator<T> allocator;
                return std::allocate_shared<T>(allocator, std::forward<Args>(args)...);
            }
        };

    private:
        class GlobalPool
        {
        public:
            static const size_t block_size = 1024;
            typedef std::array<unsigned char, block_size * sizeof(T)> block;

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

        ThreadLocalPool& GetPool()
        {
            thread_local static ThreadLocalPool pool;
            return pool;
        }

    };


    template <class T, class U>
    bool operator==(const SmallObjectAllocator<T>&, const SmallObjectAllocator<U>&) { return true; }

    template <class T, class U>
    bool operator!=(const SmallObjectAllocator<T>&, const SmallObjectAllocator<U>&) { return false; }

} // namespace Geometry

#endif // SMALL_OBJECT_POOL_ALLOCATOR_H
