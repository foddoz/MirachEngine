#ifndef ECS_TYPES_HPP
#define ECS_TYPES_HPP

#include <cstdint>

using Entity = uint32_t;

constexpr Entity INVALID_ENTITY = static_cast<Entity>(-1);

#endif