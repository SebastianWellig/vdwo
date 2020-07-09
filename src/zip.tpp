template <template <typename, std::size_t> class V, std::size_t N, typename ...Ts>
Zip<V,N,Ts...>::Zip(V<Ts, N>& ...args) : _data(std::tuple<V<Ts, N>*...> {&args...}) {
};

template <template <typename, std::size_t> class V, std::size_t N, typename ...Ts>
std::tuple<Ts&...> Zip<V,N,Ts...>::operator[](int column_idx) {
    return _zipper(column_idx, std::make_index_sequence<sizeof...(Ts)>{});
};

template <template <typename, std::size_t> class V, std::size_t N, typename ...Ts>
std::tuple<Ts&...> Zip<V,N,Ts...>::at(int column_idx) {
    return _zipper_s(column_idx, std::make_index_sequence<sizeof...(Ts)>{});
};

template <template <typename, std::size_t> class V, std::size_t N, typename ...Ts>
Zip<V,N,Ts...>::iterator::iterator(Zip<V,N,Ts...>* zip, int idx): _zip(zip), _idx(idx) {
};

template <template <typename, std::size_t> class V, std::size_t N, typename ...Ts>
typename Zip<V,N,Ts...>::iterator Zip<V,N,Ts...>::iterator::operator++() {
    _idx++;
    return *this;
};

template <template <typename, std::size_t> class V, std::size_t N, typename ...Ts>
bool Zip<V,N,Ts...>::iterator::operator!=(iterator& other) {
    return _idx != other._idx;
};

template <template <typename, std::size_t> class V, std::size_t N, typename ...Ts>
std::tuple<Ts&...> Zip<V,N,Ts...>::iterator::operator*() {
    return (*_zip)[_idx];
};

template <template <typename, std::size_t> class V, std::size_t N, typename ...Ts>
typename Zip<V,N,Ts...>::iterator Zip<V,N,Ts...>::begin() {
    return iterator(this, 0);
};

template <template <typename, std::size_t> class V, std::size_t N, typename ...Ts>
typename Zip<V,N,Ts...>::iterator Zip<V,N,Ts...>::end() {
    return iterator(this, std::get<0>(_data)->size());
};

template<template <typename, std::size_t> class V, std::size_t N, typename ...Ts>
template<std::size_t ...Is>
std::tuple<Ts&...> Zip<V,N,Ts...>::_zipper(int& column_idx,
                                        std::index_sequence<Is...>) {
    ASSERT((column_idx < std::get<Is>(_data)->size())&&...,
            "Zip element is out of range")
    return std::forward_as_tuple((*std::get<Is>(_data))[column_idx]...);
};

template<template <typename, std::size_t> class V, std::size_t N, typename ...Ts>
template<std::size_t ...Is>
std::tuple<Ts&...> Zip<V,N,Ts...>::_zipper_s(int& column_idx, std::index_sequence<Is...>) {
    return std::forward_as_tuple((*std::get<Is>(_data)).at(column_idx)...);
};
