#include <string>

struct single_data {
    int id;
    std::string service_name;
    std::string url;
    std::string user_name;
    std::string password;

    single_data(
        int id,
        std::string service_name,
        std::string url,
        std::string user_name,
        std::string password):
        id(id),
        service_name(service_name),
        url(url),
        user_name(user_name),
        password(password)
    {};
};