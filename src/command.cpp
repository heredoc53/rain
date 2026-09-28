#include <sstream>
#include <ranges>
#include <tuple>
#include <memory>
#include <array>
#include <string_view>
#include <stdexcept>

#include <command.hpp>

#include <boost/process.hpp>
#include <boost/asio.hpp>
//#define FMT_HEADER_ONLY
#include <fmt/core.h>
//REMINDER: this boost::process is new v2 version, so api is completely diffrent from older version
namespace proc = boost::process;
namespace asio = boost::asio;
namespace vws = std::views;
namespace rngs = std::ranges;

using std::string_view;
using std::string;
using std::tuple;
using std::vector;

template<typename T>
concept PipeReader = requires(T t, string_view str){
    t(str);
};

namespace {

template<PipeReader Handler>
void read_pipe(asio::readable_pipe& pipe, Handler handler){
    auto buff = std::make_shared<std::array<char, 4096>>();

    pipe.async_read_some(asio::buffer(*buff), [&pipe, buff, handler](boost::system::error_code ec, std::size_t n){
        if(!ec){
            handler(string_view(buff->data(), n));
            read_pipe(pipe, handler);
        }else if(ec != asio::error::eof){
            throw std::runtime_error("Error reading output of process");
        }
    });
}

}

namespace command {

[[deprecated("Use execute_command(const Command&) instead")]]
tuple<string, string, int> execute_command(const string& com){
    asio::io_context ctx;
    asio::readable_pipe out_pipe(ctx), err_pipe(ctx);
    //asio::streambuf out_buf, err_buf;
    string out_str, err_str;

    const auto [name, args] = [&com]() -> tuple<string, vector<string>> {
        vector<string> temp;
        for(const auto&& str : com | vws::split(' ') | vws::filter([](auto&& p){ return !rngs::empty(p); }))
            temp.emplace_back(str.begin(), str.end());
        return std::make_tuple(temp.at(0), vector<string>(temp.begin() + 1, temp.end()));
    }();
                       //this function find executable by name and returns full path
    proc::process proc(ctx, proc::environment::find_executable(name),
        args, proc::process_stdio{.out=out_pipe, .err=err_pipe} );  //cmd_args[0] is name of executable everthing else is arguments

    read_pipe(out_pipe, [&](string_view data){ out_str.append(data); });
    read_pipe(err_pipe, [&](string_view data){ err_str.append(data); });

    ctx.run();
    const int exit_code = proc.wait();

    return std::make_tuple(out_str, err_str, exit_code);
}

tuple<string, string, int> execute_command(const Command& com){
    asio::io_context ctx;
    asio::readable_pipe out_pipe(ctx), err_pipe(ctx);
    //asio::streambuf out_buf, err_buf;
    string out_str, err_str;

    proc::process proc(ctx, proc::environment::find_executable(com.name),
        com.args, proc::process_stdio{.out=out_pipe, .err=err_pipe}
    );

    read_pipe(out_pipe, [&](string_view data){ out_str.append(data); });
    read_pipe(err_pipe, [&](string_view data){ err_str.append(data); });

    ctx.run();
    const int exit_code = proc.wait();

    return std::make_tuple(out_str, err_str, exit_code);
}

}
