#ifndef DIPLOM_SRC_HELP_FUNCTION_H_
#define DIPLOM_SRC_HELP_FUNCTION_H_

using byte  = unsigned char;
using fbyte = unsigned int;

namespace diplomproject {

inline constexpr fbyte ToUintFromBytes(const byte* buffer) {
  return (buffer[0] << 24) | (buffer[1] << 16) |
    (buffer[2] << 8) | buffer[3];
}

inline constexpr fbyte ChangeEndianUInt32(fbyte data) noexcept {
  return ((data & 0x000000FF) << 24) |
    ((data & 0x0000FF00) << 8) |
    ((data & 0x00FF0000) >> 8) |
    ((data & 0xFF000000) >> 24);
}

}; // diplomproject

#endif // !DIPLOM_SRC_HELP_FUNCTION_H_
