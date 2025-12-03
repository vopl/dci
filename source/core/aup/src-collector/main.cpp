// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/exception.hpp>
#include <dci/logger.hpp>
#include <dci/integration/info.hpp>
#include <boost/program_options.hpp>
#include "collector.hpp"

namespace po = boost::program_options;

int main(int argc, char* argv[])
{
    try
    {
        //options
        dci::aup::Collector c;

        ////////////////////////////////////////////////////////////////////////////////
        po::options_description desc("dci-aup-collector");
        desc.add_options()
                ("help", "produce help message")
                ("version", "print version info")

                (
                    "meta-file",
                    po::value<std::string>()->notifier([&](auto v){c.setMetaFile(v);}),
                    "path to artifacts meta file"
                )
                (
                    "vendor-key",
                    po::value<std::string>()->notifier([&](auto v){c.setVendorKey(v);}),
                    "ed25519 secret key for signing"
                )
                (
                    "storage-dir",
                    po::value<std::string>()->notifier([&](auto v){c.setStorageDir(v);}),
                    "resulting storage directory"
                )
                (
                    "with-sources",
                    po::bool_switch()->notifier([&](auto v){c.setIgnoreSources(!v);}),
                    "collect source files"
                )
                (
                    "with-debug-for-targets",
                    po::bool_switch()->notifier([&](auto v){c.setIgnoreDebug4Targets(!v);}),
                    "collect debug info for dci executable files and libraries"
                )
                (
                    "with-debug-for-others",
                    po::bool_switch()->notifier([&](auto v){c.setIgnoreDebug4Others(!v);}),
                    "collect debug info for non-dci executable files and libraries"
                )
                ;

        ////////////////////////////////////////////////////////////////////////////////
        po::variables_map vars;
        po::store(po::command_line_parser(argc, argv).options(desc).run(), vars);
        po::notify(vars);

        ////////////////////////////////////////////////////////////////////////////////
        if(vars.empty() || vars.count("version"))
        {
            std::cout << dci::integration::info::version() << std::endl;
            return EXIT_SUCCESS;
        }

        ////////////////////////////////////////////////////////////////////////////////
        if(vars.count("help"))
        {
            std::cout << desc << std::endl;
            return EXIT_SUCCESS;
        }

        ////////////////////////////////////////////////////////////////////////////////
        c.run();
    }
    catch(...)
    {
        LOGE(dci::exception::toString(std::current_exception()));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
