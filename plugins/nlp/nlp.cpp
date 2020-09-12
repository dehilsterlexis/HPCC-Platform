/*##############################################################################
    HPCC SYSTEMS software Copyright (C) 2019 HPCC Systems®.
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

#include "platform.h"
#include "eclrtl.hpp"
#include "nlp.hpp"

#define NLP_VERSION "nlp plugin 1.0.0"

ECL_NLP_API bool getECLPluginDefinition(ECLPluginDefinitionBlock *pb)
{
    /*  Warning:    This function may be called without the plugin being loaded fully.
     *              It should not make any library calls or assume that dependent modules
     *              have been loaded or that it has been initialised.
     *
     *              Specifically:  "The system does not call DllMain for process and thread
     *              initialization and termination.  Also, the system does not load
     *              additional executable modules that are referenced by the specified module."
     */

    if (pb->size != sizeof(ECLPluginDefinitionBlock))
        return false;

    pb->magicVersion = PLUGIN_VERSION;
    pb->version = NLP_VERSION;
    pb->moduleName = "lib_nlp";
    pb->ECL = NULL;
    pb->flags = PLUGIN_IMPLICIT_MODULE;
    pb->description = "ECL plugin library for nlp++";
    return true;
}

#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "machine.h"

#include "vtapi.h"
#include "u_out.h"
#include "Uanalyze.h"

namespace nlp
{
    //--------------------------------------------------------------------------------
    //                           NLP++ MAIN FUNCTION
    //--------------------------------------------------------------------------------
    
    #define MAX_STR 10000

