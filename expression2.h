/*****************************************************************************
*
* ALPS Project: Algorithms and Libraries for Physics Simulations
*
* ALPS Libraries
*
* Copyright (C) 2001-2004 by Matthias Troyer <troyer@comp-phys.org>,
*                            Synge Todo <wistaria@comp-phys.org>
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

/* $Id$ */

#ifndef ALPS_EXPRESSION2_H
#define ALPS_EXPRESSION2_H

#include <alps/config.h>

#ifndef ALPS_WITH_NEW_EXPRESSION

#else

#include <alps/cctype.h>
#include <alps/parameters.h>
#include <alps/parser/parser.h>
#include <alps/typetraits.h>

#include <boost/smart_ptr.hpp>
#include <boost/throw_exception.hpp>

#include <complex>
#include <string>
#include <vector>

namespace alps {

template<class T = std::complex<double> > class Expression;
template<class T = std::complex<double> > class Term;
template<class T = std::complex<double> > class Factor;

template<class T = std::complex<double> > class Evaluator;
template<class T = std::complex<double> > class ParameterEvaluator;

template<class T>
class Evaluator {
public:
  typedef T value_type;
  enum Direction { left_to_right, right_to_left };
  Evaluator() {}
  virtual ~Evaluator() {}

  virtual bool can_evaluate(const std::string&) const;
  virtual bool can_evaluate_function(const std::string&, const Expression<T>&) const;
  virtual value_type evaluate(const std::string&) const;
  virtual value_type evaluate_function(const std::string&, const Expression<T>&) const;
  virtual Expression<T> partial_evaluate(const std::string& name) const;
  virtual Expression<T> partial_evaluate_function(const std::string& name, const Expression<T>&) const;
  virtual Direction direction() const;
};

template<class T>
class ParameterEvaluator : public Evaluator<T> {
public:
  typedef T value_type;
  ParameterEvaluator(const Parameters& p) : parms_(p) {}
  virtual ~ParameterEvaluator() {}

  bool can_evaluate(const std::string&) const;
  value_type evaluate(const std::string&) const;
  Expression<T> partial_evaluate(const std::string& name) const;
  const Parameters& parameters() const { return parms_;}

private:
  Parameters parms_;
};

namespace detail {

template<class T>
class Evaluatable {
public:
  typedef T value_type;

  Evaluatable() {}
  virtual ~Evaluatable() {}
  virtual value_type value(const Evaluator<T>&) const = 0;
  virtual bool can_evaluate(const Evaluator<T>&) const = 0;
  virtual void output(std::ostream&) const = 0;
  virtual Evaluatable* clone() const = 0;
  virtual boost::shared_ptr<Evaluatable> flatten_one() { return boost::shared_ptr<Evaluatable>(); }
  virtual Evaluatable* partial_evaluate_replace(const Evaluator<T>&) { return this; }
  virtual bool is_single_term() const { return false; }
  virtual Term<T> term() const;
  virtual bool depends_on(const std::string&) const { return false; }
};

} // end namespace detail

template<class T>
class Factor : public detail::Evaluatable<T> {
public:
  typedef T value_type;

  Factor(std::istream&, bool inverse = false);
  Factor(value_type x);
  Factor(const std::string& s);
  Factor(const Factor& v)
    : detail::Evaluatable<T>(v), term_(), is_inverse_(v.is_inverse_)
  {
    if (v.term_) term_.reset(v.term_->clone());
  }
  Factor(const detail::Evaluatable<T>& v)
    : detail::Evaluatable<T>(v), term_(v.clone()), is_inverse_(false) {}
  virtual ~Factor() {}

  const Factor& operator=(const Factor& v);

