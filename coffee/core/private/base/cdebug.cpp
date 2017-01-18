#include <coffee/core/base/debug/debugprinter.h>

namespace Coffee{

uint8 PrintingVerbosityLevel = 0;

/* Move exported functions to common place, not spreading it like this */

using namespace DebugFun;

/* The below functions are exported to SWIG */

void cDebug(cstring f)
{
    DebugPrinter::cDebug("{0}",f);
}
void cWarning(cstring f)
{
    DebugPrinter::cWarning("{0}",f);
}
void cFatal(cstring f)
{
    DebugPrinter::cFatal("{0}",f);
}
void cLog(cstring id, cstring msg)
{
    /* TODO: Pipe this to a proper logger */
    DebugPrinter::cBasicPrint("LOGR_EXTERNAL:{0}: {1}",id,msg);
}

namespace DebugFun{

CString cStringReplace(
        CString const& fmt, size_t const& index,
        bigscalar const& arg)
{
    /* Regexes, man, these fucking regexes */
    Regex::Pattern patt = Regex::Compile(".*?(\\{\\d+:(\\d+)\\}).*");
    auto match = Regex::Match(patt,fmt,true);

    if(match.size()>=3)
    {
        int32 prec = Convert::strtoint(match[2].s_match[0].c_str());

        CString rep;
#ifdef COFFEE_USE_IOSTREAMS
        std::stringstream ss;
        ss << std::fixed << std::setprecision(prec) << arg;
        ss >> rep;
#else
        rep = Convert::scalartostring(arg);
        cstring dot1 = Search::ChrFind(rep.c_str(),'.');
        cstring dot2 = Search::ChrFind(rep.c_str(),',');
        cstring ptr_start = rep.c_str();
        cstring ptr_end = rep.c_str()+rep.size();
        if(dot1)
            rep.resize(dot1-ptr_start+1+prec,'0');
        else if(dot2)
            rep.resize(dot2-ptr_start+1+prec,'0');
#endif

        return extArgReplacePhrase(fmt,match[1].s_match[0],rep);
    }
    else
        return extArgReplace(fmt,index,conversion::to_string(arg));
}

}

}
