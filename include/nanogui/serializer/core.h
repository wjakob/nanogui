/*
    nanogui/serializer/core.h -- helper class to serialize
    the full state of an application to a convenient binary format

    NanoGUI was developed by Wenzel Jakob <wenzel@inf.ethz.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <nanogui/widget.h>
#include <unordered_map>
#include <fstream>
#include <memory>
#include <set>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace half_float { class half; }
#endif

NAMESPACE_BEGIN(nanogui)

NAMESPACE_BEGIN(detail)
/**
 * \struct serialization_helper core.h nanogui/serializer/core.h
 *
 * \rst
 * The primary serialization helper class; preliminary specializations
 * are in :ref:`file_include_nanogui_serializer_core.h`, see :ref:`class_nanogui__Serializer`.
 * \endrst
 */
template <typename T> struct serialization_helper;
NAMESPACE_END(detail)

/**
 * \class Serializer core.h nanogui/serializer/core.h
 *
 * \brief Serialization helper class
 *
 * This class can be used to store and retrieve a great variety of C++ data
 * types using a compact binary file format. The intended purpose is to quickly
 * save and restore the complete state of an application, e.g. to facilitate
 * debugging sessions. This class supports all core C++ types, NanoGUI widgets,
 * sparse and dense Eigen matrices, as well as OpenGL shaders and buffer
 * objects.
 *
 * Note that this header file just provides the basics; the files
 * ``nanogui/serializer/opengl.h``, and ``nanogui/serializer/sparse.h`` must
 * be included to serialize the respective data types.
 */
class Serializer {
protected:
// this friendship breaks the documentation
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    template <typename T> friend struct detail::serialization_helper;
#endif

public:
    /// Create a new serialized file for reading or writing
    Serializer(const std::string &filename, bool write);

    /// Release all resources
    ~Serializer();

    /// Check whether a file contains serialized data
    static bool isSerializedFile(const std::string &filename);

    /// Return the current size of the output file
    size_t size();

    /**
     * Push a name prefix onto the stack (use this to isolate
     * identically-named data fields)
     */
    void push(const std::string &name);

    /// Pop a name prefix from the stack
    void pop();

    /// Return all field names under the current name prefix
    std::vector<std::string> keys() const;

    /**
     * \brief Enable/disable compatibility mode
     *
     * When enabled, missing attributes cause a warning to be printed,
     * but \ref get() does not throw an exception.
     */
    void setCompatibility(bool compatibility) { mCompatibility = compatibility; }

    /// Return whether compatibility mode is enabled
    bool compatibility() { return mCompatibility; }

    /// Store a field in the serialized file (when opened with ``write=true``)
    template <typename T> void set(const std::string &name, const T &value) {
        typedef detail::serialization_helper<T> helper;
        set_base(name, helper::type_id());
        if (!name.empty())
            push(name);
        helper::write(*this, &value, 1);
        if (!name.empty())
            pop();
    }

    /// Retrieve a field from the serialized file (when opened with ``write=false``)
    template <typename T> bool get(const std::string &name, T &value) {
        typedef detail::serialization_helper<T> helper;
        if (!get_base(name, helper::type_id()))
            return false;
        if (!name.empty())
            push(name);
        helper::read(*this, &value, 1);
        if (!name.empty())
            pop();
        return true;
    }
protected:
    void set_base(const std::string &name, const std::string &type_id);
    bool get_base(const std::string &name, const std::string &type_id);

    void writeTOC();
    void readTOC();

    void read(void *p, size_t size);
    void write(const void *p, size_t size);
    void seek(size_t pos);
private:
    std::string mFilename;
    bool mWrite, mCompatibility;
    std::fstream mFile;
    std::unordered_map<std::string, std::pair<std::string, uint64_t>> mTOC;
    std::vector<std::string> mPrefixStack;
};

NAMESPACE_BEGIN(detail)

/**
 * \struct serialization_traits core.h nanogui/seralizer/core.h
 *
 * \brief SFINAE helper struct for generic traits serialization.
 *
 * Must be fully specialized for any type that needs to be serialized.
 *
 * \tparam T
 *     The type to explicity be serialized.
 */
template <typename T, typename SFINAE = void> struct serialization_traits { };

