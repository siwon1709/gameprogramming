#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <ctime>
#include <chrono>
#include <conio.h>
#include <windows.h>

// ============================================================================
// [Upgrade 28] OOP 구조화: 사운드, 그래픽, 키패드 엔진, 앱 매니저 클래스 분리
// [Upgrade 30] 모던 C++ 적용: std::string, std::vector, chrono 기반의 메모리 안전성 확보
// ============================================================================

// ----------------------------------------------------------------------------
// 1. 사운드 관리자 (SoundManager)
// ----------------------------------------------------------------------------
class SoundManager {
private:
    bool isMuted; // [Upgrade 05] 사운드 On/Off 음소거 상태 토글 플래그

public:
    SoundManager() : isMuted(false) {}

    // [Upgrade 05] 음소거 토글
    void toggleMute() {
        isMuted = !isMuted;
    }

    bool getMuteStatus() const {
        return isMuted;
    }

    // [Upgrade 01] 키패드 터치 기본 효과음
    // [Upgrade 02] 버튼별 음계 매핑 (1~9번 키마다 도·레·미·파·솔·라·시·도 주파수 차등)
    void playKeyTone(char key) {
        if (isMuted) return;

        int frequencies[] = {
            262, // 0: 낮은 도 (C4)
            523, // 1: 도 (C5)
            587, // 2: 레 (D5)
            659, // 3: 미 (E5)
            698, // 4: 파 (F5)
            784, // 5: 솔 (G5)
            880, // 6: 라 (A5)
            988, // 7: 시 (B5)
            1047,// 8: 높은 도 (C6)
            1175 // 9: 높은 레 (D6)
        };

        if (key >= '0' && key <= '9') {
            Beep(frequencies[key - '0'], 60);
        } else if (key == '*' || key == '#') {
            Beep(1318, 60); // E6
        } else if (key == 'T' || key == 't') {
            // [Upgrade 33] 상용구 자동 완성 효과음 (화음 느낌의 2단음)
            Beep(880, 50);
            Beep(1175, 70);
        }
    }

    // [Upgrade 04] 오타 / 백스페이스 한계 시 에러 경고음
    void playErrorTone() {
        if (isMuted) return;
        Beep(180, 120);
    }

    // [Upgrade 31] 전체 삭제(Clear All) 리셋 전용 사운드
    void playClearAllTone() {
        if (isMuted) return;
        Beep(440, 60);
        Beep(330, 80);
        Beep(220, 100);
    }

    // [Upgrade 03] 전송 완료 및 미션 성공 팡파르 멜로디
    void playSuccessFanfare() {
        if (isMuted) return;
        Beep(523, 100); // 도
        Beep(659, 100); // 미
        Beep(784, 100); // 솔
        Beep(1047, 250);// 높은 도
    }

    // [Upgrade 11] 부팅 전용 레트로 멜로디
    void playBootJingle() {
        if (isMuted) return;
        Beep(659, 80);
        Beep(784, 80);
        Beep(1047, 150);
    }

    // [Upgrade 14] 전원 종료 멜로디
    void playShutdownJingle() {
        if (isMuted) return;
        Beep(1047, 100);
        Beep(784, 100);
        Beep(523, 200);
    }
};

// ----------------------------------------------------------------------------
// 2. 콘솔 그래픽 및 UI 관리자 (ConsoleRenderer)
// ----------------------------------------------------------------------------
class ConsoleRenderer {
private:
    HANDLE hConsole;

public:
    ConsoleRenderer() {
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        setCursorVisible(false);
    }

    void gotoxy(int x, int y) {
        COORD pos = { (SHORT)(x - 1), (SHORT)(y - 1) };
        SetConsoleCursorPosition(hConsole, pos);
    }

    // [Upgrade 06] 콘솔 텍스트 색상 커스텀 지원
    void setColor(WORD color) {
        SetConsoleTextAttribute(hConsole, color);
    }

    void setCursorVisible(bool visible) {
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(hConsole, &cursorInfo);
        cursorInfo.bVisible = visible;
        SetConsoleCursorInfo(hConsole, &cursorInfo);
    }

