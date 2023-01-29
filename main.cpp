#include <onion/onion.h>
#include <onion/dict.h>
#include <onion/handler.h>
#include <onion/response.h>
#include <onion/request.h>
#include <fstream>
#include <iostream>

std::string read_file(const char* filename)
{
    std::ifstream ifs(filename, std::ios::in | std::ios::binary | std::ios::ate);
    std::ifstream::pos_type pos = ifs.tellg();

    std::vector<char> result(pos);

    ifs.seekg(0, std::ios::beg);
    ifs.read(&result[0], pos);

    return std::string(result.data(), result.size());
}

int main(int argc, char **argv){
  onion *o=onion_new(O_POOL);
  onion_set_port(o, "8080");
  onion_url *urls=onion_root_url(o);
  onion_url_add(urls, "^$", [](onion_request *req, onion_response *res){
    std::string html = read_file("index.html");
    onion_response_write_header(res, "Content-Type", "text/html");
    onion_response_write(res, html.c_str(), html.size());
    return OCS_PROCESSED;
  });
  onion_listen(o);
  onion_free(o);
  return 0;
}
