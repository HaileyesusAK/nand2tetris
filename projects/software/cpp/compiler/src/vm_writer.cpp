#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "vm_writer.hpp"

VmWriter::VmWriter(const fs::path _outputPath) : outputPath(_outputPath) {}


void VmWriter::writePush(const Segment& segment, uint16_t index) {
    instructions.push_back("push " + segments[segment] + " " + std::to_string(static_cast<int>(index)));
}

void VmWriter::writePop(const Segment& segment, uint16_t index) {
    instructions.push_back("pop " + segments[segment] + " " + std::to_string(static_cast<int>(index)));
}

void VmWriter::writeArithmetic(const Command& command) {
    instructions.push_back(commands[command]);
}

void VmWriter::writeLabel(const std::string& label) {
    instructions.push_back("label " + label);
}

void VmWriter::writeGoto(const std::string& label) {
    instructions.push_back("goto " + label);
}

void VmWriter::writeIf(const std::string& label) {
    instructions.push_back("if-goto " + label);
}

void VmWriter::writeCall(const std::string& name, uint16_t nArgs) {
    instructions.push_back("call " + name + " " + std::to_string(static_cast<int>(nArgs)));
}

void VmWriter::writeFunction(const std::string& name, uint16_t nLocals) {
    instructions.push_back("function " + name + " " + std::to_string(static_cast<int>(nLocals)));
}

void VmWriter::writeReturn() {
    instructions.push_back("return");
}

void VmWriter::write() {
	outputPath.replace_extension(".vm");
	std::ofstream os(outputPath);
	for(const auto& line : instructions)
		os << line << std::endl;
}