    // [Upgrade 13] 화면 전환 애니메이션 연출 (스크롤 전환 효과)
    void wipeScreen() {
        setColor(0x07);
        for (int y = 1; y <= 25; ++y) {
            gotoxy(1, y);
            std::cout << "                                                                                ";
            Sleep(5);
        }
    }

    // [Upgrade 07] 상단 상태바: 통신사 안테나 및 배터리 잔량 그래픽 표시
    void drawStatusBar(bool isMuted, const std::string& modeName) {
        gotoxy(3, 2);
        setColor(0x0B); // 밝은 시안
        std::cout << "[SKT] ";
        setColor(0x0A); // 녹색
        std::cout << "ANT:|||| ";
        setColor(0x0E); // 노란색
        std::cout << "BAT:[===] ";
        setColor(0x07);
        std::cout << "| 모드: " << modeName << " ";
        if (isMuted) {
            setColor(0x0C);
            std::cout << "[음소거]";
        } else {
            setColor(0x0A);
            std::cout << "[소리ON]";
        }
    }

    // [Upgrade 09] 전용 3줄 레트로 LCD 디스플레이 창 프레임 렌더링
    void drawLcdBox() {
        setColor(0x20); // 녹색 배경에 검은 글자 (레트로 흑백 LCD 느낌)
        gotoxy(3, 4);  std::cout << "+--------------------------------------+";
        gotoxy(3, 5);  std::cout << "|                                      |";
        gotoxy(3, 6);  std::cout << "|                                      |";
        gotoxy(3, 7);  std::cout << "|                                      |";
        gotoxy(3, 8);  std::cout << "+--------------------------------------+";
        setColor(0x07);
    }

    // [Upgrade 10] 깜빡이는 커서(_) 및 LCD 화면 텍스트 실시간 반영
    // [Upgrade 32] 80바이트 SMS 실시간 용량 카운터 및 LMS 초과 경고 표시
    void updateLcdText(const std::string& committedText, char pendingChar, bool cursorState) {
        std::string fullText = committedText;
        if (pendingChar != '\0') {
            fullText += pendingChar;
        }

        // [Upgrade 32] 바이트 수 계산 (영문/기호 1Byte 기준)
        int currentBytes = (int)fullText.length();
        gotoxy(24, 5);
        if (currentBytes > 80) {
            setColor(0x2C); // 초과 시 빨간색 강조 (LMS 전환 안내)
            std::cout << "[" << currentBytes << "/80B LMS]";
        } else {
            setColor(0x20);
            std::cout << "[" << currentBytes << "/80B SMS]";
        }

        // 최대 36자 표시 (가로폭 기준 스크롤)
        std::string displayText = fullText;
        if (displayText.length() > 36) {
            displayText = displayText.substr(displayText.length() - 36);
        }

        setColor(0x20); // LCD 녹색 배경
        gotoxy(5, 6);
        std::cout << displayText;
        if (cursorState) {
            std::cout << "_";
        } else {
            std::cout << " ";
        }

        int remaining = 36 - (int)displayText.length() - 1;
        for (int i = 0; i < remaining; ++i) std::cout << " ";
        setColor(0x07);
    }

    // [Upgrade 17] 글자 확정 타이머 바 (남은 시간 게이지 실시간 출력)
    void drawCommitTimerBar(double remainRatio) {
        gotoxy(3, 9);
        setColor(0x0E);
        std::cout << "글자 확정: [";
        int totalBars = 10;
        int activeBars = (int)(remainRatio * totalBars);
        for (int i = 0; i < totalBars; ++i) {
            if (i < activeBars) std::cout << "=";
            else std::cout << " ";
        }
        std::cout << "] ";
        setColor(0x07);
    }

