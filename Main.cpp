# include <Siv3D.hpp>
# include "db.hpp"

using App = SceneManager<String>;

// ログインシーン
class Login : public App::Scene
{
public:

    // コンストラクタ（必ず実装）
    Login(const InitData& init): IScene(init)
    {

    }

    // 更新関数
    void update() override
    {
        if(SimpleGUI::Button(U"Red", Vec2(100, 100)))
        {
            if(/*ログイン関数*/1){
                // メインシーンに 1 秒かけて遷移
                changeScene(U"MainScene", 1.0s);
            }
        }
    }

    // 描画関数 (const 修飾)
    void draw() const override
    {
        Scene::SetBackground(ColorF(0.3, 0.4, 0.5));
    }
};

// メインシーン
class MainScene : public App::Scene
{
public:
    MainScene(const InitData& init): IScene(init)
    {

    }

    void update() override
    {

    }

    void draw() const override
    {
        Scene::SetBackground(ColorF(0.2, 0.8, 0.6));
    }
};

void kowerkoint_dbg() {
    db database;
    //database.register_passwd(U"vim");
    Print << database.login(U"vim");
    Array<single_data> data = database.read_data();
    for (int i = 0; i < data.size(); i++) {
        Print << data[i].service_name << data[i].user_name << data[i].password;
    }
}

void Main()
{
    // シーンマネージャーを作成
    App manager;

    // ログインシーン（名前は U"Login"）を登録
    manager.add<Login>(U"Login");

    // メインシーン（名前は U"MainScene"）を登録
    manager.add<MainScene>(U"MainScene");

    // フェードイン・フェードアウト時の画面の色
    manager.setFadeColor(Palette::Skyblue);

    kowerkoint_dbg();
    while(System::Update())
    {
        // 現在のシーンを実行
        if(!manager.update())
        {
            break;
        }
    }
}