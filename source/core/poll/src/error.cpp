// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/poll/error.hpp>

namespace dci::poll
{
    namespace
    {
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        const class ErrorCatogory
            : public std::error_category
        {
        public:
            ErrorCatogory()
                : std::error_category()
            {
            }

            const char* name() const noexcept override
            {
                return "dci.poll";
            }

            std::string message(int value) const override
            {
                switch(static_cast<error>(value))
                {
                case error::unknown:
                    return "unknown";
                case error::already_initialized:
                    return "already initialized";
                case error::already_started:
                    return "already started";
                case error::already_stopped:
                    return "already stopped";
                case error::no_engine_available:
                    return "no engine available";
                case error::not_initialized:
                    return "not initialized";
                case error::not_stopped:
                    return "not stopped";
                case error::bad_descriptor:
                    return "bad descriptor";
                case error::already_installed:
                    return "already installed";
                case error::not_installed:
                    return "not installed";
                }

                return "dci.poll.general error";
            }
        } errorCatogory;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const std::error_category& error_category()
    {
        return errorCatogory;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code make_error_code(error e)
    {
        return std::error_code(static_cast<int>(e), error_category());
    }
}
