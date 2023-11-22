#define _WIN32_DCOM

#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0600

#include <iostream>
#include <string>

#include <comdef.h>
#include <objbase.h>
#include <wbemidl.h>

#include <platforms/win32/wmi.h>

using std::cout;
using std::endl;
using std::hex;
using std::wcout;

#pragma comment(lib, "wbemuuid.lib")

struct WMIData
{
    IWbemLocator*  locator;
    IWbemServices* service;
};

static WMIData* m_wmi_data = nullptr;

namespace {

auto string_to_bstr(std::string const& str)
{
    return SysAllocStringByteLen(str.data(), str.size());
}

}

int InitCOMInterface()
{
    if(m_wmi_data)
        return 0;

    HRESULT hres;

    // Step 1: --------------------------------------------------
    // Initialize COM. ------------------------------------------

    hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if(FAILED(hres))
    {
        cout << "Failed to initialize COM library. Error code = 0x" << hex
             << hres << endl;
        return 1; // Program has failed.
    }

    // Step 2: --------------------------------------------------
    // Set general COM security levels --------------------------

    hres = CoInitializeSecurity(
        nullptr,
        -1,                          // COM authentication
        nullptr,                     // Authentication services
        nullptr,                     // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication
        RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation
        nullptr,                     // Authentication info
        EOAC_NONE,                   // Additional capabilities
        nullptr                      // Reserved
    );

    if(FAILED(hres))
    {
        cout << "Failed to initialize security. Error code = 0x" << hex << hres
             << endl;
        CoUninitialize();
        return 1; // Program has failed.
    }

    // Step 3: ---------------------------------------------------
    // Obtain the initial locator to WMI -------------------------

    IWbemLocator* pLoc = nullptr;

    hres = CoCreateInstance(
        CLSID_WbemLocator,
        0,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator,
        (LPVOID*)&pLoc);

    if(FAILED(hres))
    {
        cout << "Failed to create IWbemLocator object."
             << " Err code = 0x" << hex << hres << endl;
        CoUninitialize();
        return 1; // Program has failed.
    }

    // Step 4: -----------------------------------------------------
    // Connect to WMI through the IWbemLocator::ConnectServer method

    IWbemServices* pSvc = nullptr;

    // Connect to the root\cimv2 namespace with
    // the current user and obtain pointer pSvc
    // to make IWbemServices calls.4
    auto wmi_namespace = string_to_bstr("ROOT\\CIMV2");
    hres = pLoc->ConnectServer(
        wmi_namespace, // Object path of WMI namespace
        nullptr,                 // User name. NULL = current user
        nullptr,                 // User password. NULL = current
        0,                       // Locale. NULL indicates current
        0,                       // Security flags.
        0,                       // Authority (for example, Kerberos)
        0,                       // Context object
        &pSvc                    // pointer to IWbemServices proxy
    );
    SysFreeString(wmi_namespace);

    if(FAILED(hres))
    {
        cout << "Could not connect. Error code = 0x" << hex << hres << endl;
        pLoc->Release();
        CoUninitialize();
        return 1; // Program has failed.
    }

    cout << "Connected to ROOT\\CIMV2 WMI namespace" << endl;

    // Step 5: --------------------------------------------------
    // Set security levels on the proxy -------------------------

    hres = CoSetProxyBlanket(
        pSvc,                        // Indicates the proxy to set
        RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
        RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
        nullptr,                        // Server principal name
        RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx
        RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
        nullptr,                        // client identity
        EOAC_NONE                    // proxy capabilities
    );

    if(FAILED(hres))
    {
        cout << "Could not set proxy blanket. Error code = 0x" << hex << hres
             << endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return 1; // Program has failed.
    }

    m_wmi_data = new WMIData;

    m_wmi_data->locator = pLoc;
    m_wmi_data->service = pSvc;

    return 0;
}

bool WMI_Query(const char* query, const wchar_t* property, std::string& target)
{
    if(InitCOMInterface() != 0)
        return false;

    HRESULT hres;
    auto    pSvc = m_wmi_data->service;

    IEnumWbemClassObject* pEnumerator = NULL;
    auto query_language = string_to_bstr("WQL");
    auto query_ = string_to_bstr(query);
    hres                              = pSvc->ExecQuery(
        query_language,
        query_,
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pEnumerator);
    SysFreeString(query_language);
    SysFreeString(query_);

    if(FAILED(hres))
    {
        cout << "Query failed,"
             << " error code = 0x" << hex << hres << endl;
        return 1; // Program has failed.
    }

    // Step 7: -------------------------------------------------
    // Get the data from the query in step 6 -------------------

    IWbemClassObject* pclsObj = NULL;
    ULONG             uReturn = 0;

    while(pEnumerator)
    {
        HRESULT hr = pEnumerator->Next(2, 1, &pclsObj, &uReturn);

        if(0 == uReturn)
        {
            break;
        }

        VARIANT vtProp;

        // Get the value of the Name property
        hr = pclsObj->Get(property, 0, &vtProp, 0, 0);
        //        cout << "Boolean: " << vtProp.boolVal << endl;
        //        wcout << " OS Name : " << vtProp.bstrVal << endl;
        switch(vtProp.vt)
        {
        case VT_BSTR: {
            std::wstring output_w = vtProp.bstrVal;
            target = std::string(output_w.begin(), output_w.end());
            break;
        }
        default:
            break;
        }
        VariantClear(&vtProp);

        pclsObj->Release();
    }
    return 0;
}

std::optional<std::string> platform::info::wmi::detail::query(
    std::string const& query, std::wstring const& property)
{
    if(InitCOMInterface() != 0)
        return std::nullopt;
    std::string out;
    if(!WMI_Query(query.c_str(), property.c_str(), out))
        return std::nullopt;
    return out;
}
