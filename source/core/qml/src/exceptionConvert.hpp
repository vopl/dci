// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include <dci/qml/error.hpp>

namespace dci::qml
{
    inline std::exception_ptr exceptionConvert(const QJSValue& v)
    {
        if(v.isQObject())
        {
            Error* error = qobject_cast<Error*>(v.toQObject());
            if(error)
            {
                std::exception_ptr cppException = error->cppException();

                if(cppException)
                {
                    return cppException;
                }
            }
        }

        return exception::buildInstance<dci::Exception>(v.toString().toStdString());
    }

    inline QJSValue exceptionConvert(QJSEngine* jse, std::exception_ptr e)
    {
        return jse->newQObject(new Error{e});
    }
}
