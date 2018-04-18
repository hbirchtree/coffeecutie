#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Sema/Template.h>
#include <memory>
#include <iostream>

#include <coffee/core/plat/memory/stlstring_ops.h>

//#define SHOW_UNHANDLED
//#define SHOW_DEFINITIONS

using Decl = clang::Decl;

static clang::Sema* global_sema = nullptr;

std::string prettify_cxx_name(std::string const& name)
{
    using namespace Coffee::Mem::Search;

    auto out = CStrReplace(name, "::", "_");

    out = CStrReplace(out, "_CResources_", "_");
    out = CStrReplace(out, "_CResources_", "_");

    return out;
}

clang::QualType PurifyType(clang::QualType const& type,
                           clang::ASTContext& context)
{
    clang::QualType canonPointerType = type.getCanonicalType();
    bool added_ptr = false;

    if(canonPointerType.getNonReferenceType() != canonPointerType)
    {
        canonPointerType = context.getPointerType(
                    canonPointerType.getNonReferenceType()
                    );
        added_ptr = true;
    }

    return canonPointerType;
}

template<typename T, typename T2>
T2* Instantiate(
        clang::Sema& sema,
        clang::DeclContext* dContext,
        T* decl,
        std::vector<clang::TemplateArgument> const& args)
{
    if(!dContext)
        return nullptr;

    clang::TemplateArgumentList arg_list {
        clang::TemplateArgumentList::OnStack,
        args.data(), static_cast<uint32_t>(args.size())
    };
    clang::MultiLevelTemplateArgumentList m_arg_list {arg_list};

    clang::TemplateDeclInstantiator instantiator {
        sema, dContext, m_arg_list
    };

    auto instantiated = instantiator.Visit(decl);

    if(auto inst_decl = dynamic_cast<T*>(
                instantiated
                ))
        return inst_decl->getTemplatedDecl();
    return nullptr;
}

clang::QualType AddPtrLevel(clang::QualType const& type,
                            clang::ASTContext& context)
{
    return context.getPointerType(type);
}

template<typename T, typename Src>
T& Cast(Src& src)
{
    return static_cast<T&>(src);
}

template<typename T, typename Src>
T Cast(Src* src)
{
    return static_cast<T>(src);
}

static bool IsTemplateDecl(clang::NamedDecl const& dec)
{
    return Cast<clang::TemplateDecl const*>(&dec);
}

clang::NamedDecl const* ExtractParent(clang::NamedDecl const& dec)
{
    clang::NamedDecl const* parent = Cast<clang::NamedDecl const*>(
                dec.getNonClosureContext()
                );

    auto parentPtr = dec.getDeclContext();

    if(parentPtr)
    {
        switch(parentPtr->getDeclKind())
        {
        case Decl::Namespace:
        {
            parent = static_cast<clang::NamespaceDecl const*>(
                        parentPtr
                        );
            break;
        }
        case Decl::Record:
        case Decl::CXXRecord:
        {
            parent = static_cast<clang::RecordDecl const*>(
                        parentPtr
                        );

            break;
        }
        case Decl::CXXConstructor:
        case Decl::CXXDestructor:
        case Decl::CXXMethod:
        {
            parent = ExtractParent(*Cast<clang::CXXMethodDecl const*>(
                            parentPtr
                        ));
            break;
        }
        case Decl::Function:
        {
            parent = Cast<clang::FunctionDecl const*>(
                        parentPtr
                        );
            break;
        }
        case Decl::Enum:
        {
            parent = static_cast<clang::EnumDecl const*>(
                        parentPtr
                        );
            break;
        }
        default:
#if defined(SHOW_UNHANDLED)
            std::cerr << "Unhandled kind: " << parentPtr->getDeclKindName()
                      << " ("
                      << Cast<clang::NamedDecl*>(dec.getCanonicalDecl())
                         ->getQualifiedNameAsString()
                      << ", "
                      << dec.getDeclKindName()
                      << ")"
                      << std::endl;
#endif
            break;
        }
    }

    return parent;
}

