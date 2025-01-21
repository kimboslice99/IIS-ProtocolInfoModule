#define WIN32_LEAN_AND_MEAN

#include <sal.h>
#include "pch.h"
#include <httpserv.h>
#include <windows.h>
#include <wchar.h>
#include <http.h>
#include "Algorithms.h"

class ProtocolInfoModule : public CGlobalModule
{
public:
    GLOBAL_NOTIFICATION_STATUS 
        OnGlobalPreBeginRequest(
        IN IPreBeginRequestProvider* pProvider
    )
    {
        IHttpContext* pHttpContext = pProvider->GetHttpContext();
        if (pHttpContext == nullptr)
            return GL_NOTIFICATION_CONTINUE;

        IHttpRequest* pHttpRequest = pHttpContext->GetRequest();
        if (pHttpRequest == nullptr)
            return GL_NOTIFICATION_CONTINUE;

        HTTP_REQUEST* pRawRequest = pHttpRequest->GetRawHttpRequest();
        if (pRawRequest == nullptr)
            return GL_NOTIFICATION_CONTINUE;


        DWORD length;
        PCWSTR value;
        HRESULT hr = pHttpContext->GetServerVariable("HTTPS", &value, &length);
        if (SUCCEEDED(hr)) {
            if (wcscmp(value, L"on") == 0) {
                pHttpContext->SetServerVariable("HTTP_X_FORWARDED_PROTOCOL", L"https");
                pHttpContext->SetServerVariable("HTTP_X_FORWARDED_PROTO", L"https");
            }
            else if (wcscmp(value, L"off") == 0) {
                pHttpContext->SetServerVariable("HTTP_X_FORWARDED_PROTOCOL", L"http");
                pHttpContext->SetServerVariable("HTTP_X_FORWARDED_PROTO", L"http");
            }
        }
        length = NULL;
        value = NULL;
        hr = pHttpContext->GetServerVariable("CRYPT_PROTOCOL", &value, &length);
        if (SUCCEEDED(hr)) {
            if (wcscmp(value, L"4") == 0 || wcscmp(value, L"8") == 0) {
                pHttpContext->SetServerVariable("TLS_VERSION", value);
            }
            else if (wcscmp(value, L"10") == 0 || wcscmp(value, L"20") == 0) {
                pHttpContext->SetServerVariable("TLS_VERSION", L"SSLv3");
            }
            else if (wcscmp(value, L"40") == 0 || wcscmp(value, L"80") == 0) {
                pHttpContext->SetServerVariable("TLS_VERSION", L"TLS1.0");
            }
            else if (wcscmp(value, L"100") == 0 || wcscmp(value, L"200") == 0) {
                pHttpContext->SetServerVariable("TLS_VERSION", L"TLS1.1");
            }
            else if (wcscmp(value, L"400") == 0 || wcscmp(value, L"800") == 0) {
                pHttpContext->SetServerVariable("TLS_VERSION", L"TLS1.2");
            }
            else if (wcscmp(value, L"1000") == 0 || wcscmp(value, L"2000") == 0) {
                pHttpContext->SetServerVariable("TLS_VERSION", L"TLS1.3");
            }
        }

        length = NULL;
        value = NULL;
        hr = pHttpContext->GetServerVariable("CRYPT_CIPHER_ALG_ID", &value, &length);
        if (SUCCEEDED(hr)) {
            pHttpContext->SetServerVariable("TLS_CIPHER_ALG", findValue(value));
        }

        length = NULL;
        value = NULL;
        hr = pHttpContext->GetServerVariable("CRYPT_HASH_ALG_ID", &value, &length);
        if (SUCCEEDED(hr)) {
            pHttpContext->SetServerVariable("TLS_HASH_ALG", findValue(value));
        }

        length = NULL;
        value = NULL;
        hr = pHttpContext->GetServerVariable("CRYPT_KEYEXCHANGE_ALG_ID", &value, &length);
        if (SUCCEEDED(hr)) {
            pHttpContext->SetServerVariable("TLS_KEYEXCHANGE_ALG", findValue(value));
        }


        ULONG flags = pRawRequest->Flags;
        USHORT HttpMajorVersion = pRawRequest->Version.MajorVersion;
        USHORT HttpMinorVersion = pRawRequest->Version.MinorVersion;

        WCHAR szProtocol[32];
        if (flags & HTTP_REQUEST_FLAG_HTTP3)
        {
            swprintf_s(szProtocol, L"HTTP/3");
        }
        else if (flags & HTTP_REQUEST_FLAG_HTTP2)
        {
            swprintf_s(szProtocol, L"HTTP/2");
        }
        else
        {
            swprintf_s(szProtocol, L"HTTP/%u.%u", HttpMajorVersion, HttpMinorVersion);
        }

        // this check is to stop the defaultdocumentmodule and url rewrite from messing with the protocol
        // its spoofable and i hate it but its okay for informational stuff? hm
        length = NULL;
        value = NULL;
        hr = pHttpContext->GetServerVariable("HTTP_X_SERVER_PROTOCOL", &value, &length);
        if (FAILED(hr)) {
#ifdef _DEBUG
            OutputDebugString(L"Request was missing HTTP_X_SERVER_PROTOCOL");
#endif
            pHttpContext->SetServerVariable("HTTP_X_SERVER_PROTOCOL", szProtocol);
        }

#ifdef _DEBUG
        WCHAR debugMessage[64];
        swprintf_s(debugMessage, L"Request was %s", szProtocol);
        OutputDebugString(debugMessage);
#endif

        return GL_NOTIFICATION_CONTINUE;
    }

    VOID Terminate()
    {
        delete this;
    }
};

HRESULT
__stdcall
RegisterModule(
    DWORD dwServerVersion,
    IHttpModuleRegistrationInfo* pModuleInfo,
    IHttpServer* pGlobalInfo
)
{
    UNREFERENCED_PARAMETER(dwServerVersion);
    UNREFERENCED_PARAMETER(pGlobalInfo);

    // Create an instance of the global module class.
    ProtocolInfoModule* pGlobalModule = new ProtocolInfoModule;
    // Test for an error.
    if (NULL == pGlobalModule)
    {
        return HRESULT_FROM_WIN32(ERROR_NOT_ENOUGH_MEMORY);
    }
    // Set the global notifications and exit.
    return pModuleInfo->SetGlobalNotifications(
        pGlobalModule, GL_PRE_BEGIN_REQUEST);
}