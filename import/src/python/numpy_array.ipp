/*****************************************************************************
*
* ALPS Project: Algorithms and Libraries for Physics Simulations
*
* ALPS Libraries
*
* Copyright (C) 1994-2010 by Ping Nang Ma <pingnang@itp.phys.ethz.ch>,
*                            Lukas Gamper <gamperl@gmail.com>,
*                            Matthias Troyer <troyer@itp.phys.ethz.ch>
*
* This software is part of the ALPS libraries, published under the ALPS
* Library License; you can use, redistribute it and/or modify it under
* the terms of the license, either version 1 or (at your option) any later
* version.
*
* You should have received a copy of the ALPS Library License along with
* the ALPS Libraries; see the file LICENSE.txt. If not, the license is also
* available from http://alps.comp-phys.org/.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
* SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
* FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*
*****************************************************************************/

/* $Id: pyalea.cpp 3520 2010-04-09 16:49:53Z tamama $ */

#include <alps/python/numpy_array.hpp>

namespace alps { 
    namespace python {
        namespace numpy {

            void import() {
                static bool inited = false;
                if (!inited) {
                    import_array();  
                    boost::python::numeric::array::set_module_and_type("numpy", "ndarray");
                    inited = true;
                }
            }

            /*
             template <class T>
             static boost::python::str print_vector_list(std::vector<T> self)
             {
             boost::python::str s;
             for (typename std::vector<T>::iterator it = self.begin(); it != self.end(); ++it)
             {
             s += boost::python::str(*it);
             s += boost::python::str("\n");
             }
             return s;
             }
             
             */
          
            void convert(boost::python::object const & source, std::vector<double> & target) {
                import();
                target.resize(PyArray_Size(source.ptr()));
                memcpy(&target.front(), static_cast<double *>(PyArray_DATA(source.ptr())), PyArray_ITEMSIZE((PyArrayObject *)source.ptr()) * target.size());
            }

            boost::python::numeric::array convert(double source) {
                return convert(std::vector<double>(1, source));
            }

            std::vector<double> convert(boost::python::object const & source) {
               std::vector<double> target;
               convert(source, target);
               return target;
            }

            boost::python::numeric::array convert(std::vector<double> const & source) {
                import();
                npy_intp size = source.size();
                boost::python::object obj(boost::python::handle<>(PyArray_SimpleNew(1, &size, PyArray_DOUBLE)));
                void * ptr = PyArray_DATA((PyArrayObject*) obj.ptr());
                memcpy(ptr, &source.front(), PyArray_ITEMSIZE((PyArrayObject*) obj.ptr()) * size);
                return boost::python::extract<boost::python::numeric::array>(obj);
            }

            boost::python::numeric::array convert(std::vector<std::vector<double> > const & source) {
                import();
                npy_intp size[2] = {static_cast<npy_intp>(source.size()), static_cast<npy_intp>(source[0].size()) };
                boost::python::object obj(boost::python::handle<>(PyArray_SimpleNew(2, size, PyArray_DOUBLE)));
                void * ptr = PyArray_DATA((PyArrayObject*) obj.ptr());
                for (std::size_t i = 0; i < source.size(); ++i)
                    memcpy(static_cast<double *>(ptr) + i * size[1], &source[i].front(), PyArray_ITEMSIZE((PyArrayObject*) obj.ptr()) * size[1]);
                return boost::python::extract<boost::python::numeric::array>(obj);
            }

            boost::python::numeric::array convert(std::vector<std::vector<std::vector<double> > > const & source) {
                import();
                npy_intp size[3] = {
                      static_cast<npy_intp>(source.size())
                    , static_cast<npy_intp>(source[0].size())
                    , static_cast<npy_intp>(source[0][0].size())
                };
                boost::python::object obj(boost::python::handle<>(PyArray_SimpleNew(3, size, PyArray_DOUBLE)));
                void * ptr = PyArray_DATA((PyArrayObject*) obj.ptr());
                for (std::size_t i = 0; i < source.size(); ++i)
                    for (std::size_t j = 0; j < source[i].size(); ++j)
                        memcpy(static_cast<double *>(ptr) + i * size[1] * size[2] + j * size[2], &source[i][j].front(), PyArray_ITEMSIZE((PyArrayObject*) obj.ptr()) * size[2]);
                return boost::python::extract<boost::python::numeric::array>(obj);
            }
        }
    }
}
