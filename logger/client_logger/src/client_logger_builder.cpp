#include <filesystem>
#include <utility>
#include <not_implemented.h>
#include "../include/client_logger_builder.h"
#include <not_implemented.h>

using namespace nlohmann;

logger_builder& client_logger_builder::add_file_stream(
    std::string const &stream_file_path,
    logger::severity severity) &
{
    _output_streams[severity].first.push_front(
        client_logger::refcounted_stream(stream_file_path));
    return *this;
}

logger_builder& client_logger_builder::add_console_stream(
    logger::severity severity) &
{
    _output_streams[severity].second = true;
    return *this;
}

logger_builder& client_logger_builder::transform_with_configuration(
    std::string const &configuration_file_path,
    std::string const &configuration_path) &
{
    std::ifstream config_stream(configuration_file_path);

    if (!config_stream.is_open()) {
        throw std::runtime_error("Failed to open config file: " 
            + configuration_file_path);
    }

    // Read config file
    json config_json;
    config_stream >> config_json;

    // Get config for current logger
    auto& config = config_json[configuration_path];

    if (config.contains("format")) {
        set_format(config["format"]);
    }

    for (auto& [key, value] : config.items()) {
        if (key == "format") {
            continue;
        }
        logger::severity severity = logger_builder::string_to_severity(key);
        parse_severity(severity, value);
    }

    return *this;
}

logger_builder& client_logger_builder::clear() &
{
    _output_streams.clear();
    return *this;
}

logger *client_logger_builder::build() const
{
    return new client_logger(_output_streams, _format);
}

logger_builder& client_logger_builder::set_format(const std::string &format) &
{
    _format = format;
    return *this;
}

void client_logger_builder::parse_severity(logger::severity sev, nlohmann::json& j)
{
    if (j.contains("console")) {
        add_console_stream(sev);
    }
    if (j.contains("paths")) {
        for (auto& path : j["paths"]) {
            add_file_stream(path, sev);
        }
    }
}

logger_builder& client_logger_builder::set_destination(const std::string &format) &
{
    throw not_implemented("logger_builder *client_logger_builder::set_destination(const std::string &format)", "invalid call");
}
