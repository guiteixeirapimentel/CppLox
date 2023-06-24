#pragma once
#include <functional>
#include <string>

namespace pimentel
{
    class ErrorManager
    {
    public:
        ~ErrorManager() = default;

        static ErrorManager& get();

        bool hasError() const;
        void resetError();
        void report(int line, const std::string& where, const std::string& message);
        void report(int line, const std::string& message);

    private:
        ErrorManager() = default;

    private:
        bool m_hasError = false;
    };
}