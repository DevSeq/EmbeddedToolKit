/*
    Embedded Tool Kit
    Copyright (C) 2015 Samuel Cowen

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ETK_SHORT_TERM_MEMORY_H
#define ETK_SHORT_TERM_MEMORY_H

#include "loop_range.h"

namespace etk
{

/**
 * \class ShortTermMemory
 *
 * \brief ShortTermMemory stores a data in a buffer. As more data is received, the old data is eventually overwritten and forgotten. It has some uses in digital signal processing, adaptive control and filtering.
 *
 * It's similar to a RingBuffer, except it has it's own memory and will always overwrite old data.
 *
 * ShortTermMemory is also iterable, which make it nice and easy to use.
 *
 * Example
 * @code
 etk::ShortTermMemory<float, 10> stm;
 stm.fill(0.0); //initialise memory with zeroes.
 for(float i = 5; i < 6.0; i += 0.1)
 	stm.put(i);
 for(auto i : stm)
     cout << i << " ";
    @endcode
 * Output: 5 5.1 5.2 5.3 5.4 5.5 5.6 5.7 5.8 5.9
 *
 * @tparam L the maximum length of the string in bytes.
 */


template <typename T, uint32_t LEN> class ShortTermMemory
{
public:
    class Iterator
    {
    public:
        inline Iterator(ShortTermMemory<T,LEN>& p, uint32_t position=0) : stm(p)
        {
            pos = position;
        }

        inline T operator*()
        {
            return stm.peek_ahead(pos);
        }

        inline Iterator& operator++ ()
        {
            pos++;
            return *this;
        }

        inline bool operator==(Iterator iter)
        {
            return (pos == iter.pos);
        }

        inline bool operator!=(Iterator iter)
        {
            return (pos != iter.pos);
        }

    private:
        ShortTermMemory<T,LEN>& stm;
        uint32_t pos = 0;
    };


    ShortTermMemory()
    {
        start = 0;
        buf_end = 0;
    }

    Iterator begin()
    {
        return Iterator(*this);
    }

    Iterator end()
    {
        return Iterator(*this, LEN);
    }

	/**
	 * \brief Add an item to memory.
	 */
    void put(T b)
    {
        buf_end = (buf_end + 1) % LEN;
        buf[buf_end] = b;
        if(is_full())
        {
            start = (start+1)%LEN;
        }
    }

    /**
     * \brief Returns true if memory is full.
     */
    bool is_full()
    {
        return (buf_end + 1) % LEN == start;
    }

    /**
     * \brief Umm . . . actually not sure what this function could possibly be used for.
     */
    void increment()
    {
        buf_end = (buf_end + 1) % LEN;
    }

	/**
	 * \brief Returns the next item without removing it from memory.
	 */
    T peek_ahead(uint16_t n=0)
    {
        uint16_t pos = (start+n) % LEN;
        return buf[pos];
    }

	/**
	 * \brief Empties memory.
	 */
    void empty()
    {
        start = 0;
        buf_end = 0;
    }

	/**
	 * \brief Fills the entire memory with a value.
	 */
    void fill(T t)
    {
        for(auto i : range(LEN))
        {
            put(t);
            (void)(i);
        }
        start = 0;
        buf_end = LEN-1;
    }

private:
    uint16_t start;
    uint16_t buf_end;
    T buf[LEN];
};

};

#endif