#pragma once

#include <new>
#include <forward_list>
#include <memory>
#include <mutex>

#include "Allocator.hpp"

template <typename T,
          typename Allocator = DoublingAllocator<T>>
class MemPool {
    std::forward_list<std::unique_ptr<Byte[]>> m_chunks;
    std::mutex m_mutex;
    ListNode<T> * m_freeList;
    Allocator m_allocator;
    
    void EnlistChunk() {
        auto slab = m_allocator.Allocate();
        m_chunks.emplace_front(std::move(slab.data));
        for (size_t i = 0; i < slab.length / sizeof(ListNode<T>); ++i) {
            auto & cell = reinterpret_cast<ListNode<T> *>(m_chunks.front().get())[i];
            cell.next = m_freeList;
            m_freeList = &cell;
        }
    }
public:
    template <typename ...Args>
    T * New(Args && ...args) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_freeList == nullptr) {
            EnlistChunk();
        }
        auto mem = new (&m_freeList->data) T(std::forward<Args>(args)...);
        m_freeList = m_freeList->next;
        return mem;
    }

    void Delete(T * mem) {
        std::lock_guard<std::mutex> lock(m_mutex);
        static const size_t headerBytes = sizeof(ListNode<T> *);
        mem->~T();
        auto cell = reinterpret_cast<ListNode<T> *>((Byte *)mem - headerBytes);
        cell->next = m_freeList;
        m_freeList = cell;
    }

    MemPool() : m_freeList(nullptr) {}
    
    MemPool(const MemPool & other) = delete;
};
