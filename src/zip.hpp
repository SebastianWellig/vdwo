#pragma once

#include <utility>
#include "debug.hpp"

template <template <typename, std::size_t> class V, std::size_t N, typename ...Ts>
class Zip {
    public:
        Zip(V<Ts, N>& ...args);
        
        std::tuple<Ts&...> operator[](int column_idx);
        std::tuple<Ts&...> at(int column_idx);
        
        class iterator {
            public:
                iterator(Zip<V,N,Ts...>* zip, int idx);
                iterator operator++();
                bool operator!=(iterator& other);
                std::tuple<Ts&...> operator*();
            protected:
                int _idx = 0;
                Zip<V,N,Ts...>* _zip;
        };
        
        iterator begin();
        
        iterator end();
        
    protected:
        std::tuple<V<Ts, N>*...> _data;
        
        template<std::size_t ...Is>
        std::tuple<Ts&...> _zipper(int& column_idx, std::index_sequence<Is...>);
        
        template<std::size_t ...Is>
        std::tuple<Ts&...> _zipper_s(int& column_idx, std::index_sequence<Is...>);  
};

#include "zip.tpp"

