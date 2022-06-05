#include "source.h"

namespace NKaleidoscope {

// TSource
std::string_view TSource::GetBuffer() const {
    return Buffer_;
}

const std::string* TSource::GetFileName() const {
    return FileName_ ? &FileName_.value() : nullptr;
}

TSource::TSource(std::optional<std::string> fileName, std::string buffer)
    : FileName_{std::move(fileName)}
    , Buffer_{std::move(buffer)}
{}

TSource TSource::FromString(std::string buffer) {
    return TSource{/* fileName = */ std::nullopt, std::move(buffer)};
}

// TSourceLocation
std::string_view TSourceRange::AsStringView() const {
    const auto* data = Source->GetBuffer().data();
    return {data + Offset, Length};
}

double TSourceRange::AsDouble() const {
    const std::string_view sv = AsStringView();
    char* strEnd = const_cast<char*>(sv.data()) + Length;
    return std::strtod(sv.data(), &strEnd);
}

} // namespace NKaleidoscope
