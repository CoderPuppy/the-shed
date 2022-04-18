#ifndef helpers_hpp
#define helpers_hpp

#include "includes.hpp"

string opToString(BusALU::Operation op);
string toHexString(int fill, long value);

template<typename T>
T *make_array(size_t n, function<void(size_t, T*)> init) {
  T *arr = (T*) ::operator new(n * sizeof(T));
  for (int i = 0; i < n; i++) {
    init(i, &arr[i]);
  }
  return arr;
}

#endif
