/*
作者：aaaa0ggMC                 仅在bilibili
更新时间:2020/8/9
文件作用：运行游戏
*/
#define WINVER 0x1008611        //可以使用更多的windows.h中的函数，在winMusicPlayer.h中包含了windows.h
#include <SFML/Graphics.hpp>    //支持Sprite,RenderedWindow,Texture...的外部库
#include <string>               //支持string的C++库
#include <stdio.h>              //支持printf,fread,fwrite...等IO的C库
#include <vector>               //支持vector的C++库
#include <iostream>             //支持cout的C++库
#include <stdlib.h>             //支持system(const char *)的C库
#include <time.h>               //C库，用来指示时间
#include "winMusicPlayer.h"     //可以播放WAV的mmsystem音乐播放器
#include <fstream>
#include "main.h"
#include <stdint.h>
#include <ctype.h>
#include <pthread.h>

using namespace std;            //C++标准库的命名空间
using namespace sf;             //SFML的命名空间

struct HelpWindow{
    HelpWindow(Window * to);
    void displayWindow();
    void m_crt();
    RenderWindow window;
    thread newOpen;
    Window * to;
};

string uppercase(string tar);

void * thread_run(void *);

Color alpha = Color((Uint8)0,(Uint8)100,(Uint8)100);//图片的通道，为灰色
const string gameTitle = "Five Nights At Freddy's\t(FANF)";//游戏标题
const int maxfr = 4,backFr = 256,dllFr = 4;//最多的主题数，最多的雪花帧
Font f1;//字体{f1:像素字体1;}
int nfr = 1,nbkfr = 0,ndllFr = 0;//当前的主题，当前的雪花帧
string frstr = "sources/fr"/*图片导入的模板字符串*/,theme = "Freddy"/*主题*/,templateThem = "author:aaaa0ggMC In Bilibili\ntoggle theme!\nTheme:"/*主题的模板*/;
const int tsize = 2;
Texture icon;
char buf;//读写
int st;//Style

