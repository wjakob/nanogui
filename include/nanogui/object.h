/*
    nanogui/object.h -- Object base class with support for reference counting

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <nanogui/common.h>
#include <atomic>
#include <assert.h>

NAMESPACE_BEGIN(nanogui)

struct RttiClass
{
  const char *mRttiName;
  int mRttiObjSize;
  RttiClass* mRttiBase;
  uint32_t mRttiUid;

  //RttiClass* createObject();
  bool inherited(const RttiClass* base) const
  {
    //assert(this != nullptr);
    assert(base != nullptr);
    if (base == nullptr)
      return false;

    const RttiClass* self = this;
    while (self != nullptr)
    {
      if (self == base)
        return true;
      self = self->mRttiBase;
    }
    return false;
  }

  //static RttiClass* createObjectFromName(const char *name);
  //static RttiClass* createObjectFromUid(int rttid);
  //static void rttidump();
};


/**
 * \class Object object.h nanogui/object.h
 *
 * \brief Reference counted object base class.
 */
class NANOGUI_EXPORT Object {
public:
    /// Default constructor
    Object() { }

    /// Copy constructor
    Object(const Object &) : m_refCount(0) {}

    /// Return the current reference count
    int getRefCount() const { return m_refCount; };

    /// Increase the object's reference count by one
    void incRef() const { ++m_refCount; }

    /** \brief Decrease the reference count of
     * the object and possibly deallocate it.
     *
     * The object will automatically be deallocated once
     * the reference count reaches zero.
     */
    void decRef(bool dealloc = true) const;

    virtual RttiClass* rttiClass() const;
    inline bool isKindOf(const RttiClass* info) const
    {
      RttiClass* self = rttiClass();
      return self->inherited(info);
    }

    static const RttiClass rttiInfoObject;
protected:
    /** \brief Virtual protected deconstructor.
     * (Will only be called by \ref ref)
     */
    virtual ~Object();
private:
    mutable std::atomic<int> m_refCount { 0 };
};

struct StrHashNamingParams
{
  static constexpr uint32_t basis = 3277247372U;
  static constexpr uint32_t salt = 84190203;
};

constexpr uint32_t ClsNameHashRecursive(const char *s, uint32_t h = StrHashNamingParams::basis)
{
  return *s == 0 ? h
            : ClsNameHashRecursive(s + 1, ((int64_t)h * (int64_t)StrHashNamingParams::salt) ^ uint32_t(*s));
}

constexpr uint32_t StrNameFnHash(const char* str) { return ClsNameHashRecursive(str); }

#define RTTI_CLASS_UID(class_name) static const uint32_t RTTI_CLASS_UID = std::integral_constant<uint32_t, StrNameFnHash(#class_name)>::value;

#define RTTI_CLASS_INFO(class_name) ((RttiClass*)(&class_name::rttiInfo##class_name))
#define RTTI_CLASS(class_name) RTTI_CLASS_INFO(class_name)

#define RTTI_DECLARE_INFO(class_name) \
  static const RttiClass rttiInfo##class_name; \
  static const RttiClass* staticRttiClass(); \
  virtual RttiClass* rttiClass() const override; \
  static class_name* cast(Object*); \
  static const class_name* cast(const Object*);