    // [Upgrade 08] 현재 누른 키 실시간 하이라이트 (키패드 배경 반전 피드백)
    // [Upgrade 31] 상단 기능 소프트키 [C: 취소] [T: 상용구] UI 렌더링
    void drawPhoneKeypad(char highlightedKey = '\0') {
        // [Upgrade 31, 33] 상단 기능 소프트키 (취소 및 상용구 버튼)
        gotoxy(5, 11);
        if (highlightedKey == 'C' || highlightedKey == 'c') setColor(0xCF);
        else setColor(0x0B);
        std::cout << "[ C:전체삭제 ]";

        gotoxy(27, 11);
        if (highlightedKey == 'T' || highlightedKey == 't') setColor(0xCF);
        else setColor(0x0B);
        std::cout << "[ T:상용구추가 ]";

        struct KeyInfo {
            int x, y;
            char key;
            const char* label;
        };

        KeyInfo keys[] = {
            { 5,  13, '1', "1 .qz" }, { 17, 13, '2', "2 abc" }, { 29, 13, '3', "3 def" },
            { 5,  16, '4', "4 ghi" }, { 17, 16, '5', "5 jkl" }, { 29, 16, '6', "6 mno" },
            { 5,  19, '7', "7 prs" }, { 17, 19, '8', "8 tuv" }, { 29, 19, '9', "9 wxy" },
            { 5,  22, '*', "* 기호" }, { 17, 22, '0', "0 띄움" }, { 29, 22, '#', "# 변환" }
        };

        for (const auto& k : keys) {
            gotoxy(k.x, k.y);
            if (k.key == highlightedKey) {
                setColor(0xCF); // 빨간 배경 + 밝은 흰색 (하이라이트)
            } else {
                setColor(0x0F);
            }
            std::cout << "+----------+";
            gotoxy(k.x, k.y + 1);
            std::cout << "| " << k.label << "  |";
            gotoxy(k.x, k.y + 2);
            std::cout << "+----------+";
        }
        setColor(0x07);
    }

    // [Upgrade 11] 레트로 2G Anycall 스타일 부팅 로고 애니메이션
    void playBootAnimation() {
        system("cls");
        setColor(0x0B);
        std::cout << "\n\n";
        std::cout << "\t   =======================================\n";
        std::cout << "\t        RETRO 2G CELLULAR PHONE v2.0      \n";
        std::cout << "\t             Connecting to Network...     \n";
        std::cout << "\t   =======================================\n\n";
        setColor(0x0A);
        std::cout << "\t   Booting System: [";
        for (int i = 0; i < 20; ++i) {
            std::cout << "#";
            Sleep(35);
        }
        std::cout << "] OK!\n";
        Sleep(300);
    }

    // [Upgrade 14] 굿바이 파워 오프 애니메이션 연출
    void playShutdownAnimation() {
        system("cls");
        setColor(0x0C);
        std::cout << "\n\n\n\n";
        std::cout << "\t\t===============================\n";
        std::cout << "\t\t       POWERING OFF...         \n";
        std::cout << "\t\t      좋은 하루 되세요!        \n";
        std::cout << "\t\t===============================\n";
        Sleep(800);
        setColor(0x07);
    }
};

// ----------------------------------------------------------------------------
// 3. 키패드 문자 입력 엔진 (KeypadEngine)
// ----------------------------------------------------------------------------
class KeypadEngine {
public:
    enum InputMode { LOWER_CASE, UPPER_CASE, NUM_MODE }; // [Upgrade 16] 입력 모드 열거형

private:
    std::string keyMap[10];
    std::string symbolMap;
    int symbolIndex;

    // [Upgrade 33] 2G 상용구 템플릿 목록 및 인덱스
    std::vector<std::string> templatePhrases;
    int templateIndex;

public:
    InputMode currentMode;

    KeypadEngine() {
        keyMap[0] = " ";
        keyMap[1] = ".qz";
        keyMap[2] = "abc";
        keyMap[3] = "def";
        keyMap[4] = "ghi";
        keyMap[5] = "jkl";
        keyMap[6] = "mno";
        keyMap[7] = "prs";
        keyMap[8] = "tuv";
        keyMap[9] = "wxy";

        // [Upgrade 15] 특수문자 모음 매핑
        symbolMap = "!?,@#-_:;()";
        symbolIndex = 0;
        currentMode = LOWER_CASE;

        // [Upgrade 33] 상용구 프리셋 등록
        templatePhrases.push_back("Call me!");
        templatePhrases.push_back("Where r u?");
        templatePhrases.push_back("Busy now!");
        templatePhrases.push_back("On my way!");
        templatePhrases.push_back("CU soon!");
        templateIndex = 0;
    }

