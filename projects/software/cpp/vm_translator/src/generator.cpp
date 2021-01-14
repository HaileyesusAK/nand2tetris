#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <string>
#include <unordered_map>
#include <vector>
#include "generator.hpp"

/************************** Segment **********************/
AsmInst Segment::push(const AsmInst &prePush) const {
    AsmInst insts;

    std::copy(prePush.begin(), prePush.end(), std::back_inserter(insts));
    std::copy(this->pushD.begin(), this->pushD.end(), std::back_inserter(insts));
    return insts;
}

AsmInst Segment::pop(const AsmInst &postPop) const {
    AsmInst insts;

    std::copy(this->popD.begin(), this->popD.end(), std::back_inserter(insts));
    std::copy(postPop.begin(), postPop.end(), std::back_inserter(insts));
    return insts;
}

AsmInst Segment::pop(uint16_t idx) const { return {}; }
/*********************************************************/


/*********************** StaticSegment *******************/
AsmInst StaticSegment::push(uint16_t idx) const {
    AsmInst insts {
        "@" + this->fileName + "." + std::to_string(idx),
        "D=M"
    };
    return Segment::push(insts);
}

AsmInst StaticSegment::pop(uint16_t idx) const {
    AsmInst insts {
        "@" + this->fileName + "." + std::to_string(idx),
        "M=D"
    };
    return Segment::pop(insts);
}
/*********************************************************/

/************************* ConstantSegment ***************/
AsmInst ConstantSegment:: push(uint16_t value) const {
    AsmInst insts {
        "@" + std::to_string(value),
        "D=A"
    };
    return Segment::push(insts);
}
/*********************************************************/

/************************* NamedSegment ******************/
AsmInst NamedSegment::push(const std::string& name, uint16_t idx) const {
    AsmInst insts {
        "@" + std::to_string(idx),
        "D=A", "@" + name, "A=D+M", "D=M"
    };
    return Segment::push(insts);
}

AsmInst NamedSegment::pop(const std::string& name, uint16_t idx) const {
    AsmInst prePop {"@" + std::to_string(idx), "D=A", "@" + name, "M=D+M"};
    AsmInst postPop {"@" + name, "A=M", "M=D", "@" + std::to_string(idx),
                     "D=A", "@" + name, "M=M-D"};

    postPop = Segment::pop(postPop);
    std::copy(postPop.begin(), postPop.end(), std::back_inserter(prePop));
    return prePop;
}
/*********************************************************/

/************************* LocalSegment ******************/
AsmInst LocalSegment::push(uint16_t idx) const { return NamedSegment::push("LCL", idx);}
AsmInst LocalSegment::pop(uint16_t idx) const { return NamedSegment::pop("LCL", idx);}
/*********************************************************/

/************************* ArgumentSegment ******************/
AsmInst ArgumentSegment::push(uint16_t idx) const { return NamedSegment::push("ARG", idx);}
AsmInst ArgumentSegment::pop(uint16_t idx) const { return NamedSegment::pop("ARG", idx);}
/*********************************************************/

/************************* ThisSegment ******************/
AsmInst ThisSegment::push(uint16_t idx) const { return NamedSegment::push("THIS", idx);}
AsmInst ThisSegment::pop(uint16_t idx) const { return NamedSegment::pop("THIS", idx);}
/*********************************************************/

/************************* ThatSegment ******************/
AsmInst ThatSegment::push(uint16_t idx) const { return NamedSegment::push("THAT", idx);}
AsmInst ThatSegment::pop(uint16_t idx) const { return NamedSegment::pop("THAT", idx);}
/*********************************************************/

/************************* FixedSegment ******************/
AsmInst FixedSegment::push(uint16_t baseAddr, uint16_t idx) const {
    AsmInst insts {
        "@" + std::to_string(baseAddr + idx),
        "D=M"
    };
    return Segment::push(insts);
}

AsmInst FixedSegment::pop(uint16_t baseAddr, uint16_t idx) const {
    AsmInst insts {
        "@" + std::to_string(baseAddr + idx),
        "M=D"
    };
    return Segment::pop(insts);
}
/*********************************************************/

/********************** TempSegment *********************/
AsmInst TempSegment::push(uint16_t idx) const {
    return FixedSegment::push(TempSegment::BASE_ADDR, idx);
} 

