#include <array>
#include <cstring>

namespace sys {

template<typename T>
T create_zeroed() {
	T t;
	std::memset( &t, 0, sizeof( T ) );
	return std::move( t );
}

template<typename T, std::size_t N>
std::array<T, N> create_zeroed() {
	std::array<T, N> a;
	std::memset( a.data(), 0, sizeof( T ) * N );
	return std::move( a );
}

}
