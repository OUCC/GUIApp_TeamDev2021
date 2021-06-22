#include <Siv3D.hpp>
using App = SceneManager<String>;

// ログインシーン
class Login : public App::Scene {
private:
    Font *font;
    TextEditState tes;
    Vec2 *center;
    ColorF *buttonColor;
    Circle *button;
    Transition *press;
    const String text = U"Welcome";

public:
    Login(const InitData& init): IScene(init) { // コンストラクタ（必ず実装
        font = new Font(60, Typeface::Bold);
        center = new Vec2(400, 70);
        buttonColor = new ColorF(1.0, 90.0, 205.0, 0.5);
        button = new Circle(700, 400, 20);
        press = new Transition(0.05s, 0.05s);

        Scene::SetBackground(Color(106.0, 90.0, 205.0, 1.0));
        tes.text = U"Password";
    }

    ~Login(){ // Destructor
        delete font;
        delete center;
        delete buttonColor;
        delete button;
        delete press;
    }

    void update() override { // 更新関数
        if(/*db有無確認*/1) changeScene(U"CreatePassword"); // パスワード作成シーンに遷移

        const bool mouseOver = button->mouseOver();
        if(mouseOver) Cursor::RequestStyle(CursorStyle::Hand); // 円の上にマウスカーソルがあれば
        press->update(button->leftPressed());
        const double t = press->value();
        button->movedBy(Vec2(0, 0).lerp(Vec2(0, 4), t))
            .drawShadow(Vec2(0, 6).lerp(Vec2(0, 1), t), 12-t*7, 5-t*4)
            .draw(*buttonColor);

        // center から (4, 4) ずらした位置を中心にテキストを描く
        (*font)(text).drawAt(center->movedBy(4, 4), ColorF(106, 90, 205, 0.5));
        (*font)(text).drawAt(*center);
        SimpleGUI::TextBox(tes, Vec2(200, 420), 250, 18);
        if(SimpleGUI::Button(U"Clear", Vec2(470, 420))) tes.clear();
        if(button->leftPressed()){
            if(/*ログイン関数*/1) changeScene(U"MainScene", 1.0s); // メインシーンに 1 秒かけて遷移
        }
    }

    void draw() const override { // 描画関数 (const 修飾)
        Scene::SetBackground(ColorF(0.3, 0.4, 0.5));
    }
};

// パスワード作成シーン
class CreatePassword : public App::Scene {
private:
    ColorF* buttonColor1;
    Circle* button1;
    Transition* press;
    Font* font1;
    const String text = U"Please create your password";
    TextEditState tes1;

public:
    CreatePassword(const InitData& init) : IScene(init) { // コンストラクタ（必ず実装
        buttonColor1 = new ColorF(245, 245, 245, 1);
        button1 = new Circle(760, 560, 19);
        press = new Transition(0.05s, 0.05s);
        font1 = new Font(25, Typeface::Heavy);
        Scene::SetBackground(Color(192, 192, 192));
    }

    ~CreatePassword(){ // Destructor
        delete buttonColor1;
        delete button1;
        delete press;
        delete font1;
    }

    void update() override { // 更新関数
        const size_t length = static_cast<size_t>(Scene::Time() / 0.1); // 文字カウントを 0.1 秒ごとに増やす

        // text の文字数以上の length は切り捨てられる
        (*font1)(text.substr(0, length)).drawAt(Scene::Center(), Color(41, 26, 33));
        SimpleGUI::TextBox(tes1, Vec2(280, 340), 250, 18);

        const bool mouseOver = (*button1).mouseOver();
        if(mouseOver) Cursor::RequestStyle(CursorStyle::Hand); // マウスカーソルを手の形に
        (*press).update((*button1).leftPressed());
        const double t = (*press).value();
        (*button1).movedBy(Vec2(0, 0).lerp(Vec2(0, 4), t))
            .drawShadow(Vec2(0, 6).lerp(Vec2(0, 1), t), 12 - t * 7, 5 - t * 4)
            .draw(*buttonColor1);
    }

    void draw() const override { // 描画関数 (const 修飾)
        
    }
};

// メインシーン
class MainScene : public App::Scene {
public:
    MainScene(const InitData& init): IScene(init) {

    }

    void update() override {

    }

    void draw() const override {
        Scene::SetBackground(ColorF(0.2, 0.8, 0.6));
    }
};

void Main() {
    App manager; // シーンマネージャーを作成
    manager.add<Login>(U"Login"); // ログインシーン（名前は U"Login"）を登録
    manager.add<CreatePassword>(U"CreatePassword"); // パスワード作成シーン（名前は U"CreatePassword"）を登録
    manager.add<MainScene>(U"MainScene"); // メインシーン（名前は U"MainScene"）を登録
    manager.setFadeColor(Palette::Skyblue); // フェードイン・フェードアウト時の画面の色
    Window::SetStyle(WindowStyle::Sizable);

    while(System::Update()){
        if(!manager.update()) break; // 現在のシーンを実行
    }
}
