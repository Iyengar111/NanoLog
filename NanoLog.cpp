/*

Distributed under the MIT License (MIT)

    Copyright (c) 2016 Karthik Iyengar

Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the "Software"), to deal in the 
Software without restriction, including without limitation the rights to 
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
of the Software, and to permit persons to whom the Software is furnished 
to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included 
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS 
OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include "NanoLog.hpp"
#include <cstring>
#include <chrono>
#include <thread>
#include <tuple>
#include <atomic>
#include <fstream>

namespace
{

    /* Returns microseconds since epoch */
    uint64_t timestamp_now()
    {
    	return std::chrono::high_resolution_clock::now().time_since_epoch() / std::chrono::microseconds(1);
    }

    std::thread::id this_thread_id()
    {
	static thread_local const std::thread::id id = std::this_thread::get_id();
	return id;
    }

    template < typename T, typename Tuple >
    struct TupleIndex;

    template < typename T,typename ... Types >
    struct TupleIndex < T, std::tuple < T, Types... > > 
    {
	static constexpr const std::size_t value = 0;
    };

    template < typename T, typename U, typename ... Types >
    struct TupleIndex < T, std::tuple < U, Types... > > 
    {
	static constexpr const std::size_t value = 1 + TupleIndex < T, std::tuple < Types... > >::value;
    };

} // anonymous namespace

namespace nanolog
{
    typedef std::tuple < char, uint32_t, uint64_t, int32_t, int64_t, double, NanoLogLine::string_literal_t, char * > SupportedTypes;

    char const * to_string(LogLevel loglevel)
    {
	switch (loglevel)
	{
	case LogLevel::INFO:
	    return "INFO";
	case LogLevel::WARN:
	    return "WARN";
	case LogLevel::CRIT:
	    return "CRIT";
	}
	return "XXXX";
    }

    template < typename Arg >
    void NanoLogLine::encode(Arg arg)
    {
	*reinterpret_cast<Arg*>(buffer()) = arg;
	m_bytes_used += sizeof(Arg);
    }

    template < typename Arg >
    void NanoLogLine::encode(Arg arg, uint8_t type_id)
    {
	resize_buffer_if_needed(sizeof(Arg) + sizeof(uint8_t));
	encode < uint8_t >(type_id);
	encode < Arg >(arg);
    }

    NanoLogLine::NanoLogLine(LogLevel level, char const * file, char const * function, uint32_t line)
	: m_bytes_used(0)
	, m_buffer_size(sizeof(m_stack_buffer))
    {
	encode < uint64_t >(timestamp_now());
	encode < std::thread::id >(this_thread_id());
	encode < string_literal_t >(string_literal_t(file));
	encode < string_literal_t >(string_literal_t(function));
	encode < uint32_t >(line);
	encode < LogLevel >(level);
    }

    NanoLogLine::~NanoLogLine() = default;

    void NanoLogLine::stringify(std::ostream & os)
    {
	char * b = !m_heap_buffer ? m_stack_buffer : m_heap_buffer.get();
	char const * const end = b + m_bytes_used;
	uint64_t timestamp = *reinterpret_cast < uint64_t * >(b); b += sizeof(uint64_t);
	std::thread::id threadid = *reinterpret_cast < std::thread::id * >(b); b += sizeof(std::thread::id);
	string_literal_t file = *reinterpret_cast < string_literal_t * >(b); b += sizeof(string_literal_t);
	string_literal_t function = *reinterpret_cast < string_literal_t * >(b); b += sizeof(string_literal_t);
	uint32_t line = *reinterpret_cast < uint32_t * >(b); b += sizeof(uint32_t);
	LogLevel loglevel = *reinterpret_cast < LogLevel * >(b); b += sizeof(LogLevel);

	os << '[' << timestamp << ']'
	   << '[' << to_string(loglevel) << ']'
	   << '[' << threadid << ']'
	   << '[' << file.m_s << ':' << function.m_s << ':' << line << "] ";

	stringify(os, b, end);

	os << std::endl;
    }

