#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/loader/Mod.hpp>
#include <algorithm>
#include <cstdlib>
#include <random>
#include <string>
#include <vector>

using namespace geode::prelude;

namespace StreamChat {
    enum class Lang { English, Russian };

    static Lang language() {
        auto value = Mod::get()->getSettingValue<std::string>("language");
        return value == "Русский" ? Lang::Russian : Lang::English;
    }

    static std::vector<std::string> names = {
        "xXSlayerXx", "DashEnjoyer", "cube_lord", "ZobrosFan",
        "WaveMachine", "GDPlayer204", "NotRobTop", "AmongDash",
        "ConexionFan", "StereoEnjoyer", "FpsDrop", "JumperGD",
        "M ROBTOP", "M Conexion", "M Doggie"
    };

    static std::vector<std::string> death1 = {
        "bruh 💀", "oh no", "rip", "oof"
    };
    static std::vector<std::string> death23 = {
        "broo please 😭", "come on bro", "you got this", "BRO PLEASE"
    };
    static std::vector<std::string> death4 = {
        "NAHH 💀", "not again", "one more try"
    };
    static std::vector<std::string> death5 = {
        "SKILL ISSUE 💀", "bro is NOT cooking", "💀💀💀"
    };
    static std::vector<std::string> win = {
        "GG!!!", "W STREAMER", "LET'S GOOOO 🔥", "ABSOLUTE CINEMA",
        "BRO IS HIM", "GG WP", "INSANE CLUTCH"
    };

    static std::string pick(std::vector<std::string> const& values) {
        static std::mt19937 rng(std::random_device{}());
        return values[rng() % values.size()];
    }

    static std::string localize(std::string const& en, std::string const& ru) {
        return language() == Lang::Russian ? ru : en;
    }
}

