# include <Siv3D.hpp>

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

    // 更新関数
    void update() override
    {

    }

    void draw() const override
    {
        Scene::SetBackground(ColorF(0.2, 0.8, 0.6));
    }
};

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

    while(System::Update())
    {
        // 現在のシーンを実行
        if(!manager.update())
        {
            break;
        }
    }
}
//loginの画面
void Main()
{
    Scene::SetBackground(Color(106.0, 90.0, 205.0,1.0));
    Window::SetStyle(WindowStyle::Sizable);
    const Font font1(30, Typeface::Bold);
 
    TextEditState tes2;
    tes2.text = U"Password";

    const Font font2(60, Typeface::Bold);
    constexpr Vec2 center(400, 70);
    const String text = U"Welcome";
    
    constexpr ColorF buttonColor(1.0, 90.0, 205.0,0.5);
    constexpr Circle button(700, 400, 20);
    Transition press(0.05s, 0.05s);

        
    while (System::Update())
{
    const bool mouseOver = button.mouseOver();
    // 円の上にマウスカーソルがあれば
    if (mouseOver)
    {
        Cursor::RequestStyle(CursorStyle::Hand);
    }
    press.update(button.leftPressed());
    const double t = press.value();
    button.movedBy(Vec2(0, 0).lerp(Vec2(0, 4), t))
    .drawShadow(Vec2(0, 6).lerp(Vec2(0, 1), t), 12 - t * 7, 5 - t * 4)
    .draw(buttonColor);
    
    // center から (4, 4) ずらした位置を中心にテキストを描く
    font2(text).drawAt(center.movedBy(4, 4), ColorF(106, 90, 205,0.5));
    font2(text).drawAt(center);
    SimpleGUI::TextBox(tes2, Vec2(200, 420),250,18);
    if (SimpleGUI::Button(U"Clear", Vec2(470, 420)))
    {
    tes2.clear();
    }


    }
   
};
//最初にloginする時passwordを作る画面
void Main()
{
    constexpr ColorF buttonColor1(245,245,245,1);
    constexpr Circle button1(760, 560, 19);
    Transition press(0.05s, 0.05s);
    Scene::SetBackground(Color(192,192,192));
    const Font font1(25, Typeface::Heavy);

    const String text = U"Please create your password";


    TextEditState tes1;
    while (System::Update())
    {
        // 文字カウントを 0.1 秒ごとに増やす
        const size_t length = static_cast<size_t>(Scene::Time() / 0.1);

        // text の文字数以上の length は切り捨てられる
        font1(text.substr(0, length)).drawAt(Scene::Center(), Color(41,26,33));
        SimpleGUI::TextBox(tes1, Vec2(280, 340), 250,18);
        const bool mouseOver = button1.mouseOver();
        if (mouseOver)
        {
        // マウスカーソルを手の形に
        Cursor::RequestStyle(CursorStyle::Hand);
        }
        press.update(button1.leftPressed());

        const double t = press.value();

        button1.movedBy(Vec2(0, 0).lerp(Vec2(0, 4), t))
        .drawShadow(Vec2(0, 6).lerp(Vec2(0, 1), t), 12 - t * 7, 5 - t * 4)
        .draw(buttonColor1);
    }
}