template<typename T>
clang::NamedDecl* ExtractParentC(clang::NamedDecl* src)
{
    return ExtractParent(Cast<T&>(*src));
}

static std::string MakeParent(clang::NamedDecl const* token,
                              clang::ASTContext& context)
{
    (void)context;

    std::string out;
    auto parent = ExtractParent(*token);

    if(parent)
        out = parent->getQualifiedNameAsString();

    return out;
}

static clang::QualType ExtractRecordType(clang::NamedDecl const* token,
                                         clang::ASTContext& context)
{
    auto parent = ExtractParent(*token);

    if(!parent)
        return context.VoidTy;

    switch(parent->getKind())
    {
    case Decl::CXXRecord:
    {
        return Cast<clang::RecordDecl const&>(*parent)
                .getTypeForDecl()->getCanonicalTypeUnqualified();
    }
    default:
        return context.VoidTy;
    }
}

static void GenerateFunction(std::ostream& strm,
                             clang::NamedDecl* token,
                             clang::ASTContext& context)
{
    auto& func = Cast<clang::FunctionDecl>(*token);

    auto func_name = MakeParent(token, context)
            + "::" + token->getName().str();

    func_name = prettify_cxx_name(func_name);

    clang::QualType return_type = context.VoidTy;

    strm << "extern \"C\" "
         << prettify_cxx_name(return_type.getAsString())
         << " " << func_name << "(";

    bool has_parameters = false;

    if(func.isCXXInstanceMember())
    {
        has_parameters = true;
        strm << std::endl << "\t\t"
             << prettify_cxx_name(ExtractRecordType(
                                      ExtractParent(*token),
                                      context).getAsString()
                                  )
             << "* instance"
             << std::endl;
    }

    if(!func.getType().isPODType(context))
    {
        if(!has_parameters)
        {
            has_parameters = true;
            strm << std::endl;
        }
        strm << "\t\t"
             << prettify_cxx_name(
                    AddPtrLevel(PurifyType(func.getReturnType(),
                                           context),
                                context)
                    .getAsString())
             << " rvalue"
             << std::endl;
    }

    for(auto child : func.decls())
    {
        if(child->getKind() != Decl::ParmVar)
            continue;

        if(!has_parameters)
        {
            has_parameters = true;
            strm << std::endl;
        }

        auto& parm = Cast<clang::ParmVarDecl>(*child);

        auto pType = PurifyType(parm.getType(), context);

        strm << "\t\t"
             << pType.getAsString()
             << " "
             << parm.getName().str()
             << std::endl;
    }

    if(has_parameters)
        strm << "\t";

    strm << ");"
         << std::endl << std::endl;
}

static void GenerateStruct(clang::NamedDecl const* token,
                           clang::ASTContext& context,
                           std::ostream& strm)
{
    auto& record = Cast<clang::RecordDecl const&>(*token);

    auto cname = MakeParent(token, context)
            + "::" +  token->getName().str();

    cname = prettify_cxx_name(cname);

    /* Template class declarations must be instantiated */
//    if(IsTemplateDecl(record))
//        return;

    strm << "typedef struct " << cname << std::endl
         << "{" << std::endl;

    for(auto child  : record.decls())
    {
//        std::cerr << "Unhandled decl: " << child->getDeclKindName()
//                  << std::endl;

        if(child->getKind() != Decl::Field)
            continue;

        auto namedChild = Cast<clang::FieldDecl*>(child);

        strm << "\t"
             << PurifyType(namedChild->getType(), context)
                .getAsString()
             << " "
             << namedChild->getName().str()
             << ";"
             << std::endl;
    }

    strm << "} "
         << cname
         << ";"
         << std::endl;

    for(auto child : record.decls())
    {
        auto namedChild = Cast<clang::NamedDecl*>(child);

        if(namedChild->getAccess() != clang::AS_public)
            continue;

        if(!namedChild->getIdentifier())
            continue;

        switch(child->getKind())
        {
        case Decl::CXXConstructor:
        case Decl::CXXDestructor:
        case Decl::CXXMethod:
        case Decl::CXXConversion:
        {
            GenerateFunction(std::cout, namedChild, context);

            break;
        }
        default:
            break;
        }
    }
}