class $modify(StreamChatPlayLayer, PlayLayer) {
    struct Fields {
        CCNode* m_streamChat = nullptr;
        CCLabelBMFont* m_chatLabel = nullptr;
        CCLabelBMFont* m_likeLabel = nullptr;
        int m_deathStreak = 0;
        int m_likes = 0;
        bool m_chatActive = false;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects))
            return false;

        if (m_isPracticeMode)
            return true;

        m_fields->m_chatActive = true;
        createStreamChat();
        addMessage(StreamChat::localize("Welcome to the stream!", "Добро пожаловать на стрим!"));
        return true;
    }

    void createStreamChat() {
        auto winSize = CCDirector::get()->getWinSize();
        m_fields->m_streamChat = CCNode::create();
        m_fields->m_streamChat->setID("gd-streamchat-overlay");
        m_fields->m_streamChat->setAnchorPoint({1.f, 0.f});

        auto bg = CCLayerColor::create({15, 12, 28, 190}, 205.f, 255.f);
        bg->setAnchorPoint({0, 0});
        m_fields->m_streamChat->addChild(bg);

        auto header = CCLayerColor::create({90, 45, 150, 230}, 205.f, 28.f);
        header->setPosition({0, 227});
        m_fields->m_streamChat->addChild(header);

        auto title = CCLabelBMFont::create("LIVE  •  CHAT", "bigFont.fnt");
        title->setAnchorPoint({0, 0.5f});
        title->setPosition({8, 241});
        title->setScale(0.42f);
        m_fields->m_streamChat->addChild(title);

        m_fields->m_likeLabel = CCLabelBMFont::create("♥ 0", "bigFont.fnt");
        m_fields->m_likeLabel->setAnchorPoint({1, 0.5f});
        m_fields->m_likeLabel->setPosition({197, 241});
        m_fields->m_likeLabel->setScale(0.42f);
        m_fields->m_streamChat->addChild(m_fields->m_likeLabel);

        m_fields->m_chatLabel = CCLabelBMFont::create("", "chatFont.fnt");
        m_fields->m_chatLabel->setAnchorPoint({0, 1});
        m_fields->m_chatLabel->setPosition({8, 218});
        m_fields->m_chatLabel->setScale(0.42f);
        m_fields->m_chatLabel->setWidth(185);
        m_fields->m_streamChat->addChild(m_fields->m_chatLabel);

        auto opacity = static_cast<int>(Mod::get()->getSettingValue<int64_t>("opacity"));
        bg->setOpacity(static_cast<GLubyte>(std::clamp(opacity, 0, 100) * 2.55));
        header->setOpacity(static_cast<GLubyte>(std::clamp(opacity + 20, 0, 100) * 2.55));

        auto scale = static_cast<float>(Mod::get()->getSettingValue<int64_t>("scale")) / 100.f;
        m_fields->m_streamChat->setScale(scale);

        auto pos = Mod::get()->getSettingValue<std::string>("position");
        if (pos == "Top left") {
            m_fields->m_streamChat->setAnchorPoint({0, 1});
            m_fields->m_streamChat->setPosition({12, winSize.height - 12});
        } else if (pos == "Top right") {
            m_fields->m_streamChat->setAnchorPoint({1, 1});
            m_fields->m_streamChat->setPosition({winSize.width - 12, winSize.height - 12});
        } else if (pos == "Bottom left") {
            m_fields->m_streamChat->setAnchorPoint({0, 0});
            m_fields->m_streamChat->setPosition({12, 12});
        } else {
            m_fields->m_streamChat->setAnchorPoint({1, 0});
            m_fields->m_streamChat->setPosition({winSize.width - 12, 12});
        }
        this->addChild(m_fields->m_streamChat, 1000);
    }

    void addMessage(std::string const& message) {
        if (!m_fields->m_chatActive || !m_fields->m_chatLabel) return;
        std::string name = StreamChat::pick(StreamChat::names);
        std::string line = name + ": " + message + "\n";
        std::string old = m_fields->m_chatLabel->getString();
        m_fields->m_chatLabel->setString((old + line).c_str());
        // Keep the newest chat lines visible.
        std::string current = m_fields->m_chatLabel->getString();
        if (current.size() > 900) {
            auto cut = current.find('\n', current.size() - 600);
            if (cut != std::string::npos)
                m_fields->m_chatLabel->setString(current.substr(cut + 1).c_str());
        }
    }

    void addLike() {
        if (!m_fields->m_chatActive || !Mod::get()->getSettingValue<bool>("likes")) return;
        m_fields->m_likes += 1 + (std::rand() % 4);
        if (m_fields->m_likeLabel) {
            auto text = fmt::format("♥ {}", m_fields->m_likes);
            m_fields->m_likeLabel->setString(text.c_str());
        }

        auto heart = CCLabelBMFont::create("♥", "bigFont.fnt");
        heart->setColor({255, 90, 150});
        heart->setOpacity(140);
        heart->setScale(0.5f);
        auto p = m_fields->m_streamChat->getPosition();
        heart->setPosition({p.x - 30, p.y + 35});
        this->addChild(heart, 1001);
        auto move = CCMoveBy::create(1.25f, {0, 65});
        auto shrink = CCScaleTo::create(1.25f, 0.08f);
        auto fade = CCFadeOut::create(1.25f);
        heart->runAction(CCSpawn::create(move, shrink, fade, nullptr));
        heart->runAction(CCSequence::create(
            CCDelayTime::create(1.25f),
            CCRemoveSelf::create(),
            nullptr
        ));
    }

    void destroyPlayer(PlayerObject* player, GameObject* object) {
        PlayLayer::destroyPlayer(player, object);
        if (!m_fields->m_chatActive) return;
        ++m_fields->m_deathStreak;
        if (m_fields->m_deathStreak == 1) {
            addMessage(StreamChat::pick(StreamChat::death1));
        } else if (m_fields->m_deathStreak <= 3) {
            addMessage(StreamChat::pick(StreamChat::death23));
        } else if (m_fields->m_deathStreak == 4) {
            addMessage(StreamChat::pick(StreamChat::death4));
        } else {
            addMessage(StreamChat::pick(StreamChat::death5));
        }
        if (std::rand() % 3 == 0) addLike();
    }

    void levelComplete() {
        if (m_fields->m_chatActive) {
            std::string levelName = m_level ? std::string(m_level->m_levelName) : "the level";
            addMessage(fmt::format("\"{}\" beat!", levelName));
            for (int i = 0; i < 8; ++i)
                addMessage(StreamChat::pick(StreamChat::win));
            for (int i = 0; i < 12; ++i)
                addLike();
        }
        PlayLayer::levelComplete();
    }
};
