# include <Siv3D.hpp>
# include "db.hpp"

using App = SceneManager<String>;



// ログインシーン
class Login : public App::Scene
{
public:

    // コンストラクタ（必ず実装）
    Login(const InitData& init) : IScene(init)
    {

    }

    // 更新関数
    void update() override
    {
        if (SimpleGUI::Button(U"Login", Vec2(100, 100)))
        {
            if (/*ログイン関数*/1) {
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

struct PassUnit {
    String serviceName;
    String userName;
    String password;

    PassUnit(String _serviceName, String _userName, String _password) {
        serviceName = _serviceName;
        userName = _userName;
        password = _password;
    }
};
//パスワードの三項目をセットにした配列
Array<PassUnit> passArray;

// メインシーン
class MainScene : public App::Scene
{
public:
    MainScene(const InitData& init) : IScene(init)
    {

    }

    // 更新関数
    void update() override
    {
        Setup();
        ClearPrint();
        noticeTimer += Scene::DeltaTime();
        screenSize = Vec2(Window::ClientWidth() , Window::ClientHeight());
        scroll.max = floor((screenSize.y - 100) / 50);

        //■



        if (Rect(50, 50, screenSize.x - 100, screenSize.y - 110).drawFrame(10, Design::frame).draw(Design::inFrame).mouseOver() && popupState == notPopup) {
            scroll.wheel += Mouse::Wheel();
            if (scroll.wheel < 0) scroll.wheel = 0;
            if (scroll.wheel > passArray.size() - 1) scroll.wheel = passArray.size() - 1;

            scroll.current = floor(scroll.wheel);
        }

        FontAsset(U"Regular")(U"サービス名").draw(70, 10, Design::fontColor);
        FontAsset(U"Regular")(U"ユーザー名").draw(ratioPos(0.35,0).x, 10, Design::fontColor);
        FontAsset(U"Regular")(U"パスワード").draw(ratioPos(0.6,0).x, 10, Design::fontColor);

        if (TextureAsset(isVisiblePass ? U"visible" : U"invisible").resized(30).draw(Arg::center(ratioPos(0.6, 0).x + 130, isVisiblePass ? 25 : 26)).mouseOver() && popupState == notPopup) {
            Cursor::RequestStyle(CursorStyle::Hand);
            if (MouseL.down()) {
                isVisiblePass ^= true;
                //■パスワードの可視/不可視処理

            }
        }

        if (SimpleGUI::Button(U"追加", Vec2(screenSize.x - 130, 5))) {
            popupState = forAdd;
        }


        for (int i = 0; i < scroll.max; i++) {
            if (scroll.current + i >= passArray.size()) break;

            int height = 50 + 50 * i;

            FontAsset(U"Regular")(passArray[scroll.current + i].serviceName).draw(70, height, Design::fontColor);
            FontAsset(U"Regular")(passArray[scroll.current + i].userName).draw(ratioPos(0.35, 0).x, height, Design::fontColor);
            FontAsset(U"Regular")(passArray[scroll.current + i].password).draw(ratioPos(0.6, 0).x, height, Design::fontColor);


            //Array passArrayのインデックスは scroll.current + i

            if (TextureAsset(U"copy").resized(30).draw(screenSize.x - 160, height).mouseOver() && popupState == notPopup) {
                Cursor::RequestStyle(CursorStyle::Hand);
                if (MouseL.down()) {
                    //■パスワードのコピー処理


                    //コピーできたら以下をする
                    noticeType = notice_copy;
                    noticeTimer = 0.0;
                }
            }
            if (TextureAsset(U"edit").resized(30).draw(screenSize.x - 120, height).mouseOver() && popupState == notPopup) {
                Cursor::RequestStyle(CursorStyle::Hand);
                if (MouseL.down()) {
                    popupIndex = scroll.current + i;
                    popupState = forEdit;
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
            
            
            if (popupState != confirming) {
                FontAsset(U"Regular")(popupState == forAdd ? U"パスワードの追加" : U"パスワードの変更").draw(ratioPos(0.5,0.3), Design::fontColor);
                if (SimpleGUI::Button(U"決定", ratioPos(0.5,0.5))) {
                    lastPopupState = popupState;
                    popupState = confirming;
                }
                if (SimpleGUI::Button(U"キャンセル", ratioPos(0.5,0.65))) {
                    popupState = notPopup;
                }
                
            }
            else {
                //再確認
                RectF(ratioPos(0.5,0.3),ratioPos(0.33,0.4)).drawFrame(5, Design::frame);
                FontAsset(U"Regular")(U"この内容で\n確定しますか？").draw(ratioPos(0.57,0.38), Design::fontColor);
                if (SimpleGUI::Button(U"はい", ratioPos(0.54, 0.58),80)) {
                    //■パスワードの追加・変更処理
                    //追加：lastPopupState == forAdd のとき
                    //変更：lastPopupState == forEdit のとき
                    //      Array passArrayのインデックスは popupIndex



                    popupState = notPopup;

                    //追加・変更できたら以下をする
                    if(lastPopupState == forAdd) noticeType = notice_add;
                    if(lastPopupState == forEdit) noticeType = notice_edit;
                    noticeTimer = 0.0;
                }
                if (SimpleGUI::Button(U"いいえ", ratioPos(0.7,0.58),80)) {
                    popupState = lastPopupState;
                }

            }
            break;

        case forDelete:

            RectF(Arg::center(screenSize / 2), 300, 200).draw(Design::deletePopupBG);
            FontAsset(U"Regular")(U"本当に削除しますか？").draw(Arg::center(screenSize.x * 0.5 , screenSize.y * 0.45), Design::deletePopupString);

            if (SimpleGUI::Button(U"はい", ratioPos(0.5, 0.55) - Vec2(100,0), 80)) {
                //■パスワードの削除処理
                //Array passArrayのインデックスは popupIndex



                popupState = notPopup;
                //削除できたら以下をする
                noticeType = notice_delete;
                noticeTimer = 0.0;
            }
            if (SimpleGUI::Button(U"いいえ", ratioPos(0.5,0.55) + Vec2(20,00), 80)) {
                popupState = notPopup;
            }
            break;
        default: break;
        }
        
        int copyNoticeX = (2.5 - abs(noticeTimer - 2.5)) * 300;
        if (copyNoticeX > 200) copyNoticeX = 200;
        Rect(screenSize.x - copyNoticeX, screenSize.y -50, 200, 50).draw(Design::fontColor);
        String noticeMessage;
        switch (noticeType) {
        case notice_copy:  noticeMessage = U"コピーしました。"; break;
        case notice_delete:noticeMessage = U"削除しました。"; break;
        case notice_add:  noticeMessage = U"追加しました。"; break;
        case notice_edit:noticeMessage = U"変更しました。"; break;
        }
        FontAsset(U"Regular")(noticeMessage).draw(screenSize.x + 20 - copyNoticeX, screenSize.y - 40, Design::background);

    }


    /// <summary>パスワードの欄の表示を切り替えるための変数</summary>
    static inline bool isVisiblePass = false;


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

    static inline int popupIndex = -1;

    void draw() const override
    {

    }

    static inline bool isDoneSetup = false;
    /// <summary>最初に一度だけ実行する</summary>
    void Setup() {
        if (isDoneSetup) return;
        isDoneSetup = true;

        Graphics::SetTargetFrameRateHz(30);
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

        


        for (int i = 0; i < 20; i++) {
            passArray << PassUnit(U"serviceName{}"_fmt(i), U"userName{}"_fmt(i), U"password{}"_fmt(i));
        }


    }


    //デザイン用定数
    struct Design {
        static inline Color inFrame = Palette::Whitesmoke;
        static inline Color frame = Palette::Lightgrey;
        static inline Color background = Palette::White;
        static inline Color fontColor = Palette::Black;

        static inline Color deletePopupBG = Palette::Red;
        static inline Color deletePopupString = Palette::Yellow;
        

    };
    Vec2 screenSize;
    
    Vec2 ratioPos(double x, double y) {
        return Vec2(screenSize.x * x, screenSize.y * y);
    }

    //スクロール用変数
    struct {
        int current = 0;
        int max = 10;

        double wheel = 0.0;
    }scroll;

    //テキストボックス用
    TextEditState serviceNameText;
    TextEditState userNameText;
    TextEditState passwordText;

    //完了通知用
    double noticeTimer = 5.0;
    enum {
        notice_copy,
        notice_delete,
        notice_add,
        notice_edit,
    }noticeType;

};

void kowerkoint_dbg() {
    db database;
    //database.register_passwd(U"vim");
    Print << database.login(U"vim");
    //Print << database.login(U"emacs");
    Array<single_data> data = database.read_data();
    //data << single_data(U"サービス名", U"ユーザー名", U"passwd");
    data << single_data(U"サービス名2", U"ユーザー名2", U"passwd2");
    database.write_data(data);
    data = database.read_data();
    for (int i = 0; i < data.size(); i++) {
        Print << data[i].service_name << data[i].user_name << data[i].password;
    }
    /*
    database.change_passwd(U"vim", U"emacs");
    data = database.read_data();
    for (int i = 0; i < data.size(); i++) {
        Print << data[i].service_name << data[i].user_name << data[i].password;
    }
    */
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
    manager.setFadeColor(Palette::Black);


    //kowerkoint_dbg();
    while(System::Update())
    {
        // 現在のシーンを実行
        if(!manager.update())
        {
            break;
        }
    }
}