#pragma once

#include "asio.hpp"
#include "json/json.h"

using asio::ip::tcp;

namespace BitsaversSearch {
    class Updater {
        private:
        asio::io_context ioContext;
        tcp::resolver tcpResolver;
        tcp::socket tcpSocket;

        Updater();

        void Update();
        Json::Value GetIndicesAsJson();
        std::string GetWebTextFileAsString(const std::string&, const std::string&);
        
        public:
        Updater(Updater const&) = delete;
        void operator=(Updater const&) = delete;

        void UpdateIfNeeded();

        static Updater& GetInstance();
    };
}