#ifndef GAME_OF_LIFE_ARRAY_SIZE_HPP
#define GAME_OF_LIFE_ARRAY_SIZE_HPP

#include <cstddef>

// Helper to get the array size statically
template<typename TArrayType, std::size_t TArraySize>
constexpr std::size_t array_size(const TArrayType (&array)[TArraySize]) { return TArraySize; }

#endif // GAME_OF_LIFE_ARRAY_SIZE_HPP
