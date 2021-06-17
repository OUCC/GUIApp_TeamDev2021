#include <stdexcept>

using namespace std;

/**
 * @brief SQL文の実行に失敗したときに発生する例外クラス
 * @details 基本的に防ぎようがない、一旦これの発生は考慮しないことにしてもいいかもしれない
 * 
 */

class sql_exception : public runtime_error {
public:
    sql_exception(string message): runtime_error(message.c_str()) {}
};