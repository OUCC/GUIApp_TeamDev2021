#include <stdexcept>

using namespace std;

class db_exception : public runtime_error {
public:
    db_exception(string message): runtime_error(message.c_str()) {}
};