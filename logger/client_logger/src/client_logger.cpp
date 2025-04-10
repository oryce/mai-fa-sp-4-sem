#include <string>
#include <sstream>
#include <algorithm>
#include <utility>
#include "../include/client_logger.h"
#include <not_implemented.h>

std::unordered_map<std::string, std::pair<size_t, std::ofstream>> client_logger::refcounted_stream::_global_streams;


logger& client_logger::log(
    const std::string &text,
    logger::severity severity) &
{
    auto streams_iter = _output_streams.find(severity);

    if (streams_iter == _output_streams.end()) {
        return *this;
    }

    std::string formatted_text = make_format(text, severity);

    auto& streams = streams_iter->second;

    // If console stream is enabled, print to console
    if (streams.second) {
        std::cout << formatted_text << std::endl;
    }

    // Print to all file streams
    for (auto& file_stream : streams.first) {
        auto stream_ptr = file_stream._stream.second;
        if (stream_ptr != nullptr) {
            *stream_ptr << formatted_text << std::endl;
        }
    }

    return *this;
}

std::string client_logger::make_format(const std::string &message, severity sev) const
{
    std::ostringstream msg;

    bool in_flag = false;

    for (auto& c : _format) {
        if (c == '%') {
            in_flag = true;
            continue;
        } else if (!in_flag) {
            msg << c;
            continue;
        }

        client_logger::flag flag = client_logger::char_to_flag(c);

        switch (flag) {
            case client_logger::flag::DATE:
                msg << logger::current_date_to_string();
                break;
            case client_logger::flag::TIME:
                msg << logger::current_time_to_string();
                break;
            case client_logger::flag::SEVERITY:
                msg << logger::severity_to_string(sev);
                break;
            case client_logger::flag::MESSAGE:
                msg << message;
                break;
            case client_logger::flag::NO_FLAG:
                msg << c;
                break;
        }

        in_flag = false;
    }

    return msg.str();
}


client_logger::client_logger(
        const std::unordered_map<logger::severity, std::pair<std::forward_list<refcounted_stream>, bool>> &streams,
        std::string format) 
    : _output_streams(streams), _format(format)
{
    for (auto& [severity, streams] : _output_streams) {
        for (auto& stream : streams.first) {
            stream.open();
        }
    }
}

client_logger::flag client_logger::char_to_flag(char c) noexcept
{
    switch (c) {
        case 'd': return flag::DATE;
        case 't': return flag::TIME;
        case 's': return flag::SEVERITY;
        case 'm': return flag::MESSAGE;
        default: return flag::NO_FLAG;
    }
}

client_logger::client_logger(const client_logger &other)
{
    _output_streams = other._output_streams;
    _format = other._format;
}

client_logger &client_logger::operator=(const client_logger &other)
{
    _output_streams = other._output_streams;
    _format = other._format;
    return *this;
}

client_logger::client_logger(client_logger &&other) noexcept
{
    if (this != &other) {
        _output_streams = std::move(other._output_streams);
        _format = std::move(other._format);
    }
}

client_logger &client_logger::operator=(client_logger &&other) noexcept
{
    if (this != &other) {
        _output_streams = std::move(other._output_streams);
        _format = std::move(other._format);
    }
    return *this;
}

client_logger::~client_logger() noexcept = default;

client_logger::refcounted_stream::refcounted_stream(const std::string &path)
{
    auto opened_stream = _global_streams.find(path);

    if (opened_stream == _global_streams.end()) {
        // Register empty stream
        _global_streams[path] = {1, std::ofstream()};
    } else {
        // Increment refcount
        opened_stream->second.first++;
    }

    _stream = {path, nullptr};
}

client_logger::refcounted_stream::refcounted_stream(const client_logger::refcounted_stream &oth)
{
    _stream = oth._stream;
    _global_streams[_stream.first].first++;
}

client_logger::refcounted_stream &
client_logger::refcounted_stream::operator=(const client_logger::refcounted_stream &oth)
{
    _stream = oth._stream;
    _global_streams[_stream.first].first++;

    return *this;
}

client_logger::refcounted_stream::refcounted_stream(client_logger::refcounted_stream &&oth) noexcept
{
    if (this != &oth) {
        std::swap(_stream, oth._stream);
    }
}

client_logger::refcounted_stream &client_logger::refcounted_stream::operator=(client_logger::refcounted_stream &&oth) noexcept
{
    if (this != &oth) {
        std::swap(_stream, oth._stream);
    }
    return *this;
}

void client_logger::refcounted_stream::open()
{
    if (_stream.second != nullptr) {
        return;
    }

    auto& stream = _global_streams[_stream.first].second;

    if (stream.is_open()) {
        _stream.second = &stream;
        return;
    }

    stream.open(_stream.first);

    if (!stream.is_open()) {
        throw std::runtime_error("Failed to open file: " + _stream.first);
    }

    _stream.second = &stream;
}

client_logger::refcounted_stream::~refcounted_stream()
{
    auto& stream = _global_streams[_stream.first];
    if (--stream.first == 0) {
        stream.second.close();
        _global_streams.erase(_stream.first);
    }
}
