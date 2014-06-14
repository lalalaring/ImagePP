#pragma once
#include "HCStream.h"
#include <stdio.h> // fopen
#include <string.h> // memcpy
// Portable version of fopen.
inline FILE * fileOpen(const char * fileName, const char * mode)
{
	assert(fileName != NULL);
#if CC_MSVC && _MSC_VER >= 1400
	FILE * fp;
	if (fopen_s(&fp, fileName, mode) == 0) {
		return fp;
	}
	return NULL;
#else
	return fopen(fileName, mode);
#endif
}
class HCStdStream :public HCStream
{

private: 
	HCStdStream(const HCStdStream &);
	HCStdStream &operator=(const HCStdStream &);
public:

	/// Ctor.
	HCStdStream(FILE * fp, bool autoclose = true) : m_fp(fp), m_autoclose(autoclose) { }

	/// Dtor. 
	virtual ~HCStdStream()
	{
		if (m_fp != NULL && m_autoclose) {
#if OS_WIN32
			_fclose_nolock(m_fp);
#else
			fclose(m_fp);
#endif
		}
	}


	/** @name Stream implementation. */
	//@{
	virtual void seek(uint pos)
	{
		assert(m_fp != NULL);
		assert(pos <= size());
#if OS_WIN32
		_fseek_nolock(m_fp, pos, SEEK_SET);
#else
		fseek(m_fp, pos, SEEK_SET);
#endif
	}

	virtual uint tell() const
	{
		assert(m_fp != NULL);
#if OS_WIN32
		return _ftell_nolock(m_fp);
#else
		return (uint)ftell(m_fp);
#endif
	}

	virtual uint size() const
	{
		assert(m_fp != NULL);
#if OS_WIN32
		uint pos = _ftell_nolock(m_fp);
		_fseek_nolock(m_fp, 0, SEEK_END);
		uint end = _ftell_nolock(m_fp);
		_fseek_nolock(m_fp, pos, SEEK_SET);
#else
		uint pos = (uint)ftell(m_fp);
		fseek(m_fp, 0, SEEK_END);
		uint end = (uint)ftell(m_fp);
		fseek(m_fp, pos, SEEK_SET);
#endif
		return end;
	}

	virtual bool isError() const
	{
		return m_fp == NULL || ferror(m_fp) != 0;
	}

	virtual void clearError()
	{
		assert(m_fp != NULL);
		clearerr(m_fp);
	}

	// @@ The original implementation uses feof, which only returns true when we attempt to read *past* the end of the stream. 
	// That is, if we read the last byte of a file, then isAtEnd would still return false, even though the stream pointer is at the file end. This is not the intent and was inconsistent with the implementation of the MemoryStream, a better 
	// implementation uses use ftell and fseek to determine our location within the file.
	virtual bool isAtEnd() const
	{
		assert(m_fp != NULL);
		//return feof( m_fp ) != 0;
#if OS_WIN32
		uint pos = _ftell_nolock(m_fp);
		_fseek_nolock(m_fp, 0, SEEK_END);
		uint end = _ftell_nolock(m_fp);
		_fseek_nolock(m_fp, pos, SEEK_SET);
#else
		uint pos = (uint)ftell(m_fp);
		fseek(m_fp, 0, SEEK_END);
		uint end = (uint)ftell(m_fp);
		fseek(m_fp, pos, SEEK_SET);
#endif
		return pos == end;
	}

	/// Always true.
	virtual bool isSeekable() const { return true; }
	//@}

protected:

	FILE * m_fp;
	bool m_autoclose;
};



/// Standard output stream.
class HCStdOutputStream : public HCStdStream
{

private:
	HCStdOutputStream(const HCStdOutputStream &);
	HCStdOutputStream &operator=(const HCStdOutputStream &);
public:

	/// Construct stream by file name.
	HCStdOutputStream(const char * name) : HCStdStream(fileOpen(name, "wb")) { }

	/// Construct stream by file handle.
	HCStdOutputStream(FILE * fp, bool autoclose = true) : HCStdStream(fp, autoclose)
	{
	}

	/** @name Stream implementation. */
	//@{
	/// Write data.
	virtual uint serialize(void * data, uint len)
	{
		assert(data != NULL);
		assert(m_fp != NULL);
#if OS_WIN32
		return (uint)_fwrite_nolock(data, 1, len, m_fp);
#elif OS_LINUX
		return (uint)fwrite_unlocked(data, 1, len, m_fp);
#elif OS_DARWIN
		// @@ No error checking, always returns len.
		for (uint i = 0; i < len; i++) {
			putc_unlocked(((char *)data)[i], m_fp);
		}
		return len;
#else
		return (uint)fwrite(data, 1, len, m_fp);
#endif
	}

	virtual bool isLoading() const
	{
		return false;
	}

	virtual bool isSaving() const
	{
		return true;
	}
	//@}

};


