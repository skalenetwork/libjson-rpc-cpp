#ifndef PYTHON_CLIENT_STUB_GENERATOR_H
#define PYTHON_CLIENT_STUB_GENERATOR_H

#include "../stubgenerator.h"

namespace jsonrpcskale
{
    /**
     * The stub client this class generates requires jsonrpcskale_pyclient
     * to be installed from pypi.
     * https://github.com/tvannoy/jsonrpcskale_pyclient
     */
    class PythonClientStubGenerator : public StubGenerator
    {
        public:


            PythonClientStubGenerator(const std::string& stubname, std::vector<Procedure> &procedures, std::ostream& outputstream);
            PythonClientStubGenerator(const std::string& stubname, std::vector<Procedure> &procedures, const std::string filename);

            virtual void generateStub();

            void generateMethod(Procedure& proc);
            void generateAssignments(Procedure& proc);
            void generateProcCall(Procedure &proc);
            std::string generateParameterDeclarationList(Procedure &proc);
            static std::string class2Filename(const std::string &classname);
            static std::string normalizeString(const std::string &text);
    };
}
#endif // PYTHON_CLIENT_STUB_GENERATOR_H
