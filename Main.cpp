#include <Siv3D.hpp>
#include "db.hpp"

using App = SceneManager<String>;
db Database;
Array<single_data> passArray; //パスワードの三項目をセットにした配列

// ログインシーン
class Login : public App::Scene {
private:
    Font *font;
    TextEditState tes;
    Vec2 *center;
    ColorF *buttonColor;
    Circle *button;
    Transition *press;
    String text = U"Welcome";
    bool retInitialize;
    bool isInitialize;
    Vec2 screenSize;
    String initWarmText = U"";
    String key_db_name;
    String main_db_name;


    Vec2 ratioPos(double x, double y) {
        return Vec2(screenSize.x * x, screenSize.y * y);
    }

public:
    Login(const InitData& init): IScene(init) { // コンストラクタ（必ず実装
        font = new Font(60, Typeface::Bold);
        center = new Vec2(400, 70);
        buttonColor = new ColorF(1.0, 90.0, 205.0, 0.5);
        button = new Circle(700, 400, 20);
        press = new Transition(0.05s, 0.05s);
        retInitialize = false;
        isInitialize = false;
        FontAsset::Register(U"Regular", 20);
        key_db_name = U"key.dat";
        main_db_name = U"main.dat";


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
        if(!Database.is_registered()) changeScene(U"CreatePassword"); // パスワード作成シーンに遷移

        screenSize = Vec2(Window::ClientWidth(), Window::ClientHeight());
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
        SimpleGUI::TextBox(tes, Vec2(200, 420), 250, 64, !retInitialize);
        if(SimpleGUI::Button(U"Clear", Vec2(470, 420), 100, !retInitialize)) tes.clear();
        if(button->leftClicked() && !retInitialize){
            int cnt = 0;
            bool valid = true;
            for(char32_t var: tes.text) {
                if(U'!'<=var && var<=U'~') cnt++;
                else{
                    valid = false;
                    break;
                }
            }
            if(!valid || !cnt) text = U"Invalid Password.";
            else if(Database.login(tes.text)) changeScene(U"MainScene", 1.0s); // メインシーンに 1 秒かけて遷移
            else text = U"Wrong Password";
        }

        /// <summary>
        /// 初期化部分
        /// </summary>
        if (SimpleGUI::Button(U"初期化", Vec2(0, 560), 100, !retInitialize)) {
            retInitialize = true;
        }
        RectF(Vec2(0, 0), screenSize).draw(ColorF(Palette::Black, !retInitialize == true ? 0.0 : 0.5));
        if (retInitialize == true) {
            RectF(ratioPos(0.15, 0.25), ratioPos(0.7, 0.5)).draw(ColorF(0.8, 0, 0, 1.0));
            //FontAsset(U"Regular")(U"警告").draw(ratioPos(0.5, 0.4), ColorF(1, 1, 1, 1));
            initWarmText = U"警告";
            (*font)(initWarmText).drawAt(ratioPos(0.5, 0.35), ColorF(1, 1, 1, 1));
            FontAsset(U"Regular")(U"初期化を行うと、マネージャのパスワード\n及び保存されたデータは完全に失われます").draw(ratioPos(0.27, 0.43), ColorF(1, 1, 1, 1));
            FontAsset(U"Regular")(U"初期化しますか？").draw(ratioPos(0.4, 0.58), ColorF(1, 1, 1, 1));
            if (SimpleGUI::Button(U"キャンセル", ratioPos(0.55, 0.65))) {
                initWarmText = U"";
                retInitialize = false;
            }
            if (SimpleGUI::Button(U"はい", ratioPos(0.29, 0.65))) {
                isInitialize = true;
            }
            if (isInitialize == true){
                initWarmText = U"";
                bool isSuccessCopyKey = FileSystem::Copy(key_db_name, U"intTMP" + key_db_name, CopyOption::OverwriteExisting);
                bool isSuccessCopyMain = FileSystem::Copy(main_db_name, U"mainInitTMP.dat", CopyOption::OverwriteExisting);
                bool isSuccessRemoveKey = FileSystem::Remove(U"key.dat", false);
                bool isSuccessRemoveMain = FileSystem::Remove(U"main.dat", false);
                if (((isSuccessRemoveKey || !isSuccessCopyKey) && (isSuccessRemoveMain || !isSuccessCopyMain))) {
                    FileSystem::Remove(U"keyInitTMP.dat", false);
                    FileSystem::Remove(U"mainInitTMP.dat", false);
                    isInitialize = false;
                    retInitialize = false;
                    text = U"初期化成功";
                    //changeScene(U"CreatePassword");
                }
                else {
                    initWarmText = U"";
                    text = U"初期化失敗";
                    FileSystem::Copy(U"keyInitTMP.dat", U"key.dat", CopyOption::OverwriteExisting);
                    FileSystem::Copy(U"mainInitTMP.dat", U"main.dat", CopyOption::OverwriteExisting);
                    FileSystem::Remove(U"keyInitTMP.dat", false);
                    FileSystem::Remove(U"mainInitTMP.dat", false);
                    isInitialize = false;
                    retInitialize = false;
                }

            }
        }
    }

