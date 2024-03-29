#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#include <iostream>
#include <map>
#include <sstream>

int main(int argc, char *argv[]) {
    namespace po = boost::program_options;

    po::options_description desc("Allowed options");

    // clang-format off
    desc.add_options()
        ("help", "display help message")
        ("host", po::value<std::string>()->default_value("127.0.0.1"), "set host")
        ("port", po::value<std::string>()->default_value("0-1024"), "set port range in the format 'begin[-end]'")
        ("show", po::value<std::string>()->default_value("open"), "display only 'open', 'closed', or 'all' ports")
    ;
    // clang-format on

    po::variables_map vm;

    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch (const po::error &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }

    const std::string host = vm["host"].as<std::string>();

    const std::string portRange = vm["port"].as<std::string>();

    unsigned int portBegin = 0;
    unsigned int portEnd = 0;

    std::size_t pos = portRange.find('-');
    if (pos != std::string::npos) {
        std::string portBeginString = portRange.substr(0, pos);
        std::string portEndString = portRange.substr(pos + 1);

        std::istringstream(portBeginString) >> portBegin;
        std::istringstream(portEndString) >> portEnd;
    } else {
        std::istringstream(portRange) >> portBegin;
        portEnd = portBegin;
    }

    if (portBegin > portEnd || portBegin < 0 || portBegin > 65535 || portEnd < 0 || portEnd > 65535) {
        std::cerr << "Error: Invalid port range." << std::endl;
        return 1;
    }

    const std::string show = vm["show"].as<std::string>();
    if (show != "open" && show != "closed" && show != "all") {
        std::cerr << "Error: Invalid value for --show. Use 'open', 'closed', or 'all'." << std::endl;
        return 1;
    }

    boost::asio::io_context io_service;
    std::map<unsigned int, std::unique_ptr<boost::asio::ip::tcp::socket>> sockets;
    for (unsigned int port = portBegin; port <= portEnd; port++) {
        boost::asio::ip::tcp::resolver resolver(io_service);
        boost::asio::ip::tcp::resolver::query query(host, std::to_string(port));
        try {
            boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);
            auto socket = std::make_unique<boost::asio::ip::tcp::socket>(io_service);
            socket->async_connect(iterator->endpoint(), [port, show](const boost::system::error_code &error) -> void {
                if ((error && show == "closed") || (!error && show == "open")) {
                    std::cout << "Port " << port << "/tcp is " << (error ? "closed" : "open") << "." << std::endl;
                }
            });
            sockets.emplace(port, std::move(socket));
        } catch (const boost::system::system_error &e) {
            std::cerr << e.what() << std::endl;
            return 1;
        }
    }
    io_service.run();

    return 0;
}
