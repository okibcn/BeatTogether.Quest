/*

██████╗ ███████╗ █████╗ ████████╗                                   
██╔══██╗██╔════╝██╔══██╗╚══██╔══╝                                   
██████╔╝█████╗  ███████║   ██║                                      
██╔══██╗██╔══╝  ██╔══██║   ██║                                      
██████╔╝███████╗██║  ██║   ██║                                      
╚═════╝ ╚══════╝╚═╝  ╚═╝   ╚═╝                                      
                                                                    
████████╗ ██████╗  ██████╗ ███████╗████████╗██╗  ██╗███████╗██████╗ 
╚══██╔══╝██╔═══██╗██╔════╝ ██╔════╝╚══██╔══╝██║  ██║██╔════╝██╔══██╗
   ██║   ██║   ██║██║  ███╗█████╗     ██║   ███████║█████╗  ██████╔╝
   ██║   ██║   ██║██║   ██║██╔══╝     ██║   ██╔══██║██╔══╝  ██╔══██╗
   ██║   ╚██████╔╝╚██████╔╝███████╗   ██║   ██║  ██║███████╗██║  ██║
   ╚═╝    ╚═════╝  ╚═════╝ ╚══════╝   ╚═╝   ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝

Apocrypha version by Mioki
*/

#include <random>
#include <iostream>

#include "modloader/shared/modloader.hpp"

#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"

#include "GlobalNamespace/PlatformAuthenticationTokenProvider.hpp"
#include "GlobalNamespace/AuthenticationToken.hpp"
#include "GlobalNamespace/MasterServerEndPoint.hpp"
#include "GlobalNamespace/MainMenuViewController.hpp"
#include "GlobalNamespace/MainSystemInit.hpp"
#include "GlobalNamespace/NetworkConfigSO.hpp"
#include "GlobalNamespace/UserCertificateValidator.hpp"
#include "GlobalNamespace/QuickPlaySongPacksDropdown.hpp"
using namespace GlobalNamespace;

#include "System/Threading/Tasks/Task_1.hpp"

#include "System/Action_1.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Resources.hpp"

#include "TMPro/TextMeshProUGUI.hpp"

//#include "Polyglot/LocalizedTextMeshProUGUI.hpp"
//#include "Polyglot/LanguageDirection.hpp"
//using namespace Polyglot;

#ifndef HOST_NAME
#error "Define HOST_NAME!"
#endif

#ifndef PORT
#error "Define PORT!"
#endif

#ifndef STATUS_URL
#error "Define STATUS_URL!"
#endif

#define BUTTON "Beat\nTogether"

#define SAVE(doc, key, type, value) \
    if (doc.HasMember(key))         \
    {                               \
        doc.RemoveMember(key);      \
    };                              \
    doc.AddMember(key, value, doc.GetAllocator());

#define LOAD(doc, name, type, default)          \
    doc.HasMember(name) && doc[name].Is##type() \
        ? doc[name].Get##type()                 \
        : (default)

static ModInfo modInfo;

Logger &logger()
{
    static auto _logger = new Logger(modInfo, LoggerOptions(false, true));
    return *_logger;
}

extern "C" void setup(ModInfo &info)
{
    info.id = ID;
    info.version = VERSION;
    modInfo = info;
    logger().info("Setup Completed!");
}

Configuration &getConfig()
{
    static Configuration _config((ModInfo){ID, VERSION});
    _config.Load();
    return _config;
}

MAKE_HOOK_MATCH(MainSystemInit_Init, &MainSystemInit::Init, void, MainSystemInit *self)
{
    MainSystemInit_Init(self);
    auto *networkConfig = self->networkConfig;

    logger().info("Overriding master server end point . . .");
    logger().info("Original status URL: %s", to_utf8(csstrtostr(networkConfig->masterServerStatusUrl)).c_str());
    logger().info("Original host: %s:%i", to_utf8(csstrtostr(networkConfig->masterServerHostName)).c_str(), networkConfig->masterServerPort);

    // Loads configuration from JSON config file managed by BS-Hook lib
    auto cfgRepo = getConfig();
    auto &cfgValue = cfgRepo.config; //load value array from config repo

    // Replaces the online game server data
    networkConfig->masterServerHostName = il2cpp_utils::newcsstr(LOAD(cfgValue, "hostname", String, HOST_NAME));
    networkConfig->masterServerPort = LOAD(cfgValue, "port", Int, PORT);
    networkConfig->masterServerStatusUrl = il2cpp_utils::newcsstr(LOAD(cfgValue, "status_url", String, STATUS_URL));

    logger().info("BeatTogether status URL: %s", to_utf8(csstrtostr(networkConfig->masterServerStatusUrl)).c_str());
    logger().info("BeatTogether host: %s:%i", to_utf8(csstrtostr(networkConfig->masterServerHostName)).c_str(), networkConfig->masterServerPort);

    // If config file or keys are missing, creates all the configurable keys
    SAVE(cfgValue, "hostname", String, to_utf8(csstrtostr(networkConfig->masterServerHostName)));
    SAVE(cfgValue, "port", Int, networkConfig->masterServerPort);
    SAVE(cfgValue, "status_url", String, to_utf8(csstrtostr(networkConfig->masterServerStatusUrl)));
    if (!cfgValue["replace_username"].IsBool())
    {
        SAVE(cfgValue, "replace_username", Bool, false);
    }
    if (!cfgValue["random_id"].IsBool())
    {
        SAVE(cfgValue, "random_id", Bool, false);
    }
    if (!cfgValue["username"].IsString())
    {
        SAVE(cfgValue, "username", String, "");
    }
    cfgRepo.Write();
    logger().info("Host configuration updated.");
}