int main()
{
    //ShowWindow(GetConsoleWindow(),SW_HIDE);//隐藏窗口
    srand(time(0));//初始化随机数种子
    f1.loadFromFile("sources/font1.TTF");//导入字体
    ///region 声明与初始化变量///

        ///region 其他信息///
        int frame = 0,finishFrame = 0;//游戏帧
        const int mfinishFrame = 500;
        bool loadProcess = true/*载入图片是否成功,true表示载入成功，false反之*/,isShine = false;
        bool chicaEatTheMouse = false;
        bool light = false;
        bool ShowIll = false,dwill = false;
        bool openDoor = false;//门是否打开
        bool lostfocus = false;
        bool shake = true;
        bool exitBar = false;
        bool finish = false;
        bool startKey = false;
        bool bonnieGo = false;
        bool drawBonnie = false;
        bool shark = false;
        double energy = 100.000;
        Clock clock;//钟表
        Color shader;
        ifstream reader;
        MusicPlayer player("sources/head.wav",player.Async | player.Loop);//声明音乐播放器，见winMusicPlayer.h
        CPlayer ballon("sources/ballon.wav");//("sources/ballon.wav",player.Sync,true);
        CPlayer bonnieSound;
        ///open cplayer///
        ///end///
        string errMsg = "can't load "/*载入失败的错误信息*/,allTheme = templateThem + theme/*主题信息*/;
        vector<Sprite> snows;
        IntRect egg,egg2,egg3;
        int windowDLL = 10;
        VertexArray floorSp(sf::LinesStrip,3);
        VertexArray floorSp2(sf::Lines,2);
        VertexArray door(sf::Quads,4);
        VertexArray doorLine(sf::LinesStrip,4);
        ///end-region 其他信息///

        ///region 各种delay
        const Time mouseClickDelay = sf::milliseconds(500);
        Time shineOnLightDelay = sf::milliseconds(3200),
             shineOffLightDelay = sf::milliseconds(200),
             snowDelayC = sf::milliseconds(50),
             showTimeC = sf::seconds(60),
             spendTimeC = sf::seconds(6*60),
             keyDelayC = sf::seconds(8),
             bonnieTimeC = sf::seconds(16);
        Time clickDelay;///鼠标单击时的延迟
        Time shineDelay;///闪烁的灯光的延迟
        Time snowDelay;///雪花的频率
        Time showTime;///egg
        Time nSpendTime,odspTime;
        Time keyDelay;
        Time bonnieTime;
        ///end-region 各种delay

        ///region 声明文字///
        Text toggleTheme(allTheme,f1,16);
        Text startBtn("   NEW GAME",f1,32);
        Text exitBtn("   EXIT",f1,32);
        Text continBtn("   CONTINUE",f1,32);
        Text doorBtn("door",f1,16);
        Text helpBtn("help",f1,16);
        Text strTime("played:",f1,16);
        Text strEn("energy:",f1,16);
        Text finishText("05AM",f1);
        ///end-region 声明文字///

        ///region 声明贴图///
        Texture head;//开头的东西
        Texture snow;//雪花，用来在雪花屏，像素大小：3*3
        Texture bk;//闪屏的线
        Texture bed;//床
        Texture eggIll;//吊瓶，几率：千分之一，开头决定，随机时间显示（1/10000）
        Texture bonnie;//邦尼
        ///end-region 声明贴图///

        ///region 加载贴图///
        if(!head.loadFromFile("sources/fr1.png")){
            loadProcess = false;
            errMsg += "sources/fr1.png;";
        }
        if(!snow.loadFromFile("sources/snow.png")){
            loadProcess = false;
            errMsg += "sources/snow.png;";
        }
        if(!bk.loadFromFile("sources/back.png")){
            loadProcess = false;
            errMsg += "sources/back.png;";
        }
        if(!bed.loadFromFile("sources/bed.png")){
            loadProcess = false;
            errMsg += "sources/bed.png;";
        }
        if(!eggIll.loadFromFile("sources/bottle.png")){
            loadProcess = false;
            errMsg += "sources/bottle.png;";
        }
        if(!icon.loadFromFile("sources/icon.png")){
            loadProcess = false;
            errMsg += "sources/icon.png;";
        }
        if(!bonnie.loadFromFile("sources/bonnie.png")){
            loadProcess = false;
            errMsg += "sources/bonnie.png;";
        }
        ///end-region 加载贴图///

        ///region 检查是否load成功///
        if(!loadProcess){
            MessageBox(NULL,(LPCSTR)errMsg.c_str(),(LPCSTR)gameTitle.c_str(),MB_OK | MB_TOPMOST | MB_ICONERROR);
            ShowWindow(GetConsoleWindow(),SW_SHOW);//在IDE内
            ballon.Stop();
            ballon.Close();
            player.Stop();
            SetWindowText(GetConsoleWindow(),(LPCSTR)"Thanks for playing!!!!");
            printf("Thanks for playing!!!!\n");
            Sleep(100);
            return EXIT_FAILURE;
        }
        ///end-region 检查完毕///

        ///region 声明Sprite///
        Sprite sphead(head);
        Sprite spsnow(snow);
        Sprite spBlkSnow(snow);
        Sprite spbk(bk);
        Sprite spbed(bed);
        Sprite spill(eggIll);
        Sprite spBonnie(bonnie);
        ///end-region 声明结束///

        ///region设置Sprite///
        sphead.setPosition(0,0);
        sphead.setColor(alpha);
        spBlkSnow.setColor(sf::Color((Uint8)0,(Uint8)0,(Uint8)0));
        Color spbkColor = spbk.getColor();
        spbkColor.a = 64;
        spbk.setColor(spbkColor);
        spbed.setColor(Color((Uint8)0,(Uint8)64,(Uint8)64));
        spbed.scale(0.75,1);
        spbed.setPosition(320,0);
        spill.setPosition(320,35);
        spBonnie.setPosition(17,110);
        ///endregion 停止设置///

        ///region 文字的设置///
        toggleTheme.setPosition(2,2);
        startBtn.setPosition(100,200);
        exitBtn.setPosition(100,264);
        continBtn.setPosition(100,328);
        doorBtn.setColor(Color::Yellow);
        doorBtn.setPosition(100,70);
        helpBtn.setPosition(600,0);
        helpBtn.setColor(Color::White);
        strTime.setPosition(0,0);
        strEn.setPosition(10,395);
        finishText.setPosition((640 - 30*finishText.getString().toAnsiString().length())/2,100);
        ///end-region 文字的设置

    ///end-region 声明与初始化变量///

    ///初始化彩蛋///
    egg.left = 410;
    egg.top = 45;
    egg.height = 90;
    egg.width = 50;
    egg2.left = 36;
    egg2.top = 270;
    egg2.width = 60;
    egg2.height = 80;
    egg3.left = 520;
    egg3.top = 180;
    egg3.width = 50;
    egg3.height = 40;
    ///结束///

    ///初始化floor///
    for(int i = 0;i < (int)floorSp.getVertexCount();i++){
        floorSp[i].color = sf::Color::Black;
    }
    floorSp[0].position = Vector2f(0,420);
    floorSp[1].position = Vector2f(90,160);
    floorSp[2].position = Vector2f(640,160);
    floorSp2[0].position = Vector2f(90,160);
    floorSp2[1].position = Vector2f(90,-10);
    floorSp2[0].color = sf::Color::Black;
    floorSp2[1].color = sf::Color::Black;
    door[0].position = Vector2f(76,200);
    door[1].position = Vector2f(76,0);
    door[2].position = Vector2f(17,110);
    door[3].position = Vector2f(17,370);
    doorLine[0].position = Vector2f(76,200);
    doorLine[1].position = Vector2f(76,0);
    doorLine[2].position = Vector2f(17,110);
    doorLine[3].position = Vector2f(17,370);
    for(int i = 0;i < (int)doorLine.getVertexCount();i++)doorLine[i].color = Color::Black;
    ///结束///

    ///文件配置///
    OP:
    reader.open("gamedata.gmd",ios::in);
    if(!reader.is_open()){
        reader.close();
        ofstream of;
        string default_ = "shakeWindow:true;\ntitleBar:true;\n";
        of.open("gamedata.gmd",ios::out);
        of.write(default_.c_str(),default_.length());
        of.close();
        goto OP;
    }
    reader.close();
    CeData data;

    if(!data.load("gamedata.gmd")){
        cout << "解析码错误！！！" << endl;
        ShowWindow(GetConsoleWindow(),SW_SHOW);
        Sleep(1000);
        return EXIT_FAILURE;
    }
    for(const auto i : data._exportData()){
        if(i.m_name.compare("shakeWindow") == 0){
            if(uppercase(i.m_value).compare("TRUE") == 0)shake = true;
            else if(uppercase(i.m_value).compare("FALSE") == 0)shake = false;
            else{
                cout << "不支持的值" << i.m_name <<"：" << i.m_value << endl;
                ShowWindow(GetConsoleWindow(),SW_SHOW);
                Sleep(1000);
                return EXIT_FAILURE;
            }
        }else if(i.m_name.compare("titleBar") == 0){
            if(uppercase(i.m_value).compare("TRUE") == 0)st = Style::Titlebar | Style::Close;
            else if(uppercase(i.m_value).compare("FALSE") == 0)st = Style::None;
            else{
                cout << "不支持的值" << i.m_name <<"：" << i.m_value << endl;
                ShowWindow(GetConsoleWindow(),SW_SHOW);
                Sleep(1000);
                return EXIT_FAILURE;
            }
        }
    }

    /*reader.read(&buf,1);

    if(buf == 'n')st = Style::None;
    else st = Style::Titlebar | Style::Close;

    reader.read(&buf,1);
    if(buf == 'n')shake = false;*/
    ///结束///

    player.Play();

    // Create the main window
    sf::RenderWindow app(sf::VideoMode(640, 420), gameTitle,st);
    HelpWindow window(&app);

    string decode = "";

    app.setIcon(icon.getSize().x,icon.getSize().y,icon.copyToImage().getPixelsPtr());

    app.setFramerateLimit(64);
	// Start the game loop
    while (app.isOpen())
    {
        Vector2i pos = Mouse::getPosition(app);
        sf::Event event;
        Time delay = clock.restart();
        if(startKey)keyDelay += delay;
        while (app.pollEvent(event))
        {
            // Close window : exit
            if (event.type == sf::Event::Closed){
                app.close();
                goto EXTMEM;
            }else if(event.type == Event::KeyReleased){
                if(event.key.code == Keyboard::F){
                    if(frame == 1){
                        light = !light;
                    }
                }else if(event.key.code == sf::Keyboard::Escape){
                    if(frame == 1){
                        exitBar = !exitBar;
                    }
                }else if(event.key.code == Keyboard::A){
                    decode += 'A';
                    if(!startKey)startKey = true;
                }else if(event.key.code == Keyboard::Num0){
                    decode += '0';
                    if(!startKey)startKey = true;
                }else if(event.key.code == Keyboard::M){
                    decode += 'M';
                    if(!startKey)startKey = true;
                }else if(event.key.code == Keyboard::C){
                    decode += 'C';
                    if(!startKey)startKey = true;
                }else if(event.key.code == Keyboard::G){
                    decode += 'G';
                    if(!startKey)startKey = true;
                }
                if(decode.length() > 9 || decode.compare("AAAA0GGMC") == 0 || keyDelay >= keyDelayC){
                    keyDelay = sf::seconds(0);
                    startKey = false;
                    cout << decode << endl;
                    if(decode.compare("AAAA0GGMC") == 0){
                        nSpendTime = sf::seconds(355);
                    }
                    decode = "";
                }
            }else if(event.type == Event::LostFocus){
                lostfocus = true;
            }else if(event.type == Event::GainedFocus){
                lostfocus = false;
            }
        }

        if(lostfocus){
            Sleep(250);
            continue;
        }

        clickDelay += delay;
        // Process events

        if(Mouse::isButtonPressed(Mouse::Left) && clickDelay >= mouseClickDelay){
            clickDelay = sf::microseconds(0);
            cout << Mouse::getPosition(app).x << " " << Mouse::getPosition(app).y << endl;
            if(nfr == 3){
                if(egg.contains(pos.x,pos.y)){
                    MessageBox(NULL,(LPCSTR)"你找到了个彩蛋！！！",(LPCSTR)gameTitle.c_str(),MB_OK | MB_TOPMOST);
                    isShine = true;
                    alpha = Color((Uint8)200,(Uint8)200,(Uint8)200);
                    sphead = Sprite(head);
                    sphead.setColor(alpha);
                    shineDelay = sf::milliseconds(0);
                    shineOffLightDelay = sf::seconds(2.5);
                }
            }else if(nfr == 2){
                if(egg2.contains(pos.x,pos.y)){
                    ballon.Stop();
                    ballon.Close();
                    ballon.Open((LPCSTR)"sources/ballon.wav");
                    ballon.SetVolume(1000);
                    ballon.SetPos(0);
                    ballon.Play();
                }
            }else if(nfr == 1){
                if(egg3.contains(pos.x,pos.y)){
                    chicaEatTheMouse = !chicaEatTheMouse;
                }
            }
            switch(frame){
            case 0:
                if(toggleTheme.getGlobalBounds().contains(pos.x,pos.y)){
                    nfr++;
                    if(nfr >= maxfr+1)
                        nfr = 1;
                    errMsg = frstr + to_string(nfr) + ".png";
                    if(!head.loadFromFile(errMsg)){
                        loadProcess = false;
                        errMsg += ";";
                    }
                    if(!loadProcess){
                        MessageBox(NULL,(LPCSTR)errMsg.c_str(),(LPCSTR)gameTitle.c_str(),MB_OK | MB_TOPMOST | MB_ICONERROR);
                        exit(1);
                    }
                    sphead = Sprite(head);
                    sphead.setColor(alpha);
                    switch(nfr){
                    case 1:
                        theme = "Freddy";
                        break;
                    case 2:
                        theme = "Sister Location";
                        break;
                    case 3:
                        theme = "FANF World";
                        break;
                    case 4:
                        theme = "Freddy's big \"Family\"";
                        break;
                    }
                    allTheme = templateThem + theme;
                    toggleTheme.setString(allTheme);
                }else if(startBtn.getGlobalBounds().contains(pos.x,pos.y)){
                    frame++;
                    if(rand()%1000 > 998)ShowIll = true;
                    //if(1)ShowIll = true;
                    player.Stop();
                }else if(exitBtn.getGlobalBounds().contains(pos.x,pos.y)){
                    app.close();
                    goto EXTMEM;
                }else if(continBtn.getGlobalBounds().contains(pos.x,pos.y)){
                    MessageBox(NULL,(LPCSTR)"对不起！现在仅支持新的游戏，不支持读档！！！",(LPCSTR)gameTitle.c_str(),MB_OK | MB_TOPMOST | MB_ICONERROR);
                }else if(helpBtn.getGlobalBounds().contains(pos.x,pos.y)){
                    window.displayWindow();
                    //MessageBox(NULL,(LPCSTR)"对不起！现在仅支持帮助，不支持读档！！！",(LPCSTR)gameTitle.c_str(),MB_OK | MB_TOPMOST | MB_ICONERROR);
                }
                break;
            case 1:
                if(doorBtn.getGlobalBounds().contains(pos.x,pos.y)){
                    openDoor = !openDoor;
                }else if(exitBar && exitBtn.getGlobalBounds().contains(pos.x,pos.y)){
                    frame = 0;
                    exitBar = false;
                    continue;
                }
                break;
            }
        }

        if(exitBar){
            if(exitBtn.getGlobalBounds().contains(pos.x,pos.y)){
                exitBtn.setString(">>>EXIT");
            }else{
                exitBtn.setString("   EXIT");
            }
            app.clear(Color::Black);
            app.draw(exitBtn);
            app.display();
            continue;
        }

        if(finish){
            finishFrame++;
            if(finishFrame > mfinishFrame){
                finishFrame = 0;
                finish = false;
                frame = 0;
                player.Play();
                continue;
            }
            if(finishFrame == 200){
                app.clear(Color::White);
                app.display();
                Sleep(160);
                finishText.setString("06AM!");
                finishText.setPosition((640-30*finishText.getString().toAnsiString().length())/2,100);
                continue;
            }else
                app.clear(Color::Black);
            if(finishFrame > 200 && finishFrame < 456){
                Text ending("Next night is a new night!!!!",f1,16+(int)((finishFrame-200)/10));
                ending.setPosition((640-(16+(int)((finishFrame-200)/10))/2*ending.getString().toAnsiString().length())/2,160);
                finishText.setColor(Color((Uint8)255,(Uint8)255,(Uint8)255,456 - finishFrame));
                ending.setColor(Color((Uint8)255,(Uint8)255,(Uint8)255,finishFrame-200));
                app.draw(ending);
            }
            app.draw(finishText);
            app.display();
            continue;
        }

        if(rand()%10000 > 9998 && ShowIll){dwill = true;ShowIll = false;}

        //if(rand()%2 > 0 && ShowIll){dwill = true;ShowIll = false;cout << "HM" << endl;}

        if(dwill){
            showTime += delay;
            if(showTime > showTimeC){showTime = sf::seconds(0);ShowIll = true;dwill = false;}
        }

        snowDelay += delay;
        if(snowDelay >= snowDelayC){
            snowDelay = sf::seconds(0);
            if(frame == 0){
                snows.clear();
                for(int i = 0;i < 380;i++){
                    sf::Sprite s = (rand()%2 == 0)?spsnow:spBlkSnow;
                    s.setPosition((rand() % app.getSize().x),(rand() % app.getSize().y));
                    snows.push_back(s);
                    if(rand() % 10 > 8){
                        for(int i2 = 0;i2 < 10;i2++){
                            if((rand() % 10 > 4)){
                                i++;
                                sf::Sprite s2 = (rand()%2 == 0)?spsnow:spBlkSnow;
                                s2.setPosition(s.getPosition().x+(rand()%3+1),s.getPosition().y+(rand()%2+1));
                                snows.push_back(s2);
                            }
                        }
                    }
                }
            }
        }

        shineDelay += delay;
        if(!isShine && shineDelay >= shineOnLightDelay){
            shineDelay = sf::microseconds(0);
            if(frame == 0){
                isShine = true;
                alpha = Color((Uint8)200,(Uint8)200,(Uint8)200);
                sphead = Sprite(head);
                sphead.setColor(alpha);
                int t = 0;
                if((rand() % 10000) > 9990){
                    t = rand() % 701 + 2500;
                }else{
                    t = rand() % 30001 + 200;
                }
                shineOnLightDelay = sf::milliseconds(t);
            }
        }else if(isShine && shineDelay >= shineOffLightDelay){
            shineDelay = sf::microseconds(0);
            if(frame == 0){
                isShine = false;
                alpha = Color((Uint8)100,(Uint8)100,(Uint8)100);
                sphead = Sprite(head);
                sphead.setColor(alpha);
                int t = 0;
                if((rand() % 10000) > 9998){
                    t = rand() % 200 + 1001;
                }else{
                    t = rand() % 200 + 1;
                }
                shineOffLightDelay = sf::milliseconds(t);
            }
        }

        if(frame == 0){
            if(startBtn.getGlobalBounds().contains(pos.x,pos.y)){
                startBtn.setString(">>>NEW GAME");
                exitBtn.setString("   EXIT");
                continBtn.setString("   CONTINUE");
            }else if(exitBtn.getGlobalBounds().contains(pos.x,pos.y)){
                startBtn.setString("   NEW GAME");
                exitBtn.setString(">>>EXIT");
                continBtn.setString("   CONTINUE");
            }else if(continBtn.getGlobalBounds().contains(pos.x,pos.y)){
                startBtn.setString("   NEW GAME");
                exitBtn.setString("   EXIT");
                continBtn.setString(">>>CONTINUE");
            }else{
                startBtn.setString("   NEW GAME");
                exitBtn.setString("   EXIT");
                continBtn.setString("   CONTINUE");
            }
        }

        // Clear screen
        app.clear(shader);

        switch(frame){
        case 0:
            if(nbkfr >= backFr || spbk.getPosition().y > app.getSize().y || rand() % 11 > 9){
                nbkfr = 0;
                spbk.setPosition(0,rand() % app.getSize().y+1);
            }else{
                nbkfr++;
                spbk.move(0,4);
            }
            if(shake){
                if(ndllFr > dllFr){
                    ndllFr = 0;
                    app.setPosition(Vector2i(app.getPosition().x+(windowDLL == 10 ? 5 : -5),
                                    app.getPosition().y+(windowDLL == 10 ? 4 : -4)));
                    windowDLL = -windowDLL;
                }else{
                    ndllFr++;
                }
            }
            if(chicaEatTheMouse){
                Mouse::setPosition(Vector2i(545,200),app);
            }
            app.draw(sphead);
            app.draw(toggleTheme);
            app.draw(startBtn);
            app.draw(exitBtn);
            app.draw(continBtn);
            app.draw(spbk);
            app.draw(helpBtn);
            for(int i = 0;i < (int)snows.size();i++){
                app.draw(snows[i]);
            }
            break;
        case 1:
            string timem = "";
            nSpendTime += delay;
            if(nSpendTime > spendTimeC){nSpendTime = sf::seconds(0);finish = true;}
            if(nSpendTime.asSeconds() < 60)timem = "12PM";
            else timem = "0" + to_string((int)(nSpendTime.asSeconds()/60)) + "AM";
            if(nSpendTime.asSeconds() >= odspTime.asSeconds()+2){
                odspTime = nSpendTime;
                energy -= 0.1 * (light?2.5:1) * (openDoor?1:2);
            }
            if((int)nSpendTime.asSeconds() == 45 && !bonnieGo){
                bonnieGo = true;
                ballon.Close();
                cout <<"Play Bonnie:" << bonnieSound.Open("sources/leg.wav") << endl;
                bonnieSound.Play();
            }
            if(bonnieGo){
                if(bonnieSound.GetLength(MCI_STATUS_POSITION) >= bonnieSound.GetLength(MCI_STATUS_LENGTH)){
                    bonnieSound.Stop();
                    bonnieGo = false;
                    drawBonnie = true;
                }
            }
            strTime.setString("TIME=" + timem);
            strEn.setString("ENERGY=" + to_string((int)energy));
            if(light){
                Uint8 shaderColor = (Uint8)(rand()%20+236);
                shader = Color(shaderColor,shaderColor,shaderColor);
                strTime.setColor(Color::Black);
                strEn.setColor(Color::Black);
            }
            else {
                shader = Color((Uint8)0,(Uint8)64,(Uint8)64);
                strTime.setColor(Color::White);
                strEn.setColor(Color::White);
            }
            for(int i = 0;i < (int)door.getVertexCount();i++){
                if(!openDoor)
                    door[i].color = shader;
                else
                    door[i].color = Color::Black;
            }
            spbed.setColor(shader);
            app.draw(floorSp);
            app.draw(floorSp2);
            app.draw(door);
            app.draw(doorLine);
            app.draw(strTime);
            app.draw(strEn);
            if(drawBonnie){
                bonnieTime += delay;
                if(bonnieTime > bonnieTimeC){
                    drawBonnie = false;
                    if(openDoor)shark = true;
                }
                spBonnie.setColor(Color::Cyan);
                if(openDoor)
                    app.draw(spBonnie);
            }
            if(shark){bonnieSound.Close();ballon.Open("sources/ballon.wav");ballon.Play();player.Stop();player.Play();frame=0;}
            ///////////////////////////
            if(dwill){spill.setColor(shader);app.draw(spill);}
            app.draw(spbed);
            app.draw(doorBtn);
            break;
        }

        // Update the window
        app.display();
    }
    EXTMEM:
    ShowWindow(GetConsoleWindow(),SW_SHOW);
    ballon.Stop();
    ballon.Close();
    player.Stop();
    SetWindowText(GetConsoleWindow(),(LPCSTR)"Thanks for playing!!!!");
    printf("Thanks for playing!!!!\n");
    Sleep(100);
    return EXIT_SUCCESS;
}

