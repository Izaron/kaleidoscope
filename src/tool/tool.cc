#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>

#include <fstream>

#include "codegen.h"
#include "lexer.h"
#include "parser.h"

using namespace llvm;

namespace {

std::string Print(auto* element) {
    std::string str;
    llvm::raw_string_ostream rso{str};
    element->print(rso);
    return str;
}

std::string CalculateOutputFile(std::string_view sourceFile) {
    constexpr std::string_view sourceExt = ".ka";
    constexpr std::string_view outputExt = ".o";

    std::string outputFile{sourceFile};
    std::size_t pos = outputFile.find(sourceExt);
    outputFile.replace(pos, sourceExt.size(), outputExt);
    return outputFile;
}

} // namespace

int main(int argc, char** argv) {
    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();

    auto targetTriple = sys::getDefaultTargetTriple();
    errs() << "Compile for triple \"" << targetTriple << "\"\n";

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

    // parse code from file
    if (argc < 2) {
        errs() << "Please write the name of the source file";
        return 1;
    }

    const std::string sourceFile{argv[1]};
    std::ifstream istr{sourceFile};
    std::stringstream buffer;
    buffer << istr.rdbuf();
    const std::string sourceStr = buffer.str();

    NKaleidoscope::TCodegenVisitor codegen{NKaleidoscope::EOptimizationLevel::High};
    auto source = NKaleidoscope::TSource::FromString(sourceStr);
    auto tokens = NKaleidoscope::LexTokens(source);
    auto parser = NKaleidoscope::TParser{std::move(tokens)};
    for (auto&& astNode : parser.ParseChunk()) {
        astNode->Accept(codegen);
    }

    errs() << Print(codegen.GetFunction());

    const std::string filename = CalculateOutputFile(sourceFile);
    std::error_code errorCode;
    raw_fd_ostream dest(filename, errorCode, sys::fs::OF_None);
    if (errorCode) {
        errs() << "Could not open file: " << errorCode.message();
        return 1;
    }

    legacy::PassManager pass;
    auto FileType = CGFT_ObjectFile;
    if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
        errs() << "TargetMachine can't emit a file of this type";
        return 1;
    }

    pass.run(codegen.GetModule());
    dest.flush();
}
