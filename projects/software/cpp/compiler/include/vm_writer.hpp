#ifndef __VM_WRITER_H__
#define __VM_WRITER_H__

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace fs = std::filesystem;

enum class Segment {
   CONST, ARGUMENT, LOCAL, STATIC, THIS, THAT, POINTER, TEMP 
};

enum class Command {
    ADD, SUB, NEG, EQ, GT, LT, AND, OR, NOT
};

class VmWriter {
    fs::path outputPath;
    std::vector<std::string> instructions;
    static inline std::unordered_map<Segment, std::string> segments {
        {Segment::CONST, "constant"}, {Segment::ARGUMENT, "argument"}, {Segment::LOCAL, "local"},
        {Segment::STATIC, "static"}, {Segment::THIS, "this"}, {Segment::THAT, "that"},
        {Segment::POINTER, "pointer"}, {Segment::TEMP, "temp"},
    };
    static inline std::unordered_map<Command, std::string> commands {
        {Command::ADD, "add"}, {Command::SUB, "sub"}, {Command::NEG, "neg"},
        {Command::EQ, "eq"}, {Command::GT, "gt"}, {Command::LT, "lt"},
        {Command::AND, "and"}, {Command::OR, "or"}, {Command::NOT, "not"}
    };

    public:
    VmWriter(const fs::path outputPath);
    void writePush(const Segment& segment, uint16_t index);
    void writePop(const Segment& segment, uint16_t index);
    void writeArithmetic(const Command& command);
    void writeLabel(const std::string& label);
    void writeGoto(const std::string& label);
    void writeIf(const std::string& label);
    void writeCall(const std::string& name, uint16_t nArgs);
    void writeFunction(const std::string& name, uint16_t nLocals);
    void writeReturn();
    void write();
};

#endif