HelpWindow::HelpWindow(Window * to){
    this->to = to;
}

void HelpWindow::displayWindow(){
    this->to->setVisible(false);
    newOpen = static_cast<thread>([&]{m_crt();});
    newOpen.detach();
}

void HelpWindow::m_crt(){
    window.create(sf::VideoMode(to->getSize().x,to->getSize().y),"Help Window",st);
    window.setPosition(Vector2i(to->getPosition().x + 10,to->getPosition().y + 10));
    window.setIcon(icon.getSize().x,icon.getSize().y,icon.copyToImage().getPixelsPtr());
    window.setVisible(true);
    string help = "1.Press \"F\" to toggle the light.\n"//\t按下F来切换光源\n"
                  "2.Click a yellow text \"door\" to toggle the door.\n"//"\t点击一个黄色的字\"door\"来切换门的开关\n"
                  "3.there was 4 eggs in the game.(three eggs are on start page,\n    the other egg is very hard to get,1/10000000)\n"
                  "4.Press \"ESC\" to pause the game!";//"\t有四个彩蛋（三个在启动页，另外一个十分难获得，几率:1/10000000）\n";
    sf::Text sayings(help,f1,16);

    sf::Text exitBtn("exit",f1,16);
    exitBtn.setPosition(600,0);
    sayings.setPosition(10,10);
    while(window.isOpen()){
        sf::Event event;
        while(window.pollEvent(event)){
            if(event.type == Event::Closed){
                MessageBox(NULL,"close me to wait...","FNAF",MB_OK | MB_TOPMOST);
                this->to->setVisible(true);
                Sleep(200);
                window.close();
            }
        }
        if(Mouse::isButtonPressed(Mouse::Left)){
            Vector2i pos = Mouse::getPosition(window);
            if(exitBtn.getGlobalBounds().contains(pos.x,pos.y)){
                MessageBox(NULL,"close me to wait...","FNAF",MB_OK | MB_TOPMOST);
                this->to->setVisible(true);
                Sleep(200);
                window.close();
            }
        }
        window.clear(Color::Black);
        window.draw(sayings);
        window.draw(exitBtn);
        window.display();
    }
}

