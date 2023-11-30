#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#include <iostream>
#include <map>

int main(int argc, char *argv[]) {
    namespace po = boost::program_options;

    po::options_description desc("Allowed options");

    // clang-format off
    desc.add_options()
        ("help", "display help message")
        ("host", po::value<std::string>()->default_value("127.0.0.1"), "set host")
        ("protocol", po::value<std::string>()->default_value("tcp"), "set protocol (tcp/udp)")
        ("begin-port", po::value<unsigned short>()->default_value(0), "set begin-port")
        ("end-port", po::value<unsigned short>()->default_value(65535), "set end-port")
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

    if (!vm.count("host") || !vm.count("begin-port") || !vm.count("end-port")) {
        std::cerr << desc << std::endl;
        return 1;
    }

    const std::string host = vm["host"].as<std::string>();

    const std::string protocol = vm["protocol"].as<std::string>();
    if (protocol != "tcp" && protocol != "udp") {
        std::cerr << "Error: Invalid value for --protocol. Use 'tcp' or 'udp'." << std::endl;
        return 1;
    }

    const unsigned short beginPort = vm["begin-port"].as<unsigned short>();
    const unsigned short endPort = vm["end-port"].as<unsigned short>();
    if (beginPort > endPort || endPort > 65535) {
        std::cerr << "Error: Invalid port range. begin-port should be less than or equal to end-port, and both should be in the range [0, 65535]." << std::endl;
        return 1;
    }

    const std::string show = vm["show"].as<std::string>();
    if (show != "open" && show != "closed" && show != "all") {
        std::cerr << "Error: Invalid value for --show. Use 'open', 'closed', or 'all'." << std::endl;
        return 1;
    }

    boost::asio::io_context io_service;

    boost::asio::ip::tcp::resolver resolver(io_service);
    boost::asio::ip::tcp::resolver::query query(host, "0");
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    if (protocol == "tcp") {
        std::map<unsigned short, std::unique_ptr<boost::asio::ip::tcp::socket>> sockets;
        for (unsigned short port = beginPort; port < endPort; port++) {
            auto s = std::make_unique<boost::asio::ip::tcp::socket>(io_service);
            auto endpoint = boost::asio::ip::tcp::endpoint(endpoint_iterator->endpoint().address(), port);
            s->async_connect(endpoint, [show, protocol, port](const boost::system::error_code& error) {
                if (!error) {
                    if (show != "closed") {
                        std::cout << "Port " << port << "/" << protocol << " is " << "open" << "." << std::endl;
                    }
                } else {
                    if (show != "open") {
                        std::cout << "Port " << port << "/" << protocol << " is " << "closed" << "." << std::endl;
                    }
                }
            });
            sockets.emplace(port, std::move(s));
        }
        io_service.run();
    }

    return 0;
}
