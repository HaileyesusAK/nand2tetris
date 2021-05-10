#include <algorithm>
#include <string>
#include <unordered_map>
#include "stack_segment.hpp"

/********************* StackSegment **********************/
AsmInst StackSegment::pushD(const AsmInst& prePush) {
    AsmInst insts;
    std::copy(prePush.begin(), prePush.end(), std::back_inserter(insts));
    std::copy(instPushD.begin(), instPushD.end(), std::back_inserter(insts));
    return insts;
}

AsmInst StackSegment::popD(const AsmInst &postPop) {
    AsmInst insts;
    std::copy(instPopD.begin(), instPopD.end(), std::back_inserter(insts));
    std::copy(postPop.begin(), postPop.end(), std::back_inserter(insts));
    return insts;
}

AsmInst StackSegment::pushNamedSegment(const std::string& name, uint16_t idx) {
    AsmInst insts { "@" + std::to_string(static_cast<int>(idx)), "D=A", "@" + name, "A=D+M", "D=M" };
    return pushD(insts);
}

AsmInst StackSegment::popNamedSegment(const std::string& name, uint16_t idx) {
    AsmInst prePop {"@" + std::to_string(static_cast<int>(idx)), "D=A", "@" + name, "M=D+M"};
    AsmInst postPop {"@" + name, "A=M", "M=D", "@" + std::to_string(static_cast<int>(idx)),
                     "D=A", "@" + name, "M=M-D"};

    postPop = popD(postPop);
    std::copy(postPop.begin(), postPop.end(), std::back_inserter(prePop));
    return prePop;
}
/*********************************************************/

/*********************** StaticSegment *******************/
void StaticSegment::setFileName(const std::string& s) {
    fileName = s;
}

AsmInst StaticSegment::push(uint16_t idx) {
    return pushD({"@" + fileName + "." + std::to_string(static_cast<int>(idx)), "D=M" });
}

AsmInst StaticSegment::pop(uint16_t idx) {
    return popD({"@" + fileName + "." + std::to_string(static_cast<int>(idx)), "M=D" });
}
/*********************************************************/

/************************* ConstantSegment ***************/
AsmInst ConstantSegment::push(uint16_t value) {
    return pushD({"@" + std::to_string(static_cast<int>(value)), "D=A" });
}

AsmInst ConstantSegment::pop(uint16_t idx) { (void)idx; return {}; }
/*********************************************************/

/************************* LocalSegment ******************/
AsmInst LocalSegment::push(uint16_t idx) { return pushNamedSegment("LCL", idx);}
AsmInst LocalSegment::pop(uint16_t idx) { return popNamedSegment("LCL", idx);}
/*********************************************************/

/************************* ArgumentSegment ******************/
AsmInst ArgumentSegment::push(uint16_t idx) { return pushNamedSegment("ARG", idx);}
AsmInst ArgumentSegment::pop(uint16_t idx) { return popNamedSegment("ARG", idx);}
/*********************************************************/

/************************* ThisSegment ******************/
AsmInst ThisSegment::push(uint16_t idx) { return pushNamedSegment("THIS", idx);}
AsmInst ThisSegment::pop(uint16_t idx) { return popNamedSegment("THIS", idx);}
/*********************************************************/

/************************* ThatSegment ******************/
AsmInst ThatSegment::push(uint16_t idx) { return pushNamedSegment("THAT", idx);}
AsmInst ThatSegment::pop(uint16_t idx) { return popNamedSegment("THAT", idx);}
/*********************************************************/

/********************** TempSegment *********************/
AsmInst TempSegment::push(uint16_t idx) {
    return pushD({ "@" + std::to_string(static_cast<int>(BASE_ADDR + idx)), "D=M" });
}

AsmInst TempSegment::pop(uint16_t idx) {
    return popD({ "@" + std::to_string(static_cast<int>(BASE_ADDR + idx)), "M=D" });
}
/********************************************************/

/********************** PointerSegment ******************/
AsmInst PointerSegment::push(uint16_t idx) {
    return pushD({ "@" + std::to_string(static_cast<int>(BASE_ADDR + idx)), "D=M" });
}

AsmInst PointerSegment::pop(uint16_t idx) {
    return popD({ "@" + std::to_string(static_cast<int>(BASE_ADDR + idx)), "M=D" });
}
/********************************************************/
