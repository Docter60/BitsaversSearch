#include "Updater.h"

#include <ctime>
#include <fstream>
#include <iostream>
#include <unordered_map>

namespace BitsaversSearch
{
    static const std::time_t SECONDS_PER_DAY = 60 * 60 * 24;
    static const std::time_t UPDATE_FREQUENCY = SECONDS_PER_DAY;

    static const std::string BITSAVERS_URL_BASE = "www.bitsavers.org";
    static const std::string BITSAVERS_INDEX_FILENAME = "IndexByDate.txt";
    static const std::string UPDATE_TIME_FILENAME = "LastUpdated.txt";
    static const std::string ASSETS_DIRECTORY_NAME = "assets";
    static const std::string DATA_FILENAME = "data.json";
    static const std::string DATA_JSON_FILEPATH = ASSETS_DIRECTORY_NAME + "/" + DATA_FILENAME;

    static const std::string BITSAVERS_SECTION_SOFTWARE = "bits";
    static const std::string BITSAVERS_SECTION_COMPUTING = "pdf";
    static const std::string BITSAVERS_SECTION_COMMUNICATIONS = "communications";
    static const std::string BITSAVERS_SECTION_COMPONENTS = "components";
    static const std::string BITSAVERS_SECTION_MAGAZINE = "magazines";
    static const std::string BITSAVERS_SECTION_TEST_EQUIPMENT = "test_equipment";

    static const std::string BITSAVERS_URL_SOFTWARE_INDEX_FILE = "/" + BITSAVERS_SECTION_SOFTWARE + "/" + BITSAVERS_INDEX_FILENAME;
    static const std::string BITSAVERS_URL_COMPUTING_INDEX_FILE = "/" + BITSAVERS_SECTION_COMPUTING + "/" + BITSAVERS_INDEX_FILENAME;
    static const std::string BITSAVERS_URL_COMMUNICATIONS_INDEX_FILE = "/" + BITSAVERS_SECTION_COMMUNICATIONS + "/" + BITSAVERS_INDEX_FILENAME;
    static const std::string BITSAVERS_URL_COMPONENTS_INDEX_FILE = "/" + BITSAVERS_SECTION_COMPONENTS + "/" + BITSAVERS_INDEX_FILENAME;
    static const std::string BITSAVERS_URL_MAGAZINE_INDEX_FILE = "/" + BITSAVERS_SECTION_MAGAZINE + "/" + BITSAVERS_INDEX_FILENAME;
    static const std::string BITSAVERS_URL_TEST_EQUIPMENT_INDEX_FILE = "/" + BITSAVERS_SECTION_TEST_EQUIPMENT + "/" + BITSAVERS_INDEX_FILENAME;

    static const std::unordered_map<std::string, std::string> BITSAVERS_INDICES = {
        {BITSAVERS_SECTION_SOFTWARE, BITSAVERS_URL_SOFTWARE_INDEX_FILE},
        {BITSAVERS_SECTION_COMPUTING, BITSAVERS_URL_COMPUTING_INDEX_FILE},
        {BITSAVERS_SECTION_COMMUNICATIONS, BITSAVERS_URL_COMMUNICATIONS_INDEX_FILE},
        {BITSAVERS_SECTION_COMPONENTS, BITSAVERS_URL_COMPONENTS_INDEX_FILE},
        {BITSAVERS_SECTION_MAGAZINE, BITSAVERS_URL_MAGAZINE_INDEX_FILE},
        {BITSAVERS_SECTION_TEST_EQUIPMENT, BITSAVERS_URL_TEST_EQUIPMENT_INDEX_FILE},
    };

    static std::string GenerateGetRequest(const std::string &baseURL, const std::string &pathURL)
    {
        return "GET " + pathURL + " HTTP/1.0\r\n" +
               "Host: " + baseURL + "\r\n" +
               "Accept: */*\r\n" +
               "Connection: close\r\n\r\n";
    }

    static time_t GetCurrentTime()
    {
        return std::time(0);
    }

    static time_t GetLastTimeUpdated()
    {
        std::ifstream inFile(UPDATE_TIME_FILENAME);
        if (!inFile.good())
        {
            std::cerr << "Error opening "
                      << UPDATE_TIME_FILENAME
                      << " for reading."
                      << std::endl;
            return 0;
        }

        std::time_t luTime;
        inFile >> luTime;
        inFile.close();
        return luTime;
    }

    static void SetLastTimeUpdated()
    {
        std::ofstream outFile(UPDATE_TIME_FILENAME);
        if (!outFile.is_open())
        {
            std::cerr << "Error opening "
                      << UPDATE_TIME_FILENAME
                      << " for writing."
                      << std::endl;
            return;
        }
        time_t ct = GetCurrentTime();
        outFile << ct;
        outFile.close();
    }