static std::string MakeDeclaration(
        clang::NamedDecl* token,
        clang::ASTContext& context
        )
{
    using Decl = clang::Decl;

    std::string out;

    switch(token->getKind())
    {
    case Decl::Namespace:
    {
        out += "namespace ";
        out += token->getQualifiedNameAsString();
        break;
    }
    case Decl::Record:
    case Decl::CXXRecord:
    {
        GenerateStruct(token, context, std::cout);

        break;
    }
    case Decl::Function:
    {
        GenerateFunction(std::cout, token, context);

        break;
    }
    case Decl::TemplateTypeParm:
    case Decl::NonTypeTemplateParm:
    {
        out += MakeParent(token, context);
        out += " template typename ";
        out += token->getName();
        break;
    }
    case Decl::TypeAliasTemplate:
    {
        auto& typealias =
                static_cast<clang::TypeAliasTemplateDecl&>(
                    *token
                    );

        out += "template typedef ";
        out += token->getName();
        out += " = ";
        out += typealias.getTemplatedDecl()->getName();
        out += " ";

        std::cerr << "template typedef" << typealias.getQualifiedNameAsString() << std::endl;

        break;
    }
    case Decl::Typedef:
    case Decl::TypeAlias:
    {
        auto& typed =
                static_cast<clang::TypedefNameDecl&>(
                    *token
                    );

        auto def = typed.getTypeSourceInfo()->getType().getCanonicalType();

        if(def.getTypePtr()->getAsStructureType())
        {
            clang::RecordDecl const* rec = nullptr;

//            auto i = Instantiate<clang::ClassTemplateDecl, clang::RecordDecl>(
//                        *global_sema,
//                        token->getDeclContext()->getParent(),
//                        Cast<clang::ClassTemplateDecl*>(token), {
//                        });

//            if(i)
//                std::cerr << i->getName().str() << std::endl;

//            GenerateStruct(rec, context, std::cout);
        }

        std::cout << "typedef "
                  << def.getAsString()
                  << " "
                  << token->getName().str()
                  << ";"
                  << std::endl;

        break;
    }
    case Decl::Enum:
    {
        auto& typed = Cast<clang::EnumDecl>(*token);

        auto enum_name = MakeParent(token, context)
                + "::" + token->getName().str();

        enum_name = prettify_cxx_name(enum_name);

        std::cout << "/* Underlying type: "
                  << typed.getIntegerType().getAsString()
                  << " */" << std::endl;

        std::cout << "enum " << enum_name << std::endl
                  << "{" << std::endl;

        auto enum_prefix = prettify_cxx_name(token->getName().str());

        for(auto child : typed.decls())
        {
            auto& constant = Cast<clang::EnumConstantDecl>(*child);
            std::cout << "\t"
                      << enum_prefix
                      << "_"
                      << constant.getName().str()
                      << " = "
                      << std::to_string(constant.getInitVal().getExtValue())
                      << ","
                      << std::endl;
        }

        std::cout << "};" << std::endl << std::endl;

        break;
    }
    default:
        out = MakeParent(token, context)
                + " " + token->getDeclKindName()
                + " " + token->getName().str();
        break;
    }

    return out;
}

class SymbolVisitor : public clang::RecursiveASTVisitor<SymbolVisitor>
{
    clang::SourceManager& srcManager;
    clang::ASTContext* context;

public:
    SymbolVisitor(clang::SourceManager& srcManager):
        srcManager(srcManager),
        context(nullptr)
    {
    }

    void setContext(clang::ASTContext& context)
    {
        this->context = &context;
    }

