/*
 * vector_detail.hpp
 *
 *  Created on: Oct 29, 2014
 *      Author: zmij
 */

#ifndef PUSHKIN_VECTOR_DETAIL_HPP_
#define PUSHKIN_VECTOR_DETAIL_HPP_

#include <type_traits>
#include <utility>

#include <pushkin/math/detail/axis_names.hpp>
#include <pushkin/math/detail/value_traits.hpp>

namespace psst {
namespace math {

namespace detail {

/** Variadic template arguments unwrapping */
template< ::std::size_t Index, typename T >
struct data_holder {
    static constexpr ::std::size_t index = Index;
    using type = T;

    type value;

    constexpr data_holder() = default;
    data_holder(data_holder const&) = default;
    data_holder(data_holder&&) = default;
    data_holder&
    operator = (data_holder const&) = default;
    data_holder&
    operator = (data_holder&&) = default;

    constexpr data_holder(type const& value)
        : value(value) {}

    template< typename Y >
    constexpr data_holder(Y const& value)
        : value(value) {}

    void
    swap(data_holder<Index, T>& rhs)
    {
        using std::swap;
        swap(value, rhs.value);
    }

    template< size_t idx1, typename Y >
    data_holder&
    operator =(const data_holder< idx1, Y >& rhs)
    {
        value = rhs.value;
        return *this;
    }
};

template < typename IndexTuple, typename T >
struct vector_builder;

template < ::std::size_t ... Indexes, typename T >
struct vector_builder< std::index_sequence< Indexes ... >, T > :
    // TODO Replace the data holder with an std::array
    data_holder< Indexes, T > ... {

    static constexpr ::std::size_t size = sizeof ... (Indexes);
    using indexes_tuple_type    = std::index_sequence< Indexes ... >;
    using this_type             = vector_builder< indexes_tuple_type, T >;

    using element_type          = T;
    using value_traits          = vector_value_traits<T>;
    using value_type            = typename value_traits::value_type;
    using lvalue_reference      = typename value_traits::lvalue_reference;
    using const_reference       = typename value_traits::const_reference;

    using magnitude_type        = typename value_traits::magnitude_type;

    using pointer               = value_type*;
    using const_pointer         = value_type const*;

    using iterator              = value_type*;
    using const_iterator        = value_type const*;

    constexpr vector_builder() = default;

    /**
     * Single value constructor, for initializing all values to the value
     * @param val
     */
    constexpr vector_builder(T val)
        : data_holder< Indexes, T >(val) ... {}

    /**
     * Construct vector from a vector of larger size
     * @param rhs
     * @param
     */
    template < size_t ... IndexesR, typename U >
    constexpr vector_builder(
            vector_builder< std::index_sequence< IndexesR ... >, U > const& rhs,
            typename ::std::enable_if< size <= sizeof ... (IndexesR) >::type* = 0 )
        : data_holder< Indexes, T >( rhs.template at< Indexes >() ) ... {}

    template < typename ... E >
    constexpr vector_builder(E const& ... args,
            typename ::std::enable_if< size == sizeof ... (E) >::type* = 0)
        : data_holder< Indexes, T >(args) ... {}

    template < typename ... E >
    constexpr vector_builder(E&& ... args,
            typename ::std::enable_if< size == sizeof ... (E) >::type* = 0)
        : data_holder< Indexes, T >( std::forward<E>(args) ) ... {}

    template < typename U >
    constexpr vector_builder(::std::initializer_list<U> const& args)
        : data_holder< Indexes, T >(*(args.begin() + Indexes))... {}

    constexpr vector_builder(const_pointer p)
        : data_holder< Indexes, T >(*(p + Indexes))... {}

    pointer
    data()
    {
        return &this->data_holder< 0, T >::value;
    }

    constexpr const_pointer
    data() const
    {
        return &this->data_holder< 0, T >::value;
    }

    template < size_t N >
    lvalue_reference
    at()
    {
        return this->data_holder< N, T >::value;
    }

    template < size_t N >
    constexpr const_reference
    at() const
    {
        return this->data_holder< N, T >::value;
    }

    iterator
    begin()
    {
        return &this->data_holder< 0, T >::value;
    }

    constexpr const_iterator
    begin() const
    {
        return cbegin();
    }
    constexpr const_iterator
    cbegin() const
    {
        return &this->data_holder< 0, T >::value;
    }

    iterator
    end()
    {
        return begin() + size;
    }

    constexpr const_iterator
    end() const
    {
        return cend();
    }
    constexpr const_iterator
    cend() const
    {
        return begin() + size;
    }

    lvalue_reference
    operator[](size_t idx)
    {
        assert(idx < size);
        return begin()[idx];
    }

    constexpr const_reference
    operator[](size_t idx) const
    {
        assert(idx < size);
        return begin()[idx];
    }
};

template < ::std::size_t L, ::std::size_t R >
struct min : ::std::conditional<
        L < R,
        ::std::integral_constant<::std::size_t, L>,
        ::std::integral_constant<::std::size_t, R>
    >::type {};

template < ::std::size_t N, typename T, typename U >
struct vector_cmp;

template < ::std::size_t N, typename T, typename U,
    ::std::size_t TSize, ::std::size_t USize,
    typename Axes >
struct vector_cmp<N, vector<T, TSize, Axes>, vector<U, USize, Axes>> {
    using left_side     = vector<T, TSize, Axes>;
    using traits_type   = typename left_side::value_traits;
    using right_side    = vector<U, USize, Axes>;
    using prev_elem     = vector_cmp<N - 1, left_side, right_side>;

