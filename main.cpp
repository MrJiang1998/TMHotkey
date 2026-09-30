#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

#include <spdlog/sinks/basic_file_sink.h>

#include <fstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <filesystem>

namespace
{
	// ---------- 配置 ----------
	struct Settings
	{
		std::uint32_t key = 66;  // 默认 F8 (DIK 十进制)
		bool requireCtrl = false;
		bool requireAlt = false;
		bool requireShift = false;
	};

	Settings g_settings;

	std::string Trim(const std::string& s)
	{
		auto b = s.find_first_not_of(" \t\r\n");
		if (b == std::string::npos) return "";
		auto e = s.find_last_not_of(" \t\r\n");
		return s.substr(b, e - b + 1);
	}

	bool ParseBool(const std::string& v, bool def)
	{
		std::string x = Trim(v);
		std::transform(x.begin(), x.end(), x.begin(),
			[](unsigned char c) { return static_cast<char>(std::tolower(c)); });
		if (x == "true" || x == "1" || x == "yes" || x == "on") return true;
		if (x == "false" || x == "0" || x == "no" || x == "off") return false;
		return def;
	}

	// 从 INI 加载（找不到则用默认）
	void LoadSettings(const std::string& a_path)
	{
		std::ifstream f(a_path);
		if (!f.is_open())
			return;

		std::string line;
		while (std::getline(f, line)) {
			line = Trim(line);
			if (line.empty() || line[0] == ';' || line[0] == '#')
				continue;
			const auto eq = line.find('=');
			if (eq == std::string::npos)
				continue;

			const auto k = Trim(line.substr(0, eq));
			const auto v = Trim(line.substr(eq + 1));

			if (k == "Key")           g_settings.key = static_cast<std::uint32_t>(std::stoul(v));
			else if (k == "RequireCtrl")  g_settings.requireCtrl = ParseBool(v, g_settings.requireCtrl);
			else if (k == "RequireAlt")   g_settings.requireAlt = ParseBool(v, g_settings.requireAlt);
			else if (k == "RequireShift") g_settings.requireShift = ParseBool(v, g_settings.requireShift);
		}
	}

	// ---------- 热键监听 ----------
	// DIK 十进制扫描码
	constexpr std::uint32_t DIK_LSHIFT = 42;
	constexpr std::uint32_t DIK_RSHIFT = 54;
	constexpr std::uint32_t DIK_LCONTROL = 29;
	constexpr std::uint32_t DIK_RCONTROL = 157;
	constexpr std::uint32_t DIK_LMENU = 56;   // 左 Alt
	constexpr std::uint32_t DIK_RMENU = 184;  // 右 Alt

	class HotkeySink final : public RE::BSTEventSink<RE::InputEvent*>
	{
	public:
		static HotkeySink* GetSingleton()
		{
			static HotkeySink singleton;
			return &singleton;
		}

		RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_event,
		                                      RE::BSTEventSource<RE::InputEvent*>*) override
		{
			if (!a_event)
				return RE::BSEventNotifyControl::kContinue;

			for (auto* event = *a_event; event; event = event->next) {
				const auto* btn = event->AsButtonEvent();
				if (!btn)
					continue;

				const auto id = btn->GetIDCode();

				// 记录修饰键状态
				switch (id) {
				case DIK_LSHIFT: case DIK_RSHIFT:   m_shift = btn->IsDown(); continue;
				case DIK_LCONTROL: case DIK_RCONTROL: m_ctrl = btn->IsDown(); continue;
				case DIK_LMENU: case DIK_RMENU:     m_alt = btn->IsDown(); continue;
				default: break;
				}

				// 主键按下瞬间
				if (id == g_settings.key && btn->IsDown() && btn->IsPressed()) {
					const bool ctrlOk = (g_settings.requireCtrl == m_ctrl);
					const bool altOk = (g_settings.requireAlt == m_alt);
					const bool shiftOk = (g_settings.requireShift == m_shift);

					if (ctrlOk && altOk && shiftOk) {
						ToggleMenus();
					}
				}
			}
			return RE::BSEventNotifyControl::kContinue;
		}

	private:
		// 调用引擎原生菜单切换（等同控制台 TM）
		static void ToggleMenus()
		{
			auto* ui = RE::UI::GetSingleton();
			if (!ui)
				return;
			ui->ShowMenus(!ui->IsShowingMenus());
		}

		bool m_shift = false;
		bool m_ctrl = false;
		bool m_alt = false;
	};
}

// 插件版本信息
extern "C" __declspec(dllexport) constinit auto SKSEPlugin_Version = []() {
	SKSE::PluginVersionData v{};
	v.PluginVersion({ 1, 0, 0, 0 });
	v.PluginName("TMHotkey");
	v.AuthorName("TMHotkey Project");
	v.UsesAddressLibrary(true);
	v.UsesUpdatedStructs(true);
	return v;
}();

extern "C" __declspec(dllexport) bool SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse);

	// 日志
	if (auto dir = SKSE::log::log_directory(); dir) {
		*dir /= "TMHotkey.log";
		auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(dir->string(), true);
		auto log = std::make_shared<spdlog::logger>("TMHotkey", std::move(sink));
		spdlog::set_default_logger(std::move(log));
	}

	// 配置路径：Data/SKSE/Plugins/TMHotkey.ini
	// SKSE 提供插件所在 Data 路径，这里用相对路径，由游戏工作目录解析
	LoadSettings("Data/SKSE/Plugins/TMHotkey.ini");

	spdlog::info("TMHotkey loaded, hotkey DIK {}", g_settings.key);

	// 注册输入
	if (auto* reg = SKSE::GetInputEventRegister()) {
		reg->RegisterSink(HotkeySink::GetSingleton());
	}

	return true;
}
