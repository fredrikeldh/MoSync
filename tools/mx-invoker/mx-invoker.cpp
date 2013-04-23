#include <stdio.h>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <string.h>

#include <idl-common/idl-common.h>

using namespace std;

static int generateInvoker(const char* input, const char* output);
static void outputInvoker(const char* output, const Interface&);
static void outputNativeHeader(const char* output, const Interface&);

int main(int argc, const char** argv) {
	const char* input = NULL;
	const char* output = NULL;

	// parse arguments
	for(int i=1; i<argc; i++) {
		if(strcmp(argv[i], "-i") == 0) {
			i++;
			if(i >= argc)
				break;
			input = argv[i];
			continue;
		} else if(strcmp(argv[i], "-o") == 0) {
			i++;
			if(i >= argc)
				break;
			output = argv[i];
			continue;
		} else {
			printf("Unknown parameter: '%s'", argv[i]);
			input = output = NULL;
			break;
		}
	}

	if(input && output) {
		return generateInvoker(input, output);
	} else {
		printf("Usage: mx-invoker -i <input.idl> -o <output directory>\n");
		return 1;
	}
}

static int generateInvoker(const char* input, const char* output) {
	vector<Ix> dummyIxs;
	try {
		Interface inf = parseInterface(dummyIxs, input);
		outputInvoker(output, inf);
		outputNativeHeader(output, inf);
	} catch(exception& e) {
		printf("Exception: %s\n", e.what());
		return 1;
	}
	return 0;
}

static void outputInvoker(const char* output, const Interface& inf) {
	ofstream stream((string(output)+"/invoke-extension.cpp").c_str());
	stream <<
		"#include \"extensionCommon.h\"\t//found in /mosync/ext-include/\n"
		"#include \"invoke-extension.h\"\t//found in /mosync/ext-include/\n"
		"#include \"cpp_mx_"<<inf.name<<".h\"\n"
		"\n";
	streamInvokeSyscall(stream, inf, false, 1);
	stream << "\n"
		"static VoidFunction sFunctions[] = {\n";
	for(size_t i=0; i<inf.functions.size(); i++) {
		const Function& f(inf.functions[i]);
		stream << "\tinvoke_" << f.name << ",\n";
	}
	stream << "};\n"
		"\n"
		"int* gRegs;\n"
		"void* gMemDs;\n"
		"\n"
		"extern \"C\"\n"
		"void DLLEXPORT initializeExtension(ExtensionData* ed, const CoreData* cd) {\n"
		"\ted->nFunctions = sizeof(sFunctions) / sizeof(*sFunctions);\n"
		"\ted->functions = sFunctions;\n"
		"\ted->idlHash = IDL_HASH_"<<inf.name<<";\n"
		"\n"
		"\tgRegs = cd->regs;\n"
		"\tgMemDs = cd->memDs;\n"
		"}\n";
	flushStream(stream);
}

void streamInvokePrefix(ostream& stream, const Function& f) {
	stream << "static void invoke_"<<f.name<<"() ";
}

static void outputNativeHeader(const char* output, const Interface& inf) {
	ofstream stream((string(output)+"/cpp_mx_"+inf.name+".h").c_str());
	string headerName = "CPP_MX_" + inf.name;
	stream << "#ifndef "<<headerName<<"\n";
	stream << "#define "<<headerName<<"\n";
	stream << "\n";
	stream << "#include \"extension.h\"\t//found in /mosync/ext-include/\n";
	stream << "\n";
	stream << "#define IDL_HASH_"<<inf.name<<" 0x"<<setfill('0') << setw(8) << hex << calculateChecksum(inf)<<"\n";
	stream << "\n";
	streamCppDefs(stream, inf, MAIN_INTERFACE, headerName);
	streamHeaderFunctions(stream, inf, true);
	stream << "\n";
	stream << "#endif\t//"<<headerName<<"\n";
	flushStream(stream);
}