string Trim(string & str){
    string blanks("\f\v\r\t\n ");
    string temp;
    for(int i = 0;i < (int)str.length();i++){
        if(str[i] == '\0')
            str[i] = '\t';
        else if(blanks.find(str[i]) == string::npos)
            break;
    }
    str.erase(0,str.find_first_not_of(blanks));
    str.erase(str.find_last_not_of(blanks) + 1);
    temp = str;
    return temp;
}

void split(vector<string> & vct,const string & line,const char sep){
    const size_t size = line.size();
    const char* str = line.c_str();
    int start = 0,end = 0;
    for(int i = 0;i < (int)size;i++){
        if(str[i] == sep){
            string st = line.substr(start,end);
            Trim(st);
            vct.push_back(st);
            start = i + 1;
            end = 0;
        }else
            end++;
    }
    if(end > 0){
        string st = line.substr(start,end);
        Trim(st);
        vct.push_back(st);
    }
}

bool CeData::load(LPCSTR fileName){
    FILE * file;
    char ch;
    int fc = 0;
    vector<string> temp;
    if((file = fopen(fileName,"r")) == NULL){
        printf("加载一个叫%s的文件失败!!!\n",fileName);
        return false;
    }
    while((ch = getc(file)) != EOF){
        m_file += ch;
        if(ch == ';')fc++;
    }
    if(fclose(file) != 0){
        printf("关闭一个叫%s的文件失败!!!\n",fileName);
        return false;
    }
    if(fc == 0)return false;
    split(m_temp,m_file,';');
    for(int i = 0;i < (int)m_temp.size();i++){
        Trim(m_temp[i]);
    }
    for(const auto i : m_temp){
        if(i.compare("") == 0){
            continue;
        }
        temp.clear();
        split(temp,i,':');
        if(!addItem(CeObject(temp[0],temp[1]))){
            cout << "--------------------------" << endl;
            cout << "变量重名错误!!!!" << endl;
            cout << i << endl;
            cout << "--------------------------" << endl;
        }
    }
    return true;
}

bool CeData::load(string fileName){return load((LPCSTR)fileName.c_str());}

bool CeData::addItem(CeObject v){
    for(const CeObject i : m_data){
        if(i.m_name.compare(v.m_name) == 0)
            return false;
    }
    m_data.push_back(v);
    return true;
}

CeObject::CeObject(string name,string value)
:m_name(name)
,m_value(value){

}

vector<CeObject> CeData::_exportData(){
    vector<CeObject> vec;
    for(auto i : m_data){
        vec.push_back(i);
    }
    return vec;
}

string uppercase(string tar){
    string rt;
    for(auto i : tar){
        rt += toupper(i);
    }
    return rt;
}

void * thread_run(void *){
    return NULL;
}
