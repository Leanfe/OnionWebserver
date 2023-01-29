#include <onion/onion.h>
#include <onion/request.h>
#include <onion/response.h>
#include <onion/dict.h>
#include <onion/log.h>
#include <string>
#include <fstream>
#include <sqlite3pp.h>

using namespace sqlite3pp;

// Function to read a file into a string
std::string read_file(const char *filename) {
  std::ifstream in(filename, std::ios::in | std::ios::binary);
  if (in) {
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();
    return contents;
  }
  return "";
}

int main(int argc, char **argv) {
    onion *o = onion_new(O_POOL);
    onion_set_hostname(o, "localhost");
    onion_set_port(o, "8080");
    onion_url *urls = onion_root_url(o);

    database db("users.db");
    if (!db.exists("user")) {
        db.exec("CREATE TABLE user (username TEXT PRIMARY KEY, password TEXT);");
    }

    onion_url_add(urls, "^/$", [](onion_request *req, onion_response *res) {
        std::string html = read_file("index.html");
        onion_response_write_header(res, "Content-Type", "text/html");
        onion_response_write(res, html.c_str(), html.size());
        return OCS_PROCESSED;
    });

    onion_url_add(urls, "^/bootstrap.min.css$", [](onion_request *req, onion_response *res) {
        std::string css = read_file("bootstrap.min.css");
        onion_response_write_header(res, "Content-Type", "text/css");
        onion_response_write(res, css.c_str(), css.size());
        return OCS_PROCESSED;
    });

    onion_url_add(urls, "^/login$", [](onion_request *req, onion_response *res) {
        if (onion_request_get_flags(req) & OR_POST) {
            onion_dict *post = onion_request_get_post_dict(req);
            const char *username = onion_dict_get(post, "username");
            const char *password = onion_dict_get(post, "password");

            if (!username || !password) {
                onion_response_write_header(res, "Content-Type", "text/html");
                onion_response_write(res, "Missing username or password.", 26);
                return OCS_PROCESSED;
            }

            transaction trans(db);
            query q(db, "SELECT password FROM user WHERE username=?");
            q.bind(username);
            for (query::iterator i = q.begin(); i != q.end(); ++i) {
                if (password == i->getstd::string(0)) {
                    // Login successful, start session
                    onion_dict *session = onion_request_get_session(req, O_RDWR);
                    onion_dict_add(session, "username", username, 0);
                    onion_response_write_header(res, "Content-Type", "text/html");
                    onion_response_write(res, "Login successful.", 18);
                    return OCS_PROCESSED;
                }
            }
            // Login failed
            onion_response_write_header(res, "Content-Type", "text/html");
            onion_response_write(res, "Login failed.", 14);
            return OCS_PROCESSED;
        } else {
            // Show login form
            std::string form = read_file("login.html");
            onion_response_write_header(res, "Content-Type", "text/html");
            onion_response_write(res, form.c_str(), form.size());
            return OCS_PROCESSED;
        }
    });

    onion_url_add(urls, "^/logout$", [](onion_request *req, onion_response *res){
        onion_dict *session = onion_request_get_session(req, O_RDWR);
        onion_dict_clear(session);
        onion_response_write_header(res, "Content-Type", "text/html");
        onion_response_write(res, "Logout successful.", 19);
        return OCS_PROCESSED;
    });

    onion_listen(o);

    onion_free(o);
    return 0;
}
