#pragma once

namespace bmin {

template <typename T>
constexpr T&& move(T& t) noexcept {
  return static_cast<T&&>(t);
}

template <typename T>
constexpr T&& forward(T& t) noexcept {
  return static_cast<T&&>(t);
}

template <typename T>
void swap(T& a, T& b) noexcept {
  T tmp = move(a);
  a = move(b);
  b = move(tmp);
}

template <typename T, typename U = T>
T exchange(T& obj, U&& newValue) {
  T old = move(obj);
  obj = forward<U>(newValue);
  return old;
}

}  // namespace bmin
