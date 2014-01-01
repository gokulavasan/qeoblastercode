#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <string>

using boost::asio::ip::tcp;

int main(int argc, char * argv[]) {
  try {
    if (argc != 2) {
      std::cerr << "Usage : client <port>" << std::endl;
      return 1;
    }

    boost::asio::io_service io_service;
    tcp::resolver resolver(io_service);
    tcp::resolver::query query("127.0.0.1", argv[1]);
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::socket socket(io_service);
    boost::asio::connect(socket, endpoint_iterator);
    boost::asio::streambuf init;
    boost::system::error_code error;
    boost::asio::read_until(socket, init, "\n", error);
    std::istream stri(&init);
    std::string si;
    std::getline(stri, si);
    std::cout << "Server Sent : " << si << std::endl;
    for (;;) {
      std::string s;
      getline(std::cin, s);
      s.append("\n");
      boost::asio::write(socket, boost::asio::buffer(s), error);
      std::cout << "Client Sent : " << s << std::endl;
      if (error) {
        std::cout << "Some Error " << std::endl;
        break;
      } else {
          boost::asio::streambuf buf;
          boost::asio::read_until (socket, buf, "\n", error);
          std::istream str(&buf);
          std::string s;
          std::getline(str, s);
          std::cout << "Server Sent : " << s << std::endl;
      }
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
