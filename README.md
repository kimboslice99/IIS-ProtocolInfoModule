# ProtocolInfoModule

## Overview

The ProtocolInfoModule was created to address a limitation in IIS where the `SERVER_PROTOCOL` and `HTTP_VERSION` server variables do not accurately reflect whether a request is using HTTP/2 or HTTP/3.

I suspect someone at MS forgot about this (somehow) when completing the work for HTTP/2.

## Usage

To use the ProtocolInfoModule, download the release, drop it in your `%windir%\System32\inetsrv` folder, and install using IIS dialogs.

If using kernel caching you may find `HTTP_X_SERVER_PROTOCOL` is only correct for the first few requests, there is no way around this short of disabling kernel caching, sadly... MS will just have to fix this eventually (I hope)