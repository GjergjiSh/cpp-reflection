#include <iostream>
#include <memory_resource>
#include <cstddef>
#include <atomic>
#include <iomanip>
#include <ostream>
#include <sys/resource.h>
#include <unistd.h>

#define POOL_SIZE 10

struct memblock {
    memblock(size_t size) { data.resize(size); }
    std::vector<std::byte> data;
};

class CustomMemoryResource : public std::pmr::synchronized_pool_resource {
private:
    std::atomic<size_t> total_bytes_allocated { 0 };

protected:
    void* do_allocate(std::size_t bytes, std::size_t alignment) override
    {
        if (total_bytes_allocated + bytes > max_size) { throw std::bad_alloc(); }
        total_bytes_allocated += bytes;
        std::cout << "Allocating " << bytes << " bytes. Total allocated: " << total_bytes_allocated << std::endl;
        return std::pmr::synchronized_pool_resource::do_allocate(bytes, alignment);
    }

    void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override
    {
        total_bytes_allocated -= bytes;
        std::cout << "Deallocating " << bytes << " bytes. Total allocated: " << total_bytes_allocated << std::endl;
        std::pmr::synchronized_pool_resource::do_deallocate(p, bytes, alignment);
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override
    {
        return this == &other;
    }

    size_t max_size = POOL_SIZE;

public:
    CustomMemoryResource(std::pmr::memory_resource* upstream)
        : std::pmr::synchronized_pool_resource(std::pmr::pool_options(), upstream)
    {
    }

    std::size_t get_total_bytes_allocated() const
    {
        return total_bytes_allocated.load();
    }
};

struct hexdump {
    void const* data;
    int len;

    hexdump(void const* data, int len)
        : data(data)
        , len(len)
    {
    }

    template <class T>
    hexdump(T const& v)
        : data(&v)
        , len(sizeof v)
    {
    }

    friend std::ostream& operator<<(std::ostream& s, hexdump const& v)
    {
        // don't change formatting for s
        std::ostream out(s.rdbuf());
        out << std::hex << std::setfill('0');

        unsigned char const* pc = reinterpret_cast<unsigned char const*>(v.data);

        std::string buf;
        buf.reserve(17); // premature optimization

        int i;
        for (i = 0; i < v.len; ++i, ++pc) {
            if ((i % 16) == 0) {
                if (i) {
                    out << "  " << buf << '\n';
                    buf.clear();
                }
                out << "  " << std::setw(4) << i << ' ';
            }

            out << ' ' << std::setw(2) << unsigned(*pc);
            buf += (0x20 <= *pc && *pc <= 0x7e) ? *pc : '.';
        }
        if (i % 16) {
            char const* spaces16x3 = "                                                ";
            out << &spaces16x3[3 * (i % 16)];
        }
        out << "  " << buf << '\n';

        return s;
    }
};

class Object {
public:
    Object(std::pmr::memory_resource* resource, size_t size)
        : resource(resource)
        , size(size)
        , data(static_cast<std::byte*>(resource->allocate(size)))
    {
    }

    ~Object()
    {
        resource->deallocate(data, size);
    }

private:
    std::byte* data;
    std::pmr::memory_resource* resource;
    size_t size;
};

class HObject {
public:
    HObject(size_t size)
        : size(size)
        , data(new std::byte[size])
    {
    }

    ~HObject()
    {
        delete[] data;
    }

private:
    std::byte* data;
    size_t size;
};

static const size_t heap_size = 10;
size_t current_heap_size = 0;

void* operator new(std::size_t size) {
    if (current_heap_size + size > heap_size) {
        throw std::bad_alloc();
    }

    current_heap_size += size;

    // Call the default memory allocation function
    return std::malloc(size);
}

void operator delete(void* ptr, std::size_t size) noexcept {
    current_heap_size -= size;
    std::free(ptr);
}

int main()
{
    // HObject obj(1);
    // {
    //     HObject obj2(2);
    // }
    // HObject obj3(5);
    // {
    //     HObject obj4(2);
    // }

    // // There should be 4 free bytes left in the pool
    // HObject obj5(4);
    // std::byte* byte = new std::byte(static_cast<std::byte>(1));

    auto stackBuffer = new std::byte[1];
    std::pmr::monotonic_buffer_resource upstream(stackBuffer, sizeof(stackBuffer));
    CustomMemoryResource resource(&upstream);

    Object obj(&resource, 1);
    {
        Object obj2(&resource, 2);
    }
    Object obj3(&resource, 5);
    {
        Object obj4(&resource, 2);
    }

    // There should be 4 free bytes left in the pool
    Object obj5(&resource, 4);

    // std::pmr::vector<std::byte*> allocated_bytes(&resource);

    // for (int i = 0; i < POOL_SIZE; i++) {
    //     auto alloc = std::pmr::polymorphic_allocator<std::byte>(&resource);
    //     auto allocated_byte_ptr = alloc.allocate(1);
    //     auto allocated_byte = new (allocated_byte_ptr) std::byte(static_cast<std::byte>(1));
    //     allocated_bytes.push_back(allocated_byte);
    // }

    // for (auto byte : allocated_bytes) {
    //     auto alloc = std::pmr::polymorphic_allocator<std::byte>(&resource);
    //     std::allocator_traits<decltype(alloc)>::destroy(alloc, byte);
    //     alloc.deallocate(byte, 1);
    // }

    // delete[] stackBuffer;
    // return 0;
}
