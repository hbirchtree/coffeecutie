#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/CompilerInstance.h>
#include <memory>
#include <iostream>

static std::string MakeParent(clang::NamedDecl* token)
{
    using Decl = clang::Decl;

    auto parent = &static_cast<clang::NamedDecl&>(
                *token->getLexicalDeclContext()
                    ->getNonClosureAncestor()
                );

    if(!parent)
    {
        switch(token->getKind())
        {
        case Decl::Field:
        {
            auto& field =
                    static_cast<clang::FieldDecl&>(
                        *token
                        );

            parent =
                    static_cast<clang::NamedDecl*>(
                        field.getParent()
                        );
            break;
        }
        case Decl::CXXConversion:
        case Decl::CXXConstructor:
        case Decl::CXXMethod:
        case Decl::Function:
        {
            auto& func =
                    static_cast<clang::FunctionDecl&>(
                        *token
                        );

            switch(token->getKind())
            {
            case Decl::CXXConversion:
            case Decl::CXXConstructor:
            case Decl::CXXMethod:
                parent = static_cast<clang::CXXRecordDecl*>(
                            func.getParent()
                            );
                break;
            case Decl::Function:
                switch(func.getParent()->getDeclKind())
                {
                case Decl::Namespace:
                    parent = static_cast<clang::NamespaceDecl*>(
                                func.getParent()
                                );
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
            break;
        }
        case Decl::Typedef:
        case Decl::TypeAlias:
        {
            auto& alias =
                    static_cast<clang::TypedefNameDecl&>(
                        *token
                        );

            parent =
                    static_cast<clang::NamedDecl*>(
                        alias.getNonClosureContext()
                        );
            break;
        }
        case Decl::Record:
        case Decl::CXXRecord:
        {
            auto& cls =
                    static_cast<clang::RecordDecl&>(
                        *token
                        );

            switch(cls.getParent()->getDeclKind())
            {
            case Decl::Namespace:
                parent =
                        static_cast<clang::NamespaceDecl*>(
                            cls.getParent()
                            );
                break;
            case Decl::Record:
            case Decl::CXXRecord:
                parent =
                        static_cast<clang::RecordDecl*>(
                            cls.getParent()
                            );
                break;
            default:
                auto d = cls.getParent()->getDeclKindName();
                std::cerr << d << std::endl;
                break;
            }
            break;
        }
        case Decl::ParmVar:
        {
            auto& parm =
                    static_cast<clang::ParmVarDecl&>(
                        *token
                        );

            parent =
                    static_cast<clang::FunctionDecl*>(
                        parm.getParentFunctionOrMethod()
                        );
            break;
        }
        default:
            std::cerr << "Unhandled: " << token->getDeclKindName()
                      <<  std::endl;
            break;
        }
    }

    std::string out = "::";
    if(parent)
        out += parent->getQualifiedNameAsString();
    else
        out += "(none)";

    return out;
}

static std::string MakeDeclaration(
        clang::NamedDecl* token
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
    case Decl::CXXRecord:
    {
        out += MakeParent(token);
        out += " class ";
        out += token->getName();
        break;
    }
    case Decl::CXXConversion:
    {
        out = "Conversion";
        break;
    }
    case Decl::CXXMethod:
    case Decl::CXXConstructor:
    case Decl::Function:
    {
        out += MakeParent(token);
        if(token->isCXXInstanceMember())
            out += ".";
        else
            out += ":: ";
        out += token->getName();
        out += "()";
        break;
    }
    case Decl::ParmVar:
    {
        out += MakeParent(token);
        out += " <- ";
        out += token->getName();
        break;
    }
    case Decl::Field:
    {
        auto& field =
                static_cast<clang::FieldDecl&>(
                    *token
                    );

        out += field.getType().getCanonicalType().getAsString();
        out += " ";
        out += MakeParent(token);
        out += ".";
        out += token->getName();
        break;
    }
    case Decl::TemplateTypeParm:
    case Decl::NonTypeTemplateParm:
    {
        out += MakeParent(token);
        out += " template typename ";
        out += token->getName();
        break;
    }
    case Decl::TypeAliasTemplate:
    {
        auto& typealias =
                static_cast<clang::RedeclarableTemplateDecl&>(
                    *token
                    );

        out += "template typedef ";
        out += token->getName();
        out += " = ";
        out += typealias.getUnderlyingDecl()->getNameAsString();
        break;
    }
    case Decl::Typedef:
    case Decl::TypeAlias:
    {
        auto& typed =
                static_cast<clang::TypedefNameDecl&>(
                    *token
                    );

        out += MakeParent(token);
        out += " typedef ";
        out += token->getName();
        out += " = ";
        out += typed.getTypeSourceInfo()->getType().getAsString();
        break;
    }
    default:
        out = token->getDeclKindName();
        break;
    }

    return out;
}

class SymbolVisitor : public clang::RecursiveASTVisitor<SymbolVisitor>
{
    clang::SourceManager& srcManager;

public:
    SymbolVisitor(clang::SourceManager& srcManager):
        srcManager(srcManager)
    {
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
        case Decl::FunctionTemplate:
        case Decl::ClassTemplate:
        case Decl::TypeAliasTemplate:
        case Decl::Var:
            return true;
        default:
            break;
        }

        if(!namedDecl->hasLinkage()
                && namedDecl->getKind() != Decl::ParmVar)
            return true;

        if(!namedDecl->getIdentifier())
            return true;

        std::cout << MakeDeclaration(namedDecl) << std::endl;

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
        return std::make_unique<SymbolConsumer>(CI.getSourceManager());
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
