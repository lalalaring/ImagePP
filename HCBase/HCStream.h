#pragma once
#include <cassert>
using namespace  std;
#define USE_LITTLE_ENDIAN 1

typedef unsigned int uint;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long  uint64;

//typedef int int;
typedef signed char int8;
typedef signed short int16;
typedef signed int int32;
typedef signed long long  int64;
class HCStream
{
public:


	enum ByteOrder
	{
		LittleEndian ,
		BigEndian,
	};

	static ByteOrder getSystemByteOrder()
	{
#if USE_LITTLE_ENDIAN
		return LittleEndian;
#else
		return BigEndian;
#endif
	}



	HCStream() : m_byteOrder(LittleEndian) { }


	virtual ~HCStream() {}

	//设置字节序
	void setByteOrder(ByteOrder bo) { m_byteOrder = bo; }

	//获取字节序
	ByteOrder byteOrder() const { return m_byteOrder; }


	//序列化数据
	virtual uint serialize(void * data, uint len) = 0;

	/// Move to the given position in the archive.
	virtual void seek(uint pos) = 0;

	/// Return the current position in the archive.
	virtual uint tell() const = 0;

	/// Return the current size of the archive.
	virtual uint size() const = 0;

	/// Determine if there has been any error.
	virtual bool isError() const = 0;

	/// Clear errors.
	virtual void clearError() = 0;

	/// Return true if the stream is at the end.
	virtual bool isAtEnd() const = 0;

	/// Return true if the stream is seekable.
	virtual bool isSeekable() const = 0;

	/// Return true if this is an input stream.
	virtual bool isLoading() const = 0;

	/// Return true if this is an output stream.
	virtual bool isSaving() const = 0;


	void advance(uint offset) { seek(tell() + offset); }


	// friends	
	friend HCStream & operator<<(HCStream & s, bool & c) {
		static_assert(sizeof(bool) == 1, "static_assert error");
		s.serialize(&c, 1);

		return s;
	}
	friend HCStream & operator<<(HCStream & s, char & c) {
		static_assert(sizeof(char) == 1,"static_assert error");
		s.serialize(&c, 1);
		return s;
	}
	friend HCStream & operator<<(HCStream & s, uint8 & c) {
		static_assert(sizeof(uint8) == 1, "static_assert error");
		s.serialize(&c, 1);
		return s;
	}
	friend HCStream & operator<<(HCStream & s, int8 & c) {
		static_assert(sizeof(int8) == 1, "static_assert error");
		s.serialize(&c, 1);
		return s;
	}
	friend HCStream & operator<<(HCStream & s, uint16 & c) {
		static_assert(sizeof(uint16) == 2, "static_assert error");
		return s.byteOrderSerialize(&c, 2);
	}
	friend HCStream & operator<<(HCStream & s, int16 & c) {
		static_assert(sizeof(int16) == 2, "static_assert error");
		return s.byteOrderSerialize(&c, 2);
	}
	friend HCStream & operator<<(HCStream & s, uint32 & c) {
		static_assert(sizeof(uint32) == 4, "static_assert error");
		return s.byteOrderSerialize(&c, 4);
	}
	friend HCStream & operator<<(HCStream & s, int32 & c) {
		static_assert(sizeof(int32) == 4, "static_assert error");
		return s.byteOrderSerialize(&c, 4);
	}
	friend HCStream & operator<<(HCStream & s, uint64 & c) {
		static_assert(sizeof(uint64) == 8, "static_assert error");
		return s.byteOrderSerialize(&c, 8);
	}
	friend HCStream & operator<<(HCStream & s, int64 & c) {
		static_assert(sizeof(int64) == 8, "static_assert error");
		return s.byteOrderSerialize(&c, 8);
	}
	friend HCStream & operator<<(HCStream & s, float & c) {
		static_assert(sizeof(float) == 4, "static_assert error");
		return s.byteOrderSerialize(&c, 4);
	}
	friend HCStream & operator<<(HCStream & s, double & c) {
		static_assert(sizeof(double) == 8, "static_assert error");
		return s.byteOrderSerialize(&c, 8);
	}

protected:

	/// Serialize in the stream byte order.
	HCStream & byteOrderSerialize(void * v, uint len) {
		if (m_byteOrder == getSystemByteOrder()) {
			serialize(v, len);
		}
		else {
			for (uint i = len; i > 0; i--) {
				serialize((uint8 *)v + i - 1, 1);
			}
		}
		return *this;
	}

private:

	ByteOrder m_byteOrder;
};

