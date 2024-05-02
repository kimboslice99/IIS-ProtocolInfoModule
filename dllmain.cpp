#define WIN32_LEAN_AND_MEAN

#include <sal.h>
#include "pch.h"
#include <httpserv.h>
#include <windows.h>
#include <wchar.h>
#include <http.h>

class ProtocolInfoModule : public CGlobalModule
{
public:
    GLOBAL_NOTIFICATION_STATUS 
        OnGlobalPreBeginRequest(
        IN IPreBeginRequestProvider* pProvider
    )
    {
        UNREFERENCED_PARAMETER(pProvider);

        IHttpContext* pHttpContext = pProvider->GetHttpContext();
        if (pHttpContext == nullptr)
        {
            return GL_NOTIFICATION_CONTINUE;
        }

        HTTP_REQUEST* pRawRequest = pHttpContext->GetRequest()->GetRawHttpRequest();
        if (pRawRequest == nullptr)
        {
            return GL_NOTIFICATION_CONTINUE;
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
        DWORD length;
        PCWSTR value;
        // this bit is to stop the defaultdocumentmodule from messing with the protocol
        HRESULT hr = pHttpContext->GetServerVariable("X_SERVER_PROTOCOL", &value, &length);
        if (FAILED(hr)) {
            pHttpContext->SetServerVariable("X_SERVER_PROTOCOL", szProtocol);
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