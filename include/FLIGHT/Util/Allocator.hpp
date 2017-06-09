#pragma once
#include <memory>

using Byte = uint8_t;

struct Slab {
    std::unique_ptr<Byte[]> data;
    size_t length;
};

template <typename T>
struct ListNode {
    ListNode<T> * next;
    T data;
};

template <typename T>
class DoublingAllocator {
    size_t m_allocQuota = 4;
public:
    Slab Allocate() {
        const auto allocSize = m_allocQuota * sizeof(ListNode<T>);
        Slab s {
            std::unique_ptr<Byte[]>(new Byte[allocSize]), allocSize };
        m_allocQuota *= 2;
        return s;
    }
};

template <typename T, size_t Size>
struct FixedAllocator {
    Slab Allocate() {
        return Slab { std::unique_ptr<Byte[]>(new Byte[Size]), Size };
    }
};
