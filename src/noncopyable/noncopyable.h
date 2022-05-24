#pragma once

namespace NKaleidoscope {

struct TNonCopyable {
    TNonCopyable(const TNonCopyable&) = delete;
    TNonCopyable& operator=(const TNonCopyable&) = delete;

    TNonCopyable() = default;
    ~TNonCopyable() = default;
};

} // namespace NKaleidoscope
