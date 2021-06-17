#include <string>

/**
 * @brief 1つの項目のデータを定めた構造体
 * @details すべて復号済み/未暗号化の文字列として使用する
 * 
 */
struct single_data {
/** データ管理に使用するID。UIに表示する必要はない。SQLのPRIMARY KEY AUTOINCREMENTにて自動で割り振っているため重複しないはず */
    int id; 
/** サービス名 */
    std::string service_name;
/** ユーザー名 */
    std::string user_name;
/** パスワード */
    std::string password;

/**
 * @brief コンストラクタ
 * 
 * @param id ID
 * @param service_name サービス名
 * @param user_name ユーザー名
 * @param password パスワード
 */
    single_data(
        int id,
        std::string service_name,
        std::string user_name,
        std::string password):
        id(id),
        service_name(service_name),
        user_name(user_name),
        password(password)
    {};
};