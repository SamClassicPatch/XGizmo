/* Copyright (c) 2024 Dreamy Cecil
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

#ifndef XGIZMO_INCL_ANYVALUE_H
#define XGIZMO_INCL_ANYVALUE_H

#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <algorithm>

// Type-safe container for single values of any type
class CAnyValue {
  public:
    // Supported types
    enum EType {
      E_NULL   = 0,
      E_BOOL   = 1,
      E_INDEX  = 2,
      E_FLOAT  = 3,
      E_DOUBLE = 4,
      E_STRING = 5,
      E_PTR    = 6,
    };

    // Value placeholder
    struct Placeholder {
      virtual ~Placeholder() {};
      virtual EType GetType() const = 0;
      virtual Placeholder *Clone() const = 0;
    };

    // Value holder of a specific type
    template<class Type, EType eType>
    struct Holder : public Placeholder {
      Type _value;

      Holder(const Type &valSet) : _value(valSet) {};
      virtual EType GetType() const { return eType; };
      virtual Placeholder *Clone() const { return new Holder(_value); };
    };

    typedef Holder<INDEX,    E_BOOL>   Bool_t;
    typedef Holder<INDEX,    E_INDEX>  Int_t;
    typedef Holder<FLOAT,    E_FLOAT>  Float_t;
    typedef Holder<DOUBLE,   E_DOUBLE> Double_t;
    typedef Holder<CTString, E_STRING> String_t;
    typedef Holder<void *,   E_PTR>    Ptr_t;

  private:
    Placeholder *_content; // Currently held value

  public:
    // Default constructor
    CAnyValue() : _content(NULL) {};

    // Constructors from supported types
    CAnyValue(bool   bSet) : _content(new Bool_t  (bSet)) {};
    CAnyValue(int    iSet) : _content(new Int_t   (iSet)) {};
    CAnyValue(float  fSet) : _content(new Float_t (fSet)) {};
    CAnyValue(double fSet) : _content(new Double_t(fSet)) {};
    CAnyValue(const CTString &strSet) : _content(new String_t(strSet)) {};
    CAnyValue(const char     *strSet) : _content(new String_t(strSet)) {};
    CAnyValue(void             *pSet) : _content(new Ptr_t(pSet)) {};

    // Copy constructor
    CAnyValue(const CAnyValue &other) : _content(!other.IsEmpty() ? other._content->Clone() : NULL) {};

    // Destructor
    ~CAnyValue() {
      delete _content;
    };

  public:
    // Get value type
    inline EType GetType() const {
      return !IsEmpty() ? _content->GetType() : E_NULL;
    };

    // Swap values
    inline void Swap(CAnyValue &other) {
      std::swap(_content, other._content);
    };

    // Assign a new value by swapping values with a temporary copy
    inline CAnyValue &operator=(const CAnyValue &other) {
      CAnyValue(other).Swap(*this);
      return *this;
    };

    // Check if value is empty
    inline bool IsEmpty() const {
      return _content == NULL;
    };

  // Value accessors
  public:

    inline INDEX &GetIndex(void) {
      ASSERT(GetType() == E_BOOL || GetType() == E_INDEX);
      return ((Int_t *)_content)->_value;
    };

    inline FLOAT &GetFloat(void) {
      ASSERT(GetType() == E_FLOAT);
      return ((Float_t *)_content)->_value;
    };

    inline DOUBLE &GetDouble(void) {
      ASSERT(GetType() == E_DOUBLE);
      return ((Double_t *)_content)->_value;
    };

    inline CTString &GetString(void) {
      ASSERT(GetType() == E_STRING);
      return ((String_t *)_content)->_value;
    };

    inline void *&GetPtr(void) {
      ASSERT(GetType() == E_PTR);
      return ((Ptr_t *)_content)->_value;
    };
    
  // Value converters
  public:

    inline bool IsTrue(void) const {
      switch (GetType()) {
        case E_BOOL:
        case E_INDEX:  return ((Int_t    *)_content)->_value != 0;
        case E_FLOAT:  return ((Float_t  *)_content)->_value != 0.0f;
        case E_DOUBLE: return ((Double_t *)_content)->_value != 0.0;
        case E_STRING: return ((String_t *)_content)->_value.Length() != 0;
        case E_PTR:    return ((Ptr_t    *)_content)->_value != NULL;
      }

      ASSERTALWAYS("Unknown value type in CAnyValue::IsTrue()");
      return false;
    };

    // Implicit conversion
    __forceinline operator bool() const {
      return IsTrue();
    };

    inline INDEX ToIndex(void) const {
      switch (GetType()) {
        case E_BOOL:
        case E_INDEX:  return ((Int_t    *)_content)->_value;
        case E_FLOAT:  return ((Float_t  *)_content)->_value;
        case E_DOUBLE: return ((Double_t *)_content)->_value;
      }

      ASSERTALWAYS("Unknown value type in CAnyValue::ToIndex()");
      return 0;
    };

    inline DOUBLE ToFloat(void) const {
      switch (GetType()) {
        case E_BOOL:
        case E_INDEX:  return ((Int_t    *)_content)->_value;
        case E_FLOAT:  return ((Float_t  *)_content)->_value;
        case E_DOUBLE: return ((Double_t *)_content)->_value;
      }

      ASSERTALWAYS("Unknown value type in CAnyValue::ToFloat()");
      return 0.0;
    };

    inline CTString ToString(void) const {
      switch (GetType()) {
        case E_BOOL:   return ((Int_t   *)_content)->_value ? "1" : "0";
        case E_INDEX:  return CTString(0, "%d", ((Int_t    *)_content)->_value);
        case E_FLOAT:  return CTString(0, "%g", ((Float_t  *)_content)->_value);
        case E_DOUBLE: return CTString(0, "%g", ((Double_t *)_content)->_value);
        case E_STRING: return ((String_t *)_content)->_value;
        case E_PTR:    return CTString(0, "0x%p", ((Ptr_t  *)_content)->_value);
      }

      ASSERTALWAYS("Unknown value type in CAnyValue::ToString()");
      return "";
    };
};

#endif
