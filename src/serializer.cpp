#include <nanogui/serializer/core.h>
#include <iostream>
#include <string>

NAMESPACE_BEGIN(nanogui)

static const char *serialized_header_id = "SER_V1";
static const int serialized_header_id_length = 6;
static const int serialized_header_size =
    serialized_header_id_length + sizeof(uint64_t) + sizeof(uint32_t);

Serializer::Serializer(const std::string &filename, bool write_)
    : mFilename(filename), mWrite(write_), mCompatibility(false) {
    mFile.open(filename, write_ ? (std::ios::out | std::ios::trunc | std::ios::binary)
                                : (std::ios::in  | std::ios::binary));
    if (!mFile.is_open())
        throw std::runtime_error("Could not open \"" + filename + "\"!");

    if (!mWrite)
        readTOC();
    seek(serialized_header_size);
    mPrefixStack.push_back("");
}

Serializer::~Serializer() {
    if (mWrite)
        writeTOC();
}

bool Serializer::isSerializedFile(const std::string &filename) {
    Serializer s(filename, false);
    return true;
}

size_t Serializer::size() {
    mFile.seekg(0, std::ios_base::end);
    return (uint64_t) mFile.tellg();
}

void Serializer::push(const std::string &name) {
    mPrefixStack.push_back(mPrefixStack.back() + name + ".");
}

void Serializer::pop() {
    mPrefixStack.pop_back();
}

std::vector<std::string> Serializer::keys() const {
    const std::string &prefix = mPrefixStack.back();
    std::vector<std::string> result;
    for (auto const &kv : mTOC) {
        if (kv.first.substr(0, prefix.length()) == prefix)
            result.push_back(kv.first.substr(prefix.length()));
    }
    return result;
}

bool Serializer::get_base(const std::string &name,
                          const std::string &type_id) {
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

void Serializer::set_base(const std::string &name,
                          const std::string &type_id) {
    if (!mWrite)
        throw std::runtime_error("\"" + mFilename + "\": not open for writing!");

    std::string fullName = mPrefixStack.back() + name;
    auto it = mTOC.find(fullName);
    if (it != mTOC.end())
        throw std::runtime_error("\"" + mFilename + "\": field named \"" +
                                 fullName + "\" already exists!");

    mTOC[fullName] = std::make_pair(type_id, (uint64_t) mFile.tellp());
}

void Serializer::writeTOC() {
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

void Serializer::readTOC() {
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

void Serializer::read(void *p, size_t size) {
    mFile.read((char *) p, size);
    if (!mFile.good())
        throw std::runtime_error("\"" + mFilename +
                                 "\": I/O error while attempting to read " +
                                 std::to_string(size) + " bytes.");
}

void Serializer::write(const void *p, size_t size) {
    mFile.write((char *) p, size);
    if (!mFile.good())
        throw std::runtime_error(
            "\"" + mFilename + "\": I/O error while attempting to write " +
            std::to_string(size) + " bytes.");
}

void Serializer::seek(size_t pos) {
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

NAMESPACE_END(nanogui)
