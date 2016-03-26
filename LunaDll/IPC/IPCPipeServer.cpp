#include <stdio.h>
#include <io.h>
#include "../Defines.h"
#include "../Globals.h"
#include "IPCPipeServer.h"
#include "../libs/json/json.hpp"

#include <thread>
#include <iostream>
#include <sstream>
#include <exception>

using json = nlohmann::json;

// Example test method
static json echoMethod(const json& params)
{
    return params;
}

IPCPipeServer::IPCPipeServer() :
    mOutFD(-1), mInFD(-1)
{
    RegisterMethod("echo", echoMethod);
}

IPCPipeServer::~IPCPipeServer()
{
    if (mOutFD != -1) close(mOutFD);
    if (mInFD != -1) close(mInFD);
}

void IPCPipeServer::AttachStdinStdout() {
    HANDLE stdOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (stdOutHandle != INVALID_HANDLE_VALUE && stdOutHandle != 0)
    {
        int fileDescriptor = _open_osfhandle((intptr_t)stdOutHandle, 0);
        if (fileDescriptor != -1)
        {
            SetStdHandle(STD_OUTPUT_HANDLE, 0);
            mOutFD = fileDescriptor;
        }
    }

    HANDLE stdInHandle = GetStdHandle(STD_INPUT_HANDLE);
    if (stdInHandle != INVALID_HANDLE_VALUE && stdInHandle != 0)
    {
        int fileDescriptor = _open_osfhandle((intptr_t)stdInHandle, 0);
        if (fileDescriptor != -1)
        {
            SetStdHandle(STD_INPUT_HANDLE, 0);
            mInFD = fileDescriptor;
        }
    }

    // If we have both streams, start the read thread
    if ((mOutFD != -1) && (mOutFD != -1))
    {
        std::thread* readThread = new std::thread([this]() {
            this->ReadThread();
        });
    }
}

void IPCPipeServer::RegisterMethod(std::string str, IPCCallback cb)
{
    mCallbacks[str] = cb;
}

void IPCPipeServer::ReadThread()
{
    while (1)
    {
        // Read a string packet
        std::string pktStr = ReadMsgString();
        if (pktStr.size() == 0) break;
        
        // Try to parse as JSON
        json pkt;
        try
        {
            pkt = json::parse(pktStr);
        }
        catch (std::exception)
        {
            SendJsonError(-32700, "Parse error");
            continue;
        }

        // Validate some of the structure of the jsonrpc object
        if ((pkt.find("jsonrpc") == pkt.end()) || (pkt["jsonrpc"] != "2.0") ||
            (pkt.find("method") == pkt.end()) || (!pkt["method"].is_string()) ||
            (pkt.find("id") == pkt.end()) || !(pkt["id"].is_null() || pkt["id"].is_string() || pkt["id"].is_number()) ||
            ((pkt.find("params") == pkt.end()) && !pkt["params"].is_structured())
            )
        {
            SendJsonError(-32600, "Invalid Request");
            continue;
        }
        std::string pktMethod = pkt["method"];
        json pktId = pkt["id"];
        json pktParams = pkt["params"];

        // See if we have a callback for the method...
        const auto it = mCallbacks.find(pktMethod);
        if (it != mCallbacks.cend()) {
            IPCCallback cb = (*it).second;

            // If any exceptions occur, report as internal error
            try {
                json resultData = cb(pktParams);

                // Send reply if id is not null
                if (!pktId.is_null())
                {
                    json replyPkt = {
                        { "jsonrpc", "2.0" },
                        { "result", resultData },
                        { "id", pktId }
                    };
                    SendMsgString(replyPkt.dump());
                }
            }
            catch (std::exception)
            {
                if (!pktId.is_null())
                {
                    SendJsonError(-32603, "Internal error");
                }
            }
            continue;
        }

        SendJsonError(-32601, "Method not found");
    }
}

// Sends a string message to the pipe, in "netstring" encoding as defined http://cr.yp.to/proto/netstrings.txt
void IPCPipeServer::SendMsgString(const std::string& pkt)
{
    // Note: This is not written to be particularly efficient right now. Just
    //       readable enough and safe.

    std::ostringstream pktStringStream;
    pktStringStream << pkt.length() << ":" << pkt << ",";

    std::string pktString = pktStringStream.str();
    write(mOutFD, pktString.c_str(), pktString.length());
}

// Recieves a string message from the pipe, in "netstring" encoding as defined http://cr.yp.to/proto/netstrings.txt
// Returns empty string if the pipe breaks, and is blocking while waiting to receive another whole message.
std::string IPCPipeServer::ReadMsgString()
{
    // Note: This is not written to be particularly efficient right now. Just
    //       readable enough and safe.
    char c;
    std::vector<char> data;
    while (1)
    {
        data.clear();

        // Read until : delimiter
        bool err = false;
        while (1)
        {
            int bytesRead = read(mInFD, &c, 1);
            if (bytesRead != 1)
            {
                return "";
            }
            if (c == ':') break;
            if ((c > '9') || (c < '0'))
            {
                err = true;
                break;
            }
            data.push_back(c);
        }
        if (err) continue;
        std::string byteCountStr = std::string(&data[0], data.size());
        data.clear();

        // Interpret as number
        int byteCount = std::stoi(byteCountStr);
        if (byteCount <= 0) continue;

        int byteCursor = 0;
        data.resize(byteCount);
        while (byteCursor < byteCount)
        {
            int bytesRead = read(mInFD, &data[byteCursor], byteCount - byteCursor);
            if (bytesRead < 0)
            {
                return "";
            }
            byteCursor += bytesRead;
        }

        // Get following comma
        {
            int bytesRead = read(mInFD, &c, 1);
            if (bytesRead != 1) {
                return "";
            }
            if (c != ',')
            {
                continue;
            }
        }

        return std::string(&data[0], data.size());
    }
}

void IPCPipeServer::SendJsonError(int errCode, const std::string& errStr)
{
    json err = {
        { "jsonrpc", "2.0" },
        { "error",{ { "code", errCode },{ "message", errStr } } },
        { "id", nullptr }
    };
    SendMsgString(err.dump());
}
