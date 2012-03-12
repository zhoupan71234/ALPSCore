/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                 *
 * ALPS Project: Algorithms and Libraries for Physics Simulations                  *
 *                                                                                 *
 * ALPS Libraries                                                                  *
 *                                                                                 *
 * Copyright (C) 2010 - 2011 by Lukas Gamper <gamperl@gmail.com>                   *
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

#ifndef ALPS_NGS_HDF5_STD_PAIR
#define ALPS_NGS_HDF5_STD_PAIR

#include <alps/ngs/hdf5.hpp>
#include <alps/ngs/convert.hpp>

#include <boost/type_traits/remove_cv.hpp>
#include <boost/type_traits/remove_reference.hpp>

#include <utility>

namespace alps {
    namespace hdf5 {

        #define ALPS_NGS_HDF5_PAIR_SAVE(ARCHIVE)                                                                                                                \
            template <typename T, typename U> void save(                                                                                                        \
                  ARCHIVE & ar                                                                                                                                  \
                , std::string const & path                                                                                                                      \
                , std::pair<T, U> const & value                                                                                                                 \
                , std::vector<std::size_t> size = std::vector<std::size_t>()                                                                                    \
                , std::vector<std::size_t> chunk = std::vector<std::size_t>()                                                                                   \
                , std::vector<std::size_t> offset = std::vector<std::size_t>()                                                                                  \
            ) {                                                                                                                                                 \
                save(ar, ar.complete_path(path) + "/first", value.first);                                                                                       \
                save(ar, ar.complete_path(path) + "/second", value.second);                                                                                     \
            }
        ALPS_NGS_HDF5_PAIR_SAVE(archive)
        #ifdef ALPS_HDF5_HAVE_DEPRECATED
            ALPS_NGS_HDF5_PAIR_SAVE(oarchive)
        #endif
        #undef ALPS_NGS_HDF5_PAIR_SAVE

        #define ALPS_NGS_HDF5_PAIR_LOAD(ARCHIVE)                                                                                                                \
            template <typename T, typename U> void load(                                                                                                        \
                  ARCHIVE & ar                                                                                                                                  \
                , std::string const & path                                                                                                                      \
                , std::pair<T, U> & value                                                                                                                       \
                , std::vector<std::size_t> chunk = std::vector<std::size_t>()                                                                                   \
                , std::vector<std::size_t> offset = std::vector<std::size_t>()                                                                                  \
            ) {                                                                                                                                                 \
                load(ar, ar.complete_path(path) + "/first", value.first);                                                                                       \
                load(ar, ar.complete_path(path) + "/second", value.second);                                                                                     \
            }
        ALPS_NGS_HDF5_PAIR_LOAD(archive)
        #ifdef ALPS_HDF5_HAVE_DEPRECATED
            ALPS_NGS_HDF5_PAIR_LOAD(iarchive)
        #endif
        #undef ALPS_NGS_HDF5_PAIR_LOAD

        template<typename T> struct scalar_type<std::pair<T *, std::vector<std::size_t> > > {
            typedef typename scalar_type<typename boost::remove_reference<typename boost::remove_cv<T>::type>::type>::type type;
        };

        template<typename T> struct has_complex_elements<std::pair<T *, std::vector<std::size_t> > > 
            : public has_complex_elements<typename detail::remove_cvr<T>::type>
        {};

        namespace detail {

            template<typename T> struct get_extent<std::pair<T *, std::vector<std::size_t> > > {
                static std::vector<std::size_t> apply(std::pair<T *, std::vector<std::size_t> > const & value) {
                    using alps::hdf5::get_extent;
                    std::vector<std::size_t> extent(value.second);
                    std::vector<std::size_t> size(value.second.size() ? get_extent(*value.first) : std::vector<std::size_t>());
                    if (!is_continous<T>::value && value.second.size()) {
                        for (std::size_t i = 1; i < std::accumulate(value.second.begin(), value.second.end(), std::size_t(1), std::multiplies<std::size_t>()); ++i)
                            if (!std::equal(size.begin(), size.end(), get_extent(value.first[i]).begin()))
                                throw std::runtime_error("no rectengual matrix" + ALPS_STACKTRACE);
                    }
                    std::copy(size.begin(), size.end(), std::back_inserter(extent));
                    return extent;
                }
            };

