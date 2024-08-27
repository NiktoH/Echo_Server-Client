
#include <stdio.h>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

using namespace boost::asio;
using namespace boost::placeholders;
using boost::system::error_code;

io_service service;

size_t read_complete(char* buf, const error_code & err, size_t bytes) {
    if(err) return 0;
    bool found = std::find(buf, buf+bytes, '\n') < buf + bytes;
    return found ? 0 : 1;
}

ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 8001);

void sync_echo(std::string msg) {
    msg += "\n";
    ip::tcp::socket sock(service);
    sock.connect(ep);
    sock.write_some(buffer(msg));
    char buf[1024];
    int bytes = read(sock, buffer(buf), boost::bind(read_complete,buf,_1, _2));
std::string copy(buf, bytes-1);
msg = msg.substr(0, msg.size() - 1);
std::cout << "server echoed our " << msg << ": " << (copy == msg ? "OK" : "FAIL") << std::endl;

sock.close();
}

int main(int argc, char* argv[]) {
    char* messages[] = {"C++ is difficult language", "Horny Bot", "JS", "Boost Asio is terrible!", 0};
    boost::thread_group threads;
    for(char** message = messages; *message; ++message) {
        threads.create_thread(boost::bind(sync_echo, *message));
        boost::this_thread::sleep(boost::posix_time::millisec(100));
    }
    threads.join_all();
    return 0;
}