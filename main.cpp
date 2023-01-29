#include <onion/onion.h>

int main(int argc, char **argv){
  onion *o=onion_new(O_POOL);
  onion_set_port(o, "8080");
  onion_url *urls=onion_root_url(o);
  onion_url_add(urls, "^$", [](onion_request *req, onion_response *res){
    onion_response_write0(res, "Hello, World!");
    return OCS_PROCESSED;
  });
  onion_listen(o);
  onion_free(o);
  return 0;
}