    void draw() const override { // 描画関数 (const 修飾)

    }
};

// パスワード作成シーン
class CreatePassword : public App::Scene {
private:
    ColorF* buttonColor1;
    Circle* button1;
    Transition* press;
    Font* font1;
    String text = U"Please create your password";
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
        SimpleGUI::TextBox(tes1, Vec2(280, 340), 250, 64);

        const bool mouseOver = (*button1).mouseOver();
        if(mouseOver) Cursor::RequestStyle(CursorStyle::Hand); // マウスカーソルを手の形に
        (*press).update((*button1).leftPressed());
        const double t = (*press).value();
        (*button1).movedBy(Vec2(0, 0).lerp(Vec2(0, 4), t))
            .drawShadow(Vec2(0, 6).lerp(Vec2(0, 1), t), 12 - t * 7, 5 - t * 4)
            .draw(*buttonColor1);

        if(button1->leftClicked()){
            int cnt = 0;
            bool valid = true;
            for(char32_t var: tes1.text) {
                if(U'!'<=var && var<=U'~') cnt++;
                else{
                    valid = false;
                    break;
                }
            }
            if(valid && cnt){
                Database.register_passwd(tes1.text);
                changeScene(U"MainScene", 1.0s); // メインシーンに 1 秒かけて遷移
            }
            else text = U"Invalid Password.";
        }
    }

    void draw() const override { // 描画関数 (const 修飾)
        
    }
};

// メインシーン
class MainScene : public App::Scene {
private:
    /// <summary>パスワードの欄の表示を切り替えるための変数</summary>
    static inline bool isVisiblePass = false;
    static inline int popupIndex = -1;

    /// <summary>ポップアップの状態を表す</summary>
    enum PopupState {
        notPopup,
        forAdd,
        forEdit,
        confirming,
        forDelete,
    };
    PopupState popupState = notPopup;
    PopupState lastPopupState;

    // デザイン用定数
    struct Design {
        static inline Color inFrame = Palette::Whitesmoke;
        static inline Color frame = Palette::Lightgrey;
        static inline Color background = Palette::White;
        static inline Color fontColor = Palette::Black;

        static inline Color deletePopupBG = Palette::Red;
        static inline Color deletePopupString = Palette::Yellow;
    };

    // スクロール用変数
    struct {
        int current = 0;
        int max = 10;
        double wheel = 0.0;
    } scroll;

    // テキストボックス用
    TextEditState serviceNameText, userNameText, passwordText;

    // 完了通知用
    double noticeTimer = 5.0;
    enum {
        notice_copy,
        notice_delete,
        notice_add,
        notice_edit,
    } noticeType;

    Vec2 screenSize;

    Vec2 ratioPos(double x, double y) {
        return Vec2(screenSize.x * x, screenSize.y * y);
    }

public:
    MainScene(const InitData& init) : IScene(init) {
        Scene::SetBackground(Design::background);
        System::SetTerminationTriggers(UserAction::CloseButtonClicked);
        Window::SetTitle(U"Password Manager");
        Window::SetStyle(WindowStyle::Sizable);
        Scene::SetScaleMode(ScaleMode::ResizeFill);

        //通常表示用フォントアセット
        FontAsset::Register(U"Regular", 20);

        //画像アセット
        TextureAsset::Register(U"copy", U"images/copy.png");
        TextureAsset::Register(U"edit", U"images/edit.png");
        TextureAsset::Register(U"delete", U"images/delete.png");
        TextureAsset::Register(U"visible", U"images/visible.png");
        TextureAsset::Register(U"invisible", U"images/invisible.png");

        passArray = Database.read_data();
    }

