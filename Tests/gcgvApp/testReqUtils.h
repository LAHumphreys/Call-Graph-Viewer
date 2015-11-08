#ifndef __GCGV_TEST_REQ_REPS_UTILS_H__
#define __GCGV_TEST_REQ_REPS_UTILS_H__

#include <SimpleJSON.h>
#include <ReqServer.h>

namespace {
    template <class JSON>
    bool Parse(testLogger& log, JSON& json, std::string rawJson) {
        bool ok = true;

        std::string parseError;
        if (!json.Parse(rawJson.c_str(),parseError)) {
            log << "Failed to parse json: " << rawJson << endl;
            log << "Error was: " << parseError << endl;
            ok = false;
        }

        return ok;
    }
    struct Result {
        bool error;
        std::string message;
    };

    Result MakeRequest(
               testLogger& log,
               RequestServer& server,
               std::string name,
               std::string request)
    {
        std::string msg = name + " " + request;

        Result res { true, "" };

        log << ">> " << msg << endl;
        res.message = server.HandleMessage(msg);
        log << "<< " << res.message << endl;
        if(res.message.substr(0,6) == "ERROR ") {
            res.error = true;
            res.message = res.message.substr(6);
            log << "<< (error) >" << res.message << "<" << endl;
        } else {
            res.error = false;
        }

        return res;
    }

    bool ValidateError(testLogger& log, Result& res, int errCode, std::string msg)
    {
        bool ok = true;

        NewIntField(errorNumber);
        NewStringField(errorText);
        typedef SimpleParsedJSON<errorNumber,errorText> ErrorMsg;

        static ErrorMsg errJSON;
        errJSON.Clear();
        std::string parseError;

        if (!errJSON.Parse(res.message.c_str(),parseError)) {
            ok = false;
            log << "Failed to parse error message: " << endl;
            log << res.message << endl;
            log << "Error was: " << endl;
            log << parseError << endl;
        }

        if (!res.error) {
            ok = false;
            log << "Expected error, but got message: " << endl;
            log << msg << endl;
        }

        if (errJSON.Get<errorNumber>() != errCode) {
            ok = false;
            log << "Expected error code " << errCode
                << " but got " << errJSON.Get<errorNumber>() << endl;
        }

        if (errJSON.Get<errorText>() != msg) {
            ok = false;
            log << "Unexpected error text: " << endl
                << ">> " << msg << endl
                << "<< " << errJSON.Get<errorText>() << endl;
        }
        return ok;
    }
}

#endif