    constexpr static bool
    eq(left_side const& lhs, right_side const& rhs)
    {
        return prev_elem::eq(lhs, rhs) &&
                traits_type::eq(lhs.template at<N>(),
                                rhs.template at<N>());
    }

    constexpr static bool
    less(left_side const& lhs, right_side const& rhs)
    {
        auto p = prev_elem::cmp(lhs, rhs);
        return p < 0 ? true : traits_type::less(
                                lhs.template at<N>(),
                                rhs.template at<N>());
    }
    constexpr static int
    cmp(left_side const& lhs, right_side const& rhs)
    {
        auto p = prev_elem::cmp(lhs, rhs);
        if (p == 0) {
            return traits_type::cmp(lhs.template at<N>(),
                    rhs.template at<N>());
        }
        return p;
    }
};

template < typename T, typename U,
        ::std::size_t TSize, ::std::size_t USize,
        typename Axes >
struct vector_cmp<0, vector<T, TSize, Axes>, vector<U, USize, Axes>> {
    using left_side     = vector<T, TSize, Axes>;
    using right_side    = vector<U, USize, Axes>;
    using traits_type   = typename left_side::value_traits;

    constexpr static bool
    eq(left_side const& lhs, right_side const& rhs)
    {
        return traits_type::eq(lhs.template at<0>(), rhs.template at<0>());
    }

    constexpr static bool
    less(left_side const& lhs, right_side const& rhs)
    {
        return traits_type::less(lhs.template at<0>(), rhs.template at<0>());
    }

    constexpr static int
    cmp(left_side const& lhs, right_side const& rhs)
    {
        return traits_type::cmp(lhs.template at<0>(), rhs.template at<0>());
    }
};

template < ::std::size_t N, typename T >
struct dot_product;

template < ::std::size_t N, typename T, ::std::size_t Size, typename Axes >
struct dot_product< N, vector<T, Size, Axes> > {
    using vector_type   = vector<T, Size, Axes>;
    using value_type    = typename vector_type::magnitude_type;

    value_type
    operator()( vector_type const& lhs, vector_type const& rhs )
    {
        return dot_product< N -1, vector<T, Size, Axes> >()(lhs, rhs) +
                lhs.template at<N>() * rhs.template at<N>();
    }
};

template < typename T, ::std::size_t Size, typename Axes >
struct dot_product< 0, vector<T, Size, Axes> > {
    using vector_type   = vector<T, Size, Axes>;
    using value_type    = typename vector_type::magnitude_type;

