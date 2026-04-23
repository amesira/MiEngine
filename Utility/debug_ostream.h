#ifndef DEBUG_OSTREAM_H
#define DEBUG_OSTREAM_H

#include<Windows.h>
#include<sstream>

namespace hal {
    class debugbuf :public std::basic_stringbuf<char, std::char_traits<char>> {
        // std::basic_stringbuf<>を継承したクラス、debugbufを作成
    public :
        virtual ~debugbuf() {   // デストラクタ（オブジェクトが削除されるときに自動で呼び出す）
            sync();
        }
    protected:
        int sync();
    };
    class debug_ostream :public std::basic_ostream<char, std::char_traits<char>> {
    public:
        debug_ostream():std::basic_ostream<char,std::char_traits<char>>(new debugbuf()){}   // コントラクタ（オブジェクト生成時に呼び出し）
        ~debug_ostream() { delete rdbuf(); }    // デストラクタ
    };

    extern debug_ostream dout;
}

#endif