AsmInst TempSegment::pop(uint16_t idx) const {
    return FixedSegment::pop(TempSegment::BASE_ADDR, idx);
}
/********************************************************/

/********************** PointerSegment ******************/
AsmInst PointerSegment::push(uint16_t idx) const {
    return FixedSegment::push(PointerSegment::BASE_ADDR, idx);
}

AsmInst PointerSegment::pop(uint16_t idx) const {
    return FixedSegment::pop(PointerSegment::BASE_ADDR, idx);
}
/********************************************************/

/*********************** Generator *********************/
AsmInst Generator::generate() { return {}; }
AsmInst Generator::generate(const std::string& arg) { return {}; }
AsmInst Generator::generate(const std::string& arg, uint16_t idx) { return {}; }
AsmInst Generator::generate(const std::string& arg1, const std::string& arg2) { return {}; }
/********************************************************/

/******************* StackPushGenerator ****************/
AsmInst StackPushGenerator::generate(const std::string& segment, uint16_t idx) const {
        return map.at(segment).get()->push(idx);
}
/********************************************************/

/******************* StackPopGenerator ****************/
AsmInst StackPopGenerator::generate(const std::string& segment, uint16_t idx) const {
        return map.at(segment).get()->pop(idx);
}
/********************************************************/

/******************* FunctionGenerator ****************/
AsmInst FunctionGenerator::generate(const std::string& fName, uint16_t nLocals){
    /*   
        create a label (f_name)
        push constant 0 n_locals times
    */
    std::string loop = fName + std::string("_SET_LCL");
    std::string end = fName + std::string("_SET_LCL_END");
    
    AsmInst funcInsts {
        // Label the function's starting point
        "(" + fName + ")",

        //Setup loop variables
        "@LCL", "D=M", "@i", "M=D", "@" + std::to_string(nLocals), "D=D+A", "@n", "M=D",
        
        //Begin loop
        "(" + loop + ")",

        //Check for loop termination condition
        "@n", "D=M", "@i", "D=M-D", "@" + end, "D;JGE",

        //Push 0 onto the local segment
        "@i", "A=M", "M=0",

        //Increment SP and loop variable, and goto the beginning
        "@i", "M=M+1", "@SP", "M=M+1", "@" + loop, "0;JMP",
        
        //End loop
        "(" + end + ")"
    };

    return funcInsts; 
}
/********************************************************/


/****************** CallGenerator ***********************/
const std::vector<std::string>& CallGenerator::getSegments() {
    static const std::vector<std::string> segments {"LCL", "ARG", "THIS", "THAT"};
    return segments;
}

AsmInst CallGenerator::generate(const std::string& fName, uint16_t nArgs) {
    /*
        push return-address
        push LCL
        push ARG
        push THIS
        push THAT
        ARG = SP - n - 5
        LCL = SP
        goto f_name
        (return_address)
    */

    static uint16_t nCalls = 1;
    std::string retAddr = fName + "$end" + std::to_string(nCalls++);
    AsmInst callInsts {
        //Save return address
        "@" + retAddr, "D=A", "@SP", "A=M", "M=D", "@SP", "M=M+1",
        
    };

    //Save caller's state
    for(auto& segment: CallGenerator::getSegments()) {
        AsmInst insts = {"@" + segment, "D=M", "@SP", "A=M", "M=D", "@SP", "M=M+1"};
        std::copy(insts.begin(), insts.end(), std::back_inserter(callInsts));
    }

    AsmInst insts {
        //Setup ARG for the callee
        "@" + std::to_string(CallGenerator::ARG_START),
        "D=A", "@" + std::to_string(nArgs), "D=D+A", "@SP", "D=M-D",
        "@ARG", "M=D",

        //Setup LCL for the callee
        "@SP", "D=M", "@LCL", "M=D",
    
        //Jump to the called function
        "@" + fName, "0;JMP",

        //Label end of function
        "(" + retAddr + ")"
    };

    std::copy(insts.begin(), insts.end(), std::back_inserter(callInsts));

    return callInsts;
}
/********************************************************/