    template < typename Arg >
    char * decode(std::ostream & os, char * b, Arg * dummy)
    {
	Arg arg = *reinterpret_cast < Arg * >(b);
	os << arg;
	return b + sizeof(Arg);
    }

    template <>
    char * decode(std::ostream & os, char * b, NanoLogLine::string_literal_t * dummy)
    {
	NanoLogLine::string_literal_t s = *reinterpret_cast < NanoLogLine::string_literal_t * >(b);
	os << s.m_s;
	return b + sizeof(NanoLogLine::string_literal_t);
    }

    template <>
    char * decode(std::ostream & os, char * b, char ** dummy)
    {
	while (*b != '\0')
	{
	    os << *b;
	    ++b;
	}
	return ++b;
    }

    void NanoLogLine::stringify(std::ostream & os, char * start, char const * const end)
    {
	if (start == end)
	    return;

	int type_id = static_cast < int >(*start); start++;
	
	switch (type_id)
	{
	case 0:
	    stringify(os, decode(os, start, static_cast<std::tuple_element<0, SupportedTypes>::type*>(nullptr)), end);
	    return;
	case 1:
	    stringify(os, decode(os, start, static_cast<std::tuple_element<1, SupportedTypes>::type*>(nullptr)), end);
	    return;
	case 2:
	    stringify(os, decode(os, start, static_cast<std::tuple_element<2, SupportedTypes>::type*>(nullptr)), end);
	    return;
	case 3:
	    stringify(os, decode(os, start, static_cast<std::tuple_element<3, SupportedTypes>::type*>(nullptr)), end);
	    return;
	case 4:
	    stringify(os, decode(os, start, static_cast<std::tuple_element<4, SupportedTypes>::type*>(nullptr)), end);
	    return;
	case 5:
	    stringify(os, decode(os, start, static_cast<std::tuple_element<5, SupportedTypes>::type*>(nullptr)), end);
	    return;
	case 6:
	    stringify(os, decode(os, start, static_cast<std::tuple_element<6, SupportedTypes>::type*>(nullptr)), end);
	    return;
	case 7:
	    stringify(os, decode(os, start, static_cast<std::tuple_element<7, SupportedTypes>::type*>(nullptr)), end);
	    return;
	}
    }

    char * NanoLogLine::buffer()
    {
	return !m_heap_buffer ? &m_stack_buffer[m_bytes_used] : &(m_heap_buffer.get())[m_bytes_used];
    }
    
    void NanoLogLine::resize_buffer_if_needed(size_t additional_bytes)
    {
	size_t const required_size = m_bytes_used + additional_bytes;

	if (required_size <= m_buffer_size)
	    return;

	if (!m_heap_buffer)
	{
	    m_buffer_size = std::max(static_cast<size_t>(512), required_size);
	    m_heap_buffer.reset(new char[m_buffer_size]);
	    memcpy(m_heap_buffer.get(), m_stack_buffer, m_bytes_used);
	    return;
	}
	else
	{
	    m_buffer_size = std::max(static_cast<size_t>(2 * m_buffer_size), required_size);
	    std::unique_ptr < char [] > new_heap_buffer(new char[m_buffer_size]);
	    memcpy(new_heap_buffer.get(), m_heap_buffer.get(), m_bytes_used);
	    m_heap_buffer.swap(new_heap_buffer);
	}
    }

    void NanoLogLine::encode(char const * arg)
    {
	if (arg != nullptr)
	    encode_c_string(arg, strlen(arg));
    }

    void NanoLogLine::encode(char * arg)
    {
	if (arg != nullptr)
	    encode_c_string(arg, strlen(arg));
    }