    // 更新関数
    void update() override {
        ClearPrint();
        noticeTimer += Scene::DeltaTime();
        screenSize = Vec2(Window::ClientWidth(), Window::ClientHeight());
        scroll.max = floor((screenSize.y-100)/50);

        if (Rect(50, 50, screenSize.x-100, screenSize.y-110).drawFrame(10, Design::frame).draw(Design::inFrame).mouseOver() && popupState == notPopup) {
            scroll.wheel += Mouse::Wheel();
            if (scroll.wheel < 0) scroll.wheel = 0;
            if (scroll.wheel > passArray.size()-1) scroll.wheel = passArray.size()-1;

            scroll.current = floor(scroll.wheel);
        }

        FontAsset(U"Regular")(U"サービス名").draw(70, 10, Design::fontColor);
        FontAsset(U"Regular")(U"ユーザー名").draw(ratioPos(0.35,0).x, 10, Design::fontColor);
        FontAsset(U"Regular")(U"パスワード").draw(ratioPos(0.6,0).x, 10, Design::fontColor);

        if (TextureAsset(isVisiblePass ? U"visible" : U"invisible").resized(30).draw(Arg::center(ratioPos(0.6, 0).x+130, isVisiblePass ? 25 : 26)).mouseOver() && popupState == notPopup) {
            Cursor::RequestStyle(CursorStyle::Hand);
            if (MouseL.down()) isVisiblePass ^= true;
        }

        if (SimpleGUI::Button(U"追加", Vec2(screenSize.x - 130, 5))) {
            popupState = forAdd;
            serviceNameText.text = U"";
            userNameText.text = U"";
            passwordText.text = U"";
        }

        for (int i = 0; i < scroll.max; i++) {
            if (scroll.current + i >= passArray.size()) break;
            int height = 50 + 50*i;

            // Array passArrayのインデックスは scroll.current + i
            FontAsset(U"Regular")(passArray[scroll.current + i].service_name).draw(70, height, Design::fontColor);
            FontAsset(U"Regular")(passArray[scroll.current + i].user_name).draw(ratioPos(0.35, 0).x, height, Design::fontColor);
            FontAsset(U"Regular")(isVisiblePass ? passArray[scroll.current + i].password : U"*****").draw(ratioPos(0.6, 0).x, height, Design::fontColor);

            if (TextureAsset(U"copy").resized(30).draw(screenSize.x-160, height).mouseOver() && popupState == notPopup) {
                Cursor::RequestStyle(CursorStyle::Hand);
                if (MouseL.down()) {
                    // パスワードのコピー処理
                    Clipboard::SetText(passArray[scroll.current + i].password);
                    // コピーできたら以下をする
                    noticeType = notice_copy;
                    noticeTimer = 0.0;
                }
            }
            if (TextureAsset(U"edit").resized(30).draw(screenSize.x - 120, height).mouseOver() && popupState == notPopup) {
                Cursor::RequestStyle(CursorStyle::Hand);
                if (MouseL.down()) {
                    popupIndex = scroll.current + i;
                    popupState = forEdit;
                    serviceNameText.text = passArray[popupIndex].service_name;
                    userNameText.text = passArray[popupIndex].user_name;
                    passwordText.text = passArray[popupIndex].password;
                }
            }
            if (TextureAsset(U"delete").resized(30).draw(screenSize.x - 75, height).mouseOver() && popupState == notPopup) {
                Cursor::RequestStyle(CursorStyle::Hand);
                if (MouseL.down()) {
                    popupIndex = scroll.current + i;
                    popupState = forDelete;
                }
            }
        }

        RectF(Vec2(0,0),screenSize).draw(ColorF(Palette::Black, popupState == notPopup ? 0.0 : 0.5));
        int cnt = 0;
        bool valid = true;
        //ポップアップ時
        switch (popupState) {
            case forAdd:
            case forEdit:
            case confirming:
                RectF(ratioPos(0.15,0.25), ratioPos(0.7,0.5)).draw(Design::background);

                FontAsset(U"Regular")(U"サービス名").draw(ratioPos(0.2,0.3), Design::fontColor);
                SimpleGUI::TextBox(serviceNameText, ratioPos(0.2,0.35), screenSize.x / 4, unspecified, popupState != confirming);
                FontAsset(U"Regular")(U"ユーザー名").draw(ratioPos(0.2, 0.42), Design::fontColor);
                SimpleGUI::TextBox(userNameText, ratioPos(0.2, 0.47), screenSize.x / 4, unspecified, popupState != confirming);
                FontAsset(U"Regular")(U"パスワード").draw(ratioPos(0.2, 0.54), Design::fontColor);
                SimpleGUI::TextBox(passwordText, ratioPos(0.2, 0.59), screenSize.x /4, unspecified, popupState != confirming);

                for(char32_t var: passwordText.text) {
                    if(U'!'<=var && var<=U'~') cnt++;
                    else{
                        valid = false;
                        break;
                    }
                }
                
                if (popupState != confirming || !valid || !cnt) {
                    FontAsset(U"Regular")(popupState == forAdd ? U"パスワードの追加" : U"パスワードの変更").draw(ratioPos(0.5,0.3), Design::fontColor);
                    if (!valid || !cnt) FontAsset(U"Regular")(U"0文字以上の ASCII 文字のみ\n対応です。").draw(ratioPos(0.5, 0.4), Design::fontColor);
                    else if (SimpleGUI::Button(U"決定", ratioPos(0.5,0.5))) {
                        lastPopupState = popupState;
                        popupState = confirming;
                    }
                    if (SimpleGUI::Button(U"キャンセル", ratioPos(0.5,0.65))) popupState = notPopup;
                }
                else {
                    // 再確認
                    RectF(ratioPos(0.5,0.3),ratioPos(0.33,0.4)).drawFrame(5, Design::frame);
                    FontAsset(U"Regular")(U"この内容で\n確定しますか？").draw(ratioPos(0.57,0.38), Design::fontColor);
                    if (SimpleGUI::Button(U"はい", ratioPos(0.54, 0.58),80)) {
                        single_data temp(serviceNameText.text, userNameText.text, passwordText.text);
                        // パスワードの追加・変更処理
                        // Array passArray のインデックスは popupIndex
                        if(lastPopupState == forAdd){ // 追加
                            passArray << temp;
                            noticeType = notice_add;
                        }else if(lastPopupState == forEdit){ // 変更
                            passArray[popupIndex] = temp;
                            noticeType = notice_edit;
                        }
                        Database.write_data(passArray);
                        popupState = notPopup;
                        noticeTimer = 0.0;
                    }
                    if (SimpleGUI::Button(U"いいえ", ratioPos(0.7,0.58),80)) popupState = lastPopupState;
                }
                break;

            case forDelete:
                RectF(Arg::center(screenSize/2), 300, 200).draw(Design::deletePopupBG);
                FontAsset(U"Regular")(U"本当に削除しますか？").draw(Arg::center(screenSize.x*0.5 , screenSize.y*0.45), Design::deletePopupString);

                if (SimpleGUI::Button(U"はい", ratioPos(0.5, 0.55) - Vec2(100,0), 80)) {
                    // パスワードの削除処理
                    // Array passArrayのインデックスは popupIndex
                    for(int j=popupIndex; j<passArray.size()-1; j++){
                        passArray[j] = passArray[j+1];
                    }
                    passArray.pop_back();
                    Database.write_data(passArray);
                    popupState = notPopup;

                    // 削除できたら以下をする
                    noticeType = notice_delete;
                    noticeTimer = 0.0;
                }
                if (SimpleGUI::Button(U"いいえ", ratioPos(0.5,0.55) + Vec2(20,00), 80)) popupState = notPopup;
                break;

            default: break;
        }
        
        int copyNoticeX = (2.5-abs(noticeTimer-2.5))*300;
        if (copyNoticeX > 200) copyNoticeX = 200;
        Rect(screenSize.x-copyNoticeX, screenSize.y-50, 200, 50).draw(Design::fontColor);
        String noticeMessage;
        switch (noticeType) {
            case notice_copy:
                noticeMessage = U"コピーしました。";
                break;
            case notice_delete:
                noticeMessage = U"削除しました。";
                break;
            case notice_add:
                noticeMessage = U"追加しました。";
                break;
            case notice_edit:
                noticeMessage = U"変更しました。";
                break;
        }
        FontAsset(U"Regular")(noticeMessage).draw(screenSize.x + 20 - copyNoticeX, screenSize.y - 40, Design::background);
    }

