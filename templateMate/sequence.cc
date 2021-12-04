#include<iostream>
#include <tuple>
template<size_t ...I>
struct seq{
    using type = seq;
};

template<class ...I>
struct concat;

template<size_t L,size_t ...H>
struct concat<seq<L>,seq<H...>> : public seq<L,(H + 1)...>{};

template<size_t N>
struct make:public concat<seq<0>,typename make<N-1>::type>{};
template<>
struct make<1>:public seq<0>{};
template<>
struct make<0>:public seq<>{};

template<size_t N,size_t F,size_t ...I>
constexpr size_t get(seq<F,I...> q){
    if constexpr(N == 0){
        return F;
    }
    else {
        return get<N-1>(seq<I...>{});
    }
}

int main(){
    auto seq = make<2>{};
    auto ty = make<2>::type{};
    constexpr int x = get<1>(seq);
    //std::cout<<get<2>(seq);
}