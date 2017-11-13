#include <coffee/core/plat/environment/argument_parse.h>
#include <coffee/core/types/cdef/infotypes.h>
#include <coffee/core/CDebug>

#include <coffee/core/CFiles>

namespace Coffee{

void ArgumentParser::addSwitch(
        cstring name,
        cstring longname, cstring shortname, cstring help)
{
    this->switches.push_back({name, longname, shortname, help});
}

void ArgumentParser::addArgument(
        cstring name,
        cstring longname, cstring shortname, cstring help)
{
    this->arguments.push_back({name, longname, shortname, help});
}

void ArgumentParser::addPositionalArgument(
        cstring name, cstring help)
{
    this->posargs.push_back({name, help});
}

ArgumentResult ArgumentParser::parseArguments(AppArg &args)
{
    ArgumentResult result;

    aargswitch const* consumer = nullptr;

    cstring arg_p = nullptr;

    szptr num_positionals = 0;

    for(cstring_w arg : args.m_ptrStorage)
    {
        if(!arg)
            continue;

        CString arg_w = arg;

        if(consumer)
        {
            result.arguments.insert({consumer->name, arg_w});
            consumer = nullptr;
            continue;
        }

        arg_p  = nullptr;

        if(arg_w.substr(0, 2) == "--")
            arg_p = &arg_w[2];
        else if(arg_w.substr(0, 1) == "-")
            arg_p = &arg_w[1];
        else if(num_positionals < posargs.size())
        {
            result.positional.insert({
                                         posargs.at(num_positionals).name,
                                         arg_w
                                     });
            num_positionals++;
            continue;
        }else
        {
            cWarning("Unused arguments found: {0}", arg_w);
            continue;
        }

        for(auto const& sw : switches)
            if((sw.longname && StrCmp(arg_p, sw.longname))
                    || (sw.shortname && StrCmp(arg_p, sw.shortname)))
            {
                result.switches.insert(sw.name);
            }

        for(auto const& arg : arguments)
            if(StrCmp(arg_p, arg.longname) || StrCmp(arg_p, arg.shortname))
            {
                consumer = &arg;
                continue;
            }
    }

    return result;
}

CString ArgumentParser::helpMessage() const
{
    auto out = cStringFormat(
                "{0}",
                Env::ExecutableName()
    );

    CString desc = {};

    if(posargs.size())
        desc += "\nPositional arguments";

    for(auto p : posargs)
    {
        out += cStringFormat(" {0}",  p.name);
        desc += cStringFormat("\n\t{0}\t\t{1}\n",
                              p.name,  p.help);
    }

    if(arguments.size() || switches.size())
        desc += "\nOptional arguments";

    for(auto a : arguments)
    {
        out += " [";
        if(a.longname)
            (out += "--") += a.longname;
        if(a.longname && a.shortname)
            out += " ";
        if(a.shortname)
            (out += "-") +=  a.shortname;

        out += " ";
        out += a.name;
        out += "]";

        desc += cStringFormat("\n\t{0}\t\t{1}\n",
                              a.name,  a.help);
    }

    for(auto s : switches)
    {
        out += " [";
        if(s.longname)
            (out += "--") += s.longname;
        if(s.longname && s.shortname)
            out += " ";
        if(s.shortname)
            (out += "-") +=  s.shortname;

        out += "]";

        if(s.longname && s.shortname)
            desc += cStringFormat("\n  -{0}, --{1}\t\t{2}",
                                  s.shortname, s.longname,
                                  s.help);
        else if(s.longname)
            desc += cStringFormat("\n      --{0}\t\t{1}",
                                  s.longname, s.help);
        else if(s.shortname)
            desc += cStringFormat("\n  -{0},\t\t\t{1}",
                                  s.shortname, s.help);
    }

    return out + desc;
}



}
