#define _WIN32_DCOM
#define _WIN32_WINNT 0x0600

#include <iostream>
#include <WbemIdl.h>
#include <comdef.h>
#include <objbase.h>

using std::wcout;
using std::cout;
using std::endl;
using std::hex;

#pragma comment(lib, "wbemuuid.lib")

struct WMIData
{
    IWbemLocator* locator;
    IWbemServices* service;
};

static WMIData* m_wmi_data = nullptr;

int InitCOMInterface()
{
    if(m_wmi_data)
        return 0;

    HRESULT hres;

    // Step 1: --------------------------------------------------
    // Initialize COM. ------------------------------------------

    hres =  CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hres))
    {
        cout << "Failed to initialize COM library. Error code = 0x"
             << hex << hres << endl;
        return 1;                  // Program has failed.
    }

    // Step 2: --------------------------------------------------
    // Set general COM security levels --------------------------

    hres =  CoInitializeSecurity(
                NULL,
                -1,                          // COM authentication
                NULL,                        // Authentication services
                NULL,                        // Reserved
                RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication
                RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation
                NULL,                        // Authentication info
                EOAC_NONE,                   // Additional capabilities
                NULL                         // Reserved
                );


    if (FAILED(hres))
    {
        cout << "Failed to initialize security. Error code = 0x"
             << hex << hres << endl;
        CoUninitialize();
        return 1;                    // Program has failed.
    }

    // Step 3: ---------------------------------------------------
    // Obtain the initial locator to WMI -------------------------

    IWbemLocator *pLoc = NULL;

    hres = CoCreateInstance(
                CLSID_WbemLocator,
                0,
                CLSCTX_INPROC_SERVER,
                IID_IWbemLocator, (LPVOID *) &pLoc);

    if (FAILED(hres))
    {
        cout << "Failed to create IWbemLocator object."
             << " Err code = 0x"
             << hex << hres << endl;
        CoUninitialize();
        return 1;                 // Program has failed.
    }

    // Step 4: -----------------------------------------------------
    // Connect to WMI through the IWbemLocator::ConnectServer method

    IWbemServices *pSvc = NULL;

    // Connect to the root\cimv2 namespace with
    // the current user and obtain pointer pSvc
    // to make IWbemServices calls.4
    hres = pLoc->ConnectServer(
                _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
                NULL,                    // User name. NULL = current user
                NULL,                    // User password. NULL = current
                0,                       // Locale. NULL indicates current
                NULL,                    // Security flags.
                0,                       // Authority (for example, Kerberos)
                0,                       // Context object
                &pSvc                    // pointer to IWbemServices proxy
                );

    if (FAILED(hres))
    {
        cout << "Could not connect. Error code = 0x"
             << hex << hres << endl;
        pLoc->Release();
        CoUninitialize();
        return 1;                // Program has failed.
    }

    cout << "Connected to ROOT\\CIMV2 WMI namespace" << endl;


    // Step 5: --------------------------------------------------
    // Set security levels on the proxy -------------------------

    hres = CoSetProxyBlanket(
                pSvc,                        // Indicates the proxy to set
                RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
                RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
                NULL,                        // Server principal name
                RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx
                RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
                NULL,                        // client identity
                EOAC_NONE                    // proxy capabilities
                );

    if (FAILED(hres))
    {
        cout << "Could not set proxy blanket. Error code = 0x"
             << hex << hres << endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return 1;               // Program has failed.
    }

    m_wmi_data = new WMIData;

    m_wmi_data->locator = pLoc;
    m_wmi_data->service = pSvc;

    return 0;

    // Step 6: --------------------------------------------------
    // Use the IWbemServices pointer to make requests of WMI ----

    // For example, get the name of the operating system
    IEnumWbemClassObject* pEnumerator = NULL;
    hres = pSvc->ExecQuery(
                bstr_t("WQL"),
                bstr_t("SELECT * FROM Win32_OperatingSystem"),
                WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                NULL,
                &pEnumerator);

    if (FAILED(hres))
    {
        cout << "Query for operating system name failed."
             << " Error code = 0x"
             << hex << hres << endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return 1;               // Program has failed.
    }

    // Step 7: -------------------------------------------------
    // Get the data from the query in step 6 -------------------

    IWbemClassObject *pclsObj = NULL;
    ULONG uReturn = 0;

    while (pEnumerator)
    {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
                                       &pclsObj, &uReturn);

        if(0 == uReturn)
        {
            break;
        }

        VARIANT vtProp;

        // Get the value of the Name property
        hr = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
        wcout << " OS Name : " << vtProp.bstrVal << endl;
        VariantClear(&vtProp);

        pclsObj->Release();
    }

    // Cleanup
    // ========

//    pSvc->Release();
//    pLoc->Release();
//    pEnumerator->Release();
//    CoUninitialize();

    return 0;   // Program successfully completed.
}

bool WMI_Query(const char* query, const wchar_t* property)
{
    if(InitCOMInterface() != 0)
        return false;

    HRESULT hres;
    auto pSvc = m_wmi_data->service;

    IEnumWbemClassObject* pEnumerator = NULL;
    hres = pSvc->ExecQuery(
                bstr_t("WQL"),
                bstr_t(query),
                WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                NULL,
                &pEnumerator);

    if (FAILED(hres))
    {
        cout << "Query failed,"
             << " error code = 0x"
             << hex << hres << endl;
        return 1;               // Program has failed.
    }

    // Step 7: -------------------------------------------------
    // Get the data from the query in step 6 -------------------

    IWbemClassObject *pclsObj = NULL;
    ULONG uReturn = 0;

    while (pEnumerator)
    {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
                                       &pclsObj, &uReturn);

        if(0 == uReturn)
        {
            break;
        }

        VARIANT vtProp;

        // Get the value of the Name property
        hr = pclsObj->Get(property, 0, &vtProp, 0, 0);
//        cout << "Boolean: " << vtProp.boolVal << endl;
        wcout << " OS Name : " << vtProp.bstrVal << endl;
        VariantClear(&vtProp);

        pclsObj->Release();
    }

    return true;
}
