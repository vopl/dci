/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "tls.hpp"
#include "tls/client/channel.hpp"
#include "tls/server/channel.hpp"
#include "tls/utils.hpp"

namespace dci::module::www
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Tls::Tls()
        : api::Tls<>::Opposite{idl::interface::Initializer{}}
        , _sslCtx{SSL_CTX_new(TLS_method()), &SSL_CTX_free}
    {
        methods()->setServername() += serviceSol() * [this](String&& servername)
        {
            _settings._servername = std::move(servername);

            if(!_settings._servername.empty())
            {
                dci::utils::dns::canonicalize(_settings._servername);

                SSL_CTX_set_tlsext_servername_callback(_sslCtx.get(), (int(*)(SSL*,int*, void*))[](SSL* ssl, int* /*ad*/, void* arg) -> int
                {
                    Tls* this_ = static_cast<Tls*>(arg);

                    const char *servername = SSL_get_servername(ssl, TLSEXT_NAMETYPE_host_name);
                    if (this_->_settings._servername.empty())
                        return SSL_TLSEXT_ERR_NOACK;

                    if (servername != NULL)
                    {
                        if (OPENSSL_strcasecmp(servername, this_->_settings._servername.c_str()))
                            return SSL_TLSEXT_ERR_ALERT_FATAL;
                    }
                    return SSL_TLSEXT_ERR_OK;

                });
                SSL_CTX_set_tlsext_servername_arg(_sslCtx.get(), this);
            }

            return cmt::readyFuture(None{});
        };

        methods()->setAlpnProtos() += serviceSol() * [this](const List<String>&& protos)
        {
            ERR_clear_error();
            std::size_t encodedLen{};
            for(const String& proto : protos)
                encodedLen += 1 + proto.size();
            _settings._encodedAlpnProtos.clear();
            _settings._encodedAlpnProtos.reserve(encodedLen);
            for(const String& proto : protos)
            {
                _settings._encodedAlpnProtos.push_back(proto.size());
                _settings._encodedAlpnProtos.insert(_settings._encodedAlpnProtos.end(), proto.begin(), proto.end());
            }

            if(encodedLen != _settings._encodedAlpnProtos.size())
            {
                _settings._encodedAlpnProtos = {};
                return cmt::readyFuture<None>(exception::buildInstance<api::tls::error::SetAlpnProtosFailed>("bad input"));
            }

            SSL_CTX_set_alpn_select_cb(_sslCtx.get(), [](SSL */*ssl*/,
                                       const unsigned char **out, unsigned char *outlen,
                                       const unsigned char *in, unsigned int inlen,
                                       void *arg)
            {
                Tls* this_ = static_cast<Tls*>(arg);

                int selectorRes = SSL_select_next_proto(
                            const_cast<unsigned char **>(out), outlen,
                            this_->_settings._encodedAlpnProtos.data(), this_->_settings._encodedAlpnProtos.size(),
                            in, inlen);

                return OPENSSL_NPN_NEGOTIATED == selectorRes ?
                            SSL_TLSEXT_ERR_OK :
                            SSL_TLSEXT_ERR_ALERT_FATAL;
            }, this);

            return cmt::readyFuture(None{});
        };

        methods()->setAuth() += serviceSol() * [this](const String& cert, const String& certPasswd, const String& key, const String& keyPasswd)
        {
            {
                ERR_clear_error();
                std::unique_ptr<BIO, int(*)(BIO*)> certBio{BIO_new_mem_buf(cert.data(), cert.size()), &BIO_free};

                std::unique_ptr<X509, void(*)(X509*)> x509{nullptr, &X509_free};
                x509.reset(PEM_read_bio_X509_AUX(certBio.get(), nullptr, nullptr, (void*)certPasswd.c_str()));
                if(!x509)
                {
                    unsigned long error = ERR_get_error();
                    return cmt::readyFuture<None>(exception::buildInstance<api::tls::error::ReadCertFailed>(tls::utils::errorString(error)));
                }

                int opResult = SSL_CTX_use_certificate(_sslCtx.get(), x509.get());
                if(1 != opResult)
                {
                    unsigned long error = ERR_get_error();
                    return cmt::readyFuture<None>(exception::buildInstance<api::tls::error::UseCertFailed>(tls::utils::errorString(error)));
                }

                opResult = SSL_CTX_clear_chain_certs(_sslCtx.get());
                if(1 != opResult)
                {
                    unsigned long error = ERR_get_error();
                    return cmt::readyFuture<None>(exception::buildInstance<api::tls::error::ClearChainFailed>(tls::utils::errorString(error)));
                }

                for(;;)
                {
                    x509.reset(PEM_read_bio_X509(certBio.get(), nullptr, nullptr, (void*)certPasswd.c_str()));
                    if(!x509)
                    {
                        unsigned long error = ERR_get_error();
                        if(!error || (ERR_GET_LIB(error) == ERR_LIB_PEM && ERR_GET_REASON(error) == PEM_R_NO_START_LINE))
                            break;

                        return cmt::readyFuture<None>(exception::buildInstance<api::tls::error::ReadCertFailed>(tls::utils::errorString(error)));
                    }
                    opResult = SSL_CTX_add_extra_chain_cert(_sslCtx.get(), x509.get());
                    if(1 != opResult)
                    {
                        unsigned long error = ERR_get_error();
                        return cmt::readyFuture<None>(exception::buildInstance<api::tls::error::AddCertChainFailed>(tls::utils::errorString(error)));
                    }
                }
            }

            {
                ERR_clear_error();
                std::unique_ptr<BIO, int(*)(BIO*)> keyBio{BIO_new_mem_buf(key.data(), key.size()), &BIO_free};

                std::unique_ptr<EVP_PKEY, void(*)(EVP_PKEY*)> key{nullptr, &EVP_PKEY_free};
                key.reset(PEM_read_bio_PrivateKey(keyBio.get(), nullptr, nullptr, (void*)keyPasswd.c_str()));
                if(!key)
                {
                    unsigned long error = ERR_get_error();
                    return cmt::readyFuture<None>(exception::buildInstance<api::tls::error::ReadKeyFailed>(tls::utils::errorString(error)));
                }

                int opResult = SSL_CTX_use_PrivateKey(_sslCtx.get(), key.get());
                if(1 != opResult)
                {
                    unsigned long error = ERR_get_error();
                    return cmt::readyFuture<None>(exception::buildInstance<api::tls::error::UseKeyFailed>(tls::utils::errorString(error)));
                }
            }

            return cmt::readyFuture(None{});
        };

        methods()->setDefaultTrustedCAs() += serviceSol() * [this]()
        {
            ERR_clear_error();
            X509_STORE* certStore = SSL_CTX_get_cert_store(_sslCtx.get());
            if(!certStore)
            {
                unsigned long error = ERR_get_error();
                return cmt::readyFuture<None>(exception::buildInstance<api::tls::error::CertStoreFailed>(tls::utils::errorString(error)));
            }

            int opResult = X509_STORE_set_default_paths(certStore);
            if(1 != opResult)
            {
                unsigned long error = ERR_get_error();
                return cmt::readyFuture<None>(exception::buildInstance<api::tls::error::CertStoreFailed>(tls::utils::errorString(error)));
            }

            return cmt::readyFuture(None{});
        };

        methods()->addTrustedCAs() += serviceSol() * [this](const List<String>& cas)
        {
            ERR_clear_error();
            X509_STORE* certStore = SSL_CTX_get_cert_store(_sslCtx.get());
            if(!certStore)
            {
                unsigned long error = ERR_get_error();
                return cmt::readyFuture<None>(exception::buildInstance<api::tls::error::CertStoreFailed>(tls::utils::errorString(error)));
            }

            for(const String& ca : cas)
            {
                std::unique_ptr<X509, void(*)(X509*)> x509{nullptr, &X509_free};
                std::unique_ptr<BIO, int(*)(BIO*)> caBio{BIO_new_mem_buf(ca.data(), ca.size()), &BIO_free};

                for(;;)
                {
                    x509.reset(PEM_read_bio_X509(caBio.get(), nullptr, nullptr, nullptr));
                    if(!x509)
                    {
                        unsigned long error = ERR_get_error();
                        if(!error || (ERR_GET_LIB(error) == ERR_LIB_PEM && ERR_GET_REASON(error) == PEM_R_NO_START_LINE))
                            break;

                        return cmt::readyFuture<None>(exception::buildInstance<api::tls::error::ReadCertFailed>(tls::utils::errorString(error)));
                    }

                    int opResult = X509_STORE_add_cert(certStore, x509.get());
                    if(1 != opResult)
                    {
                        unsigned long error = ERR_get_error();
                        return cmt::readyFuture<None>(exception::buildInstance<api::tls::error::AddCAFailed>(tls::utils::errorString(error)));
                    }
                }
            }

            return cmt::readyFuture(None{});
        };

        methods()->setVerify() += serviceSol() * [this](bool enable, uint8 depth)
        {
            int v = enable ?
                        SSL_VERIFY_PEER :
                        SSL_VERIFY_NONE;

            SSL_CTX_set_verify(_sslCtx.get(), v, SSL_CTX_get_verify_callback(_sslCtx.get()));
            SSL_CTX_set_verify_depth(_sslCtx.get(), depth);

            return cmt::readyFuture(None{});
        };

        methods()->setTmpDh()+= serviceSol() * [this](const String& tmpDh)
        {
            ERR_clear_error();
            std::unique_ptr<BIO, int(*)(BIO*)> bio{BIO_new_mem_buf(tmpDh.data(), tmpDh.size()), &BIO_free};
            std::unique_ptr<EVP_PKEY, void(*)(EVP_PKEY*)> pkey{PEM_read_bio_Parameters(bio.get(), nullptr), &EVP_PKEY_free};
            if(!pkey)
            {
                unsigned long error = ERR_get_error();
                return cmt::readyFuture<None>(exception::buildInstance<api::tls::error::SetTmpDhFailed>(tls::utils::errorString(error)));
            }

            int opResult = SSL_CTX_set0_tmp_dh_pkey(_sslCtx.get(), pkey.get());
            if(1 != opResult)
            {
                unsigned long error = ERR_get_error();
                pkey.reset();
                return cmt::readyFuture<None>(exception::buildInstance<api::tls::error::SetTmpDhFailed>(tls::utils::errorString(error)));
            }
            pkey.release();

            return cmt::readyFuture(None{});
        };

        methods()->client() += serviceSol() * [this](api::stream::Channel<>&& channel, Opt<String>&& servername)
        {
            return launch<api::tls::client::Channel<>, tls::client::Channel>(std::move(channel), servername.has_value() ? &servername.value() : nullptr);
        };

        methods()->server() += serviceSol() * [this](api::stream::Channel<>&& channel)
        {
            return launch<api::tls::server::Channel<>, tls::server::Channel>(std::move(channel));
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Tls::~Tls()
    {
        serviceSol().flush();
    }
}
