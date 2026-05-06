#ifndef FOC_SPSC_QUEUE_H
#define FOC_SPSC_QUEUE_H

#include <atomic>
#include <vector>
#include <optional>
#include <cstdint>

/**
 * FOC_SPSC_Queue: A Single-Producer Single-Consumer lock-free queue.
 * Designed for high-throughput event ingestion.
 * 
 * Mechanical Sympathy:
 * - Uses cache-line padding to prevent false sharing between producer and consumer.
 * - Power-of-two size for fast modulo operations.
 */

template <typename T>
class SPSCQueue {
public:
    explicit SPSCQueue(size_t capacity) : capacity_(next_pow2(capacity)), mask_(capacity_ - 1), buffer_(capacity_) {
        head_.store(0, std::memory_order_relaxed);
        tail_.store(0, std::memory_order_relaxed);
    }

    // Producer: Push to queue
    bool push(const T& item) {
        const size_t head = head_.load(std::memory_order_relaxed);
        if (head - tail_.load(std::memory_order_acquire) == capacity_) {
            return false; // Queue full
        }
        buffer_[head & mask_] = item;
        head_.store(head + 1, std::memory_order_release);
        return true;
    }

    // Consumer: Pop from queue
    std::optional<T> pop() {
        const size_t tail = tail_.load(std::memory_order_relaxed);
        if (tail == head_.load(std::memory_order_acquire)) {
            return std::nullopt; // Queue empty
        }
        T item = buffer_[tail & mask_];
        tail_.store(tail + 1, std::memory_order_release);
        return item;
    }

    // Batch Pop for performance
    size_t pop_batch(std::vector<T>& out, size_t max_items) {
        size_t count = 0;
        while (count < max_items) {
            auto item = pop();
            if (!item) break;
            out.push_back(*item);
            count++;
        }
        return count;
    }

    size_t size() const {
        return head_.load(std::memory_order_relaxed) - tail_.load(std::memory_order_relaxed);
    }

private:
    static size_t next_pow2(size_t n) {
        n--;
        n |= n >> 1;
        n |= n >> 2;
        n |= n >> 4;
        n |= n >> 8;
        n |= n >> 16;
        n |= n >> 32;
        n++;
        return n;
    }

    const size_t capacity_;
    const size_t mask_;
    std::vector<T> buffer_;

    // Align to 64 bytes (typical cache line) to prevent false sharing
    alignas(64) std::atomic<size_t> head_;
    alignas(64) std::atomic<size_t> tail_;
};

#endif
