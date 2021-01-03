#define CATCH_CONFIG_MAIN
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include "generator.hpp"
#include "utils.hpp"

using namespace std;

void saveAsm(const AsmInst& insts, const string& filename) {
    ofstream outFile(string("../data/") + filename);

    for(auto& inst: insts) {
        if(inst.front() == '(')
            outFile << inst << endl;
        else
            outFile << "\t" << inst << endl;
    }
}

pair<string, int> test_push(const StackCodeMap& stackMap, const string& segment, uint16_t idx) {
    auto insts = stackMap.at(segment).get()->push(idx);
    string filename(string("../data/push") + segment);
    string asmFile = filename + ".asm";
    saveAsm(insts, asmFile);
    string tstFile = filename + ".tst";
    return execute(string("CPUEmulator ") + tstFile);
}

int main() {
    StackCodeMap stackMap;
    stackMap["argument"] = std::unique_ptr<Segment>(new ArgumentSegment());
    stackMap["constant"] = std::unique_ptr<Segment>(new ConstantSegment());
    stackMap["local"] = std::unique_ptr<Segment>(new LocalSegment());
    stackMap["pointer"] = std::unique_ptr<Segment>(new PointerSegment());
    stackMap["static"] = std::unique_ptr<Segment>(new StaticSegment("test.vm"));
    stackMap["temp"] = std::unique_ptr<Segment>(new TempSegment());
    stackMap["that"] = std::unique_ptr<Segment>(new ThatSegment());
    stackMap["this"] = std::unique_ptr<Segment>(new ThisSegment());

    for(auto& pair: stackMap) {
        auto rc = test_push(stackMap, pair.first, 5);
        cout << "Testing " << pair.first << ": " << rc.second << endl;
    }
    return 0;
}
