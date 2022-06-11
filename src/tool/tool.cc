#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>

#include <iostream>

using namespace llvm;

int main(int argc, char** argv) {
    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();

    auto targetTriple = sys::getDefaultTargetTriple();
    outs() << "Compile for triple \"" << targetTriple << "\"\n";

    std::string error;
    auto target = TargetRegistry::lookupTarget(targetTriple, error);
    if (!target) {
        errs() << error;
        return 1;
    }

    auto cpu = "generic";
    auto features = "";

    TargetOptions opt;
    auto relocModel = Optional<Reloc::Model>();
    auto targetMachine = target->createTargetMachine(targetTriple, cpu, features, opt, relocModel);
    if (!targetMachine) {
        errs() << "Could not create target machine";
        return 1;
    }
    errs() << "Syka";
    return 0;

    //constexpr std::string_view filename = "output.o";
    //std::error_code errorCode;
    //raw_fd_ostream dest(filename, errorCode, sys::fs::OF_None);
    //if (errorCode) {
        //errs() << "Could not open file: " << errorCode.message();
        //return 1;
    //}

    //legacy::PassManager pass;
    //auto FileType = CGFT_ObjectFile;
    //if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
        //errs() << "TargetMachine can't emit a file of this type";
        //return 1;
    //}

    ////

    //outs() << "Hallo\n";
}
