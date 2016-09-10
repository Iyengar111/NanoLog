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

#ifndef NANO_LOG_HEADER_GUARD
#define NANO_LOG_HEADER_GUARD

#include <stdint.h>
#include <memory>
#include <string>
#include <iosfwd>
#include <type_traits>

namespace nanolog
{
    enum class LogLevel : uint8_t { INFO, WARN, CRIT };
    
    class NanoLogLine
    {
    public:
	NanoLogLine(LogLevel level, char const * file, char const * function, uint32_t line);
	~NanoLogLine();

	NanoLogLine(NanoLogLine &&) = default;
	NanoLogLine& operator=(NanoLogLine &&) = default;

	void stringify(std::ostream & os);

	NanoLogLine& operator<<(char arg);
	NanoLogLine& operator<<(int32_t arg);
	NanoLogLine& operator<<(uint32_t arg);
	NanoLogLine& operator<<(int64_t arg);
	NanoLogLine& operator<<(uint64_t arg);
	NanoLogLine& operator<<(double arg);
	NanoLogLine& operator<<(std::string const & arg);

	template < size_t N >
	NanoLogLine& operator<<(const char (&arg)[N])
	{
	    encode(string_literal_t(arg));
	    return *this;
	}

	template < typename Arg >
	typename std::enable_if < std::is_same < Arg, char const * >::value, NanoLogLine& >::type
	operator<<(Arg const & arg)
	{
	    encode(arg);
	    return *this;
	}

	template < typename Arg >
	typename std::enable_if < std::is_same < Arg, char * >::value, NanoLogLine& >::type
	operator<<(Arg const & arg)
	{
	    encode(arg);
	    return *this;
	}

	struct string_literal_t
	{
	    explicit string_literal_t(char const * s) : m_s(s) {}
	    char const * m_s;
	};

    private:	
	char * buffer();

	template < typename Arg >
	void encode(Arg arg);

	template < typename Arg >
	void encode(Arg arg, uint8_t type_id);

	void encode(char * arg);
	void encode(char const * arg);
	void encode(string_literal_t arg);
	void encode_c_string(char const * arg, size_t length);
	void resize_buffer_if_needed(size_t additional_bytes);
	void stringify(std::ostream & os, char * start, char const * const end);

    private:
	uint32_t m_bytes_used;
	uint32_t m_buffer_size;
	std::unique_ptr < char [] > m_heap_buffer;
	char m_stack_buffer[256 - 2 * sizeof(uint32_t) - sizeof(decltype(m_heap_buffer)) - 8 /* Reserved */];
    };
    
    struct NanoLog
    {
	/*
	 * Ideally this should have been operator+=
	 * Could not get that to compile, so here we are...
	 */
	bool operator==(NanoLogLine &);
    };

    void set_log_level(LogLevel level);
    
    bool is_logged(LogLevel level);
    
    /*
     * Ensure initialize() is called prior to any log statements.
     * log_directory - where to create the logs. For example - "/tmp/"
     * log_file_name - root of the file name. For example - "nanolog"
     * This will create log files of the form -
     * /tmp/nanolog.1.txt
     * /tmp/nanolog.2.txt
     * etc.
     */
    void initialize(std::string const & log_directory, std::string const & log_file_name, uint32_t log_file_roll_size_mb);

} // namespace nanolog

#define LOG(LEVEL) nanolog::NanoLog() == nanolog::NanoLogLine(LEVEL, __FILE__, __PRETTY_FUNCTION__, __LINE__)
#define LOG_INFO nanolog::is_logged(nanolog::LogLevel::INFO) && LOG(nanolog::LogLevel::INFO)
#define LOG_WARN nanolog::is_logged(nanolog::LogLevel::WARN) && LOG(nanolog::LogLevel::WARN)
#define LOG_CRIT nanolog::is_logged(nanolog::LogLevel::CRIT) && LOG(nanolog::LogLevel::CRIT)

#endif /* NANO_LOG_HEADER_GUARD */

