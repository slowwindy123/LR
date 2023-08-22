#ifndef ARALOGCOMMON_H__
#define ARALOGCOMMON_H__

#include <memory>
#include <string>
#include "ara/core/string_view.h"
#include "ara/core/error_code.h"
#include "ara/core/instance_specifier.h"
#include "ara/core/span.h"
#include "ara/core/utility.h"

namespace ara {
namespace log {

enum class LogLevel : uint8_t {
  kOff      = 0x00, ///< No logging
  kFatal    = 0x01, ///< Fatal error, not recoverable
  kError    = 0x02, ///< Error with impact to correct functionality
  kWarn     = 0x03, ///< Warning if correct behavior cannot be ensured
  kInfo     = 0x04, ///< Informational, providing high level understanding
  kDebug    = 0x05, ///< Detailed information for programmers
  kVerbose  = 0x06  ///< Extra-verbose debug messages (highest grade of information)
};

enum class LogMode : uint8_t {
  kRemote   = 0x01,  ///< Sent remotely
  kFile     = 0x02,  ///< Save to file
  kConsole  = 0x04   ///< Forward to console
};


namespace internal {

enum class  LogReturnValue : int8_t {
    kReturnLoggingDisabled  = -7,
    kReturnUserBufferFull   = -6,
    kReturnWrongParameter   = -5,
    kReturnBufferFull       = -4,
    kReturnPipeFull         = -3,
    kReturnPipeError        = -2,
    kReturnError            = -1,
    kReturnOk               = 0,
    kReturnTrue             = 1
};

}; // namespace internal

//for Wrapper object creator
template <typename... T>
struct Regist {};

template <typename U, typename... T>
struct isRegister {};
template <typename U, typename t, typename... T>
struct isRegister<U, t, T...> {
    enum { value = std::is_same<t, U>::value || isRegister<U, T...>::value };
};
template <typename U>
struct isRegister<U> { enum { value = 0 }; };


template <typename T, typename... U>
bool isValid(const T& a, const Regist<U...>& b) {
    return isRegister<T, U...>::value;
};

typedef Regist<int, double, float, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, uint64_t, std::string ,char*, const char*, const char* const, long long, long double,\
 char,bool, core::StringView, core::Span<const core::Byte>> Reg;//注册的类型，常见算术类以及core的两个特例化类型
//for Special case template

template<class T>
class Argument
{
public:
    Argument<T>(T t, const char* name, const char* unit):t_(t),name_(name),unit_(unit){}
    T t_;
    const char* name_;
    const char* unit_;
};
template <typename M>
Argument<M> Arg(M &arg, const char* name = nullptr, const char* unit = nullptr) noexcept
{
    if(isValid(arg, Reg()))
        return Argument<M>(std::forward<M>(arg),name,unit);
    int tmpbuffer = 0;
    return Argument<M>(std::forward<M>((M)tmpbuffer),nullptr,nullptr);
}
//for Wrapper object creator

inline LogMode operator|(LogMode lhs, LogMode rhs)
{
    return (static_cast<LogMode>(static_cast<typename std::underlying_type<LogMode>::type>(lhs) |
                                 static_cast<typename std::underlying_type<LogMode>::type>(rhs)));
}

inline typename std::underlying_type<LogMode>::type operator&(LogMode lhs, LogMode rhs)
{
    return (static_cast<typename std::underlying_type<LogMode>::type>(lhs) &
            static_cast<typename std::underlying_type<LogMode>::type>(rhs));
}

/// @}
} /* namespace log */
} /* namespace ara */

#endif  // ARALOGCOMMON_H__