    void draw() const override {

    }
};

void kowerkoint_dbg() {
    db database;
    //database.register_passwd(U"abc");
    Print << database.login(U"abc");
    /*
    Print << database.login(U"vim");
    //Print << database.login(U"emacs");
    */
    Array<single_data> data = database.read_data();
    for(int i = 0; i < data.size(); i++) {
        Print << data[i].password;
    }
    data << single_data(U"サービス2", U"あいうえお", U"applebananachocolatecookie!^as~\"\\e!&");
    //data << single_data(U"サービス", U"ユーザー", U"123456789");
    database.write_data(data);
}

void Main()
{
    App manager; // シーンマネージャーを作成
    manager.add<Login>(U"Login"); // ログインシーン（名前は U"Login"）を登録
    manager.add<CreatePassword>(U"CreatePassword"); // パスワード作成シーン（名前は U"CreatePassword"）を登録
    manager.add<MainScene>(U"MainScene"); // メインシーン（名前は U"MainScene"）を登録
    manager.setFadeColor(Palette::Skyblue); // フェードイン・フェードアウト時の画面の色
    Window::SetStyle(WindowStyle::Sizable);
    Graphics::SetTargetFrameRateHz(60);

    //kowerkoint_dbg();
    while(System::Update()){
        if(!manager.update()) break; // 現在のシーンを実行
    }
}
