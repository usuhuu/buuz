/*
 * Copyright 2009-2012 Odbayar Nyamtseren <odbayar.n@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef COMP_STRING_H
#define COMP_STRING_H

int const maxCompLen = 80;
int const maxClauseLen = 4;

template<typename ItemType, DWORD capacity_>
class CompBuffer : private NonCopyable {
    ItemType* buffer_;
    DWORD* size_;
    ItemType tempItem_;

public:
    void init(ItemType* buffer, DWORD* size) {
        buffer_ = buffer;
        size_ = size;
    }

    bool append(ItemType value) {
        if (*size_ < capacity_) {
            buffer_[*size_] = value;
            *size_ += 1;
            return true;
        }
        return false;
    }

    bool append(const ItemType* data, DWORD size) {
        if (*size_ + size <= capacity_) {
            for (DWORD i = 0; i < size; ++i)
                buffer_[*size_ + i] = data[i];
            *size_ += size;
            return true;
        }
        return false;
    }

    bool insert(DWORD pos, ItemType value) {
        if (pos <= *size_ && *size_ < capacity_) {
            for (DWORD i = *size_; i > pos; --i)
                buffer_[i] = buffer_[i - 1];
            buffer_[pos] = value;
            *size_ += 1;
            return true;
        }
        return false;
    }

    ItemType pop() {
        if (*size_ > 0) {
            *size_ -= 1;
            ItemType ret = buffer_[*size_];
            buffer_[*size_] = 0;
            return ret;
        }
        return 0;
    }

    void erase(DWORD pos) {
        if (pos < *size_) {
            DWORD i;
            for (i = pos; i < *size_ - 1; ++i)
                buffer_[i] = buffer_[i + 1];
            buffer_[i] = 0;
            *size_ -= 1;
        }
    }

    ItemType& operator[](DWORD index) {
        if (index < capacity_)
            return buffer_[index];
        else
            return tempItem_;
    }

    const ItemType* ptr() { return buffer_; }

    DWORD size() { return *size_; }
    DWORD capacity() { return capacity_; }

    bool resize(DWORD size) {
        if (size <= capacity_) {
            if (size < *size_) {
                for (DWORD i = size; i < *size_; ++i)
                    buffer_[i] = 0;
                *size_ = size;
            }
            else if (size > *size_) {
                for (DWORD i = *size_; i < size; ++i)
                    buffer_[i] = 0;
                *size_ = size;
            }
            return true;
        }
        return false;
    }

    void setData(const ItemType* data, DWORD size) {
        if (size > capacity_)
            size = capacity_;

        DWORD i = 0;
        for (; i < size; ++i)
            buffer_[i] = data[i];
        for (; i < capacity_; ++i)
            buffer_[i] = 0;
        *size_ = size;
    }

    void repeat(ItemType value, DWORD repeat) {
        if (repeat > capacity_)
            repeat = capacity_;

        DWORD i = 0;
        for (; i < repeat; ++i)
            buffer_[i] = value;
        for (; i < capacity_; ++i)
            buffer_[i] = 0;
        *size_ = repeat;
    }
};

template<DWORD capacity_>
class ClauseBuffer : private NonCopyable {
    DWORD* buffer_;
    DWORD* size_;
    DWORD tempItem_;

public:
    void init(DWORD* buffer, DWORD* size) {
        buffer_ = buffer;
        size_ = size;
    }

    bool append(DWORD value) {
        if ((*size_ / 4) < capacity_) {
            buffer_[(*size_ / 4)] = value;
            *size_ += 4;
            return true;
        }
        return false;
    }

    DWORD& operator[](DWORD index) {
        if (index < capacity_)
            return buffer_[index];
        else
            return tempItem_;
    }

    bool resize(DWORD size) {
        if (size <= capacity_) {
            DWORD trueSize = (*size_ / 4);
            if (size < trueSize) {
                for (DWORD i = size; i < trueSize; ++i)
                    buffer_[i] = 0;
                *size_ = size * 4;
            }
            else if (size > trueSize) {
                for (DWORD i = trueSize; i < size; ++i)
                    buffer_[i] = 0;
                *size_ = size * 4;
            }
            return true;
        }
        return false;
    }
};

class InputContext;

class CompString : private NonCopyable {
    InputContext* imc_;
    COMPOSITIONSTRING* cs_;

public:
    CompBuffer<BYTE , maxCompLen> compReadAttr;
    ClauseBuffer<maxClauseLen>    compReadClause;
    CompBuffer<TCHAR, maxCompLen> compReadStr;
    CompBuffer<BYTE , maxCompLen> compAttr;
    ClauseBuffer<maxClauseLen>    compClause;
    CompBuffer<TCHAR, maxCompLen> compStr;
    ClauseBuffer<maxClauseLen>    resultReadClause;
    CompBuffer<TCHAR, maxCompLen> resultReadStr;
    ClauseBuffer<maxClauseLen>    resultClause;
    CompBuffer<TCHAR, maxCompLen> resultStr;

    CompString(InputContext* imc);
    ~CompString();

    bool create();

    bool lock();
    void unlock();

    void clearComp();
    void clearResult();
    void clearCompAndResult();

    DWORD cursorPos() { return cs_->dwCursorPos; }
    void setCursorPos(DWORD pos) { cs_->dwCursorPos = pos; }

    DWORD deltaStart() { return cs_->dwDeltaStart; }
    void setDeltaStart(DWORD pos) { cs_->dwDeltaStart = pos; }

    void updateCompReadClause();
    void updateCompClause();
    void updateResultReadClause();
    void updateResultClause();

private:
    void updateBufferWrappers();
};

#endif // COMP_STRING_H
