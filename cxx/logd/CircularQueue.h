#ifndef _CIRCULAR_QUEUE_H
#define _CIRCULAR_QUEUE_H

#include <stdio.h>
#include <string.h>

#ifdef WIN32
#   include <OS.h>
#else
#   include <netinet/in.h>
#endif


template<typename T>
class CircularQueue
{
public:

    void resize(uint32_t size)
    {
        _maxsz = size;
    }

    // Adds an element to the back of queue.
    void push(const T& val)
    {
        if ( spare() > 0 )
        {
            memcpy( (T*)&_data + _tail, &val, sizeof(val) );
            if ( ++_tail >= _maxsz )
                _tail = 0;
        }
        else
            printf("\n======= no memory!  ========");
    }

    // Removes the first element.
    void pop()
    {
        if ( !empty() )
        {
            if ( ++_head >= _maxsz )
                _head = 0;
        }
    }

    // Tests if the queue is empty.
    bool empty() const
    {
        return (_head == _tail);
    }

    // Returns the number of elements in the queue.
    uint32_t size() const
    {
        return (_tail >= _head) ? (_tail - _head) : (_maxsz - _head + _tail);
    }

    uint32_t spare() const
    {
        return _maxsz - size();
    }

    uint32_t head() const
    {
        return _head;
    }

    uint32_t tail() const
    {
        return _tail;
    }

    uint32_t max_size() const
    {
        return _maxsz;
    }

    T const* at(uint32_t pos) const
    {
        if ( !empty() )// no enough condition.
            return (T*)&_data + pos;
        else
            return NULL;
    }

    T const* front() const
    {
        return at(_head);
    }

    T const* back() const
    {
        return at(_tail);
    }

private:
    CircularQueue(){}
    ~CircularQueue(){}

    uint32_t _head; // pos, index, start with 0.
    uint32_t _tail; // pos, index, start with 0.
    uint32_t _maxsz;
    uint8_t  _data;
};


#endif//(_CIRCULAR_QUEUE_H)