/// Standard input stream.
class HCStdInputStream : public HCStdStream
{
private:
	HCStdInputStream(const HCStdInputStream &);
	HCStdInputStream &operator=(const HCStdInputStream &);
public:

	/// Construct stream by file name.
	HCStdInputStream(const char * name) : HCStdStream(fileOpen(name, "rb")) { }

	/// Construct stream by file handle.
	HCStdInputStream(FILE * fp, bool autoclose = true) : HCStdStream(fp, autoclose)
	{
	}

	/** @name Stream implementation. */
	//@{
	/// Read data.
	virtual uint serialize(void * data, uint len)
	{
		assert(data != NULL);
		assert(m_fp != NULL);
#if OS_WIN32
		return (uint)_fread_nolock(data, 1, len, m_fp);
#elif OS_LINUX
		return (uint)fread_unlocked(data, 1, len, m_fp);
#elif OS_DARWIN
		// @@ No error checking, always returns len.
		for (uint i = 0; i < len; i++) {
			((char *)data)[i] = getc_unlocked(m_fp);
		}
		return len;
#else
		return (uint)fread(data, 1, len, m_fp);
#endif

	}

	virtual bool isLoading() const
	{
		return true;
	}

	virtual bool isSaving() const
	{
		return false;
	}
	//@}
};



/// Memory input stream.
class  HCMemoryInputStream : public HCStream
{
	//NV_FORBID_COPY(MemoryInputStream);
private:
	HCMemoryInputStream(const HCMemoryInputStream &);
	HCMemoryInputStream &operator=(const HCMemoryInputStream &);
public:

	/// Ctor.
	HCMemoryInputStream(const uint8 * mem, uint size) : m_mem(mem), m_ptr(mem), m_size(size) { }

	/** @name Stream implementation. */
	//@{
	/// Read data.
	virtual uint serialize(void * data, uint len)
	{
		assert(data != NULL);
		assert(!isError());

		uint left = m_size - tell();
		if (len > left) len = left;

		memcpy(data, m_ptr, len);
		m_ptr += len;

		return len;
	}

	virtual void seek(uint pos)
	{
		assert(!isError());
		m_ptr = m_mem + pos;
		assert(!isError());
	}

	virtual uint tell() const
	{
		assert(m_ptr >= m_mem);
		return uint(m_ptr - m_mem);
	}

	virtual uint size() const
	{
		return m_size;
	}

	virtual bool isError() const
	{
		return m_mem == NULL || m_ptr > m_mem + m_size || m_ptr < m_mem;
	}

	virtual void clearError()
	{
		// Nothing to do.
	}

	virtual bool isAtEnd() const
	{
		return m_ptr == m_mem + m_size;
	}

	/// Always true.
	virtual bool isSeekable() const
	{
		return true;
	}

	virtual bool isLoading() const
	{
		return true;
	}

	virtual bool isSaving() const
	{
		return false;
	}
	//@}

	const uint8 * ptr() const { return m_ptr; }


private:

	const uint8 * m_mem;
	const uint8 * m_ptr;
	uint m_size;

};





/// Protected input stream.
class  HCProtectedStream : public HCStream
{
	//NV_FORBID_COPY(ProtectedStream);
private:
	HCProtectedStream(const HCProtectedStream &);
	HCProtectedStream &operator=(const HCProtectedStream &);
public:

	/// Ctor.
	HCProtectedStream(HCStream & s) : m_s(&s), m_autodelete(false)
	{
	}

	/// Ctor.
	HCProtectedStream(HCStream * s, bool autodelete = true) :
		m_s(s), m_autodelete(autodelete)
	{
		assert(m_s != NULL);
	}

	/// Dtor.
	virtual ~HCProtectedStream()
	{
		if (m_autodelete) {
			delete m_s;
		}
	}

	/** @name Stream implementation. */
	//@{
	/// Read data.
	virtual uint serialize(void * data, uint len)
	{
		assert(data != NULL);
		len = m_s->serialize(data, len);

		if (m_s->isError()) {
			throw;
		}

		return len;
	}

	virtual void seek(uint pos)
	{
		m_s->seek(pos);

		if (m_s->isError()) {
			throw;
		}
	}

	virtual uint tell() const
	{
		return m_s->tell();
	}

	virtual uint size() const
	{
		return m_s->size();
	}

	virtual bool isError() const
	{
		return m_s->isError();
	}

	virtual void clearError()
	{
		m_s->clearError();
	}

	virtual bool isAtEnd() const
	{
		return m_s->isAtEnd();
	}

	virtual bool isSeekable() const
	{
		return m_s->isSeekable();
	}

	virtual bool isLoading() const
	{
		return m_s->isLoading();
	}

	virtual bool isSaving() const
	{
		return m_s->isSaving();
	}
	//@}


private:

	HCStream * const m_s;
	bool const m_autodelete;

};
