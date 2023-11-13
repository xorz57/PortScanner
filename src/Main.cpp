#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#include <iostream>

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
    } catch (const std::exception &) {
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

    if (!vm.count("host")) return 1;
    const std::string host = vm["host"].as<std::string>();

    if (!vm.count("begin-port")) return 1;
    const unsigned short beginPort = vm["begin-port"].as<unsigned short>();

    if (!vm.count("end-port")) return 1;
    const unsigned short endPort = vm["end-port"].as<unsigned short>();

    for (unsigned short port = beginPort; port <= endPort; ++port) {
        if (isPortOpen(host, port)) {
            std::cout << "Port " << port << " is open." << std::endl;
        } else {
            std::cout << "Port " << port << " is closed." << std::endl;
        }
    }

    return 0;
}