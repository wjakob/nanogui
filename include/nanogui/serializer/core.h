/*
    NanoGUI was developed by Wenzel Jakob <wenzel@inf.ethz.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/**
 * \file nanogui/serializer/core.h
 *
 * \brief Helper class to serialize the full state of an application to a
 *        convenient binary format.
 */

#pragma once

#include <nanogui/widget.h>
#include <unordered_map>
#include <fstream>
#include <memory>
#include <set>
#include <iostream>

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
 * are in :ref:`file_nanogui_serializer_core.h`, see :class:`nanogui::Serializer`.
 * \endrst
 */
template <typename T> struct serialization_helper;
NAMESPACE_END(detail)

/// Utility variable for \ref nanogui::Serializer --- do not modify.
static const char *serialized_header_id = "SER_V1";

/// Utility variable for \ref nanogui::Serializer --- do not modify.
static const int serialized_header_id_length = 6;

/// Utility variable for \ref nanogui::Serializer --- do not modify.
static const int serialized_header_size =
    serialized_header_id_length + sizeof(uint64_t) + sizeof(uint32_t);

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
 *
 * \rst
 * .. tip:: See :ref:`nanogui_example_5` for example usage of this class.
 * \endrst
 */
class Serializer {
protected:
// this friendship breaks the documentation
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    template <typename T> friend struct detail::serialization_helper;
#endif

public:
    /// Create a new serialized file for reading or writing
    Serializer(const std::string &filename, bool write)
    : mFilename(filename)
    , mWrite(write)
    , mCompatibility(false) {

        mFile.open(filename, write ? (std::ios::out | std::ios::trunc | std::ios::binary)
                                   : (std::ios::in  | std::ios::binary));
        if (!mFile.is_open())
            throw std::runtime_error("Could not open \"" + filename + "\"!");

        if (!mWrite)
            readTOC();
        seek(serialized_header_size);
        mPrefixStack.push_back("");
    }

    /// Release all resources
    ~Serializer() {
        if (mWrite)
            writeTOC();
    }

    /// Check whether a file contains serialized data
    static bool isSerializedFile(const std::string &filename) {
        try {
            Serializer s(filename, false);
            return true;
        } catch (const std::exception &) {
            return false;
        }
    }

    /// Return the current size of the output file
    size_t size() {
        mFile.seekg(0, std::ios_base::end);
        return (uint64_t) mFile.tellg();
    }

    /**
     * Push a name prefix onto the stack (use this to isolate
     * identically-named data fields)
     */
    void push(const std::string &name) {
        mPrefixStack.push_back(mPrefixStack.back() + name + ".");
    }

    /// Pop a name prefix from the stack
    void pop() {
        mPrefixStack.pop_back();
    }

    /// Return all field names under the current name prefix
    std::vector<std::string> keys() const {
        const std::string &prefix = mPrefixStack.back();
        std::vector<std::string> result;
        for (auto const &kv : mTOC) {
            if (kv.first.substr(0, prefix.length()) == prefix)
                result.push_back(kv.first.substr(prefix.length()));
        }
        return result;
    }

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
    /// The core ``set`` method; intended use only by public ``set`` method.
    void set_base(const std::string &name, const std::string &type_id) {
        if (!mWrite)
            throw std::runtime_error("\"" + mFilename + "\": not open for writing!");

        std::string fullName = mPrefixStack.back() + name;
        auto it = mTOC.find(fullName);
        if (it != mTOC.end())
            throw std::runtime_error("\"" + mFilename + "\": field named \"" +
                                     fullName + "\" already exists!");

        mTOC[fullName] = std::make_pair(type_id, (uint64_t) mFile.tellp());
    }

    /// The core ``get`` method; intended use only by public ``get`` method.
    bool get_base(const std::string &name, const std::string &type_id) {
        if (mWrite)
            throw std::runtime_error("\"" + mFilename +
                                     "\": not open for reading!");

        std::string fullName = mPrefixStack.back() + name;

        auto it = mTOC.find(fullName);
        if (it == mTOC.end()) {
            std::string message = "\"" + mFilename +
                                  "\": unable to find field named \"" +
                                  fullName + "\"!";
            if (!mCompatibility)
                throw std::runtime_error(message);
            else
                std::cerr << "Warning: " << message << std::endl;

            return false;
        }

        const auto &record = it->second;
        if (record.first != type_id)
            throw std::runtime_error(
                "\"" + mFilename + "\": field named \"" + fullName +
                "\" has an incompatible type (expected \"" + type_id +
                "\", got \"" + record.first + "\")!");

        seek((size_t) record.second);

        return true;
    }