MAKE_HOOK_MATCH(PlatformAuthenticationTokenProvider_GetAuthenticationToken, &PlatformAuthenticationTokenProvider::GetAuthenticationToken, System::Threading::Tasks::Task_1<GlobalNamespace::AuthenticationToken> *, PlatformAuthenticationTokenProvider *self)
{
    logger().info("Current Username: %s", to_utf8(csstrtostr(self->userName)).c_str());
    logger().info("Current User ID : %s", to_utf8(csstrtostr(self->userId)).c_str());

    auto cfgRepo = getConfig();
    auto &cfgValue = cfgRepo.config; //load config from config file in config storage

    if (cfgValue["replace_username"].GetBool() && // The replace username switch is activated
        cfgValue.HasMember("username") &&         // and There is a username entry
        cfgValue["username"].IsString() &&        // and username's value is a string
        cfgValue["username"].GetStringLength())   // and it is not an empty string
    {
        logger().info("Overriding username...");
        self->userName = il2cpp_utils::newcsstr(cfgValue["username"].GetString());
        logger().info("New username: %s", to_utf8(csstrtostr(self->userName)).c_str());
    }

    if (cfgValue["random_id"].GetBool())
    {
        logger().info("Generating random user ID...");
        std::mt19937_64 gen(std::random_device{}());
        std::uint64_t randomNumber = gen();
        auto newId = std::to_string(randomNumber);
        self->userId = il2cpp_utils::newcsstr(newId);
        logger().info("New user ID: %s", to_utf8(csstrtostr(self->userId)).c_str());
    }

    //SAVE(cfgValue, "username", String, to_utf8(csstrtostr(self->userName)));
    //SAVE(cfgValue, "userId", String, to_utf8(csstrtostr(self->userId)));
    //cfgRepo.Write();
    //logger().info("User configuration updated.");

    logger().info("Returning custom authentication token!");
    return System::Threading::Tasks::Task_1<AuthenticationToken>::New_ctor(AuthenticationToken(
        AuthenticationToken::Platform::OculusQuest,
        self->userId,
        self->userName,
        Array<uint8_t>::NewLength(0)));
}

MAKE_HOOK_MATCH(UserCertificateValidator_ValidateCertificateChainInternal, &UserCertificateValidator::ValidateCertificateChainInternal, void, UserCertificateValidator *self, GlobalNamespace::MasterServerEndPoint *endPoint, System::Security::Cryptography::X509Certificates::X509Certificate2 *certificate, ::Array<::Array<uint8_t> *> *certificateChain)
{
    // TODO: Support disabling the mod if official multiplayer is ever fixed
    // It'd be best if we do certificate validation here...
    // but for now we'll just skip it.
}

// Change the "Online" menu text to "Beat Together"
MAKE_HOOK_MATCH(MainMenuViewController_DidActivate, &MainMenuViewController::DidActivate, void, MainMenuViewController *self, bool firstActivation, bool addedToHierarchy, bool systemScreenEnabling)
{
    // Get pointer to button
    // Find the GameObject for the online button's text
    // static auto *searchPath = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("MainContent/OnlineButton");
    // static auto *textProperty = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("Text");
    // auto* sceneObject = self->get_gameObject();
    // auto* buttonObject = sceneObject->get_transform()->Find(searchPath)->get_gameObject();
    // auto* onlineButtonTextObj = buttonObject->get_transform()->Find(textProperty)->get_gameObject();
    // TMPro::TextMeshProUGUI *onlineButtonText = onlineButtonTextObj->GetComponent<TMPro::TextMeshProUGUI *>();
    static auto* searchPath = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("MainContent/OnlineButton/Text");
    TMPro::TextMeshProUGUI* onlineButtonText = self->get_gameObject()->get_transform()->Find(searchPath)->get_gameObject()->GetComponent<TMPro::TextMeshProUGUI*>();

    logger().info("Replacing the Online button text.");
    static auto *text = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>(BUTTON);
    onlineButtonText->set_text(text);

    // Align the Text in the Center
    // logger().info("BUTTON: set centered.");
    // onlineButtonText->set_alignment(TMPro::TextAlignmentOptions::Center);

    logger().info("BUTTON: Call original MainMenuViewController_DidActivate.");
    MainMenuViewController_DidActivate(self, firstActivation, addedToHierarchy, systemScreenEnabling);
}

// Disable QuickplaySongPacksOverrides if MQE is missing
MAKE_HOOK_MATCH(QuickPlaySongPacksDropdown_LazyInit, &QuickPlaySongPacksDropdown::LazyInit, void, QuickPlaySongPacksDropdown* self) {
    self->dyn__quickPlaySongPacksOverride() = nullptr;
    QuickPlaySongPacksDropdown_LazyInit(self);
}

extern "C" void load()
{
    logger().info("Loading BeatTogether...");
    il2cpp_functions::Init();

    INSTALL_HOOK(logger(), PlatformAuthenticationTokenProvider_GetAuthenticationToken);
    INSTALL_HOOK(logger(), MainSystemInit_Init);
    INSTALL_HOOK(logger(), UserCertificateValidator_ValidateCertificateChainInternal);
    INSTALL_HOOK(logger(), MainMenuViewController_DidActivate);
    // Checks if MQE is installed
    
    auto ModList = Modloader::getMods();
    if (ModList.find("multiquestensions") != ModList.end())
    {
        logger().info("Hello MQE!");
        logger().debug("MQE detected!");
    }
    else
    {
        logger().info("MQE not found, CustomSongs will not work!");
        INSTALL_HOOK(logger(), QuickPlaySongPacksDropdown_LazyInit);
    }
    
    logger().info("Finished loading BeatTogether.");
}