  value_type value(const Evaluator<T>& p) const
  {
    if (!term_)
      boost::throw_exception(std::runtime_error("Empty value in expression"));
    return is_inverse() ? 1./term_->value(p) : term_->value(p);
  }
  void output(std::ostream&) const;
  bool can_evaluate(const Evaluator<T>& p) const;
  detail::Evaluatable<T>* clone() const { return new Factor<T>(*this); }
  boost::shared_ptr<Factor> flatten_one_value();
  bool is_inverse() const { return is_inverse_; }
  void partial_evaluate(const Evaluator<T>& p);
  bool is_single_term() const { return term_ ? term_->is_single_term() : false; }
  Term<T> term() const { return term_ ? term_->term() : Term<T>(); }
  bool depends_on(const std::string& s) const
  {
    return term_ ? term_->depends_on(s) : false;
  }

private:
  boost::shared_ptr<detail::Evaluatable<T> > term_;
  bool is_inverse_;
};

template<class T>
class Term : public detail::Evaluatable<T> {
public:
  typedef T value_type;
  typedef typename std::vector<Factor<T> >::const_iterator factor_iterator;

  Term(std::istream& is, bool negate = false);
  Term() : is_negative_(false) {}
  Term(value_type x) : is_negative_(false), terms_(1,Factor<T>(x)) {}
  Term(const detail::Evaluatable<T>& e)
    : is_negative_(false), terms_(1,Factor<T>(e)) {}
  virtual ~Term() {}

  value_type value(const Evaluator<T>& p) const;

  bool can_evaluate(const Evaluator<T>& p) const;
  void output(std::ostream&) const;
  detail::Evaluatable<T>* clone() const { return new Term<T>(*this); }
  bool is_negative() const { return is_negative_;}
  boost::shared_ptr<Term> flatten_one_term();
  void partial_evaluate(const Evaluator<T>& p);

  const Term& operator*=(const Factor<T>& v)
  {
    terms_.push_back(v);
    return *this;
  }
  const Term& operator*=(const std::string& s)
  {
    return operator*=(Factor<T>(s));
  }
  void simplify();

  virtual std::pair<factor_iterator,factor_iterator> factors() const
  {
    return std::make_pair(terms_.begin(),terms_.end());
  }

  bool depends_on(const std::string&) const;

  int num_factors() const {return terms_.size(); }
private:
  void negate() { is_negative_ = !is_negative_;}
  bool is_negative_;
  std::vector<Factor<T> > terms_;
};

template<class T>
class Expression : public detail::Evaluatable<T> {
public:
  typedef T value_type;
  typedef Term<T> term_type;
  typedef typename std::vector<Term<T> >::const_iterator term_iterator;

  Expression() {}
  Expression(const std::string& str) { parse(str); }
  Expression(std::istream& in) { parse(in); }
  Expression(value_type val) : terms_(1,Term<T>(val)) {}
  Expression(const detail::Evaluatable<T>& e) : terms_(1,Term<T>(e)) {}
  Expression(const Term<T>& e) : terms_(1,e) {}
  virtual ~Expression() {}

  value_type value(const Evaluator<T>& p = Evaluator<T>()) const;
  value_type value(const Parameters& p) const {
    return value(ParameterEvaluator<T>(p));
  }

  bool can_evaluate(const Evaluator<T>& p = Evaluator<T>()) const;
  bool can_evaluate(const Parameters& p) const
  {
    return can_evaluate(ParameterEvaluator<T>(p));
  }
  void partial_evaluate(const Evaluator<T>& p=Evaluator<T>());
  void partial_evaluate(const Parameters& p) {
    partial_evaluate(ParameterEvaluator<T>(p));
  }

  void output(std::ostream& os) const;

  detail::Evaluatable<T>* clone() const { return new Expression<T>(*this); }
  std::pair<term_iterator,term_iterator> terms() const
  {
    return std::make_pair(terms_.begin(),terms_.end());
  }
  void flatten(); // multiply out all blocks
  boost::shared_ptr<Expression> flatten_one_expression();
  const Expression& operator+=(const Term<T>& term)
  {
    terms_.push_back(term);
    return *this;
  }
  const Expression& operator+=(const Expression& e)
  {
    std::copy(e.terms_.begin(),e.terms_.end(),std::back_inserter(terms_));
    partial_evaluate();
    return *this;
  }
  void simplify();

