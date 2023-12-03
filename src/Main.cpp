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
        ("port", po::value<std::string>()->default_value("0:65535"), "set port range in the format 'begin[:end]'")
        ("protocol", po::value<std::string>()->default_value("tcp"), "set protocol (tcp/udp)")
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

    std::size_t pos = portRange.find(':');
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

    const std::string protocol = vm["protocol"].as<std::string>();
    if (!(protocol == "tcp" || protocol == "udp")) {
        std::cerr << "Error: Invalid value for --protocol. Use 'tcp' or 'udp'." << std::endl;
        return 1;
    }

    const std::string show = vm["show"].as<std::string>();
    if (show != "open" && show != "closed" && show != "all") {
        std::cerr << "Error: Invalid value for --show. Use 'open', 'closed', or 'all'." << std::endl;
        return 1;
    }

    if (protocol == "tcp") {
        boost::asio::io_context io_service;

        boost::asio::ip::tcp::resolver resolver(io_service);
        boost::asio::ip::tcp::resolver::query query(host, "0");
        boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        std::map<unsigned int, std::unique_ptr<boost::asio::ip::tcp::socket>> sockets;
        for (unsigned int port = portBegin; port <= portEnd; port++) {
            auto socket = std::make_unique<boost::asio::ip::tcp::socket>(io_service);
            boost::asio::ip::tcp::endpoint peer_endpoint(endpoint_iterator->endpoint().address(), port);
            socket->async_connect(peer_endpoint, [&sockets, port, protocol, show](const boost::system::error_code& error)-> void {
                if (!error) {
                    if (show != "closed") {
                        std::cout << "Port " << port << "/" << protocol << " is open." << std::endl;
                    }
                } else {
                    if (show != "open") {
                        std::cout << "Port " << port << "/" << protocol << " is closed." << std::endl;
                    }
                }
                sockets.erase(port);
            });
            sockets.emplace(port, std::move(socket));
        }

        io_service.run();
    }

    return 0;
}
