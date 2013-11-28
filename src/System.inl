#include <array>

template<typename T>
T CreateZeroed() {
	T t;
	memset( &t, 0, sizeof( T ) );
	return std::move( t );
}

template<typename T, std::size_t N>
std::array<T, N> CreateZeroed() {
	std::array<T, N> a;
	memset( a.data(), 0, sizeof( T ) * N );
	return std::move( a );
}
