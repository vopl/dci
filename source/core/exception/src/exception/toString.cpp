// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/exception/toString.hpp>
#include <dci/exception.hpp>
#include <stdexcept>

namespace dci::exception
{
    std::string API_DCI_EXCEPTION toString(std::exception_ptr ptr)
    {
        std::string res;
        while(ptr)
        {
            if(!res.empty())
                res += ", caused by ";

            try
            {
                try
                {
                    std::rethrow_exception(ptr);
                }
                catch(const std::logic_error& e)
                {
                    res = res + "logic_error{" + e.what() + "}";
                    std::rethrow_if_nested(e);
                }
                catch(const std::runtime_error& e)
                {
                    res = res + "runtime_error{" + e.what() + "}";
                    std::rethrow_if_nested(e);
                }
                catch(const dci::Exception& e)
                {
                    res = res + std::string{e.name()} + "{" + e.what() + "}";
                    std::rethrow_if_nested(e);
                }
                catch(const std::exception& e)
                {
                    res = res + "exception{" + e.what() + "}";
                    std::rethrow_if_nested(e);
                }
                catch(const std::nested_exception& e)
                {
                    res = res + "unknown exception";
                    std::rethrow_exception(e.nested_ptr());
                }
                catch(...)
                {
                    res = res + "unknown exception";
                }

                //no nesteds more
                ptr = std::exception_ptr();
            }
            catch(...)
            {
                //nested
                ptr = std::current_exception();
            }
        }

        return res;
    }

    std::string API_DCI_EXCEPTION currentToString()
    {
        return toString(std::current_exception());
    }
}