    static time_t GetTimeSinceLastUpdate()
    {
        return GetCurrentTime() - GetLastTimeUpdated();
    }

    static bool DataFilePresent() {
        std::ifstream dataFile(DATA_JSON_FILEPATH);
        return dataFile.good();
    }

    static bool ApplicationShouldPerformUpdate()
    {
        return GetTimeSinceLastUpdate() > UPDATE_FREQUENCY || !DataFilePresent();
    }

    static Json::Value GenerateJsonSectionElement(const std::string &lineEntry)
    {
        Json::Value sectionElementArray;
        std::stringstream ss(lineEntry);
        std::string element;
        std::string elements[3];
        int i = 0;
        while (std::getline(ss, element, ' ') && i < 3)
        {
            elements[i] = element;
            ++i;
        }
        sectionElementArray[0] = elements[0] + ' ' + elements[1];
        size_t lastSlash = elements[2].find_last_of('/');
        if (lastSlash == std::string::npos)
        {
            sectionElementArray[1] = "";
            sectionElementArray[2] = elements[2];
        }
        else
        {
            sectionElementArray[1] = elements[2].substr(0, lastSlash);
            sectionElementArray[2] = elements[2].substr(lastSlash + 1);
        }
        return sectionElementArray;
    }

    static Json::Value GenerateJsonSection(const std::string &rawSectionIndex)
    {
        Json::Value sectionArray;
        std::stringstream ss(rawSectionIndex);
        std::string line;
        int i = 0;
        while (std::getline(ss, line, '\n'))
        {
            sectionArray[i] = GenerateJsonSectionElement(line);
            ++i;
        }
        return sectionArray;
    }

    Updater::Updater() : ioContext(), tcpResolver(ioContext), tcpSocket(ioContext) {}

    void Updater::UpdateIfNeeded()
    {
        if (!ApplicationShouldPerformUpdate())
            return;

        Update();
    }

    void Updater::Update()
    {
        std::ofstream outFile(DATA_JSON_FILEPATH);
        if (!outFile.is_open())
        {
            std::cerr << "Error opening "
                      << DATA_JSON_FILEPATH
                      << " for writing."
                      << std::endl;
            return;
        }
        Json::FastWriter fw;
        std::string jsonString = fw.write(GetIndicesAsJson());
        outFile << jsonString;
        outFile.close();
        SetLastTimeUpdated();
    }

    Json::Value Updater::GetIndicesAsJson()
    {
        Json::Value root;
        for (const auto &pair : BITSAVERS_INDICES)
        {
            std::string rawSectionIndex = GetWebTextFileAsString(BITSAVERS_URL_BASE, pair.second);
            root[pair.first] = GenerateJsonSection(rawSectionIndex);
        }
        return root;
    }

    std::string Updater::GetWebTextFileAsString(const std::string &domain, const std::string &path)
    {
        try
        {
            tcp::resolver::results_type endpoints = tcpResolver.resolve(domain, "http");
            asio::connect(tcpSocket, endpoints);
            std::string httpString = GenerateGetRequest(domain, path);
            asio::streambuf sb;
            std::ostream rStream(&sb);
            rStream << httpString;
            asio::write(tcpSocket, sb);
            asio::streambuf response;
            asio::read_until(tcpSocket, response, "\r\n");

            std::istream responseStream(&response);
            std::string httpVersion;
            responseStream >> httpVersion;
            unsigned int statusCode;
            responseStream >> statusCode;
            std::string statusError;
            std::getline(responseStream, statusError);

            if (!responseStream || httpVersion.substr(0, 5) != "HTTP/")
            {
                std::cerr << "Invalid response" << std::endl;
                throw;
            }
            if (statusCode != 200)
            {
                std::cerr << "Response returned with status code "
                          << statusCode
                          << std::endl
                          << statusError
                          << std::endl;
                throw;
            }

            std::error_code error;
            std::stringstream ss;
            while (asio::read(tcpSocket, response, asio::transfer_at_least(1), error))
            {
                ss << &response;
            }
            if (error != asio::error::eof)
                std::cerr << "EOF not reached during http response capture" << std::endl;

            std::string responseString = ss.str();
            size_t split = responseString.find("\r\n\r\n") + 4;
            return responseString.substr(split);
        }
        catch (std::exception &e)
        {
            std::cerr << "Exception: "
                      << e.what()
                      << std::endl;
            return "";
        }
    }

    Updater &Updater::GetInstance()
    {
        static Updater instance;
        return instance;
    }
}