/// Serialization id for ``int8_t`` is ``u8``.
template <> struct serialization_traits<int8_t>           { const char *type_id = "u8";  };
/// Serialization id for ``uint8_t`` is ``s8``.
template <> struct serialization_traits<uint8_t>          { const char *type_id = "s8";  };
/// Serialization id for ``int16_t`` is ``u16``.
template <> struct serialization_traits<int16_t>          { const char *type_id = "u16"; };
/// Serialization id for ``uint16_t`` is ``s16``.
template <> struct serialization_traits<uint16_t>         { const char *type_id = "s16"; };
/// Serialization id for ``int32_t`` is ``u32``.
template <> struct serialization_traits<int32_t>          { const char *type_id = "u32"; };
/// Serialization id for ``uint32_t`` is ``s32``.
template <> struct serialization_traits<uint32_t>         { const char *type_id = "s32"; };
/// Serialization id for ``int64_t`` is ``u64``.
template <> struct serialization_traits<int64_t>          { const char *type_id = "u64"; };
/// Serialization id for ``uint64_t`` is ``s64``.
template <> struct serialization_traits<uint64_t>         { const char *type_id = "s64"; };
#ifndef DOXYGEN_SHOULD_SKIP_THIS
/// Serialization id for ``half_float::half`` is ``f16``.
template <> struct serialization_traits<half_float::half> { const char *type_id = "f16"; };
#endif
/// Serialization id for ``float`` is ``f32``.
template <> struct serialization_traits<float>            { const char *type_id = "f32"; };
/// Serialization id for ``double`` is ``f64``.
template <> struct serialization_traits<double>           { const char *type_id = "f64"; };
/// Serialization id for ``bool`` is ``b8``.
template <> struct serialization_traits<bool>             { const char *type_id = "b8";  };
/// Serialization id for ``char`` is ``c8``.
template <> struct serialization_traits<char>             { const char *type_id = "c8";  };

#ifndef DOXYGEN_SHOULD_SKIP_THIS
template <typename T> struct serialization_traits<T> :
    serialization_traits<typename std::underlying_type<T>::type,
                         typename std::enable_if<std::is_enum<T>::value>::type> { };

template <typename T> struct serialization_helper {
    static std::string type_id() { return serialization_traits<T>().type_id; }

    static void write(Serializer &s, const T *value, size_t count) {
        s.write(value, sizeof(T) * count);
    }

    static void read(Serializer &s, T *value, size_t count) {
        s.read(value, sizeof(T) * count);
    }
};

template <> struct serialization_helper<std::string> {
    static std::string type_id() { return "Vc8"; }

    static void write(Serializer &s, const std::string *value, size_t count) {
        for (size_t i = 0; i<count; ++i) {
            uint32_t length = (uint32_t) value->length();
            s.write(&length, sizeof(uint32_t));
            s.write((char *) value->data(), sizeof(char) * value->length());
            value++;
        }
    }

    static void read(Serializer &s, std::string *value, size_t count) {
        for (size_t i = 0; i<count; ++i) {
            uint32_t length;
            s.read(&length, sizeof(uint32_t));
            value->resize(length);
            s.read((char *) value->data(), sizeof(char) * length);
            value++;
        }
    }
};

template <typename T1, typename T2> struct serialization_helper<std::pair<T1, T2>> {
    static std::string type_id() {
        return "P" +
            serialization_helper<T1>::type_id() +
            serialization_helper<T2>::type_id();
    }

    static void write(Serializer &s, const std::pair<T1, T1> *value, size_t count) {
        std::unique_ptr<T1> first (new T1[count]);
        std::unique_ptr<T2> second(new T2[count]);

        for (size_t i = 0; i<count; ++i) {
            first.get()[i]  = value[i].first;
            second.get()[i] = value[i].second;
        }

        serialization_helper<T1>::write(s, first.get(), count);
        serialization_helper<T2>::write(s, second.get(), count);
    }

    static void read(Serializer &s, std::pair<T1, T1> *value, size_t count) {
        std::unique_ptr<T1> first (new T1[count]);
        std::unique_ptr<T2> second(new T2[count]);

        serialization_helper<T1>::read(s, first.get(), count);
        serialization_helper<T2>::read(s, second.get(), count);

        for (size_t i = 0; i<count; ++i) {
            value[i].first = first.get()[i];
            value[i].second = second.get()[i];
        }
    }
};

template <typename T> struct serialization_helper<std::vector<T>> {
    static std::string type_id() {
        return "V" + serialization_helper<T>::type_id();
    }

    static void write(Serializer &s, const std::vector<T> *value, size_t count) {
        for (size_t i = 0; i<count; ++i) {
            uint32_t size = (uint32_t) value->size();
            s.write(&size, sizeof(uint32_t));
            serialization_helper<T>::write(s, value->data(), size);
            value++;
        }
    }

    static void read(Serializer &s, std::vector<T> *value, size_t count) {
        for (size_t i = 0; i<count; ++i) {
            uint32_t size = 0;
            s.read(&size, sizeof(uint32_t));
            value->resize(size);
            serialization_helper<T>::read(s, value->data(), size);
            value++;
        }
    }
};

