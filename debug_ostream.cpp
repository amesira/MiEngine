#include"debug_ostream.h"

#include "engine_service_locator.h"

namespace hal {
    debug_ostream dout;

    int debugbuf::sync() {
        std::string message = str();

        // 貯めていた出力を、出力ウィンドウへ
        OutputDebugStringA(message.c_str());

        // Debug View への出力も行う
        EngineServiceLocator::AddLogMessage(message);

        // バッファクリア
        str(std::basic_string<char>());

        return 0;
    }
}