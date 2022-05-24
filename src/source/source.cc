#include "source.h"

namespace NKaleidoscope {

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

} // namespace NKaleidoscope
