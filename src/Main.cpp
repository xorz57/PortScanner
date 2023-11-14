#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#include <future>
#include <iostream>
#include <mutex>
#include <vector>

bool isTCPPortOpen(const std::string &host, unsigned int port) {
    try {
        boost::asio::io_context io_context;
        boost::asio::ip::tcp::socket socket(io_context);
        boost::asio::ip::tcp::resolver resolver(io_context);
        boost::asio::ip::tcp::resolver::query query(host, std::to_string(port));
        boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
        boost::asio::connect(socket, endpoint_iterator);
        return true;
    } catch (const boost::system::system_error &) {
        return false;
    }
}

int main(int argc, char *argv[]) {
    namespace po = boost::program_options;

    po::options_description desc("Allowed options");

    // clang-format off
    desc.add_options()
        ("help", "display help message")
        ("host", po::value<std::string>()->default_value("127.0.0.1"), "set host")
        ("begin-port", po::value<unsigned int>()->default_value(0), "set begin-port")
        ("end-port", po::value<unsigned int>()->default_value(65535), "set end-port")
        ("show", po::value<std::string>()->default_value("all"), "display only 'open', 'closed', or 'all' ports")
        ("protocol", po::value<std::string>()->default_value("tcp"), "set protocol (tcp/udp)")
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

    if (vm.empty() || vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }

    if (!vm.count("host") || !vm.count("begin-port") || !vm.count("end-port")) {
        std::cerr << "Usage: " << argv[0] << " --host <host> --begin-port <begin-port> --end-port <end-port> [--show open/closed/all] [--protocol tcp/udp]" << std::endl;
        return 1;
    }

    const std::string host = vm["host"].as<std::string>();

    const unsigned int beginPort = vm["begin-port"].as<unsigned int>();
    const unsigned int endPort = vm["end-port"].as<unsigned int>();
    if (beginPort > endPort || endPort > 65535) {
        std::cerr << "Error: Invalid port range. begin-port should be less than or equal to end-port, and both should be in the range [0, 65535]." << std::endl;
        return 1;
    }

    const std::string showOption = vm["show"].as<std::string>();
    if (showOption != "open" && showOption != "closed" && showOption != "all") {
        std::cerr << "Error: Invalid value for --show. Use 'open', 'closed', or 'all'." << std::endl;
        return 1;
    }

    const std::string protocolOption = vm["protocol"].as<std::string>();
    if (protocolOption != "tcp" && protocolOption != "udp") {
        std::cerr << "Error: Invalid value for --protocol. Use 'tcp' or 'udp'." << std::endl;
        return 1;
    }

    std::vector<std::future<void>> futures;
    std::mutex mutex;

    for (unsigned int port = beginPort; port <= endPort; ++port) {
        futures.emplace_back(std::async(std::launch::async, [showOption, protocolOption, host, port, &mutex]() {
            if (protocolOption == "tcp") {
                bool tcpPortStatus = isTCPPortOpen(host, port);
                if ((showOption == "open" && tcpPortStatus) || (showOption == "closed" && !tcpPortStatus) || (showOption == "all")) {
                    std::lock_guard lock(mutex);
                    std::cout << "Port " << port << "/tcp is " << (tcpPortStatus ? "open" : "closed") << "." << std::endl;
                }
            }
        }));
    }

    for (const auto &future: futures) {
        future.wait();
    }

    return 0;
}
