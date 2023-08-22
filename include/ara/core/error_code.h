// --------------------------------------------------------------------------
// |              _    _ _______     .----.      _____         _____        |
// |         /\  | |  | |__   __|  .  ____ .    / ____|  /\   |  __ \       |
// |        /  \ | |  | |  | |    .  / __ \ .  | (___   /  \  | |__) |      |
// |       / /\ \| |  | |  | |   .  / / / / v   \___ \ / /\ \ |  _  /       |
// |      / /__\ \ |__| |  | |   . / /_/ /  .   ____) / /__\ \| | \ \       |
// |     /________\____/   |_|   ^ \____/  .   |_____/________\_|  \_\      |
// |                              . _ _  .                                  |
// --------------------------------------------------------------------------
//
// All Rights Reserved.
// Any use of this source code is subject to a license agreement with the
// AUTOSAR development cooperation.
// More information is available at www.autosar.org.
//
// Disclaimer
//
// This work (specification and/or software implementation) and the material
// contained in it, as released by AUTOSAR, is for the purpose of information
// only. AUTOSAR and the companies that have contributed to it shall not be
// liable for any use of the work.
//
// The material contained in this work is protected by copyright and other
// types of intellectual property rights. The commercial exploitation of the
// material contained in this work requires a license to such intellectual
// property rights.
//
// This work may be utilized or reproduced without any modification, in any
// form or by any means, for informational purposes only. For any other
// purpose, no part of the work may be utilized or reproduced, in any form
// or by any means, without permission in writing from the publisher.
//
// The work has been developed for automotive applications only. It has
// neither been developed, nor tested for non-automotive applications.
//
// The word AUTOSAR and the AUTOSAR logo are registered trademarks.
// --------------------------------------------------------------------------

#ifndef APD_ARA_CORE_ERROR_CODE_H_
#define APD_ARA_CORE_ERROR_CODE_H_

#include "ara/core/error_domain.h"
#include "ara/core/string_view.h"

#include <ostream>
#include <utility>
#include <exception>
#include <type_traits>

namespace ara
{
namespace core
{

/// @uptrace{SWS_CORE_00501, be5b5489048bfb41411da8f88111f5102e2e71dd}
/// @uptrace{SWS_CORE_10300, fb702eea3858e6b102d5b5115462ee151ae7fe51}
class ErrorCode
{
    friend std::ostream& operator<<(std::ostream& out, ErrorCode const& e)
    {
        return (out << e.mDomain->Name() << ":" << e.mValue << ":" << e.mSupportData);
    }

    using CodeType = ErrorDomain::CodeType;
    using SupportDataType = ErrorDomain::SupportDataType;

public:
    /// @uptrace{SWS_CORE_00512, e2ae56420dfd8ea09ada3e24785d23b66e41a17a}
    template <typename EnumT, typename = typename std::enable_if<std::is_enum<EnumT>::value>::type>
    constexpr ErrorCode(EnumT e, SupportDataType data = 0) noexcept
        // Call MakeErrorCode() unqualified, so the correct overload is found via ADL.
        : ErrorCode(MakeErrorCode(e, data))
    {}

    /// @uptrace{SWS_CORE_00513, 781fc3757b508e288831cf3e6a7fed7998a47a4e}
    constexpr ErrorCode(CodeType value, ErrorDomain const& domain, SupportDataType data = 0) noexcept
        : mValue(value)
        , mSupportData(data)
        , mDomain(&domain)
    {}

    /// @uptrace{SWS_CORE_00514, f6ac8b25fb9bef30366995ab5830c63ccc1268f5}
    constexpr CodeType Value() const noexcept
    {
        return mValue;
    }

    /// @uptrace{SWS_CORE_00516, 8dbb809651ded2a3beb28b7f2e1aa978e71ba394}
    constexpr SupportDataType SupportData() const noexcept
    {
        return mSupportData;
    }

    /// @uptrace{SWS_CORE_00515, 31b1f3c15edf4b1f31c2eee929e70ef3e13ead12}
    constexpr ErrorDomain const& Domain() const noexcept
    {
        return *mDomain;
    }

    /// @uptrace{SWS_CORE_00518, b179630f0d8ac36744b356ae182c1c3543b0787d}
    StringView Message() const noexcept
    {
        return Domain().Message(Value());
    }

    /// @uptrace{SWS_CORE_00519, 04e88d14e2394731903c19d5f58adf122c262121}
    [[noreturn]] void ThrowAsException() const noexcept(false)
    {
        Domain().ThrowAsException(*this);

        // Never reached, but apparently needed to avoid warnings from certain compilers (such as 5.4.0).
        std::terminate();
    }

private:
    CodeType mValue;
    SupportDataType mSupportData;
    ErrorDomain const* mDomain;  // non-owning pointer to the associated ErrorDomain
};

/// @uptrace{SWS_CORE_00571, d8bc4234a29160b85a772366563541b5e67bc6c2}
constexpr inline bool operator==(ErrorCode const& lhs, ErrorCode const& rhs)
{
    return lhs.Domain() == rhs.Domain() && lhs.Value() == rhs.Value();
}

/// @uptrace{SWS_CORE_00572, 34b7ad493630cce977c3b7a938dc5057d4e0a1e2}
constexpr inline bool operator!=(ErrorCode const& lhs, ErrorCode const& rhs)
{
    return lhs.Domain() != rhs.Domain() || lhs.Value() != rhs.Value();
}

template <typename ExceptionType>
void ThrowOrTerminate(ErrorCode errorCode)
{
#ifndef ARA_NO_EXCEPTIONS
    throw ExceptionType(std::move(errorCode));
#else
    (void)errorCode;
    std::terminate();
#endif
}

}  // namespace core
}  // namespace ara

#endif  // APD_ARA_CORE_ERROR_CODE_H_
