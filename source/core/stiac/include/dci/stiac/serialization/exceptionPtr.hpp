/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "../exceptionSerializersRegistry.hpp"
#include <dci/primitives.hpp>
#include <dci/exception.hpp>
#include <dci/bytes.hpp>
#include <dci/stiac/link/sink.hpp>
#include <dci/stiac/link/source.hpp>
#include <dci/idl/introspection.hpp>

namespace dci::stiac::serialization
{
    namespace details
    {
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template <class=void>
        Bytes saveFields(const dci::Exception* v, link::Hub4Sink* context)
        {
            auto saver = exceptionSerializersRegistry.getSaver(v->eid());
            dbgAssert(saver);
            if(saver)
            {
                Bytes data;
                link::Sink sink{context, data.begin(), false};
                saver(sink, v);
                return data;
            }

            return {};
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template <class=void>
        void loadFields(dci::Exception* v, Bytes&& data, link::Hub4Source* context)
        {
            auto loader = exceptionSerializersRegistry.getLoader(v->eid());
            dbgAssert(loader);
            if(loader)
            {
                link::Source source{context, data.begin(), false};
                loader(source, v);
            }
        }

        enum class ExceptionType : uint8
        {
            null                    = 0,
            unknown                 = 1,

            unknown_std_exception   = 10,
            std_logic_error         = 11,
            std_runtime_error       = 12,

            dci_exception           = 100,
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void save(auto& ar, const ExceptionPtr& v)
    {
        ExceptionPtr fail;

        ExceptionPtr ptr = v;
        while(ptr)
        {
            try
            {
                try
                {
                    std::rethrow_exception(ptr);
                }
                catch(const std::logic_error& e)
                {
                    ar << details::ExceptionType::std_logic_error;
                    ar << std::string(e.what());
                    std::rethrow_if_nested(e);
                }
                catch(const std::runtime_error& e)
                {
                    ar << details::ExceptionType::std_runtime_error;
                    ar << std::string(e.what());
                    std::rethrow_if_nested(e);
                }
                catch(const dci::Exception& e)
                {
                    ar << details::ExceptionType::dci_exception;
                    ar << e.eid();

                    try
                    {
                        ar << details::saveFields(&e, ar.context());
                    }
                    catch(...)
                    {
                        fail = std::current_exception();
                        break;
                    }

                    std::rethrow_if_nested(e);
                }
                catch(const std::exception& e)
                {
                    ar << details::ExceptionType::unknown_std_exception;
                    ar << std::string(e.what());
                    std::rethrow_if_nested(e);
                }
                catch(const std::nested_exception& e)
                {
                    ar << details::ExceptionType::unknown;
                    std::rethrow_exception(e.nested_ptr());
                }
                catch(...)
                {
                    ar << details::ExceptionType::unknown;
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

        if(fail)
        {
            std::rethrow_exception(fail);
        }

        //terminator
        ar << details::ExceptionType::null;
    }

    namespace details
    {
        class UnknownException
        {
        };

        class UnknownStdException
            : public std::exception
        {
        public:
            UnknownStdException(const UnknownStdException&) = default;
            UnknownStdException(UnknownStdException&&) = default;

            UnknownStdException& operator=(const UnknownStdException&) = default;
            UnknownStdException& operator=(UnknownStdException&&) = default;

        public:
            UnknownStdException(std::string&& what)
                : _what(std::move(what))
            {
            }

            const char* what() const noexcept override
            {
                return _what.c_str();
            }

        private:
            std::string _what;
        };

        class UnknownDciException
            : public dci::exception::Skeleton<UnknownDciException, dci::Exception>
        {
        public:
            using dci::exception::Skeleton<UnknownDciException, dci::Exception>::Skeleton;

        public:
            static constexpr Eid _eid {0x71,0x4c,0xd4,0x89,0x37,0x78,0x4a,0x09,0x8f,0xca,0xe8,0xd9,0xd7,0x88,0x45,0xee};
        };

        void loadExceptionsStack(auto& ar, ExceptionPtr& fail)
        {
            details::ExceptionType et;
            ar >> et;

            switch(et)
            {
            case details::ExceptionType::null:
                break;

            case details::ExceptionType::unknown:
                {
                    try         { loadExceptionsStack(ar, fail); if(fail) return; }
                    catch (...) { std::throw_with_nested(UnknownException()); }

                    throw UnknownException();
                }
                break;

            case details::ExceptionType::std_logic_error:
                {
                    std::string what;
                    ar >> what;

                    try         { loadExceptionsStack(ar, fail); if(fail) return; }
                    catch (...) { std::throw_with_nested(std::logic_error(std::move(what))); }

                    throw std::logic_error(std::move(what));
                }
                break;

            case details::ExceptionType::std_runtime_error:
                {
                    std::string what;
                    ar >> what;

                    try         { loadExceptionsStack(ar, fail); if(fail) return; }
                    catch (...) { std::throw_with_nested(std::runtime_error(std::move(what))); }

                    throw std::runtime_error(std::move(what));
                }
                break;

            case details::ExceptionType::unknown_std_exception:
                {
                    std::string what;
                    ar >> what;

                    try         { loadExceptionsStack(ar, fail); if(fail) return; }
                    catch (...) { std::throw_with_nested(UnknownStdException(std::move(what))); }

                    throw UnknownStdException(std::move(what));
                }
                break;

            case details::ExceptionType::dci_exception:
                {
                    Eid eid;
                    ar >> eid;

                    Bytes fieldsData;

                    try
                    {
                        ar >> fieldsData;
                    }
                    catch(...)
                    {
                        fail = std::current_exception();
                        return;
                    }

                    ExceptionPtr cause;
                    try         { loadExceptionsStack(ar, fail); if(fail) return; }
                    catch (...) { cause = std::current_exception(); }

                    ExceptionPtr ptr = dci::exception::buildInstance(eid, cause);

                    if(ptr)
                    {
                        try
                        {
                            std::rethrow_exception(ptr);
                        }
                        catch(dci::Exception& e)
                        {
                            details::loadFields(&e, std::move(fieldsData), ar.context());
                        }

                        std::rethrow_exception(ptr);
                    }

                    UnknownDciException e;
                    details::loadFields(&e, std::move(fieldsData), ar.context());

                    if(cause)
                    {
                        try         { std::rethrow_exception(cause); }
                        catch (...) { std::throw_with_nested(e); }
                    }

                    throw e;
                }

            default:
                ar.fail("malformed input stream (corrupted data)");
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void load(auto& ar, ExceptionPtr& v)
    {
        ExceptionPtr fail;

        try
        {
            details::loadExceptionsStack(ar, fail);

            if(!fail)
            {
                if(v)
                {
                    v = ExceptionPtr();
                }
            }
        }
        catch(...)
        {
            v = std::current_exception();
        }

        if(fail)
        {
            std::rethrow_exception(fail);
        }
    }
}

#include <dci/stiac/link/sink.ipp>
#include <dci/stiac/link/source.ipp>
