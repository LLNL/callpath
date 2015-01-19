/////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2010, Lawrence Livermore National Security, LLC.  
// Produced at the Lawrence Livermore National Laboratory  
// Written by Todd Gamblin, tgamblin@llnl.gov.
// LLNL-CODE-417602
// All rights reserved.  
// 
// This file is part of Libra. For details, see http://github.com/tgamblin/libra.
// Please also read the LICENSE file for further information.
// 
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
// 
//  * Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the disclaimer below.
//  * Redistributions in binary form must reproduce the above copyright notice, this list of
//    conditions and the disclaimer (as noted below) in the documentation and/or other materials
//    provided with the distribution.
//  * Neither the name of the LLNS/LLNL nor the names of its contributors may be used to endorse
//    or promote products derived from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
// OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// LAWRENCE LIVERMORE NATIONAL SECURITY, LLC, THE U.S. DEPARTMENT OF ENERGY OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
/////////////////////////////////////////////////////////////////////////////////////////////////
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
bool operator==(const safe_bool<T>& lhs,const safe_bool<U>& rhs) {
  lhs.this_type_does_not_support_comparisons();
  return false;
}

template <typename T,typename U> 
bool operator!=(const safe_bool<T>& lhs,const safe_bool<U>& rhs) {
  lhs.this_type_does_not_support_comparisons();
  return false;
}

#endif // SAFE_BOOL_H