  bool is_single_term() const { return terms_.size() == 1; }
  Term<T> term() const;
  bool depends_on(const std::string&) const;

  void parse(const std::string& str);
  void parse(std::istream& is);

private:
  std::vector<Term<T> > terms_;
};

namespace detail {

template<class T>
class Block : public Expression<T>
{
public:
  Block(std::istream&);
  Block(const Expression<T>& e) : Expression<T>(e) {}
  void output(std::ostream&) const;
  Evaluatable<T>* clone() const { return new Block<T>(*this); }
  void flatten();
  boost::shared_ptr<Evaluatable<T> > flatten_one();
  Evaluatable<T>* partial_evaluate_replace(const Evaluator<T>& p);
};

template<class T>
class Symbol : public Evaluatable<T> {
public:
  typedef T value_type;

  Symbol(const std::string& n) : name_(n) {}
  value_type value(const Evaluator<T>& p) const;
  bool can_evaluate(const Evaluator<T>& ev) const
  {
    return ev.can_evaluate(name_);
  }
  void output(std::ostream& os) const { os << name_; }
  Evaluatable<T>* clone() const { return new Symbol<T>(*this); }
  Evaluatable<T>* partial_evaluate_replace(const Evaluator<T>& p);
  bool depends_on(const std::string& s) const;
private:
  std::string name_;
};

template<class T>
class Function : public Evaluatable<T> {
public:
  typedef T value_type;

  Function(std::istream&, const std::string&);
  Function(const std::string& n, const Expression<T>& e) : name_(n), arg_(e) {}
  value_type value(const Evaluator<T>& p) const;
  bool can_evaluate(const Evaluator<T>& p) const;
  void output(std::ostream&) const;
  Evaluatable<T>* clone() const { return new Function<T>(*this); }
  boost::shared_ptr<Evaluatable<T> > flatten_one();
  Evaluatable<T>* partial_evaluate_replace(const Evaluator<T>& p);
  bool depends_on(const std::string& s) const;
private:
 std::string name_;
 Expression<T> arg_;
};

template<class T>
class Number : public Evaluatable<T> {
public:
  typedef T value_type;
  typedef typename alps::TypeTraits<T>::real_t real_type;