#define RTTI_IMPLEMENT_INFO(class_name, base_class_name) \
  const RttiClass class_name::rttiInfo##class_name = { #class_name, sizeof(class class_name), RTTI_CLASS(base_class_name), class_name::RTTI_CLASS_UID }; \
  RttiClass* class_name::rttiClass() const { return RTTI_CLASS(class_name); } \
  const RttiClass* class_name::staticRttiClass() { return &class_name::rttiInfo##class_name; } \
  class_name* class_name::cast(Object*v) { if (v && v->isKindOf(class_name::staticRttiClass()))  return static_cast<class_name*>(v);  return nullptr; } \
  const class_name* class_name::cast(const Object*v) { if (v && v->isKindOf(class_name::staticRttiClass()))  return static_cast<const class_name*>(v);  return nullptr; }

/**
 * \class ref object.h nanogui/object.h
 *
 * \brief Reference counting helper.
 *
 * The \a ref template is a simple wrapper to store a pointer to an object. It
 * takes care of increasing and decreasing the object's reference count as
 * needed. When the last reference goes out of scope, the associated object
 * will be deallocated.
 *
 * The advantage over C++ solutions such as ``std::shared_ptr`` is that
 * the reference count is very compactly integrated into the base object
 * itself.
 */
template <typename T> class ref {
public:
    /// Create a ``nullptr``-valued reference
    ref() { }

    /// Construct a reference from a pointer
    ref(T *ptr) : m_ptr(ptr) {
        if (m_ptr)
            ((Object *) m_ptr)->incRef();
    }

    /// Copy constructor
    ref(const ref &r) : m_ptr(r.m_ptr) {
        if (m_ptr)
            ((Object *) m_ptr)->incRef();
    }

    /// Move constructor
    ref(ref &&r) noexcept : m_ptr(r.m_ptr) {
        r.m_ptr = nullptr;
    }

    /// Destroy this reference
    ~ref() {
        if (m_ptr)
            ((Object *) m_ptr)->decRef();
    }

    /// Move another reference into the current one
    ref& operator=(ref&& r) noexcept {
        if (&r != this) {
            if (m_ptr)
                ((Object *) m_ptr)->decRef();
            m_ptr = r.m_ptr;
            r.m_ptr = nullptr;
        }
        return *this;
    }

    /// Overwrite this reference with another reference
    ref& operator=(const ref& r) {
        if (m_ptr != r.m_ptr) {
            if (r.m_ptr)
                ((Object *) r.m_ptr)->incRef();
            if (m_ptr)
                ((Object *) m_ptr)->decRef();
            m_ptr = r.m_ptr;
        }
        return *this;
    }

    /// Overwrite this reference with a pointer to another object
    ref& operator=(T *ptr) {
        if (m_ptr != ptr) {
            if (ptr)
                ((Object *) ptr)->incRef();
            if (m_ptr)
                ((Object *) m_ptr)->decRef();
            m_ptr = ptr;
        }
        return *this;
    }

    /// Compare this reference with another reference
    bool operator==(const ref &r) const { return m_ptr == r.m_ptr; }

    /// Compare this reference with another reference
    bool operator!=(const ref &r) const { return m_ptr != r.m_ptr; }

    /// Compare this reference with a pointer
    bool operator==(const T* ptr) const { return m_ptr == ptr; }

    /// Compare this reference with a pointer
    bool operator!=(const T* ptr) const { return m_ptr != ptr; }

    /// Access the object referenced by this reference
    T* operator->() { return m_ptr; }

    /// Access the object referenced by this reference
    const T* operator->() const { return m_ptr; }

    /// Return a C++ reference to the referenced object
    T& operator*() { return *m_ptr; }

    /// Return a const C++ reference to the referenced object
    const T& operator*() const { return *m_ptr; }

    /// Return a pointer to the referenced object
    operator T* () { return m_ptr; }

    /// Return a const pointer to the referenced object
    T* get() { return m_ptr; }

    /// Return a pointer to the referenced object
    const T* get() const { return m_ptr; }

    /// Check if the object is defined
    operator bool() const { return m_ptr != nullptr; }
private:
    T *m_ptr = nullptr;
};

#define PROPSETTER(type,setter) template<typename FF, typename First, typename... Args> void set(const type& h, const Args&... args) { ((FF*)this)->setter(h.value);  ((FF*)this)->template set<FF, Args...>(args...); }
#define PROPSETTERVAL(type,setter) template<typename FF, typename First, typename... Args> void set(const type& h, const Args&... args) { ((FF*)this)->setter(h);  ((FF*)this)->template set<FF, Args...>(args...); }
#define DECLSETTER(name,type) struct NANOGUI_EXPORT name { type value; };
#define DECLSETTERDEF(name,type,def) struct NANOGUI_EXPORT name { type value = def; };
#define DECLSETTERARGS(name,type) struct NANOGUI_EXPORT name { type value; template<typename... Args> name(const Args&... args) { value = type(args...); }; };
#define DECLSETTERILIST(name,type) struct NANOGUI_EXPORT name { type value; template<typename... Args> name(const Args&... args) { value = type{args...}; }; };
#define DECLSETTERARGSNEW(name,type) struct NANOGUI_EXPORT name { type* value; template<typename... Args> name(const Args&... args) { value = new type(args...); }; };

NAMESPACE_END(nanogui)
