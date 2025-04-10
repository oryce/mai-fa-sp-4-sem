#include <not_implemented.h>
#include <httplib.h>
#include "../include/server_logger.h"

#include <server_logger_builder.h>

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

static void check(httplib::Result&& res)
{
    if (!res || res->status != httplib::NoContent_204)
    {
        throw std::runtime_error("request to server failed");
    }
}

server_logger::~server_logger() noexcept
{
    httplib::Params par;
    par.emplace("pid", std::to_string(server_logger::inner_getpid()));
    check(_client.Get("/destroy", par, httplib::Headers()));
}

logger& server_logger::log(const std::string& message,
                           logger::severity severity) &
{
    std::stringstream stringstream;

    stringstream << "[" << current_date_to_string() << " " << current_time_to_string() <<
        "][" << severity_to_string(severity) << "] " << message;

    httplib::Params par;
    par.emplace("pid", std::to_string(server_logger::inner_getpid()));
    par.emplace("sev", severity_to_string(severity));
    par.emplace("message", stringstream.str());

    check(_client.Get("/log", par, httplib::Headers()));
    return *this;
}

server_logger::server_logger(const std::string& dest,
                             const std::unordered_map<logger::severity, std::pair<std::string, bool>>&
                             streams) : _client(dest)
{
    for (const auto& [fst, snd] : streams)
    {
        httplib::Params par;
        par.emplace("pid", std::to_string(server_logger::inner_getpid()));
        par.emplace("sev", severity_to_string(fst));
        par.emplace("path", snd.first);
        par.emplace("console", snd.second ? "1" : "0");

        check(_client.Get("/init", par, httplib::Headers()));
    }
}

int server_logger::inner_getpid()
{
#ifdef _WIN32
     return ::_getpid();
#else
    return getpid();
#endif
}

server_logger::server_logger(const server_logger& other) : _client(
    httplib::Client(other._client.host(), other._client.port()))
{
}

server_logger& server_logger::operator=(const server_logger& other)
{
    if (this != &other)
    {
        httplib::Params par;
        par.emplace("pid", std::to_string(server_logger::inner_getpid()));
        check(_client.Get("/destroy", par, httplib::Headers()));
        _client = httplib::Client(other._client.host(), other._client.port());
    }
    return *this;
}

server_logger::server_logger(server_logger&& other) noexcept : _client(std::move(other._client))
{
}

server_logger& server_logger::operator=(server_logger&& other) noexcept
{
    if (this != &other)
    {
        httplib::Params par;
        par.emplace("pid", std::to_string(server_logger::inner_getpid()));
        check(_client.Get("/destroy", par, httplib::Headers()));
        _client = std::move(other._client);
    }
    return *this;
}