  Number(value_type x) : val_(x) {}
  value_type value(const Evaluator<T>& p) const;
  bool can_evaluate(const Evaluator<T>&) const { return true; }
  void output(std::ostream&) const;
  Evaluatable<T>* clone() const { return new Number<T>(*this); }
private:
  value_type val_;
};

} // end namespace detail

//
// expression traits class
//

template<class T>
struct expression {
  typedef std::complex<T> value_type;
  typedef Expression<value_type> type;
  typedef Term<value_type> term_type;
};

template<class T>
struct expression<std::complex<T> > {
  typedef std::complex<T> value_type;
  typedef Expression<value_type> type;
  typedef Term<value_type> term_type;
};

template<class T>
struct expression<Expression<T> > {
  typedef T value_type;
  typedef Expression<value_type> type;
  typedef Term<value_type> term_type;
};

namespace detail {

template<class U, class T>
struct numeric_cast {
  static U value(T x) { return x; }
};

template<class U, class T>
struct numeric_cast<U, std::complex<T> > {
  static U value(const std::complex<T>& x) {
    if (x.imag() != 0)
      boost::throw_exception(std::runtime_error("can not convert complex to real"));
    return x.real();
  }
};

template<class U>
struct evaluate_helper
{
  typedef U value_type;
  template<class T>
  static U value(const Term<T>& ex, const Evaluator<T>&) { return ex; }
  template<class T>
  static U value(const Expression<T>& ex, const Evaluator<T>&) { return ex; }
  static U real(U u) { return u; }
};

template<class U>
struct evaluate_helper<Expression<U> >
{
  typedef U value_type;
  static Expression<U> value(const Term<U> ex, const Evaluator<U>& ev) {
    Term<U> t(ex);
    t.partial_evaluate(ev);
    return t;
  }
  static Expression<U> value(const Expression<U>& ex, const Evaluator<U>& ev) {
    Expression<U> e(ex);
    e.partial_evaluate(ev);
    return e;
  }
};

template<>
struct evaluate_helper<double>
{
  typedef double value_type;
  template<class T>
  static double value(const Term<T>& ex, const Evaluator<T>& ev)
  {
    return numeric_cast<double, T>::value(ex.value(ev));
  }
  template<class T>
  static double value(const Expression<T>& ex, const Evaluator<T>& ev)
  {
    return numeric_cast<double, T>::value(ex.value(ev));
  }
  static double real(double u) { return u; }
  static double imag(double) { return 0; }
  static bool can_evaluate_symbol(const std::string& name)
  {
    return (name=="Pi" || name=="PI" || name == "pi");
  }
  static double evaluate_symbol(const std::string& name)
  {
    if (name=="Pi" || name=="PI" || name == "pi") return std::acos(-1.);
    boost::throw_exception(std::runtime_error("can not evaluate " + name));
    return 0.;
  }
};

template<>
struct evaluate_helper<float>
{
  typedef float value_type;
  template<class T>
  static float value(const Term<T>& ex, const Evaluator<T>& ev)
  {
    return numeric_cast<float, T>::value(ex.value(ev));
  }
  template<class T>
  static float value(const Expression<T>& ex, const Evaluator<T>& ev)
  {
    return numeric_cast<float, T>::value(ex.value(ev));
  }
  static float real(float u) { return u; }
  static float imag(float) { return 0; }
  static bool can_evaluate_symbol(const std::string& name)
  {
    return (name=="Pi" || name=="PI" || name == "pi");
  }
  static float evaluate_symbol(const std::string& name)
  {
    if (name=="Pi" || name=="PI" || name == "pi") return std::acos(-1.);
    boost::throw_exception(std::runtime_error("can not evaluate " + name));
    return 0.;
  }
};

template<>
struct evaluate_helper<long double>
{
  typedef long double value_type;
  template<class T>
  static long double value(const Term<T>& ex, const Evaluator<T>& ev)
  {
    return numeric_cast<long double, T>::value(ex.value(ev));
  }
  template<class T>
  static long double value(const Expression<T>& ex, const Evaluator<T>& ev)
  {
    return numeric_cast<long double, T>::value(ex.value(ev));
  }
  static long double real(long double u) { return u; }
  static long double imag(long double) { return 0; }
  static bool can_evaluate_symbol(const std::string& name)
  {
    return (name=="Pi" || name=="PI" || name == "pi");
  }
  static long double evaluate_symbol(const std::string& name)
  {
    if (name=="Pi" || name=="PI" || name == "pi") return std::acos(-1.);
    boost::throw_exception(std::runtime_error("can not evaluate " + name));
    return 0.;
  }
};

template<class U>
struct evaluate_helper<std::complex<U> >
{
  typedef std::complex<U> value_type;
  template<class T>
  static std::complex<U> value(const Term<T>& ex, const Evaluator<T>& ev)
  {
    return ex.value(ev);
  }
  template<class T>
  static std::complex<U> value(const Expression<T>& ex, const Evaluator<T>& ev)
  {
    return ex.value(ev);
  }
  static U real(const std::complex<U>& u) { return u.real(); }
  static U imag(const std::complex<U>& u) { return u.imag(); }
  static bool can_evaluate_symbol(const std::string& name)
  {
    return (name=="Pi" || name=="PI" || name == "pi" || name == "I");
  }
  static std::complex<U> evaluate_symbol(const std::string& name)
  {
    if (name=="Pi" || name=="PI" || name == "pi") return std::acos(-1.);
    if (name=="I") return std::complex<U>(0.,1.);
    boost::throw_exception(std::runtime_error("can not evaluate " + name));
    return 0.;
  }
};

} // end namespace detail

template<class T>
inline bool can_evaluate(const detail::Evaluatable<T>& ex, const Evaluator<T>& ev)
{
  return ex.can_evaluate(ev);
}

template<class T>
inline bool can_evaluate(const std::string& v, const Evaluator<T>& p)
{
  return Expression<T>(v).can_evaluate(p);
}

inline bool can_evaluate(const std::string& v, const Parameters& p)
{
  return can_evaluate(v, ParameterEvaluator<>(p));
}

template<class U>
inline bool can_evaluate(const std::string& v, const Parameters& p, const U&)
{
  return can_evaluate(v, ParameterEvaluator<U>(p));
}

inline bool can_evaluate(const StringValue& v, const Parameters& p)
{
  return can_evaluate(static_cast<std::string>(v), p);
}

template<class U>
inline bool can_evaluate(const StringValue& v, const Parameters& p, const U&)
{
  return can_evaluate(static_cast<std::string>(v), p, U());
}

template<class U, class T>
inline U evaluate(const Expression<T>& ex, const Evaluator<T>& ev = Evaluator<T>())
{
  return detail::evaluate_helper<U>::value(ex, ev);
}

template<class U, class T>
inline U evaluate(const Term<T>& ex, const Evaluator<T>& ev = Evaluator<T>())
{
  return detail::evaluate_helper<U>::value(ex, ev);
}

template<class U, class T>
inline U evaluate(const std::string& v, const Evaluator<T>& ev)
{
  return detail::evaluate_helper<U>::value(Expression<T>(v), ev);
}

template<class U, class T>
inline U evaluate(const StringValue& v, const Evaluator<T>& ev)
{
  return evaluate<U>(static_cast<std::string>(v), ev);
}

template<class U>
inline U evaluate(const std::string& v)
{
  return evaluate<U,U>(v, Evaluator<typename detail::evaluate_helper<U>::value_type>());
}

template<class U>
inline U evaluate(const StringValue& v)
{
  return evaluate<U,U>(v, Evaluator<typename detail::evaluate_helper<U>::value_type>());
}

template<class U>
inline U evaluate(const std::string& v, const Parameters& p)
{
  return evaluate<U,typename detail::evaluate_helper<U>::value_type>(v, ParameterEvaluator<typename detail::evaluate_helper<U>::value_type>(p));
}

template<class U>
inline U evaluate(const StringValue& v, const Parameters& p)
{
  return evaluate<U,typename detail::evaluate_helper<U>::value_type>(v, ParameterEvaluator<typename detail::evaluate_helper<U>::value_type>(p));
}

//
// function is_zero and is_nonzero
//

template<class T>
bool is_zero(T x) { return x == T(0); }

template<class T>
bool is_zero(Expression<T> x)
{
  std::string s = boost::lexical_cast<std::string>(x);
  return s=="" || s=="0" || s=="0.";
}

template<class T>
bool is_zero(Term<T> x)
{
  std::string s = boost::lexical_cast<std::string>(x);
  return s=="" || s=="0" || s=="0.";
}

template<class T>
bool is_nonzero(T x) { return !is_zero(x); }

} // end namespace alps

