/*##############################################################################

    HPCC SYSTEMS software Copyright (C) 2012 HPCC Systems®.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
############################################################################## */

#ifndef NLP_INCL
#define NLP_INCL

#ifdef _WIN32
#define NLP_CALL _cdecl
#else
#define NLP_CALL
#endif

#ifdef NLP_EXPORTS
#define NLP_API DECL_EXPORT
#else
#define NLP_API DECL_IMPORT
#endif

#include "platform.h"
#include "hqlplugins.hpp"

extern "C" {

#ifdef NLP_EXPORTS
NLP_API bool getECLPluginDefinition(ECLPluginDefinitionBlock *pb);
NLP_API void setPluginContext(IPluginContext * _ctx);
#endif

NLP_API void NLP_CALL nlpAnalyzer(unsigned & anaLen, const char * analyzer, unsigned & tgtLen, char * & tgt, unsigned srcLen, const char * src);
}

#endif