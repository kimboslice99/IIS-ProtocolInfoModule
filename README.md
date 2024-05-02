# ProtocolInfoModule

## Overview

The ProtocolInfoModule was created to address a limitation in IIS where the `SERVER_PROTOCOL` and `HTTP_VERSION` server variables do not accurately reflect whether a request is using HTTP/2 or HTTP/3.

## Usage

To use the ProtocolInfoModule, simply deploy it within your IIS environment. It automatically intercepts incoming requests and sets the `X_SERVER_PROTOCOL` server variable to the correct protocol version.
