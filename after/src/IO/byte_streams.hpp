
#ifndef FLUID3D_BYTE_STREAMS_HPP
#define FLUID3D_BYTE_STREAMS_HPP

#include <string>
#include <vector>
#include <cassert>
#include <cstdint>
#include <memory>

class ByteReadStream;

const size_t DEFAULT_CAPACITY = 256;

class ByteWriteStream
{
    friend class ByteReadStream;
public:
    explicit ByteWriteStream(size_t size = DEFAULT_CAPACITY);
    ~ByteWriteStream();
    void reserve(size_t size = DEFAULT_CAPACITY);

    void write(const void *data, size_t size);
    template<typename T>
    void write(const T &val) { write((const void*)&val, sizeof(T)); }
    void write(const char *str);
    void write(const std::string &str);

    template<typename T>
    void writeVector(const std::vector<T> &v)
    {
        uint32_t size = v.size();
        write(size);
        if (size > 0) {
            write((const void*)&v.front(), sizeof(T)*v.size());
        }
    }

    template<typename T>
    ByteWriteStream& operator<<(const T &val) { write(val); return *this; }
    ByteWriteStream& operator<<(const char *str) { write(str); return *this; }
    ByteWriteStream& operator<<(const std::string &str) { write(str); return *this; }

    inline size_t getSize() const { return bufferSize; }
    inline const uint8_t *getBuffer() const { return buffer; }

protected:
    size_t bufferSize;
    size_t capacity;
    uint8_t *buffer;
};

class ByteReadStream
{
public:
    explicit ByteReadStream(ByteWriteStream &stream);
    ByteReadStream(void *_buffer, size_t _byteCount);
    ByteReadStream(const void *_buffer, size_t _byteCount);
    ~ByteReadStream();
    inline size_t getSize() const { return bufferSize; }

    void read(void *data, size_t size);
    template<typename T>
    void read(T &val) { read((void*)&val, sizeof(T)); }
    void read(std::string &str);

    template<typename T>
    void readVector(std::vector<T> &v)
    {
        uint32_t size;
        read(size);
        if (size > 0) {
            v.resize(size);
            read((void*)&v.front(), sizeof(T)*v.size());
        }
    }

    template<typename T>
    ByteReadStream& operator>>(T &val) { read(val); return *this; }
    ByteReadStream& operator>>(std::string &str) { read(str); return *this; }

protected:
    size_t bufferSize;
    size_t readOffset;
    uint8_t *buffer;
};

#endif
