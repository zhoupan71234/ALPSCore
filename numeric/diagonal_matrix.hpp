/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                 *
 * ALPS Project: Algorithms and Libraries for Physics Simulations                  *
 *                                                                                 *
 * ALPS Libraries                                                                  *
 *                                                                                 *
 * Copyright (C) 2010 - 2012 by Andreas Hehn <hehn@phys.ethz.ch>                   *
 *                                                                                 *
 * This software is part of the ALPS libraries, published under the ALPS           *
 * Library License; you can use, redistribute it and/or modify it under            *
 * the terms of the license, either version 1 or (at your option) any later        *
 * version.                                                                        *
 *                                                                                 *
 * You should have received a copy of the ALPS Library License along with          *
 * the ALPS Libraries; see the file LICENSE.txt. If not, the license is also       *
 * available from http://alps.comp-phys.org/.                                      *
 *                                                                                 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT       *
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE       *
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,     *
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER     *
 * DEALINGS IN THE SOFTWARE.                                                       *
 *                                                                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef ALPS_DIAGONAL_MATRIX_HPP
#define ALPS_DIAGONAL_MATRIX_HPP

#include <vector>
#include <algorithm>
#include <functional>
#include <cassert>
#include <ostream>
#include <boost/lambda/lambda.hpp>
#include <cmath>
#include <alps/numeric/matrix/matrix_interface.hpp>

namespace alps {
    namespace numeric {

    namespace detail {
//        struct functor_##name { template<class T> return_type operator() (arg_type t) { return (static_cast<return_type (*) (arg_type)>(name))(t); } };

        #define DEFINE_FUNCTION_OBJECT(name, return_type, arg_type) \
        struct functor_##name { template<class T> return_type operator() (arg_type t) {using std::name; return name(t); } };

            DEFINE_FUNCTION_OBJECT(sqrt, T, T const &)
            DEFINE_FUNCTION_OBJECT(exp, T, T const &)

        #undef DEFINE_FUNCTION_OBJECT

    }

    template<typename T>
    class diagonal_matrix
    {
    public:
        typedef T                       value_type;
        typedef T&                      reference;
        typedef T const&                const_reference;
        typedef std::size_t             size_type;
        typedef std::ptrdiff_t          difference_type;

        typedef typename std::vector<T>::iterator       diagonal_iterator;
        typedef typename std::vector<T>::const_iterator const_diagonal_iterator;

        diagonal_matrix()
        {
        }

        template<class Vector>
        explicit diagonal_matrix(Vector const & init)
        : data_(init.begin(), init.end()) { }

        template<class Tp>
        diagonal_matrix(diagonal_matrix<Tp> const & rhs)
        : data_(rhs.diagonal().first, rhs.diagonal().second) { }

        explicit diagonal_matrix(std::size_t size, T const & init = T())
        : data_(size, init) { }

        const std::vector<T> & get_values() const { return data_; }
        std::vector<T> & get_values() { return data_; }

        size_type num_rows() const { return data_.size(); }
        size_type num_cols() const { return data_.size(); }

        T const & operator[](size_type i) const { return data_[i]; }
        T & operator[](size_type i) { return data_[i]; }

        T const & operator()(size_type i, size_type j) const
        {
            assert(i == j);
            return data_[i];
        }
        T & operator()(std::size_t i, std::size_t j)
        {
            assert(i == j);
            return data_[i];
        }

        diagonal_matrix<T>& operator += (diagonal_matrix const& rhs)
        {
            assert(rhs.data_.size() == data_.size());
            using std::transform;
            transform(data_.begin(),data_.end(),rhs.data_.begin(),data_.begin(), std::plus<T>());
            return *this;
        }

        diagonal_matrix<T>& operator -= (diagonal_matrix const& rhs)
        {
            assert(rhs.data_.size() == data_.size());
            using std::transform;
            transform(data_.begin(),data_.end(),rhs.data_.begin(),data_.begin(), std::minus<T>());
            return *this;
        }

        template <typename T2>
        diagonal_matrix<T>& operator *= (T2 const& t)
        {
            using std::for_each;
            for_each(data_.begin(), data_.end(), boost::lambda::_1 *= t);
            return *this;
        }

        template <typename T2>
        diagonal_matrix<T>& operator /= (T2 const& t)
        {
            using std::for_each;
            for_each(data_.begin(), data_.end(), boost::lambda::_1 *= t);
            return *this;
        }

        std::pair<diagonal_iterator, diagonal_iterator> diagonal()
        {
            return std::make_pair(data_.begin(), data_.end());
        }

        std::pair<const_diagonal_iterator, const_diagonal_iterator> diagonal() const
        {
            return std::make_pair(data_.begin(), data_.end());
        }

