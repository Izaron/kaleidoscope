#pragma once

#include <optional>
#include <string>

#include "noncopyable.h"

namespace NKaleidoscope {

class TSource : private TNonCopyable {
public:
    std::string_view GetBuffer() const;
    const std::string* GetFileName() const;

    static TSource FromString(std::string buffer);
    // TODO: add FromFile

private:
    TSource(std::optional<std::string> fileName, std::string buffer);

private:
    std::optional<std::string> FileName_;
    std::string Buffer_;
};

// Points to a contigious range of a source
struct TSourceLocation {
    const TSource* Source;
    std::size_t Offset;
    std::size_t Length;
};

} // namespace NKaleidoscope