/**************** RetGenerator **************************/
AsmInst RetGenerator::generate() {
    /*
        frame = LCL
        ret = *(frame - 5)
        *ARG = pop()
        SP = ARG + 1
        THAT = *(frame - 1)
        THIS = *(frame - 2)
        ARG = *(frame - 3)
        LCL = *(frame - 4)

        goto ret
    */
    AsmInst retInsts {
        //Save the current function's LCL in a variable
        "@LCL", "D=M", "@frame", "M=D",

        //Save the return address in a variable
        "@" + std::to_string(CallGenerator::ARG_START),
        "D=A", "@frame", "A=M-D", "D=M", "@ret", "M=D",

        //Return value to the caller
        "@SP", "A=M-1", "D=M", "@ARG", "A=M", "M=D",

        //Update SP
        "@ARG", "D=M+1", "@SP", "M=D"
    };

    //Reset caller's state in reverse order
    auto& segments = CallGenerator::getSegments();
    for(auto it = segments.rbegin(); it != segments.rend(); ++it) {
        AsmInst insts = {"@frame", "AM=M-1", "D=M", "@" + *it, "M=D"};
        std::copy(insts.begin(), insts.end(), std::back_inserter(retInsts));
    }

    //Return control to the caller
    retInsts.push_back("@ret");
    retInsts.push_back("A=M");
    retInsts.push_back("0;JMP");

    return retInsts;
}
/********************************************************/

/********************* LabelGenerator *******************/ 
AsmInst LabelGenerator::generate(const std::string& funcName, const std::string& label) {
    return {"(" + funcName + "$" + label + ")"};
}
/********************************************************/

/********************** GotoGenerator *******************/ 
AsmInst GotoGenerator::generate(const std::string& funcName, const std::string& label) {
    return {"@" + funcName + "$" + label, "0;JMP"};
}
/********************************************************/

/********************* IfGotoGenerator ******************/ 
AsmInst IfGotoGenerator::generate(const std::string& funcName, const std::string& label) {
    return { "@SP", "AM=M-1", "D=M", "@" + funcName + "$" + label, "D;JNE"};
}
/********************************************************/

/***************** ArithmeticGenerator ******************/ 
AsmInst ArithmeticGenerator::generate(const AluOperator& op) {
    AsmInst insts {"@SP", "AM=M-1", "D=M", "@SP", "A=M-1"};
    auto c = this->opMap.at(op);
    if(op == AluOperator::MINUS)
        insts.push_back("M=M-D");
    else
        insts.push_back("M=D" + c + "M");

    return insts;
}

AsmInst AddGenerator::generate() { return ArithmeticGenerator::generate(AluOperator::PLUS); }
AsmInst SubGenerator::generate() { return ArithmeticGenerator::generate(AluOperator::MINUS); }
AsmInst AndGenerator::generate() { return ArithmeticGenerator::generate(AluOperator::AND); }
AsmInst OrGenerator::generate() { return ArithmeticGenerator::generate(AluOperator::OR); }
/********************************************************/

/********************** RelGenerator ******************/
AsmInst RelGenerator::generate(RelOperator op) {
    static uint64_t i = 1;
    std::string jmpInst;
    std::string endLabel("END_REL_OP_" + std::to_string(i++));

    switch(op) {
        case RelOperator::EQ:
            jmpInst = "D;JEQ"; break;

        case RelOperator::LT:
            jmpInst = "D;JLT"; break;

        case RelOperator::GT:
            jmpInst = "D;JGT"; break;
    }

    return {
        "@SP", "AM=M-1", "D=M", "@SP", "A=M-1", "D=M-D", "M=-1",
        "@" + endLabel, jmpInst, "@SP", "A=M-1", "M=0", "(" + endLabel + ")"
    };
}

AsmInst EqGenerator::generate() { return RelGenerator::generate(RelOperator::EQ); }
AsmInst LtGenerator::generate() { return RelGenerator::generate(RelOperator::LT); }
AsmInst GtGenerator::generate() { return RelGenerator::generate(RelOperator::GT); }
/********************************************************/

/*********************** NotGenerator *******************/ 
AsmInst NotGenerator::generate() {
    static AsmInst insts {"@SP", "A=M-1", "M=!M"};
    return insts;
}
/******************************************************/

/*********************** NegGenerator *******************/ 
AsmInst NegGenerator::generate() {
    static AsmInst insts {"@SP", "A=M-1", "M=-M"};
    return insts;
}
/********************************************************/