    void nlpAnalyze(const char * analyzer, unsigned & tgtLen, char * & tgt, unsigned srcLen, const char * src) {

        _TCHAR *input=0, *output=0;
        bool develop = false;      // Development mode.
        _TCHAR *sequence=0;
        _TCHAR appdir[MAXSTR];     // Directory for the application.
        appdir[0] = '\0';
        bool compiled=false;       // Run compiled/interp analyzer.
        bool silent=false;		   // No log/debug output files.

        /////////////////////////////////////////////////
        // GET APP INFORMATION
        /////////////////////////////////////////////////

        _TCHAR *path = _T("./analyzers");
        _stprintf(appdir, _T("%s%c%s"), path,DIR_CH,analyzer);

        _TCHAR *vtpath = _T("./");                                     // 08/26/20 Dd.
        _TCHAR rfbdir[MAXSTR];	   // Directory for VisualText data.   // 08/28/02 AM.
        rfbdir[0] = '\0';                                              // 08/28/02 AM.
        _stprintf(rfbdir, _T("%s%cdata%crfb%cspec"),                   // 08/28/02 AM.
                                vtpath,DIR_CH,DIR_CH,DIR_CH);          // 08/28/02 AM.

        // A general temporary directory.                              // 08/28/02 AM.
        _TCHAR *tmpdir = _T("./tmp");                                  // 08/26/20 Dd.
        _TCHAR logfile[MAXSTR];		 // General VT log file.           // 08/28/02 AM.
        logfile[0] = '\0';                                             // 08/28/02 AM.
        _stprintf(logfile, _T("%s%cvisualtext.log"),tmpdir,DIR_CH);    // 08/28/02 AM.

        // Set up subdirectories with default names.
        // Could read a configuration file here.
        // Could set up to label output files based on input file name
        // or some user-supplied prefix.  Or create unique file names.
        _TCHAR specdir[MAXSTR];
        _TCHAR infile[MAXSTR];
        _TCHAR outfile[MAXSTR];
        _TCHAR seqfile[MAXSTR];

        _stprintf(specdir, _T("%s%sspec"), appdir, DIR_STR);

        if (!input) input = _T("input.txt");         // Default.
        if (!output) output = _T("output.txt");       // Default.

        // Get analyzer-sequence file name.
        if (!sequence) sequence = _T("analyzer.seq");   // Default.

        _stprintf(infile, _T("%s"), input);
        _stprintf(outfile, _T("%s"), output);
        _stprintf(seqfile, _T("%s%s%s"), specdir,DIR_STR,sequence);

        /////////////////////////////////////////////////
        // INITIALIZE VISUALTEXT RUNTIME SYSTEM                        // 08/27/02 AM.
        /////////////////////////////////////////////////
        VTRun *vtrun = VTRun::makeVTRun(                               // 07/21/03 AM.
            logfile,                // Verbose/error log file.         // 08/28/02 AM.
            rfbdir,                 // VisualText dir for RFB spec.    // 08/28/02 AM.
            true                    // Build silently.                 // 08/28/02 AM.
            );

        /////////////////////////////////////////////////
        // INITIALIZE ANALYZER RUNTIME ENGINE
        /////////////////////////////////////////////////
        // Create and initialize an NLP object to manage text analysis.
        // NOTE: This init will dynamically load the user extensions dll at
        // appdir\user\debug\user.dll
        NLP *nlp = vtrun->makeNLP(appdir,analyzer,develop,silent,compiled);  // 07/21/03 AM.

        /////////////////////////////////////////////////
        // SET UP THE KNOWLEDGE BASE
        /////////////////////////////////////////////////

        CG *cg = vtrun->makeCG(                                        // 07/21/03 AM.
                appdir,
                true,      // LOAD COMPILED KB IF POSSIBLE.
                nlp);      // Associated analyzer object.              // 07/21/03 AM.

        if (!cg)                                                       // 07/21/03 AM.
        {
            _t_cerr << _T("[Couldn't make knowledge base.]") << endl;  // 07/21/03 AM.
            vtrun->deleteNLP(nlp);                                     // 07/21/03 AM.
            VTRun::deleteVTRun(vtrun);                                 // 07/21/03 AM.
            return -1;
        }

        _t_cerr << _T("[Loaded knowledge base.]") << endl;             // 02/19/19 AM.
        // Root of the KB hierarchy.
        CONCEPT *root = VTRun::getKBroot(cg);                          // 12/19/03 AM.

        /////////////////////////////////////////////////
        // BUILD ANALYZER APPLICATION
        /////////////////////////////////////////////////
        // Create an analyzer dynamically using the sequence file and rules files
        // under appdir\spec.

        if (!nlp->make_analyzer(seqfile, appdir, develop,
            silent,              // Debug/log file output.             // 06/16/02 AM.
            0,
            false,               // false == Don't compile during load.
            compiled))           // Compiled/interp analyzer.
        {
            _t_cerr << _T("[Couldn't build analyzer.]") << endl;
            vtrun->deleteNLP(nlp);                                     // 07/21/03 AM.
            VTRun::deleteVTRun(vtrun);                                 // 07/21/03 AM.
            return -1;
        }

        // Analyzer can output to a stream.
        _TCHAR ofstr[MAXSTR];
        #ifdef LINUX
        _stprintf(ofstr,_T("./dummy.txt"));
        #else
        _stprintf(ofstr,_T("e:\\dummy.txt"));
        #endif
        _t_ofstream os(TCHAR2CA(ofstr), ios::out);                     // 08/07/02 AM.

        // Testing output to buffer.
        _TCHAR obuf[MAX_STR];						                   // 05/11/02 AM.

        /////////////////////////////////////////////////
        // ANALYZE INPUT TEXT FILES
        /////////////////////////////////////////////////

        // If input is from a buffer, specify its name and length.
        nlp->analyze(infile, outfile, appdir, develop,
            silent,        // Debug/log output files.                  // 06/16/02 AM.
            0,             // Outdir.
            src,           // Input buffer.
            srcLen,        // Length of input buffer, or 0.
            compiled,      // If running compiled analyzer.
                &os,	   // Rebind cout output stream in analyzer    // 08/07/02 AM.
                tgt,       // 05/11/02 AM.
                tgtLen	   // 05/11/02 AM.
            );

        /////////////////////////////////////////////////
        // CLEANUP VISUALTEXT RUNTIME
        /////////////////////////////////////////////////

        // This will close the user.dll for the application also.
        vtrun->deleteNLP(nlp);                                         // 07/21/03 AM.
        VTRun::deleteVTRun(vtrun);                                     // 07/21/03 AM.
        object_counts();    // Report memory leaks to standard output.

        return 0;
    }

    //--------------------------------------------------------------------------------
    //                           NLP ENTRYPOINTS
    //--------------------------------------------------------------------------------

    NLP_API void NLP_CALL runAnalyzer( unsigned & anaLen, const char * analyzer, unsigned & tgtLen, char * & tgt, unsigned srcLen, const char * src)
    {
        nlpAnalyze(analyzer,tgtLen,tgt,srcLen,src);
    }


} // namespace nlp