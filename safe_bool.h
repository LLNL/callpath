#ifndef SAFE_BOOL_H
#define SAFE_BOOL_H

///
/// Safe bool idiom, adapted from:
///   http://www.artima.com/cppsource/safeboolP.html
/// 
/// The original code is broken; it attempts access to protected
/// members from derived classes.  This version is fixed, but 
/// uses only non-virtual access to the bool_type() method.
/// 
/// To use, just add a method "bool boolean_test()" to your class
/// that returns what you want instances to evaluate to in a bool
/// context.
///
/// Example:
///
///   class Testable : public safe_bool <Testable> {
///   public:
///     bool boolean_test() const {
///       // Perform Boolean logic here
///     }
///   };
///
/// You can now use instances of Testable in a bool context safely.
///
template <typename T> class safe_bool {
protected:
  typedef void (safe_bool::*bool_type)() const;
  void this_type_does_not_support_comparisons() const {}

  safe_bool() {}
  safe_bool(const safe_bool&) {}
  safe_bool& operator=(const safe_bool&) {return *this;}
  ~safe_bool() {}

public:
  operator bool_type() const {
    return (static_cast<const T*>(this))->boolean_test()
      ? &safe_bool::this_type_does_not_support_comparisons : 0;
  }
};


//
// Below template methods disallow == and != comparisons unless they're
// implemented explicitly in your base class.  This prevents you from
// unintentionally comparing things as though they were bools.
//
template <typename T, typename U> 
void operator==(const safe_bool<T>& lhs,const safe_bool<U>& rhs) {
  lhs.this_type_does_not_support_comparisons();
  return false;
}

template <typename T,typename U> 
void operator!=(const safe_bool<T>& lhs,const safe_bool<U>& rhs) {
  lhs.this_type_does_not_support_comparisons();
  return false;
}

#endif // SAFE_BOOL_H
