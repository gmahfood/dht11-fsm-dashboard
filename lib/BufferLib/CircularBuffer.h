#pragma once

#include <stdint.h>

// =============================================================
//  CircularBuffer<T, N>
//  A fixed-size ring buffer — no dynamic memory, safe for MCUs.
//  Template parameter N sets capacity at compile time.
// =============================================================
template <typename T, uint8_t N>
class CircularBuffer {
public:
    CircularBuffer() : head_(0), count_(0) {}

    // Add a value; overwrites oldest if full
    void push(T value) {
        data_[head_] = value;
        head_ = (head_ + 1) % N;
        if (count_ < N) ++count_;
    }

    // Average of all stored values
    float average() const {
        if (count_ == 0) return 0.0f;
        float sum = 0.0f;
        for (uint8_t i = 0; i < count_; ++i) sum += data_[i];
        return sum / count_;
    }

    uint8_t size()     const { return count_; }
    bool    isEmpty()  const { return count_ == 0; }
    bool    isFull()   const { return count_ == N; }

    // Access by logical index (0 = oldest)
    T operator[](uint8_t index) const {
        uint8_t real = (head_ - count_ + index + N) % N;
        return data_[real];
    }

private:
    T       data_[N];
    uint8_t head_;
    uint8_t count_;
};
