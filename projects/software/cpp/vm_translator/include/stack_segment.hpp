#ifndef __SEGMENT_H__
#define __SEGMENT_H__ 

#include <string>
#include <vector>

using AsmInst = std::vector<std::string>;

struct StackSegment {
    public:
    virtual AsmInst push(uint16_t idx) = 0;
    virtual AsmInst pop(uint16_t idx) = 0;

    protected:
    static const inline AsmInst instPushD {"@SP", "A=M", "M=D", "@SP", "M=M+1"};
    static const inline AsmInst instPopD {"@SP", "AM=M-1", "D=M"};
    AsmInst pushNamedSegment(const std::string& name, uint16_t idx);
    AsmInst popNamedSegment(const std::string& name, uint16_t idx);
    AsmInst pushD(const AsmInst &prePush);
    AsmInst popD(const AsmInst &postPop);
};

struct StaticSegment: StackSegment {
    private:
    std::string fileName;

    public:
    AsmInst push(uint16_t idx);
    AsmInst pop(uint16_t idx);
    void setFileName(const std::string& s);
};

struct ConstantSegment: StackSegment {
    AsmInst push(uint16_t value);
    AsmInst pop(uint16_t idx);
};

struct LocalSegment: StackSegment {
    AsmInst push(uint16_t idx);
    AsmInst pop(uint16_t idx);
};

struct ArgumentSegment: StackSegment {
    AsmInst push(uint16_t idx);
    AsmInst pop(uint16_t idx);
};

struct ThisSegment: StackSegment {
    AsmInst push(uint16_t idx);
    AsmInst pop(uint16_t idx);
};

struct ThatSegment: StackSegment {
    AsmInst push(uint16_t idx);
    AsmInst pop(uint16_t idx);
};

struct TempSegment: StackSegment {
    private:
    static const uint16_t BASE_ADDR = 5; //RAM location 5

    public:
    AsmInst push(uint16_t idx);
    AsmInst pop(uint16_t idx);
};

struct PointerSegment: StackSegment {
    private:
    static const uint16_t BASE_ADDR = 3; //RAM location 3, coincides with THIS 

    public:
    AsmInst push(uint16_t idx);
    AsmInst pop(uint16_t idx);
};

#endif
