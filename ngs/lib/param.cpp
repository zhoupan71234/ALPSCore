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

#include <alps/ngs/param.hpp>

namespace alps {

    std::ostream & operator<<(std::ostream & os, param const & value) {
        return os << value.str();
    }

	#define ALPS_NGS_PARAM_ADD_OPERATOR(T)								\
		T operator+(param const & p, T const & s) {						\
			return static_cast<T>(p) + s;								\
		}																\
		T operator+(T const & s, param const & p) {						\
			return s + static_cast<T>(p);								\
		}
	ALPS_NGS_PARAM_ADD_OPERATOR(char)
    ALPS_NGS_PARAM_ADD_OPERATOR(signed char)
    ALPS_NGS_PARAM_ADD_OPERATOR(unsigned char)
    ALPS_NGS_PARAM_ADD_OPERATOR(short)
    ALPS_NGS_PARAM_ADD_OPERATOR(unsigned short)
    ALPS_NGS_PARAM_ADD_OPERATOR(int)
    ALPS_NGS_PARAM_ADD_OPERATOR(unsigned)
    ALPS_NGS_PARAM_ADD_OPERATOR(long)
    ALPS_NGS_PARAM_ADD_OPERATOR(unsigned long)
    ALPS_NGS_PARAM_ADD_OPERATOR(long long)
    ALPS_NGS_PARAM_ADD_OPERATOR(unsigned long long)
    ALPS_NGS_PARAM_ADD_OPERATOR(float)
    ALPS_NGS_PARAM_ADD_OPERATOR(double)
    ALPS_NGS_PARAM_ADD_OPERATOR(long double)
    ALPS_NGS_PARAM_ADD_OPERATOR(bool)
    #undef ALPS_NGS_PARAM_ADD_OPERATOR

    std::string operator+(param const & p, char const * s) {
        return p.str() + s;
    }

    std::string operator+(char const * s, param const & p) {
        return s + p.str();
    }

    std::string operator+(param const & p, std::string const & s) {
        return p.str() + s;
    }

    std::string operator+(std::string const & s, param const & p) {
        return s + p.str();
    }

}