    void NanoLogLine::encode_c_string(char const * arg, size_t length)
    {
	if (length == 0)
	    return;
	
	resize_buffer_if_needed(1 + length + 1);
	char * b = buffer();
	auto type_id = TupleIndex < char *, SupportedTypes >::value;
	*reinterpret_cast<uint8_t*>(b++) = static_cast<uint8_t>(type_id);
	memcpy(b, arg, length + 1);
	m_bytes_used += 1 + length + 1;
    }

    void NanoLogLine::encode(string_literal_t arg)
    {
	encode < string_literal_t >(arg, TupleIndex < string_literal_t, SupportedTypes >::value);
    }

    NanoLogLine& NanoLogLine::operator<<(std::string const & arg)
    {
	encode_c_string(arg.c_str(), arg.length());
	return *this;
    }

    NanoLogLine& NanoLogLine::operator<<(int32_t arg)
    {
	encode < int32_t >(arg, TupleIndex < int32_t, SupportedTypes >::value);
	return *this;
    }

    NanoLogLine& NanoLogLine::operator<<(uint32_t arg)
    {
	encode < uint32_t >(arg, TupleIndex < uint32_t, SupportedTypes >::value);
	return *this;
    }

    NanoLogLine& NanoLogLine::operator<<(int64_t arg)
    {
	encode < int64_t >(arg, TupleIndex < int64_t, SupportedTypes >::value);
	return *this;
    }

    NanoLogLine& NanoLogLine::operator<<(uint64_t arg)
    {
	encode < uint64_t >(arg, TupleIndex < uint64_t, SupportedTypes >::value);
	return *this;
    }

    NanoLogLine& NanoLogLine::operator<<(double arg)
    {
	encode < double >(arg, TupleIndex < double, SupportedTypes >::value);
	return *this;
    }

    NanoLogLine& NanoLogLine::operator<<(char arg)
    {
	encode < char >(arg, TupleIndex < char, SupportedTypes >::value);
	return *this;
    }


    /* Multi Producer Single Consumer Ring Buffer */
    class RingBuffer
    {
    public:
    	struct alignas(64) Item
    	{
	    Item() 
		: logline(LogLevel::INFO, __FILE__, __FUNCTION__, __LINE__)
		, written(0)
		, flag(ATOMIC_FLAG_INIT)
	    {
	    }

	    NanoLogLine logline;
    	    char written;
    	    std::atomic_flag flag;
	    char padding[256 - sizeof(std::atomic_flag) - sizeof(char) - sizeof(NanoLogLine)];
    	};

    	struct SpinLock
    	{
    	    SpinLock(std::atomic_flag & flag) : m_flag(flag)
    	    {
    		while (m_flag.test_and_set(std::memory_order_acquire));
    	    }

    	    ~SpinLock()
    	    {
    		m_flag.clear(std::memory_order_release);
    	    }

    	private:
	    std::atomic_flag & m_flag;
    	};
	
    	RingBuffer(size_t const size) 
    	    : m_size(size)
    	    , m_ring(static_cast<Item*>(std::malloc(size * sizeof(Item))))
    	    , m_read_index(0)
    	    , m_write_index(0)
    	{
    	    for (size_t i = 0; i < m_size; ++i)
    	    {
    		new (&m_ring[i]) Item();
    	    }
	    static_assert(sizeof(Item) == 256, "Unexpected size != 256");
    	}

    	~RingBuffer()
    	{
    	    for (size_t i = 0; i < m_size; ++i)
    	    {
    		m_ring[i].~Item();
    	    }
    	    std::free(m_ring);
    	}

    	void push(NanoLogLine && logline)
    	{
    	    unsigned int write_index = m_write_index.fetch_add(1, std::memory_order_relaxed) % m_size;
    	    Item & item = m_ring[write_index];
    	    SpinLock spinlock(item.flag);
	    item.logline = std::move(logline);
	    item.written = 1;
    	}

    	bool try_pop(NanoLogLine & logline)
    	{
    	    Item & item = m_ring[m_read_index % m_size];
    	    SpinLock spinlock(item.flag);
    	    if (item.written == 1)
    	    {
    		logline = std::move(item.logline);
    		item.written = 0;
		++m_read_index;
    		return true;
    	    }
    	    return false;
    	}

