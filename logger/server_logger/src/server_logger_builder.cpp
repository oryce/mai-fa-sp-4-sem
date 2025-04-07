#include <not_implemented.h>
#include "../include/server_logger_builder.h"
#include <fstream>

logger_builder &server_logger_builder::add_file_stream(
    std::string const &stream_file_path,
    logger::severity severity) & {
    auto it = _output_streams.find(severity);

    if (it == _output_streams.end()) {
        it = _output_streams.emplace(severity, std::make_pair(std::string(), false)).first;
    }

    it->second.first = stream_file_path;
    return *this;
}

logger_builder &server_logger_builder::add_console_stream(
    logger::severity severity) & {
    auto it = _output_streams.find(severity);

    if (it == _output_streams.end()) {
        it = _output_streams.emplace(severity, std::make_pair(std::string(), false)).first;
    }

    it->second.second = true;
    return *this;
}

logger_builder &server_logger_builder::transform_with_configuration(
    std::string const &configuration_file_path,
    std::string const &configuration_path) & {
    using namespace nlohmann;
    std::ifstream config_stream(configuration_file_path);

    if (!config_stream.is_open()) {
        throw std::runtime_error("Failed to open config file: "
                                 + configuration_file_path);
    }

    // Read config file
    json config_json;
    config_stream >> config_json;

    // Get config for current logger
    auto &config = config_json[configuration_path];

    if (config.contains("format")) {
        set_format(config["format"]);
    }

    for (auto &[key, value]: config.items()) {
        if (key == "format") {
            continue;
        }
        logger::severity severity = logger_builder::string_to_severity(key);
        parse_severity(severity, value);
    }
    return *this;
}

void server_logger_builder::parse_severity(logger::severity sev, nlohmann::json& j)
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

logger_builder &server_logger_builder::clear() & {
    _output_streams.clear();
    _destination = "http://127.0.0.1:9200";
    return *this;
}

logger *server_logger_builder::build() const {
    return new server_logger(_destination, _output_streams);
}

logger_builder &server_logger_builder::set_destination(const std::string &dest) & {
    _destination = dest;
    return *this;
}

logger_builder &server_logger_builder::set_format(const std::string &format) & {
    return *this;
}