    // [Upgrade 16] 대소문자/숫자 모드 순환 토글
    void toggleMode() {
        if (currentMode == LOWER_CASE) currentMode = UPPER_CASE;
        else if (currentMode == UPPER_CASE) currentMode = NUM_MODE;
        else currentMode = LOWER_CASE;
    }

    std::string getModeName() const {
        if (currentMode == LOWER_CASE) return "영문(소)";
        if (currentMode == UPPER_CASE) return "영문(대)";
        return "숫자(123)";
    }

    char getCharacter(char key, int pressCount) {
        if (currentMode == NUM_MODE) {
            return key;
        }

        if (key >= '0' && key <= '9') {
            const std::string& chars = keyMap[key - '0'];
            char c = chars[pressCount % chars.length()];
            if (currentMode == UPPER_CASE && c >= 'a' && c <= 'z') {
                c = c - ('a' - 'A'); // [Upgrade 16] 대문자 변환
            }
            return c;
        }
        return '\0';
    }

    // [Upgrade 15] 특수문자 순환 취득
    char getNextSymbol() {
        char s = symbolMap[symbolIndex];
        symbolIndex = (symbolIndex + 1) % symbolMap.length();
        return s;
    }

    // [Upgrade 33] 상용구 순환 취득
    std::string getNextTemplate() {
        std::string phrase = templatePhrases[templateIndex];
        templateIndex = (templateIndex + 1) % templatePhrases.size();
        return phrase;
    }
};

// ----------------------------------------------------------------------------
// 4. 레트로 폰 메인 애플리케이션 (RetroPhoneApp)
// ----------------------------------------------------------------------------
class RetroPhoneApp {
private:
    SoundManager sound;
    ConsoleRenderer renderer;
    KeypadEngine engine;

public:
    void run() {
        sound.playBootJingle(); // [Upgrade 11]
        renderer.playBootAnimation();

        // [Upgrade 12] 다채로운 메인 메뉴 시스템
        int choice = -1;
        while (choice != 0) {
            system("cls");
            renderer.setColor(0x0E);
            std::cout << "\n\t====================================\n";
            std::cout << "\t     RETRO 2G PHONE MAIN MENU      \n";
            std::cout << "\t====================================\n";
            renderer.setColor(0x0F);
            std::cout << "\t  1. 자유 문자 메시지 작성 (SMS)\n";
            std::cout << "\t  2. 2G 키패드 타자 속도 검정 게임\n";
            std::cout << "\t  3. 보낸 문자 보관함 확인\n";
            std::cout << "\t  4. 타자 챌린지 랭킹 (Top 5)\n";
            std::cout << "\t  5. 사운드 On/Off 토글 (현재: " << (sound.getMuteStatus() ? "MUTE" : "ON") << ")\n";
            std::cout << "\t  0. 전원 끄기 (Power Off)\n";
            std::cout << "\t====================================\n";
            std::cout << "\t메뉴를 선택하세요 > ";

            char ch = _getch();
            choice = ch - '0';

            switch (choice) {
                case 1: composeSmsMode(); break;
                case 2: runTypingGameMode(); break;
                case 3: viewOutboxMessages(); break;
                case 4: viewTopRankings(); break;
                case 5:
                    sound.toggleMute(); // [Upgrade 05]
                    break;
                case 0:
                    sound.playShutdownJingle(); // [Upgrade 14]
                    renderer.playShutdownAnimation();
                    break;
                default:
                    sound.playErrorTone(); // [Upgrade 04]
                    break;
            }
        }
    }

private:
    // [Upgrade 26] 보낸 문자 메시지 타임스탬프와 함께 파일 영구 저장
    void saveMessageToFile(const std::string& message) {
        std::ofstream outFile("sent_messages.txt", std::ios::app);
        if (outFile.is_open()) {
            time_t now = time(nullptr);
            tm* ltm = localtime(&now);
            char timeBuf[64];
            strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", ltm);

            outFile << "[" << timeBuf << "] " << message << "\n";
            outFile.close();
        }
    }