        void remove_rows(size_type k, size_type n = 1)
        {
            data_.erase(data_.begin(), data_.begin()+n);
        }

        void remove_cols(size_type k, size_type n = 1)
        {
            remove_rows(k, n);
        }

        void resize(size_type r, size_type c, T v = T())
        {
            assert(r == c);
            data_.resize(r, v);
        }

    private:
        std::vector<T> data_;
    };

    template<typename T, class Matrix>
    void gemm(Matrix const & m1, diagonal_matrix<T> const & m2, Matrix & m3)
    {
        assert(num_cols(m1) == num_rows(m2));
        resize(m3, num_rows(m1), num_cols(m2));
        // We optimize for a Matrix which is column-major (like alps::numeric::matrix)
        for (std::size_t j = 0; j < num_cols(m2); ++j)
            for (std::size_t i = 0; i < num_rows(m1); ++i)
                m3(i,j) = m1(i,j) * m2(j,j);
    }

    template<typename T, class Matrix>
    void gemm(diagonal_matrix<T> const & m1, Matrix const & m2, Matrix & m3)
    {
        assert(num_cols(m1) == num_rows(m2));
        resize(m3, num_rows(m1), num_cols(m2));
        for (std::size_t j = 0; j < num_cols(m2); ++j)
            for (std::size_t i = 0; i < num_rows(m1); ++i)
                m3(i,j) = m1(i,i) * m2(i,j);
    }

    template <typename T>
    const diagonal_matrix<T> operator + (diagonal_matrix<T> a, diagonal_matrix<T> const& b)
    {
        a += b;
        return a;
    }

    template <typename T>
    const diagonal_matrix<T> operator - (diagonal_matrix<T> a, diagonal_matrix<T> const& b)
    {
        a -= b;
        return a;
    }

    template <typename T>
    const diagonal_matrix<T> operator - (diagonal_matrix<T> a)
    {
        using std::transform;
        std::pair<
              typename diagonal_matrix<T>::diagonal_iterator
            , typename diagonal_matrix<T>::diagonal_iterator
        > range(a.diagonal());
        transform(range.first, range.second, range.first, std::negate<T>());
        return a;
    }

    template<typename T, typename T2>
    const diagonal_matrix<T> operator * (diagonal_matrix<T> m, T2 const& t)
    {
        return m*=t;
    }

    template<typename T, typename T2>
    const diagonal_matrix<T> operator * (T2 const& t, diagonal_matrix<T> m)
    {
        return m*=t;
    }

    template<typename T>
    typename diagonal_matrix<T>::size_type num_rows(diagonal_matrix<T> const & m)
    {
        return m.num_rows();
    }

    template<typename T>
    typename diagonal_matrix<T>::size_type num_cols(diagonal_matrix<T> const & m)
    {
        return m.num_cols();
    }

    template<typename T>
    void sqrt_inplace(diagonal_matrix<T> & m)
    {
        using std::transform;
        transform(m.diagonal().first, m.diagonal().second, m.diagonal().first, detail::functor_sqrt());
    }

    template<typename T>
    diagonal_matrix<T> sqrt(diagonal_matrix<T> m)
    {
        sqrt_inplace(m);
        return m;
    }

    template<typename T>
    void exp_inplace(diagonal_matrix<T> & m)
    {
        using std::transform;
        transform(m.diagonal().first, m.diagonal().second, m.diagonal().first, detail::functor_exp());
    }

    template<typename T>
    diagonal_matrix<T> exp(diagonal_matrix<T> m)
    {
        exp_inplace(m);
        return m;
    }

    template<typename T>
    std::ostream& operator<<(std::ostream& os, diagonal_matrix<T> const & m)
    {
        std::copy(m.diagonal().first, m.diagonal().second, std::ostream_iterator<T>(os, " "));
        return os;
    }

    template<typename T>
    void remove_rows(diagonal_matrix<T> & m, std::size_t k, std::size_t n = 1)
    {
        m.remove_rows(k, n);
    }

    template<typename T>
    void remove_cols(diagonal_matrix<T> & m, std::size_t k, std::size_t n = 1)
    {
        m.remove_cols(k, n);
    }

    template<typename T>
    void resize(diagonal_matrix<T> & m, std::size_t r, std::size_t c, T v = T())
    {
        m.resize(r, c, v);
    }

    ALPS_IMPLEMENT_MATRIX_DIAGONAL_ITERATOR_INTERFACE(diagonal_matrix<T>,<typename T>)

    } // namespace numeric
} // namespace alps

#endif //ALPS_DIAGONAL_MATRIX_HPP