#ifndef BOOST_NO_OPERATORS_IN_NAMESPACE
namespace alps {
namespace detail {
#endif

template<class T>
inline std::ostream& operator<<(std::ostream& os, const alps::detail::Evaluatable<T>& e)
{
  e.output(os);
  return os;
}

#ifndef BOOST_NO_OPERATORS_IN_NAMESPACE
} // end namespace detail
#endif

template<class T>
inline alps::Expression<T> operator+(const alps::Expression<T>& ex1, const alps::Expression<T>& ex2)
{
  alps::Expression<T> ex(ex1);
  ex += ex2;
  return ex;
}

template<class T>
inline std::istream& operator>>(std::istream& is, alps::Expression<T>& e)
{
  std::string s;
  is >> s;
  e.parse(s);
  return is;
}

template<class T>
inline bool operator==(const alps::Expression<T>& ex1, const alps::Expression<T>& ex2)
{
  return (boost::lexical_cast<std::string>(ex1) ==
          boost::lexical_cast<std::string>(ex2));
}

template<class T>
inline bool operator==(const alps::Expression<T>& ex, const std::string& s)
{
  return boost::lexical_cast<std::string>(ex) == s;
}

template<class T>
inline bool operator==(const std::string& s, const alps::Expression<T>& ex)
{
  return ex == s;
}

template<class T>
inline bool operator==(const alps::Factor<T>& ex1, const alps::Factor<T>& ex2)
{
  return (boost::lexical_cast<std::string>(ex1) ==
          boost::lexical_cast<std::string>(ex2));
}

template<class T>
inline bool operator==(const alps::Factor<T>& ex, const std::string& s)
{
  return boost::lexical_cast<std::string>(ex) == s;
}

template<class T>
inline bool operator==(const std::string& s, const alps::Factor<T>& ex)
{
  return ex == s;
}

template<class T>
inline bool operator==(const alps::Term<T>& ex1, const alps::Term<T>& ex2)
{
  return (boost::lexical_cast<std::string>(ex1) ==
          boost::lexical_cast<std::string>(ex2));
}

template<class T>
inline bool operator==(const alps::Term<T>& ex, const std::string& s)
{
  return boost::lexical_cast<std::string>(ex) == s;
}

template<class T>
inline bool operator==(const std::string& s, const alps::Term<T>& ex)
{
  return ex == s;
}

template<class T>
inline bool operator<(const alps::Expression<T>& ex1, const alps::Expression<T>& ex2)
{
  return (boost::lexical_cast<std::string>(ex1) <
          boost::lexical_cast<std::string>(ex2));
}

template<class T>
inline bool operator<(const alps::Expression<T>& ex, const std::string& s)
{
  return boost::lexical_cast<std::string>(ex) < s;
}

template<class T>
inline bool operator<(const std::string& s, const alps::Expression<T>& ex)
{
  return s < boost::lexical_cast<std::string>(ex);
}

template<class T>
inline bool operator<(const alps::Factor<T>& ex1, const alps::Factor<T>& ex2)
{
  return (boost::lexical_cast<std::string>(ex1) <
          boost::lexical_cast<std::string>(ex2));
}

template<class T>
inline bool operator<(const alps::Factor<T>& ex, const std::string& s)
{
  return boost::lexical_cast<std::string>(ex) < s;
}

template<class T>
inline bool operator<(const std::string& s, const alps::Factor<T>& ex)
{
  return s < boost::lexical_cast<std::string>(ex);
}

template<class T>
inline bool operator<(const alps::Term<T>& ex1, const alps::Term<T>& ex2)
{
  return (boost::lexical_cast<std::string>(ex1) <
          boost::lexical_cast<std::string>(ex2));
}

template<class T>
inline bool operator<(const alps::Term<T>& ex, const std::string& s)
{
  return boost::lexical_cast<std::string>(ex) < s;
}

template<class T>
inline bool operator<(const std::string& s, const alps::Term<T>& ex)
{
  return s < boost::lexical_cast<std::string>(ex);
}

#ifndef BOOST_NO_OPERATORS_IN_NAMESPACE
} // end namespace alps
#endif

#include <alps/expression2_impl.h>

#endif // ! ALPS_WITH_NEW_EXPRESSION

#endif // ! ALPS_EXPRESSION2_H