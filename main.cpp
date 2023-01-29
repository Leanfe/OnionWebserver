#include <onion/onion.h>
#include <onion/dict.h>
#include <onion/handler.h>
#include <onion/response.h>
#include <onion/request.h>

int main(int argc, char **argv){
  onion *o=onion_new(O_POOL);
  onion_set_port(o, "8080");
  onion_url *urls=onion_root_url(o);
  onion_url_add(urls, "^$", [](onion_request *req, onion_response *res){
    onion_response_write_header(res, "Content-Type", "text/html");
    onion_response_write0(res, "<html><body><h1>Hello, World!</h1></body></html>");
    return OCS_PROCESSED;
  });
  onion_listen(o);
  onion_free(o);
  return 0;
}
