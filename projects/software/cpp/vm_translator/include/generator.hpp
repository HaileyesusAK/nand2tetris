#ifndef __VM_TRANSLATOR__
#define __VM_TRANSLATOR__

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>


struct Segment;

using AsmInst = std::vector<std::string>;
using StackCodeMap = std::unordered_map<std::string, std::shared_ptr<Segment>>; 
enum class AluOperator {
    MINUS,
    PLUS,
    OR,
    AND
};

enum class RelOperator {
    EQ,
    GT,
    LT
};

struct Generator {
    virtual AsmInst generate();
    virtual AsmInst generate(const std::string& arg);
    virtual AsmInst generate(const std::string& arg, uint16_t idx);
    virtual AsmInst generate(const std::string& arg1, const std::string& arg2);
    virtual AsmInst generateBootstrap() final;
    virtual AsmInst generateClose() final;
};

struct Segment {
    AsmInst pushD {"@SP", "A=M", "M=D", "@SP", "M=M+1"};
    AsmInst popD {"@SP", "AM=M-1", "D=M"};

    AsmInst push(const AsmInst &prePush) const;
    virtual AsmInst push(uint16_t idx) const = 0;
    AsmInst pop(const AsmInst &postPop) const;
    virtual AsmInst pop(uint16_t idx) const;
};

struct StaticSegment: Segment {
    private:
    std::string fileName;

    public:
    AsmInst push(uint16_t idx) const;
    AsmInst pop(uint16_t idx) const;
    void setFileName(const std::string fileName);
};

struct ConstantSegment: Segment {
    AsmInst push(uint16_t value) const;
};

struct NamedSegment: Segment {
    AsmInst push(const std::string& name, uint16_t idx) const;
    AsmInst pop(const std::string& name, uint16_t idx) const;
};

struct LocalSegment: NamedSegment {
    AsmInst push(uint16_t idx) const;
    AsmInst pop(uint16_t idx) const;
};

struct ArgumentSegment: NamedSegment {
    AsmInst push(uint16_t idx) const;
    AsmInst pop(uint16_t idx) const;
};

struct ThisSegment: NamedSegment {
    AsmInst push(uint16_t idx) const;
    AsmInst pop(uint16_t idx) const;
};

struct ThatSegment: NamedSegment {
    AsmInst push(uint16_t idx) const;
    AsmInst pop(uint16_t idx) const;
};

struct FixedSegment: Segment {
    AsmInst push(uint16_t baseAddr, uint16_t idx) const;
    AsmInst pop(uint16_t baseAddr, uint16_t idx) const;
};

struct TempSegment: FixedSegment {
    private:
    static const uint16_t BASE_ADDR = 5; //RAM location 5
    public:
    AsmInst push(uint16_t idx) const;
    AsmInst pop(uint16_t idx) const;
};

struct PointerSegment: FixedSegment {
    private:
    static const uint16_t BASE_ADDR = 3; //RAM location 3, coincides with THIS 
    public:
    AsmInst push(uint16_t idx) const;
    AsmInst pop(uint16_t idx) const;
};

struct StackGenerator: Generator {
    private:
    static const StackCodeMap& getGeneratorMap();

    public:
    static void setStaticFileName(const std::string& fileName);
    AsmInst push(const std::string& segment, uint16_t idx);
    AsmInst pop(const std::string& segment, uint16_t idx);
};

struct StackPushGenerator: StackGenerator {
    AsmInst generate(const std::string& segment, uint16_t idx);
};

struct StackPopGenerator: StackGenerator {
    AsmInst generate(const std::string& segment, uint16_t idx);
};

struct FunctionGenerator: Generator {
    AsmInst generate(const std::string& fName, uint16_t nLocals);
};

struct CallGenerator: Generator {
    static const uint16_t ARG_START = 5;
    static const std::vector<std::string>& getSegments();
    AsmInst generate(const std::string& fName, uint16_t nArgs);
};

struct ReturnGenerator: Generator {
    AsmInst generate();
};

struct LabelGenerator: Generator {
    AsmInst generate(const std::string& funcName, const std::string& label);
};

struct GotoGenerator: Generator {
    AsmInst generate(const std::string& funcName, const std::string& label);
};

struct IfGotoGenerator: Generator {
    AsmInst generate(const std::string& funcName, const std::string& label);
};

struct ArithmeticGenerator: Generator {
    private:
    static const std::unordered_map<AluOperator, std::string>& getOperatorMap();

    public:
    AsmInst generate(const AluOperator& op);
};

struct AddGenerator: ArithmeticGenerator {
    AsmInst generate();
};

struct SubGenerator: ArithmeticGenerator {
    AsmInst generate();
};

struct AndGenerator: ArithmeticGenerator {
    AsmInst generate();
};

struct OrGenerator: ArithmeticGenerator {
    AsmInst generate();
};

struct RelGenerator: Generator {
    AsmInst generate(RelOperator op);
};

struct EqGenerator: RelGenerator {
    AsmInst generate();
};

struct LtGenerator: RelGenerator {
    AsmInst generate();
};

struct GtGenerator: RelGenerator {
    AsmInst generate();
};

struct NotGenerator: Generator {
    AsmInst generate();
};

struct NegGenerator: Generator {
    AsmInst generate();
};

#endif
