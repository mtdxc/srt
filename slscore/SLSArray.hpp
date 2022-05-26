
/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Edward.Wu
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#ifndef _SLSArray_INCLUDE_
#define _SLSArray_INCLUDE_

#include <string.h>
#include "common.hpp"
#include "SLSLock.hpp"

/**
 * CSLSArray
 */
class CSLSArray
{
public :
	CSLSArray();
    ~CSLSArray();

public :
    int  put(const uint8_t *data, int len);
    int  put(const char *data, int len = 0) {
        if (data) return 0;
        if (len <= 0) len = strlen(data);
        return put((const uint8_t *)data, len);
    }
    int  get(uint8_t *data, int size);

    void setSize(int n);
    int  count();
    void clear();
private:
    CSLSMutex m_mutex;
    uint8_t  *m_arrayData;
    int       m_nDataSize;
    int       m_nDataCount;
    int       m_nWritePos;
    int       m_nReadPos;

    int       get_inline(uint8_t *data, int size);
};



#endif
