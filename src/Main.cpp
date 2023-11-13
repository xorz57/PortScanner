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
    ;
    // clang-format on

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (!vm.count("host") || !vm.count("begin-port") || !vm.count("end-port")) {
        std::cerr << "Usage: " << argv[0] << " --host <host> --begin-port <begin-port> --end-port <end-port>" << std::endl;
        return 1;
    }

    const std::string host = vm["host"].as<std::string>();
    const unsigned short beginPort = vm["begin-port"].as<unsigned short>();
    const unsigned short endPort = vm["end-port"].as<unsigned short>();

    std::vector<std::future<void>> futures;
    std::mutex mutex;

    for (unsigned short port = beginPort; port <= endPort; ++port) {
        futures.push_back(std::async(std::launch::async, [host, port, &mutex]() {
            if (isPortOpen(host, port)) {
                std::lock_guard lock(mutex);
                std::cout << "Port " << port << " is open." << std::endl;
            } else {
                std::lock_guard lock(mutex);
                std::cout << "Port " << port << " is closed." << std::endl;
            }
        }));
    }

    for (const auto &future: futures) {
        future.wait();
    }

    return 0;
}