    	RingBuffer(RingBuffer const &) = delete;	
    	RingBuffer& operator=(RingBuffer const &) = delete;

    private:
    	size_t const m_size;
    	Item * m_ring;
	char pad0[64];
    	unsigned int m_read_index;
	char pad1[64];
    	std::atomic < unsigned int > m_write_index;
    };

    class FileWriter
    {
    public:
	FileWriter(std::string const & log_directory, std::string const & log_file_name, uint32_t log_file_roll_size_mb)
	    : m_log_file_roll_size_bytes(log_file_roll_size_mb * 1024 * 1024)
	    , m_name(log_directory + log_file_name)
	{
	    roll_file();
	}
	
	void write(NanoLogLine & logline)
	{
	    auto pos = m_os->tellp();
	    logline.stringify(*m_os);
	    m_bytes_written += m_os->tellp() - pos;
	    if (m_bytes_written > m_log_file_roll_size_bytes)
	    {
		roll_file();
	    }
	}

    private:
	void roll_file()
	{
	    if (m_os)
	    {
		m_os->flush();
		m_os->close();
	    }

	    m_bytes_written = 0;
	    m_os.reset(new std::ofstream());
	    std::string log_file_name = m_name;
	    log_file_name.append(std::to_string(++m_file_number));
	    log_file_name.append(".txt");
	    m_os->open(log_file_name, std::ofstream::out | std::ofstream::app);
	}

    private:
	uint32_t m_file_number = 0;
	uint32_t m_bytes_written = 0;
	uint32_t const m_log_file_roll_size_bytes;
	std::string const m_name;
	std::unique_ptr < std::ofstream > m_os;
    };

    class NanoLogger
    {
    public:
	NanoLogger(std::string const & log_directory, std::string const & log_file_name, uint32_t log_file_roll_size_mb)
	    : m_disabled(0)
	    , m_thread(&NanoLogger::pop, this)
	    , m_ring_buffer(4 * 1024 * 4)
	    , m_file_writer(log_directory, log_file_name, log_file_roll_size_mb)
	{
	}

	~NanoLogger()
	{
	    m_disabled.store(1);
	    m_thread.join();
	}

	void add(NanoLogLine && logline)
	{
	    m_ring_buffer.push(std::move(logline));
	}
	
	void pop()
	{
	    NanoLogLine logline(LogLevel::INFO, __FILE__, __FUNCTION__, __LINE__);

	    while (!m_disabled.load())
	    {
		if (m_ring_buffer.try_pop(logline))
		    m_file_writer.write(logline);
		else
		    std::this_thread::sleep_for(std::chrono::microseconds(50));
	    }
	    
	    // Pop and log all remaining entries
	    while (m_ring_buffer.try_pop(logline))
	    {
		m_file_writer.write(logline);
	    }
	}
	
    private:
	std::atomic < unsigned int > m_disabled;
	std::thread m_thread;
	RingBuffer m_ring_buffer;
	FileWriter m_file_writer;
    };

    std::unique_ptr < NanoLogger > nanologger;

    bool NanoLog::operator==(NanoLogLine & logline)
    {
	nanologger->add(std::move(logline));
	return true;
    }

    void initialize(std::string const & log_directory, std::string const & log_file_name, uint32_t log_file_roll_size_mb)
    {
	nanologger.reset(new NanoLogger(log_directory, log_file_name, log_file_roll_size_mb));
    }

    std::atomic < unsigned int > loglevel = {0};

    void set_log_level(LogLevel level)
    {
	loglevel.store(static_cast<unsigned int>(level), std::memory_order_release);
    }

    bool is_logged(LogLevel level)
    {
	return static_cast<unsigned int>(level) >= loglevel.load(std::memory_order_relaxed);
    }

} // namespace nanologger
