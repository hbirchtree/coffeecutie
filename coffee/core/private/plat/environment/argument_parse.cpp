#include <coffee/core/plat/environment/argument_parse.h>

namespace Coffee{

bool ArgumentCollection::registerArgument(ArgumentCollection::ArgType l, cstring lname, cstring sname, cstring information)
{
    if(l != Switch && l != Argument)
        return false;

    arg_variant var = {};
    var.type = l;
    var.information = information;

    arguments.push_back(var);
    if(lname)
        long_args[lname] = &arguments.back();
    if(sname)
        short_args[sname] = &arguments.back();

    return true;
}

void ArgumentCollection::get_argument(int argc, cstring *argv, Set<int> &_indices, ArgumentCollection::ArgType type, cstring sw, cstring sw_s, ArgumentCollection::arg_variant &var)
{
    if(type == Switch)
    {
        int idx = -1;
        var.switched = var.switched || ArgParse::Check(argc,argv,sw,(sw_s) ? sw_s[0] : 0,&idx);
        if(idx>=0)
            _indices.insert(idx);
    }else{
        int idx = -1;
        var.data = ArgParse::Get(argc,argv,sw,(sw_s) ? sw_s[0] : 0,&idx);
        if(idx>=0)
        {
            _indices.insert(idx);
            _indices.insert(idx+1);
        }
    }
}

void ArgumentCollection::parseArguments(int argc, cstring_w *_argv)
{
    cstring* argv = (cstring*)_argv;

    argv = &argv[1];
    argc--;

    Set<int> _indices;

    for(Pair<CString,arg_variant*> const& arg : short_args)
        get_argument(argc,argv,_indices,arg.second->type,
                     nullptr,arg.first.data(),*arg.second);

    for(Pair<CString,arg_variant*> const& arg : long_args)
        get_argument(argc,argv,_indices,arg.second->type,
                     arg.first.data(),nullptr,*arg.second);

    for(int i=0;i<argc;i++)
        if(!_indices.count(i))
            positional.push_back(argv[i]);
}

Map<CString, cstring> ArgumentCollection::getArgumentOptions()
{
    Map<CString,cstring> opts;
    for(Pair<CString,arg_variant*> const& opt : long_args)
    {
        if(opt.second->type == Argument)
            opts[opt.first] = opt.second->data;
    }
    return opts;
}

Map<CString, bool> ArgumentCollection::getSwitchOptions()
{
    Map<CString,bool> opts;
    for(Pair<CString,arg_variant*> const& opt : long_args)
    {
        if(opt.second->type == Switch)
            opts[opt.first] = opt.second->switched;
    }
    for(Pair<CString,arg_variant*> const& opt : short_args)
    {
        if(opt.second->type == Switch)
            opts[opt.first] = opt.second->switched;
    }
    return opts;
}

CString ArgumentCollection::helpMessage() const
{
    CString out;
    for(arg_variant const& v : arguments)
    {
        cstring short_arg = nullptr;
        cstring long_arg = nullptr;
        for(Pair<CString,arg_variant*> const& p : short_args)
        {
            if(p.second == &v)
            {
                short_arg = p.first.c_str();
                break;
            }
        }
        for(Pair<CString,arg_variant*> const& p : long_args)
        {
            if(p.second == &v)
            {
                long_arg = p.first.c_str();
                break;
            }
        }

        CString opt;
        if(short_arg)
        {
            opt += "-";
            opt += short_arg;
        }
        if(long_arg)
        {
            if(short_arg)
                opt += ",";
            opt += "--";
            opt += long_arg;
        }
        opt += " : ";
        opt += v.information;

        out += opt;
        out += "\n";
    }
    return out;
}

bool ArgParse::Check(int argc, String *argv, String sw_l, Character sw_s, int *pos)
{
    for(int i=0;i<argc;i++)
        /* For short switches, check that sw_s is not '0' */
        /* For long switches, check that sw_l is not '0x0' */
        if((sw_s != 0 && _cmp_short_switch(argv[i],sw_s))||(sw_l && _cmp_long_switch(argv[i],sw_l)))
        {
            if(pos)
                *pos = i;
            return true;
        }
    return false;
}

cstring ArgParse::Get(int argc, String *argv, String sw, Character sw_s, int *pos)
{
    for(int i=1;i<argc-1;i++)
        if(_cmp_short_switch(argv[i],sw[0])||_cmp_long_switch(argv[i],sw))
        {
            if(pos)
                *pos = i;
            return argv[i+1];
        }
    return nullptr;
}

}
