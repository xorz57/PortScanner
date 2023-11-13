#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#include <future>
#include <iostream>
#include <mutex>
#include <vector>

bool isPortOpen(const std::string &host, unsigned short port) {
    try {
        using boost::asio::ip::tcp;
        boost::asio::io_context io_context;
        tcp::socket socket(io_context);
        tcp::resolver resolver(io_context);
        tcp::resolver::query query(host, std::to_string(port));
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
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
        ("host", po::value<std::string>()->required(), "set host")
        ("begin-port", po::value<unsigned short>()->required(), "set begin-port")
        ("end-port", po::value<unsigned short>()->required(), "set end-port")
        ("show", po::value<std::string>()->default_value("all"), "display only 'open', 'closed', or 'all' ports")
    ;
    // clang-format on

    po::variables_map vm;

    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    if (!vm.count("host") || !vm.count("begin-port") || !vm.count("end-port")) {
        std::cerr << "Usage: " << argv[0] << " --host <host> --begin-port <begin-port> --end-port <end-port> [--show open/closed/all]" << std::endl;
        return 1;
    }

    const std::string host = vm["host"].as<std::string>();

    const unsigned short beginPort = vm["begin-port"].as<unsigned short>();
    const unsigned short endPort = vm["end-port"].as<unsigned short>();
    if (beginPort > endPort) {
        std::cerr << "Error: begin-port should be less than or equal to end-port." << std::endl;
        return 1;
    }

    const std::string showOption = vm["show"].as<std::string>();
    if (showOption != "open" && showOption != "closed" && showOption != "all") {
        std::cerr << "Error: Invalid value for --show. Use 'open', 'closed', or 'all'." << std::endl;
        return 1;
    }

    std::vector<std::future<void>> futures;
    std::mutex mutex;

    for (unsigned short port = beginPort; port <= endPort; ++port) {
        futures.push_back(std::async(std::launch::async, [host, port, &mutex, showOption]() {
            bool status = isPortOpen(host, port);
            std::lock_guard lock(mutex);
            if ((showOption == "open" && status) || (showOption == "closed" && !status) || (showOption == "all")) {
                std::cout << "Port " << port << " is " << (status ? "open" : "closed") << "." << std::endl;
            }
        }));
    }

    for (const auto &future: futures) {
        future.wait();
    }

    return 0;
}