    /// Writes the TOC (heading information) for the serialized file.
    void writeTOC() {
        uint64_t trailer_offset = (uint64_t) mFile.tellp();
        uint32_t nItems = (uint32_t) mTOC.size();

        seek(0);
        write(serialized_header_id, serialized_header_id_length);
        write(&trailer_offset, sizeof(uint64_t));
        write(&nItems, sizeof(uint32_t));
        seek((size_t) trailer_offset);

        for (auto item : mTOC) {
            uint16_t size = (uint16_t) item.first.length();
            write(&size, sizeof(uint16_t));
            write(item.first.c_str(), size);
            size = (uint16_t) item.second.first.length();
            write(&size, sizeof(uint16_t));
            write(item.second.first.c_str(), size);

            write(&item.second.second, sizeof(uint64_t));
        }
    }

    /// Reads the TOC (heading information) for the serialized file.
    void readTOC() {
        uint64_t trailer_offset = 0;
        uint32_t nItems = 0;
        char header[serialized_header_id_length];

        read(header, serialized_header_id_length);
        if (memcmp(header, serialized_header_id, serialized_header_id_length) != 0)
            throw std::runtime_error("\"" + mFilename + "\": invalid file format!");
        read(&trailer_offset, sizeof(uint64_t));
        read(&nItems, sizeof(uint32_t));
        mFile.seekg(trailer_offset);

        for (uint32_t i = 0; i < nItems; ++i) {
            std::string field_name, type_id;
            uint16_t size;
            uint64_t offset;

            read(&size, sizeof(uint16_t)); field_name.resize(size);
            read((char *) field_name.data(), size);
            read(&size, sizeof(uint16_t)); type_id.resize(size);
            read((char *) type_id.data(), size);
            read(&offset, sizeof(uint64_t));

            mTOC[field_name] = std::make_pair(type_id, offset);
        }
    }

    /// Reads in the serialized file's data.
    void read(void *p, size_t size) {
        mFile.read((char *) p, size);
        if (!mFile.good())
            throw std::runtime_error("\"" + mFilename +
                                     "\": I/O error while attempting to read " +
                                     std::to_string(size) + " bytes.");
    }

    /// Writes out the serialized file's data.
    void write(const void *p, size_t size) {
        mFile.write((char *) p, size);
        if (!mFile.good())
            throw std::runtime_error(
                "\"" + mFilename + "\": I/O error while attempting to write " +
                std::to_string(size) + " bytes.");
    }

    /// Seeks to a specific position in the serialized file.
    void seek(size_t pos) {
        if (mWrite)
            mFile.seekp(pos);
        else
            mFile.seekg(pos);

        if (!mFile.good())
            throw std::runtime_error(
                "\"" + mFilename +
                "\": I/O error while attempting to seek to offset " +
                std::to_string(pos) + ".");
    }

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

// bypass template specializations for now
#ifndef DOXYGEN_SHOULD_SKIP_THIS
template <> struct serialization_traits<int8_t>           { const char *type_id = "u8";  };
template <> struct serialization_traits<uint8_t>          { const char *type_id = "s8";  };
template <> struct serialization_traits<int16_t>          { const char *type_id = "u16"; };
template <> struct serialization_traits<uint16_t>         { const char *type_id = "s16"; };
template <> struct serialization_traits<int32_t>          { const char *type_id = "u32"; };
template <> struct serialization_traits<uint32_t>         { const char *type_id = "s32"; };
template <> struct serialization_traits<int64_t>          { const char *type_id = "u64"; };
template <> struct serialization_traits<uint64_t>         { const char *type_id = "s64"; };
template <> struct serialization_traits<half_float::half> { const char *type_id = "f16"; };
template <> struct serialization_traits<float>            { const char *type_id = "f32"; };
template <> struct serialization_traits<double>           { const char *type_id = "f64"; };
template <> struct serialization_traits<bool>             { const char *type_id = "b8";  };
template <> struct serialization_traits<char>             { const char *type_id = "c8";  };

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