    bool VisitNamedDecl(clang::NamedDecl* namedDecl)
    {
        using Decl = clang::Decl;

        auto loc = srcManager.getFilename(namedDecl->getLocStart());

        if(loc.str().substr(0, 4) == "/usr")
            return true;

        if(namedDecl->getVisibility() == clang::HiddenVisibility
                || namedDecl->isInStdNamespace()
                || namedDecl->isInAnonymousNamespace()
                || (namedDecl->getAccess() != clang::AS_public
                    && namedDecl->getAccess() != clang::AS_none)
                )
            return true;

        switch(namedDecl->getKind())
        {
        case Decl::EnumConstant:
        case Decl::CXXMethod:
        case Decl::Field:
        case Decl::ParmVar:
        case Decl::Var:
            return true;
        default:
            break;
        }

        /* If there is no identifier, skip */
        if(!namedDecl->getIdentifier())
            return true;

        /* Skip elements without linkage, except parameters,
         *  they never have linkage */
        if(!namedDecl->hasLinkage())
            switch(namedDecl->getKind())
            {
            case Decl::Typedef:
            case Decl::TypeAlias:
            case Decl::TypeAliasTemplate:
            case Decl::ParmVar:
                break;
            default:
#if defined(SHOW_UNHANDLED)
                std::cerr << "Skipping " << namedDecl->getName().str()
                          << std::endl;
#endif
                return true;
            }

        std::string declaration = MakeDeclaration(namedDecl,
                                                  *context);
#if defined(SHOW_DEFINITIONS)
        std::cout << declaration << std::endl;
#endif

        return true;
    }
};

class SymbolConsumer : public clang::ASTConsumer
{
    SymbolVisitor visitor;

public:
    SymbolConsumer(clang::SourceManager& manager):
        visitor(manager)
    {
    }

    virtual void HandleTranslationUnit(clang::ASTContext &Ctx)
    {
        auto decs = Ctx.getTranslationUnitDecl()->decls();

        visitor.setContext(Ctx);

        for(auto& decl : decs)
            visitor.TraverseDecl(decl);
    }
};

class SymbolFinder : public clang::ASTFrontendAction
{
protected:
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
            clang::CompilerInstance &CI,
            llvm::StringRef /*InFile*/
            )
    {

        std::unique_ptr<clang::ASTConsumer> consumer =
                std::make_unique<SymbolConsumer>(
                    CI.getSourceManager()
                    );

        CI.setSema(new clang::Sema(
                       CI.getPreprocessor(), CI.getASTContext(),
                       *consumer
                       ));

        global_sema = &CI.getSema();

        return consumer;
    }
};

std::array<const char*, 1> default_arguments = {
    {
        "-D__CODEGEN__"
}};

int main()
{
    std::string error_msg;

    /* Load build configuration from CMake */
    auto db = clang::tooling::CompilationDatabase::loadFromDirectory(
                "/home/havard/Code/coffeecutie/build_", error_msg
                );

    if(!db)
        std::cerr << "Clang failed to load: "
                  << error_msg
                  << std::endl;

    clang::tooling::ClangTool tool(
                *db, db->getAllFiles()
                );

    /* Add some directories to include path */
    tool.appendArgumentsAdjuster(
                [](
                clang::tooling::CommandLineArguments const& args,
                clang::StringRef /*filename*/
                )
    {
        clang::tooling::CommandLineArguments newArgs;
        newArgs.reserve(args.size() + 1 + default_arguments.size());
        std::copy(args.begin(), args.end(),
                  std::back_inserter(newArgs));

        newArgs.push_back("-I/usr/lib/llvm-3.8/lib/clang/3.8.0/include");
        newArgs.insert(newArgs.end(),
                       default_arguments.begin(),
                       default_arguments.end()
                       );

        return newArgs;
    });

    /* Create factory for SymbolVisitor */
    SymbolFinder action;

    return
            tool.run(
                clang::tooling::newFrontendActionFactory<SymbolFinder>()
                .get()
                );
}