            template<typename T> struct set_extent<std::pair<T *, std::vector<std::size_t> > > {
                static void apply(std::pair<T *, std::vector<std::size_t> > & value, std::vector<std::size_t> const & size) {
                    using alps::hdf5::set_extent;
                    if (value.second.size() > size.size() || !std::equal(value.second.begin(), value.second.end(), size.begin()))
                        throw std::runtime_error("invalid data size" + ALPS_STACKTRACE);
                    if (!is_continous<T>::value && value.second.size() && value.second.size() < size.size())
                        for (std::size_t i = 0; i < std::accumulate(value.second.begin(), value.second.end(), std::size_t(1), std::multiplies<std::size_t>()); ++i)
                             set_extent(value.first[i], std::vector<std::size_t>(size.begin() + value.second.size(), size.end()));
                }
            };

            template<typename T> struct is_vectorizable<std::pair<T *, std::vector<std::size_t> > > {
                static bool apply(std::pair<T *, std::vector<std::size_t> > const & value) {
                    using alps::hdf5::get_extent;
                    using alps::hdf5::is_vectorizable;
                    std::vector<std::size_t> size(get_extent(*value.first));
                    for (std::size_t i = 1; i < std::accumulate(value.second.begin(), value.second.end(), std::size_t(1), std::multiplies<std::size_t>()); ++i)
                        if (!is_vectorizable(value.first[i]) || !std::equal(size.begin(), size.end(), get_extent(value.first[i]).begin()))
                            return false;
                    return true;
                }
            };

            template<typename T> struct get_pointer<std::pair<T *, std::vector<std::size_t> > > {
                static typename alps::hdf5::scalar_type<std::pair<T *, std::vector<std::size_t> > >::type * apply(std::pair<T *, std::vector<std::size_t> > & value) {
                    using alps::hdf5::get_pointer;
                    return get_pointer(*value.first);
                }
            };

            template<typename T> struct get_pointer<std::pair<T *, std::vector<std::size_t> > const> {
                static typename alps::hdf5::scalar_type<std::pair<T *, std::vector<std::size_t> > >::type const * apply(std::pair<T *, std::vector<std::size_t> > const & value) {
                    using alps::hdf5::get_pointer;
                    return get_pointer(*value.first);
                }
            };

        }

