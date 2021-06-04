#include <stdexcept>

using namespace std;

class sql_exception : public runtime_error {
public:
    sql_exception(string message): runtime_error(message.c_str()) {}
};