template <typename T> struct serialization_helper<std::set<T>> {
    static std::string type_id() {
        return "S" + serialization_helper<T>::type_id();
    }

    static void write(Serializer &s, const std::set<T> *value, size_t count) {
        for (size_t i = 0; i<count; ++i) {
            std::vector<T> temp(value->size());
            uint32_t idx = 0;
            for (auto it = value->begin(); it != value->end(); ++it)
                temp[idx++] = *it;
            serialization_helper<std::vector<T>>::write(s, &temp, 1);
            value++;
        }
    }

    static void read(Serializer &s, std::set<T> *value, size_t count) {
        for (size_t i = 0; i<count; ++i) {
            std::vector<T> temp;
            serialization_helper<std::vector<T>>::read(s, &temp, 1);
            value->clear();
            for (auto k: temp)
                value->insert(k);
            value++;
        }
    }
};

template <typename Scalar, int Rows, int Cols, int Options, int MaxRows, int MaxCols>
struct serialization_helper<Eigen::Matrix<Scalar, Rows, Cols, Options, MaxRows, MaxCols>> {
    typedef Eigen::Matrix<Scalar, Rows, Cols, Options, MaxRows, MaxCols> Matrix;

    static std::string type_id() {
        return "M" + serialization_helper<Scalar>::type_id();
    }

    static void write(Serializer &s, const Matrix *value, size_t count) {
        for (size_t i = 0; i<count; ++i) {
            uint32_t rows = value->rows(), cols = value->cols();
            s.write(&rows, sizeof(uint32_t));
            s.write(&cols, sizeof(uint32_t));
            serialization_helper<Scalar>::write(s, value->data(), rows*cols);
            value++;
        }
    }

    static void read(Serializer &s, Matrix *value, size_t count) {
        for (size_t i = 0; i<count; ++i) {
            uint32_t rows = 0, cols = 0;
            s.read(&rows, sizeof(uint32_t));
            s.read(&cols, sizeof(uint32_t));
            value->resize(rows, cols);
            serialization_helper<Scalar>::read(s, value->data(), rows*cols);
            value++;
        }
    }
};

template <> struct serialization_helper<nanogui::Color>
    : public serialization_helper<Eigen::Matrix<float, 4, 1>> { };

template <typename Scalar, int Options>
struct serialization_helper<Eigen::Quaternion<Scalar, Options>>
    : public serialization_helper<Eigen::Matrix<Scalar, 4, 1>> {
    typedef Eigen::Quaternion<Scalar, Options> Quat;

    static std::string type_id() {
        return "Q" + serialization_helper<Scalar>::type_id();
    }

    static void write(Serializer &s, const Quat *value, size_t count) {
        for (size_t i = 0; i<count; ++i) {
            serialization_helper<Scalar>::write(s, value->coeffs().data(), 4);
            value++;
        }
    }

    static void read(Serializer &s, Quat *value, size_t count) {
        for (size_t i = 0; i<count; ++i) {
            serialization_helper<Scalar>::read(s, value->coeffs().data(), 4);
            value++;
        }
    }
};

template <>
struct serialization_helper<Widget> {
    static std::string type_id() {
        return "W";
    }

    static void write(Serializer &s, const Widget *value, size_t count) {
        for (size_t i = 0; i<count; ++i) {
            if (!value->id().empty()) {
                if (count > 1)
                    s.push(value->id());
                value->save(s);
            }

            for (const Widget *child : value->children()) {
                if (child->id().empty())
                    write(s, child, 1);
                else
                    s.set(child->id(), *child);
            }

            if (!value->id().empty() && count > 1)
                s.pop();

            ++value;
        }
    }

    static void read(Serializer &s, Widget *value, size_t count) {
        for (size_t i = 0; i<count; ++i) {
            if (!value->id().empty()) {
                if (count > 1)
                    s.push(value->id());
                value->load(s);
            }

            for (Widget *child : value->children()) {
                if (child->id().empty())
                    read(s, child, 1);
                else
                    s.get(child->id(), *child);
            }

            if (!value->id().empty() && count > 1)
                s.pop();

            ++value;
        }
    }
};

#endif // DOXYGEN_SHOULD_SKIP_THIS

NAMESPACE_END(detail)
NAMESPACE_END(nanogui)