        #define ALPS_NGS_HDF5_PAIR_VECTOR_SAVE(ARCHIVE)                                                                                                         \
            template<typename T> void save(                                                                                                                     \
                  ARCHIVE & ar                                                                                                                                  \
                , std::string const & path                                                                                                                      \
                , std::pair<T *, std::vector<std::size_t> > const & value                                                                                       \
                , std::vector<std::size_t> size = std::vector<std::size_t>()                                                                                    \
                , std::vector<std::size_t> chunk = std::vector<std::size_t>()                                                                                   \
                , std::vector<std::size_t> offset = std::vector<std::size_t>()                                                                                  \
            ) {                                                                                                                                                 \
                if (is_continous<T>::value) {                                                                                                                   \
                    std::vector<std::size_t> extent(get_extent(value));                                                                                         \
                    std::copy(extent.begin(), extent.end(), std::back_inserter(size));                                                                          \
                    std::copy(extent.begin(), extent.end(), std::back_inserter(chunk));                                                                         \
                    std::fill_n(std::back_inserter(offset), extent.size(), 0);                                                                                  \
                    ar.write(path, get_pointer(value), size, chunk, offset);                                                                                    \
                } else if (value.second.size() == 0)                                                                                                            \
                    ar.write(path, static_cast<int const *>(NULL), std::vector<std::size_t>());                                                                 \
                else if (is_vectorizable(value)) {                                                                                                              \
                    std::copy(value.second.begin(), value.second.end(), std::back_inserter(size));                                                              \
                    std::fill_n(std::back_inserter(chunk), value.second.size(), 1);                                                                             \
                    for (                                                                                                                                       \
                        std::size_t i = 0;                                                                                                                      \
                        i < std::accumulate(value.second.begin(), value.second.end(), std::size_t(1), std::multiplies<std::size_t>());                          \
                        ++i                                                                                                                                     \
                    ) {                                                                                                                                         \
                        std::vector<std::size_t> local_offset(offset);                                                                                          \
                        local_offset.push_back(                                                                                                                 \
                            i / std::accumulate(value.second.begin() + 1, value.second.end(), std::size_t(1), std::multiplies<std::size_t>())                   \
                        );                                                                                                                                      \
                        for (std::vector<std::size_t>::const_iterator it = value.second.begin() + 1; it != value.second.end(); ++it)                            \
                            local_offset.push_back((i % std::accumulate(                                                                                        \
                                it, value.second.end(), std::size_t(1), std::multiplies<std::size_t>()                                                          \
                            )) / std::accumulate(                                                                                                               \
                                it + 1, value.second.end(), std::size_t(1), std::multiplies<std::size_t>()                                                      \
                            ));                                                                                                                                 \
                        save(ar, path, value.first[i], size, chunk, local_offset);                                                                              \
                    }                                                                                                                                           \
                } else {                                                                                                                                        \
                    if (path.find_last_of('@') != std::string::npos)                                                                                            \
                        throw std::runtime_error("attributes needs to be vectorizable: " + path + ALPS_STACKTRACE);                                                   \
                    if (ar.is_data(path))                                                                                                                       \
                        ar.delete_data(path);                                                                                                                   \
                    offset = std::vector<std::size_t>(value.second.size(), 0);                                                                                  \
                    do {                                                                                                                                        \
                        std::size_t last = offset.size() - 1, pos = 0;                                                                                          \
                        std::string location = "";                                                                                                              \
                        for (std::vector<std::size_t>::const_iterator it = offset.begin(); it != offset.end(); ++it) {                                          \
                            location += "/" + convert<std::string>(*it);                                                                                        \
                            pos += *it * std::accumulate(                                                                                                       \
                                value.second.begin() + (it - offset.begin()) + 1,                                                                               \
                                value.second.end(),                                                                                                             \
                                std::size_t(1),                                                                                                                 \
                                std::multiplies<std::size_t>()                                                                                                  \
                            );                                                                                                                                  \
                        }                                                                                                                                       \
                        save(ar, path + location, value.first[pos]);                                                                                            \
                        if (offset[last] + 1 == value.second[last] && last) {                                                                                   \
                            for (pos = last; ++offset[pos] == value.second[pos] && pos; --pos);                                                                 \
                            for (++pos; pos <= last; ++pos)                                                                                                     \
                                offset[pos] = 0;                                                                                                                \
                        } else                                                                                                                                  \
                            ++offset[last];                                                                                                                     \
                    } while (offset[0] < value.second[0]);                                                                                                      \
                }                                                                                                                                               \
            }
        ALPS_NGS_HDF5_PAIR_VECTOR_SAVE(archive)
        #ifdef ALPS_HDF5_HAVE_DEPRECATED
            ALPS_NGS_HDF5_PAIR_VECTOR_SAVE(oarchive)
        #endif
        #undef ALPS_NGS_HDF5_PAIR_VECTOR_SAVE

