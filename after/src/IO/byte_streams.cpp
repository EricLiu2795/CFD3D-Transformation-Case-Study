
#include "IO/byte_streams.hpp"
#include <algorithm>
#include <iostream>
#include <cstring>
#include <cmath>

ByteWriteStream::ByteWriteStream(size_t size  )
{
    bufferSize = 0;
    capacity = 0;
    buffer = nullptr;
    reserve(size);
}

ByteWriteStream::~ByteWriteStream()
{
    if (buffer) {
        delete[] buffer;
        buffer = nullptr;
        capacity = 0;
        bufferSize = 0;
    }
}

void ByteWriteStream::write(const void *data, size_t size)
{
    if (bufferSize + size > capacity) {
        reserve(std::max(bufferSize + size, bufferSize*2));
    }

    assert(bufferSize + size <= capacity);
    if (size != 0) {
        memcpy(buffer + bufferSize, data, size);
    }
    bufferSize += size;
}

void ByteWriteStream::write(const char *str)
{
    uint32_t charCount = uint32_t(strlen(str));
    write(charCount);
    write((void*)str, charCount);
}

void ByteWriteStream::write(const std::string &str)
{
    uint32_t charCount = uint32_t(str.size());
    write(charCount);
    write((void*)str.c_str(), charCount);
}

void ByteWriteStream::reserve(size_t size  )
{
    size = std::max((size_t)4, size);
    if (size > capacity) {
        auto *_buffer = new uint8_t[size];
        if (buffer) {
            memcpy(_buffer, buffer, bufferSize);
            delete[] buffer;
        }
        buffer = _buffer;
        capacity = size;
    }
}



ByteReadStream::ByteReadStream(ByteWriteStream &stream)
{
    buffer = stream.buffer;
    bufferSize = stream.bufferSize;
    readOffset = 0;

    stream.buffer = nullptr;
    stream.bufferSize = 0;
    stream.capacity = 0;
}

ByteReadStream::ByteReadStream(void *_buffer, size_t _byteCount)
{
    buffer = (uint8_t*)_buffer;
    bufferSize = _byteCount;
    readOffset = 0;
}

ByteReadStream::ByteReadStream(const void *_buffer, size_t _byteCount)
{
    buffer = new uint8_t[_byteCount];
    memcpy(buffer, _buffer, _byteCount);
    bufferSize = _byteCount;
    readOffset = 0;
}

ByteReadStream::~ByteReadStream()
{
    if (buffer) {
        delete[] buffer;
        buffer = nullptr;
        readOffset = 0;
        bufferSize = 0;
    }
}

void ByteReadStream::read(void *data, size_t size)
{
    if (readOffset + size > bufferSize) {
        std::cerr << "FATAL ERROR: ByteReadStream::read(void*, size_t)" << std::endl;
        return;
    }
    if (size != 0) {
        memcpy(data, buffer + readOffset, size);
    }
    readOffset += size;
}

void ByteReadStream::read(std::string &str)
{
    uint32_t charCount = 0;
    read(charCount);
    if (readOffset + (size_t)charCount > bufferSize) {
        std::cerr << "FATAL ERROR: ByteReadStream::read(string&)" << std::endl;
        return;
    }
    std::vector<char> charBuffer(charCount + 1);
    charBuffer[charCount] = '\0';
    read((void*)charBuffer.data(), charCount);
    str = charBuffer.data();
}