    // [Upgrade 26] 보관함 조회 기능
    void viewOutboxMessages() {
        renderer.wipeScreen();
        renderer.gotoxy(3, 2);
        renderer.setColor(0x0B);
        std::cout << "=== [보낸 문자 메시지 보관함] ===";
        renderer.setColor(0x07);

        std::ifstream inFile("sent_messages.txt");
        int lineCount = 4;
        if (!inFile.is_open()) {
            renderer.gotoxy(3, lineCount);
            std::cout << "저장된 문자 메시지가 없습니다.";
        } else {
            std::string line;
            while (std::getline(inFile, line) && lineCount < 22) {
                renderer.gotoxy(3, lineCount++);
                std::cout << line;
            }
            inFile.close();
        }

        renderer.gotoxy(3, 24);
        renderer.setColor(0x0E);
        std::cout << "아무 키나 누르면 메뉴로 돌아갑니다...";
        _getch();
    }

    // ------------------------------------------------------------------------
    // 모드 1: 자유 문자 메시지 작성 모드
    // ------------------------------------------------------------------------
    void composeSmsMode() {
        renderer.wipeScreen();
        std::string text = "";
        char lastKey = '\0';
        char pendingChar = '\0';
        int pressCount = 0;
        auto lastPressTime = std::chrono::steady_clock::now();
        bool cursorBlink = true;
        auto lastBlinkTime = std::chrono::steady_clock::now();

        renderer.drawStatusBar(sound.getMuteStatus(), engine.getModeName());
        renderer.drawLcdBox();
        renderer.drawPhoneKeypad();

        renderer.gotoxy(3, 25);
        renderer.setColor(0x07);
        std::cout << "[단축키] C:전체삭제 | T:상용구 | BS:한글자삭제 | Enter:전송 | ESC:메뉴";

        while (true) {
            auto now = std::chrono::steady_clock::now();

            // [Upgrade 10] 0.4초 주기로 커서 깜빡임
            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastBlinkTime).count() > 400) {
                cursorBlink = !cursorBlink;
                lastBlinkTime = now;
                renderer.updateLcdText(text, pendingChar, cursorBlink);
            }

            // [Upgrade 17] 글자 입력 1.5초 타이머 바 갱신 및 자동 확정
            if (pendingChar != '\0') {
                double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastPressTime).count();
                double remainRatio = 1.0 - (elapsed / 1500.0);
                if (remainRatio < 0.0) remainRatio = 0.0;
                renderer.drawCommitTimerBar(remainRatio);