        #define ALPS_NGS_HDF5_PAIR_VECTOR_LOAD(ARCHIVE)                                                                                                         \
            template<typename T> void load(                                                                                                                     \
                  ARCHIVE & ar                                                                                                                                  \
                , std::string const & path                                                                                                                      \
                , std::pair<T *, std::vector<std::size_t> > & value                                                                                             \
                , std::vector<std::size_t> chunk = std::vector<std::size_t>()                                                                                   \
                , std::vector<std::size_t> offset = std::vector<std::size_t>()                                                                                  \
            ) {                                                                                                                                                 \
                if (ar.is_group(path)) {                                                                                                                        \
                    offset = std::vector<std::size_t>(value.second.size(), 0);                                                                                  \
                    do {                                                                                                                                        \
                        std::size_t last = offset.size() - 1, pos = 0;                                                                                          \
                        std::string location = "";                                                                                                              \
                        for (std::vector<std::size_t>::const_iterator it = offset.begin(); it != offset.end(); ++it) {                                          \
                            location += "/" + convert<std::string>(*it);                                                                                        \
                            pos += *it * std::accumulate(                                                                                                       \
                                value.second.begin() + (it - offset.begin()) + 1,                                                                               \
                                value.second.end(),                                                                                                             \
                                std::size_t(1),                                                                                                                 \
                                std::multiplies<std::size_t>()                                                                                                  \
                            );                                                                                                                                  \
                        }                                                                                                                                       \
                        load(ar, path + location, value.first[pos]);                                                                                            \
                        if (offset[last] + 1 == value.second[last] && last) {                                                                                   \
                            for (pos = last; ++offset[pos] == value.second[pos] && pos; --pos);                                                                 \
                            for (++pos; pos <= last; ++pos)                                                                                                     \
                                offset[pos] = 0;                                                                                                                \
                        } else                                                                                                                                  \
                            ++offset[last];                                                                                                                     \
                    } while (offset[0] < value.second[0]);                                                                                                      \
                } else {                                                                                                                                        \
                    std::vector<std::size_t> size(ar.extent(path));                                                                                             \
                    set_extent(value, std::vector<std::size_t>(size.begin() + chunk.size(), size.end()));                                                       \
                    if (is_continous<T>::value) {                                                                                                               \
                        std::copy(size.begin(), size.end(), std::back_inserter(chunk));                                                                         \
                        std::fill_n(std::back_inserter(offset), size.size(), 0);                                                                                \
                        ar.read(path, get_pointer(value), chunk, offset);                                                                                       \
                    } else if (value.second.size()) {                                                                                                           \
                        std::fill_n(std::back_inserter(chunk), value.second.size(), 1);                                                                         \
                        for (                                                                                                                                   \
                            std::size_t i = 0;                                                                                                                  \
                            i < std::accumulate(value.second.begin(), value.second.end(), std::size_t(1), std::multiplies<std::size_t>());                      \
                            ++i                                                                                                                                 \
                        ) {                                                                                                                                     \
                            std::vector<std::size_t> local_offset(offset);                                                                                      \
                            local_offset.push_back(                                                                                                             \
                                i / std::accumulate(value.second.begin() + 1, value.second.end(), std::size_t(1), std::multiplies<std::size_t>())               \
                            );                                                                                                                                  \
                            for (std::vector<std::size_t>::iterator it = value.second.begin() + 1; it != value.second.end(); ++it)                              \
                                local_offset.push_back((i % std::accumulate(                                                                                    \
                                    it, value.second.end(), std::size_t(1), std::multiplies<std::size_t>()                                                      \
                                )) / std::accumulate(                                                                                                           \
                                    it + 1, value.second.end(), std::size_t(1), std::multiplies<std::size_t>()                                                  \
                                ));                                                                                                                             \
                            load(ar, path, value.first[i], chunk, local_offset);                                                                                \
                        }                                                                                                                                       \
                    }                                                                                                                                           \
                }                                                                                                                                               \
            }
        ALPS_NGS_HDF5_PAIR_VECTOR_LOAD(archive)
        #ifdef ALPS_HDF5_HAVE_DEPRECATED
            ALPS_NGS_HDF5_PAIR_VECTOR_LOAD(iarchive)
        #endif
        #undef ALPS_NGS_HDF5_PAIR_VECTOR_SAVE

    }
}

#endif

