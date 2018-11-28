#include <platforms/argument_parse.h>

#include <algorithm>
#include <peripherals/libc/string_ops.h>
#include <platforms/environment.h>

namespace platform {
namespace args {

void ArgumentParser::addSwitch(
    cstring name, cstring longname, cstring shortname, cstring help)
{
    this->switches.push_back({name, longname, shortname, help});
}

void ArgumentParser::addArgument(
    cstring name, cstring longname, cstring shortname, cstring help)
{
    this->arguments.push_back({name, longname, shortname, help});
}

void ArgumentParser::addPositionalArgument(cstring name, cstring help)
{
    this->posargs.push_back({name, help});
}

u32 short_arg_match(cstring specifiedArgs, cstring target)
{
    u32 result = 0;
    for(auto i : Range<>(libc::str::len(specifiedArgs)))
        if(specifiedArgs[i] == target[0])
            result++;

    return result;
}

ArgumentResult ArgumentParser::parseArguments(AppArg& args)
{
    enum arg_class_t
    {
        arg_long  = 1,
        arg_short = 2,
        arg_posit = 3,
    };

    ArgumentResult result;

    aargswitch const* consumer = nullptr;

    cstring arg_p = nullptr;

    szptr num_positionals = 0;
    szptr arg_idx         = 0;

    Vector<bool> consumed;
    consumed.resize(args.m_ptrStorage.size());

    for(cstring_w arg : args.m_ptrStorage)
    {
        arg_idx++;

        if(!arg)
            continue;

        auto    is_consumed = consumed.at(arg_idx - 1);
        CString arg_w       = arg;

        if(consumer)
        {
            result.arguments.insert({consumer->name, arg_w});
            consumer = nullptr;
            is_consumed.flip();
            continue;
        }

        arg_class_t arg_t = arg_posit;
        arg_p             = nullptr;

        if(arg_w.substr(0, 2) == "--")
        {
            arg_t = arg_long;
            arg_p = &arg_w[2];
        } else if(arg_w.substr(0, 1) == "-")
        {
            arg_t = arg_short;
            arg_p = &arg_w[1];
        } else if(num_positionals < posargs.size())
        {
            arg_t = arg_posit;

            result.positional.insert({posargs.at(num_positionals).name, arg_w});
            is_consumed.flip();
            num_positionals++;
            continue;
        } else
            continue;

        u32 short_matches = 0;

        for(auto const& sw : switches)
        {
            if(sw.shortname)
                short_matches = short_arg_match(arg_p, sw.shortname);

            if((sw.longname && arg_t == arg_long &&
                libc::str::cmp(arg_p, sw.longname)) ||
               (sw.shortname && arg_t == arg_short && short_matches > 0))
            {
                result.switches[sw.name] += short_matches;
                is_consumed.flip();
            }
        }

        for(auto const& arg : arguments)
        {
            if(arg.shortname)
                short_matches = short_arg_match(arg_p, arg.shortname);

            if((arg.longname && arg_t == arg_long &&
                libc::str::cmp(arg_p, arg.longname)) ||
               (arg.shortname && arg_t == arg_short && short_matches > 0))
            {
                consumer = &arg;
                is_consumed.flip();
                continue;
            }
        }
    }

    /* After marking the arguments that are consumed,  */

    for(auto i : range_rev<>(consumed.size()))
        if(consumed.at(i))
            args.m_ptrStorage.at(i) = nullptr;

    auto it = args.m_ptrStorage.end();
    ;

    while((it = std::find(
               args.m_ptrStorage.begin(), args.m_ptrStorage.end(), nullptr)) !=
          args.m_ptrStorage.end())
    {
        args.m_ptrStorage.erase(it);
    }

    return result;
}

CString ArgumentParser::helpMessage() const
{
    auto out = Env::ExecutableName();

    CString desc = {};

    if(posargs.size())
        desc += "\nPositional arguments";

    for(auto p : posargs)
    {
        out += " [" + str::encapsulate(p.name) + "]";
        desc += "\n  " + str::encapsulate(p.name)
                + "\t\t" + str::encapsulate(p.help) + "\n";
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
            (out += "-") += a.shortname;

        out += " ";
        out += a.name;
        out += "]";

        if(a.longname && a.shortname)
            desc += "\n  -" + str::encapsulate(a.shortname)
                    + ", --" + str::encapsulate(a.longname)
                    + "\t\t" + str::encapsulate(a.help);
        else if(a.longname)
            desc += "\n      --" + str::encapsulate(a.longname)
                    + "\t\t" + str::encapsulate(a.help);
        else if(a.shortname)
                desc += "\n  -" + str::encapsulate(a.shortname)
                        + "\t\t\t" + str::encapsulate(a.help);
    }

    for(auto s : switches)
    {
        out += " [";
        if(s.longname)
            (out += "--") += s.longname;
        if(s.longname && s.shortname)
            out += " ";
        if(s.shortname)
            (out += "-") += s.shortname;

        out += "]";

        if(s.longname && s.shortname)
            desc += "\n  -" + str::encapsulate(s.shortname)
                    + ", --" + str::encapsulate(s.longname)
                    + "\t\t" + str::encapsulate(s.help);
        else if(s.longname)
            desc += "\n      --" + str::encapsulate(s.longname)
                    + "\t\t" + str::encapsulate(s.help);
        else if(s.shortname)
                desc += "\n  -" + str::encapsulate(s.shortname)
                        + "\t\t\t" + str::encapsulate(s.help);
    }

    return out + desc;
}

} // namespace args
} // namespace platform