                if (elapsed >= 1500.0) {
                    text += pendingChar; // 1.5초 초과 시 자동 확정
                    pendingChar = '\0';
                    lastKey = '\0';
                    pressCount = 0;
                    renderer.drawCommitTimerBar(0.0);
                    renderer.updateLcdText(text, pendingChar, cursorBlink);
                }
            }

            // 키보드 비동기 입력 처리
            if (_kbhit()) {
                char ch = _getch();

                if (ch == 27) { // ESC: 작성 취소
                    break;
                }

                // [Upgrade 31] C 키: 전체 화면 내용 초기화 (Clear All)
                if (ch == 'c' || ch == 'C') {
                    text.clear();
                    pendingChar = '\0';
                    lastKey = '\0';
                    pressCount = 0;
                    renderer.drawCommitTimerBar(0.0);
                    sound.playClearAllTone();
                    renderer.drawPhoneKeypad('C');
                    Sleep(100);
                    renderer.drawPhoneKeypad();
                    renderer.updateLcdText(text, pendingChar, cursorBlink);
                    continue;
                }

                // [Upgrade 33] T 키: 상용구(빠른 답장 매크로) 원터치 삽입
                if (ch == 't' || ch == 'T') {
                    if (pendingChar != '\0') {
                        text += pendingChar;
                        pendingChar = '\0';
                    }
                    if (!text.empty() && text.back() != ' ') {
                        text += " ";
                    }
                    text += engine.getNextTemplate();
                    lastKey = '\0';
                    pressCount = 0;
                    renderer.drawCommitTimerBar(0.0);
                    sound.playKeyTone('T');
                    renderer.drawPhoneKeypad('T');
                    Sleep(100);
                    renderer.drawPhoneKeypad();
                    renderer.updateLcdText(text, pendingChar, cursorBlink);
                    continue;
                }

                // [Upgrade 18] 즉시 확정 키 (방향키 우측 '->' 처리)
                if (ch == -32 || ch == 0) {
                    char arrow = _getch();
                    if (arrow == 77) { // Right Arrow
                        if (pendingChar != '\0') {
                            text += pendingChar;
                            pendingChar = '\0';
                            lastKey = '\0';
                            pressCount = 0;
                            renderer.drawCommitTimerBar(0.0);
                            sound.playKeyTone('0');
                        }
                        continue;
                    }
                }

                // [Upgrade 19] 백스페이스(Backspace) 삭제 기능
                if (ch == 8) {
                    if (pendingChar != '\0') {
                        pendingChar = '\0';
                        lastKey = '\0';
                        pressCount = 0;
                        renderer.drawCommitTimerBar(0.0);
                    } else if (!text.empty()) {
                        text.pop_back();
                    } else {
                        sound.playErrorTone(); // [Upgrade 04] 더 지울 게 없음
                    }
                    renderer.updateLcdText(text, pendingChar, cursorBlink);
                    continue;
                }

                // [Upgrade 20] 엔터 키 누를 시 메시지 전송 및 저장
                if (ch == 13) {
                    if (pendingChar != '\0') {
                        text += pendingChar;
                        pendingChar = '\0';
                    }
                    if (!text.empty()) {
                        saveMessageToFile(text); // [Upgrade 26]
                        sound.playSuccessFanfare(); // [Upgrade 03]
                        renderer.gotoxy(3, 9);
                        renderer.setColor(0x0A);
                        std::cout << ">> 메시지가 성공적으로 전송/저장되었습니다!       ";
                        Sleep(1200);
                    }
                    break;
                }

                // [Upgrade 16] # 키: 입력 모드 순환
                if (ch == '#') {
                    if (pendingChar != '\0') {
                        text += pendingChar;
                        pendingChar = '\0';
                    }
                    engine.toggleMode();
                    sound.playKeyTone('#');
                    renderer.drawStatusBar(sound.getMuteStatus(), engine.getModeName());
                    renderer.drawPhoneKeypad('#'); // [Upgrade 08]
                    Sleep(70);
                    renderer.drawPhoneKeypad();
                    continue;
                }

                // [Upgrade 15] * 키: 특수 기호 입력
                if (ch == '*') {
                    if (pendingChar != '\0') {
                        text += pendingChar;
                    }
                    pendingChar = engine.getNextSymbol();
                    lastKey = '*';
                    lastPressTime = std::chrono::steady_clock::now();
                    sound.playKeyTone('*');
                    renderer.drawPhoneKeypad('*'); // [Upgrade 08]
                    Sleep(70);
                    renderer.drawPhoneKeypad();
                    renderer.updateLcdText(text, pendingChar, cursorBlink);
                    continue;
                }

                // [Upgrade 20] 0번 키 또는 스페이스 키로 공백 입력
                if (ch == ' ' || ch == '0') {
                    if (pendingChar != '\0') {
                        text += pendingChar;
                        pendingChar = '\0';
                    }
                    text += ' ';
                    lastKey = '\0';
                    pressCount = 0;
                    sound.playKeyTone('0');
                    renderer.drawPhoneKeypad('0'); // [Upgrade 08]
                    Sleep(70);
                    renderer.drawPhoneKeypad();
                    renderer.updateLcdText(text, pendingChar, cursorBlink);
                    continue;
                }

                // [Upgrade 29] 유효한 숫자 키패드 1~9 입력 검증
                if (ch >= '1' && ch <= '9') {
                    sound.playKeyTone(ch); // [Upgrade 01, 02]
                    renderer.drawPhoneKeypad(ch); // [Upgrade 08]

                    if (ch == lastKey && pendingChar != '\0') {
                        pressCount++;
                    } else {
                        if (pendingChar != '\0') {
                            text += pendingChar;
                        }
                        pressCount = 0;
                    }

                    pendingChar = engine.getCharacter(ch, pressCount);
                    lastKey = ch;
                    lastPressTime = std::chrono::steady_clock::now();

                    Sleep(70);
                    renderer.drawPhoneKeypad();
                    renderer.updateLcdText(text, pendingChar, cursorBlink);
                } else {
                    sound.playErrorTone(); // [Upgrade 04]
                }
            }
            Sleep(15);
        }
    }

    // ------------------------------------------------------------------------
    // 모드 2: [Upgrade 21] 2G 키패드 타자 검정 게임 모드
    // ------------------------------------------------------------------------
    void runTypingGameMode() {
        renderer.wipeScreen();
        std::vector<std::string> quizList = {
            "hello", "retro", "cyon", "phone", "apple", "smart"
        };
        srand((unsigned int)time(nullptr));
        std::string targetWord = quizList[rand() % quizList.size()];

        renderer.drawStatusBar(sound.getMuteStatus(), "타자 검정");
        renderer.drawPhoneKeypad();

        renderer.gotoxy(3, 4);
        renderer.setColor(0x0E);
        std::cout << "=== 2G 타자 챌린지: 제시어를 키패드로 정확히 입력하세요! ===";
        renderer.gotoxy(3, 6);
        renderer.setColor(0x0F);
        std::cout << "제시어: [ " << targetWord << " ]";

        renderer.gotoxy(3, 8);
        std::cout << "입력창: ";
        renderer.setColor(0x0A);

        std::string userWord = "";
        char lastKey = '\0';
        char pendingChar = '\0';
        int pressCount = 0;
        int combo = 0; // [Upgrade 24] 콤보 카운터
        int totalKeyStrokes = 0;

        auto startTime = std::chrono::steady_clock::now();
        auto lastPressTime = std::chrono::steady_clock::now();

        while (true) {
            auto now = std::chrono::steady_clock::now();

            if (pendingChar != '\0') {
                double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastPressTime).count();
                if (elapsed >= 1200.0) {
                    userWord += pendingChar;
                    pendingChar = '\0';
                    lastKey = '\0';
                    pressCount = 0;
                }
            }

            renderer.gotoxy(11, 8);
            renderer.setColor(0x0A);
            std::cout << userWord << (pendingChar != '\0' ? std::string(1, pendingChar) : "") << "       ";

            // [Upgrade 24] 콤보 표시
            renderer.gotoxy(35, 6);
            renderer.setColor(0x0D);
            std::cout << "COMBO: x" << combo << " ";

            if (userWord.length() >= targetWord.length()) {
                break;
            }

            if (_kbhit()) {
                char ch = _getch();
                totalKeyStrokes++;

                if (ch == 27) return; // ESC 중도 포기

                if (ch == -32 || ch == 0) {
                    char arrow = _getch();
                    if (arrow == 77 && pendingChar != '\0') {
                        userWord += pendingChar;
                        pendingChar = '\0';
                        lastKey = '\0';
                        pressCount = 0;
                        combo++;
                    }
                    continue;
                }

                if (ch >= '1' && ch <= '9') {
                    sound.playKeyTone(ch);
                    renderer.drawPhoneKeypad(ch);

                    if (ch == lastKey && pendingChar != '\0') {
                        pressCount++;
                    } else {
                        if (pendingChar != '\0') {
                            userWord += pendingChar;
                            combo++;
                        }
                        pressCount = 0;
                    }

                    pendingChar = engine.getCharacter(ch, pressCount);
                    lastKey = ch;
                    lastPressTime = std::chrono::steady_clock::now();

                    Sleep(60);
                    renderer.drawPhoneKeypad();
                }
            }
            Sleep(20);
        }

        auto endTime = std::chrono::steady_clock::now();
        double totalSeconds = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() / 1000.0;
        if (totalSeconds < 0.5) totalSeconds = 0.5;

        // [Upgrade 22] 타자 속도 CPM / WPM 계산
        int cpm = (int)((userWord.length() / totalSeconds) * 60.0);

        // [Upgrade 23] 정확도(%) 계산
        int correctChars = 0;
        for (size_t i = 0; i < targetWord.length() && i < userWord.length(); ++i) {
            if (targetWord[i] == userWord[i]) correctChars++;
        }
        double accuracy = ((double)correctChars / targetWord.length()) * 100.0;

        // [Upgrade 25] 결과 요약 대시보드 및 등급(S, A, B, C) 산출
        std::string grade = "C";
        if (accuracy >= 100.0 && cpm >= 80) grade = "S (레트로 타자 마스터)";
        else if (accuracy >= 80.0) grade = "A (우수)";
        else if (accuracy >= 50.0) grade = "B (보통)";

        sound.playSuccessFanfare(); // [Upgrade 03]
        renderer.gotoxy(3, 10);
        renderer.setColor(0x0E);
        std::cout << "================== [검정 결과 대시보드] ==================";
        renderer.gotoxy(3, 12); std::cout << "제시어 / 입력값 : " << targetWord << " / " << userWord;
        renderer.gotoxy(3, 13); std::cout << "소요 시간       : " << totalSeconds << " 초";
        renderer.gotoxy(3, 14); std::cout << "분당 타수 (CPM) : " << cpm << " 타";
        renderer.gotoxy(3, 15); std::cout << "정확도 (Acc)    : " << accuracy << " %";
        renderer.gotoxy(3, 16); std::cout << "최종 평가 등급  : " << grade;
        renderer.gotoxy(3, 17); std::cout << "==========================================================";

        // [Upgrade 27] 랭킹 파일 기록
        saveScoreToFile(cpm, accuracy);

        renderer.gotoxy(3, 19);
        renderer.setColor(0x0A);
        std::cout << "기록이 저장되었습니다. 아무 키나 누르면 메뉴로 이동합니다...";
        _getch();
    }

    // [Upgrade 27] 랭킹 파일 영구 저장 및 Top 5 조회
    void saveScoreToFile(int cpm, double acc) {
        std::ofstream outFile("ranking_records.txt", std::ios::app);
        if (outFile.is_open()) {
            outFile << cpm << " " << acc << "\n";
            outFile.close();
        }
    }

    void viewTopRankings() {
        renderer.wipeScreen();
        renderer.gotoxy(3, 2);
        renderer.setColor(0x0E);
        std::cout << "=== [2G 타자 검정 역대 랭킹 기록] ===";
        renderer.setColor(0x07);

        std::ifstream inFile("ranking_records.txt");
        if (!inFile.is_open()) {
            renderer.gotoxy(3, 4);
            std::cout << "저장된 랭킹 기록이 없습니다.";
        } else {
            int cpm;
            double acc;
            int rank = 1;
            int yPos = 4;
            while (inFile >> cpm >> acc && rank <= 5) {
                renderer.gotoxy(3, yPos++);
                std::cout << rank << "위 | 타수: " << cpm << " CPM | 정확도: " << acc << "%";
                rank++;
            }
            inFile.close();
        }

        renderer.gotoxy(3, 22);
        renderer.setColor(0x0B);
        std::cout << "아무 키나 누르면 메뉴로 돌아갑니다...";
        _getch();
    }
};

// ----------------------------------------------------------------------------
// 프로그램 시작점
// ----------------------------------------------------------------------------
int main() {
    SetConsoleTitleA("2G Cellular Retro Phone Simulator (33 Upgrades Full Version)");

    RetroPhoneApp app;
    app.run();

    return 0;
}