    value_type
    operator()(vector_type const& lhs, vector_type const& rhs)
    {
        return lhs.template at< 0 >() * rhs.template at< 0 >();
    }
};

template < ::std::size_t N, typename T >
struct vector_scalar_multiplication;

template < ::std::size_t N, typename T, ::std::size_t Size, typename Axes >
struct vector_scalar_multiplication< N, vector< T, Size, Axes > > {
    using vector_type   = vector<T, Size, Axes>;

    template < typename U >
    void
    operator()( vector_type& v, U s )
    {
        vector_scalar_multiplication< N - 1, vector<T, Size, Axes> >{}(v, s);
        v.template at<N>() *= s;
    }
};

template < typename T, ::std::size_t Size, typename Axes >
struct vector_scalar_multiplication< 0, vector< T, Size, Axes > > {
    using vector_type   = vector<T, Size, Axes>;

    template < typename U >
    void
    operator()( vector_type& v, U s )
    {
        v.template at<0>() *= s;
    }
};

template < ::std::size_t N, typename T >
struct vector_scalar_division;

template < ::std::size_t N, typename T, ::std::size_t Size, typename Axes >
struct vector_scalar_division< N, vector< T, Size, Axes > > {
    using vector_type   = vector<T, Size, Axes>;

    template < typename U >
    void
    operator()( vector_type& v, U s )
    {
        vector_scalar_division< N - 1, vector<T, Size, Axes> >{}(v, s);
        v.template at<N>() /= s;
    }
};

template < typename T, ::std::size_t Size, typename Axes >
struct vector_scalar_division< 0, vector< T, Size, Axes > > {
    using vector_type   = vector<T, Size, Axes>;

    template < typename U >
    void
    operator()( vector_type& v, U s )
    {
        v.template at<0>() /= s;
    }
};

template < ::std::size_t N, typename T >
struct vector_addition;

template < ::std::size_t N, typename T, ::std::size_t Size, typename Axes >
struct vector_addition< N, vector< T, Size, Axes > > {
    template < typename U >
    void
    operator()( vector<T, Size, Axes>& lhs, vector<U, Size, Axes> const& rhs)
    {
        vector_addition<N - 1, vector<T, Size, Axes> >{}(lhs, rhs);
        lhs.template at<N>() += rhs.template at<N>();
    }
};

template < typename T, ::std::size_t Size, typename Axes >
struct vector_addition< 0, vector< T, Size, Axes > > {
    template < typename U >
    void
    operator()( vector<T, Size, Axes>& lhs, vector<U, Size, Axes> const& rhs)
    {
        lhs.template at<0>() += rhs.template at<0>();
    }
};

template < ::std::size_t N, typename T >
struct vector_substraction;

template < ::std::size_t N, typename T, ::std::size_t Size, typename Axes >
struct vector_substraction< N, vector< T, Size, Axes > > {
    template < typename U >
    void
    operator()( vector<T, Size, Axes>& lhs, vector<U, Size, Axes> const& rhs)
    {
        vector_substraction<N - 1, vector<T, Size, Axes> >{}(lhs, rhs);
        lhs.template at<N>() -= rhs.template at<N>();
    }
};

template < typename T, ::std::size_t Size, typename Axes >
struct vector_substraction< 0, vector< T, Size, Axes > > {
    template < typename U >
    void
    operator()( vector<T, Size, Axes>& lhs, vector<U, Size, Axes> const& rhs)
    {
        lhs.template at<0>() -= rhs.template at<0>();
    }
};

template < ::std::size_t N, typename T >
struct set_all_elements;

template < ::std::size_t N, typename T, ::std::size_t Size, typename Axes >
struct set_all_elements< N, vector< T, Size, Axes > > {
    template < typename U >
    void
    operator()( vector<T, Size, Axes>& v, U s )
    {
        set_all_elements< N - 1, vector< T, Size, Axes > >{}(v, s);
        v.template at<N>() = s;
    }
};

template < typename T, ::std::size_t Size, typename Axes >
struct set_all_elements< 0, vector< T, Size, Axes > > {
    template < typename U >
    void
    operator()( vector<T, Size, Axes>& v, U s )
    {
        v.template at<0>() = s;
    }
};

} // namespace detail

}  // namespace math
}  /* namespace psst */

#endif /* PUSHKIN_VECTOR_DETAIL_HPP_ */
