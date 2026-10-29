// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "UI\WeaponWheel.hpp"

#include "CrimsonEfk.hpp"
#include "../ThirdParty/SDL2/SDL_gamecontroller.h"
#include "../ThirdParty/glm/glm.hpp"
#include "CrimsonGUI.hpp"
#include "CrimsonLicense.hpp"
#include "Core/Core.hpp"
#include "CrimsonGameplay.hpp"
#include "Camera.hpp"
#include <shellapi.h>
#include "Core/GUI.hpp"
#include "Core/Core_ImGui.hpp"
#include "Global/GlobalBase.hpp"
#include "Global/GUIBase.hpp"
#include "CrimsonUtil.hpp"
#include "CrimsonFileHandling.hpp"
#include "CrimsonOnboarding.hpp"
#include "DMC3Input.hpp"
#include "File.hpp"
#include "ImGuiExtra.hpp"
#include "Patreon.hpp"
#include "FMOD.hpp"
#include "Graphics.hpp"
#include "Internal.hpp"
#include "Actor.hpp"
#include "ActorBase.hpp"
#include "Arcade.hpp"
#include "Config.hpp"
#include "Event.hpp"
#include "Exp.hpp"
#include "Global.hpp"
#include "HUD.hpp"
#include "Scene.hpp"
#include "Sound.hpp"
#include "Speed.hpp"
#include "Training.hpp"
#include "Window.hpp"
#include "WebAPICalls.hpp"

#include <cmath>
#include <array>
#include <format>
#include <memory>

#include <shellapi.h>

#include <windows.h>
#include <dxgi.h>
#include <d3d11.h>
#include <thread>

#undef VOID

#include <stdio.h>
#include <intrin.h>
#include <string>
#include <sstream>
#include <iostream>
#include <format>
#include <thread>
#include <chrono>
#include "Vars.hpp"

#include "Core/Macros.h"

#include "Core/DebugSwitch.hpp"
#include <SDL_joystick.h>
#include "CrimsonTimers.hpp"
#include "CrimsonOnTick.hpp"
#include "CrimsonGameModes.hpp"
#include "CrimsonVisualStyle.hpp"
#include "CrimsonSDL.hpp"
#include "CrimsonPatches.hpp"
#include "CrimsonDetours.hpp"
#include "UI\Texture2DD3D11.hpp"
#include "UI\EmbeddedImages.hpp"
#include "CrimsonCameraController.hpp"
#include "CrimsonBetterArkham2.hpp"

#include "DebugDrawDX11.hpp"

#include "ImGui/imgui.h"
#include "CrimsonHUD.hpp"
#include "CrimsonFX.hpp"
#include "CrimsonReversedCalls.hpp"

#define SDL_FUNCTION_DECLRATION(X) decltype(X)* fn_##X
#define LOAD_SDL_FUNCTION(X) fn_##X = GetSDLFunction<decltype(X)*>(#X)


namespace CrimsonGUI {
float scaleFactorX;
float scaleFactorY;
float scaleFactor;
float scaledFontSize;
float itemWidth;
}

using namespace CrimsonGUI;

void DrawMainContent(ID3D11Device* pDevice, UI::UIContext& context);

namespace UI {
WebAPIResult versionCheckResult = WebAPIResult::Awaiting;
WebAPIResult patronsQueueResult = WebAPIResult::Awaiting;

void DrawCrimson(IDXGISwapChain* pSwapChain, const char* title, bool* pIsOpened) {
	if (pIsOpened != nullptr && *pIsOpened == false)
		return;

	ID3D11Device* pDevice = nullptr;

	pSwapChain->GetDevice(IID_PPV_ARGS(&pDevice));

	// Todo: move to context
	static bool uiElementsInitialized = false;
	if (!uiElementsInitialized) {
		g_Image_CrimsonHeaderLogo.ResizeByRatioW(size_t(g_UIContext.DefaultFontSize * 13.23f));

		uiElementsInitialized = true;
	}

	
	// Default font
	ImGui::PushFont(g_ImGuiFont_Roboto[g_UIContext.DefaultFontSize]);
	// 6 tab buttons + a bit more for the update when there is new version
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, { g_UIContext.DefaultFontSize * 9.38f * 6.0f + g_UIContext.DefaultFontSize * 3.0f, g_UIContext.DefaultFontSize * 35.0f });
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f * scaleFactorY);
	ImGui::PushStyleColor(ImGuiCol_Button, SwapColorEndianness(0xDA1B53FF));
	ImGui::Begin(title, nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
	float scaleFactorY = ImGui::GetIO().DisplaySize.y / 1080;
	ImGui::SetWindowFontScale(scaleFactorY);
	float scaledFontSize = g_UIContext.DefaultFontSize * scaleFactorY;
	ImGui::PopStyleVar(4);

	ImVec2 customSpacing(10.0f * scaleFactorY, 7.0f * scaleFactorY); // Horizontal and vertical spacing
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, customSpacing);
	// Scale style variables
	ImGuiStyle& style = ImGui::GetStyle();

	float scaleFactorUltrawideY = g_renderSize.y / 610;
	style.WindowPadding = ImVec2(6.0f * scaleFactorUltrawideY, 3.0f * scaleFactorUltrawideY);
	style.WindowRounding = 0.0f * scaleFactorY;
	style.WindowBorderSize = 1.0f * scaleFactorUltrawideY;
	style.WindowMinSize = ImVec2(32.0f * scaleFactorUltrawideY, 32.0f * scaleFactorUltrawideY);
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f); // Alignment values are typically not scaled
	style.WindowMenuButtonPosition = ImGuiDir_Left; // Enum values are not scaled
	style.ChildRounding = 0.0f * scaleFactorY;
	style.ChildBorderSize = 1.0f * scaleFactorUltrawideY;
	style.PopupRounding = 0.0f * scaleFactorY;
	style.PopupBorderSize = 1.0f * scaleFactorUltrawideY;
	style.FramePadding = ImVec2(5.0f * scaleFactorY, 1.0f * scaleFactorY);
	style.FrameRounding = 3.0f * scaleFactorY;
	style.FrameBorderSize = 1.0f * scaleFactorUltrawideY;
	style.ItemSpacing = ImVec2(8.0f * scaleFactorUltrawideY, 4.0f * scaleFactorUltrawideY);
	style.ItemInnerSpacing = ImVec2(4.0f * scaleFactorUltrawideY, 4.0f * scaleFactorUltrawideY);
	style.CellPadding = ImVec2(4.0f * scaleFactorUltrawideY, 2.0f * scaleFactorUltrawideY);
	style.IndentSpacing = 21.0f * scaleFactorUltrawideY;
	style.ColumnsMinSpacing = 6.0f * scaleFactorUltrawideY;
	style.ScrollbarSize = 13.0f * scaleFactorY;
	style.ScrollbarRounding = 16.0f * scaleFactorY;
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.05f, 0.05f, 0.50f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.588f, 0.094f, 0.235f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.729f, 0.122f, 0.294f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.439f, 0.071f, 0.176f, 1.00f);
	style.GrabMinSize = 20.0f * scaleFactorY;
	style.GrabRounding = 2.0f * scaleFactorY;
	style.TabRounding = 4.0f * scaleFactorY;
	style.TabBorderSize = 1.0f * scaleFactorUltrawideY;
	style.TabMinWidthForCloseButton = 0.0f * scaleFactorUltrawideY;
	style.ColorButtonPosition = ImGuiDir_Right; // Enum values are not scaled
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f); // Alignment values are typically not scaled
	style.SelectableTextAlign = ImVec2(0.0f, 0.0f); // Alignment values are typically
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		ImRect wndRect = window->Rect();
		ImGuiStyle& style = ImGui::GetStyle();
		
		queuedCrimsonConfig.System.windowSizeX = window->Size.x;
		queuedCrimsonConfig.System.windowSizeY = window->Size.y;
		queuedCrimsonConfig.System.windowPosX = window->Pos.x;
		queuedCrimsonConfig.System.windowPosY = window->Pos.y;
		window->ContentRegionRect = ImRect{ window->ContentRegionRect.Min, window->ContentRegionRect.Max - ImVec2{ 0.0f, 70.0f } };

		float contentMinHeightOffsetFromTop;
		float contentMaxHeightOffsetFromBottom;

		// Draw the main window header graphic
		{
			// DMC3Crimson Logo
			static const Texture2DD3D11 logo(g_Image_CrimsonHeaderLogo.GetRGBAData(), g_Image_CrimsonHeaderLogo.GetWidth(), g_Image_CrimsonHeaderLogo.GetHeight(), pDevice);

			auto logoWidth = logo.GetWidth() * scaleFactorY;
			auto logoHeight = logo.GetHeight() * scaleFactorY;

			ImVec2 logoPos = { (wndRect.Max.x + wndRect.Min.x) * 0.5f - logoWidth * 0.5f, wndRect.Min.y + logoHeight * 0.3f };

			window->DrawList->AddLine({ wndRect.Min.x + scaledFontSize * 1.7f, logoPos.y + logoHeight * 0.5f },
				{ logoPos.x - scaledFontSize * 0.52f, logoPos.y + logoHeight * 0.5f }, SwapColorEndianness(0xE01D42FF));

			window->DrawList->AddLine({ logoPos.x + logoWidth + scaledFontSize * 0.52f, logoPos.y + logoHeight * 0.5f },
				{ wndRect.Max.x - scaledFontSize * 1.7f, logoPos.y + logoHeight * 0.5f }, SwapColorEndianness(0xE01D42FF));

			auto logoSize = ImVec2(logoPos.x + logoWidth, logoPos.y + logoHeight);

			window->DrawList->AddImage(logo, logoPos, logoSize);

			// BETA Notice 
			const char *text = "BETA";
			ImGui::PushFont(g_ImGuiFont_RussoOne[g_UIContext.DefaultFontSize * 0.8f]);
			ImVec2 textSize = ImGui::CalcTextSize(text);
			float padding = scaledFontSize * 0.2f;
			ImVec2 buttonSize = { textSize.x + padding, textSize.y + padding};

			ImVec2 buttonPos = {
				logoPos.x + logoWidth * 0.5f - buttonSize.x * 0.5f,
				logoPos.y + logoHeight + scaledFontSize * -0.3f // spacing below logo
			};
			ImVec2 buttonMax = buttonPos + buttonSize;

			ImVec2 textPos = {
				buttonPos.x + (buttonSize.x - textSize.x) * 0.5f,
				buttonPos.y + (buttonSize.y - textSize.y) * 0.5f
			};
			window->DrawList->AddText(ImGui::GetFont(), ImGui::GetFontSize(), textPos, IM_COL32(255, 255, 255, 255), text);

			ImGui::PopFont();
		}

		// Draw the close button
		{
			if (pIsOpened != nullptr) {
				const auto cursorBackUp = ImGui::GetCursorScreenPos();
				ImGui::SetCursorScreenPos({ wndRect.Max.x - scaledFontSize * 1.17f - 10.0f, wndRect.Min.y + scaledFontSize * 0.45f });

				if (CloseButton("#Close", { scaledFontSize * 1.17f, scaledFontSize * 1.17f })) {
					*pIsOpened = false;
				}

				ImGui::SetCursorScreenPos(cursorBackUp);
			}
		}

		// Selected game mode text
		{
			const ImVec2 pos = wndRect.Min + ImVec2{ scaledFontSize * 1.33f, scaledFontSize * 2.66f };

			window->DrawList->AddText(g_ImGuiFont_Roboto[scaledFontSize], scaledFontSize, pos, SwapColorEndianness(0xFFFFFFFF), "已选游戏模式： ");

			const float modeTextWidth = ImGui::CalcTextSize("已选游戏模式： ").x;

			const char* gameModeString = nullptr;
			ImU32 gameModeStringColor = 0;

			switch (g_UIContext.SelectedGameMode) {
			case UIContext::GameModes::Vanilla:
				gameModeString = "原版模式";
				gameModeStringColor = 0xFFFFFFFF;
				break;

			case UIContext::GameModes::StyleSwitcher:
				gameModeString = "风格切换模式";
				gameModeStringColor = SwapColorEndianness(0xE8BA18FF);
				break;

			case UIContext::GameModes::Crimson:
				gameModeString = "CRIMSON 模式";
				gameModeStringColor = SwapColorEndianness(0xDA1B53FF);
				break;

			case UIContext::GameModes::Custom:
				gameModeString = "自定义模式";
				gameModeStringColor = SwapColorEndianness(0x4050FFFF);
				break;

			default:
				gameModeString = "未知";
				break;
			}

			const float gameModeStringWidth = ImGui::CalcTextSize(gameModeString).x;

			const char* ccsRestartString = "需重启任务以更新 CCS。";
			auto ccsRestartStringColor = SwapColorEndianness(0x1DD6FFFF);

			window->DrawList->AddText(g_ImGuiFont_Benguiat[scaledFontSize], scaledFontSize, pos + ImVec2{ modeTextWidth, 0.0f }, 
				gameModeStringColor, gameModeString);

			if (activeConfig.Actor.enable != queuedConfig.Actor.enable && g_scene == SCENE::GAME) {
				window->DrawList->AddText(g_ImGuiFont_Roboto[scaledFontSize], scaledFontSize * 0.9f, ImVec2(pos.x + (250.0f * scaleFactorY), pos.y - (35.0f * scaleFactorY)), ccsRestartStringColor, ccsRestartString);
			}
		}

		// Version text
		{
			std::string versionStr{};
			std::string latestUpdateStr{};
			std::string newVersionText{};

			if (g_UIContext.CurrentVersion.PatchLetter == 0) {
				versionStr = std::format("v{}.{}", g_UIContext.CurrentVersion.Major, g_UIContext.CurrentVersion.Minor);
			}
			else {
				versionStr = std::format("v{}.{}{}", g_UIContext.CurrentVersion.Major, g_UIContext.CurrentVersion.Minor, g_UIContext.CurrentVersion.PatchLetter);
			}

			if (g_UIContext.LatestVersion.PatchLetter == 0) {
				newVersionText = std::format("新版本可用 (v{}.{})", g_UIContext.LatestVersion.Major, g_UIContext.LatestVersion.Minor);
			}
			else {
				newVersionText = std::format("新版本可用 (v{}.{}{})", g_UIContext.LatestVersion.Major, g_UIContext.LatestVersion.Minor, g_UIContext.LatestVersion.PatchLetter);
			}

			switch (UI::versionCheckResult) {
			case WebAPIResult::Success:
				latestUpdateStr = std::format("Latest Update: {}/{}/{}", g_UIContext.LatestUpdateDate.Day, g_UIContext.LatestUpdateDate.Month, g_UIContext.LatestUpdateDate.Year);
				break;

			case WebAPIResult::Awaiting:
				latestUpdateStr = "最新更新...";
				break;

			default:
				latestUpdateStr = "";
				break;
			}

			const float latestUpdateTextWidth = ImGui::CalcTextSize(latestUpdateStr.c_str()).x;
			const float newVersionTextWidth = g_UIContext.NewVersionAvailable ? ImGui::CalcTextSize(newVersionText.c_str()).x : 0.0f;
			const float maxLenthUpdateSection = latestUpdateTextWidth > newVersionTextWidth ? latestUpdateTextWidth : newVersionTextWidth;

			const float versionButtonFontSize = size_t(g_UIContext.DefaultFontSize * 1.45f);

			ImGui::PushFont(g_ImGuiFont_RussoOne[versionButtonFontSize * 0.8f]);

			const float versionButtonWidth = ImGui::CalcTextSize(versionStr.c_str()).x + style.FramePadding.x * 2.0f;

			ImGui::PopFont();

			const ImVec2 pos = ImVec2{ wndRect.Max.x - maxLenthUpdateSection - versionButtonWidth - scaledFontSize * 2.0f,
										wndRect.Min.y + scaledFontSize * 2.66f };

			// Two lines of text without space in between each with the default font size
			float dateTextAndButtonHeightSum = scaledFontSize * 2.0f;

			window->DrawList->AddText(g_ImGuiFont_Roboto[scaledFontSize * 0.9f], scaledFontSize,
				g_UIContext.NewVersionAvailable ? pos + ImVec2{
					0.0f,
					g_UIContext.DefaultFontSize * 0.5f - dateTextAndButtonHeightSum * 0.5f } : pos, SwapColorEndianness(0xFFFFFFFF), latestUpdateStr.c_str());

			auto cursorBackUp = ImGui::GetCursorScreenPos();
			ImGui::SetCursorScreenPos(pos + ImVec2{ maxLenthUpdateSection + scaledFontSize * 0.3f,
													style.FramePadding.y + scaledFontSize * 0.5f - scaledFontSize * 0.65f });

			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.5f * scaleFactorY);
			ImGui::PushFont(g_ImGuiFont_RussoOne[g_UIContext.DefaultFontSize * 1.0f]);

			if (InfoButton(versionStr.c_str())) {
				// Todo: Redirect to patch notes
			}

			ImGui::PopFont();
			ImGui::PopStyleVar();

			if (g_UIContext.NewVersionAvailable) {
				ImGui::SetCursorScreenPos(pos + ImVec2{ 0.0f, scaledFontSize * 0.5f });

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 5.0f, 0.0f });
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);

				ImGui::PushFont(g_ImGuiFont_Roboto[g_UIContext.DefaultFontSize * 0.9f]);

				std::string latestVersionURL = UI::g_UIContext.LatestVersionURL.empty() ? 
					"https://github.com/berthrage/Devil-May-Cry-3-Crimson/releases/latest" : UI::g_UIContext.LatestVersionURL;

				// NEW VERSION AVAILABLE
 				if (InfoButton(newVersionText.c_str())) {
 					ShellExecute(0, 0, latestVersionURL.c_str(), 0, 0, SW_SHOW);
 				}

				ImGui::PopFont();

				ImGui::PopStyleVar(2);
			}

			ImGui::SetCursorScreenPos(cursorBackUp);
		}

		// Draw main tabs / sub tabs
		{
			const ImVec2 tabBtnSize = { scaledFontSize * 9.38f, scaledFontSize * 2.4f };
			const ImVec2 subTabBtnSize = { tabBtnSize.x * 0.8f, tabBtnSize.y * 0.8f };

			// The black strip behind tab buttons
			window->DrawList->AddRectFilled(
				{ wndRect.Min.x, wndRect.Min.y + 2.0f * tabBtnSize.y },
				{ wndRect.Max.x, wndRect.Min.y + 2.0f * tabBtnSize.y + tabBtnSize.y },
				SwapColorEndianness(0x000000FF)
			);

			ImGui::PushFont(g_ImGuiFont_RussoOne[g_UIContext.DefaultFontSize * 0.9f]);

			contentMinHeightOffsetFromTop = 2.0f * tabBtnSize.y + tabBtnSize.y;

			// Sub tabs
			if (g_UIContext.SelectedTab == UIContext::MainTabs::Options || g_UIContext.SelectedTab == UIContext::MainTabs::CheatsAndDebug) {
				contentMinHeightOffsetFromTop += subTabBtnSize.y;

				// Grey strip behind the subtabs
				window->DrawList->AddRectFilled(
					{ wndRect.Min.x, wndRect.Min.y + 2.0f * tabBtnSize.y + tabBtnSize.y },
					{ wndRect.Max.x, wndRect.Min.y + 2.0f * tabBtnSize.y + tabBtnSize.y + subTabBtnSize.y },
					SwapColorEndianness(0x00000050)
				);

				switch (g_UIContext.SelectedTab) {
				case UIContext::MainTabs::Options:
				{
					const float tabBarWidth = wndRect.GetSize().x;
					const float tabButtonsGap = (tabBarWidth - float(UIContext::OptionsSubTabs::Size) * subTabBtnSize.x) /
						float((size_t)UIContext::OptionsSubTabs::Size + 1);

					auto cursorPosBackup = ImGui::GetCursorScreenPos();
					ImGui::SetCursorScreenPos({ wndRect.Min.x, wndRect.Min.y + 2.0f * tabBtnSize.y + tabBtnSize.y });

					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + tabButtonsGap);
					if (TabButton("玩法", g_UIContext.SelectedOptionsSubTab == UIContext::OptionsSubTabs::Gameplay, true, false, subTabBtnSize)) {
						g_UIContext.SelectedOptionsSubTab = UIContext::OptionsSubTabs::Gameplay;
					}

					ImGui::SameLine(0.0f, tabButtonsGap);
					if (TabButton("镜头", g_UIContext.SelectedOptionsSubTab == UIContext::OptionsSubTabs::Camera, true, false, subTabBtnSize)) {
						g_UIContext.SelectedOptionsSubTab = UIContext::OptionsSubTabs::Camera;
					}

					ImGui::SameLine(0.0f, tabButtonsGap);
					if (TabButton("快捷键", g_UIContext.SelectedOptionsSubTab == UIContext::OptionsSubTabs::Hotkeys, true, false, subTabBtnSize)) {
						g_UIContext.SelectedOptionsSubTab = UIContext::OptionsSubTabs::Hotkeys;
					}

					ImGui::SameLine(0.0f, tabButtonsGap);
					if (TabButton("界面", g_UIContext.SelectedOptionsSubTab == UIContext::OptionsSubTabs::Interface, true, false, subTabBtnSize)) {
						g_UIContext.SelectedOptionsSubTab = UIContext::OptionsSubTabs::Interface;
					}

					ImGui::SameLine(0.0f, tabButtonsGap);
					if (TabButton("音效", g_UIContext.SelectedOptionsSubTab == UIContext::OptionsSubTabs::Sound, true, false, subTabBtnSize)) {
						g_UIContext.SelectedOptionsSubTab = UIContext::OptionsSubTabs::Sound;
					}

					ImGui::SameLine(0.0f, tabButtonsGap);
					if (TabButton("画面", g_UIContext.SelectedOptionsSubTab == UIContext::OptionsSubTabs::Visual, true, false, subTabBtnSize)) {
						g_UIContext.SelectedOptionsSubTab = UIContext::OptionsSubTabs::Visual;
					}

					ImGui::SameLine(0.0f, tabButtonsGap);
					if (TabButton("系统", g_UIContext.SelectedOptionsSubTab == UIContext::OptionsSubTabs::System, true, false, subTabBtnSize)) {
						g_UIContext.SelectedOptionsSubTab = UIContext::OptionsSubTabs::System;
					}

					ImGui::SetCursorScreenPos(cursorPosBackup);
				}
				break;

				case UIContext::MainTabs::CheatsAndDebug:
				{
					const float tabBarWidth = wndRect.GetSize().x;
					const float tabButtonsGap = (tabBarWidth - float(UIContext::CheatsAndDebugSubTabs::Size) * subTabBtnSize.x) /
						float((size_t)UIContext::CheatsAndDebugSubTabs::Size + 1);

					auto cursorPosBackup = ImGui::GetCursorScreenPos();
					ImGui::SetCursorScreenPos({ wndRect.Min.x, wndRect.Min.y + 2.0f * tabBtnSize.y + tabBtnSize.y });

					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + tabButtonsGap);
					if (TabButton("通用作弊", g_UIContext.SelectedCheatsAndDebugSubTab == UIContext::CheatsAndDebugSubTabs::CommonCheats, true, false, subTabBtnSize)) {
						g_UIContext.SelectedCheatsAndDebugSubTab = UIContext::CheatsAndDebugSubTabs::CommonCheats;
					}

					ImGui::SameLine(0.0f, tabButtonsGap);
					ImGui::PushFont(g_ImGuiFont_RussoOne[g_UIContext.DefaultFontSize * 0.83f]);
					if (TabButton("角色作弊", g_UIContext.SelectedCheatsAndDebugSubTab == UIContext::CheatsAndDebugSubTabs::CharacterCheats, true, false, subTabBtnSize)) {
						g_UIContext.SelectedCheatsAndDebugSubTab = UIContext::CheatsAndDebugSubTabs::CharacterCheats;
					}
					ImGui::PopFont();

					ImGui::SameLine(0.0f, tabButtonsGap);
					if (TabButton("传送", g_UIContext.SelectedCheatsAndDebugSubTab == UIContext::CheatsAndDebugSubTabs::Teleporter, true, false, subTabBtnSize)) {
						g_UIContext.SelectedCheatsAndDebugSubTab = UIContext::CheatsAndDebugSubTabs::Teleporter;
					}

					ImGui::SameLine(0.0f, tabButtonsGap);
					if (TabButton("敌人生成器", g_UIContext.SelectedCheatsAndDebugSubTab == UIContext::CheatsAndDebugSubTabs::EnemySpawner, true, false, subTabBtnSize)) {
						g_UIContext.SelectedCheatsAndDebugSubTab = UIContext::CheatsAndDebugSubTabs::EnemySpawner;
					}

					ImGui::SameLine(0.0f, tabButtonsGap);
					if (TabButton("FMOD 点唱机", g_UIContext.SelectedCheatsAndDebugSubTab == UIContext::CheatsAndDebugSubTabs::FMODJukebox, true, false, subTabBtnSize)) {
						g_UIContext.SelectedCheatsAndDebugSubTab = UIContext::CheatsAndDebugSubTabs::FMODJukebox;
					}

					ImGui::SetCursorScreenPos(cursorPosBackup);
				}
				break;

				default:
					break;
				}

			}

			ImGui::PopFont();

			ImGui::PushFont(g_ImGuiFont_RussoOne[size_t(g_UIContext.DefaultFontSize * 1.15f)]);

			// Draw main tabs
			{
				const float tabBarWidth = wndRect.GetSize().x;
				const float tabButtonsGap = (tabBarWidth - float(UIContext::MainTabs::Size) * tabBtnSize.x) /
					float((size_t)UIContext::MainTabs::Size + 2);

				auto cursorPosBackup = ImGui::GetCursorScreenPos();
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f * tabBtnSize.y);

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + tabButtonsGap);
				if (TabButton("游戏模式", g_UIContext.SelectedTab == UIContext::MainTabs::GameMode, true, false, tabBtnSize)) {
					g_UIContext.SelectedTab = UIContext::MainTabs::GameMode;
				}

				ImGui::SameLine(0.0f, tabButtonsGap);
				if (TabButton("玩家", g_UIContext.SelectedTab == UIContext::MainTabs::Player, true, false, tabBtnSize)) {
					g_UIContext.SelectedTab = UIContext::MainTabs::Player;
				}

				ImGui::SameLine(0.0f, tabButtonsGap);
				if (TabButton("快速游玩", g_UIContext.SelectedTab == UIContext::MainTabs::Quickplay, true, false, tabBtnSize)) {
					g_UIContext.SelectedTab = UIContext::MainTabs::Quickplay;
				}

				ImGui::SameLine(0.0f, tabButtonsGap);
				if (TabButton("音乐切换", g_UIContext.SelectedTab == UIContext::MainTabs::MusicSwitcher, false, false, tabBtnSize)) {
					g_UIContext.SelectedTab = UIContext::MainTabs::MusicSwitcher;
				}

				ImGui::SameLine(0.0f, tabButtonsGap);
				if (TabButton("选项", g_UIContext.SelectedTab == UIContext::MainTabs::Options, true, true, tabBtnSize)) {
					g_UIContext.SelectedTab = UIContext::MainTabs::Options;
				}

				ImGui::SameLine(0.0f, tabButtonsGap);
				if (TabButton("作弊与调试", g_UIContext.SelectedTab == UIContext::MainTabs::CheatsAndDebug, true, true, tabBtnSize)) {
					g_UIContext.SelectedTab = UIContext::MainTabs::CheatsAndDebug;
				}

				ImGui::SetCursorScreenPos(cursorPosBackup);
			}

			ImGui::PopFont();

			// Background faded window name text
			{
				ImVec2 pos{ wndRect.Min.x + g_UIContext.DefaultFontSize * 0.1f,
							wndRect.Min.y + (2.0f * tabBtnSize.y)/*Header Space*/ + scaledFontSize * 1.3f };

				switch (g_UIContext.SelectedTab) {
				case UIContext::MainTabs::GameMode:
				{
					window->DrawList->AddText(g_ImGuiFont_RussoOne256, scaledFontSize * 9.6f, pos,
						SwapColorEndianness(0xFFFFFF10), "游戏模式");
				}
				break;

				case UIContext::MainTabs::Player:
				{
					window->DrawList->AddText(g_ImGuiFont_RussoOne256, scaledFontSize * 9.6f, pos,
						SwapColorEndianness(0xFFFFFF10), "玩家");
				}
				break;

				case UIContext::MainTabs::Quickplay:
				{
					window->DrawList->AddText(g_ImGuiFont_RussoOne256, scaledFontSize * 9.6f, pos,
						SwapColorEndianness(0xFFFFFF10), "快速游玩");
				}
				break;

				case UIContext::MainTabs::MusicSwitcher:
				{
					window->DrawList->AddText(g_ImGuiFont_RussoOne256, scaledFontSize * 9.6f, pos,
						SwapColorEndianness(0xFFFFFF10), "音乐切换");
				}
				break;

				case UIContext::MainTabs::Options:
				{
					// Has subtab buttons
					pos += ImVec2{ 0.0f, subTabBtnSize.y };

					if (g_UIContext.SelectedOptionsSubTab == UIContext::OptionsSubTabs::Gameplay) {
						window->DrawList->AddText(g_ImGuiFont_RussoOne256, scaledFontSize * 9.6f, pos,
							SwapColorEndianness(0xFFFFFF10), "玩法");
					}
					else if (g_UIContext.SelectedOptionsSubTab == UIContext::OptionsSubTabs::Camera) {
						window->DrawList->AddText(g_ImGuiFont_RussoOne256, scaledFontSize * 9.6f, pos,
							SwapColorEndianness(0xFFFFFF10), "镜头");
					}
					else if (g_UIContext.SelectedOptionsSubTab == UIContext::OptionsSubTabs::Hotkeys) {
						window->DrawList->AddText(g_ImGuiFont_RussoOne256, scaledFontSize * 9.6f, pos,
							SwapColorEndianness(0xFFFFFF10), "快捷键");
					}
					else if (g_UIContext.SelectedOptionsSubTab == UIContext::OptionsSubTabs::Interface) {
						window->DrawList->AddText(g_ImGuiFont_RussoOne256, scaledFontSize * 9.6f, pos,
							SwapColorEndianness(0xFFFFFF10), "界面");
					}
					else if (g_UIContext.SelectedOptionsSubTab == UIContext::OptionsSubTabs::Sound) {
						window->DrawList->AddText(g_ImGuiFont_RussoOne256, scaledFontSize * 9.6f, pos,
							SwapColorEndianness(0xFFFFFF10), "音效");
					}
					else if (g_UIContext.SelectedOptionsSubTab == UIContext::OptionsSubTabs::Visual) {
						window->DrawList->AddText(g_ImGuiFont_RussoOne256, scaledFontSize * 9.6f, pos,
							SwapColorEndianness(0xFFFFFF10), "画面");
					}
					else if (g_UIContext.SelectedOptionsSubTab == UIContext::OptionsSubTabs::System) {
						window->DrawList->AddText(g_ImGuiFont_RussoOne256, scaledFontSize * 9.6f, pos,
							SwapColorEndianness(0xFFFFFF10), "系统");
					}


				}
				break;

				case UIContext::MainTabs::CheatsAndDebug:
				{
					// Has subtab buttons
					pos += ImVec2{ 0.0f, subTabBtnSize.y + g_UIContext.DefaultFontSize * 0.3f };


					if (g_UIContext.SelectedCheatsAndDebugSubTab == UIContext::CheatsAndDebugSubTabs::CommonCheats) {
						window->DrawList->AddText(g_ImGuiFont_RussoOne256, scaledFontSize * 9.6f, pos,
							SwapColorEndianness(0xFFFFFF10), "通用作弊");
					}
					else if (g_UIContext.SelectedCheatsAndDebugSubTab == UIContext::CheatsAndDebugSubTabs::CharacterCheats) {
						window->DrawList->AddText(g_ImGuiFont_RussoOne256, scaledFontSize * 9.6f, pos,
							SwapColorEndianness(0xFFFFFF10), "角色作弊");
					}
					else if (g_UIContext.SelectedCheatsAndDebugSubTab == UIContext::CheatsAndDebugSubTabs::Teleporter) {
						window->DrawList->AddText(g_ImGuiFont_RussoOne256, scaledFontSize * 9.6f, pos,
							SwapColorEndianness(0xFFFFFF10), "传送");
					}
					else if (g_UIContext.SelectedCheatsAndDebugSubTab == UIContext::CheatsAndDebugSubTabs::EnemySpawner) {
						window->DrawList->AddText(g_ImGuiFont_RussoOne256, scaledFontSize * 9.6f, pos,
							SwapColorEndianness(0xFFFFFF10), "敌人生成器");
					}
					else if (g_UIContext.SelectedCheatsAndDebugSubTab == UIContext::CheatsAndDebugSubTabs::FMODJukebox) {
						window->DrawList->AddText(g_ImGuiFont_RussoOne256, scaledFontSize * 9.6f, pos,
							SwapColorEndianness(0xFFFFFF10), "FMOD 点唱机");
					}
				}
				break;

				default:
					break;
				}
			}
		}

		// Footer of the window
		{
			// Footer background
			window->DrawList->AddRectFilled({ wndRect.Min.x, wndRect.Max.y - scaledFontSize * 2.24f }, { wndRect.Max.x, wndRect.Max.y },
				SwapColorEndianness(0x1F1718FF), window->WindowRounding, ImDrawCornerFlags_Bot);

			contentMaxHeightOffsetFromBottom = scaledFontSize * 2.24f;

			ImVec2 footerInfoBtnSize{ 0.0f, scaledFontSize * 1.7f };

			// Left footer section
			{
				constexpr auto PATREON_BUTTON_TEXT = "PATREON";

				float aboutButtonWidth = ImGui::CalcTextSize(PATREON_BUTTON_TEXT).x + style.FramePadding.x * 2.0f;

				ImVec2 pos{ wndRect.Min.x + scaledFontSize * 1.0f, wndRect.Max.y - scaledFontSize * 1.68f };

				auto cursorPosBackup = ImGui::GetCursorScreenPos();

				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 7.0f);
				ImGui::PushFont(g_ImGuiFont_RussoOne[g_UIContext.DefaultFontSize]);

				ImGui::SetCursorScreenPos(pos + ImVec2{ 0.0f, scaledFontSize * 0.5f - footerInfoBtnSize.y * 0.5f });
				if (InfoButton(PATREON_BUTTON_TEXT, footerInfoBtnSize)) {
					ShellExecute(0, 0, "https://www.patreon.com/berthrage", 0, 0, SW_SHOW);
				}

				ImGui::PopFont();
				ImGui::PopStyleVar();

				ImGui::SetCursorScreenPos(cursorPosBackup);
			}

			// Middle footer section
			{
				constexpr auto BACKGROUND_FADED_TEXT = u8"C•Team";
				constexpr auto CREDIT_TEXT = u8"Berthrage • SSSiyan • deepdarkkapustka • Darkness • Charlie • The Hitchhiker • RaccMoon • Lenam • Airdragon • ZippoIG • Vainiuss1 ";
				constexpr auto ABOUT_BUTTON_TEXT = "关于";

				ImGui::PushFont(g_ImGuiFont_Roboto[g_UIContext.DefaultFontSize * 0.85f]);

				float creditTextWidth = ImGui::CalcTextSize((const char*)CREDIT_TEXT).x;
				float aboutButtonWidth = ImGui::CalcTextSize(ABOUT_BUTTON_TEXT).x + style.FramePadding.x * 2.0f;

				ImVec2 pos{ (wndRect.Min.x + wndRect.Max.x) * 0.5f - (creditTextWidth + aboutButtonWidth) * 0.5f, wndRect.Max.y - scaledFontSize * 1.68f };

				// Background faded text
				ImVec2 bgFadedTextSize = ImGui::CalcTextSize((const char*)BACKGROUND_FADED_TEXT);
				window->DrawList->AddText(g_ImGuiFont_RussoOne256, scaledFontSize * 4.8f,
					{ pos.x - scaledFontSize * 5.8f,
					pos.y - scaledFontSize * 4.8f * 0.5f },
					SwapColorEndianness(0xFFFFFF10), (const char*)BACKGROUND_FADED_TEXT);

				window->DrawList->AddText(pos, SwapColorEndianness(0xFFFFFFFF), (const char*)CREDIT_TEXT);

				ImGui::PopFont();

				auto cursorPosBackup = ImGui::GetCursorScreenPos();

				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 7.0f);
				ImGui::PushFont(g_ImGuiFont_RussoOne[g_UIContext.DefaultFontSize]);

				ImGui::SetCursorScreenPos(pos + ImVec2{ creditTextWidth, scaledFontSize * 0.5f - footerInfoBtnSize.y * 0.5f });
				if (InfoButton(ABOUT_BUTTON_TEXT, footerInfoBtnSize)) {
					g_UIContext.SelectedTab = UIContext::MainTabs::None;
				}

				ImGui::PopFont();
				ImGui::PopStyleVar();

				ImGui::SetCursorScreenPos(cursorPosBackup);
			}

			// Right footer section
			{
				constexpr auto CREDIT_TEXT = "基于 serpentiem 的 ";
				constexpr auto DDMK_BUTTON_TEXT = "DDMK";

				ImGui::PushFont(g_ImGuiFont_Roboto[g_UIContext.DefaultFontSize]);

				float creditTextWidth = ImGui::CalcTextSize(CREDIT_TEXT).x;
				float aboutBtnWidth = ImGui::CalcTextSize(DDMK_BUTTON_TEXT).x + style.FramePadding.x * 2.0f;

				ImVec2 pos{ wndRect.Max.x - creditTextWidth - aboutBtnWidth - scaledFontSize, wndRect.Max.y - scaledFontSize * 1.68f };

				window->DrawList->AddText(pos, SwapColorEndianness(0xFFFFFFFF), CREDIT_TEXT);

				ImGui::PopFont();

				auto cursorPosBackup = ImGui::GetCursorScreenPos();

				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 7.0f);
				ImGui::PushFont(g_ImGuiFont_RussoOne[g_UIContext.DefaultFontSize]);

				ImGui::SetCursorScreenPos(pos + ImVec2{ creditTextWidth, scaledFontSize * 0.5f - footerInfoBtnSize.y * 0.5f });
				if (InfoButton(DDMK_BUTTON_TEXT, footerInfoBtnSize)) {
					ShellExecute(0, 0, "https://github.com/serpentiem/ddmk/", 0, 0, SW_SHOW);
				}

				ImGui::PopFont();
				ImGui::PopStyleVar();

				ImGui::SetCursorScreenPos(cursorPosBackup);
			}
		}

		// Actual content
		{
			ImVec2 contentMin = { wndRect.Min.x + scaledFontSize * 0.3f,
								  wndRect.Min.y + contentMinHeightOffsetFromTop + scaledFontSize * 0.3f };

			ImGui::SetNextWindowPos(contentMin, ImGuiCond_Always);
			ImGui::BeginChildEx("内容窗口", window->GetID("内容窗口"),
				window->Size - ImVec2{ scaledFontSize * 0.3f * 2.0f, (contentMinHeightOffsetFromTop + scaledFontSize * 0.3f + contentMaxHeightOffsetFromBottom) }, false, 0);
			ImGui::SetWindowFontScale(scaleFactorY);
			DrawMainContent(pDevice, g_UIContext);
			ImGui::EndChild();
		}
	}
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
	ImGui::End();
	ImGui::PopFont();
}
}

bool visibleMain = false;
bool lastVisibleMain = false;

bool visibleShop = false;
bool lastVisibleShop = false;

bool updateWeaponWheel = false;

#pragma region Common

const char* saveNamesDante[] = {
	"但丁 0000",
	"但丁 0001",
	"但丁 0002",
	"但丁 0003",
	"但丁 0004",
	"但丁 0005",
	"但丁 0006",
	"但丁 0007",
	"但丁 0008",
	"但丁 0009",
};


const char* saveNamesVergil[] = {
	"维吉尔 0000",
	"维吉尔 0001",
	"维吉尔 0002",
	"维吉尔 0003",
	"维吉尔 0004",
	"维吉尔 0005",
	"维吉尔 0006",
	"维吉尔 0007",
	"维吉尔 0008",
	"维吉尔 0009",
};


const char* indexNames[] = {
	"0000",
	"0001",
	"0002",
	"0003",
	"0004",
	"0005",
	"0006",
	"0007",
	"0008",
	"0009",
	"0010",
	"0011",
	"0012",
	"0013",
	"0014",
	"0015",
	"0016",
	"0017",
	"0018",
	"0019",
	"0020",
	"0021",
	"0022",
	"0023",
	"0024",
	"0025",
	"0026",
	"0027",
	"0028",
	"0029",
	"0030",
	"0031",
	"0032",
	"0033",
	"0034",
	"0035",
	"0036",
	"0037",
	"0038",
	"0039",
	"0040",
	"0041",
	"0042",
	"0043",
	"0044",
	"0045",
	"0046",
	"0047",
	"0048",
	"0049",
	"0050",
	"0051",
	"0052",
	"0053",
	"0054",
	"0055",
	"0056",
	"0057",
	"0058",
	"0059",
	"0060",
	"0061",
	"0062",
	"0063",
	"0064",
	"0065",
	"0066",
	"0067",
	"0068",
	"0069",
	"0070",
	"0071",
	"0072",
	"0073",
	"0074",
	"0075",
	"0076",
	"0077",
	"0078",
	"0079",
	"0080",
	"0081",
	"0082",
	"0083",
	"0084",
	"0085",
	"0086",
	"0087",
	"0088",
	"0089",
	"0090",
	"0091",
	"0092",
	"0093",
	"0094",
	"0095",
	"0096",
	"0097",
	"0098",
	"0099",
};


const char* buttonIndexNames[] = {
	"按键 1",
	"按键 2",
	"按键 3",
	"按键 4",
};

const char* dataNames[] = {
	"数据 0",
	"数据 1",
	"数据 2",
	"数据 3",
	"数据 4",
	"数据 5",
	"数据 6",
	"数据 7",
};

const char* buttonNames[] = {
	"无",
	"左扳机",
	"右扳机",
	"左肩键",
	"右肩键",
	"Y",
	"B",
	"A",
	"X",
	"返回",
	"左摇杆",
	"右摇杆",
	"左右摇杆",
	"开始",
	"Up",
	"右",
	"下",
	"左",
};
const int buttonNamesCount = IM_ARRAYSIZE(buttonNames);

constexpr byte16 buttons[] = {
	0,
	GAMEPAD::LEFT_TRIGGER,
	GAMEPAD::RIGHT_TRIGGER,
	GAMEPAD::LEFT_SHOULDER,
	GAMEPAD::RIGHT_SHOULDER,
	GAMEPAD::Y,
	GAMEPAD::B,
	GAMEPAD::A,
	GAMEPAD::X,
	GAMEPAD::BACK,
	GAMEPAD::LEFT_STICK_CLICK,
	GAMEPAD::RIGHT_STICK_CLICK,
	GAMEPAD::LEFT_PLUS_RIGHT_STICK_CLICK,
	GAMEPAD::START,
	GAMEPAD::UP,
	GAMEPAD::RIGHT,
	GAMEPAD::DOWN,
	GAMEPAD::LEFT,
};

const char* directionNames[] = {
	"Up",
	"右",
	"下",
	"左",
};

const char* playerIndexNames[] = {
	"1P",
	"2P",
	"3P",
	"4P",
};


const char* characterIndexNames[] = {
	"角色 1",
	"角色 2",
	"角色 3",
};

const char* entityNames[] = {
	"本体",
	"分身",
};

const char* entityEnumNames[] = {
	"ENTITY::MAIN",
	"ENTITY::CLONE",
};

const char* ddmkCharacter2PNames[] = {
	"但丁",
	"Bob",
	"蕾蒂",
	"维吉尔",
};


const char* costumeRespectsProgressionNames[] = {
	"Off",
	"原版",
	"Crimson",
};

const char* cameraSensitivityNames[] = {
	"低（原版默认）",
	"中",
	"高",
	"最高",
};

const char* cameraFollowUpSpeedNames[] = {
	"低（原版默认）",
	"中",
	"高",
};

const char* cameraDistanceNames[] = {
	"远（原版默认）",
	"较近",
	"动态",
};

const char* cameraLockOnDistanceNames[] = {
	"较近（原版默认）",
	"Far",
	"动态",
};


const char* cameraTiltNames[] = {
	"原始（原版默认）",
	"更贴近地面",
};

const char* cameraSmoothingNames[] = {
	"最高",
	"高（原版默认）",
	"中",
	"Low",
};

const char* GUITransparencyNames[] = {
	"Off",
	"静态",
	"动态",
};


const char* ddmkCharacterNames[] = {
	"但丁",
	"BoB 维吉尔",
	"蕾蒂",
	"维吉尔",
	"Boss 蕾蒂",
	"Boss 维吉尔",
};

constexpr uint8 ddmkCharactersMap[] = {
	CHARACTER::DANTE,
	CHARACTER::BOB,
	CHARACTER::LADY,
	CHARACTER::VERGIL,
	CHARACTER::BOSS_LADY,
	CHARACTER::BOSS_VERGIL,
};

const char* crimsonCharacterNames[] = {
	"但丁",
	"维吉尔",
};

constexpr uint8 crimsonCharacterMap[] = {
	CHARACTER::DANTE,
	CHARACTER::VERGIL,
};

const char* collisionGroupNames[] = { "玩家", "敌人" };

constexpr uint8 collisionGroups[] = {
	COLLISION_GROUP::PLAYER,
	COLLISION_GROUP::ENEMY,
};

const char* styleNamesDante[] = {
	"剑圣",
	"枪神",
	"骗术师",
	"皇家护卫",
	"水银",
	"分身",
};

const char* styleNamesFX[] = {
	"骗术",
	"剑圣",
	"Gun",
	"皇卫",
	"水银",
	"分身",
	"DT",
	"DTE",
	"Ready!"
};

constexpr uint8 stylesDante[] = {
	STYLE::SWORDMASTER,
	STYLE::GUNSLINGER,
	STYLE::TRICKSTER,
	STYLE::ROYALGUARD,
	STYLE::QUICKSILVER,
	STYLE::DOPPELGANGER,
};

const char* styleNamesVergil[] = {
	"暗黑骑士",
	"分身",
	"水银",
};

const char* styleNamesDanteGameplay[] = {
	"剑圣",
	"枪神",
	"骗术师",
	"皇家护卫",
	"水银",
	"分身",
};

const char* styleNamesVergilGameplay[] = {
	"占位符",
	"占位符",
	"暗黑骑士",
	"占位符",
	"水银",
	"分身",
};

constexpr uint8 stylesVergil[] = {
	STYLE::DARK_SLAYER,
	STYLE::DOPPELGANGER,
	STYLE::QUICKSILVER,
};

const char* meleeWeaponNamesDante[] = {
	"叛逆",
	"刻耳柏洛斯",
	"阿耆尼与楼陀罗",
	"涅梵",
	"贝奥武夫",
};

constexpr uint8 meleeWeaponsDante[] = {
	WEAPON::REBELLION,
	WEAPON::CERBERUS,
	WEAPON::AGNI_RUDRA,
	WEAPON::NEVAN,
	WEAPON::BEOWULF_DANTE,
};

const char* meleeWeaponNamesVergil[] = {
	"阎魔刀",
	"贝奥武夫",
	"力量之刃",
};

constexpr uint8 meleeWeaponsVergil[] = {
	WEAPON::YAMATO_VERGIL,
	WEAPON::BEOWULF_VERGIL,
	WEAPON::YAMATO_FORCE_EDGE,
};

const char* rangedWeaponNamesDante[] = {
	"黑檀木与白象牙",
	"霰弹枪",
	"阿尔忒弥斯",
	"螺旋",
	"卡琳娜·安",
};

constexpr uint8 rangedWeaponsDante[] = {
	WEAPON::EBONY_IVORY,
	WEAPON::SHOTGUN,
	WEAPON::ARTEMIS,
	WEAPON::SPIRAL,
	WEAPON::KALINA_ANN,
};

const char* costumeNamesDante[] = {
	"DMC3",
	"DMC3 无外套",
	"DMC3 破损",
	"DMC1",
	"DMC1 无外套",
	"斯巴达",
	"超级但丁",
	"超级 DMC1 但丁",
};

const char* costumeNamesVergil[] = {
	"DMC3",
	"DMC3 无外套",
	"超级维吉尔",
	"腐化维吉尔",
	"超级腐化维吉尔",
};

const char* costumeNamesLady[] = {
	"DMC3",
	"骑手服",
};

const char* weaponSwitchTypeNames[] = {
	"线性",
	"任意",
};

const char* stickNames[] = {
	"右摇杆",
	"左摇杆",
};

const char* rightStickCenterCamNames[] = {
	"Off",
	"就近侧",
	"On",
};

uint8 rightStickCenterCamMap[] = {
	0,
	1,
	2,
};

const char* cameraShakeNames[] = {
	"Off",
	"仅单人镜头下",
	"始终开启",
};

uint8 cameraShakeMap[] = {
	0,
	1,
	2,
};

const char* styleSwitchTypeNames[] = {
	"Crimson",
	"DMC3 切换",
};

uint8 styleSwitchTypeMap[] = {
	0,
	1,
};

const char* styleRankNames[] = {
	"无",
	"酷",
	"疯狂",
	"爆发",
	"不错",
	"精彩",
	"好戏上演",
	"华丽",
};

const char* missionNames[] = {
	"电影",
	"任务 1",
	"任务 2",
	"任务 3",
	"任务 4",
	"任务 5",
	"任务 6",
	"任务 7",
	"任务 8",
	"任务 9",
	"任务 10",
	"任务 11",
	"任务 12",
	"任务 13",
	"任务 14",
	"任务 15",
	"任务 16",
	"任务 17",
	"任务 18",
	"任务 19",
	"任务 20",
	"血宫",
};

const char* modeNames[] = {
	"简单",
	"普通",
	"困难",
	"极难",
	"但丁必须死",
	"天堂或地狱",
};

constexpr uint32 modes[] = {
	DIFFICULTY_MODE::EASY,
	DIFFICULTY_MODE::NORMAL,
	DIFFICULTY_MODE::HARD,
	DIFFICULTY_MODE::VERY_HARD,
	DIFFICULTY_MODE::DANTE_MUST_DIE,
	DIFFICULTY_MODE::HEAVEN_OR_HELL,
};

const char* floorNames[] = {
	"第 1 层",
	"第 2 层",
	"第 3 层",
	"第 4 层",
	"第 5 层",
	"第 6 层",
	"第 7 层",
	"第 8 层",
	"第 9 层",
	"第 10 层",
	"刻耳柏洛斯",
	"巨蜈蚣",
	"阿耆尼与楼陀罗",
	"涅梵",
	"贝奥武夫",
	"格里昂",
	"分身",
	"利维坦",
	"诅咒棋子",
	"维吉尔 1",
	"维吉尔 2",
	"维吉尔 3",
	"蕾蒂",
	"阿卡姆",
	"小丑 1",
	"小丑 2",
	"小丑 3",
};

const char* sceneNames[] = {
	"SCENE::BOOT",
	"SCENE::INTRO",
	"SCENE::MAIN",
	"SCENE::MISSION_SELECT",
	"SCENE::LOAD",
	"SCENE::GAME",
	"SCENE::CUTSCENE",
	"SCENE::MISSION_START",
	"SCENE::MISSION_RESULT",
	"SCENE::GAME_OVER",
};

const char* eventNames[] = {
	"EVENT::INIT",
	"EVENT::MAIN",
	"EVENT::TELEPORT",
	"EVENT::PAUSE",
	"EVENT::STATUS",
	"EVENT::OPTIONS",
	"EVENT::DEATH",
	"EVENT::ITEM",
	"EVENT::MESSAGE",
	"EVENT::CUSTOMIZE",
	"EVENT::SAVE",
	"EVENT::DELETE",
	"EVENT::END",
};

const char* trackFilenames[] = {
	"Battle_00.ogg",
	"Battle_01.ogg",
	"Battle_01b.ogg",
	"Battle_02.ogg",
	"Battle_03.ogg",
	"Battle_04.ogg",
	"Battle_05.ogg",
	"Battle_06.ogg",
	"Battle_07.ogg",
	"Battle_08.ogg",
	"Battle_0a.ogg",
	"Boss_01.ogg",
	"Boss_01b.ogg",
	"Boss_02.ogg",
	"Boss_02b.ogg",
	"Boss_03.ogg",
	"Boss_03b.ogg",
	"Boss_04.ogg",
	"Boss_04b.ogg",
	"Boss_05.ogg",
	"Boss_05b.ogg",
	"Boss_06.ogg",
	"Boss_06b.ogg",
	"Boss_07.ogg",
	"Boss_08.ogg",
	"Boss_08b.ogg",
	"Boss_09.ogg",
	"Continue.ogg",
	"dummy.ogg",
	"Hine_01.ogg",
	"Hine_02.ogg",
	"Jester.ogg",
	"Jikushinzou.ogg",
	"Kaze_01.ogg",
	"Kaze_02.ogg",
	"Kaze_03.ogg",
	"Kaze_04.ogg",
	"Kaze_05.ogg",
	"Kaze_06.ogg",
	"Lady.ogg",
	"m01_c00.ogg",
	"m01_v00.ogg",
	"m01_v01.ogg",
	"m01_v02.ogg",
	"m01_v03.ogg",
	"m01_v04.ogg",
	"m01_v05.ogg",
	"m01_v06.ogg",
	"m01_v07.ogg",
	"m01_v08.ogg",
	"m01_v09.ogg",
	"m02_c01.ogg",
	"m02_s00.ogg",
	"m03_b00.ogg",
	"m03_b01.ogg",
	"m03_c00.ogg",
	"m03_s00.ogg",
	"m03_s01.ogg",
	"m04_c00.ogg",
	"m04_s00.ogg",
	"m05_b00.ogg",
	"m05_c00.ogg",
	"m05_s00.ogg",
	"m06_c00.ogg",
	"m07_b00.ogg",
	"m07_s00.ogg",
	"m08_c00.ogg",
	"m09_b00.ogg",
	"m09_b01.ogg",
	"m09_c00.ogg",
	"m10_c00.ogg",
	"m10_s00.ogg",
	"m11_b00.ogg",
	"m11_b01.ogg",
	"m11_c00.ogg",
	"m11_s00.ogg",
	"m12_b00.ogg",
	"m12_b01.ogg",
	"m12_b02.ogg",
	"m12_c00.ogg",
	"m12_s00.ogg",
	"m13_b00.ogg",
	"m13_s00.ogg",
	"m14_s01.ogg",
	"m15_c00.ogg",
	"m15_s00.ogg",
	"m16_b00.ogg",
	"m16_c00.ogg",
	"m16_s00.ogg",
	"m17_b00.ogg",
	"m17_c00.ogg",
	"m17_s00.ogg",
	"m18_c00.ogg",
	"m18_s00.ogg",
	"m19_b00.ogg",
	"m19_b01.ogg",
	"m19_s00.ogg",
	"m20_b00.ogg",
	"m20_c00.ogg",
	"m20_s00.ogg",
	"m99_c00.ogg",
	"m99_c01.ogg",
	"m99_c03.ogg",
	"Maguma.ogg",
	"Mizu_01.ogg",
	"Mizu_02.ogg",
	"M_clear.ogg",
	"m_demo_02_001.ogg",
	"m_demo_04_004.ogg",
	"m_demo_04_004v.ogg",
	"m_demo_04_014.ogg",
	"m_demo_06_007.ogg",
	"m_demo_06_011.ogg",
	"m_demo_06_012.ogg",
	"m_demo_09_003.ogg",
	"m_demo_09_003v.ogg",
	"m_demo_10_002.ogg",
	"m_demo_10_002v.ogg",
	"m_demo_10_004.ogg",
	"m_demo_10_005.ogg",
	"m_demo_12_004.ogg",
	"m_demo_12_005.ogg",
	"m_demo_15_001.ogg",
	"m_demo_15_001v.ogg",
	"m_demo_19_001.ogg",
	"m_demo_19_016.ogg",
	"M_start.ogg",
	"M_start2.ogg",
	"Nausica.ogg",
	"Option.ogg",
	"Result.ogg",
	"Room_01.ogg",
	"Sm_clear.ogg",
	"Sm_failure.ogg",
	"Staffroll.ogg",
	"Stage_01.ogg",
	"Stage_02.ogg",
	"Stage_02b.ogg",
	"Stage_03.ogg",
	"Stage_04.ogg",
	"Stage_04b.ogg",
	"Stage_05.ogg",
	"Stage_05b.ogg",
	"Stage_06.ogg",
	"Stage_07.ogg",
	"Stage_08.ogg",
	"Stage_09.ogg",
	"Torocco.ogg",
	"T_boss.ogg",
	"T_bossb.ogg",
	"Versil_01.ogg",
	"Versil_02.ogg",
	"Versil_03.ogg",
	"ZZZ.ogg",
};

const char* trackNames[] = {
	"Battle_00",
	"Battle_01",
	"Battle_01b",
	"Battle_02",
	"Battle_03",
	"Battle_04",
	"Battle_05",
	"Battle_06",
	"Battle_07",
	"Battle_08",
	"Battle_0a",
	"Boss_01",
	"Boss_01b",
	"Boss_02",
	"Boss_02b",
	"Boss_03",
	"Boss_03b",
	"Boss_04",
	"Boss_04b",
	"Boss_05",
	"Boss_05b",
	"Boss_06",
	"Boss_06b",
	"Boss_07",
	"Boss_08",
	"Boss_08b",
	"Boss_09",
	"继续",
	"dummy",
	"Hine_01",
	"Hine_02",
	"小丑",
	"Jikushinzou",
	"Kaze_01",
	"Kaze_02",
	"Kaze_03",
	"Kaze_04",
	"Kaze_05",
	"Kaze_06",
	"蕾蒂",
	"m01_c00",
	"m01_v00",
	"m01_v01",
	"m01_v02",
	"m01_v03",
	"m01_v04",
	"m01_v05",
	"m01_v06",
	"m01_v07",
	"m01_v08",
	"m01_v09",
	"m02_c01",
	"m02_s00",
	"m03_b00",
	"m03_b01",
	"m03_c00",
	"m03_s00",
	"m03_s01",
	"m04_c00",
	"m04_s00",
	"m05_b00",
	"m05_c00",
	"m05_s00",
	"m06_c00",
	"m07_b00",
	"m07_s00",
	"m08_c00",
	"m09_b00",
	"m09_b01",
	"m09_c00",
	"m10_c00",
	"m10_s00",
	"m11_b00",
	"m11_b01",
	"m11_c00",
	"m11_s00",
	"m12_b00",
	"m12_b01",
	"m12_b02",
	"m12_c00",
	"m12_s00",
	"m13_b00",
	"m13_s00",
	"m14_s01",
	"m15_c00",
	"m15_s00",
	"m16_b00",
	"m16_c00",
	"m16_s00",
	"m17_b00",
	"m17_c00",
	"m17_s00",
	"m18_c00",
	"m18_s00",
	"m19_b00",
	"m19_b01",
	"m19_s00",
	"m20_b00",
	"m20_c00",
	"m20_s00",
	"m99_c00",
	"m99_c01",
	"m99_c03",
	"Maguma",
	"Mizu_01",
	"Mizu_02",
	"M_clear",
	"m_demo_02_001",
	"m_demo_04_004",
	"m_demo_04_004v",
	"m_demo_04_014",
	"m_demo_06_007",
	"m_demo_06_011",
	"m_demo_06_012",
	"m_demo_09_003",
	"m_demo_09_003v",
	"m_demo_10_002",
	"m_demo_10_002v",
	"m_demo_10_004",
	"m_demo_10_005",
	"m_demo_12_004",
	"m_demo_12_005",
	"m_demo_15_001",
	"m_demo_15_001v",
	"m_demo_19_001",
	"m_demo_19_016",
	"M_start",
	"M_start2",
	"Nausica",
	"选项",
	"结算",
	"Room_01",
	"Sm_clear",
	"Sm_failure",
	"制作人员名单",
	"Stage_01",
	"Stage_02",
	"Stage_02b",
	"Stage_03",
	"Stage_04",
	"Stage_04b",
	"Stage_05",
	"Stage_05b",
	"Stage_06",
	"Stage_07",
	"Stage_08",
	"Stage_09",
	"Torocco",
	"T_boss",
	"T_bossb",
	"Versil_01",
	"Versil_02",
	"Versil_03",
	"ZZZ",
};

std::vector<std::string> SwordFormationShortcutNames = {
	"Off",
	"On",
	"反转",
};

std::vector<std::string> weaponWheelThemeNames = {
	"Crimson",
	"绯红早期测试版",
	"DMC3 切换",
};

const char* hudElementScaleStateNames[] = {
	"小",
	"Big",
};

constexpr uint8 hudElementScaleStateMap[2] = {
	HUDELEMENTSCALESTATE::SMALL,
	HUDELEMENTSCALESTATE::BIG,
};

const char* hudElementShowStateNames[] = {
	"Off",
	"仅多人模式",
	"始终",
};

constexpr uint8 hudElementShowStateMap[3] = {
	HUDELEMENTSHOWSTATE::OFF,
	HUDELEMENTSHOWSTATE::ONLY_IN_MP,
	HUDELEMENTSHOWSTATE::ALWAYS,
};

const char* stylesDisplayStateNames[] = {
	"Off",
	"有碎玻璃",
	"无碎玻璃",
};

constexpr uint8 stylesDisplayStateMap[3] = {
	STYLESDISPLAY::OFF,
	STYLESDISPLAY::WITH_BROKEN_GLASS,
	STYLESDISPLAY::NO_BROKEN_GLASS,
};

const char* styleNamesTextureSetNames[] = {
	"Crimson",
	"DMC3 切换",
};

constexpr uint8 styleNamesTextureSetMap[2] = {
	STYLENAMESSET::CRIMSON,
	STYLENAMESSET::DMC3_SWITCH,
};

const char* missionTimerDisplayStateNames[] = {
	"Off",
	"仅血宫",
	"始终",
};

constexpr uint8 missionTimerDisplayStateMap[3] = {
	MISSIONTIMERDISPLAY::OFF,
	MISSIONTIMERDISPLAY::ONLY_IN_BP,
	MISSIONTIMERDISPLAY::ALWAYS,
};

const char* ldkModeNames[] = {
	"Off",
	"LDK",
	"超级 LDK",
	"超级 LDK + Boss",
};

constexpr uint8 ldkModes[] = {
	LDKMODE::OFF,
	LDKMODE::LDK,
	LDKMODE::SUPER_LDK,
	LDKMODE::SUPER_LDK_BOSSES,
};

const char* enemyDTModeNames[] = {
	"默认",
	"敌人立即魔人化",
	"敌人不魔人化",
};

constexpr uint8 enemyDTModes[] = {
	ENEMYDTMODE::DEFAULT,
	ENEMYDTMODE::INSTANT_DT,
	ENEMYDTMODE::NO_ENEMY_DT,
};

const char* forceDifficultyNames[] = {
	"Off",
	"简单",
	"普通",
	"困难",
	"极难",
	"但丁必须死",
	"天堂或地狱",
};

constexpr uint32 forceDifficultyModes[] = {
	DIFFICULTY_MODE::FORCE_DIFFICULTY_OFF,
	DIFFICULTY_MODE::EASY,
	DIFFICULTY_MODE::NORMAL,
	DIFFICULTY_MODE::HARD,
	DIFFICULTY_MODE::VERY_HARD,
	DIFFICULTY_MODE::DANTE_MUST_DIE,
	DIFFICULTY_MODE::HEAVEN_OR_HELL,
};

constexpr uint8 delayedComboSFXMap[] = {
	DELAYEDCOMBOSFX::TYPE_A,
	DELAYEDCOMBOSFX::TYPE_B,
};

const char* delayedComboSFXNames[] = {
	"类型 A",
	"类型 B",
};


static_assert(countof(trackFilenames) == countof(trackNames));

#pragma endregion

float GetFadeToBlackProgress(float targetTotal = 60.0f) {
	auto transitionAmount = *reinterpret_cast<float*>(appBaseAddr + 0x5CFAA4);
	auto transitionTotal = *reinterpret_cast<float*>(appBaseAddr + 0x5CFAA8);
	if (transitionTotal != targetTotal)
		return 0.0f;
	auto percentage = (transitionAmount / transitionTotal);
	percentage = (std::max)(0.0f, percentage);
	percentage = (std::min)(1.0f, percentage);
	return percentage;
}
ImVec4 LerpFadeToBlack(ImVec4 color, float target) {
	return ImLerp(color, ImVec4(0, 0, 0, color.w), GetFadeToBlackProgress(target)); // default is white
}
ImVec4 LerpFadeToBlack(ImVec4 color) {
	return ImLerp(color, ImVec4(0, 0, 0, color.w), GetFadeToBlackProgress()); // default is white
}

void PauseWhenGUIOpened() {
	if (activeCrimsonConfig.GUI.pauseWhenOpened) {
		if (g_show) {
			CrimsonPatches::PauseGameTime(true);
			auto pool_C90E10 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
			if (!pool_C90E10 || !pool_C90E10[5]) {
				return;
			}
			auto& cSceneGameMain = *reinterpret_cast<CSceneGameMain*>(pool_C90E10[5]);
			if (cSceneGameMain.event == EVENT::MAIN) {
				g_inGUIPause = true;
			}
		} else {
			CrimsonPatches::PauseGameTime(false);
			g_inGUIPause = false;
		}
	} else {
		CrimsonPatches::PauseGameTime(false);
		g_inGUIPause = false;
	}
}

std::unique_ptr<WW::WeaponWheel> dummyWeaponWheel;
std::unique_ptr<WW::WeaponWheel> meleeWeaponWheel[PLAYER_COUNT];
std::unique_ptr<WW::WeaponWheel> rangedWeaponWheel[PLAYER_COUNT];
std::unique_ptr<WW::WeaponWheel>  meleeWorldSpaceWeaponWheel[PLAYER_COUNT];
std::unique_ptr<WW::WeaponWheel>  rangedWorldSpaceWeaponWheel[PLAYER_COUNT];
bool multiplayerWheelsLoaded = false;

#pragma endregion

#pragma region Weapon Wheel Controllers

void UpdateMeleeWeaponIDs(PlayerActorData& actorData, std::vector<WW::WeaponIDs>(&currentWeapons)[CHARACTER_COUNT], size_t charIdx) {
	auto playerIndex = actorData.newPlayerIndex;
	auto playerData = GetPlayerData(playerIndex);
	auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

	for (size_t i = 0; i < currentWeapons[charIdx].size(); i++) {
		auto& characterData = playerData.characterData[charIdx][ENTITY::MAIN];

		if (characterData.character == CHARACTER::DANTE) {
			switch (characterData.meleeWeapons[i]) {

			case WEAPON::REBELLION:
				if (!characterData.sparda) {
					if (sessionData.unlockDevilTrigger)
						currentWeapons[charIdx][i] = WW::WeaponIDs::RebellionAwakened;
					else
						currentWeapons[charIdx][i] = WW::WeaponIDs::RebellionDormant;
				}
				else {
					currentWeapons[charIdx][i] = WW::WeaponIDs::ForceEdge;
				}

				break;

			case WEAPON::CERBERUS:
				currentWeapons[charIdx][i] = WW::WeaponIDs::Cerberus;
				break;

			case WEAPON::BEOWULF_DANTE:
				currentWeapons[charIdx][i] = WW::WeaponIDs::Beowulf;
				break;

			case WEAPON::AGNI_RUDRA:
				currentWeapons[charIdx][i] = WW::WeaponIDs::AgniRudra;
				break;

			case WEAPON::NEVAN:
				currentWeapons[charIdx][i] = WW::WeaponIDs::Nevan;
				break;
			}
		}
		else if (characterData.character == CHARACTER::VERGIL) {
			switch (characterData.meleeWeapons[i]) {

			case WEAPON::YAMATO_VERGIL:
				currentWeapons[charIdx][i] = WW::WeaponIDs::Yamato;
				break;

			case WEAPON::BEOWULF_VERGIL:
				currentWeapons[charIdx][i] = WW::WeaponIDs::Beowulf;
				break;

			case WEAPON::YAMATO_FORCE_EDGE:
				currentWeapons[charIdx][i] = WW::WeaponIDs::ForceEdgeYamato;
				break;

			}
		}
	}
}

void UpdateRangedWeaponIDs(PlayerActorData& actorData, std::vector<WW::WeaponIDs>(&currentWeapons)[CHARACTER_COUNT], size_t charIdx) {
	auto playerIndex = actorData.newPlayerIndex;
	auto playerData = GetPlayerData(playerIndex);

	for (size_t i = 0; i < currentWeapons[charIdx].size(); i++) {
		auto& characterData = playerData.characterData[charIdx][ENTITY::MAIN];

		if (characterData.character == CHARACTER::DANTE) {
			switch (characterData.rangedWeapons[i]) {

			case WEAPON::EBONY_IVORY:
				currentWeapons[charIdx][i] = WW::WeaponIDs::EbonyIvory;
				break;

			case WEAPON::SHOTGUN:
				currentWeapons[charIdx][i] = WW::WeaponIDs::Shotgun;
				break;

			case WEAPON::ARTEMIS:
				currentWeapons[charIdx][i] = WW::WeaponIDs::Artemis;
				break;

			case WEAPON::SPIRAL:
				currentWeapons[charIdx][i] = WW::WeaponIDs::Spiral;
				break;

			case WEAPON::KALINA_ANN:
				currentWeapons[charIdx][i] = WW::WeaponIDs::KalinaAnn;
				break;
			}
		}
	}
}

struct WeaponWheelState {
	int oldWeaponIndex = -1;
	int oldCharIndex = -1;
	WW::WheelThemes charTheme = WW::WheelThemes::Neutral;
	std::string oldTheme = activeCrimsonConfig.WeaponWheel.theme;
	std::vector<WW::WeaponIDs> currentWeapons[CHARACTER_COUNT];
	std::array<std::array<uint8_t, 5>, CHARACTER_COUNT> oldCharWeapons;
	std::array<uint8_t, CHARACTER_COUNT> oldPlayerDataChars;
	std::array<uint8_t, CHARACTER_COUNT> oldCharCostumes;
	bool oldUnlockedDevilTrigger = false;
	bool initializedOld = false;
	double startTime = ImGui::GetTime();
};

bool WeaponWheelController(PlayerActorData& actorData, IDXGISwapChain* pSwapChain, std::unique_ptr<WW::WeaponWheel>& pWeaponWheel, 
	const char* windowName, bool cornerPositioning, ImVec2 windowPos, ImVec2 wheelSize, 
	bool alwaysShow, bool trackHide, bool isMelee, WeaponWheelState& state,
	float deltaTimeGameSpeed, float deltaTime) {

	if (!InGame()) {
		return false;
	}
	auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

	auto playerIndex = actorData.newPlayerIndex;
	auto playerData = GetPlayerData(playerIndex);
	auto& characterData = GetCharacterData(actorData);

	auto& gamepad = GetGamepad(actorData.newGamepad);

	if (InCutscene() || InCredits() || !activeConfig.Actor.enable || g_inGameCutscene ||
		!((characterData.character == CHARACTER::DANTE) || (characterData.character == CHARACTER::VERGIL))) {
		return false;
	}

	auto& charIndex = actorData.newCharacterIndex;
	auto& weaponIndex = isMelee ? characterData.meleeWeaponIndex : characterData.rangedWeaponIndex;
	auto& weaponCount = isMelee ? playerData.characterData[charIndex][ENTITY::MAIN].meleeWeaponCount : playerData.characterData[charIndex][ENTITY::MAIN].rangedWeaponCount;
	auto activeGameSpeed = GetCurrentGameSpeed();

	auto weapons = isMelee ? playerData.characterData[charIndex][ENTITY::MAIN].meleeWeapons : playerData.characterData[charIndex][ENTITY::MAIN].rangedWeapons;

	// Feed old arrays for comparisons. -- only once during runtime.
	if (!state.initializedOld) {
		for (size_t charIdx = 0; charIdx < CHARACTER_COUNT; ++charIdx) {
			for (size_t j = 0; j < 5; ++j) {
				state.oldCharWeapons[charIdx][j] = weapons[j];
			}
			state.oldPlayerDataChars[charIdx] = playerData.characterData[charIdx][0].character;
			state.oldCharCostumes[charIdx] = playerData.characterData[charIdx][0].costume;
		}
		state.oldUnlockedDevilTrigger = false;
		state.initializedOld = true;
	}

	// Set character theme based on actor's character.
	switch (actorData.character) {
	case CHARACTER::DANTE:
		state.charTheme = WW::WheelThemes::Dante;
		break;
	case CHARACTER::VERGIL:
		state.charTheme = WW::WheelThemes::Vergil;
		break;
	default:
		state.charTheme = WW::WheelThemes::Neutral;
		break;
	}

	// Create and feed current arrays for comparisons.
	std::array<std::array<uint8_t, 5>, CHARACTER_COUNT> currentCharWeapons;
	std::array<uint8, 3> currentPlayerDataChars;
	std::array<uint8, 3> currentCharCostumes;
	for (size_t charIdx = 0; charIdx < CHARACTER_COUNT; ++charIdx) {
		for (size_t j = 0; j < 5; ++j) {
			currentCharWeapons[charIdx][j] = weapons[j];
		}
		currentPlayerDataChars[charIdx] = playerData.characterData[charIdx][0].character;
		currentCharCostumes[charIdx] = playerData.characterData[charIdx][0].costume;
	}

	if (!g_allActorsSpawned) {
		return false;
	}

	// Reload the Wheel if Character Settings/Loadouts have changed to update all weapon sprites.
	if (state.currentWeapons[charIndex].size() != weaponCount
		|| state.oldCharWeapons[charIndex] != currentCharWeapons[charIndex]
		|| state.oldUnlockedDevilTrigger != sessionData.unlockDevilTrigger
			|| state.oldPlayerDataChars[charIndex] != currentPlayerDataChars[charIndex]
			|| state.oldCharCostumes[charIndex] != currentCharCostumes[charIndex]) {

		state.currentWeapons[charIndex].resize(weaponCount);

		if (isMelee) {
			UpdateMeleeWeaponIDs(actorData, state.currentWeapons, charIndex);
		} else {
			UpdateRangedWeaponIDs(actorData, state.currentWeapons, charIndex);
		}

		state.oldCharWeapons[charIndex] = currentCharWeapons[charIndex];
		state.oldPlayerDataChars[charIndex] = currentPlayerDataChars[charIndex];
		state.oldCharCostumes[charIndex] = currentCharCostumes[charIndex];

		if (pWeaponWheel)
			pWeaponWheel->SetWeapons(state.currentWeapons[charIndex]);
	}

	// Instancing the Wheel class.
	if (!pWeaponWheel) {
		ID3D11DeviceContext* pDeviceContext = nullptr;

		ID3D11Device* pD3D11Device = nullptr;

		pSwapChain->GetDevice(IID_PPV_ARGS(&pD3D11Device));

		pD3D11Device->GetImmediateContext(&pDeviceContext);

		auto initialTheme = WW::WheelThemes::Neutral;
		if (activeCrimsonConfig.WeaponWheel.theme == "Crimson") {
			initialTheme = state.charTheme;
		} else if (activeCrimsonConfig.WeaponWheel.theme == "绯红早期测试版") {
			initialTheme = state.charTheme == WW::WheelThemes::Dante ? WW::WheelThemes::BetaDante : WW::WheelThemes::BetaVergil;
		}

		pWeaponWheel = std::make_unique<WW::WeaponWheel>(pD3D11Device, pDeviceContext, wheelSize.x, wheelSize.y,
			state.currentWeapons[0], initialTheme);

		if (pWeaponWheel) {
			pWeaponWheel->ToggleAlwaysVisible(alwaysShow);
			pWeaponWheel->ToggleAnalogSwitchingUI(false);
		}

		state.oldWeaponIndex = weaponIndex;
	}

	if (!isMelee && characterData.character == CHARACTER::VERGIL) {
		return false;
	}

	if (!pWeaponWheel) {
		return false;
	}

	// Switching Characters
	if (state.oldWeaponIndex != (int)weaponIndex || state.oldCharIndex != (int)charIndex) {// If changed set it
		if (isMelee) {
			UpdateMeleeWeaponIDs(actorData, state.currentWeapons, actorData.newCharacterIndex);
		} else {
			UpdateRangedWeaponIDs(actorData, state.currentWeapons, actorData.newCharacterIndex);
		}

		auto newTheme = WW::WheelThemes::Neutral;
		if (activeCrimsonConfig.WeaponWheel.theme == "Crimson") {
			newTheme = state.charTheme;
		} else if (activeCrimsonConfig.WeaponWheel.theme == "绯红早期测试版") {
			newTheme = state.charTheme == WW::WheelThemes::Dante ? WW::WheelThemes::BetaDante : WW::WheelThemes::BetaVergil;
		}
		pWeaponWheel->SetWheelTheme(newTheme);
		pWeaponWheel->SetWeapons(state.currentWeapons[charIndex]);
		pWeaponWheel->SetActiveSlot((int)weaponIndex);
	}

	// Switching Themes
	if (state.oldTheme != activeCrimsonConfig.WeaponWheel.theme) {
		auto switchTheme = WW::WheelThemes::Neutral;
		if (activeCrimsonConfig.WeaponWheel.theme == "Crimson") {
			switchTheme = state.charTheme;
		} else if (activeCrimsonConfig.WeaponWheel.theme == "绯红早期测试版") {
			switchTheme = state.charTheme == WW::WheelThemes::Dante ? WW::WheelThemes::BetaDante : WW::WheelThemes::BetaVergil;
		}
		pWeaponWheel->SetWheelTheme(switchTheme);
		pWeaponWheel->SetWeapons(state.currentWeapons[charIndex]);
		pWeaponWheel->SetActiveSlot((int)weaponIndex);
	}

	state.oldTheme = activeCrimsonConfig.WeaponWheel.theme;
	state.oldWeaponIndex = (int)weaponIndex;
	state.oldCharIndex = (int)charIndex;
	state.oldUnlockedDevilTrigger = sessionData.unlockDevilTrigger;

	// Draw the wheel
	static bool isOpen = true;

	auto analogSwitchingAllowed = playerIndex == 0 ? activeCrimsonConfig.WeaponWheel.analogSwitching ? true : false : true;

	static auto windowSize = ImVec2(g_renderSize.x, g_renderSize.y);
	auto cornerPos = isMelee ? windowSize - wheelSize : ImVec2(0, windowSize.y - wheelSize.y);
	ImGui::SetNextWindowSize(wheelSize);
	ImGui::SetNextWindowPos(cornerPositioning ? cornerPos : windowPos);

	pWeaponWheel->OnUpdate(deltaTimeGameSpeed);

	bool isSwitchingButtonDown = actorData.buttons[1] & GetBinding(isMelee ? BINDING::CHANGE_DEVIL_ARMS : BINDING::CHANGE_GUN);
	bool isSwitchButtonDown = (actorData.buttons[0] & playerData.switchButton) ||
		(actorData.buttons[2] & playerData.switchButton);

	auto GetDanteDoppelSwitchCondition = [](PlayerActorData& actorData) {
		if (!activeCrimsonGameplay.Gameplay.Dante.doppelgangerSplitSync)
			return false;
		auto& gamepad = GetGamepad(actorData.newGamepad);
		bool styleDown = (gamepad.buttons[0] & GetBinding(BINDING::STYLE_ACTION))
			|| (gamepad.buttons[2] & GetBinding(BINDING::STYLE_ACTION));
		if (!styleDown)
			return false;
		if (actorData.newEntityIndex != ENTITY::MAIN) {
			auto& mainActorData = *reinterpret_cast<PlayerActorData*>(GetNewActorData(actorData.newPlayerIndex, actorData.newCharacterIndex, ENTITY::MAIN).baseAddr);
			//if lockCloneStyle set only allow switch if main actor is currently in doppelganger style
			if (mainActorData == nullptr) return false;
			if (mainActorData.character != CHARACTER::DANTE) return false;
			if ((mainActorData.style == STYLE::DOPPELGANGER && styleDown)) {
				return true;
			}
		}
		if (actorData.newEntityIndex == ENTITY::MAIN) {
			if (actorData.character != CHARACTER::DANTE) return false;
			if ((actorData.style == STYLE::DOPPELGANGER && styleDown)) {
				return true;
			}
		}
		return false;
	};
	bool danteCondition = GetDanteDoppelSwitchCondition(actorData);
	bool suppressWheel = (isSwitchButtonDown || danteCondition) && isSwitchingButtonDown;

	if (suppressWheel) {
		if (pWeaponWheel->GetAnalogSwitchingMode()) {
			pWeaponWheel->ToggleAnalogSwitchingUI(false);
		}
		if (pWeaponWheel->GetAlwaysVisible()) {
			pWeaponWheel->ToggleAlwaysVisible(false);
		}
		isSwitchingButtonDown = false;
	}
	bool leftStick = (characterData.rangedWeaponSwitchStick == LEFT_STICK);
	auto radius = (leftStick) ? gamepad.leftStickRadius : gamepad.rightStickRadius;
	bool stickUsed = radius > RIGHT_STICK_DEADZONE ? true : false;

	if (analogSwitchingAllowed) {
		bool leftStick = (characterData.rangedWeaponSwitchStick == LEFT_STICK);
		auto radius = (leftStick) ? gamepad.leftStickRadius : gamepad.rightStickRadius;
		bool stickUsed = radius > RIGHT_STICK_DEADZONE;

		if (!pWeaponWheel->GetAnalogSwitchingMode()) {
			if (pWeaponWheel->GetSwitchButtonHeldEnough()) {
				pWeaponWheel->ToggleAnalogSwitchingUI(true);
			}

			if ((isSwitchingButtonDown && stickUsed)) {
				pWeaponWheel->ToggleAnalogSwitchingUI(true, true);
			}
		} else {
			if (!isSwitchingButtonDown) {
				pWeaponWheel->ToggleAnalogSwitchingUI(false);
			}
		}
	}
	
	if (pWeaponWheel->GetAlwaysVisible() != alwaysShow) {

		pWeaponWheel->ToggleAlwaysVisible(alwaysShow);
	}

	if (pWeaponWheel->GetAlwaysVisible() != isSwitchingButtonDown) {

		pWeaponWheel->ToggleAlwaysVisible(isSwitchingButtonDown);
	}

	state.startTime = ImGui::GetTime();

	pWeaponWheel->UpdateSwitchButtonHeldEnough(isSwitchingButtonDown, 500);
	pWeaponWheel->OnDraw();

	if (suppressWheel) {
		return false;
	}

	if (activeCrimsonConfig.WeaponWheel.hide && trackHide) {
		return true;
	}

	if (ImGui::Begin(windowName, &isOpen, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground)) {
		ImGui::Image(pWeaponWheel->GetSRV(), wheelSize);

		ImGui::End();
		return true;
	} 
	
	return false;
}

 void DummyWeaponWheelController (IDXGISwapChain* pSwapChain) {
	// We initiate this so that texture lovoid DummyWeaponWheelController(IDXGISwapChain* pSwapChain)ading is done on startup and not when the wheel is opened in-game.
	static WeaponWheelState dummyWWState;

	if (!dummyWeaponWheel) {
		ID3D11DeviceContext* pDeviceContext = nullptr;

		ID3D11Device* pD3D11Device = nullptr;

		pSwapChain->GetDevice(IID_PPV_ARGS(&pD3D11Device));

		pD3D11Device->GetImmediateContext(&pDeviceContext);

		dummyWeaponWheel = std::make_unique<WW::WeaponWheel>(pD3D11Device, pDeviceContext, 100, 100,
			dummyWWState.currentWeapons[0], activeCrimsonConfig.WeaponWheel.theme == "Crimson" ? dummyWWState.charTheme : WW::WheelThemes::Neutral);
	}
}

void WeaponWheels1PController(IDXGISwapChain* pSwapChain) {
	static WeaponWheelState stateMelee;
	static WeaponWheelState stateRanged;
	static bool initialized = false;

	auto pool_1431 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if (!pool_1431 || !pool_1431[3]) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(pool_1431[3]);
	auto playerIndex = actorData.newPlayerIndex;
	auto& playerScreenPosition = crimsonPlayer[playerIndex].playerScreenPosition;
	auto distanceClamped = crimsonPlayer[playerIndex].cameraPlayerDistanceClamped;

	const float deltaTimeMs = g_deltaTime * 1000.0f;

	auto inMultiplayer = activeCrimsonConfig.WeaponWheel.force1PMultiplayerPosScale || activeConfig.Actor.playerCount > 1;
	auto& scale = activeCrimsonConfig.WeaponWheel.scale;
	ImVec2 normalPos = ImVec2(g_renderSize.x - g_renderSize.y * 0.45f, g_renderSize.y - g_renderSize.y * 0.45f);
	ImVec2 normalSize = initialized ? scale == HUDELEMENTSCALESTATE::BIG ? ImVec2(g_renderSize.y * 0.45f, g_renderSize.y * 0.45f) :
		ImVec2(g_renderSize.y * 0.35f, g_renderSize.y * 0.35f) :
		ImVec2(g_renderSize.y * 0.45f, g_renderSize.y * 0.45f);

	ImVec2 multiplayerPosMelee = ImVec2(g_renderSize.x * 0.4853f, g_renderSize.y * 0.0f);
	ImVec2 multiplayerPosRanged = ImVec2(g_renderSize.x * 0.4253f, g_renderSize.y * 0.0f);
	ImVec2 multiplayerSize = initialized ? ImVec2(g_renderSize.y * 0.15f, g_renderSize.y * 0.15f) : normalSize;
	// We need to initialize with the bigger size first to avoid the wheel from displaying on a lower quality if the user changes the setting at runtime. -- Berth

	// Adjusts size dynamically based on the distance between Camera and Player

	bool alwaysShow = (activeCrimsonConfig.WeaponWheel.alwaysShow == HUDELEMENTSHOWSTATE::ONLY_IN_MP
		&& activeConfig.Actor.playerCount > 1) || activeCrimsonConfig.WeaponWheel.alwaysShow == HUDELEMENTSHOWSTATE::ALWAYS ? true : false;

	if (WeaponWheelController(actorData, pSwapChain, meleeWeaponWheel[0], "MeleeWheel1P",
		!inMultiplayer,
		inMultiplayer ? multiplayerPosMelee : normalPos, inMultiplayer ? multiplayerSize : normalSize,
		alwaysShow, true, true, stateMelee, deltaTimeMs, deltaTimeMs)) {

		initialized = true;
	}

	WeaponWheelController(actorData, pSwapChain, rangedWeaponWheel[0], "RangedWheel1P",
		!inMultiplayer,
		inMultiplayer ? multiplayerPosRanged : normalPos, inMultiplayer ? multiplayerSize : normalSize,
		alwaysShow, true, false, stateRanged, deltaTimeMs, deltaTimeMs);
}

void WeaponWheelsMultiplayerController(IDXGISwapChain* pSwapChain) {
	static WeaponWheelState stateMelee[PLAYER_COUNT];
	static bool initializedMelee[PLAYER_COUNT] = { false };

	static WeaponWheelState stateRanged[PLAYER_COUNT];
	static bool initializedRanged[PLAYER_COUNT] = { false };

	float deltaTime = ImGui::GetIO().DeltaTime;
	const float deltaTimeMs = g_deltaTime * 1000.0f;

	if (!activeCrimsonConfig.MultiplayerBars2D.show) return;

	for (uint8 playerIndex = 1; playerIndex < activeConfig.Actor.playerCount; ++playerIndex) {
		auto& playerData = GetPlayerData(playerIndex);
		auto& characterData = GetCharacterData(playerIndex, playerData.characterIndex, ENTITY::MAIN);
		auto& newActorData = GetNewActorData(playerIndex, playerData.characterIndex, ENTITY::MAIN);

		if (!newActorData.baseAddr) {
			return;
		}
		auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);

		auto& playerScreenPosition = crimsonPlayer[playerIndex].playerScreenPosition;
		auto distanceClamped = crimsonPlayer[playerIndex].cameraPlayerDistanceClamped;
		auto activeGameSpeed = GetCurrentGameSpeed();

		ImVec2 normalSize = ImVec2(g_renderSize.y * 0.45f, g_renderSize.y * 0.45f);

		ImVec2 baseMultiplayerSize = ImVec2(g_renderSize.y * 0.15f, g_renderSize.y * 0.15f);
		ImVec2 multiplayerSize = initializedMelee[playerIndex] ? baseMultiplayerSize : normalSize;

		auto& meleeWheel = meleeWeaponWheel[playerIndex];
		auto& rangedWheel = rangedWeaponWheel[playerIndex];
		const float baseSpacing = 0.37f;
		const float barSpacing = g_renderSize.x * baseSpacing;

		const float baseXMelee = g_renderSize.x * (0.1722f - baseSpacing);
		const float baseXRanged = g_renderSize.x * (0.1122f - baseSpacing);

		
		const float baseY = g_renderSize.y * 0.8462f; // Bottom of the screen


		ImVec2 multiplayerPosMelee = ImVec2(baseXMelee + barSpacing * (playerIndex), baseY);
		ImVec2 multiplayerPosRanged = ImVec2(baseXRanged + barSpacing * (playerIndex), baseY);

		std::string meleeWheelName = "MeleeWheel " + std::to_string(playerIndex + 1);
		std::string rangedWheelName = "RangedWheel " + std::to_string(playerIndex + 1);

		bool alwaysShow = (activeCrimsonConfig.WeaponWheel.alwaysShow == HUDELEMENTSHOWSTATE::ONLY_IN_MP
			&& activeConfig.Actor.playerCount > 1) || activeCrimsonConfig.WeaponWheel.alwaysShow == HUDELEMENTSHOWSTATE::ALWAYS ? true : false;

		if (WeaponWheelController(actorData, pSwapChain, meleeWheel, meleeWheelName.c_str(),
			false,
			multiplayerPosMelee, multiplayerSize,
			alwaysShow, true, true, stateMelee[playerIndex], deltaTimeMs, deltaTimeMs)) {

			initializedMelee[playerIndex] = true;
		}

		WeaponWheelController(actorData, pSwapChain, rangedWheel, rangedWheelName.c_str(),
			false,
			multiplayerPosRanged, multiplayerSize,
			alwaysShow, true, false, stateRanged[playerIndex], deltaTimeMs, deltaTimeMs);
	}

	multiplayerWheelsLoaded = true; 
}

void WorldSpaceWeaponWheels1PController(IDXGISwapChain* pSwapChain) {
	static WeaponWheelState stateMelee;
	static bool initialized = false;

	static WeaponWheelState stateRanged;

	if (!multiplayerWheelsLoaded) return;
	if (activeCrimsonConfig.WeaponWheel.worldSpaceWheels == HUDELEMENTSHOWSTATE::OFF ||
		(activeCrimsonConfig.WeaponWheel.worldSpaceWheels == HUDELEMENTSHOWSTATE::ONLY_IN_MP
			&& activeConfig.Actor.playerCount <= 1)) return;

	const float deltaTimeMs = g_deltaTime * 1000.0f;

	auto pool_1431 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if (!pool_1431 || !pool_1431[3]) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(pool_1431[3]);

	auto& playerScreenPosition = crimsonPlayer[0].playerScreenPosition;
	auto distanceClamped = crimsonPlayer[0].cameraPlayerDistanceClamped;

	ImVec2 normalSize = ImVec2(g_renderSize.y * 0.45f, g_renderSize.y * 0.45f);
	ImVec2 baseMultiplayerSize = ImVec2(g_renderSize.y * 0.17f, g_renderSize.y * 0.17f);
	ImVec2 sizeDistance = { (baseMultiplayerSize.x * (1.0f / ((float)distanceClamped / 20))),
							(baseMultiplayerSize.y * (1.0f / ((float)distanceClamped / 20))) };
	ImVec2 multiplayerSize = initialized ? sizeDistance : normalSize;
	ImVec2 worldSpaceMultiplayerPos = ImVec2(playerScreenPosition.x - (sizeDistance.x / 2.0f), playerScreenPosition.y - sizeDistance.y);

	auto& meleeWheel = meleeWorldSpaceWeaponWheel[0];
	auto& rangedWheel = rangedWorldSpaceWeaponWheel[0];

	//const float baseSpacing = 170.0f;
	const float baseSpacing = 400.0f;
	float scaleFactor = multiplayerSize.x / normalSize.x;
	float scaleFactorY = ImGui::GetIO().DisplaySize.y / 1080;
	float adjustedSpacing = baseSpacing * scaleFactorY * scaleFactor;

	ImVec2 meleeWheelPos = ImVec2(worldSpaceMultiplayerPos.x + adjustedSpacing, worldSpaceMultiplayerPos.y);
	ImVec2 rangedWheelPos = ImVec2(worldSpaceMultiplayerPos.x - adjustedSpacing, worldSpaceMultiplayerPos.y);

	std::string meleeWheelName = "MeleeWheelWorldSpace " + std::to_string(0);
	std::string rangedWheelName = "RangedWheelWorldSpace " + std::to_string(0);

	bool alwaysShow = (activeCrimsonConfig.WeaponWheel.worldSpaceAlwaysShow == HUDELEMENTSHOWSTATE::ONLY_IN_MP 
		&& activeConfig.Actor.playerCount > 1) || activeCrimsonConfig.WeaponWheel.worldSpaceAlwaysShow == HUDELEMENTSHOWSTATE::ALWAYS ? true : false;

	if (WeaponWheelController(actorData, pSwapChain, meleeWheel, meleeWheelName.c_str(),
		false,
		meleeWheelPos, multiplayerSize,
		alwaysShow, false, true,
		stateMelee, deltaTimeMs, deltaTimeMs)) {
		initialized = true;
	}

	WeaponWheelController(actorData, pSwapChain, rangedWheel, rangedWheelName.c_str(),
		false,
		rangedWheelPos, multiplayerSize,
		alwaysShow, false, false,
		stateRanged, deltaTimeMs, deltaTimeMs);

}

void WorldSpaceWeaponWheelsController(IDXGISwapChain* pSwapChain) {
	static WeaponWheelState stateMelee[PLAYER_COUNT];
	static bool initializedMelee[PLAYER_COUNT] = { false };

	static WeaponWheelState stateRanged[PLAYER_COUNT];
	static bool initializedRanged[PLAYER_COUNT] = { false };

	if (!multiplayerWheelsLoaded) return;
	if (activeCrimsonConfig.WeaponWheel.worldSpaceWheels == HUDELEMENTSHOWSTATE::OFF ||
		(activeCrimsonConfig.WeaponWheel.worldSpaceWheels == HUDELEMENTSHOWSTATE::ONLY_IN_MP
			&& activeConfig.Actor.playerCount <= 1)) return;

	const float deltaTimeMs = g_deltaTime * 1000.0f;

	for (int playerIndex = 1; playerIndex < activeConfig.Actor.playerCount; playerIndex++) {
		auto& playerData = GetPlayerData((uint8)playerIndex);
		auto& characterData = GetCharacterData(playerIndex, playerData.characterIndex, ENTITY::MAIN);
		auto& newActorData = GetNewActorData(playerIndex, playerData.characterIndex, ENTITY::MAIN);

		if (!newActorData.baseAddr) {
			return;
		}
		auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);

		auto& playerScreenPosition = crimsonPlayer[playerIndex].playerScreenPosition;
		auto distanceClamped = crimsonPlayer[playerIndex].cameraPlayerDistanceClamped;

		ImVec2 normalSize = ImVec2(g_renderSize.y * 0.45f, g_renderSize.y * 0.45f);
		ImVec2 baseMultiplayerSize = ImVec2(g_renderSize.y * 0.17f, g_renderSize.y * 0.17f);
		ImVec2 sizeDistance = { (baseMultiplayerSize.x * (1.0f / ((float)distanceClamped / 20))),
								(baseMultiplayerSize.y * (1.0f / ((float)distanceClamped / 20))) };
		ImVec2 multiplayerSize = initializedMelee[playerIndex] ? sizeDistance : normalSize;
		ImVec2 worldSpaceMultiplayerPos = ImVec2(playerScreenPosition.x - (sizeDistance.x / 2.0f), playerScreenPosition.y - sizeDistance.y);

		auto& meleeWheel = meleeWorldSpaceWeaponWheel[playerIndex];
		auto& rangedWheel = rangedWorldSpaceWeaponWheel[playerIndex];

		//const float baseSpacing = 170.0f;
		const float baseSpacing = 400.0f;
		float scaleFactor = multiplayerSize.x / normalSize.x;
		float scaleFactorY = ImGui::GetIO().DisplaySize.y / 1080;
		float adjustedSpacing = baseSpacing * scaleFactorY * scaleFactor;

		ImVec2 meleeWheelPos = ImVec2(worldSpaceMultiplayerPos.x + adjustedSpacing, worldSpaceMultiplayerPos.y);
		ImVec2 rangedWheelPos = ImVec2(worldSpaceMultiplayerPos.x - adjustedSpacing, worldSpaceMultiplayerPos.y);

		std::string meleeWheelName = "MeleeWheelWorldSpace " + std::to_string(playerIndex + 1);
		std::string rangedWheelName = "RangedWheelWorldSpace " + std::to_string(playerIndex + 1);

		bool alwaysShow = (activeCrimsonConfig.WeaponWheel.worldSpaceAlwaysShow == HUDELEMENTSHOWSTATE::ONLY_IN_MP
			&& activeConfig.Actor.playerCount > 1) || activeCrimsonConfig.WeaponWheel.worldSpaceAlwaysShow == HUDELEMENTSHOWSTATE::ALWAYS ? true : false;

		if (WeaponWheelController(actorData, pSwapChain, meleeWheel, meleeWheelName.c_str(),
			false,
			meleeWheelPos, multiplayerSize,
			alwaysShow, false, true,
			stateMelee[playerIndex], deltaTimeMs, deltaTimeMs)) {
			initializedMelee[playerIndex] = true;
		}
	
		WeaponWheelController(actorData, pSwapChain, rangedWheel, rangedWheelName.c_str(),
			false,
			rangedWheelPos, multiplayerSize,
			alwaysShow, false, false,
			stateRanged[playerIndex], deltaTimeMs, deltaTimeMs);
	}
}


#pragma endregion

#pragma region Actor

uint8 Actor_buttonIndices[PLAYER_COUNT] = {};

uint8 Actor_collisionGroupIndices[PLAYER_COUNT] = {};

uint8 Actor_newCharacterIndices[PLAYER_COUNT][CHARACTER_COUNT][ENTITY_COUNT] = {};

uint8 Actor_styleButtonIndices[PLAYER_COUNT][CHARACTER_COUNT][ENTITY_COUNT][STYLE_COUNT] = {};
uint8 Actor_styleIndices[PLAYER_COUNT][CHARACTER_COUNT][ENTITY_COUNT][STYLE_COUNT][2] = {};

uint8 Actor_meleeWeaponIndices[PLAYER_COUNT][CHARACTER_COUNT][ENTITY_COUNT][MELEE_WEAPON_COUNT] = {};
uint8 Actor_rangedWeaponIndices[PLAYER_COUNT][CHARACTER_COUNT][ENTITY_COUNT][RANGED_WEAPON_COUNT] = {};

uint8 Actor_removeBusyFlagButtonIndices[PLAYER_COUNT][4] = {};


void Actor_UpdateIndices() {
	old_for_all(uint8, playerIndex, PLAYER_COUNT) {
		auto& activePlayerData = GetActivePlayerData(playerIndex);
		auto& queuedPlayerData = GetQueuedPlayerData(playerIndex);


		UpdateMapIndex(collisionGroups, Actor_collisionGroupIndices[playerIndex], queuedPlayerData.collisionGroup);

		UpdateMapIndex(buttons, Actor_buttonIndices[playerIndex], activePlayerData.switchButton);
 
		old_for_all(uint8, characterIndex, CHARACTER_COUNT) {
			old_for_all(uint8, entityIndex, ENTITY_COUNT) {
				auto& activeCharacterData = GetActiveCharacterData(playerIndex, characterIndex, entityIndex);
				auto& queuedCharacterData = GetQueuedCharacterData(playerIndex, characterIndex, entityIndex);


				UpdateMapIndex(
					ddmkCharactersMap, Actor_newCharacterIndices[playerIndex][characterIndex][entityIndex], queuedCharacterData.character);


				old_for_all(uint8, styleIndex, STYLE_COUNT) {
					UpdateMapIndex(buttons, Actor_styleButtonIndices[playerIndex][characterIndex][entityIndex][styleIndex],
						queuedCharacterData.styleButtons[styleIndex]);

					switch (queuedCharacterData.character) {
					case CHARACTER::DANTE:
					{
						UpdateMapIndex(stylesDante, Actor_styleIndices[playerIndex][characterIndex][entityIndex][styleIndex][0],
							queuedCharacterData.styles[styleIndex][0]);
						UpdateMapIndex(stylesDante, Actor_styleIndices[playerIndex][characterIndex][entityIndex][styleIndex][1],
							queuedCharacterData.styles[styleIndex][1]);

						break;
					}
					case CHARACTER::VERGIL:
					{
						UpdateMapIndex(stylesVergil, Actor_styleIndices[playerIndex][characterIndex][entityIndex][styleIndex][0],
							queuedCharacterData.styles[styleIndex][0]);
						UpdateMapIndex(stylesVergil, Actor_styleIndices[playerIndex][characterIndex][entityIndex][styleIndex][1],
							queuedCharacterData.styles[styleIndex][1]);

						break;
					}
					}
				}

				if (queuedCharacterData.character != CHARACTER::DANTE) {
					continue;
				}

				old_for_all(uint8, meleeWeaponIndex, MELEE_WEAPON_COUNT_DANTE) {
					UpdateMapIndex(meleeWeaponsDante, Actor_meleeWeaponIndices[playerIndex][characterIndex][entityIndex][meleeWeaponIndex],
						queuedCharacterData.meleeWeapons[meleeWeaponIndex]);
				}

				old_for_all(uint8, rangedWeaponIndex, RANGED_WEAPON_COUNT_DANTE) {
					UpdateMapIndex(rangedWeaponsDante,
						Actor_rangedWeaponIndices[playerIndex][characterIndex][entityIndex][rangedWeaponIndex],
						queuedCharacterData.rangedWeapons[rangedWeaponIndex]);
				}
			}
		}


		old_for_all(uint8, buttonIndex, 4) {
			UpdateMapIndex(
				buttons, Actor_removeBusyFlagButtonIndices[playerIndex][buttonIndex], activePlayerData.removeBusyFlagButtons[buttonIndex]);
		}
	}
}

void BackgroundPlayerText(uint8 playerIndex) {
	const char* playerIndexNames[PLAYER_COUNT] = { "1P", "2P", "3P", "4P" };
	std::string playerBackgroundText = std::string(playerIndexNames[playerIndex]) + " - " + activeCrimsonConfig.PlayerProperties.playerName[playerIndex];
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	ImRect wndRect = window->Rect();
	const ImVec2 tabBtnSize = { scaledFontSize * 9.38f, scaledFontSize * 2.4f };

	ImVec2 pos{ wndRect.Min.x + scaledFontSize * 0.1f,
							wndRect.Min.y + (2.8f * tabBtnSize.y)/*Header Space*/ + scaledFontSize * 1.3f };


	window->DrawList->AddText(UI::g_ImGuiFont_RussoOne256, scaledFontSize * 4.6f, pos,
		UI::SwapColorEndianness(0xFFFFFF10), playerBackgroundText.c_str());
}

void BackgroundSPMPText(const char* SPMPText) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	ImRect wndRect = window->Rect();
	const ImVec2 tabBtnSize = { scaledFontSize * 9.38f, scaledFontSize * 2.4f };

	ImVec2 pos{ wndRect.Min.x + scaledFontSize * 0.1f,
							wndRect.Min.y + (6.9f * tabBtnSize.y)/*Header Space*/ + scaledFontSize * 1.3f };


	window->DrawList->AddText(UI::g_ImGuiFont_RussoOne256, scaledFontSize * 4.6f, pos,
		UI::SwapColorEndianness(0xFFFFFF10), SPMPText);

}

void CheckDanteUnlockedWeapons(CharacterData& queuedCharacterData, CharacterData& activeCharacterData, uint8 playerIndex, uint8 characterIndex) {
	auto& lastMaxMeleeWeaponCount = queuedCrimsonConfig.CachedSettings.lastMaxMeleeWeaponCount[playerIndex][characterIndex];
	auto& lastMaxRangedWeaponCount = queuedCrimsonConfig.CachedSettings.lastMaxRangedWeaponCount[playerIndex][characterIndex];

	if (queuedCharacterData.character == CHARACTER::DANTE) {
		if (lastMaxMeleeWeaponCount > weaponProgression.rangedWeaponIds.size()) {
			lastMaxMeleeWeaponCount = weaponProgression.rangedWeaponIds.size();
			queuedCharacterData.meleeWeaponCount = lastMaxMeleeWeaponCount;
			activeCharacterData.meleeWeaponCount = lastMaxMeleeWeaponCount;
		}

		if (lastMaxRangedWeaponCount > weaponProgression.rangedWeaponIds.size()) {
			lastMaxRangedWeaponCount = weaponProgression.rangedWeaponIds.size();
			queuedCharacterData.rangedWeaponCount = lastMaxRangedWeaponCount;
			activeCharacterData.rangedWeaponCount = lastMaxRangedWeaponCount;
		}

		for (uint8 slotEquipped = 0; slotEquipped < 5; slotEquipped++) {
			auto& weaponId = queuedCharacterData.meleeWeapons[slotEquipped];

			// Check if weaponId exists in weaponProgression.meleeWeaponIds
			bool found = false;
			for (const auto& validId : weaponProgression.meleeWeaponIds) {
				if (weaponId == validId) {
					found = true;
					break;
				}
			}

			// If not found, replace with the natural ID for this slot
			if (!found) {
				weaponId = weaponProgression.meleeWeaponNaturalIds[slotEquipped];
			}
		}


		for (uint8 slotEquipped = 0; slotEquipped < 5; slotEquipped++) {
			auto& weaponId = queuedCharacterData.rangedWeapons[slotEquipped];
			// Check if weaponId exists in weaponProgression.rangedWeaponIds
			bool found = false;
			for (const auto& validId : weaponProgression.rangedWeaponIds) {
				if (weaponId == validId) {
					found = true;
					break;
				}
			}
			// If not found, replace with the natural ID for this slot
			if (!found) {
				weaponId = weaponProgression.rangedWeaponNaturalIds[slotEquipped];
			}
		}
	}
	else if (queuedCharacterData.character == CHARACTER::VERGIL) {
		for (uint8 slotEquipped = 0; slotEquipped < 5; slotEquipped++) {
			auto& weaponId = queuedCharacterData.meleeWeapons[slotEquipped];

			// Check if weaponId exists in weaponProgression.meleeWeaponNaturalIdsVergil
			// since all Vergil's melee weapons are natural IDs aka unlocked already
			bool found = false;
			for (const auto& validId : weaponProgression.meleeWeaponNaturalIdsVergil) {
				if (weaponId == validId) {
					found = true;
					break;
				}
			}

			// If not found, replace with the natural ID for this slot
			if (!found) {
				weaponId = weaponProgression.meleeWeaponNaturalIds[slotEquipped];
			}
		}
	}
}

void Actor_CharacterTab(uint8 playerIndex, uint8 characterIndex, uint8 entityIndex, size_t defaultFontSize) {
	auto& activeCharacterData = GetActiveCharacterData(playerIndex, characterIndex, entityIndex);
	auto& queuedCharacterData = GetQueuedCharacterData(playerIndex, characterIndex, entityIndex);

	auto& activeCharacterDataClone = GetActiveCharacterData(playerIndex, characterIndex, 1);
	auto& queuedCharacterDataClone = GetQueuedCharacterData(playerIndex, characterIndex, 1);

	auto& mainActiveCharacterData = GetActiveCharacterData(playerIndex, characterIndex, ENTITY::MAIN);
	auto& mainQueuedCharacterData = GetQueuedCharacterData(playerIndex, characterIndex, ENTITY::MAIN);
	
	auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

	auto& playerData = GetPlayerData(playerIndex);
	auto& newActorData = GetNewActorData(playerIndex, playerData.characterIndex, ENTITY::MAIN);

	ImU32 checkmarkColor = UI::SwapColorEndianness(0xFFFFFFFF);

	if ((entityIndex == ENTITY::CLONE) && (mainQueuedCharacterData.character >= CHARACTER::MAX)) {
		return;
	}

	ImGui::Text("");
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColor);

	{
		const float columnWidth = 0.8f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;

		if (ImGui::BeginTable("CharacterSelection", 1)) {

			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);
			ImGui::TableSetupColumn("c1", 0, columnWidth);
			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth);

			if (!activeCrimsonGameplay.Cheats.General.legacyDDMKCharacters) {
				if (UI::ComboMapValue("角色", crimsonCharacterNames, crimsonCharacterMap,
					queuedCharacterData.character)) {
					ApplyDefaultCharacterData(queuedCharacterData, queuedCharacterData.character, playerIndex, characterIndex);
					
					if (queuedCharacterData.character == CHARACTER::DANTE || queuedCharacterData.character == CHARACTER::VERGIL) {
						queuedCharacterDataClone.character = queuedCharacterData.character;
						ApplyDefaultCharacterData(queuedCharacterDataClone, queuedCharacterDataClone.character, playerIndex, characterIndex);
					}

					CheckDanteUnlockedWeapons(queuedCharacterData, activeCharacterData, playerIndex, characterIndex);

					Actor_UpdateIndices();
				}
				
			} else {
				if ((playerIndex == 0) && (characterIndex > 0)) {
					if (UI::ComboMap("角色", ddmkCharacterNames, ddmkCharactersMap, Actor_newCharacterIndices[playerIndex][characterIndex][entityIndex],
						queuedCharacterData.character)) {
						ApplyDefaultCharacterData(queuedCharacterData, queuedCharacterData.character, playerIndex, characterIndex);

						if (queuedCharacterData.character == CHARACTER::DANTE || queuedCharacterData.character == CHARACTER::VERGIL) {
							queuedCharacterDataClone.character = queuedCharacterData.character;
							ApplyDefaultCharacterData(queuedCharacterDataClone, queuedCharacterDataClone.character, playerIndex, characterIndex);
						}

						CheckDanteUnlockedWeapons(queuedCharacterData, activeCharacterData, playerIndex, characterIndex);

						Actor_UpdateIndices();
					}
				} else {
					if (UI::Combo("角色", ddmkCharacter2PNames, queuedCharacterData.character)) {
						ApplyDefaultCharacterData(queuedCharacterData, queuedCharacterData.character, playerIndex, characterIndex);

						if (queuedCharacterData.character == CHARACTER::DANTE || queuedCharacterData.character == CHARACTER::VERGIL) {
							queuedCharacterDataClone.character = queuedCharacterData.character;
							ApplyDefaultCharacterData(queuedCharacterDataClone, queuedCharacterDataClone.character, playerIndex, characterIndex);
						}

						CheckDanteUnlockedWeapons(queuedCharacterData, activeCharacterData, playerIndex, characterIndex);

						Actor_UpdateIndices();
					}
				}
			}

			ImGui::PopItemWidth();
			ImGui::PopFont();

			BackgroundPlayerText(playerIndex);


			if ((queuedCharacterData.character == CHARACTER::DANTE) || (queuedCharacterData.character == CHARACTER::VERGIL)) {
				ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);

				ImGui::TableNextRow(0, rowWidth);
				ImGui::TableNextColumn();
				ImGui::PushItemWidth(itemWidth);

				// Costume
				{
					bool condition = queuedCharacterData.ignoreCostume || queuedCharacterData.forceFiles;

					GUI_PushDisable(condition);

					if (queuedCharacterData.character == CHARACTER::DANTE) {
						UI::Combo2("服装", costumeNamesDante, activeCharacterData.costume, queuedCharacterData.costume);
					} else if (queuedCharacterData.character == CHARACTER::VERGIL || queuedCharacterData.character == CHARACTER::BOB || 
						queuedCharacterData.character == CHARACTER::BOSS_VERGIL) {
						UI::Combo2("服装", costumeNamesVergil, activeCharacterData.costume, queuedCharacterData.costume);
					} else if (queuedCharacterData.character == CHARACTER::LADY || queuedCharacterData.character == CHARACTER::BOSS_LADY) {
						UI::Combo2("服装", costumeNamesLady, activeCharacterData.costume, queuedCharacterData.costume);
					}
					
					GUI_PopDisable(condition);

					if (GUI_Checkbox("使用游戏内设置", queuedCharacterData.ignoreCostume)) {
						queuedCharacterDataClone.ignoreCostume = queuedCharacterData.ignoreCostume;
					}
					ImGui::SameLine();
				}

				ImGui::SameLine();


				if (GUI_Checkbox("强制指定模型", queuedCharacterData.forceFiles)) {
					queuedCharacterDataClone.forceFiles = queuedCharacterData.forceFiles;
				}


				if (queuedCharacterData.forceFiles) {

					ImGui::TableNextRow(0, rowWidth);
					ImGui::TableNextColumn();

					if (UI::Combo("强制模型角色", ddmkCharacter2PNames, queuedCharacterData.forceFilesCharacter)) {
						queuedCharacterDataClone.forceFilesCharacter = queuedCharacterData.forceFilesCharacter;
					}

					if (queuedCharacterData.forceFilesCharacter == CHARACTER::DANTE) {
						UI::Combo2("强制文件服装", costumeNamesDante, activeCharacterData.forceFilesCostume, queuedCharacterData.forceFilesCostume);
					} else if (queuedCharacterData.forceFilesCharacter == CHARACTER::VERGIL)  {
						UI::Combo2("强制文件服装", costumeNamesVergil, activeCharacterData.forceFilesCostume, queuedCharacterData.forceFilesCostume);
					} else if (queuedCharacterData.forceFilesCharacter == CHARACTER::LADY || queuedCharacterData.forceFilesCharacter == CHARACTER::BOSS_LADY) {
						UI::Combo2("强制文件服装", costumeNamesLady, activeCharacterData.forceFilesCostume, queuedCharacterData.forceFilesCostume);
					}
				}
				//if we are dante and haven't unlocked doppelganger, we shouldn't show this menu option.
				if (!queuedCharacterData.character == CHARACTER::DANTE || sessionData.weaponAndStyleUnlocks[WEAPONANDSTYLEUNLOCKS::DOPPELGANGER]) {
					if (UI::Combo("分身", ddmkCharacter2PNames, queuedCharacterDataClone.character)) {
						ApplyDefaultCharacterData(queuedCharacterDataClone, queuedCharacterDataClone.character, playerIndex, characterIndex);

						Actor_UpdateIndices();
					}
				}

				ImGui::PopItemWidth();
				ImGui::PopFont();
			}


			ImGui::EndTable();
		}
	}

	ImGui::Text("");

	/*ImGui::Text("风格");

	old_for_all(uint8, styleIndex, STYLE_COUNT)
	{
			if constexpr (debug)
			{
					ImGui::Text("");
					ImGui::Text("%u", styleIndex);
			}

			UI::ComboMap
			(
					"",
					buttonNames,
					buttons,
					Actor_styleButtonIndices[playerIndex][characterIndex][entityIndex][styleIndex],
					queuedCharacterData.styleButtons[styleIndex],
					ImGuiComboFlags_HeightLargest
			);

			if constexpr (!debug)
			{
					ImGui::SameLine();
			}

			switch (queuedCharacterData.character)
			{
					case CHARACTER::DANTE:
					{
							UI::ComboMap
							(
									"",
									styleNamesDante,
									stylesDante,
									Actor_styleIndices[playerIndex][characterIndex][entityIndex][styleIndex][0],
									queuedCharacterData.styles[styleIndex][0]
							);

							if constexpr (!debug)
							{
									ImGui::SameLine();
							}

							UI::ComboMap
							(
									"",
									styleNamesDante,
									stylesDante,
									Actor_styleIndices[playerIndex][characterIndex][entityIndex][styleIndex][1],
									queuedCharacterData.styles[styleIndex][1]
							);

							break;
					}
					case CHARACTER::VERGIL:
					{
							UI::ComboMap
							(
									"",
									styleNamesVergil,
									stylesVergil,
									Actor_styleIndices[playerIndex][characterIndex][entityIndex][styleIndex][0],
									queuedCharacterData.styles[styleIndex][0]
							);

							if constexpr (debug)
							{
									ImGui::SameLine();
							}

							UI::ComboMap
							(
									"",
									styleNamesVergil,
									stylesVergil,
									Actor_styleIndices[playerIndex][characterIndex][entityIndex][styleIndex][1],
									queuedCharacterData.styles[styleIndex][1]
							);

							break;
					}
			}
	}

	ImGui::PopItemWidth();*/

	ImGui::PopStyleColor();

	if (queuedCharacterData.character != CHARACTER::DANTE && queuedCharacterData.character != CHARACTER::VERGIL) {
		return;
	}

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);
	ImGui::Text("武器配置");
	//GUI_SectionEnd(SectionFlags_NoNewLine);
	ImGui::PopFont();

	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;

		if (ImGui::BeginTable("WeaponLoadout", 2)) {

			ImGui::TableNextRow(0, rowWidth);

			if (queuedCharacterData.character == CHARACTER::DANTE) {

				ImGui::TableNextColumn();

				ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.0f]);
				ImGui::Text("远程武器");
				ImGui::PopFont();


				ImGui::PushItemWidth(itemWidth);

				auto rangedSlider = [&]() {
					if (queuedCharacterData.character == activeCharacterData.character) {
						if (GUI_Slider2<uint8>("", queuedCharacterData.rangedWeaponCount,
							activeCharacterData.rangedWeaponCount, 1, weaponProgression.gunsUnlockedQtt + 1)) {
							if (!newActorData.baseAddr) return;
							auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);
							auto& characterData = GetCharacterData(actorData);
						}
					} else {
						GUI_Slider<uint8>("", queuedCharacterData.rangedWeaponCount, 1, weaponProgression.gunsUnlockedQtt + 1);
					}
					};

				rangedSlider();

				old_for_all(uint8, rangedWeaponIndex, weaponProgression.gunsUnlockedQtt + 1) {
					bool condition = (rangedWeaponIndex >= queuedCharacterData.rangedWeaponCount);

					GUI_PushDisable(condition);

					// Check if the queuedCharacter matches the activeCharacter for realTime WeaponSwitching
					if (queuedCharacterData.character == activeCharacterData.character) {
						if (UI::ComboMapVector2("", weaponProgression.rangedWeaponNames, weaponProgression.rangedWeaponIds,
							queuedCharacterData.rangedWeapons[rangedWeaponIndex], activeCharacterData.rangedWeapons[rangedWeaponIndex])) {

							if (!newActorData.baseAddr) break;
							auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);
							actorData.rangedWeaponIndex = queuedCharacterData.rangedWeapons[rangedWeaponIndex];
						}
					} else {
						UI::ComboMapVector("", weaponProgression.rangedWeaponNames, weaponProgression.rangedWeaponIds,
							queuedCharacterData.rangedWeapons[rangedWeaponIndex]);
					}

					// Doppelganger will now have same weapons equipped as Dante - Mia.
					queuedCharacterDataClone.rangedWeapons[rangedWeaponIndex] = queuedCharacterData.rangedWeapons[rangedWeaponIndex];

					GUI_PopDisable(condition);
				}
			}

			ImGui::TableNextColumn();

			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.0f]);
			ImGui::Text("近战武器");
			ImGui::PopFont();

			ImGui::PushItemWidth(itemWidth);


			if (queuedCharacterData.character == CHARACTER::DANTE) {
				auto meleeSlider = [&]() {
					if (queuedCharacterData.character == activeCharacterData.character) {
						if (GUI_Slider2<uint8>("", queuedCharacterData.meleeWeaponCount,
							activeCharacterData.meleeWeaponCount, 1, weaponProgression.devilArmsUnlockedQtt + 1)) {
							if (!newActorData.baseAddr) return;
							auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);
							auto& characterData = GetCharacterData(actorData);
						}
					} else {
						GUI_Slider<uint8>("", queuedCharacterData.meleeWeaponCount, 1, weaponProgression.devilArmsUnlockedQtt + 1);
					}
					};

				meleeSlider();

				queuedCharacterDataClone.meleeWeaponCount = queuedCharacterData.meleeWeaponCount;


				old_for_all(uint8, meleeWeaponIndex, weaponProgression.devilArmsUnlockedQtt + 1) {
					bool condition = (meleeWeaponIndex >= queuedCharacterData.meleeWeaponCount);

					GUI_PushDisable(condition);

					// Check if the queuedCharacter matches the activeCharacter for realTime WeaponSwitching
					if (queuedCharacterData.character == activeCharacterData.character) {
						if (UI::ComboMapVector2("", weaponProgression.meleeWeaponNames, weaponProgression.meleeWeaponIds,
							queuedCharacterData.meleeWeapons[meleeWeaponIndex], activeCharacterData.meleeWeapons[meleeWeaponIndex])) {

							if (!newActorData.baseAddr) break;
							auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);
							actorData.meleeWeaponIndex = queuedCharacterData.meleeWeapons[meleeWeaponIndex];
						}
					} else {
						UI::ComboMapVector("", weaponProgression.meleeWeaponNames, weaponProgression.meleeWeaponIds,
							queuedCharacterData.meleeWeapons[meleeWeaponIndex]);
					}

					// Doppelganger will now have same weapons equipped as Dante - Mia.
					queuedCharacterDataClone.meleeWeapons[meleeWeaponIndex] = queuedCharacterData.meleeWeapons[meleeWeaponIndex];

					GUI_PopDisable(condition);
				}
			} else if (queuedCharacterData.character == CHARACTER::VERGIL) {
				auto meleeSlider = [&]() {
					if (queuedCharacterData.character == activeCharacterData.character) {
						if (GUI_Slider2<uint8>("", queuedCharacterData.meleeWeaponCount,
							activeCharacterData.meleeWeaponCount, 1, WEAPON_COUNT_VERGIL)) {
							if (!newActorData.baseAddr) return;
							auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);
							auto& characterData = GetCharacterData(actorData);
						}
					} else {
						GUI_Slider<uint8>("", queuedCharacterData.meleeWeaponCount, 1, WEAPON_COUNT_VERGIL);
					}
					};

				meleeSlider();

				queuedCharacterDataClone.meleeWeaponCount = queuedCharacterData.meleeWeaponCount;


				old_for_all(uint8, meleeWeaponIndex, WEAPON_COUNT_VERGIL) {
					bool condition = (meleeWeaponIndex >= queuedCharacterData.meleeWeaponCount);

					GUI_PushDisable(condition);

					// Check if the queuedCharacter matches the activeCharacter for realTime WeaponSwitching
					if (queuedCharacterData.character == activeCharacterData.character) {
						if (UI::ComboMapValue2("", meleeWeaponNamesVergil, meleeWeaponsVergil,
							queuedCharacterData.meleeWeapons[meleeWeaponIndex], activeCharacterData.meleeWeapons[meleeWeaponIndex])) {

							if (!newActorData.baseAddr) break;
							auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);
							actorData.meleeWeaponIndex = queuedCharacterData.meleeWeapons[meleeWeaponIndex];
						}
					} else {
						UI::ComboMapValue("", meleeWeaponNamesVergil, meleeWeaponsVergil,
							queuedCharacterData.meleeWeapons[meleeWeaponIndex]);
					}

					// Doppelganger will now have same weapons equipped as Dante - Mia.
					queuedCharacterDataClone.meleeWeapons[meleeWeaponIndex] = queuedCharacterData.meleeWeapons[meleeWeaponIndex];

					GUI_PopDisable(condition);
				}
			}
		}
			//I think one of these is ranged weapons and we only pop it if we're dante?
			if (queuedCharacterData.character == CHARACTER::DANTE) {
				ImGui::PopItemWidth();
			};
			ImGui::PopItemWidth();

			ImGui::EndTable();

		}
	ImGui::PopFont();
}

void Actor_PlayerTab(uint8 playerIndex, size_t defaultFontSize) {
	auto& activePlayerData = GetActivePlayerData(playerIndex);
	auto& queuedPlayerData = GetQueuedPlayerData(playerIndex);

	const float columnWidth = 0.5f * queuedConfig.globalScale;
	const float rowWidth = 40.0f * queuedConfig.globalScale;

	ImGui::Text("");

	ImGui::PushItemWidth(itemWidth);
	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	if (GUI_Checkbox2("任务中切换角色", activeCrimsonGameplay.Gameplay.General.charHotswap, queuedCrimsonGameplay.Gameplay.General.charHotswap)) {}
	
	GUI_PushDisable(!activeCrimsonGameplay.Gameplay.General.charHotswap);
	if (!activeCrimsonGameplay.Gameplay.General.charHotswap) {
		queuedPlayerData.characterCount = 1;
	}
	GUI_Slider<uint8>("角色数量", queuedPlayerData.characterCount, 1, CHARACTER_COUNT);


	UI::ComboMap2("切换按键", buttonNames, buttons, Actor_buttonIndices[playerIndex], activePlayerData.switchButton, queuedPlayerData.switchButton,
		ImGuiComboFlags_HeightLargest);
	ImGui::SameLine();
	TooltipHelper("(?)", "Press to Switch Loadouts or Characters.\n"
		"Hold the button while pressing L2/R2 to switch Doppelganger's weapons while it's active.\n");

	if (playerIndex != 0) {

		UI::ComboMap("类型（碰撞组）", collisionGroupNames, collisionGroups, Actor_collisionGroupIndices[playerIndex],
			queuedPlayerData.collisionGroup);

	}
	GUI_PopDisable(!activeCrimsonGameplay.Gameplay.General.charHotswap);
	BackgroundPlayerText(playerIndex);


	//     if (GUI_Button("重置")) {
	//         CopyMemory(&queuedConfig.Actor.playerData[playerIndex].removeBusyFlag, &defaultConfig.Actor.playerData[playerIndex].removeBusyFlag,
	//             sizeof(queuedConfig.Actor.playerData[playerIndex].removeBusyFlag));
	//         CopyMemory(&activeConfig.Actor.playerData[playerIndex].removeBusyFlag, &queuedConfig.Actor.playerData[playerIndex].removeBusyFlag,
	//             sizeof(activeConfig.Actor.playerData[playerIndex].removeBusyFlag));
	// 
	//         CopyMemory(&queuedConfig.Actor.playerData[playerIndex].removeBusyFlagButtons,
	//             &defaultConfig.Actor.playerData[playerIndex].removeBusyFlagButtons,
	//             sizeof(queuedConfig.Actor.playerData[playerIndex].removeBusyFlagButtons));
	//         CopyMemory(&activeConfig.Actor.playerData[playerIndex].removeBusyFlagButtons,
	//             &queuedConfig.Actor.playerData[playerIndex].removeBusyFlagButtons,
	//             sizeof(activeConfig.Actor.playerData[playerIndex].removeBusyFlagButtons));
	// 
	// 
	//         Actor_UpdateIndices();
	//     }
	/*    ImGui::Text("");*/


	// 	GUI_Checkbox2("移除忙碌标记", activePlayerData.removeBusyFlag, queuedPlayerData.removeBusyFlag);
	// 	ImGui::SameLine();
	// 	TooltipHelper("(?)", "移除角色的忙碌标记，可打出疯狂连段，也可能显得很蠢。");
	// 	ImGui::Text("");
	// 
	// 
	//     {
	//         bool condition = !activePlayerData.removeBusyFlag;
	// 
	//         GUI_PushDisable(condition);
	// 
	// 
	//         old_for_all(uint8, buttonIndex, 4) {
	//             UI::ComboMap2(buttonIndexNames[buttonIndex], buttonNames, buttons, Actor_removeBusyFlagButtonIndices[playerIndex][buttonIndex],
	//                 activePlayerData.removeBusyFlagButtons[buttonIndex], queuedPlayerData.removeBusyFlagButtons[buttonIndex],
	//                 ImGuiComboFlags_HeightLargest);
	//         }
	// 
	// 
	//         GUI_PopDisable(condition);
	// 
	//     }

	ImGui::PopItemWidth();
	ImGui::PopFont();
}

void SelectPlayerLoadoutsWeaponsTab() {
	auto defaultFontSize = UI::g_UIContext.DefaultFontSize;

	const float columnWidth = 0.8f * queuedConfig.globalScale;
	const float rowWidth = 40.0f * queuedConfig.globalScale;

	uint8 activePlayerIndex;

	ImGui::PushFont(UI::g_ImGuiFont_Benguiat[defaultFontSize * 1.0]);
	ImGui::Text("PLAYER SETTINGS / LOADOUTS");
	ImGui::PopFont();

	if (ImGui::BeginTable("CharacterTable", 2)) {

		// ImGui::TableSetColumnWidth(0, columnWidth);
		ImGui::TableSetupColumn("c1", 0, columnWidth);
		ImGui::TableNextRow(0, rowWidth);
		ImGui::TableNextColumn();


		if (ImGui::BeginTabBar("PlayerTabs")) {
			old_for_all(uint8, playerIndex, PLAYER_COUNT) {
				auto condition = (playerIndex >= queuedConfig.Actor.playerCount);

				GUI_PushDisable(condition);

				ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);

				if (ImGui::BeginTabItem(playerIndexNames[playerIndex])) {
					//ImGui::Text("");


					Actor_PlayerTab(playerIndex, defaultFontSize);
					activePlayerIndex = playerIndex;


					ImGui::EndTabItem();
				}

				ImGui::PopFont();


				GUI_PopDisable(condition);

			}

			ImGui::EndTabBar();
		}

		ImGui::TableNextColumn();

		if (ImGui::BeginTabBar("CharacterTabs")) {

			auto& activePlayerData = GetActivePlayerData(activePlayerIndex);
			auto& queuedPlayerData = GetQueuedPlayerData(activePlayerIndex);


			old_for_all(uint8, characterIndex, CHARACTER_COUNT) {
				auto condition = (characterIndex >= queuedPlayerData.characterCount);

				GUI_PushDisable(condition);

				ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);

				if (ImGui::BeginTabItem(characterIndexNames[characterIndex])) {

					Actor_CharacterTab(activePlayerIndex, characterIndex, 0, defaultFontSize);

					ImGui::EndTabItem();
				}

				ImGui::PopFont();

				GUI_PopDisable(condition);
			}

			ImGui::EndTabBar();
		}

		ImGui::EndTable();

	}

}

void CharacterSection(size_t defaultFontSize) {

	const float columnWidth = 0.8f * queuedConfig.globalScale;
	const float rowWidth = 40.0f * queuedConfig.globalScale;
	ImU32 checkmarkColor = UI::SwapColorEndianness(0xFFFFFFFF);

	bool actorCondition = (!queuedConfig.Actor.enable);

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);
	GUI_PushDisable(activeCrimsonGameplay.GameMode.preset < GAMEMODEPRESETS::CUSTOM);
	GUI_Checkbox("绯红角色系统（CCS）", queuedConfig.Actor.enable);
	GUI_PopDisable(activeCrimsonGameplay.GameMode.preset < GAMEMODEPRESETS::CUSTOM);
	ImGui::PopFont();

	if (!queuedConfig.Actor.enable) {
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.871f, 0.110f, 0.298f, 1.0f));
		ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
		ImGui::Text("绯红角色系统已禁用，大部分玩法选项和多人模式将无法使用。");
		ImGui::PopStyleColor();
		ImGui::PopFont();
	}
	UI::SeparatorEx(defaultFontSize * 23.35f);

	GUI_PushDisable(actorCondition);

	ImGui::Text("");

	{
		const float columnWidth = 0.6f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;

		if (ImGui::BeginTable("ActorPlayersTable", 2)) {

			ImGui::TableSetupColumn("c2", 0, columnWidth * 2.0f);
			ImGui::TableNextRow(0, rowWidth);

			ImGui::TableNextColumn();

			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);
			const char* SPMPText = (queuedConfig.Actor.playerCount == 1) ? SPMPText = "单人游戏" : SPMPText = "多人游戏";
			ImGui::Text(SPMPText);
			ImGui::PopFont();
			BackgroundSPMPText(SPMPText);


			ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
			ImGui::PushItemWidth(itemWidth);
			if (GUI_Slider<uint8>("玩家数量", queuedConfig.Actor.playerCount, 1, PLAYER_COUNT)) {
				if (queuedConfig.Actor.playerCount > 1) {
					activeCrimsonConfig.Camera.multiplayerCamera = true;
					queuedCrimsonConfig.Camera.multiplayerCamera = true;
					activeCrimsonConfig.Camera.thirdPersonCamera = true;
					queuedCrimsonConfig.Camera.thirdPersonCamera = true;
				} else {
					activeConfig.enablePVPFixes = false;
					queuedConfig.enablePVPFixes = false;
				}
			}
			ImGui::PopItemWidth();
			ImGui::PopFont();
			ImGui::TableNextColumn();

			ImGui::Text("");

			if (GUI_Button("按键配置")) {
				g_showControllerRemap = true;
			}
			ImGui::SameLine();
			TooltipHelper("(?)", "打开手柄按键重映射窗口，为每位玩家和角色自定义按键。");

			ImGui::TableNextColumn();


			ImGui::PushItemWidth(itemWidth);
			UI::Combo2("DMC3 服装游戏进度", costumeRespectsProgressionNames, activeConfig.costumeRespectsProgression,
				queuedConfig.costumeRespectsProgression);
			ImGui::PopItemWidth();

			ImGui::SameLine();
			TooltipHelper("(?)", "Makes DMC3 Costume update as the game progresses, as in the Vanilla game.\n"
				"\n"
				"'Crimson' updates Vergil's Costume at Mission 1.");


			ImGui::EndTable();
		}
	}

	{
		const float columnWidth = 0.6f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;

		if (ImGui::BeginTable("ActorSystemTable", 2)) {

			ImGui::TableSetupColumn("c2", 0, columnWidth * 2.0f);
			ImGui::TableNextRow(0, rowWidth);

			ImGui::TableNextColumn();

			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("PLAYER NAMES / COLORS");

			// Static persistent buffers for player names - survive across frames for proper ImGui text editing
			static char s_playerNameBuffers[PLAYER_COUNT][20] = {};
			static bool s_buffersInitialized = false;

			// Initialize buffers once from config
			if (!s_buffersInitialized) {
				for (int i = 0; i < PLAYER_COUNT; i++) {
					const std::string& configName = queuedCrimsonConfig.PlayerProperties.playerName[i];
					size_t copyLen = (configName.length() < 19) ? configName.length() : 19;
					memcpy(s_playerNameBuffers[i], configName.c_str(), copyLen);
					s_playerNameBuffers[i][copyLen] = '\0';
				}
				s_buffersInitialized = true;
			}

			for (int playerIndex = 0; playerIndex < queuedConfig.Actor.playerCount; playerIndex++) {

				ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);

				std::string inputLabel = "##playerName" + std::to_string(playerIndex);
				ImGui::PushItemWidth(itemWidth * 1.3f);

				// ImGui edits the persistent buffer directly
				if (ImGui::InputText(inputLabel.c_str(), s_playerNameBuffers[playerIndex], 20)) {
					activeCrimsonConfig.PlayerProperties.playerName[playerIndex] = s_playerNameBuffers[playerIndex];
					queuedCrimsonConfig.PlayerProperties.playerName[playerIndex] = s_playerNameBuffers[playerIndex];
					GUI::save = true;
				}

				ImGui::PopItemWidth();
				ImGui::PopFont();

				ImGui::SameLine();
				GUI_Color2("", activeCrimsonConfig.PlayerProperties.playerColor[playerIndex], queuedCrimsonConfig.PlayerProperties.playerColor[playerIndex]);
				ImGui::SameLine();
				ImGui::Text("%uP", playerIndex + 1);
				ImGui::SameLine();
				ImGui::SameLine();
				if (GUI_Button("D")) {
					CopyMemory(&activeCrimsonConfig.PlayerProperties.playerColor[playerIndex], &defaultCrimsonConfig.PlayerProperties.playerColor[playerIndex],
						sizeof(activeCrimsonConfig.PlayerProperties.playerColor[playerIndex]));
					CopyMemory(&queuedCrimsonConfig.PlayerProperties.playerColor[playerIndex], &defaultCrimsonConfig.PlayerProperties.playerColor[playerIndex],
						sizeof(queuedCrimsonConfig.PlayerProperties.playerColor[playerIndex]));
				}


			}
			ImGui::PopFont();
			ImGui::TableNextColumn();

			ImGui::Text("");
			ImGui::Text("");

			GUI_PushDisable(queuedConfig.Actor.playerCount <= 1);
			GUI_Checkbox2("PVP 修复", activeConfig.enablePVPFixes, queuedConfig.enablePVPFixes);
			ImGui::SameLine();
			TooltipHelper("(?) WARNING", "Allows you to set up PVP Multiplayer.\n"
				"Lock On in PVP requires an enemy spawned in order to work properly.\n"
				"警告：此选项会导致真实敌人卡死，请谨慎使用。", 2048.0f, true);
			GUI_PopDisable(queuedConfig.Actor.playerCount <= 1);

			ImGui::EndTable();
		}
	}

	SelectPlayerLoadoutsWeaponsTab();

	GUI_PopDisable(actorCondition);


	GUI_PushDisable(actorCondition);

	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColor);

	

	GUI_PopDisable(actorCondition);
	ImGui::PopStyleColor();
	//
	//ImGui::PopItemWidth();
	//ImGui::PopFont();
}

#pragma endregion

#pragma region Arcade

uint8 Arcade_modeIndex = 0;
uint8 Arcade_meleeWeaponIndexDante[2] = {};
uint8 Arcade_rangedWeaponIndexDante[2] = {};


void Arcade_UpdateIndices() {
	UpdateMapIndex(modes, Arcade_modeIndex, activeConfig.Arcade.mode);

	UpdateMapIndex(meleeWeaponsDante, Arcade_meleeWeaponIndexDante[0], activeConfig.Arcade.weapons[0]);

	UpdateMapIndex(meleeWeaponsDante, Arcade_meleeWeaponIndexDante[1], activeConfig.Arcade.weapons[1]);

	UpdateMapIndex(rangedWeaponsDante, Arcade_rangedWeaponIndexDante[0], activeConfig.Arcade.weapons[2]);

	UpdateMapIndex(rangedWeaponsDante, Arcade_rangedWeaponIndexDante[1], activeConfig.Arcade.weapons[3]);
}

void ArcadeSection(size_t defaultFontSize) {

	const float columnWidth = 0.5f * queuedConfig.globalScale;
	const float rowWidth = 40.0f * queuedConfig.globalScale;

	ImU32 highlightColorBg = UI::SwapColorEndianness(0xFFFFFFFF);
	ImU32 highlightColorText = UI::SwapColorEndianness(0xFF809FFF);


	if (GUI_TitleCheckbox2("街机模式", activeConfig.Arcade.enable, queuedConfig.Arcade.enable)) {
		Arcade::Toggle(activeConfig.Arcade.enable);
	}

	ImGui::PushItemWidth(itemWidth);

	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, highlightColorBg);

	if (ImGui::BeginTable("ArcadeTable", 2)) {

		// ImGui::TableSetColumnWidth(0, columnWidth);
		ImGui::TableSetupColumn("c1", 0, columnWidth);
		ImGui::TableNextRow(0, rowWidth * 0.5f);

		ImGui::TableNextColumn();

		ImGui::PushItemWidth(itemWidth);

		UI::Combo2("", missionNames, activeConfig.Arcade.mission, queuedConfig.Arcade.mission, ImGuiComboFlags_HeightLargest);

		ImGui::SameLine();
		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);
		ImGui::Text("任务");
		ImGui::PopFont();


		ImGui::TableNextColumn();

		if (activeConfig.Arcade.mission > 0) {
			ImGui::PushItemWidth(itemWidth);
			UI::ComboMap2("", modeNames, modes, Arcade_modeIndex, activeConfig.Arcade.mode, queuedConfig.Arcade.mode);

			ImGui::SameLine();
			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);
			ImGui::Text("难度");
			ImGui::PopFont();

		}

		ImGui::TableNextRow(0, rowWidth);
		ImGui::TableNextColumn();


		if (activeConfig.Arcade.mission > 0) {
			GUI_InputDefault2<float>("最大生命", activeConfig.Arcade.hitPoints, queuedConfig.Arcade.hitPoints,
				defaultConfig.Arcade.hitPoints, 1000, "%g", ImGuiInputTextFlags_EnterReturnsTrue);

			ImGui::TableNextColumn();

			GUI_InputDefault2<float>("最大魔人化", activeConfig.Arcade.magicPoints, queuedConfig.Arcade.magicPoints,
				defaultConfig.Arcade.magicPoints, 1000, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
		}

		ImGui::TableNextRow(0, rowWidth);
		ImGui::TableNextColumn();

		UI::Combo2("战役", ddmkCharacter2PNames, activeConfig.Arcade.character, queuedConfig.Arcade.character);

		{
			auto queuedCharacterData = GetQueuedCharacterData(0, 0, 0);
			bool disableCondition = queuedConfig.Actor.enable;

			if (activeConfig.Arcade.mission > 0 && (queuedCharacterData.ignoreCostume || !queuedConfig.Actor.enable)) {
				ImGui::TableNextColumn();
				if (queuedConfig.Arcade.character == CHARACTER::DANTE) {
					UI::Combo2("服装", costumeNamesDante, activeConfig.Arcade.costume, queuedConfig.Arcade.costume);
				} else if (queuedConfig.Arcade.character == CHARACTER::VERGIL) {
					UI::Combo2("服装", costumeNamesVergil, activeConfig.Arcade.costume, queuedConfig.Arcade.costume);
				} else if (queuedConfig.Arcade.character == CHARACTER::LADY || queuedConfig.Arcade.character == CHARACTER::BOSS_LADY) {
					UI::Combo2("服装", costumeNamesLady, activeConfig.Arcade.costume, queuedConfig.Arcade.costume);
				}

			}


			if ((activeConfig.Arcade.mission > 0) && (activeConfig.Arcade.character == CHARACTER::DANTE) && !queuedConfig.Actor.enable) {
				ImGui::TableNextRow(0, rowWidth);
				ImGui::TableNextColumn();

				UI::Combo2("Style                  ", styleNamesDante, activeConfig.Arcade.style, queuedConfig.Arcade.style);

				ImGui::TableNextColumn();
				UI::ComboMap2("远程 1", rangedWeaponNamesDante, rangedWeaponsDante, Arcade_rangedWeaponIndexDante[0],
					activeConfig.Arcade.weapons[2], queuedConfig.Arcade.weapons[2]);

				ImGui::TableNextRow(0, rowWidth);
				ImGui::TableNextColumn();

				UI::ComboMap2("近战 1", meleeWeaponNamesDante, meleeWeaponsDante, Arcade_meleeWeaponIndexDante[0],
					activeConfig.Arcade.weapons[0], queuedConfig.Arcade.weapons[0]);

				ImGui::TableNextColumn();
				UI::ComboMap2("远程 2", rangedWeaponNamesDante, rangedWeaponsDante, Arcade_rangedWeaponIndexDante[1],
					activeConfig.Arcade.weapons[3], queuedConfig.Arcade.weapons[3]);

				ImGui::TableNextRow(0, rowWidth);
				ImGui::TableNextColumn();
				UI::ComboMap2("近战 2", meleeWeaponNamesDante, meleeWeaponsDante, Arcade_meleeWeaponIndexDante[1],
					activeConfig.Arcade.weapons[1], queuedConfig.Arcade.weapons[1]);

			}

		}

		if ((activeConfig.Arcade.mission >= 1) && (activeConfig.Arcade.mission <= 20)) {

			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			// Room
			{
				bool condition = !activeConfig.Arcade.enableRoomSelection;

				GUI_PushDisable(condition);

				ImGui::PushItemWidth(itemWidth * 1.3f);
				UI::ComboMapValue2("", roomNames, roomsMapu32, activeConfig.Arcade.room, queuedConfig.Arcade.room, 0);
				ImGui::PopItemWidth();

				GUI_PopDisable(condition);

				ImGui::SameLine();
				ImGui::Text("房间");


				ImGui::SameLine();
				GUI_Checkbox2("", activeConfig.Arcade.enableRoomSelection, queuedConfig.Arcade.enableRoomSelection);
			}

			ImGui::TableNextColumn();

			// Position
			{
				bool condition = !activeConfig.Arcade.enablePositionSelection;

				GUI_PushDisable(condition);

				GUI_InputDefault2<uint32>("", activeConfig.Arcade.position, queuedConfig.Arcade.position,
					defaultConfig.Arcade.position, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);

				GUI_PopDisable(condition);

				ImGui::SameLine();
				ImGui::Text("位置");

				ImGui::SameLine();
				GUI_Checkbox2("", activeConfig.Arcade.enablePositionSelection, queuedConfig.Arcade.enablePositionSelection);
			}
		}

		if (activeConfig.Arcade.mission == MISSION::BLOODY_PALACE) {

			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			UI::Combo2("楼层", floorNames, activeConfig.Arcade.floor, queuedConfig.Arcade.floor, ImGuiComboFlags_HeightLargest);

			ImGui::TableNextColumn();

			GUI_InputDefault2<uint16>("关卡", activeConfig.Arcade.level, queuedConfig.Arcade.level, defaultConfig.Arcade.level, 1, "%u",
				ImGuiInputTextFlags_EnterReturnsTrue);
		}


		ImGui::EndTable();
	}


	ImGui::SameLine();

	//
	ImGui::PopItemWidth();
	ImGui::PopFont();
	ImGui::PopStyleColor();

	ImGui::Text("");

}

#pragma endregion

#pragma region MultiplayerBars

const char* barsNames[PLAYER_COUNT] = {
	"Bars1",
	"Bars2",
	"Bars3",
	"Bars4",
};

bool showBars = false;

void RoyalGaugeMainPlayer() {
	if (!(activeConfig.Actor.enable && InGame() && crimsonPlayer[0].character == CHARACTER::DANTE && !g_inGameCutscene)) {
		return;
	}
	static bool show = true;
	auto name_80 = *reinterpret_cast<byte8**>(appBaseAddr + 0xCF2680);
	if (!name_80) {
		return;
	}
	auto& hudData = *reinterpret_cast<HUDData*>(name_80);

	auto pool_10222 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if (!pool_10222 || !pool_10222[3]) {
		return;
	}
	auto& mainActorData = *reinterpret_cast<PlayerActorData*>(pool_10222[3]);

	auto royalGauge = mainActorData.royalguardReleaseDamage;
	float miragePointsColor[4] = { 1.0f , 1.0f, 1.0, hudData.topLeftAlpha / 127.0f };
	float progressBarBgColor[4] = { 0.2f , 0.2f, 0.2f, hudData.topLeftAlpha / 127.0f };

	// Base resolution (1920x1080)
	const float baseWidth = 1920.0f;
	const float baseHeight = 1080.0f;

	// Calculate scaling factors
	float widthScale = g_windowSize.x / baseWidth;
	float heightScale = g_windowSize.y / baseHeight;

	// Adjust the size of the bar
	float barLength = 130.0f * widthScale * (crimsonPlayer[0].vergilDoppelganger.maxMiragePoints / maxMiragePointsAmount);
	vec2 size = { barLength, 10.0f * heightScale };

	// Calculate position 
	float posX = 90.0f * widthScale;
	float posY = 170.0f * heightScale;

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;

	ImGui::SetNextWindowPos(ImVec2(posX, posY));

	if (ImGui::Begin("MirageMainPlayer", &show, windowFlags)) {
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, *reinterpret_cast<ImVec4*>(&miragePointsColor));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, *reinterpret_cast<ImVec4*>(&progressBarBgColor));
		ImGui::ProgressBar(royalGauge, *reinterpret_cast<ImVec2*>(&size), "");
		ImGui::PopStyleColor(2);
	}

	ImGui::End();
}

void RenderMissionResultGameModeStats() {
	using namespace UI;
	auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);
	const char* missionResultGameModeString = nullptr;
	ImU32 gameModeStringColor = 0;
	auto defaultFontSize = g_UIContext.DefaultFontSize;
	ImVec2 windowSize = ImVec2(1800.0f * scaleFactorY, 200.0f * scaleFactorY);
	ImVec2 windowPos = ImVec2(
		(g_renderSize.x - windowSize.x) * 0.5f + (990.0f * scaleFactorY),
		scaleFactorY / 2 + (147.0f * scaleFactorY)
	);

	// We use this to identify whether or not we're in the exact Mission Result Screen
	uint32 inMissionResultScreenInt = *reinterpret_cast<uint32*>(appBaseAddr + 0x5CF9A0);
	bool inMissionResultScreen = true;
	bool shouldShow = false;
	if (sessionData.mission == MISSION::BLOODY_PALACE || sessionData.mission == 0) { // Skips Screen Break
		shouldShow = g_inMissionResultBPScreen;
	}
	else if (sessionData.mission == 20) { // Skips Screen Break
		shouldShow = g_inMissionResultScreen;
	}
	else {
		if (activeConfig.skipCutscenes) { // Skips Screen Break
			shouldShow = g_inMissionResultScreen;
		}
		else {
			shouldShow = g_inMissionResultScreen && g_screenBreakTriggered;
		}
	}

	if (g_scene != SCENE::MISSION_RESULT) {
		g_screenBreakTriggered = false;
	}

	// Static variables to track fade state
	static bool wasShowing = false;
	static float fadeStartTime = 0.0f;
	static bool isFadingIn = false;
	static bool isFadingOut = false;

	float currentTime = ImGui::GetTime();
	const float fadeDuration = 1.0f; // 1 second fade

	// Handle fade state transitions
	if (shouldShow && !wasShowing) {
		// Start fade in
		isFadingIn = true;
		isFadingOut = false;
		fadeStartTime = currentTime;
	} else if (!shouldShow && wasShowing) {
		// Start fade out
		isFadingIn = false;
		isFadingOut = true;
		fadeStartTime = currentTime;
	}

	wasShowing = shouldShow;

	// Calculate alpha based on fade state
	float alpha = 1.0f;
	if (isFadingIn) {
		float fadeProgress = (currentTime - fadeStartTime) / fadeDuration;
		if (fadeProgress >= 1.0f) {
			alpha = 1.0f;
			isFadingIn = false;
		} else {
			alpha = fadeProgress;
		}
	} else if (isFadingOut) {
		float fadeProgress = GetFadeToBlackProgress(30.0f);
		if (fadeProgress >= 1.0f) {
			alpha = 0.0f;
			isFadingOut = false;
			return; // Completely faded out, stop rendering
		} else {
			alpha = 1.0f - fadeProgress;
		}
	} else if (!shouldShow) {
		return; // Not showing and not fading, don't render
	}

	// Helper function to apply alpha to color
	auto applyAlpha = [alpha](ImU32 color) -> ImU32 {
		ImU32 a = (ImU32)(alpha * 255.0f);
		return (color & 0x00FFFFFF) | (a << 24);
		};

	switch (gameModeData.missionResultGameMode) {
	case GAMEMODEPRESETS::VANILLA:
		missionResultGameModeString = gameModeData.names[GAMEMODEPRESETS::VANILLA].c_str();
		gameModeStringColor = applyAlpha(0xFFFFFFFF);
		break;

	case GAMEMODEPRESETS::STYLE_SWITCHER:
		missionResultGameModeString = gameModeData.names[GAMEMODEPRESETS::STYLE_SWITCHER].c_str();
		gameModeStringColor = applyAlpha(SwapColorEndianness(0xE8BA18FF));
		break;

	case GAMEMODEPRESETS::CRIMSON:
		missionResultGameModeString = gameModeData.names[GAMEMODEPRESETS::CRIMSON].c_str();
		gameModeStringColor = applyAlpha(SwapColorEndianness(0xDA1B53FF));
		break;

	case GAMEMODEPRESETS::CUSTOM:
		missionResultGameModeString = gameModeData.names[GAMEMODEPRESETS::CUSTOM].c_str();
		gameModeStringColor = applyAlpha(SwapColorEndianness(0x4050FFFF));
		break;

	case GAMEMODEPRESETS::UNRATED:
		missionResultGameModeString = gameModeData.names[GAMEMODEPRESETS::UNRATED].c_str();
		gameModeStringColor = applyAlpha(0xD5D9DBFF);
		break;

	default:
		missionResultGameModeString = "未知";
		gameModeStringColor = applyAlpha(0xFFFFFFFF);
		break;
	}

	// Apply alpha to ImGui style
	ImGui::PushStyleColor(ImGuiCol_Text, applyAlpha(0xD5D9DBFF));
	//ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);

	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(windowSize + ImVec2(50.0f, 50.0f), ImGuiCond_Always);
	ImFont* font = UI::g_ImGuiFont_Messenger[30.0f];
	ImGui::PushFont(font);

	if (ImGui::Begin("MissionResultStats", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoBackground)) {

		ImGui::SetWindowFontScale(scaleFactorY);

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImVec2 textSize = font->CalcTextSizeA(font->FontSize, FLT_MAX, 0.0f, missionResultGameModeString);
		ImVec2 textPos = ImGui::GetWindowPos() + ImVec2(10.0f * scaleFactorY, 0.0f);

		UI::DropShadowText(g_ImGuiFont_Benguiat256, scaledFontSize * 3.8f, textPos, UI::ColorToImVec4(UI::SwapColorEndianness(gameModeStringColor)), missionResultGameModeString,NULL,0.0f,NULL, ImVec2(5.0f,5.0f));

		ImGui::End();
	}

	ImVec2 difficultyWindowPos = ImVec2(
		(g_renderSize.x - windowSize.x) * 0.5f + (268.0f * scaleFactorY),
		scaleFactorY / 2 + (230.0f * scaleFactorY)
	);

	ImGui::SetNextWindowPos(difficultyWindowPos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(windowSize + ImVec2(50.0f, 50.0f), ImGuiCond_Always);

	auto currentDifficultyMode = activeCrimsonGameplay.Gameplay.ExtraDifficulty.forceDifficultyMode != DIFFICULTY_MODE::FORCE_DIFFICULTY_OFF ?
		activeCrimsonGameplay.Gameplay.ExtraDifficulty.forceDifficultyMode
		: sessionData.difficultyMode;

	if (ImGui::Begin("DifficultyStats", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoBackground)) {

		std::string difficultyDisplay = gameModeData.difficultyModeNames[currentDifficultyMode];
		if (currentDifficultyMode == DIFFICULTY_MODE::HARD && sessionData.oneHitKill) {
			difficultyDisplay = "天堂或地狱";
		}
		if (activeCrimsonGameplay.Gameplay.ExtraDifficulty.forceDifficultyMode != DIFFICULTY_MODE::FORCE_DIFFICULTY_OFF) {
			difficultyDisplay += " (Forced)";
		}
		const char* difficultyString = difficultyDisplay.c_str();
		ImVec2 difficultyTextSize = font->CalcTextSizeA(font->FontSize, FLT_MAX, 0.0f, difficultyString);
		ImGui::SetWindowFontScale(scaleFactorY);

		ImVec2 difficultyTextPos = ImGui::GetWindowPos() + ImVec2(10.0f * scaleFactorY, 0.0f);

		ImGui::SetCursorScreenPos(difficultyTextPos);
		ImGui::Text("%s", difficultyString);

		ImGui::SameLine();
		std::string ldkMissionText = std::string(reinterpret_cast<const char*>(u8"• ")) + ldkModeNames[gameModeData.ldkNissionResult];
		std::string mustStyleMissionText = std::string(reinterpret_cast<const char*>(u8"• Must Style (")) + std::string(styleRankNames[gameModeData.mustStyleMissionResult]) + ")";
		std::string enemyDTMissionText = (std::string(reinterpret_cast<const char*>(u8"• ")) + (std::string)enemyDTModeNames[gameModeData.enemyDTMissionResult]);
		std::string forceDifficultyText = (std::string(reinterpret_cast<const char*>(u8"• Force Difficulty")));

		if (gameModeData.ldkNissionResult != LDKMODE::OFF) {
			ImGui::Text(ldkMissionText.c_str());
		}
		ImGui::SameLine();
		if (gameModeData.mustStyleMissionResult != STYLE_RANK::NONE) {
			ImGui::Text(mustStyleMissionText.c_str());
		}
		ImGui::SameLine();
		if (gameModeData.enemyDTMissionResult != ENEMYDTMODE::DEFAULT && sessionData.difficultyMode == DIFFICULTY_MODE::DANTE_MUST_DIE) {
			ImGui::Text(enemyDTMissionText.c_str());
		}

		if (gameModeData.forceDifficultyResult) {
			ImGui::Text(forceDifficultyText.c_str());
		}

		ImGui::End();
	}

	ImGui::SetNextWindowPos(ImVec2(difficultyWindowPos.x + (70.0f), difficultyWindowPos.y + (620.0)), ImGuiCond_Always);
	ImGui::SetNextWindowSize(windowSize + ImVec2(50.0f, 50.0f), ImGuiCond_Always);

	if (ImGui::Begin("MiscGameOptionsWindow", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoBackground)) {

		if (gameModeData.arcadeMissionEnabled) {
			ImGui::PushFont(g_ImGuiFont_Messenger[25.0f]);
			ImGui::Text("街机");
			ImGui::PopFont();
		}

		if (gameModeData.arcadeMissionEnabled && gameModeData.bossRushMissionEnabled) {
			ImGui::SameLine();
			ImGui::Text((const char*)u8"•");
		}

		if (gameModeData.bossRushMissionEnabled) {
			ImGui::SameLine();
			ImGui::PushFont(g_ImGuiFont_Messenger[25.0f]);
			ImGui::Text("Boss 连战");
			ImGui::PopFont();
		}

		if ((gameModeData.arcadeMissionEnabled || gameModeData.bossRushMissionEnabled) &&
			gameModeData.characterSwitchingEnabled) {
			ImGui::SameLine();
			ImGui::Text((const char*)u8"•");
		}

		if (gameModeData.characterSwitchingEnabled) {
			ImGui::SameLine();
			ImGui::PushFont(g_ImGuiFont_Messenger[25.0f]);
			ImGui::Text("角色切换");
			ImGui::PopFont();
		}

		if ((gameModeData.arcadeMissionEnabled || gameModeData.bossRushMissionEnabled 
			|| gameModeData.characterSwitchingEnabled) &&
			gameModeData.multiplayerEnabled) {
			ImGui::SameLine();
			ImGui::Text((const char*)u8"•");
		}

		if (gameModeData.multiplayerEnabled) {
			ImGui::SameLine();
			ImGui::PushFont(g_ImGuiFont_Messenger[25.0f]);
			ImGui::Text("多人模式");

			if (activeCrimsonGameplay.Gameplay.General.multiplayerDamageScaling && 
				!activeCrimsonGameplay.Cheats.General.customDamage) {
				ImGui::SameLine();
				ImGui::Text(" (Dmg Scaling %.1f)", activeCrimsonGameplay.Cheats.Damage.enemyReceivedDmgMult);
			} else {
				ImGui::SameLine();
				ImGui::Text(" (Dmg Scaling OFF)");
			}
			ImGui::PopFont();
		}

		ImGui::End();
	}

	ImGui::PopFont();
	ImGui::PopStyleColor(); // Pop the alpha style var
}

void RenderMissionResultVersionDisplay() {
	using namespace UI;
	auto sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);
	// We use this to identify whether or not we're in the exact Mission Result Screen
	uint32 inMissionResultScreenInt = *reinterpret_cast<uint32*>(appBaseAddr + 0x5CF9A0);
	bool inMissionResultScreen = true;
	bool shouldShow = false;
	if (sessionData.mission == MISSION::BLOODY_PALACE || sessionData.mission == 0) { // Skips Screen Break
		shouldShow = g_inMissionResultBPScreen;
	}
	else if (sessionData.mission == 20) { // Skips Screen Break
		shouldShow = g_inMissionResultScreen;
	}
	else {
		if (activeConfig.skipCutscenes) { // Skips Screen Break
			shouldShow = g_inMissionResultScreen;
		}
		else {
			shouldShow = g_inMissionResultScreen && g_screenBreakTriggered;
		}
	}

	if (g_scene != SCENE::MISSION_RESULT) {
		g_screenBreakTriggered = false;
	}

	if (g_scene != SCENE::MISSION_RESULT) {
		g_screenBreakTriggered = false;
	}

	// Static variables to track fade state (shared with other mission result windows)
	static bool wasShowing = false;
	static float fadeStartTime = 0.0f;
	static bool isFadingIn = false;
	static bool isFadingOut = false;

	float currentTime = ImGui::GetTime();
	const float fadeDuration = 1.0f;

	// Handle fade state transitions
	if (shouldShow && !wasShowing) {
		isFadingIn = true;
		isFadingOut = false;
		fadeStartTime = currentTime;
	} else if (!shouldShow && wasShowing) {
		isFadingIn = false;
		isFadingOut = true;
		fadeStartTime = currentTime;
	}

	wasShowing = shouldShow;

	// Calculate alpha based on fade state
	float alpha = 1.0f;
	if (isFadingIn) {
		float fadeProgress = (currentTime - fadeStartTime) / fadeDuration;
		if (fadeProgress >= 1.0f) {
			alpha = 1.0f;
			isFadingIn = false;
		} else {
			alpha = fadeProgress;
		}
	} else if (isFadingOut) {
		float fadeProgress = GetFadeToBlackProgress(30.0f);
		if (fadeProgress >= 1.0f) {
			alpha = 0.0f;
			isFadingOut = false;
			return;
		} else {
			alpha = 1.0f - fadeProgress;
		}
	} else if (!shouldShow) {
		return;
	}

	// Build version string
	std::string versionStr{};
	if (g_UIContext.CurrentVersion.PatchLetter == 0) {
		versionStr = std::format("v{}.{}", g_UIContext.CurrentVersion.Major, g_UIContext.CurrentVersion.Minor);
	} else {
		versionStr = std::format("v{}.{}{}", g_UIContext.CurrentVersion.Major, g_UIContext.CurrentVersion.Minor, g_UIContext.CurrentVersion.PatchLetter);
	}
	std::string versionDisplay = " Crimson " + versionStr;

	ImVec2 displaySize = ImGui::GetIO().DisplaySize;
	// Offset from the right edge — larger than the RedOrb counter (~38) to keep distance
	float edgeOffsetX = 60.0f * scaleFactorY;
	float edgeOffsetY = 30.0f * scaleFactorY;

	ImFont* font = g_ImGuiFont_Benguiat[20.0f];
	ImGui::PushFont(font);
	ImVec2 textSize = ImGui::CalcTextSize(versionDisplay.c_str());

	// Window size and position - bottom right corner, with generous right offset
	ImVec2 windowSize = ImVec2(textSize.x + 30.0f * scaleFactorY, 100.0f * scaleFactorY);
	ImVec2 windowPos = ImVec2(displaySize.x - windowSize.x - edgeOffsetX, displaySize.y - windowSize.y - edgeOffsetY);

	ImGui::SetNextWindowSize(windowSize);
	ImGui::SetNextWindowPos(windowPos);

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;

	ImGui::Begin("MissionResultVersionWindow", nullptr, windowFlags);

	ImGui::SetWindowFontScale(scaleFactorY);

	// Apply alpha to text color
	ImU32 alphaPacked = (ImU32)(alpha * 255.0f);
	ImU32 textColor = (0xD5D9DBFF & 0x00FFFFFF) | (alphaPacked << 24);

	ImVec2 textPos = ImVec2(0.0f, (windowSize.y - textSize.y * scaleFactorY) / 2.0f);
	ImGui::SetCursorScreenPos(ImGui::GetWindowPos() + textPos);
	ImGui::TextColored(ImColor(textColor), "%s", versionDisplay.c_str());

	ImGui::End();
	ImGui::PopFont();
}

void RenderMissionResultCheatsUsed() {
	using namespace UI;
	const char* missionResultGameModeString = nullptr;
	ImU32 gameModeStringColor = 0;
	auto defaultFontSize = g_UIContext.DefaultFontSize;
	ImVec2 windowSize = ImVec2(1300.0f * scaleFactorY, 200.0f * scaleFactorY);
	ImVec2 windowPos = ImVec2(
		(g_renderSize.x - windowSize.x) * 0.5f + (1200.0f * scaleFactorY),
		scaleFactorY / 2 + (347.0f * scaleFactorY)
	);

	// We use this to identify whether or not we're in the exact Mission Result Screen
	uint32 inMissionResultScreenInt = *reinterpret_cast<uint32*>(appBaseAddr + 0x5CF9A0);
	bool inMissionResultScreen = inMissionResultScreenInt == 0x14FD80 ? true : false;
	bool shouldShow = (g_scene == SCENE::MISSION_RESULT && inMissionResultScreen);

	// Static variables to track fade state
	static bool wasShowing = false;
	static float fadeStartTime = 0.0f;
	static bool isFadingIn = false;
	static bool isFadingOut = false;

	float currentTime = ImGui::GetTime();
	const float fadeDuration = 1.0f; // 1 second fade

	// Handle fade state transitions
	if (shouldShow && !wasShowing) {
		// Start fade in
		isFadingIn = true;
		isFadingOut = false;
		fadeStartTime = currentTime;
	} else if (!shouldShow && wasShowing) {
		// Start fade out
		isFadingIn = false;
		isFadingOut = true;
		fadeStartTime = currentTime;
	}

	wasShowing = shouldShow;

	// Calculate alpha based on fade state
	float alpha = 1.0f;
	if (isFadingIn) {
		float fadeProgress = (currentTime - fadeStartTime) / fadeDuration;
		if (fadeProgress >= 1.0f) {
			alpha = 1.0f;
			isFadingIn = false;
		} else {
			alpha = fadeProgress;
		}
	} else if (isFadingOut) {
		float fadeProgress = GetFadeToBlackProgress(30.0f);
		if (fadeProgress >= 1.0f) {
			alpha = 0.0f;
			isFadingOut = false;
			return; // Completely faded out, stop rendering
		} else {
			alpha = 1.0f - fadeProgress;
		}
	} else if (!shouldShow) {
		return; // Not showing and not fading, don't render
	}

	// Helper function to apply alpha to color
	auto applyAlpha = [alpha](ImU32 color) -> ImU32 {
		ImU32 a = (ImU32)(alpha * 255.0f);
		return (color & 0x00FFFFFF) | (a << 24);
		};

	switch (gameModeData.missionResultGameMode) {
	case GAMEMODEPRESETS::VANILLA:
		missionResultGameModeString = gameModeData.names[GAMEMODEPRESETS::VANILLA].c_str();
		gameModeStringColor = applyAlpha(0xFFFFFFFF);
		break;

	case GAMEMODEPRESETS::STYLE_SWITCHER:
		missionResultGameModeString = gameModeData.names[GAMEMODEPRESETS::STYLE_SWITCHER].c_str();
		gameModeStringColor = applyAlpha(SwapColorEndianness(0xE8BA18FF));
		break;

	case GAMEMODEPRESETS::CRIMSON:
		missionResultGameModeString = gameModeData.names[GAMEMODEPRESETS::CRIMSON].c_str();
		gameModeStringColor = applyAlpha(SwapColorEndianness(0xDA1B53FF));
		break;

	case GAMEMODEPRESETS::CUSTOM:
		missionResultGameModeString = gameModeData.names[GAMEMODEPRESETS::CUSTOM].c_str();
		gameModeStringColor = applyAlpha(SwapColorEndianness(0x4050FFFF));
		break;

	case GAMEMODEPRESETS::UNRATED:
		missionResultGameModeString = gameModeData.names[GAMEMODEPRESETS::UNRATED].c_str();
		gameModeStringColor = applyAlpha(0xFFFFFFFF);
		break;

	default:
		missionResultGameModeString = "未知";
		gameModeStringColor = applyAlpha(0xFFFFFFFF);
		break;
	}

	// Apply alpha to ImGui style
	ImGui::PushStyleColor(ImGuiCol_Text, applyAlpha(0xD5D9DBFF));

	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(windowSize + ImVec2(50.0f, 50.0f), ImGuiCond_Always);

	if (ImGui::Begin("MissionResultCheatsUsed", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoBackground)) {

		ImGui::SetWindowFontScale(scaleFactorY);

		ImFont* font = UI::g_ImGuiFont_Benguiat[40.0f];
		ImGui::PushFont(font);

		if (gameModeData.missionUsedCheats.size() > 0)
			ImGui::Text("已用作弊：");

		for (auto cheat : gameModeData.missionUsedCheats) {
			ImGui::PushFont(g_ImGuiFont_Benguiat[23.0f]);
			ImGui::Text("%s", gameModeData.cheatsNames[cheat].c_str());
			ImGui::PopFont();
		}

		ImGui::PopFont();
	}

	ImGui::End();
	ImGui::PopStyleColor(); // Pop the alpha style var
}


void RenderOutOfViewIcon(PlayerActorData actorData, SimpleVec3& screen_pos, float screenMargin, const char* name, Config::BarsData& activeData) {
	auto pool_4449 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC8FBD0);
	if (!pool_4449 || !pool_4449[147]) return;
	auto& cameraData = *reinterpret_cast<CameraData*>(pool_4449[147]);

	auto pool_12857 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if (!pool_12857 || !pool_12857[3]) return;
	auto& mainActorData = *reinterpret_cast<PlayerActorDataDante*>(pool_12857[3]);

	auto playerIndex = actorData.newPlayerIndex;

	const float screenWidth = g_renderSize.x;
	const float screenHeight = g_renderSize.y;
	const float screenCenterX = screenWidth / 2.0f;
	const float screenCenterY = screenHeight / 2.0f;

	ImVec2 iconPosition;

	glm::vec3 cameraPos = { cameraData.data[0].x, cameraData.data[0].y, cameraData.data[0].z };
	glm::vec3 playerPos = { actorData.position.x, actorData.position.y, actorData.position.z };

	// Calculate the camera's forward vector based on the main actor's (1P) position
	glm::vec3 mainActorPos = { mainActorData.position.x, mainActorData.position.y, mainActorData.position.z };
	SimpleVec3 cameraForwardVec = {
		mainActorPos.x - cameraPos.x,
		mainActorPos.y - cameraPos.y,
		mainActorPos.z - cameraPos.z
};

	// Normalize the camera forward vector
	float magnitude = sqrt(cameraForwardVec.x * cameraForwardVec.x +
		cameraForwardVec.y * cameraForwardVec.y +
		cameraForwardVec.z * cameraForwardVec.z);
	cameraForwardVec.x /= magnitude;
	cameraForwardVec.y /= magnitude;
	cameraForwardVec.z /= magnitude;

	// Calculate and normalize the vector from the camera to the player
	SimpleVec3 cameraToPlayerVec = {
		playerPos.x - cameraPos.x,
		playerPos.y - cameraPos.y,
		playerPos.z - cameraPos.z
	};
	float magnitudeCameraToPlayer = sqrt(cameraToPlayerVec.x * cameraToPlayerVec.x +
		cameraToPlayerVec.y * cameraToPlayerVec.y +
		cameraToPlayerVec.z * cameraToPlayerVec.z);
	cameraToPlayerVec.x /= magnitudeCameraToPlayer;
	cameraToPlayerVec.y /= magnitudeCameraToPlayer;
	cameraToPlayerVec.z /= magnitudeCameraToPlayer;

	// Determine if the player is behind the camera
	float dotProduct = cameraToPlayerVec.x * cameraForwardVec.x +
		cameraToPlayerVec.y * cameraForwardVec.y +
		cameraToPlayerVec.z * cameraForwardVec.z;

	bool isBehindCamera = dotProduct < 0.0f;

	if (isBehindCamera) {
		float deltaX = -cameraToPlayerVec.x;
		float deltaY = -cameraToPlayerVec.y;

		// Introduce bias towards the bottom of the screen
		deltaY += screenHeight * 0.003f;

		// Determine the screen edge closest to the player's position
		float absDeltaX = fabs(deltaX);
		float absDeltaY = fabs(deltaY);

		if (absDeltaX > absDeltaY) {
			// Player is more to the left or right
			if (deltaX > 0) {
				// Right side of the screen
				iconPosition = ImVec2(screenWidth - screenMargin, screenCenterY + deltaY / absDeltaX * (screenHeight / 2.0f - screenMargin));
			}
			else {
				// Left side of the screen
				iconPosition = ImVec2(screenMargin, screenCenterY + deltaY / absDeltaX * (screenHeight / 2.0f - screenMargin));
			}
		}
		else {
			// Player is more to the top or bottom
			if (deltaY > 0) {
				// Bottom side of the screen
				iconPosition = ImVec2(screenCenterX + deltaX / absDeltaY * (screenWidth / 2.0f - screenMargin), screenHeight - screenMargin);
			}
			else {
				// Top side of the screen
				iconPosition = ImVec2(screenCenterX + deltaX / absDeltaY * (screenWidth / 2.0f - screenMargin), screenMargin);
			}
		}
	}
	else {
		// Apply a magnetic effect to pull the icon toward the edges of the screen
		float deltaX = screen_pos.x - screenCenterX;
		float deltaY = screen_pos.y - screenCenterY;
		float edgePullFactor = 0.7f;

		if (fabs(deltaX) > fabs(deltaY)) {
			// More horizontal movement
			iconPosition.x = screenCenterX + edgePullFactor * deltaX;
			iconPosition.y = screenCenterY + edgePullFactor * deltaY * (screenHeight / screenWidth);
		}
		else {
			// More vertical movement
			iconPosition.x = screenCenterX + edgePullFactor * deltaX * (screenWidth / screenHeight);
			iconPosition.y = screenCenterY + edgePullFactor * deltaY;
		}

		// Clamp icon position to screen edges
		iconPosition.x = CrimsonUtil::sexy_clamp(iconPosition.x, screenMargin, screenWidth - screenMargin);
		iconPosition.y = CrimsonUtil::sexy_clamp(iconPosition.y, screenMargin, screenHeight - screenMargin);
	}

	// Scale factor based on 1080p for responsiveness
	float baseResX = 1920.0f;
	float baseResY = 1080.0f;
	float scaleFactorX = g_renderSize.x / baseResX;
	float scaleFactorY = g_renderSize.y / baseResY;


	ImVec2 buttonSize(80.0f * scaleFactorX, 80.0f * scaleFactorY);

	// Clamp the Icon Position again to ensure Button doesn't go out of screen bounds:
	iconPosition.x = CrimsonUtil::sexy_clamp(iconPosition.x, screenMargin, screenWidth - screenMargin - buttonSize.x + 30.0f);
	iconPosition.y = CrimsonUtil::sexy_clamp(iconPosition.y, screenMargin, screenHeight - screenMargin - buttonSize.y + 30.0f);


	auto IsOverlapping = [&](ImVec2 pos1, ImVec2 pos2, ImVec2 size) {
		return !(pos1.x + size.x <= pos2.x || pos1.x >= pos2.x + size.x ||
			pos1.y + size.y <= pos2.y || pos1.y >= pos2.y + size.y);
		};

	// Check for overlap between icons and reposition if necessary
	for (int i = 0; i < crimsonHud.playerOutViewIconPositions.size(); ++i) {
		if (i == playerIndex) continue;

		const auto& previousPos = crimsonHud.playerOutViewIconPositions[i];
		if (IsOverlapping(iconPosition, previousPos, buttonSize)) {

			// Check if both icons are near the top or bottom
			if (iconPosition.y <= screenMargin || iconPosition.y >= screenHeight - screenMargin - buttonSize.y) {
				// Both icons are on top or bottom, move one to the side
				iconPosition.x += buttonSize.x + 5.0f;
				if (iconPosition.x + buttonSize.x > screenWidth - screenMargin) {
					// Wrap around if out of bounds horizontally
					iconPosition.x = screenMargin;
				}
			}
			// Check if both icons are near the left or right
			else if (iconPosition.x <= screenMargin || iconPosition.x >= screenWidth - screenMargin - buttonSize.x) {
				// Both icons are on left or right, move one up or down
				iconPosition.y += buttonSize.y + 5.0f;
				if (iconPosition.y + buttonSize.y > screenHeight - screenMargin) {
					// Wrap around if out of bounds vertically
					iconPosition.y = screenMargin;
				}
			}
		}
	}

	crimsonHud.playerOutViewIconPositions[playerIndex] = iconPosition;

	// Render the icon at the adjusted position with label and distance
	ImGuiWindowFlags windowFlags =
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;

	ImGui::SetNextWindowPos(crimsonHud.playerOutViewIconPositions[playerIndex]);
	ImGui::Begin((std::string(playerIndex + "out of view icon")).c_str(), nullptr, windowFlags);

	ImGui::InvisibleButton("##button", buttonSize);

	// Draw the button manually
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	ImVec2 buttonPos = ImGui::GetItemRectMin();
	ImVec2 buttonEndPos = ImGui::GetItemRectMax();
	ImVec4 playerColor = ConvertColorFromUint8ToVec4(activeCrimsonConfig.PlayerProperties.playerColor[playerIndex]);
	drawList->AddRectFilled(buttonPos, buttonEndPos, ImGui::GetColorU32(playerColor), 10.0f * scaleFactorX);  // Background color
	drawList->AddRect(buttonPos, buttonEndPos, ImGui::GetColorU32(ImGuiCol_Border), 10.0f * scaleFactorX);  // Border

	// Calculate luminance (perceived brightness)
	float luminance = 0.299f * playerColor.x + 0.587f * playerColor.y + 0.114f * playerColor.z;
	// Choose text color based on luminance 
	ImVec4 textColor = (luminance > 0.4f) ? ImVec4(0.1f, 0.1f, 0.1f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	float minFontSize = 10.0f; // Font cannot go below 10 or else ImGui asserts will come in.
	float fontSizeDistance = (std::max)(17.0f * scaleFactorY, minFontSize);
	float fontSizeNameLabel = (std::max)(23.0f * scaleFactorY, minFontSize);

	// Draw the distance text centered inside the button
	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[fontSizeDistance]);
	ImGui::PushStyleColor(ImGuiCol_Text, textColor);
	std::string distanceText = std::to_string(static_cast<int>(crimsonPlayer[playerIndex].playerTo1PDistance));
	ImVec2 distanceTextSize = ImGui::CalcTextSize(distanceText.c_str());
	ImVec2 distanceTextPos = ImVec2(
		buttonPos.x + (buttonSize.x - distanceTextSize.x) * 0.5f,
		buttonPos.y + 5.0f * scaleFactorY
	);
	drawList->AddText(distanceTextPos, ImGui::GetColorU32(ImGuiCol_Text), distanceText.c_str());
	ImGui::PopStyleColor();
	ImGui::PopFont();

	// Draw name label (2P, 3P, etc)
	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[fontSizeNameLabel]);
	ImGui::PushStyleColor(ImGuiCol_Text, textColor);
	ImVec2 nameTextSize = ImGui::CalcTextSize(name);
	ImVec2 nameTextPos = ImVec2(
		buttonPos.x + (buttonSize.x - nameTextSize.x) * 0.5f,
		buttonPos.y + (buttonSize.y - nameTextSize.y) * 0.5f
	);
	drawList->AddText(nameTextPos, ImGui::GetColorU32(ImGuiCol_Text), name);
	ImGui::PopStyleColor();
	ImGui::PopFont();

	// Calculate positions and sizes for progress bars
	ImVec2 progressBarSize = ImVec2(buttonSize.x, 7.0f * scaleFactorY);
	float progressBarPadding = 1.0f * scaleFactorY;  // Space between progress bars

	ImVec4 hitColor = CrimsonUtil::HexToImVec4(0x43fe65FF);
	ImVec4 magicColor = CrimsonUtil::HexToImVec4(0xde1c4cFF);
	ImVec4 magicColorVergil = { 0.06f, 0.74f, 0.81f, 1.0f };

	// HP Bar
	ImGui::SetCursorScreenPos(ImVec2(buttonPos.x, buttonPos.y + (buttonSize.y * 0.75f)));
	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, *reinterpret_cast<ImVec4*>(&hitColor));
	ImGui::ProgressBar(actorData.hitPoints / actorData.maxHitPoints, progressBarSize, "");
	ImGui::PopStyleColor();

	// DT Bar
	ImGui::SetCursorScreenPos(ImVec2(buttonPos.x, buttonPos.y + (buttonSize.y * 0.75f) + progressBarSize.y + progressBarPadding));
	auto& magicColorChosen = actorData.character != CHARACTER::VERGIL ? magicColor : magicColorVergil;
	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, magicColorChosen);
	ImGui::ProgressBar(actorData.magicPoints / actorData.maxMagicPoints, progressBarSize, "");
	ImGui::PopStyleColor();

	ImGui::End();
}

void RenderMultiplayerBar(
	float hitPoints, float magicPoints, const char* name, PlayerActorData& actorData) {
	if (!showBars && !activeCrimsonConfig.MultiplayerBars2D.show) {
		return;
	}

	auto playerIndex = actorData.newPlayerIndex;

	if (playerIndex == 0) {
		return;
	}
	const float alpha = ImLerp(0.27f, 1.0f, 1.0);
	ImVec4 hitColor = CrimsonUtil::HexToImVec4(0x43fe65FF);
	ImVec4 magicColor[4] = { CrimsonUtil::HexToImVec4(0xde1c4cFF),CrimsonUtil::HexToImVec4(0xde1c4cFF),CrimsonUtil::HexToImVec4(0xde1c4cFF),CrimsonUtil::HexToImVec4(0xde1c4cFF) };
	float magicColorVergil[4] = { 0.06f, 0.74f, 0.81f, 1.0f };

	const float baseSpacing = 0.37f;
	const float barSpacing = g_renderSize.x * baseSpacing; // 37% of screen width per bar
	const float baseX = g_renderSize.x * (0.02f - baseSpacing); // Corner of the screen
	const float baseY = g_renderSize.y * 0.8962f; // Bottom of the screen
	

	ImVec2 pos = ImVec2(
		baseX + barSpacing * (playerIndex),
		//baseY - barSpacing * (activeConfig.Actor.playerCount - playerIndex) // Inverted
		baseY
	);

	ImGui::SetNextWindowPos(pos);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 0));

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));


	ImGuiWindowFlags windowFlags =
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;

	std::string label = "2DMPBar " + std::to_string(playerIndex + 1);


	if (ImGui::Begin(label.c_str(), &activeCrimsonConfig.MultiplayerBarsWorldSpace.show, windowFlags)) {
		ImGui::SetWindowFontScale(scaleFactorY);

		ImVec4 playerColor = ConvertColorFromUint8ToVec4(activeCrimsonConfig.PlayerProperties.playerColor[playerIndex]);
		float luminance = 0.299f * playerColor.x + 0.587f * playerColor.y + 0.114f * playerColor.z;
		ImVec4 textColor = (luminance > 0.4f) ? ImVec4(0.1f, 0.1f, 0.1f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		
		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[18.0f]);
		ImGui::Text("  ");
		ImGui::SameLine();
		//grabbing mission data real quick
		auto name_6975 = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E30);
		if (!name_6975) {
			return;
		}
		auto& missionData = *reinterpret_cast<MissionData*>(name_6975);
		//ImGui::SameLine();
		if (actorData.dead && missionData.itemCounts[ITEM::GOLD_ORB] > 0) {
			ImGui::Text("长按开始键复活");
			ImGui::Text("  ");
			ImGui::SameLine();
			ImGui::Text("剩余金魂：%u", missionData.itemCounts[ITEM::GOLD_ORB]);
		}else
		if (actorData.dead) {
			ImGui::Text("剩余金魂：%u", missionData.itemCounts[ITEM::GOLD_ORB]);
		}
		else
		if (actorData.character == CHARACTER::DANTE || actorData.character == CHARACTER::VERGIL) {

			if (actorData.character == CHARACTER::DANTE) {
				ImGui::Text(styleNamesDanteGameplay[actorData.style]);
			} else {
				ImGui::Text(styleNamesVergilGameplay[actorData.style]);
			}
		}
		ImGui::PopFont();

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, alpha));
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, *reinterpret_cast<ImVec4*>(&hitColor));
		ImGui::ProgressBar(hitPoints, ImVec2(200 * scaleFactorY, 10 * scaleFactorY), "");
		ImGui::PopStyleColor(2);

		if (actorData.character != CHARACTER::VERGIL) {
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, *reinterpret_cast<ImVec4*>(&magicColor));
		} else {
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, *reinterpret_cast<ImVec4*>(&magicColorVergil));
		}
		ImGui::ProgressBar(magicPoints, ImVec2(200 * scaleFactorY, 10 * scaleFactorY), "");
		ImGui::PopStyleColor();

		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[18.0 * 1.1f]);
		ImGui::PushStyleColor(ImGuiCol_Button, playerColor);
		ImGui::PushStyleColor(ImGuiCol_Text, textColor);
		ImGui::Button(name, { 30.0f * scaleFactorY, 30.0f * scaleFactorY});
		ImGui::PopStyleColor(2);
		ImGui::PopFont();
		ImGui::SameLine(0.0f, 0);
		ImGui::Text("  ");

		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[22.0f]);
		ImGui::SameLine();
		ImGui::Text(activeCrimsonConfig.PlayerProperties.playerName[playerIndex].c_str());
		// 		ImGui::SameLine();
		// 		ImGui::Text(" -");
		ImGui::PopFont();
	}

	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(4);
}

void Render1PAttributes(const char* name, PlayerActorData& actorData) {
	if (!showBars && !activeCrimsonConfig.MultiplayerBars2D.show) {
		return;
	}

	auto playerIndex = actorData.newPlayerIndex;

	if (playerIndex != 0) {
		return;
	}

	if ((activeCrimsonConfig.MultiplayerBars2D.show1PAttributes == HUDELEMENTSHOWSTATE::ONLY_IN_MP
		&& activeConfig.Actor.playerCount == 1) ||
		(activeCrimsonConfig.MultiplayerBars2D.show1PAttributes == HUDELEMENTSHOWSTATE::OFF)) {
		return;
	}

	float baseX = g_renderSize.x * (0.3272f); 
	float baseY = g_renderSize.y * 0.015f; 

	ImVec2 pos = ImVec2(baseX,baseY);

	ImGui::SetNextWindowPos(pos);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 0));

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));


	ImGuiWindowFlags windowFlags =
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;

	std::string label = "1PAttributes " + std::to_string(playerIndex + 1);


	if (ImGui::Begin(label.c_str(), &activeCrimsonConfig.MultiplayerBarsWorldSpace.show, windowFlags)) {
		ImGui::SetWindowFontScale(scaleFactorY);

		ImVec4 playerColor = ConvertColorFromUint8ToVec4(activeCrimsonConfig.PlayerProperties.playerColor[playerIndex]);
		float luminance = 0.299f * playerColor.x + 0.587f * playerColor.y + 0.114f * playerColor.z;
		ImVec4 textColor = (luminance > 0.4f) ? ImVec4(0.1f, 0.1f, 0.1f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[18.0 * 1.1f]);
		ImGui::PushStyleColor(ImGuiCol_Button, playerColor);
		ImGui::PushStyleColor(ImGuiCol_Text, textColor);
		ImGui::Button(name, { 30.0f * scaleFactorY, 30.0f * scaleFactorY });
		ImGui::PopStyleColor(2);
		ImGui::PopFont();
		ImGui::SameLine(0.0f, 0);
		ImGui::Text("  ");

		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[22.0f]);
		ImGui::SameLine();
		ImGui::Text(activeCrimsonConfig.PlayerProperties.playerName[playerIndex].c_str());
		// 		ImGui::SameLine();
		// 		ImGui::Text(" -");
		ImGui::PopFont();
	}

	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(4);
}


void RenderWorldSpaceMultiplayerBar(
	float hitPoints, float magicPoints, const char* name, const PlayerActorData& actorData, const char* label, Config::BarsData& activeData/*, Config::BarsData& queuedData*/) {
	if (!showBars && !activeCrimsonConfig.MultiplayerBarsWorldSpace.show) {
		return;
	}

	auto playerIndex = actorData.newPlayerIndex;
	auto distanceClamped = crimsonPlayer[playerIndex].cameraPlayerDistanceClamped;

	// Adjusts size dynamically based on the distance between Camera and Player
	ImVec2 sizeDistance = { (activeData.size.x * (1.0f / ((float)distanceClamped / 20))) * scaleFactorY, (activeData.size.y * (1.0f / ((float)distanceClamped / 20))) * scaleFactorY };
	auto& playerScreenPosition = crimsonPlayer[playerIndex].playerScreenPosition;
	float screenWidth = g_renderSize.x;
	float screenHeight = g_renderSize.y;

	const float screenMargin = 50.0f;

	const float t = CrimsonUtil::smoothstep(0.0f, 1390.0f, crimsonPlayer[playerIndex].cameraPlayerDistance);
	const float alpha = ImLerp(0.27f, 1.0f, t);
	ImVec4 hitColor = CrimsonUtil::HexToImVec4(0x43fe65FF);
	ImVec4 magicColor = CrimsonUtil::HexToImVec4(0xde1c4cFF);
	ImVec4 magicColorVergil= { 0.06f, 0.74f, 0.81f, 1.0f };
	hitColor.z = alpha;
	magicColor.z = alpha;
	magicColorVergil.z = alpha;

	// If player is outside view: Handle Out of View Icons and stop rendering WorldSpace Bars
	if (playerScreenPosition.x < screenMargin || playerScreenPosition.x > screenWidth - screenMargin ||
		playerScreenPosition.y < screenMargin || playerScreenPosition.y > screenHeight - screenMargin) {


		if (activeCrimsonConfig.MultiplayerBarsWorldSpace.showOutOfViewIcons) {
			RenderOutOfViewIcon(actorData, playerScreenPosition, screenMargin, name, activeData);
		}

		return;
	}

	ImGui::SetNextWindowPos(ImVec2(playerScreenPosition.x - (sizeDistance.x / 2.0f), playerScreenPosition.y - sizeDistance.y));

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 0));

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));


	ImGuiWindowFlags windowFlags =
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;


	if (ImGui::Begin(label, &activeCrimsonConfig.MultiplayerBarsWorldSpace.show, windowFlags)) {
		ImGui::SetWindowFontScale(scaleFactorY);
		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[18.0 * 1.1f]);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, alpha));
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, *reinterpret_cast<ImVec4*>(&hitColor));
		ImGui::ProgressBar(hitPoints, sizeDistance, "");
		ImGui::PopStyleColor(2);

		if (actorData.character != CHARACTER::VERGIL) {
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, *reinterpret_cast<ImVec4*>(&magicColor));
		} else {
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, *reinterpret_cast<ImVec4*>(&magicColorVergil));
		}
		ImGui::ProgressBar(magicPoints, sizeDistance, "");
		ImGui::PopStyleColor();

		ImVec4 playerColor = ConvertColorFromUint8ToVec4(activeCrimsonConfig.PlayerProperties.playerColor[playerIndex]);

		float luminance = 0.299f * playerColor.x + 0.587f * playerColor.y + 0.114f * playerColor.z;
		ImVec4 textColor = (luminance > 0.4f) ? ImVec4(0.1f, 0.1f, 0.1f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

		ImGui::PushStyleColor(ImGuiCol_Button, playerColor);
		ImGui::PushStyleColor(ImGuiCol_Text, textColor);
		ImGui::Button(name, { 30.0f * scaleFactorY, 30.0f * scaleFactorY});
		ImGui::PopStyleColor(2);
		ImGui::PopFont();

		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[18.0f]);
		ImGui::SameLine();
		if (actorData.character == CHARACTER::DANTE || actorData.character == CHARACTER::VERGIL) {
			ImGui::Text("  ");
			ImGui::SameLine(0.0f, 0);

			if (actorData.character == CHARACTER::DANTE) {
				ImGui::Text(styleNamesDanteGameplay[actorData.style]);
			} else {
				ImGui::Text(styleNamesVergilGameplay[actorData.style]);
			}
		}
		ImGui::PopFont();

		if (activeCrimsonConfig.MultiplayerBarsWorldSpace.showPlayerNames) {
			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[11.0f]);
			ImGui::Text(activeCrimsonConfig.PlayerProperties.playerName[playerIndex].c_str());
			ImGui::PopFont();
		}

	}

	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(4);
}

void MultiplayerBars(IDXGISwapChain* pSwapChain) {
	if (!showBars && !(activeConfig.Actor.enable && InGame())) {
		return;
	}

	if (InCutscene() || InCredits() || !activeConfig.Actor.enable || g_inGameCutscene ) {
		return;
	}

	uint8 playerCount = (showBars) ? PLAYER_COUNT : activeConfig.Actor.playerCount;

	int minimum = 1;



	old_for_all(uint8, playerIndex, playerCount) {

		if (activeCrimsonConfig.MultiplayerBarsWorldSpace.show1PBar == HUDELEMENTSHOWSTATE::OFF) {
			minimum = 1;
		}
		else if ((activeCrimsonConfig.MultiplayerBarsWorldSpace.show1PBar == HUDELEMENTSHOWSTATE::ONLY_IN_MP
			&& activeConfig.Actor.playerCount > 1) ||
			(activeCrimsonConfig.MultiplayerBarsWorldSpace.show1PBar == HUDELEMENTSHOWSTATE::ALWAYS)) {
			minimum = 0;
		}

		if (playerIndex >= minimum) {


			float hit = 0.75f;
			float magic = 0.5f;

			[&]() {
				auto& playerData = GetPlayerData(playerIndex);

				auto& characterData = GetCharacterData(playerIndex, playerData.characterIndex, ENTITY::MAIN);
				auto& newActorData = GetNewActorData(playerIndex, playerData.characterIndex, ENTITY::MAIN);

				auto& activeCharacterData = GetCharacterData(playerIndex, playerData.activeCharacterIndex, ENTITY::MAIN);
				auto& activeNewActorData = GetNewActorData(playerIndex, playerData.activeCharacterIndex, ENTITY::MAIN);

				auto& leadCharacterData = GetCharacterData(playerIndex, 0, ENTITY::MAIN);
				auto& leadNewActorData = GetNewActorData(playerIndex, 0, ENTITY::MAIN);

				auto& mainCharacterData = GetCharacterData(playerIndex, playerData.characterIndex, ENTITY::MAIN);
				auto& mainNewActorData = GetNewActorData(playerIndex, playerData.characterIndex, ENTITY::MAIN);


				if (activeCharacterData.character >= CHARACTER::MAX) {
					hit = 1.0f;
					magic = 1.0f;

					return;
				}

				if (!activeNewActorData.baseAddr) {
					return;
				}
				auto& activeActorData = *reinterpret_cast<PlayerActorData*>(activeNewActorData.baseAddr);
				auto playerIndex = activeActorData.newPlayerIndex;

				hit = (activeActorData.hitPoints / activeActorData.maxHitPoints);
				magic = (activeActorData.magicPoints / activeActorData.maxMagicPoints);

				//                 activeConfig.barsData[playerIndex].pos.x = queuedConfig.barsData[playerIndex].pos.x;
				//                 activeConfig.barsData[playerIndex].pos.y = queuedConfig.barsData[playerIndex].pos.y;

				RenderWorldSpaceMultiplayerBar(hit, magic, playerIndexNames[playerIndex], 
					activeActorData, barsNames[playerIndex], activeConfig.barsData[playerIndex]);

				//WorldSpaceWeaponWheelsController2P(activeActorData, pSwapChain);
				}();
		}

		float hit = 0.75f;
		float magic = 0.5f;

		auto& playerData = GetPlayerData(playerIndex);

		auto& characterData = GetCharacterData(playerIndex, playerData.characterIndex, ENTITY::MAIN);
		auto& newActorData = GetNewActorData(playerIndex, playerData.characterIndex, ENTITY::MAIN);

		auto& activeCharacterData = GetCharacterData(playerIndex, playerData.activeCharacterIndex, ENTITY::MAIN);
		auto& activeNewActorData = GetNewActorData(playerIndex, playerData.activeCharacterIndex, ENTITY::MAIN);

		if (!activeNewActorData.baseAddr) {
			return;
		}
		auto& activeActorData = *reinterpret_cast<PlayerActorData*>(activeNewActorData.baseAddr);

		hit = (activeActorData.hitPoints / activeActorData.maxHitPoints);
		magic = (activeActorData.magicPoints / activeActorData.maxMagicPoints);
		RenderMultiplayerBar(hit, magic, playerIndexNames[playerIndex],
			activeActorData);

		Render1PAttributes(playerIndexNames[0], activeActorData);
	}
}

void BarsSettings(size_t defaultFontSize) {

	ImGui::Text("");

	const float columnWidth = 0.5f * queuedConfig.globalScale;
	const float rowWidth = 20.0f * queuedConfig.globalScale;
	const float rowWidth2 = 0.5f * queuedConfig.globalScale;


	auto& activePos1P = *reinterpret_cast<ImVec2*>(&activeConfig.barsData[0].pos);
	auto& queuedPos1P = *reinterpret_cast<ImVec2*>(&queuedConfig.barsData[0].pos);

	auto& activePos2P = *reinterpret_cast<ImVec2*>(&activeConfig.barsData[1].pos);
	auto& queuedPos2P = *reinterpret_cast<ImVec2*>(&queuedConfig.barsData[1].pos);

	auto& activePos3P = *reinterpret_cast<ImVec2*>(&activeConfig.barsData[2].pos);
	auto& queuedPos3P = *reinterpret_cast<ImVec2*>(&queuedConfig.barsData[2].pos);

	auto& activePos4P = *reinterpret_cast<ImVec2*>(&activeConfig.barsData[3].pos);
	auto& queuedPos4P = *reinterpret_cast<ImVec2*>(&queuedConfig.barsData[3].pos);

	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);

	if (ImGui::BeginTable("BarsSettingsTable", 2)) {

		ImGui::PushItemWidth(itemWidth);

		ImGui::TableSetupColumn("c1", 0, columnWidth);
		ImGui::TableNextRow(0, rowWidth);
		//ImGui::TableNextColumn();

		for (int i = 0; i < 4; i++) {
			if (i == 2) {
				ImGui::TableNextRow(0, rowWidth);
				ImGui::TableNextRow(0, rowWidth2);
			}
			ImGui::TableNextColumn();


			std::string title = std::to_string(i + 1) + "P 位置";
			ImGui::Text(title.c_str());

			auto& activePos = *reinterpret_cast<ImVec2*>(&activeConfig.barsData[i].pos);
			auto& queuedPos = *reinterpret_cast<ImVec2*>(&queuedConfig.barsData[i].pos);
			auto& defaultPos = *reinterpret_cast<ImVec2*>(&defaultConfig.barsData[i].pos);

			ImGui::PushItemWidth(itemWidth);
			if (GUI_Input2("X", activePos.x, queuedPos.x, 10.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue)) {

				ImGui::SetWindowPos(barsNames[i], activePos);
			}
			ImGui::SameLine();
			if (GUI_Input2("Y", activePos.y, queuedPos.y, 10.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue)) {
				ImGui::SetWindowPos(barsNames[i], activePos);
			}
			ImGui::SameLine();
			if (GUI_Button("D")) {
				CopyMemory(&activePos, &defaultPos, sizeof(activePos));
				CopyMemory(&queuedPos, &defaultPos, sizeof(queuedPos));

				ImGui::SetWindowPos(barsNames[i], activePos);

			}
			ImGui::PopItemWidth();

		}



		ImGui::EndTable();

	}

	ImGui::PopFont();


	//     for_all(playerIndex, PLAYER_COUNT) {
	//         ImGui::Text("");
	// 
	//         GUI_SectionStart(playerIndexNames[playerIndex]);
	// 
	//        
	// 
	//         BarsSettingsFunction(barsNames[playerIndex], activeConfig.barsData[playerIndex], queuedConfig.barsData[playerIndex],
	//             defaultConfig.barsData[playerIndex]);
	//     }
}

void BarsSection(size_t defaultFontSize) {

	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);

	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;


		if (ImGui::BeginTable("MultiplayerBarsOptionsTable", 2)) {

			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			ImGui::TableNextRow(0, rowWidth * 0.5f);
			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			GUI_TitleCheckbox2("2D MULTIPLAYER BARS", activeCrimsonConfig.MultiplayerBars2D.show, 
				queuedCrimsonConfig.MultiplayerBars2D.show, true, false, false,
				"", defaultFontSize * 46.70f);
			
			ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);


			ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);

			ImGui::PushItemWidth(itemWidth);
			UI::ComboMapValue2("显示 1P 属性", hudElementShowStateNames, hudElementShowStateMap,
				activeCrimsonConfig.MultiplayerBars2D.show1PAttributes,
				queuedCrimsonConfig.MultiplayerBars2D.show1PAttributes);
			ImGui::PopItemWidth();

			ImGui::PopStyleColor();
			ImGui::PopFont();


			ImGui::TableNextColumn();

			GUI_TitleCheckbox2("世界空间多人血条", activeCrimsonConfig.MultiplayerBarsWorldSpace.show,
				queuedCrimsonConfig.MultiplayerBarsWorldSpace.show, true);

			ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

			ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);

			ImGui::PushItemWidth(itemWidth);
			UI::ComboMapValue2("显示 1P 血条", hudElementShowStateNames, hudElementShowStateMap,
				activeCrimsonConfig.MultiplayerBarsWorldSpace.show1PBar,
				queuedCrimsonConfig.MultiplayerBarsWorldSpace.show1PBar);
			ImGui::PopItemWidth();

			GUI_Checkbox2("视野外图标",
				activeCrimsonConfig.MultiplayerBarsWorldSpace.showOutOfViewIcons,
				queuedCrimsonConfig.MultiplayerBarsWorldSpace.showOutOfViewIcons);
			GUI_Checkbox2("显示玩家名",
				activeCrimsonConfig.MultiplayerBarsWorldSpace.showPlayerNames,
				queuedCrimsonConfig.MultiplayerBarsWorldSpace.showPlayerNames);

			ImGui::PopFont();

			ImGui::PopStyleColor();
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::EndTable();
		}
	}
	//BarsSettings(defaultFontSize);


}

#pragma endregion

#pragma region Boss Rush

void BossRushSection(size_t defaultFontSize) {

	const float columnWidth = 0.5f * queuedConfig.globalScale;
	const float rowWidth = 30.0f * queuedConfig.globalScale;
	const float rowWidth2 = 40.0f * queuedConfig.globalScale;
	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);

	GUI_Checkbox2("Boss 连战", activeConfig.BossRush.enable, queuedConfig.BossRush.enable);

	ImGui::PopFont();

	UI::SeparatorEx(defaultFontSize * 23.35f);


	ImGui::PushItemWidth(itemWidth);

	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

	ImGui::Text("");

	// 	if (GUI_ResetButton()) {
	// 		CopyMemory(&queuedConfig.BossRush, &defaultConfig.BossRush, sizeof(queuedConfig.BossRush));
	// 		CopyMemory(&activeConfig.BossRush, &queuedConfig.BossRush, sizeof(activeConfig.BossRush));
	// 	}

	if (ImGui::BeginTable("BossRushTable", 2)) {

		ImGui::TableSetupColumn("c1", 0, columnWidth);
		ImGui::TableNextRow(0, rowWidth);
		ImGui::TableNextColumn();

		ImGui::Text("任务 5");

		ImGui::TableNextColumn();
		ImGui::Text("任务 12");

		ImGui::TableNextRow(0, rowWidth);
		ImGui::TableNextColumn();

		GUI_Checkbox2("跳过小丑", activeConfig.BossRush.Mission5.skipJester, queuedConfig.BossRush.Mission5.skipJester);

		ImGui::TableNextColumn();
		GUI_Checkbox2("跳过小丑", activeConfig.BossRush.Mission12.skipJester, queuedConfig.BossRush.Mission12.skipJester);

		ImGui::TableNextRow(0, rowWidth2);
		ImGui::TableNextColumn();

		ImGui::TableNextColumn();
		GUI_Checkbox2(
			"跳过格里昂 第 1 阶段", activeConfig.BossRush.Mission12.skipGeryonPart1, queuedConfig.BossRush.Mission12.skipGeryonPart1);

		ImGui::TableNextRow(0, rowWidth);
		ImGui::TableNextColumn();

		ImGui::Text("任务 17");

		ImGui::TableNextColumn();
		ImGui::Text("任务 19");

		ImGui::TableNextRow(0, rowWidth);
		ImGui::TableNextColumn();

		GUI_Checkbox2("跳过小丑", activeConfig.BossRush.Mission17.skipJester, queuedConfig.BossRush.Mission17.skipJester);

		ImGui::TableNextColumn();

		GUI_Checkbox2(
			"跳过阿卡姆 第 1 阶段", activeConfig.BossRush.Mission19.skipArkhamPart1, queuedConfig.BossRush.Mission19.skipArkhamPart1);

		GUI_Checkbox2(
			"跳过阿卡姆 第 2 阶段", activeConfig.BossRush.Mission19.skipArkhamPart2, queuedConfig.BossRush.Mission19.skipArkhamPart2);

		ImGui::EndTable();

	}

	ImGui::PopFont();
	ImGui::PopItemWidth();
	ImGui::PopStyleColor();

}

#pragma endregion

#pragma region Camera

const char* cameraAutoAdjustNames[] = {
	"默认",
	"手动",
	"禁用",
};

void CameraSection(size_t defaultFontSize) {
	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);

	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

	ImGui::PushItemWidth(itemWidth);

	float smallerComboMult = 0.7f;

	{
		GUI_Title("通用镜头选项");
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale * 0.5f;

		if (ImGui::BeginTable("CameraOptionsTable", 3)) {

			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * 0.7f);
			GUI_InputDefault2("视野倍率", activeCrimsonConfig.Camera.fovMultiplier, queuedCrimsonConfig.Camera.fovMultiplier, defaultCrimsonConfig.Camera.fovMultiplier, 0.1f, "%g",
				ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * 1.1f);
			UI::Combo2("灵敏度", cameraSensitivityNames, activeCrimsonConfig.Camera.sensitivity, queuedCrimsonConfig.Camera.sensitivity);
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * 1.1f);
			UI::Combo2("跟随速度", cameraFollowUpSpeedNames, activeCrimsonConfig.Camera.followUpSpd, queuedCrimsonConfig.Camera.followUpSpd);
			ImGui::PopItemWidth();

			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			GUI_PushDisable(activeCrimsonConfig.Camera.panoramicCam);
			ImGui::PushItemWidth(itemWidth * 1.1f);
			UI::Combo2("距离", cameraDistanceNames, activeCrimsonConfig.Camera.distance, queuedCrimsonConfig.Camera.distance);
			ImGui::SameLine();
			TooltipHelper("(?)", "Camera Distance relative to the player outside Lock-On.\n"
				"\n"
				"动态选项会根据玩家是否滞空自动调整。");
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * 1.1f);
			UI::Combo2(
				"锁定距离", cameraLockOnDistanceNames, activeCrimsonConfig.Camera.lockOnDistance, queuedCrimsonConfig.Camera.lockOnDistance);
			ImGui::SameLine();
			TooltipHelper("(?)", "Camera Distance relative to the player in Lock-On.\n"
				"\n"
				"动态选项会根据玩家是否滞空自动调整。");
			ImGui::PopItemWidth();
			GUI_PopDisable(activeCrimsonConfig.Camera.panoramicCam);

			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * 1.1f);
			UI::Combo2("垂直俯仰", cameraTiltNames, activeCrimsonConfig.Camera.verticalTilt, queuedCrimsonConfig.Camera.verticalTilt);
			ImGui::PopItemWidth();

			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			UI::Combo2<uint8>("Auto-Adjust", cameraAutoAdjustNames, activeCrimsonConfig.Camera.autoAdjust, queuedCrimsonConfig.Camera.autoAdjust);
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			GUI_Checkbox2("解除锁定镜头", activeCrimsonConfig.Camera.lockedOff, queuedCrimsonConfig.Camera.lockedOff);
			ImGui::SameLine();
			TooltipHelper("(?)", "在第三人称镜头模式下可用右摇杆自由旋转镜头。");

			ImGui::TableNextColumn();

			if (GUI_Checkbox2("反转 X 轴", activeCrimsonConfig.Camera.invertX, queuedCrimsonConfig.Camera.invertX)) {
				Camera::ToggleInvertX(activeCrimsonConfig.Camera.invertX);
			}

			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * 0.87f);
			UI::ComboMapValue2("镜头回中", rightStickCenterCamNames, rightStickCenterCamMap,
				activeCrimsonConfig.Camera.rightStickCameraCentering, queuedCrimsonConfig.Camera.rightStickCameraCentering);
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * 1.27f);
			UI::ComboMapValue2("镜头震动", cameraShakeNames, cameraShakeMap,
				activeCrimsonConfig.Camera.cameraShake, queuedCrimsonConfig.Camera.cameraShake);
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			if (GUI_Checkbox2("禁用 Boss 镜头", activeCrimsonConfig.Camera.disableBossCamera, queuedCrimsonConfig.Camera.disableBossCamera)) {
				Camera::ToggleDisableBossCamera(activeCrimsonConfig.Camera.disableBossCamera);
			}

			ImGui::EndTable();
		}

		ImGui::Text("");

		GUI_Title("高级镜头选项");

		if (ImGui::BeginTable("AdvancedCameraOptionsTable", 3)) {

			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			GUI_PushDisable(activeConfig.Actor.playerCount > 1);
			GUI_Checkbox2("第三人称镜头", activeCrimsonConfig.Camera.thirdPersonCamera, queuedCrimsonConfig.Camera.thirdPersonCamera);
			ImGui::SameLine();
			TooltipHelper("(?)", "Replaces (almost) every Fixed Camera with the Third Person Camera, switching to Fixed Angles when apropriate.\n"
				"多人模式下始终启用。");
			GUI_PopDisable(activeConfig.Actor.playerCount > 1);

			ImGui::TableNextColumn();

			if (GUI_Checkbox2("全景镜头", activeCrimsonConfig.Camera.panoramicCam, queuedCrimsonConfig.Camera.panoramicCam)) {
				if (activeCrimsonConfig.Camera.multiplayerCamera) {
					activeCrimsonConfig.Camera.distance = 2;
					queuedCrimsonConfig.Camera.distance = 2;

					activeCrimsonConfig.Camera.lockOnDistance = 2;
					queuedCrimsonConfig.Camera.lockOnDistance = 2;
				}
			}
			ImGui::SameLine();
			TooltipHelper("(?)", "动态镜头模式，提升战斗可读性，并根据敌人位置自动调整距离。");

			ImGui::EndTable();
		}

		ImGui::Text("");

		GUI_Title("镜头预设");

		if (GUI_Button("原版")) {
			activeCrimsonConfig.Camera.distance = 0;
			queuedCrimsonConfig.Camera.distance = 0;

			activeCrimsonConfig.Camera.lockOnDistance = 0;
			queuedCrimsonConfig.Camera.lockOnDistance = 0;

			activeCrimsonConfig.Camera.verticalTilt = 0;
			queuedCrimsonConfig.Camera.verticalTilt = 0;

			activeCrimsonConfig.Camera.fovMultiplier = 1.0f;
			queuedCrimsonConfig.Camera.fovMultiplier = 1.0f;

			activeCrimsonConfig.Camera.sensitivity = 2;
			queuedCrimsonConfig.Camera.sensitivity = 2;

			activeCrimsonConfig.Camera.invertX = true;
			queuedCrimsonConfig.Camera.invertX = true;

			activeCrimsonConfig.Camera.lockedOff = true;
			queuedCrimsonConfig.Camera.lockedOff = true;

			activeCrimsonConfig.Camera.rightStickCameraCentering = 2;
			queuedCrimsonConfig.Camera.rightStickCameraCentering = 2;

			activeCrimsonConfig.Camera.followUpSpd = 0;
			queuedCrimsonConfig.Camera.followUpSpd = 0;

			activeCrimsonConfig.Camera.autoAdjust = 0;
			queuedCrimsonConfig.Camera.autoAdjust = 0;

			if (activeConfig.Actor.playerCount <= 1) {
				activeCrimsonConfig.Camera.thirdPersonCamera = false;
				queuedCrimsonConfig.Camera.thirdPersonCamera = false;
			}

			activeCrimsonConfig.Camera.panoramicCam = false;
			queuedCrimsonConfig.Camera.panoramicCam = false;

			activeCrimsonConfig.Camera.multiplayerCamera = true;
			queuedCrimsonConfig.Camera.multiplayerCamera = true;

			activeCrimsonConfig.Camera.disableBossCamera = false;
			queuedCrimsonConfig.Camera.disableBossCamera = false;
		}

		ImGui::SameLine();

		if (GUI_Button("原版 + 第三人称")) {
			activeCrimsonConfig.Camera.distance = 0;
			queuedCrimsonConfig.Camera.distance = 0;

			activeCrimsonConfig.Camera.lockOnDistance = 0;
			queuedCrimsonConfig.Camera.lockOnDistance = 0;

			activeCrimsonConfig.Camera.verticalTilt = 0;
			queuedCrimsonConfig.Camera.verticalTilt = 0;

			activeCrimsonConfig.Camera.fovMultiplier = 1.0f;
			queuedCrimsonConfig.Camera.fovMultiplier = 1.0f;

			activeCrimsonConfig.Camera.sensitivity = 2;
			queuedCrimsonConfig.Camera.sensitivity = 2;

			activeCrimsonConfig.Camera.invertX = true;
			queuedCrimsonConfig.Camera.invertX = true;

			activeCrimsonConfig.Camera.lockedOff = true;
			queuedCrimsonConfig.Camera.lockedOff = true;

			activeCrimsonConfig.Camera.rightStickCameraCentering = 1;
			queuedCrimsonConfig.Camera.rightStickCameraCentering = 1;

			activeCrimsonConfig.Camera.followUpSpd = 0;
			queuedCrimsonConfig.Camera.followUpSpd = 0;

			activeCrimsonConfig.Camera.autoAdjust = 0;
			queuedCrimsonConfig.Camera.autoAdjust = 0;

			
			activeCrimsonConfig.Camera.thirdPersonCamera = true;
			queuedCrimsonConfig.Camera.thirdPersonCamera = true;
			

			activeCrimsonConfig.Camera.panoramicCam = false;
			queuedCrimsonConfig.Camera.panoramicCam = false;

			activeCrimsonConfig.Camera.multiplayerCamera = true;
			queuedCrimsonConfig.Camera.multiplayerCamera = true;

			activeCrimsonConfig.Camera.disableBossCamera = false;
			queuedCrimsonConfig.Camera.disableBossCamera = false;
		}

		ImGui::SameLine();

		if (GUI_Button("Crimson")) {
			activeCrimsonConfig.Camera.distance = 2;
			queuedCrimsonConfig.Camera.distance = 2;

			activeCrimsonConfig.Camera.lockOnDistance = 2;
			queuedCrimsonConfig.Camera.lockOnDistance = 2;

			activeCrimsonConfig.Camera.verticalTilt = 0;
			queuedCrimsonConfig.Camera.verticalTilt = 0;

			activeCrimsonConfig.Camera.fovMultiplier = 1.2f;
			queuedCrimsonConfig.Camera.fovMultiplier = 1.2f;

			activeCrimsonConfig.Camera.sensitivity = 2;
			queuedCrimsonConfig.Camera.sensitivity = 2;

			activeCrimsonConfig.Camera.invertX = true;
			queuedCrimsonConfig.Camera.invertX = true;

			activeCrimsonConfig.Camera.lockedOff = true;
			queuedCrimsonConfig.Camera.lockedOff = true;

			activeCrimsonConfig.Camera.rightStickCameraCentering = 1;
			queuedCrimsonConfig.Camera.rightStickCameraCentering = 1;

			activeCrimsonConfig.Camera.followUpSpd = 2;
			queuedCrimsonConfig.Camera.followUpSpd = 2;

			activeCrimsonConfig.Camera.autoAdjust = 0;
			queuedCrimsonConfig.Camera.autoAdjust = 0;

			activeCrimsonConfig.Camera.thirdPersonCamera = true;
			queuedCrimsonConfig.Camera.thirdPersonCamera = true;

			activeCrimsonConfig.Camera.panoramicCam = true;
			queuedCrimsonConfig.Camera.panoramicCam = true;

			activeCrimsonConfig.Camera.multiplayerCamera = true;
			queuedCrimsonConfig.Camera.multiplayerCamera = true;

			activeCrimsonConfig.Camera.disableBossCamera = false;
			queuedCrimsonConfig.Camera.disableBossCamera = false;
		}
	}

	ImGui::Text("");

	if (activeCrimsonGameplay.Debug.debugTools) {
		auto pool_4449 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC8FBD0);
		if (!pool_4449 || !pool_4449[147]) {
			return;
		}
		auto& cameraData = *reinterpret_cast<CameraData*>(pool_4449[147]);

		GUI_Title("实时镜头数据");

		ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);

		if (g_scene == SCENE::GAME) {
			auto pool_4449 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC8FBD0);
			if (pool_4449 && pool_4449[147]) {
				auto& cameraData = *reinterpret_cast<CameraData*>(pool_4449[147]);

				ImGui::PushItemWidth(itemWidth * 0.8f);

				const float columnWidth = 0.5f * queuedConfig.globalScale;
				const float rowWidth = 40.0f * queuedConfig.globalScale;

				if (ImGui::BeginTable("LiveCameraReadingsTable", 3)) {

					ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
					ImGui::TableNextRow(0, rowWidth);
					ImGui::TableNextColumn();
					ImGui::PushItemWidth(itemWidth * 0.8f);
					GUI_Input("FOV", cameraData.fov, 0.1f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
					ImGui::PopItemWidth();
					ImGui::Text("");

					ImGui::TableNextRow(0, rowWidth);
					ImGui::TableNextColumn();

					for (int index = 0; index < countof(cameraData.data); index++) {
						if (index > 0) {
							ImGui::TableNextColumn();
						}

						ImGui::PushItemWidth(itemWidth * 0.8f);
						ImGui::Text(dataNames[index]);
						GUI_Input("X", cameraData.data[index].x, 10.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
						GUI_Input("Y", cameraData.data[index].y, 10.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
						GUI_Input("Z", cameraData.data[index].z, 10.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
						GUI_Input("A", cameraData.data[index].a, 10.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
						ImGui::Text("");
						ImGui::PopItemWidth();
					}

					ImGui::TableNextRow(0, rowWidth);
					ImGui::TableNextColumn();

					GUI_Input("高度", cameraData.height, 10.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);

					ImGui::TableNextColumn();

					GUI_Input("俯仰", cameraData.tilt, 0.05f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);

					ImGui::TableNextRow(0, rowWidth);
					ImGui::TableNextColumn();

					GUI_Input("距离", cameraData.distanceCam, 10.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);

					ImGui::TableNextColumn();

					GUI_Input("锁定距离", cameraData.distanceLockOn, 10.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);

					ImGui::EndTable();
				}

				ImGui::PopItemWidth();
			}
		}

		ImGui::PopFont(); // Pop Roboto (defaultFontSize * 0.9f)
	}

	ImGui::PopItemWidth(); // Pop initial PushItemWidth
	ImGui::PopStyleColor(); // Pop CheckMark color
	ImGui::PopFont(); // Pop initial Roboto (defaultFontSize * 0.9f)

	ImGui::Text("");
}

#pragma endregion

#pragma region Damage

void CustomDamageSection() {
	auto defaultFontSize = UI::g_UIContext.DefaultFontSize;

	float smallerComboMult = 0.7f;
	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);
	//Gui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);
	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);
	if (GUI_Checkbox2("自定义伤害作弊", activeCrimsonGameplay.Cheats.General.customDamage, queuedCrimsonGameplay.Cheats.General.customDamage)) {
		if (!activeCrimsonGameplay.Cheats.General.customDamage) {
			// Reset damage config to default when the checkbox is unchecked
			CopyMemory(&queuedCrimsonGameplay.Cheats.Damage.playerReceivedDmgMult, 
				&defaultCrimsonGameplay.Cheats.Damage.playerReceivedDmgMult, sizeof(queuedCrimsonGameplay.Cheats.Damage.playerReceivedDmgMult));
			CopyMemory(&activeCrimsonGameplay.Cheats.Damage.playerReceivedDmgMult, 
				&queuedCrimsonGameplay.Cheats.Damage.playerReceivedDmgMult, sizeof(activeCrimsonGameplay.Cheats.Damage.playerReceivedDmgMult));

			CopyMemory(&queuedCrimsonGameplay.Cheats.Damage.enemyReceivedDmgMult,
				&defaultCrimsonGameplay.Cheats.Damage.enemyReceivedDmgMult, sizeof(queuedCrimsonGameplay.Cheats.Damage.enemyReceivedDmgMult));
			CopyMemory(&activeCrimsonGameplay.Cheats.Damage.enemyReceivedDmgMult,
				&queuedCrimsonGameplay.Cheats.Damage.enemyReceivedDmgMult, sizeof(activeCrimsonGameplay.Cheats.Damage.enemyReceivedDmgMult));
		}
	}
	ImGui::PopFont();
	UI::SeparatorEx(defaultFontSize * 23.35f);

	ImGui::Text("");

	auto DamageDataInput = [](float& active, float& queued, float& defaultVar) {
		auto defaultFontSize = UI::g_UIContext.DefaultFontSize;

		const float rowWidth = 40.0f * queuedConfig.globalScale * 0.5f;
		float smallerComboMult = 0.7f;

		ImGui::PushItemWidth(itemWidth * smallerComboMult);
		GUI_InputDefault2("", active, queued, defaultVar, 0.1f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::PopItemWidth();
		};

	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);
	{
		// Get initial cursor position for manual layout
		ImVec2 initialPos = ImGui::GetCursorPos();
		const float columnWidth = queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale * 0.5f;
		GUI_PushDisable(!activeCrimsonGameplay.Cheats.General.customDamage);

		ImGui::SetCursorPos(initialPos);
		// Start first table (left side)
		if (ImGui::BeginTable("DamageOptionsTable", 3)) {
			ImGui::TableSetupColumn("b1", 0, columnWidth);
			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();
			GUI_Checkbox2("玩家免疫伤害", activeCrimsonGameplay.Cheats.Damage.playerReceiveDmgDisable, queuedCrimsonGameplay.Cheats.Damage.playerReceiveDmgDisable);
			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("玩家受伤倍率");
			ImGui::PopFont();
			DamageDataInput(activeCrimsonGameplay.Cheats.Damage.playerReceivedDmgMult, 
				queuedCrimsonGameplay.Cheats.Damage.playerReceivedDmgMult, defaultCrimsonGameplay.Cheats.Damage.playerReceivedDmgMult);
			ImGui::SameLine();
			{
				static bool toggled = false;
				if (GUI_Button("一击杀死玩家")) {

					if (!toggled) {
						toggled = true;
						activeCrimsonGameplay.Cheats.Damage.playerReceivedDmgMult = queuedCrimsonGameplay.Cheats.Damage.playerReceivedDmgMult = 500.0f;
					}
					else {
						toggled = false;
						activeCrimsonGameplay.Cheats.Damage.playerReceivedDmgMult = queuedCrimsonGameplay.Cheats.Damage.playerReceivedDmgMult =
							defaultCrimsonGameplay.Cheats.Damage.playerReceivedDmgMult;
					}
				}
			}
			ImGui::TableNextColumn();

			GUI_Checkbox2("敌人免疫伤害", activeCrimsonGameplay.Cheats.Damage.enemyRecieveDmgDisable, queuedCrimsonGameplay.Cheats.Damage.enemyRecieveDmgDisable);
			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("敌人受伤倍率");
			ImGui::PopFont();
			DamageDataInput(activeCrimsonGameplay.Cheats.Damage.enemyReceivedDmgMult, 
				queuedCrimsonGameplay.Cheats.Damage.enemyReceivedDmgMult, defaultCrimsonGameplay.Cheats.Damage.enemyReceivedDmgMult);
			ImGui::SameLine();
			{
				static bool toggled = false;
				if (GUI_Button("一击杀死敌人")) {

					if (!toggled) {
						toggled = true;
						activeCrimsonGameplay.Cheats.Damage.enemyReceivedDmgMult = queuedCrimsonGameplay.Cheats.Damage.enemyReceivedDmgMult = 9999.0f;
					}
					else {
						toggled = false;
						activeCrimsonGameplay.Cheats.Damage.enemyReceivedDmgMult = queuedCrimsonGameplay.Cheats.Damage.enemyReceivedDmgMult = 
							defaultCrimsonGameplay.Cheats.Damage.enemyReceivedDmgMult;
					}
				}
			}


			//I do not give a fuck.
			if (g_cerbDamageValue != activeCrimsonGameplay.Cheats.Damage.enemyReceivedDmgMult)
				g_cerbDamageValue = activeCrimsonGameplay.Cheats.Damage.enemyReceivedDmgMult;

			GUI_PopDisable(!activeCrimsonGameplay.Cheats.General.customDamage);
			ImGui::EndTable();
		}
	}

	
	ImGui::PopStyleColor();
	ImGui::PopFont();
	ImGui::Text("");
}

#pragma endregion

#pragma region Dante

// @Todo: Move.
template <typename T>
void ActionData(const char* label, T(&vars)[2], T(&vars2)[2], T(&defaultVars)[2], T step = 1, const char* format = 0,
	ImGuiInputTextFlags flags = 0, float32 width = 150) {
	auto& style = ImGui::GetStyle();

	ImGui::PushItemWidth(width);
	GUI_InputDefault2("", vars[0], vars2[0], defaultVars[0], step, format, flags);
	ImGui::SameLine(0, style.ItemInnerSpacing.x);
	GUI_InputDefault2(label, vars[1], vars2[1], defaultVars[1], step, format, flags);
	ImGui::PopItemWidth();
}

#pragma endregion

#pragma region Shop

namespace Shop {

const char* tabNames[] = {
	"但丁恶魔武器",
	"但丁枪械",
	"但丁风格",
	"维吉尔恶魔武器",
	"维吉尔幻影剑",
	"维吉尔风格",
	"道具",
};

struct ShopExperienceHelper {
	const char* name;
	const char* description;
	int64 id;
	uint32 price;
	int64 last;
	int64 next;
	int64 devilarm;
	int64 gun;
	int64 child[5];
};

struct ShopExperienceStyleHelper {
	const char* name;
	const char* description;
	uint32 price;
	int64 styleid;
	int64 stylelevel;
	int64 styleexp;
	int64 moddedunlock;
	int64 moddedchild;
};


ShopExperienceHelper shopHelpersDante[] = {
	{"冲刺","脱离战斗时冲刺，类似 DMC4/5 的加速能力。", UNLOCK_DANTE::SPRINT, 3000, -1, -1,-1,-1, {-1,-1,-1,-1,-1}},
	{"空中挑衅·升空","任意武器或风格：空中按挑衅键。", UNLOCK_DANTE::SKY_LAUNCH_AIR_TAUNT, 10000, -1, -1,-1,-1, {-1,-1,-1,-1,-1}},
	{"叛逆 突刺 1 级","",UNLOCK_DANTE::REBELLION_STINGER_LEVEL_1, 2500, -1, UNLOCK_DANTE::REBELLION_STINGER_LEVEL_2,-1,-1, {UNLOCK_DANTE::REBELLION_STINGER_AIR,-1,-1,-1,-1}},
	{"叛逆 突刺 2 级","",UNLOCK_DANTE::REBELLION_STINGER_LEVEL_2, 10000, UNLOCK_DANTE::REBELLION_STINGER_LEVEL_1, -1,-1,-1, {-1,-1,-1,-1,-1}},
	{"叛逆 空中突刺","空中：锁定 + 前 + 近战。",UNLOCK_DANTE::REBELLION_STINGER_AIR, 5000, UNLOCK_DANTE::REBELLION_STINGER_LEVEL_1, -1,-1,-1, {-1,-1,-1,-1,-1}},
	{"叛逆 驱动","",UNLOCK_DANTE::REBELLION_DRIVE, 10000, -1, UNLOCK_DANTE::REBELLION_OVERDRIVE,-1,-1, {-1,-1,-1,-1,-1}},
	{"叛逆 超驱动","Hold Melee during Drive to charge, then while releasing the projectile, press Melee again to perform Overdrive.\nQuick Drive can also be held to charge for 1s, then press Melee during the swing for Overdrive.",UNLOCK_DANTE::REBELLION_OVERDRIVE, 10000, UNLOCK_DANTE::REBELLION_DRIVE, -1,-1,-1, {-1,-1,-1,-1,-1}},
	{"空中跳跃","",UNLOCK_DANTE::REBELLION_AIR_HIKE, 20000, -1, -1,-1,-1, {-1,-1,-1,-1,-1}},
	{"刻耳柏洛斯 左轮 2 级","",UNLOCK_DANTE::CERBERUS_REVOLVER_LEVEL_2, 15000, -1, -1,DEVILARMUNLOCKS::CERBERUS,-1, {-1,-1,-1,-1,-1}},
	{"刻耳柏洛斯 空中左轮","空中：锁定 + 前 + 近战。",UNLOCK_DANTE::CERBERUS_REVOLVER_AIR, 5000, -1, -1,DEVILARMUNLOCKS::CERBERUS,-1, {-1,-1,-1,-1,-1}},
	{"刻耳柏洛斯 风车","",UNLOCK_DANTE::CERBERUS_WINDMILL, 7500, -1, -1,DEVILARMUNLOCKS::CERBERUS,-1, {-1,-1,-1,-1,-1}},
	{"阿耆尼与楼陀罗 喷射 2 级","",UNLOCK_DANTE::AGNI_RUDRA_JET_STREAM_LEVEL_2, 10000, -1, UNLOCK_DANTE::AGNI_RUDRA_JET_STREAM_LEVEL_3,DEVILARMUNLOCKS::AGNI_RUDRA,-1, {-1,-1,-1,-1,-1}},
	{"阿耆尼与楼陀罗 喷射 3 级","",UNLOCK_DANTE::AGNI_RUDRA_JET_STREAM_LEVEL_3, 15000, UNLOCK_DANTE::AGNI_RUDRA_JET_STREAM_LEVEL_2, -1,DEVILARMUNLOCKS::AGNI_RUDRA,-1, {-1,-1,-1,-1,-1}},
	{"阿耆尼与楼陀罗 旋风","",UNLOCK_DANTE::AGNI_RUDRA_WHIRLWIND, 7500, -1, UNLOCK_DANTE::AGNI_RUDRA_WHIRLWIND_AIR,DEVILARMUNLOCKS::AGNI_RUDRA,-1, {-1,-1,-1,-1,-1}},
	{"阿耆尼与楼陀罗 空中旋风","空中：锁定 + 后 + 近战",UNLOCK_DANTE::AGNI_RUDRA_WHIRLWIND_AIR, 10000, UNLOCK_DANTE::AGNI_RUDRA_WHIRLWIND, -1,DEVILARMUNLOCKS::AGNI_RUDRA,-1, {-1,-1,-1,-1,-1}},
	//{"阿耆尼与楼陀罗 空中跳跃",UNLOCK_DANTE::AGNI_RUDRA_AIR_HIKE, 20000, -1, -1,DEVILARMUNLOCKS::AGNI_RUDRA,-1},
	{"涅梵 混响冲击 1 级","",UNLOCK_DANTE::NEVAN_REVERB_SHOCK_LEVEL_1, 7500, -1, UNLOCK_DANTE::NEVAN_REVERB_SHOCK_LEVEL_2,DEVILARMUNLOCKS::NEVAN,-1, {-1,-1,-1,-1,-1}},
	{"涅梵 混响冲击 2 级","",UNLOCK_DANTE::NEVAN_REVERB_SHOCK_LEVEL_2, 15000, UNLOCK_DANTE::NEVAN_REVERB_SHOCK_LEVEL_1, -1,DEVILARMUNLOCKS::NEVAN,-1, {-1,-1,-1,-1,-1}},
	{"涅梵 蝙蝠裂隙 2 级","",UNLOCK_DANTE::NEVAN_BAT_RIFT_LEVEL_2, 10000, -1, -1,DEVILARMUNLOCKS::NEVAN,-1, {-1,-1,-1,-1,-1}},
	{"涅梵 空中突袭","",UNLOCK_DANTE::NEVAN_AIR_RAID, 20000, -1, -1,DEVILARMUNLOCKS::NEVAN,-1, {-1,-1,-1,-1,-1}},
	{"涅梵 音量提升","",UNLOCK_DANTE::NEVAN_VOLUME_UP, 20000, -1, -1,DEVILARMUNLOCKS::NEVAN,-1, {-1,-1,-1,-1,-1}},
	{"贝奥武夫 直拳 2 级","",UNLOCK_DANTE::BEOWULF_STRAIGHT_LEVEL_2, 10000, -1, -1,DEVILARMUNLOCKS::BEOWULF,-1, {-1,-1,-1,-1,-1}},
	{"贝奥武夫 兽性上勾拳","",UNLOCK_DANTE::BEOWULF_BEAST_UPPERCUT, 7500, -1, UNLOCK_DANTE::BEOWULF_RISING_DRAGON,DEVILARMUNLOCKS::BEOWULF,-1, {-1,-1,-1,-1,-1}},
	{"贝奥武夫 升龙","",UNLOCK_DANTE::BEOWULF_RISING_DRAGON, 15000, UNLOCK_DANTE::BEOWULF_BEAST_UPPERCUT, UNLOCK_DANTE::BEOWULF_RISING_DRAGON_AIR, DEVILARMUNLOCKS::BEOWULF,-1, {-1,-1,-1,-1,-1}},
	{"贝奥武夫 空中龙","空中：锁定 + 后 + 近战",UNLOCK_DANTE::BEOWULF_RISING_DRAGON_AIR, 20000, UNLOCK_DANTE::BEOWULF_RISING_DRAGON, -1,DEVILARMUNLOCKS::BEOWULF,-1, {-1,-1,-1,-1,-1}},
	//{"贝奥武夫 空中跳跃",UNLOCK_DANTE::BEOWULF_AIR_HIKE, 20000, -1, -1,DEVILARMUNLOCKS::BEOWULF,-1},
};

ShopExperienceHelper shopHelpersDanteGuns[] = {
	{"黑檀木与白象牙 2 级","",UNLOCK_DANTE::EBONY_IVORY_LEVEL_2, 5000, -1, UNLOCK_DANTE::EBONY_IVORY_LEVEL_3,-1,-1, {-1,-1,-1,-1,-1}},
	{"黑檀木与白象牙 3 级","",UNLOCK_DANTE::EBONY_IVORY_LEVEL_3, 10000, UNLOCK_DANTE::EBONY_IVORY_LEVEL_2, -1,-1,-1, {-1,-1,-1,-1,-1}},
	{"霰弹枪 2 级","",UNLOCK_DANTE::SHOTGUN_LEVEL_2, 10000, -1, UNLOCK_DANTE::SHOTGUN_LEVEL_3,-1,GUNUNLOCKS::SHOTGUN, {-1,-1,-1,-1,-1}},
	{"霰弹枪 3 级","",UNLOCK_DANTE::SHOTGUN_LEVEL_3, 20000, UNLOCK_DANTE::SHOTGUN_LEVEL_2, -1,-1,GUNUNLOCKS::SHOTGUN, {-1,-1,-1,-1,-1}},
	{"阿尔忒弥斯 2 级","",UNLOCK_DANTE::ARTEMIS_LEVEL_2, 10000, -1, UNLOCK_DANTE::ARTEMIS_LEVEL_3,-1,GUNUNLOCKS::ARTEMIS, {-1,-1,-1,-1,-1}},
	{"阿尔忒弥斯 3 级","",UNLOCK_DANTE::ARTEMIS_LEVEL_3, 20000, UNLOCK_DANTE::ARTEMIS_LEVEL_2, -1,-1,GUNUNLOCKS::ARTEMIS, {-1,-1,-1,-1,-1}},
	{"螺旋 2 级","",UNLOCK_DANTE::SPIRAL_LEVEL_2, 7500, -1, UNLOCK_DANTE::SPIRAL_LEVEL_3,-1,GUNUNLOCKS::SPIRAL, {-1,-1,-1,-1,-1}},
	{"螺旋 3 级","",UNLOCK_DANTE::SPIRAL_LEVEL_3, 15000, UNLOCK_DANTE::SPIRAL_LEVEL_2, -1,-1,GUNUNLOCKS::SPIRAL, {-1,-1,-1,-1,-1}},
	{"卡琳娜·安 2 级","",UNLOCK_DANTE::KALINA_ANN_LEVEL_2, 5000, -1, UNLOCK_DANTE::KALINA_ANN_LEVEL_3,-1,GUNUNLOCKS::KALINA_ANN, {-1,-1,-1,-1,-1}},
	{"卡琳娜·安 3 级","",UNLOCK_DANTE::KALINA_ANN_LEVEL_3, 10000, UNLOCK_DANTE::KALINA_ANN_LEVEL_2, -1,-1,GUNUNLOCKS::KALINA_ANN, {-1,-1,-1,-1,-1}},
};

ShopExperienceStyleHelper shopHelpersDanteStyle[] = {
	//{"分身 2 级", 20000, STYLE::DOPPELGANGER,STYLE_LEVEL::LEVEL_TWO,STYLE_LEVEL_EXP::LEVEL_TWO},
//	{"分身 3 级", 30000, STYLE::DOPPELGANGER,STYLE_LEVEL::LEVEL_THREE,STYLE_LEVEL_EXP::LEVEL_THREE},

//	{"水银 2 级", 20000, STYLE::QUICKSILVER,STYLE_LEVEL::LEVEL_TWO,STYLE_LEVEL_EXP::LEVEL_TWO},
//	{"水银 3 级", 30000, STYLE::QUICKSILVER,STYLE_LEVEL::LEVEL_THREE,STYLE_LEVEL_EXP::LEVEL_THREE},

	{"剑圣 2 级", "",20000, STYLE::SWORDMASTER,STYLE_LEVEL::LEVEL_TWO,STYLE_LEVEL_EXP::LEVEL_TWO,-1,-1},
	{"剑圣 3 级","", 30000, STYLE::SWORDMASTER,STYLE_LEVEL::LEVEL_THREE,STYLE_LEVEL_EXP::LEVEL_THREE,-1,UNLOCK_DANTE::SWORDMASTER_MODDED_MOVES},
	{"剑圣 4 级","空中龙卷（贝奥武夫）：空中不推方向 + 风格键。降低龙卷整体伤害，可继承惯性。", 15000, STYLE::SWORDMASTER,STYLE_LEVEL::LEVEL_THREE,STYLE_LEVEL_EXP::LEVEL_THREE, UNLOCK_DANTE::SWORDMASTER_MODDED_MOVES,-1},
	
	{"枪神 2 级", "", 20000, STYLE::GUNSLINGER,STYLE_LEVEL::LEVEL_TWO,STYLE_LEVEL_EXP::LEVEL_TWO,-1,-1},
	{"枪神 3 级","", 30000, STYLE::GUNSLINGER,STYLE_LEVEL::LEVEL_THREE,STYLE_LEVEL_EXP::LEVEL_THREE,-1,UNLOCK_DANTE::GUNSLINGER_MODDED_MOVES},
	{"枪神 4 级", "后滑步（Coyote-A）：锁定 + 后 + 风格键。但丁向后滑步，击中身后敌人。",15000, STYLE::GUNSLINGER,STYLE_LEVEL::LEVEL_THREE,STYLE_LEVEL_EXP::LEVEL_THREE,UNLOCK_DANTE::GUNSLINGER_MODDED_MOVES,-1},


	{"骗术师 2 级", "",20000, STYLE::TRICKSTER,STYLE_LEVEL::LEVEL_TWO,STYLE_LEVEL_EXP::LEVEL_TWO,-1,-1},
	{"骗术师 3 级", "",30000, STYLE::TRICKSTER,STYLE_LEVEL::LEVEL_THREE,STYLE_LEVEL_EXP::LEVEL_THREE,-1,UNLOCK_DANTE::TRICKSTER_MODDED_MOVES},
	{"骗术师 4 级", "Ground Trick: Lock On + Back to Forward + Style.\n"
				"但丁传送到敌人附近的地面。",15000, STYLE::TRICKSTER,STYLE_LEVEL::LEVEL_THREE,STYLE_LEVEL_EXP::LEVEL_THREE,UNLOCK_DANTE::TRICKSTER_MODDED_MOVES,-1},

	{"皇家护卫 2 级", "",20000, STYLE::ROYALGUARD,STYLE_LEVEL::LEVEL_TWO,STYLE_LEVEL_EXP::LEVEL_TWO,-1,-1},
	{"皇家护卫 3 级", "",30000, STYLE::ROYALGUARD,STYLE_LEVEL::LEVEL_THREE,STYLE_LEVEL_EXP::LEVEL_THREE,-1,UNLOCK_DANTE::ROYALGUARD_MODDED_MOVES},
	{"皇家护卫 4 级", "魔力强化皇家护卫：普通格挡改为消耗魔力，直至魔力不足；破防转为普通格挡。",15000, STYLE::ROYALGUARD,STYLE_LEVEL::LEVEL_THREE,STYLE_LEVEL_EXP::LEVEL_THREE,UNLOCK_DANTE::ROYALGUARD_MODDED_MOVES,-1},


};

ShopExperienceStyleHelper shopHelpersVergilStyle[] = {
	{"暗黑骑士 2 级", "",20000,STYLE::DARK_SLAYER,STYLE_LEVEL::LEVEL_TWO,STYLE_LEVEL_EXP::LEVEL_TWO,-1,-1},
	{"暗黑骑士 3 级", "",30000,STYLE::DARK_SLAYER,STYLE_LEVEL::LEVEL_THREE,STYLE_LEVEL_EXP::LEVEL_THREE,-1,UNLOCK_VERGIL::DARK_SLAYER_MODDED_MOVES},
	//{"暗黑骑士 4 级", "Allows Vergil to Trick Up instead of Air Tricking when using the Style Button without Lock On.\n"
	//			"类似《鬼泣5》。",15000,STYLE::DARK_SLAYER,STYLE_LEVEL::LEVEL_THREE,STYLE_LEVEL_EXP::LEVEL_THREE,UNLOCK_VERGIL::DARK_SLAYER_MODDED_MOVES,-1},
};


ShopExperienceHelper shopHelpersVergil[] = {
	{"冲刺","脱离战斗时冲刺，类似 DMC4/5 的加速能力。", UNLOCK_VERGIL::SPRINT, 3000, -1, -1,-1,-1, {-1,-1,-1,-1,-1}},
	{"旭日空中嘲讽","任意武器或风格：空中按挑衅键。", UNLOCK_VERGIL::RISING_SUN_AIR_TAUNT, 10000, -1, -1,-1,-1, {-1,-1,-1,-1,-1}},
	{"阎魔刀 快速斩 1 级","",UNLOCK_VERGIL::YAMATO_RAPID_SLASH_LEVEL_1, 5000, -1, UNLOCK_VERGIL::YAMATO_RAPID_SLASH_LEVEL_2,-1,-1, {UNLOCK_VERGIL::YAMATO_RISING_STAR,-1,-1,-1,-1}},
	{"阎魔刀 快速斩 2 级","",UNLOCK_VERGIL::YAMATO_RAPID_SLASH_LEVEL_2, 15000, UNLOCK_VERGIL::YAMATO_RAPID_SLASH_LEVEL_1, -1,-1,-1, {-1,-1,-1,-1,-1}},
	{"阎魔刀 升星","快速斩期间按住近战键。",UNLOCK_VERGIL::YAMATO_RISING_STAR, 10000, UNLOCK_VERGIL::YAMATO_RAPID_SLASH_LEVEL_1, -1,-1,-1, {-1,-1,-1,-1,-1}},
	{"阎魔刀 上挑","锁定 + 后 + 按住近战键",UNLOCK_VERGIL::YAMATO_HIGH_TIME, 7500, -1, -1,-1,-1, {-1,-1,-1,-1,-1}},
	{"阎魔刀 次元斩 1 级","",UNLOCK_VERGIL::YAMATO_JUDGEMENT_CUT_LEVEL_1, 10000, -1, UNLOCK_VERGIL::YAMATO_JUDGEMENT_CUT_LEVEL_2,-1,-1, {-1,-1,-1,-1,-1}},
	{"阎魔刀 次元斩 2 级","",UNLOCK_VERGIL::YAMATO_JUDGEMENT_CUT_LEVEL_2, 20000, UNLOCK_VERGIL::YAMATO_JUDGEMENT_CUT_LEVEL_1, -1,-1,-1, {-1,-1,-1,-1,-1}},
	{"贝奥武夫 坠星 2 级","",UNLOCK_VERGIL::BEOWULF_STARFALL_LEVEL_2, 7500, -1, -1,-1,-1, {-1,-1,-1,-1,-1}},
	{"贝奥武夫 旭日","",UNLOCK_VERGIL::BEOWULF_RISING_SUN, 5000, -1, UNLOCK_VERGIL::BEOWULF_RISING_SUN_AIR,-1,-1, {-1,-1,-1,-1,-1}},
	{"贝奥武夫 空中旭日","空中：锁定 + 后 + 近战键",UNLOCK_VERGIL::BEOWULF_RISING_SUN_AIR, 10000, UNLOCK_VERGIL::BEOWULF_RISING_SUN, -1,-1,-1, {-1,-1,-1,-1,-1}},
	{"贝奥武夫 月相 2 级","",UNLOCK_VERGIL::BEOWULF_LUNAR_PHASE_LEVEL_2, 15000, -1, -1,-1,-1, {-1,-1,-1,-1,-1}},
	{"贝奥武夫 空中月相","空中：锁定 + 前 + 近战。",UNLOCK_VERGIL::BEOWULF_LUNAR_PHASE_AIR, 20000, -1, -1,-1,-1, {-1,-1,-1,-1,-1}},
	{"阎魔刀与力量之刃 下劈 2 级","",UNLOCK_VERGIL::YAMATO_FORCE_EDGE_HELM_BREAKER_LEVEL_2, 13000, -1, -1,-1,-1, {-1,-1,-1,-1,-1}},
	{"阎魔刀与力量之刃 突刺 1 级","",UNLOCK_VERGIL::YAMATO_FORCE_EDGE_STINGER_LEVEL_1, 5000, -1, UNLOCK_VERGIL::YAMATO_FORCE_EDGE_STINGER_LEVEL_2,-1,-1, {UNLOCK_VERGIL::YAMATO_FORCE_EDGE_STINGER_AIR,-1,-1,-1,-1}},
	{"阎魔刀与力量之刃 突刺 2 级","",UNLOCK_VERGIL::YAMATO_FORCE_EDGE_STINGER_LEVEL_2, 10000, UNLOCK_VERGIL::YAMATO_FORCE_EDGE_STINGER_LEVEL_1, -1,-1,-1, {-1,-1,-1,-1,-1}},
	{"阎魔刀与力量之刃 空中突刺","空中：锁定 + 前 + 近战。",UNLOCK_VERGIL::YAMATO_FORCE_EDGE_STINGER_AIR, 7500, UNLOCK_VERGIL::YAMATO_FORCE_EDGE_STINGER_LEVEL_1, -1,-1,-1, {-1,-1,-1,-1,-1}},
	{"阎魔刀与力量之刃 回旋投剑","",UNLOCK_VERGIL::YAMATO_FORCE_EDGE_ROUND_TRIP, 10000, -1, -1,-1,-1, {-1,-1,-1,-1,-1}},

};

ShopExperienceHelper shopHelpersVergilGuns[] = {
	{"幻影剑 2 级","", UNLOCK_VERGIL::SUMMONED_SWORDS_LEVEL_2, 7500, -1, UNLOCK_VERGIL::SUMMONED_SWORDS_LEVEL_3,-1,-1, {-1,-1,-1,-1,-1}},
	{"幻影剑 3 级","", UNLOCK_VERGIL::SUMMONED_SWORDS_LEVEL_3, 15000, UNLOCK_VERGIL::SUMMONED_SWORDS_LEVEL_2, UNLOCK_VERGIL::SUMMON_SWORDS_LEVEL_4,-1,-1, {-1,-1,-1,-1,-1}},
	{"幻影剑 4 级","Increases the speed of summoned swords by 2x, decreases their damage by half and increases hitstop from 3.0 to 8.0.\n"
	"额外速度不适用于炽热剑。",UNLOCK_VERGIL::SUMMON_SWORDS_LEVEL_4, 20000, UNLOCK_VERGIL::SUMMONED_SWORDS_LEVEL_3, -1,-1,-1, {-1,-1,-1,-1,-1}},
	{"螺旋剑 1 级","",UNLOCK_VERGIL::SPIRAL_SWORDS, 20000,-1, UNLOCK_VERGIL::STORM_SWORDS_MODDED,-1,-1, {-1,-1,-1,-1,-1}},
	{"风暴剑 2 级","Makes Storm Swords Launch Enemies. 60.0 displacement damage per sword hit.\n"
				"位移伤害由每剑 0.0 改为 60.0。",UNLOCK_VERGIL::STORM_SWORDS_MODDED, 10000, UNLOCK_VERGIL::SPIRAL_SWORDS, -1,-1,-1, {-1,-1,-1,-1,-1}},
};


struct ShopItemHelper {
	const uint8 itemIndex;
	const uint8 buyIndex;
	const uint8 maxItemCount;
	const uint32* prices;
	const uint8 priceCount;
};


constexpr ShopItemHelper itemHelpers[] = {
	{ITEM::VITAL_STAR_SMALL, BUY::VITAL_STAR_SMALL, 30, itemVitalStarSmallPrices, static_cast<uint8>(countof(itemVitalStarSmallPrices))},
	{ITEM::VITAL_STAR_LARGE, BUY::VITAL_STAR_LARGE, 30, itemVitalStarLargePrices, static_cast<uint8>(countof(itemVitalStarLargePrices))},
	{ITEM::DEVIL_STAR, BUY::DEVIL_STAR, 10, itemDevilStarPrices, static_cast<uint8>(countof(itemDevilStarPrices))},
	{ITEM::HOLY_WATER, BUY::HOLY_WATER, 30, itemHolyWaterPrices, static_cast<uint8>(countof(itemHolyWaterPrices))},
	{ITEM::BLUE_ORB, BUY::BLUE_ORB, 6, itemBlueOrbPrices, static_cast<uint8>(countof(itemBlueOrbPrices))},
	{ITEM::PURPLE_ORB, BUY::PURPLE_ORB, 7, itemPurpleOrbPrices, static_cast<uint8>(countof(itemPurpleOrbPrices))},
	{ITEM::GOLD_ORB, BUY::GOLD_ORB, 3, itemGoldOrbPrices, static_cast<uint8>(countof(itemGoldOrbPrices))},
	{ITEM::YELLOW_ORB, BUY::YELLOW_ORB, 99, itemYellowOrbPrices, static_cast<uint8>(countof(itemYellowOrbPrices))},
};


namespace ITEM_HELPER {
enum {
	BLUE_ORB = 4,
	PURPLE_ORB,
};
};

// Function Declarations
void ShowExperienceTab(int charID,ExpConfig::ExpData& expData, ShopExperienceHelper* helpers, new_size_t helperCount, MissionData& missionData,uint64 baseMoveCount);
void ShowExperienceStyleTab(ExpConfig::ExpData& expData,ShopExperienceStyleHelper* styleHelpers, new_size_t styleHelperCount, MissionData& missionData, uint8 character);

void ShowItemTab(MissionData& missionData, QueuedMissionActorData& queuedMissionActorData, ActiveMissionActorData& activeMissionActorData, bool unlockDevilTrigger);
void HandleItemPurchase(uint8 itemHelperIndex, MissionData& missionData, ActiveMissionActorData& activeMissionActorData, uint32 price);
void HandleItemSale(uint8 itemHelperIndex, MissionData& missionData, ActiveMissionActorData& activeMissionActorData);
uint32 GetItemPrice(const ShopItemHelper& itemHelper, uint8 buyCount);

void ShopWindow() {
	static bool run = false;
	static std::chrono::steady_clock::time_point shopOpenedTime;
	static bool shopCooldownActive = false;

	if (!g_showShop) {
		run = false;
		shopCooldownActive = false;
		return;
	}
	auto& io = ImGui::GetIO();

	auto missionDataPtr = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E30);
	if (!missionDataPtr) {
		return;
	}
	auto& missionData = *reinterpret_cast<MissionData*>(missionDataPtr);
	auto& queuedMissionActorData = *reinterpret_cast<QueuedMissionActorData*>(missionDataPtr + 0xC0);
	auto& activeMissionActorData = *reinterpret_cast<ActiveMissionActorData*>(missionDataPtr + 0x16C);

	bool unlockDevilTrigger = (activeMissionActorData.maxMagicPoints >= 3000);

	if (!run) {
		// TODO: Play Divine Statue Track - Mia
		//PlayTrack("afs/sound/Jikushinzou.ogg");
		run = true;
		shopOpenedTime = std::chrono::steady_clock::now();
		shopCooldownActive = true;
	}

	// Check if cooldown has expired (0.2 seconds)
	if (shopCooldownActive) {
		auto now = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - shopOpenedTime).count();
		if (elapsed >= 200) {
			shopCooldownActive = false;
		}
	}

	float width = 1500 * scaleFactorY;
	float height = 1020 * scaleFactorY;

	ImGui::SetNextWindowSize(ImVec2(width, height));
	ImGui::SetNextWindowPos(ImVec2(((g_renderSize.x - width) / 2), ((g_renderSize.y - height) / 2)));

	if (io.NavInputs[ImGuiNavInput_Cancel] && (io.NavInputsDownDuration[ImGuiNavInput_Cancel] == 0.0f)) {
		Log("controller back button");
		if (!io.NavVisible && !shopCooldownActive) {
			CloseShop();
			run = false;
			shopCooldownActive = false;
		}
	}
	
	//Keyboard back key handling
	int keyboardBackKey = activeCrimsonConfig.System.KeyboardConfig.keybinds[11]; // L key is default, but can be rebound in the config.
	if (io.KeysDown[keyboardBackKey] && (io.KeysDownDuration[keyboardBackKey] == 0.0f)) {
		if (!shopCooldownActive) {
			CloseShop();
			run = false;
			shopCooldownActive = false;
		}
	}

	if (ImGui::Begin("ShopWindow", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize)) {
		ImGui::SetWindowFontScale(scaleFactorY);
		// Darken button colors for the shop UI
		ImGui::PushStyleColor(ImGuiCol_Button, UI::SwapColorEndianness(0x96183CFF));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UI::SwapColorEndianness(0xBA1F4BFF));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, UI::SwapColorEndianness(0x70122DFF));
		ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, ImVec4(0.10f, 0.05f, 0.05f, 0.50f));
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, UI::SwapColorEndianness(0x96183CFF));
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, UI::SwapColorEndianness(0xBA1F4BFF));
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, UI::SwapColorEndianness(0x70122DFF));
		if (GUI_CloseX(true)) {
			if (!shopCooldownActive) {
				CloseShop();
				run = false;
				shopCooldownActive = false;
			}
		}
		ImGui::Text("");
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
		ImGui::PushFont(UI::g_ImGuiFont_RedOrbRusso[UI::g_UIContext.DefaultFontSize * 2.0f]);
		// Red orb icon instead of bullet
		if (auto* orbTex = CrimsonHUD::getRedOrbTexture(); orbTex && orbTex->IsValid()) {
			constexpr float ORB_ASPECT = 142.0f / 200.0f;
			float orbHeight = UI::g_UIContext.DefaultFontSize * 2.0f * scaleFactorY;
			float orbWidth = orbHeight * ORB_ASPECT;
			ImVec2 cursorPos = ImGui::GetCursorScreenPos();
			ImVec2 orbSize(orbWidth, orbHeight);
			ImGui::GetWindowDrawList()->AddImage(orbTex->GetTexture(), cursorPos, cursorPos + orbSize);
			ImGui::Dummy(orbSize);
		} else {
			constexpr auto BULLET = u8"•";
			ImGui::Text((const char*)BULLET);
		}
		ImGui::PopStyleColor();
		ImGui::SameLine();

		// Red orb count with backdrop shadow (same styling as RedOrbCounterWindow)
		{
			char numBuf[32];
			snprintf(numBuf, sizeof(numBuf), "%u", missionData.redOrbs);
			float fontSizePx = UI::g_UIContext.DefaultFontSize * 2.0f * scaleFactorY;
			ImVec2 cursorPos = ImGui::GetCursorScreenPos();
			ImU32 shadowColor = IM_COL32(100, 10, 10, 255);
			ImU32 textColor = IM_COL32(255, 255, 255, 255);
			ImGui::GetWindowDrawList()->AddText(
				UI::g_ImGuiFont_RedOrbRussoBackdrop[UI::g_UIContext.DefaultFontSize * 2.0f],
				fontSizePx, cursorPos, shadowColor, numBuf);
			ImGui::GetWindowDrawList()->AddText(
				UI::g_ImGuiFont_RedOrbRusso[UI::g_UIContext.DefaultFontSize * 2.0f],
				fontSizePx, cursorPos, textColor, numBuf);
			ImGui::Dummy(ImVec2(ImGui::CalcTextSize(numBuf).x, fontSizePx));
		}
		ImGui::PopFont();
		ImGui::Text("");
		auto BACKGROUND_FADED_TEXT = g_scene != SCENE::MISSION_START ? u8"神像" : u8"自定义";
		ImFont* fadedFont = UI::g_ImGuiFont_Benguiat256;
		float fadedFontSize = scaledFontSize * 4.8f;
		
		ImVec2 bgFadedTextSize = ImGui::CalcTextSize((const char*)BACKGROUND_FADED_TEXT, 
				nullptr, false, fadedFontSize);
		
		// Get window position and size
		ImVec2 winPos = ImGui::GetWindowPos();
		ImVec2 winSize = ImGui::GetWindowSize();

		float rightMargin = 600.0f * scaleFactorY;

		float x = winPos.x + winSize.x - bgFadedTextSize.x - rightMargin;
		float y = winPos.y + 15.0f * scaleFactorY; // top margin

		// Only show Divinity Statue text if not in mission start scene
		
		ImGui::GetWindowDrawList()->AddText(
			fadedFont, fadedFontSize,
			ImVec2(x, y),
			UI::SwapColorEndianness(0xFFFFFF10),
			(const char*)BACKGROUND_FADED_TEXT
		);
		

		// 		for (uint8 playerIndex = 0; playerIndex < activeConfig.Actor.playerCount; ++playerIndex) {
		// 			auto& gamepad = GetGamepad(actorData.newGamepad);
		// 
		// 			if ((gamepad.buttons[0] & GetBinding(BINDING::STYLE_ACTION))) {
		// 				CloseShop();
		// 			}
		// 		}

		SelectPlayerLoadoutsWeaponsTab();

		if (ImGui::BeginTabBar("ShopTabs")) {
			for (uint8 tabIndex = 0; tabIndex < TAB::COUNT; ++tabIndex) {
				ImGui::PushFont(UI::g_ImGuiFont_Benguiat[UI::g_UIContext.DefaultFontSize * 1.08f]);
				bool tabOpen = ImGui::BeginTabItem(tabNames[tabIndex]);
				ImGui::PopFont();
				if (tabOpen) {
					ImGui::Text("");

					switch (tabIndex) {
					case TAB::DANTE_DEVILARM:
						ShowExperienceTab(CHARACTER::DANTE,ExpConfig::missionExpDataDante, shopHelpersDante, sizeof(shopHelpersDante) / sizeof(ShopExperienceHelper), missionData,UNLOCK_DANTE::COUNT);
						break;
					case TAB::DANTE_GUN:
						ShowExperienceTab(CHARACTER::DANTE, ExpConfig::missionExpDataDante, shopHelpersDanteGuns, sizeof(shopHelpersDanteGuns) / sizeof(ShopExperienceHelper), missionData, UNLOCK_DANTE::COUNT);
						break;
					case TAB::DANTE_STYLE:
						ShowExperienceStyleTab(ExpConfig::missionExpDataDante, shopHelpersDanteStyle, sizeof(shopHelpersDanteStyle) / sizeof(ShopExperienceStyleHelper), missionData, CHARACTER::DANTE);
						break;
					case TAB::VERGIL_DEVILARM:
						ShowExperienceTab(CHARACTER::VERGIL, ExpConfig::missionExpDataVergil, shopHelpersVergil, sizeof(shopHelpersVergil) / sizeof(ShopExperienceHelper), missionData, UNLOCK_VERGIL::COUNT);
						break;
					case TAB::VERGIL_GUN:
						ShowExperienceTab(CHARACTER::VERGIL, ExpConfig::missionExpDataVergil, shopHelpersVergilGuns, sizeof(shopHelpersVergilGuns) / sizeof(ShopExperienceHelper), missionData, UNLOCK_VERGIL::COUNT);
						break;
					case TAB::VERGIL_STYLE:
						ShowExperienceStyleTab(ExpConfig::missionExpDataVergil, shopHelpersVergilStyle, sizeof(shopHelpersVergilStyle) / sizeof(ShopExperienceStyleHelper), missionData, CHARACTER::VERGIL);
						break;
					case TAB::ITEMS:
						ShowItemTab(missionData, queuedMissionActorData, activeMissionActorData, unlockDevilTrigger);
						break;
					}

					ImGui::EndTabItem();
				}
			}
			ImGui::EndTabBar();
		}

		ImGui::PopStyleColor(7);
		ImGui::End();
	}
}


void ShowStyleLevelsTab(ExpConfig::ExpData& expData, MissionData& missionData) {
// 	auto Buy = [&]() {
// 		if (missionData.redOrbs < helper.price) {
// 			FMOD_PlaySound(0, 19);
// 			return;
// 		}
// 
// 		missionData.redOrbs -= helper.price;
// 		FMOD_PlaySound(0, 18);
// 		expData.unlocks[helperIndex] = true;
// 		ExpConfig::UpdatePlayerActorExps();
// 		};
// 
// 	if (GUI_Button("剑圣 2 级", ImVec2(500.0f, 80.0f))) {
// 		Buy();
// 	}
// 
// 	GUI_PopDisable(condition);
// 
// 	// Display price and bullet point in a new column
// 	ImGui::SameLine(120);
// 
// 	bool priceCondition = (missionData.redOrbs < helper.price);
// 	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
// 	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[UI::g_UIContext.DefaultFontSize * 1.5f]);
// 	constexpr auto BULLET = u8"•";
// 	ImGui::Text((const char*)BULLET);
// 	ImGui::PopFont();
// 	ImGui::PopStyleColor();
// 
// 	GUI_PushDisable(priceCondition);
// 	ImGui::SameLine();
// 	ImGui::Text("%u", helper.price);
// 	GUI_PopDisable(priceCondition);
}


void ShowExperienceTab(int charID, ExpConfig::ExpData& expData, ShopExperienceHelper* helpers, new_size_t helperCount, MissionData& missionData, uint64 baseMoveCount) {
	auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

	const float columnWidth = 0.48f * queuedConfig.globalScale;
	const float rowHeight = 40.0f * queuedConfig.globalScale;
	int columnTracker = 0;
	// Center the table within the available width
	{
		float availW = ImGui::GetContentRegionAvail().x;
		float tableW = (675.0f + 155.0f + 675.0f + 155.0f) * scaleFactorY;
		float off = (availW - tableW) * 0.5f;
		if (off > 0.0f) ImGui::SetCursorPosX(off);
	}
	if (ImGui::BeginTable("SkillTable", 4)) {
		ImGui::TableSetupColumn("Col1", ImGuiTableColumnFlags_WidthAuto, 675.0f * scaleFactorY); // Default to 100.0f
		ImGui::TableSetupColumn("SellCol1", ImGuiTableColumnFlags_WidthAuto, 155.0f * scaleFactorY); // Default to 200.0f
		ImGui::TableSetupColumn("Col2", ImGuiTableColumnFlags_WidthAuto, 675.0f * scaleFactorY); // Default to 100.0f
		ImGui::TableSetupColumn("SellCol2", ImGuiTableColumnFlags_WidthAuto, 155.0f * scaleFactorY); // Default to 200.0f
		ImGui::TableNextRow(0, rowHeight);

		for (size_t helperIndex = 0; helperIndex < helperCount; ++helperIndex) {
			auto& helper = helpers[helperIndex];

			// Check if the item is accessible (prerequisites met)
			bool canAccess = true;
			if (helper.devilarm > -1 && !weaponProgression.devilArmUnlocks[helper.devilarm])
				canAccess = false;
			if (helper.gun > -1 && !weaponProgression.gunUnlocks[helper.gun])
				canAccess = false;
			// Don't allow buying modded moves if extra moves are turned off (still show if already purchased)
			if (helper.id > baseMoveCount && !activeCrimsonGameplay.Gameplay.General.extramoves && !expData.unlocks[helper.id])
				canAccess = false;

			columnTracker++;
			ImGui::TableNextColumn();
			auto Buy = [&]() {
				if (missionData.redOrbs < helper.price) {
					FMOD_PlaySound(0, 19);
					return;
				}

				missionData.redOrbs -= helper.price;
				FMOD_PlaySound(0, 18);
				expData.unlocks[helper.id] = true;
				ExpConfig::UpdatePlayerActorExps();
				};

			auto Sell = [&]() {
				missionData.redOrbs += helper.price;
				FMOD_PlaySound(0, 18);
				expData.unlocks[helper.id] = false;
				//only refund move expertise if it's not a modded move.
				//if (helper.id < baseMoveCount)
					//ExpConfig::RefundMoveExpertise(charID, helper.id);
				//Make sure everything relying on this move is properly disabled
				if (helper.next > -1)
					expData.unlocks[helper.next] = false;
				for (int64 childIndex = 0; childIndex < 5; ++childIndex) {
					if(helper.child[childIndex] > -1)
						expData.unlocks[helper.child[childIndex]] = false;
				}


				// Sets the flag off on sessionData expertise to also update non Actor System
	// 			const auto& expertiseHelper =
	// 				(sessionData.character == CHARACTER::DANTE)
	// 				? ExpConfig::expertiseHelpersDante[helperIndex]
	// 				: ExpConfig::expertiseHelpersVergil[helperIndex];
	// 
	// 			sessionData.expertise[expertiseHelper.index] -= expertiseHelper.flags;
				ExpConfig::UpdatePlayerActorExps();
				};

			bool alreadyPurchased = expData.unlocks[helper.id];
			bool condition = (alreadyPurchased || ((helper.last > -1) && !expData.unlocks[helper.last]) || !canAccess);
			GUI_PushDisable(condition);

			// Begin a new row
			ImGui::BeginGroup();
			if (alreadyPurchased) {
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
			}
			ImVec2 btnSize(555.0f * scaleFactorY, 80.0f * scaleFactorY);
			ImVec2 btnPos = ImGui::GetCursorScreenPos();
			bool result = GUI_Button(helper.name, btnSize);
			if (alreadyPurchased) {
				ImGui::PopStyleColor(3);
				const char* soldOutText = "已售罄";
				ImGui::PushFont(UI::g_ImGuiFont_Benguiat[UI::g_UIContext.DefaultFontSize * 0.7f]);
				ImVec2 textSize = ImGui::CalcTextSize(soldOutText);
				float pad = 6.0f * scaleFactorY;
				ImVec2 textPos(btnPos.x + btnSize.x - textSize.x - pad, btnPos.y + btnSize.y - textSize.y - pad);
				ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), textPos, IM_COL32(200, 60, 60, 255), soldOutText);
				ImGui::PopFont();
			}
			if (helper.description != "")
			{
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
					ImGui::BeginTooltip();
					ImGui::SetWindowFontScale(scaleFactorY);
					ImGui::PushTextWrapPos(555.0f * scaleFactorY);
					ImGui::Text(helper.description);
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
				//TooltipHelper("(?)", helper.description);
			}
			if (result && canAccess) {
				
				Buy();
			}

			GUI_PopDisable(condition);

			// Display price and bullet point in a new column
			ImGui::SameLine(120 * scaleFactorY);

			bool priceCondition = (missionData.redOrbs < helper.price);
			// Red orb icon instead of bullet
			if (auto* orbTex = CrimsonHUD::getRedOrbTexture(); orbTex && orbTex->IsValid()) {
				constexpr float ORB_ASPECT = 142.0f / 200.0f;
				float orbHeight = UI::g_UIContext.DefaultFontSize * 1.5f * scaleFactorY;
				float orbWidth = orbHeight * ORB_ASPECT;
				ImVec2 cursorPos = ImGui::GetCursorScreenPos();
				ImVec2 orbSize(orbWidth, orbHeight);
				ImGui::GetWindowDrawList()->AddImage(orbTex->GetTexture(), cursorPos, cursorPos + orbSize);
				ImGui::Dummy(orbSize);
			} else {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
				ImGui::PushFont(UI::g_ImGuiFont_RussoOne[UI::g_UIContext.DefaultFontSize * 1.5f]);
				constexpr auto BULLET = u8"•";
				ImGui::Text((const char*)BULLET);
				ImGui::PopFont();
				ImGui::PopStyleColor();
			}

			GUI_PushDisable(priceCondition);
			ImGui::SameLine();

			// Price with backdrop shadow (same styling as RedOrbCounterWindow)
			{
				char numBuf[32];
				snprintf(numBuf, sizeof(numBuf), "%u", helper.price);
				float fontSizePx = UI::g_UIContext.DefaultFontSize * 1.5f * scaleFactorY;
				ImVec2 cursorPos = ImGui::GetCursorScreenPos();
				ImU32 shadowColor = priceCondition ? IM_COL32(30, 3, 3, 255) : IM_COL32(100, 10, 10, 255);
				ImU32 textColor = priceCondition ? IM_COL32(80, 80, 80, 255) : IM_COL32(255, 255, 255, 255);
				ImGui::GetWindowDrawList()->AddText(
					UI::g_ImGuiFont_RedOrbRussoBackdrop[UI::g_UIContext.DefaultFontSize * 1.5f],
					fontSizePx, cursorPos, shadowColor, numBuf);
				ImGui::GetWindowDrawList()->AddText(
					UI::g_ImGuiFont_RedOrbRusso[UI::g_UIContext.DefaultFontSize * 1.5f],
					fontSizePx, cursorPos, textColor, numBuf);
				ImVec2 ts = UI::g_ImGuiFont_RedOrbRusso[UI::g_UIContext.DefaultFontSize * 1.5f]->CalcTextSizeA(fontSizePx, FLT_MAX, 0.0f, numBuf);
				ImGui::Dummy(ts);
			}
			GUI_PopDisable(priceCondition);

			
			

			// Display the sell button in a new column
			ImGui::EndGroup(); // End the group for the current row
			ImGui::TableNextColumn();
			//if (expData.unlocks[helper.id]) {
				//check if there's a "next move" and if it's unlocked. That means we shouldn't be able to refund. 
				condition = !expData.unlocks[helper.id] ||((helper.next > -1) && expData.unlocks[helper.next]);
				//if next is a modded move and extra moves is off, ignore that requirement 
				//if ((helper.next > baseMoveCount) && !activeCrimsonGameplay.Gameplay.General.extramoves)
					//condition = false;
				//check for extra children 
				if (!condition) {
					//hardcoded to 5 because I was being lazy, updated to base off array size when convenient
					for (int64 childIndex = 0; childIndex < 5; ++childIndex) {

						//should stop these pre-requisites from being valid if they're modded and we're disabled. 
						//if (helper.child[childIndex] > baseMoveCount && !activeCrimsonGameplay.Gameplay.General.extramoves)
							//continue;

						condition = ((helper.child[childIndex] > -1) && expData.unlocks[helper.child[childIndex]]);
						if (condition)
							break;
					}
				}

				GUI_PushDisable(condition);

				if (GUI_Button("出售",ImVec2(80.0f * scaleFactorY, 80.0f * scaleFactorY))) {
					Sell();
				}
				GUI_PopDisable(condition);
			//}


			

			if (helper.next == -1) {
				ImGui::Spacing(); // Add spacing between rows
				ImGui::TableNextRow(0, rowHeight);
			}

			//ImGui::TableNextColumn();
			//if (columnTracker % 2 == 0) {
			//	ImGui::TableNextRow(0, rowHeight);
			//	ImGui::TableNextColumn();
			//}
		}
	ImGui::EndTable();
	}
}

void ShowExperienceStyleTab(ExpConfig::ExpData& expData, ShopExperienceStyleHelper* styleHelpers, new_size_t styleHelperCount, MissionData& missionData, uint8 character) {
	auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

	auto missionDataAddr = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E30);
	auto& missionDataRef = *reinterpret_cast<MissionData*>(missionDataAddr);
	auto& queuedMissionActorData = *reinterpret_cast<QueuedMissionActorData*>(missionDataAddr + 0xC0);
	auto& activeMissionActorData = *reinterpret_cast<ActiveMissionActorData*>(missionDataAddr + 0x16C);
	//buying style upgrades here

	const float columnWidth = 0.48f * queuedConfig.globalScale;
	const float rowHeight = 40.0f * queuedConfig.globalScale;
	int columnTracker = 0;
	// Center the table within the available width
	{
		float availW = ImGui::GetContentRegionAvail().x;
		float tableW = (455.0f + 110.0f + 455.0f + 110.0f + 455.0f + 110.0f) * scaleFactorY;
		float off = (availW - tableW) * 0.5f;
		if (off > 0.0f) ImGui::SetCursorPosX(off);
	}
	if (ImGui::BeginTable("SkillTable", 6)) {
		ImGui::TableSetupColumn("Col1", ImGuiTableColumnFlags_WidthAuto, 455.0f * scaleFactorY); // Default to 100.0f
		ImGui::TableSetupColumn("SellCol1", ImGuiTableColumnFlags_WidthAuto, 110.0f * scaleFactorY); // Default to 200.0f
		ImGui::TableSetupColumn("Col2", ImGuiTableColumnFlags_WidthAuto, 455.0f * scaleFactorY); // Default to 100.0f
		ImGui::TableSetupColumn("SellCol2", ImGuiTableColumnFlags_WidthAuto, 110.0f * scaleFactorY); // Default to 200.0f
		ImGui::TableSetupColumn("Col3", ImGuiTableColumnFlags_WidthAuto, 455.0f * scaleFactorY); // Default to 100.0f
		ImGui::TableSetupColumn("SellCol3", ImGuiTableColumnFlags_WidthAuto, 110.0f * scaleFactorY); // Default to 200.0f
		for (size_t helperIndex = 0; helperIndex < styleHelperCount; ++helperIndex) {
			auto& helper = styleHelpers[helperIndex];

			//Check if the style is owned (for styles that can be missed like Quicksilver/Doppelganger)
			bool styleOwned = true;
			if (helper.styleid > -1) {
				if (helper.styleid == STYLE::QUICKSILVER and !sessionData.weaponAndStyleUnlocks[WEAPONANDSTYLEUNLOCKS::QUICKSILVER])
					styleOwned = false;
				if (helper.styleid == STYLE::DOPPELGANGER and !sessionData.weaponAndStyleUnlocks[WEAPONANDSTYLEUNLOCKS::DOPPELGANGER])
					styleOwned = false;
			}

			//Just making sure our style id is in range
			if (helper.styleid >= STYLE::MAX)
				continue;

			//Check if we're doing modded style or vanilla style
			if (helper.moddedunlock > -1) {
				//don't allow buying if extra_shop disabled AND modded move not already purchased (expData.unlocks[moddedunlock] false)
				bool canAccess = styleOwned;
				if (!activeCrimsonGameplay.Gameplay.General.extramoves && !expData.unlocks[helper.moddedunlock])
					canAccess = false;


				ImGui::TableNextColumn();
				//calculates how far through leveling up a style you were as a percentage

				auto Buy = [&]() {
					if (missionData.redOrbs < helper.price) {
						FMOD_PlaySound(0, 19);
						return;
					}

					missionData.redOrbs -= helper.price;
					FMOD_PlaySound(0, 18);
					expData.unlocks[helper.moddedunlock] = true;
					ExpConfig::UpdatePlayerActorExps();
				};


				auto Sell = [&]() {
					missionData.redOrbs += helper.price;
					FMOD_PlaySound(0, 18);
					expData.unlocks[helper.moddedunlock] = false;
					ExpConfig::UpdatePlayerActorExps();
					};

				//Buy Button gui element starts
				bool alreadyPurchased = expData.unlocks[helper.moddedunlock];
				bool condition = alreadyPurchased || (expData.styleLevels[helper.styleid] != STYLE_LEVEL::LEVEL_THREE) || !canAccess;
				GUI_PushDisable(condition);

				// Begin a new row
				ImGui::BeginGroup();
				
				if (alreadyPurchased) {
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
				}
				ImVec2 btnSize(375.0f * scaleFactorY, 80.0f * scaleFactorY);
				ImVec2 btnPos = ImGui::GetCursorScreenPos();
				if (GUI_Button(helper.name, btnSize)) {
					if (canAccess) {
						Buy();
					}
				}
				if (alreadyPurchased) {
					ImGui::PopStyleColor(3);
					const char* soldOutText = "已售罄";
					ImGui::PushFont(UI::g_ImGuiFont_Benguiat[UI::g_UIContext.DefaultFontSize * 0.7f]);
					ImVec2 textSize = ImGui::CalcTextSize(soldOutText);
					float pad = 6.0f * scaleFactorY;
					ImVec2 textPos(btnPos.x + btnSize.x - textSize.x - pad, btnPos.y + btnSize.y - textSize.y - pad);
					ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), textPos, IM_COL32(200, 60, 60, 255), soldOutText);
					ImGui::PopFont();
				}

				if (helper.description != "")
				{
					if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
						ImGui::BeginTooltip();
						ImGui::SetWindowFontScale(scaleFactorY);
						ImGui::PushTextWrapPos(375.0f * scaleFactorY);
						ImGui::Text(helper.description);
						ImGui::PopTextWrapPos();
						ImGui::EndTooltip();
					}
					//TooltipHelper("(?)", helper.description);
				}

				GUI_PopDisable(condition);

				// Display price and bullet point in a new column
				ImGui::SameLine(90);

				// Red orb icon instead of bullet
				if (auto* orbTex = CrimsonHUD::getRedOrbTexture(); orbTex && orbTex->IsValid()) {
					constexpr float ORB_ASPECT = 142.0f / 200.0f;
					float orbHeight = UI::g_UIContext.DefaultFontSize * 1.5f * scaleFactorY;
					float orbWidth = orbHeight * ORB_ASPECT;
					ImVec2 cursorPos = ImGui::GetCursorScreenPos();
					ImVec2 orbSize(orbWidth, orbHeight);
					ImGui::GetWindowDrawList()->AddImage(orbTex->GetTexture(), cursorPos, cursorPos + orbSize);
					ImGui::Dummy(orbSize);
				} else {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[UI::g_UIContext.DefaultFontSize * 1.5f]);
					constexpr auto BULLET = u8"•";
					ImGui::Text((const char*)BULLET);
					ImGui::PopFont();
					ImGui::PopStyleColor();
				}
				ImGui::SameLine();

				// Price with backdrop shadow (same styling as RedOrbCounterWindow)
				{
					char numBuf[32];
					snprintf(numBuf, sizeof(numBuf), "%u", helper.price);
					float fontSizePx = UI::g_UIContext.DefaultFontSize * 1.5f * scaleFactorY;
					ImVec2 cursorPos = ImGui::GetCursorScreenPos();
					ImU32 shadowColor = IM_COL32(100, 10, 10, 255);
					ImU32 textColor = IM_COL32(255, 255, 255, 255);
					ImGui::GetWindowDrawList()->AddText(
						UI::g_ImGuiFont_RedOrbRussoBackdrop[UI::g_UIContext.DefaultFontSize * 1.5f],
						fontSizePx, cursorPos, shadowColor, numBuf);
					ImGui::GetWindowDrawList()->AddText(
						UI::g_ImGuiFont_RedOrbRusso[UI::g_UIContext.DefaultFontSize * 1.5f],
						fontSizePx, cursorPos, textColor, numBuf);
					ImVec2 ts = UI::g_ImGuiFont_RedOrbRusso[UI::g_UIContext.DefaultFontSize * 1.5f]->CalcTextSizeA(fontSizePx, FLT_MAX, 0.0f, numBuf);
				ImGui::Dummy(ts);
				}

				//Sell button begins:
				// Display the sell button in a new column
				ImGui::EndGroup(); // End the group for the current row
				ImGui::TableNextColumn();
				//Disable selling when: The item is not our current style level OR if there's a modded style linked to this style that's been purchased.
				bool sellcondition = !expData.unlocks[helper.moddedunlock];
				GUI_PushDisable(sellcondition);

				if (GUI_Button("出售", ImVec2(80.0f * scaleFactorY, 80.0f * scaleFactorY))) {
					Sell();
				}
				GUI_PopDisable(sellcondition);


				ImGui::Spacing(); // Add spacing between rows
				ImGui::TableNextRow(0, rowHeight);


			}
			else {
				ImGui::TableNextColumn();
				//calculates how far through leveling up a style you were as a percentage
				uint32 percentage_discount = static_cast<uint32>(floor(100 * expData.styleExpPoints[helper.styleid] / helper.styleexp));
				if (percentage_discount < 0)
					percentage_discount = 0;
				if (percentage_discount > 100)
					percentage_discount = 100;

				//calculates cost of red orbs as the base price with a percentage discount based on how close you were to leveling up style
				//so 30% of the exp to level 2 = 30% discount
				uint32 rorb_cost_calculated = helper.price - (static_cast<uint32>(helper.price / 100) * percentage_discount);


				auto Buy = [&]() {
					if (missionData.redOrbs < rorb_cost_calculated) {
						FMOD_PlaySound(0, 19);
						return;
					}
					HeldStyleExpData& heldStyleExpData = (character == CHARACTER::DANTE)
						? heldStyleExpDataDante
						: heldStyleExpDataVergil;
					missionData.redOrbs -= rorb_cost_calculated;
					FMOD_PlaySound(0, 18);
					expData.styleLevels[helper.styleid] = helper.stylelevel;
					expData.styleExpPoints[helper.styleid] = 0;
					heldStyleExpData.missionStyleLevels[helper.styleid] = helper.stylelevel;
					heldStyleExpData.accumulatedStylePoints[helper.styleid] = 0;
					ExpConfig::UpdatePlayerActorExps();
					};


				auto Sell = [&]() {
					missionData.redOrbs += helper.price;
					FMOD_PlaySound(0, 18);
					HeldStyleExpData& heldStyleExpData = (character == CHARACTER::DANTE)
						? heldStyleExpDataDante
						: heldStyleExpDataVergil;

					expData.styleLevels[helper.styleid] = helper.stylelevel - 1;
					expData.styleExpPoints[helper.styleid] = 0;
					heldStyleExpData.missionStyleLevels[helper.styleid] = helper.stylelevel - 1;
					heldStyleExpData.accumulatedStylePoints[helper.styleid] = 0;
					ExpConfig::UpdatePlayerActorExps();
					};

				//Buy Button gui element starts
				bool condition = !(expData.styleLevels[helper.styleid] + 1 == helper.stylelevel);
				bool alreadyPurchased = (expData.styleLevels[helper.styleid] >= helper.stylelevel);
				GUI_PushDisable(condition || alreadyPurchased);

				// Begin a new row
				ImGui::BeginGroup();

				if (alreadyPurchased) {
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
				}
				if (GUI_Button(alreadyPurchased ? "已售罄" : helper.name, ImVec2(375.0f * scaleFactorY, 80.0f * scaleFactorY))) {
					Buy();
				}
				if (alreadyPurchased) {
					ImGui::PopStyleColor(3);
				}

				GUI_PopDisable(condition || alreadyPurchased);

				// Display price and bullet point in a new column
				ImGui::SameLine(90);

				bool priceCondition = (missionData.redOrbs < rorb_cost_calculated) || condition || alreadyPurchased;
				// Red orb icon instead of bullet
				if (auto* orbTex = CrimsonHUD::getRedOrbTexture(); orbTex && orbTex->IsValid()) {
					constexpr float ORB_ASPECT = 142.0f / 200.0f;
					float orbHeight = UI::g_UIContext.DefaultFontSize * 1.5f * scaleFactorY;
					float orbWidth = orbHeight * ORB_ASPECT;
					ImVec2 cursorPos = ImGui::GetCursorScreenPos();
					ImVec2 orbSize(orbWidth, orbHeight);
					ImGui::GetWindowDrawList()->AddImage(orbTex->GetTexture(), cursorPos, cursorPos + orbSize);
					ImGui::Dummy(orbSize);
				} else {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[UI::g_UIContext.DefaultFontSize * 1.5f]);
					constexpr auto BULLET = u8"•";
					ImGui::Text((const char*)BULLET);
					ImGui::PopFont();
					ImGui::PopStyleColor();
				}


				GUI_PushDisable(priceCondition);
				ImGui::SameLine();
				if (!alreadyPurchased) {
					float fontSizePx = UI::g_UIContext.DefaultFontSize * 1.5f * scaleFactorY;
					ImU32 shadowColor = priceCondition ? IM_COL32(30, 3, 3, 255) : IM_COL32(100, 10, 10, 255);
					ImU32 textColor = priceCondition ? IM_COL32(80, 80, 80, 255) : IM_COL32(255, 255, 255, 255);

					// Price with backdrop shadow (same styling as RedOrbCounterWindow)
					char numBuf[32];
					snprintf(numBuf, sizeof(numBuf), "%u", rorb_cost_calculated);
					ImVec2 cursorPos = ImGui::GetCursorScreenPos();
					ImGui::GetWindowDrawList()->AddText(
						UI::g_ImGuiFont_RedOrbRussoBackdrop[UI::g_UIContext.DefaultFontSize * 1.5f],
						fontSizePx, cursorPos, shadowColor, numBuf);
					ImGui::GetWindowDrawList()->AddText(
						UI::g_ImGuiFont_RedOrbRusso[UI::g_UIContext.DefaultFontSize * 1.5f],
						fontSizePx, cursorPos, textColor, numBuf);
					ImVec2 ts = UI::g_ImGuiFont_RedOrbRusso[UI::g_UIContext.DefaultFontSize * 1.5f]->CalcTextSizeA(fontSizePx, FLT_MAX, 0.0f, numBuf);
					ImGui::Dummy(ts);

					if (rorb_cost_calculated != helper.price) {
						ImGui::SameLine(0, 0);
						ImGui::Text(" (%u%% off!)", percentage_discount);
						ImGui::SameLine();
						GUI_PopDisable(priceCondition);
						TooltipHelper("(?)", "折扣按该风格获得的经验计算，购买后重置。");
						GUI_PushDisable(priceCondition);
					}
				}
				GUI_PopDisable(priceCondition);



				//Sell button begins:
				// Display the sell button in a new column
				ImGui::EndGroup(); // End the group for the current row
				ImGui::TableNextColumn();
				//Disable selling when: The item is not our current style level OR if there's a modded style linked to this style that's been purchased.
				bool sellcondition = (expData.styleLevels[helper.styleid] != helper.stylelevel) || (helper.moddedchild > -1 && expData.unlocks[helper.moddedchild]);
				GUI_PushDisable(sellcondition);

				if (GUI_Button("出售", ImVec2(80.0f * scaleFactorY, 80.0f * scaleFactorY))) {
					Sell();
				}
				GUI_PopDisable(sellcondition);


				if (helper.stylelevel == STYLE_LEVEL::LEVEL_THREE && (helper.moddedchild == -1 || !activeCrimsonGameplay.Gameplay.General.extramoves)) {
					ImGui::Spacing(); // Add spacing between rows
					ImGui::TableNextRow(0, rowHeight);
				}
			}
		}
		ImGui::EndTable();
	}
}


void ShowItemTab(MissionData& missionData, QueuedMissionActorData& queuedMissionActorData, ActiveMissionActorData& activeMissionActorData, bool unlockDevilTrigger) {
	auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);
	for (int itemHelperIndex = 0; itemHelperIndex < sizeof(itemHelpers) / sizeof(ShopItemHelper); ++itemHelperIndex) {
		auto& itemHelper = itemHelpers[itemHelperIndex];

		uint8& itemCount = missionData.itemCounts[itemHelper.itemIndex];
		uint8& buyCount = missionData.buyCounts[itemHelper.buyIndex];

		if ((itemHelper.itemIndex == ITEM::PURPLE_ORB || itemHelper.itemIndex == ITEM::DEVIL_STAR) && !unlockDevilTrigger) {
			continue;
		}
		if (itemHelper.itemIndex == ITEM::GOLD_ORB && !sessionData.useGoldOrb)
			continue;
		if (itemHelper.itemIndex == ITEM::YELLOW_ORB && sessionData.useGoldOrb)
			continue;
		uint32 price = GetItemPrice(itemHelper, buyCount);

		if (GUI_Button(itemNames[itemHelper.itemIndex], ImVec2(875.0f * scaleFactorY, 80.0f * scaleFactorY))) {
			HandleItemPurchase(itemHelperIndex, missionData, activeMissionActorData, price);
		}
		ImGui::SameLine(120);

		bool priceCondition = (missionData.redOrbs < price);
		// Red orb icon instead of bullet
		if (auto* orbTex = CrimsonHUD::getRedOrbTexture(); orbTex && orbTex->IsValid()) {
			constexpr float ORB_ASPECT = 142.0f / 200.0f;
			float orbHeight = UI::g_UIContext.DefaultFontSize * 1.5f * scaleFactorY;
			float orbWidth = orbHeight * ORB_ASPECT;
			ImVec2 cursorPos = ImGui::GetCursorScreenPos();
			ImVec2 orbSize(orbWidth, orbHeight);
			ImGui::GetWindowDrawList()->AddImage(orbTex->GetTexture(), cursorPos, cursorPos + orbSize);
			ImGui::Dummy(orbSize);
		} else {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[UI::g_UIContext.DefaultFontSize * 1.5f]);
			constexpr auto BULLET = u8"•";
			ImGui::Text((const char*)BULLET);
			ImGui::PopFont();
			ImGui::PopStyleColor();
		}

		GUI_PushDisable(priceCondition);
		ImGui::SameLine();

		// Price with backdrop shadow (same styling as RedOrbCounterWindow)
		{
			char numBuf[32];
			snprintf(numBuf, sizeof(numBuf), "%u", price);
			float fontSizePx = UI::g_UIContext.DefaultFontSize * 1.5f * scaleFactorY;
			ImVec2 cursorPos = ImGui::GetCursorScreenPos();
			ImU32 shadowColor = priceCondition ? IM_COL32(30, 3, 3, 255) : IM_COL32(100, 10, 10, 255);
			ImU32 textColor = priceCondition ? IM_COL32(80, 80, 80, 255) : IM_COL32(255, 255, 255, 255);
			ImGui::GetWindowDrawList()->AddText(
				UI::g_ImGuiFont_RedOrbRussoBackdrop[UI::g_UIContext.DefaultFontSize * 1.5f],
				fontSizePx, cursorPos, shadowColor, numBuf);
			ImGui::GetWindowDrawList()->AddText(
				UI::g_ImGuiFont_RedOrbRusso[UI::g_UIContext.DefaultFontSize * 1.5f],
				fontSizePx, cursorPos, textColor, numBuf);
			ImVec2 ts = UI::g_ImGuiFont_RedOrbRusso[UI::g_UIContext.DefaultFontSize * 1.5f]->CalcTextSizeA(fontSizePx, FLT_MAX, 0.0f, numBuf);
			ImGui::Dummy(ts);
		}
		GUI_PopDisable(priceCondition);

		ImGui::SameLine(300);

		bool quantityCondition = (itemCount >= itemHelper.maxItemCount);
		GUI_PushDisable(quantityCondition);
		ImGui::Text("x%02u / %02u", itemCount, itemHelper.maxItemCount);
		GUI_PopDisable(quantityCondition);


		if (!(itemHelper.itemIndex == ITEM::BLUE_ORB || itemHelper.itemIndex == ITEM::PURPLE_ORB) && itemCount != 0) {
			ImGui::SameLine(550);
			if (GUI_Button("出售")) {
				HandleItemSale(itemHelperIndex, missionData, activeMissionActorData);
			}
		}
	}
}

void UpdateStatsAfterPurchase(const ShopItemHelper& itemHelper, ActiveMissionActorData& activeMissionActorData) {
	auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

	if (itemHelper.itemIndex == ITEM::BLUE_ORB) {
		float value = (activeMissionActorData.maxHitPoints + 1000);

		[&]() {
			if (!g_defaultNewActorData[0].baseAddr) {
				return;
			}
			auto& actorData = *reinterpret_cast<PlayerActorData*>(g_defaultNewActorData[0].baseAddr);
			value = (actorData.maxHitPoints + 1000);
			actorData.hitPoints = actorData.maxHitPoints = value;
		}();
		//stop serp from overwriting blorb to session data on purchase, now if you reset mission without saving the max HP increase shouldn't be saved
		//activeMissionActorData.hitPoints = activeMissionActorData.maxHitPoints = sessionData.hitPoints = value;
		 activeMissionActorData.hitPoints = activeMissionActorData.maxHitPoints = value;


		for (uint8 playerIndex = 0; playerIndex < PLAYER_COUNT; ++playerIndex) {
			for (uint8 characterIndex = 0; characterIndex < CHARACTER_COUNT; ++characterIndex) {
				for (uint8 entityIndex = 0; entityIndex < ENTITY_COUNT; ++entityIndex) {
					auto& newActorData = GetNewActorData(playerIndex, characterIndex, entityIndex);

					if (!newActorData.baseAddr) {
						continue;
					}
					auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);

					actorData.hitPoints = actorData.maxHitPoints = value;
				}
			}
		}
	}
	else if (itemHelper.itemIndex == ITEM::PURPLE_ORB) {
		float value = (activeMissionActorData.maxMagicPoints + 1000);

		[&]() {
			if (!g_defaultNewActorData[0].baseAddr) {
				return;
			}
			auto& actorData = *reinterpret_cast<PlayerActorData*>(g_defaultNewActorData[0].baseAddr);
			value = (actorData.maxMagicPoints + 1000);
			actorData.magicPoints = actorData.maxMagicPoints = value;
		}();

		//stop serp from overwriting porb to session data on purchase, now if you reset mission without saving the max DT increase shouldn't be saved
		//activeMissionActorData.magicPoints = activeMissionActorData.maxMagicPoints = sessionData.magicPoints = value;
		activeMissionActorData.magicPoints = activeMissionActorData.maxMagicPoints = value;
		for (uint8 playerIndex = 0; playerIndex < PLAYER_COUNT; ++playerIndex) {
			for (uint8 characterIndex = 0; characterIndex < CHARACTER_COUNT; ++characterIndex) {
				for (uint8 entityIndex = 0; entityIndex < ENTITY_COUNT; ++entityIndex) {
					auto& newActorData = GetNewActorData(playerIndex, characterIndex, entityIndex);

					if (!newActorData.baseAddr) {
						continue;
					}
					auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);

					activeMissionActorData.magicPoints = activeMissionActorData.maxMagicPoints = value;
					actorData.magicPoints = actorData.maxMagicPoints = value;
				}
			}
		}
	}
}

void HandleItemPurchase(uint8 itemHelperIndex, MissionData& missionData, ActiveMissionActorData& activeMissionActorData, uint32 price) {
	auto& itemHelper = itemHelpers[itemHelperIndex];

	uint8& itemCount = missionData.itemCounts[itemHelper.itemIndex];
	uint8& buyCount = missionData.buyCounts[itemHelper.buyIndex];
	
	if ((itemCount >= itemHelper.maxItemCount) || (missionData.redOrbs < price)) {
		FMOD_PlaySound(0, 19);
		return;
	}

	itemCount++;
	buyCount++;

	if (buyCount > 254) {
		buyCount = 254;
	}

	missionData.redOrbs -= price;
	FMOD_PlaySound(0, 18);

	UpdateStatsAfterPurchase(itemHelper, activeMissionActorData);
}

void HandleItemSale(uint8 itemHelperIndex, MissionData& missionData, ActiveMissionActorData& activeMissionActorData) {
	auto& itemHelper = itemHelpers[itemHelperIndex];

	uint8& itemCount = missionData.itemCounts[itemHelper.itemIndex];
	uint8& buyCount = missionData.buyCounts[itemHelper.buyIndex];
	uint32 price = 0;

	itemCount--;
	if (buyCount > 0) {
		buyCount--;
	}

	price = GetItemPrice(itemHelper, 0);

	missionData.redOrbs += price;
	FMOD_PlaySound(0, 18);
}

uint32 GetItemPrice(const ShopItemHelper& itemHelper, uint8 buyCount) {
	if (buyCount >= itemHelper.priceCount) {
		return itemHelper.prices[itemHelper.priceCount - 1];
	}
	return itemHelper.prices[buyCount];
}

void CloseShop() {
	g_showShop = false;
	if (g_scene == SCENE::GAME) {
		PlayTrack("");
	}
}
}

#pragma endregion

#pragma region DebugSection

bool showExpWindow = false;
bool showMissionDataWindow = false;
bool showFileDataWindow = false;
bool showActorWindow = false;
bool showEventDataWindow = false;
bool showRegionDataWindow = false;
bool showSoundWindow = false;


void ExpWindow() {
    if (!showExpWindow) {
        return;
    }


    static bool run = false;
    if (!run) {
        run = true;


        constexpr float width  = 700;
        constexpr float height = 700;


        ImGui::SetNextWindowSize(ImVec2(width, height));

        // // Center
        // ImGui::SetNextWindowPos
        // (
        // 	ImVec2
        // 	(
        // 		((g_renderSize.x - width ) / 2),
        // 		((g_renderSize.y - height) / 2)
        // 	)
        // );

        // Top Right
        ImGui::SetNextWindowPos(ImVec2((g_renderSize.x - width), 0));
    }


    if (ImGui::Begin("经验内容", &showExpWindow)) {
		ImGui::SetWindowFontScale(scaleFactorY);
        ImGui::Text("");


        if (GUI_Button("保存")) {
            ExpConfig::SaveExp();
        }
        ImGui::SameLine();

        if (GUI_Button("读取")) {
            ExpConfig::LoadExp();
        }
        ImGui::Text("");


        ImGui::PushItemWidth(150);

        GUI_Input<new_size_t>("g_saveIndex", g_saveIndex, 1, "%llu", ImGuiInputTextFlags_EnterReturnsTrue);
        ImGui::Text("");

        ImGui::PopItemWidth();


        if (GUI_Button("SavePlayerActorExp")) {
            ExpConfig::SavePlayerActorExp();
        }
        ImGui::SameLine();

        if (GUI_Button("UpdatePlayerActorExps")) {
            ExpConfig::UpdatePlayerActorExps();
        }
        ImGui::Text("");


        auto FunctionOnce = [&](ExpConfig::ExpData& expData, const char* name, Shop::ShopExperienceHelper* shopHelpers, new_size_t count) {
            if (!ImGui::CollapsingHeader(name)) {
                return;
            }
            ImGui::Text("");

            ImGui::PushItemWidth(200);


            ImGui::Text("styleLevels");
            for_all(styleIndex, STYLE::MAX) {
                GUI_Input<uint32>(indexNames[styleIndex], expData.styleLevels[styleIndex], 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
            }
            ImGui::Text("");


            ImGui::Text("styleExpPoints");
            for_all(styleIndex, STYLE::MAX) {
                GUI_Input<float>(indexNames[styleIndex], expData.styleExpPoints[styleIndex], 1, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
            }
            ImGui::Text("");


            ImGui::Text("unlocks");
            for_all(index, count) {
                GUI_Checkbox(shopHelpers[index].name, expData.unlocks[index]);
            }
            ImGui::Text("");

			GUI_Checkbox("hasPairedWithActorSystem", expData.hasPairedWithActorSystem);
			ImGui::Text("");

            ImGui::PopItemWidth();
        };


        auto FunctionLoop = [&](ExpConfig::ExpData* expDataAddr, const char** saveNames, Shop::ShopExperienceHelper* shopHelpers, new_size_t count) {
            for_all(saveIndex, SAVE_COUNT) {
                auto& expData = expDataAddr[saveIndex];

                FunctionOnce(expData, saveNames[saveIndex], shopHelpers, count);
            }
        };


        FunctionOnce(ExpConfig::missionExpDataDante, "但丁任务", Shop::shopHelpersDante, countof(Shop::shopHelpersDante));
        FunctionOnce(ExpConfig::sessionExpDataDante, "但丁会话", Shop::shopHelpersDante, countof(Shop::shopHelpersDante));
        FunctionLoop(ExpConfig::savedExpDataDante, saveNamesDante, Shop::shopHelpersDante, countof(Shop::shopHelpersDante));
        ImGui::Text("");


        FunctionOnce(ExpConfig::missionExpDataVergil, "维吉尔任务", Shop::shopHelpersVergil, countof(Shop::shopHelpersVergil));
        FunctionOnce(ExpConfig::sessionExpDataVergil, "维吉尔会话", Shop::shopHelpersVergil, countof(Shop::shopHelpersVergil));
        FunctionLoop(ExpConfig::savedExpDataVergil, saveNamesVergil, Shop::shopHelpersVergil, countof(Shop::shopHelpersVergil));
        ImGui::Text("");


        auto Content = [&](PlayerActorData& actorData) {
            ImGui::PushItemWidth(200);

            GUI_Input<uint32>("character", actorData.character, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);


            GUI_Input<uint32>("style", actorData.style, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);


            GUI_Input<uint32>("styleLevel", actorData.styleLevel, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);

            GUI_Input<float>("styleExpPoints", actorData.styleExpPoints, 1, "%g", ImGuiInputTextFlags_EnterReturnsTrue);


            ImGui::Text("activeExpertise");
            for_all(index, countof(actorData.activeExpertise)) {
                GUI_Input<byte32>(indexNames[index], actorData.activeExpertise[index], 1, "%X",
                    ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsHexadecimal);
            }


            ImGui::Text("newWeaponLevels");
            for_all(index, countof(actorData.newWeaponLevels)) {
                GUI_Input<uint32>(indexNames[index], actorData.newWeaponLevels[index], 1, "%u",
                    ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsHexadecimal);
            }


            ImGui::PopItemWidth();
        };


        [&]() {
            if (!ImGui::CollapsingHeader("默认")) {
                return;
            }
            ImGui::Text("");

            // IntroduceMainActorData(actorData, return);

            if (!g_defaultNewActorData[0].baseAddr) {
                return;
            }
            auto& actorData = *reinterpret_cast<PlayerActorData*>(g_defaultNewActorData[0].baseAddr);


            Content(actorData);
            ImGui::Text("");
        }();


        old_for_all(uint8, playerIndex, PLAYER_COUNT) {
            if (!ImGui::CollapsingHeader(playerIndexNames[playerIndex])) {
                continue;
            }

            ImGui::Indent(20);


            old_for_all(uint8, characterIndex, CHARACTER_COUNT) {
                if (!ImGui::CollapsingHeader(characterIndexNames[characterIndex])) {
                    continue;
                }
                ImGui::Text("");


                old_for_all(uint8, entityIndex, ENTITY_COUNT) {
                    auto& playerData = GetPlayerData(playerIndex);

                    auto& characterData = GetCharacterData(playerIndex, characterIndex, entityIndex);
                    auto& newActorData  = GetNewActorData(playerIndex, characterIndex, entityIndex);

                    auto& activeCharacterData = GetCharacterData(playerIndex, playerData.activeCharacterIndex, ENTITY::MAIN);
                    auto& activeNewActorData  = GetNewActorData(playerIndex, playerData.activeCharacterIndex, ENTITY::MAIN);

                    auto& leadCharacterData = GetCharacterData(playerIndex, 0, ENTITY::MAIN);
                    auto& leadNewActorData  = GetNewActorData(playerIndex, 0, ENTITY::MAIN);

                    auto& mainCharacterData = GetCharacterData(playerIndex, characterIndex, ENTITY::MAIN);
                    auto& mainNewActorData  = GetNewActorData(playerIndex, characterIndex, ENTITY::MAIN);


                    if (!newActorData.baseAddr) {
                        continue;
                    }
                    auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);

                    ImGui::Text(entityNames[entityIndex]);
                    Content(actorData);
                    ImGui::Text("");
                }
            }


            ImGui::Unindent(20);
        }


        ImGui::Text("");
    }

    ImGui::End();
}


void MissionDataWindow() {
    if (!showMissionDataWindow) {
        return;
    }

    auto name_6975 = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E30);
    if (!name_6975) {
        return;
    }
    auto& missionData = *reinterpret_cast<MissionData*>(name_6975);


    auto pool_6996 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
    if (!pool_6996 || !pool_6996[8]) {
        return;
    }
    auto& eventData = *reinterpret_cast<EventData*>(pool_6996[8]);


    auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);


    static bool run = false;
    if (!run) {
        run = true;

        ImGui::SetNextWindowSize(ImVec2(500, 700));
        ImGui::SetNextWindowPos(ImVec2(0, 200));
    }


    if (ImGui::Begin("MissionData", &showMissionDataWindow)) {
		ImGui::SetWindowFontScale(scaleFactorY);
        ImGui::Text("");


        ImGui::PushItemWidth(200);

        GUI_Input<uint32>("红魂", missionData.redOrbs, 1000, "%u", ImGuiInputTextFlags_EnterReturnsTrue);

        ImGui::PopItemWidth();

        ImGui::Text("");

	
        GUI_Checkbox("unlockDevilTrigger", sessionData.unlockDevilTrigger);
		ImGui::SameLine();
		TooltipHelper("(?)", "作用于 sessionData，需重启任务才能生效。");
        ImGui::Text("");
		


        [&]() {
            auto name_7058 = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E30);
            if (!name_7058) {
                return;
            }
            auto& queuedMissionActorData = *reinterpret_cast<QueuedMissionActorData*>(name_7058 + 0xC0);
            auto& activeMissionActorData = *reinterpret_cast<ActiveMissionActorData*>(name_7058 + 0x16C);


            ImGui::PushItemWidth(200);

            ImGui::Text("激活");
            GUI_Input<float>("hitPoints", activeMissionActorData.hitPoints, 1000, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input<float>("maxHitPoints", activeMissionActorData.maxHitPoints, 1000, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input<float>("magicPoints", activeMissionActorData.magicPoints, 1000, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input<float>("maxMagicPoints", activeMissionActorData.maxMagicPoints, 1000, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::Text("");

            ImGui::Text("已排队");
            GUI_Input<float>("hitPoints", queuedMissionActorData.hitPoints, 1000, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input<float>("magicPoints", queuedMissionActorData.magicPoints, 1000, "%g", ImGuiInputTextFlags_EnterReturnsTrue);

            ImGui::PopItemWidth();

            ImGui::Text("");
        }();


        ImGui::Text("物品数量");
        ImGui::Text("");

        ImGui::PushItemWidth(150);

        old_for_all(uint8, itemIndex, ITEM::COUNT) {
            ImGui::Text("%.4u %llX", itemIndex, &missionData.itemCounts[itemIndex]);
            ImGui::SameLine();

            GUI_Input<uint8>(itemNames[itemIndex], missionData.itemCounts[itemIndex], 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
        }

        ImGui::PopItemWidth();

        ImGui::Text("");


        ImGui::Text("购买数量");
        ImGui::Text("");

        ImGui::PushItemWidth(150);

        old_for_all(uint8, buyIndex, BUY::COUNT) {
            ImGui::Text("%.4u %llX", buyIndex, &missionData.buyCounts[buyIndex]);
            ImGui::SameLine();

            GUI_Input<uint8>(buyNames[buyIndex], missionData.buyCounts[buyIndex], 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
        }

        ImGui::PopItemWidth();


        ImGui::Text("");

        if (GUI_Button("状态")) {
            eventData.event = EVENT::STATUS;
        }

        if (GUI_Button("自定义")) {
            eventData.event = EVENT::CUSTOMIZE;
        }


        ImGui::Text("");
    }

    ImGui::End();
}


struct FileContainer : Container<> {
    const char* operator[](uint32 index);

    void Push(uint32 size);
};

const char* FileContainer::operator[](uint32 index) {
    if (index >= count) {
        return 0;
    }

    auto& metadata = reinterpret_cast<Metadata*>(metadataAddr)[index];

    return reinterpret_cast<const char*>(dataAddr + metadata.off);
}

void FileContainer::Push(uint32 size) {
    auto& metadata = reinterpret_cast<Metadata*>(metadataAddr)[count];

    metadata.off  = pos;
    metadata.size = size;

    pos += size;
    count++;
}

void FileDataWindow() {
    constexpr uint32 fileDataGroupItemCounts[] = {
        4,
        136,
        60,
        28,
        1,
        128,
        6,
    };
    constexpr uint8 fileDataGroupCount                              = static_cast<uint8>(countof(fileDataGroupItemCounts));
    static FileContainer fileDataGroupNames                         = {};
    static FileContainer fileDataGroupItemNames[fileDataGroupCount] = {};
    static FileContainer enemyFileDataItemNames                     = {};
    static FileContainer enemyFileDataMetadataItemNames             = {};

    static bool run = false;
    if (!run) {
        run = true;

        {
            auto& container = fileDataGroupNames;

            container.Init((fileDataGroupCount * 5), (fileDataGroupCount * sizeof(FileContainer::Metadata)));

            old_for_all(uint8, groupIndex, fileDataGroupCount) {
                auto dest = reinterpret_cast<char*>(container.dataAddr + container.pos);
                auto size = (container.dataSize - container.pos);

                snprintf(dest, size, "%.4u", groupIndex);

                container.Push(5);
            }
        }


        uint32 g_itemIndex = 0;

        old_for_all(uint8, groupIndex, fileDataGroupCount) {
            auto& container = fileDataGroupItemNames[groupIndex];
            auto& itemCount = fileDataGroupItemCounts[groupIndex];

            container.Init((itemCount * 10), (itemCount * sizeof(FileContainer::Metadata)));

            old_for_all(uint32, itemIndex, itemCount) {
                auto dest = reinterpret_cast<char*>(container.dataAddr + container.pos);
                auto size = (container.dataSize - container.pos);

                snprintf(dest, size, "%.4u %.4u", itemIndex, g_itemIndex);

                container.Push(10);

                g_itemIndex++;
            }
        }


        {
            const uint32 itemCount = ENEMY_FILE_DATA_COUNT;

            auto& container = enemyFileDataItemNames;

            container.Init((itemCount * 6), (itemCount * sizeof(FileContainer::Metadata)));


            old_for_all(uint32, itemIndex, itemCount) {
                auto dest = reinterpret_cast<char*>(container.dataAddr + container.pos);
                auto size = (container.dataSize - container.pos);

                snprintf(dest, size, "%.4u ", itemIndex);

                container.Push(6);
            }
        }


        {
            const uint32 itemCount = ENEMY::COUNT;

            auto& container = enemyFileDataMetadataItemNames;

            container.Init((itemCount * 7), (itemCount * sizeof(FileContainer::Metadata)));


            old_for_all(uint32, itemIndex, itemCount) {
                auto dest = reinterpret_cast<char*>(container.dataAddr + container.pos);
                auto size = (container.dataSize - container.pos);

                snprintf(dest, size, "%.4u  ", itemIndex);

                container.Push(7);
            }
        }
    }


    if (!showFileDataWindow) {
        return;
    }


    static bool run2 = false;
    if (!run2) {
        run2 = true;

        ImGui::SetNextWindowSize(ImVec2(700, 700));
        ImGui::SetNextWindowPos(ImVec2(0, 0));
    }


    if (ImGui::Begin("FileData", &showFileDataWindow)) {
		ImGui::SetWindowFontScale(scaleFactorY);
        ImGui::Text("");

        auto GUI_FileData = [&](FileData& fileData) {
            ImGui::PushItemWidth(200);

            auto addr = &fileData;

            GUI_Input<uint64>("this", *reinterpret_cast<uint64*>(&addr), 0, "%.16llX", ImGuiInputTextFlags_ReadOnly);
            GUI_Input<uint32>("group", fileData.group, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input<uint32>("status", fileData.status, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input<uint64>(
                "typeDataAddr", *reinterpret_cast<uint64*>(&fileData.typeDataAddr), 0, "%.16llX", ImGuiInputTextFlags_EnterReturnsTrue);
            if (fileData.typeDataAddr) {
                ImGui::Text((*fileData.typeDataAddr).typeName);
            }
            GUI_Input<uint64>("file", *reinterpret_cast<uint64*>(&fileData.file), 0, "%.16llX", ImGuiInputTextFlags_EnterReturnsTrue);
            if (GUI_Button("重置")) {
                SetMemory(&fileData, 0, sizeof(fileData));
            }

            ImGui::PopItemWidth();
        };

        auto GUI_FileDataMetadata = [&](FileDataMetadata& metadata) {
            ImGui::PushItemWidth(200);

            auto addr = &metadata;

            GUI_Input<uint64>("this", *reinterpret_cast<uint64*>(&addr), 0, "%.16llX", ImGuiInputTextFlags_ReadOnly);

            GUI_Input<uint64>(
                "funcAddrs", *reinterpret_cast<uint64*>(&metadata.funcAddrs), 0, "%.16llX", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input<uint64>(
                "lastAddr", *reinterpret_cast<uint64*>(&metadata.lastAddr), 0, "%.16llX", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input<uint64>(
                "nextAddr", *reinterpret_cast<uint64*>(&metadata.nextAddr), 0, "%.16llX", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input<uint64>(
                "fileDataAddr", *reinterpret_cast<uint64*>(&metadata.fileDataAddr), 0, "%.16llX", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input<uint32>("category", metadata.category, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input<uint32>("fileSetIndex", metadata.fileSetIndex, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
            if (GUI_Button("重置")) {
                SetMemory(&metadata, 0, sizeof(metadata));
            }

            ImGui::PopItemWidth();
        };


        GUI_SectionStart("默认文件数据");

        uint32 g_itemIndex = 0;

        old_for_all(uint32, groupIndex, fileDataGroupCount) {
            auto open = ImGui::CollapsingHeader(fileDataGroupNames[groupIndex]);

            auto& itemCount = fileDataGroupItemCounts[groupIndex];

            ImGui::Indent(20);

            old_for_all(uint32, itemIndex, itemCount) {
                if (open && ImGui::CollapsingHeader(fileDataGroupItemNames[groupIndex][itemIndex])) {
                    ImGui::Indent(20);

                    auto& fileData = reinterpret_cast<FileData*>(appBaseAddr + 0xC99D30)[g_itemIndex];

                    GUI_FileData(fileData);

                    ImGui::Unindent(20);
                }

                g_itemIndex++;
            }

            ImGui::Unindent(20);
        }

        GUI_SectionEnd();
        ImGui::Text("");

        GUI_SectionStart("敌人文件数据");

        old_for_all(uint32, fileDataIndex, ENEMY_FILE_DATA_COUNT) {
            if (ImGui::CollapsingHeader(enemyFileDataItemNames[fileDataIndex])) {
                ImGui::Indent(20);

                GUI_FileData(enemyFileData[fileDataIndex]);

                ImGui::Unindent(20);
            }
        }

        GUI_SectionEnd();
        ImGui::Text("");

        GUI_SectionStart("敌人文件数据元数据");

        old_for_all(uint32, index, ENEMY::COUNT) {
            if (ImGui::CollapsingHeader(enemyFileDataMetadataItemNames[index])) {
                ImGui::Indent(20);

                GUI_FileDataMetadata(enemyFileDataMetadata[index]);

                ImGui::Unindent(20);
            }
        }

        ImGui::Text("");
    }

    ImGui::End();
}


void ActorWindow() {
    if (!showActorWindow) {
        return;
    }


    static bool run = false;
    if (!run) {
        run = true;

        ImGui::SetNextWindowSize(ImVec2(700, 700));
        ImGui::SetNextWindowPos(ImVec2(0, 0));
    }


    if (ImGui::Begin("ActorWindow", &showActorWindow)) {
		ImGui::SetWindowFontScale(scaleFactorY);
        ImGui::Text("");


        if (ImGui::CollapsingHeader("默认")) {
            ImGui::Text("");

            old_for_all(uint8, entityIndex, ENTITY_COUNT) {
                auto& newActorData = g_defaultNewActorData[entityIndex];

                // auto & characterName = characterIndexNames[characterIndex];
                auto& entityName = entityEnumNames[entityIndex];

                ImGui::Text(entityName);

                ImGui::PushItemWidth(150.0f);

                GUI_Input<byte64>(
                    "baseAddr", *reinterpret_cast<byte64*>(&newActorData.baseAddr), 0, "%llX", ImGuiInputTextFlags_EnterReturnsTrue);
                GUI_Input<uint8>("visibility", newActorData.visibility, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
                GUI_Checkbox("enableCollision", newActorData.enableCollision);
                [&]() {
                    if (!newActorData.baseAddr) {
                        return;
                    }
                    auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);

                    GUI_Input("速度", actorData.speed, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                    GUI_Input("速度倍率", actorData.speedMultiplier, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);

                    GUI_Input("x", actorData.position.x, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                    GUI_Input("y", actorData.position.y, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                    GUI_Input("z", actorData.position.z, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                    GUI_Input("a", actorData.position.a, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);

                    GUI_Input("生命值", actorData.hitPoints, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                    GUI_Input("最大生命值", actorData.maxHitPoints, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                    GUI_Input("魔力值", actorData.magicPoints, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                    GUI_Input("最大魔力值", actorData.maxMagicPoints, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                }();

                ImGui::PopItemWidth();

                ImGui::Text("");
            }
        }


        old_for_all(uint8, playerIndex, PLAYER_COUNT) {
            auto& playerData = GetPlayerData(playerIndex);

            // auto playerName = playerNames[playerIndex];

            if (!ImGui::CollapsingHeader(playerIndexNames[playerIndex])) {
                continue;
            }

            ImGui::Text("");

            ImGui::PushItemWidth(150.0f);
            GUI_Input<uint8>("characterCount", playerData.characterCount, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input<uint8>("characterIndex", playerData.characterIndex, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input<uint8>("lastCharacterIndex", playerData.lastCharacterIndex, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input<uint8>("activeCharacterIndex", playerData.activeCharacterIndex, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input<uint8>("collisionGroup", playerData.collisionGroup, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::PopItemWidth();
            ImGui::Text("");

            ImGui::Indent(20);

            old_for_all(uint8, characterIndex, CHARACTER_COUNT) {
                auto& characterName = characterIndexNames[characterIndex];

                if (!ImGui::CollapsingHeader(characterName)) {
                    continue;
                }

                ImGui::Text("");

                old_for_all(uint8, entityIndex, ENTITY_COUNT) {
                    auto& characterData = GetCharacterData(playerIndex, characterIndex, entityIndex);
                    auto& newActorData  = GetNewActorData(playerIndex, characterIndex, entityIndex);

                    // auto & characterName = characterIndexNames[characterIndex];
                    auto& entityName = entityEnumNames[entityIndex];

                    ImGui::Text(entityName);

                    ImGui::PushItemWidth(150.0f);

                    GUI_Input<uint8>("character", characterData.character, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
                    GUI_Input<uint8>("costume", characterData.costume, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
                    GUI_Input<byte64>(
                        "baseAddr", *reinterpret_cast<byte64*>(&newActorData.baseAddr), 0, "%llX", ImGuiInputTextFlags_EnterReturnsTrue);
                    GUI_Input<uint8>("visibility", newActorData.visibility, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
                    GUI_Checkbox("enableCollision", newActorData.enableCollision);
                    [&]() {
                        if (!newActorData.baseAddr) {
                            return;
                        }
                        auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);

                        GUI_Input("速度", actorData.speed, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                        GUI_Input("速度倍率", actorData.speedMultiplier, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);

                        GUI_Input("x", actorData.position.x, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                        GUI_Input("y", actorData.position.y, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                        GUI_Input("z", actorData.position.z, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                        GUI_Input("a", actorData.position.a, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);

                        GUI_Input("生命值", actorData.hitPoints, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                        GUI_Input("最大生命值", actorData.maxHitPoints, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                        GUI_Input("魔力值", actorData.magicPoints, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                        GUI_Input("最大魔力值", actorData.maxMagicPoints, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                    }();

                    ImGui::PopItemWidth();

                    ImGui::Text("");
                }
            }

            ImGui::Unindent(20);

            ImGui::Text("");
        }


        ImGui::Text("");
    }

    ImGui::End();
}


void EventDataWindow() {
    if (!showEventDataWindow) {
        return;
    }

    auto pool_8096 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
    if (!pool_8096 || !pool_8096[8]) {
        return;
    }
    auto& eventData = *reinterpret_cast<EventData*>(pool_8096[8]);


    static EventData eventData2 = {};

    static bool run = false;
    if (!run) {
        run = true;

        ImGui::SetNextWindowSize(ImVec2(700, 700));
        ImGui::SetNextWindowPos(ImVec2(0, 0));
    }


    if (ImGui::Begin("EventData", &showEventDataWindow)) {
		ImGui::SetWindowFontScale(scaleFactorY);
        ImGui::Text("");

        ImGui::PushItemWidth(150);

        GUI_Input<uint32>("room", eventData2.room, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
        GUI_Input<uint32>("position", eventData2.position, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
        GUI_Input<uint32>("event", eventData2.event, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
        GUI_Input<uint32>("subevent", eventData2.subevent, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
        GUI_Input<uint32>("screen", eventData2.screen, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
        GUI_Input<uint32>("nextScreen", eventData2.nextScreen, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);

        ImGui::PopItemWidth();

        ImGui::Text("");

        if (GUI_Button("应用")) {
            eventData.room       = eventData2.room;
            eventData.position   = eventData2.position;
            eventData.event      = eventData2.event;
            eventData.subevent   = eventData2.subevent;
            eventData.screen     = eventData2.screen;
            eventData.nextScreen = eventData2.nextScreen;
        }

        ImGui::Text("");
    }

    ImGui::End();
}


void RegionDataWindow() {
    static bool run = false;
    if (!run) {
        run = true;
    }

    if (!showRegionDataWindow) {
        return;
    }

    static bool run2 = false;
    if (!run2) {
        run2 = true;

        ImGui::SetNextWindowSize(ImVec2(700, 700));
        ImGui::SetNextWindowPos(ImVec2(0, 0));
    }

    if (ImGui::Begin("RegionData", &showRegionDataWindow)) {
		ImGui::SetWindowFontScale(scaleFactorY);
        ImGui::Text("");


        auto GUI_RegionData = [&](RegionData& regionData) {
            ImGui::PushItemWidth(200);

            GUI_Input<uint64>("metadataAddr", *reinterpret_cast<uint64*>(&regionData.metadataAddr), 0, "%llX",
                ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
            GUI_Input<uint64>("dataAddr", *reinterpret_cast<uint64*>(&regionData.dataAddr), 0, "%llX",
                ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
            GUI_Input<uint32>(
                "capacity", regionData.capacity, 0, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
            GUI_Input<uint32>(
                "boundary", regionData.boundary, 0, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
            GUI_Input<uint32>("size", regionData.size, 0, "%X", ImGuiInputTextFlags_ReadOnly);
            GUI_Input<uint32>("pipe", regionData.pipe, 0, "%u", ImGuiInputTextFlags_ReadOnly);
            GUI_Input<uint32>("count", regionData.count, 0, "%u", ImGuiInputTextFlags_ReadOnly);

            ImGui::PopItemWidth();
        };

        auto regionDataAddr = reinterpret_cast<RegionData*>(appBaseAddr + 0xCA8910);
        /*
        dmc3.exe+2C61BF - 4C 8D 25 4A279E00 - lea r12,[dmc3.exe+CA8910]
        dmc3.exe+2C61C6 - 44 8B C5          - mov r8d,ebp
        */

        static uint32 count = 3;

        GUI_Input<uint32>("数量", count, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);

        old_for_all(uint32, index, count) {
            auto& regionData = regionDataAddr[index];

            GUI_RegionData(regionData);
            ImGui::Text("");
        }

        ImGui::Text("");
    }
    ImGui::End();
}


void SoundWindow() {
    if (!showSoundWindow) {
        return;
    }

    static bool run = false;
    if (!run) {
        run = true;

        ImGui::SetNextWindowSize(ImVec2(600, 650));
        ImGui::SetNextWindowPos(ImVec2(0, 0));
    }

    if (ImGui::Begin("音效", &showSoundWindow)) {
		ImGui::SetWindowFontScale(scaleFactorY);
        ImGui::Text("");


        static uint32 fmodChannelIndex       = 0;
        static FMOD_CHANNEL* fmodChannelAddr = 0;
        static float volume                  = 1.0f;
        static FMOD_RESULT fmodResult        = 0;

        ImGui::PushItemWidth(200);
        if (GUI_Input<uint32>("FMOD 通道索引", fmodChannelIndex, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue)) {
            fmodChannelAddr = reinterpret_cast<FMOD_CHANNEL**>(appBaseAddr + 0x5DE3E0)[fmodChannelIndex];
        }
        GUI_Input<uint64>("FMOD 通道地址", *reinterpret_cast<uint64*>(&fmodChannelAddr), 0, "%llX",
            ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue);
        GUI_Input<float>("音量", volume, 0.1f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
        ImGui::PopItemWidth();
        ImGui::Text("");

        if (GUI_Button("SetVolume")) {
            fmodResult = FMOD_Channel_SetVolume(fmodChannelAddr, volume);
        }

        ImGui::Text("fmodResult %X", fmodResult);


        static byte8* headMetadataAddr = 0;

        ImGui::PushItemWidth(200);
        GUI_Input<uint64>("头部元数据地址", *reinterpret_cast<uint64*>(&headMetadataAddr), 0, "%llX",
            ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue);
        ImGui::PopItemWidth();
        ImGui::Text("");

        [&]() {
            if (!headMetadataAddr) {
                return;
            }

            auto& headMetadata = *reinterpret_cast<HeadMetadata*>(headMetadataAddr);

            auto& vagiMetadata = *reinterpret_cast<VagiMetadata*>(headMetadataAddr + headMetadata.vagiMetadataOff);


            if (ImGui::CollapsingHeader("Vagi")) {
                ImGui::Text("");

                ImGui::PushItemWidth(200);

                GUI_Input<uint32>(
                    "size", vagiMetadata.size, 0, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue);
                GUI_Input<uint32>("last", vagiMetadata.last, 0);

                ImGui::Text("");

                auto itemCount = (vagiMetadata.last + 1);

                ImGui::Text("itemCount %u", itemCount);

                old_for_all(uint32, itemIndex, itemCount) {
                    auto& item = vagiMetadata.items[itemIndex];

                    ImGui::Text("");

                    ImGui::Text("%u", itemIndex);
                    GUI_Input<uint32>(
                        "off", item.off, 0, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue);
                    GUI_Input<uint32>(
                        "size", item.size, 0, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue);
                    GUI_Input<uint32>("sampleRate", item.sampleRate, 1000, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
                }

                ImGui::PopItemWidth();

                ImGui::Text("");
            }
        }();

        ImGui::Text("");
    }
    ImGui::End();
}


void DebugSection() {
	auto defaultFontSize = UI::g_UIContext.DefaultFontSize;
	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);

	GUI_Title("调试选项");

	ImGui::PushItemWidth(itemWidth);
	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

	float smallerComboMult = 0.7f;

	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale * 0.5f;

		if (ImGui::BeginTable("DebugCheatsOptionsTable", 3)) {

			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			if (GUI_Checkbox2("调试工具", activeCrimsonGameplay.Debug.debugTools, queuedCrimsonGameplay.Debug.debugTools)) {
				if (activeCrimsonGameplay.Debug.debugTools) {
					activeConfig.debugOverlayData.enable = true;
					queuedConfig.debugOverlayData.enable = true;
				} else {
					activeConfig.debugOverlayData.enable = false;
					queuedConfig.debugOverlayData.enable = false;
				}
			}

			if (!activeCrimsonGameplay.Debug.debugTools) {
				ImGui::EndTable();
				ImGui::PopStyleColor();
				ImGui::PopFont();
				ImGui::PopItemWidth();
				ImGui::Text("");
				return;
			}
			
			ImGui::TableNextColumn();
			GUI_Checkbox2("调试叠加层", activeConfig.debugOverlayData.enable, queuedConfig.debugOverlayData.enable);
			ImGui::EndTable();
		}
	}

	

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);
	if (ImGui::CollapsingHeader("调试")) {

		ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
		

		ImGui::Text("");

		GUI_Checkbox2("显示判定框", activeCrimsonGameplay.Debug.showHitboxes, queuedCrimsonGameplay.Debug.showHitboxes);

		bool condition = (!InGame() || activeConfig.Actor.playerCount == 1);
		GUI_PushDisable(condition);
		for_each(players, 1, activeConfig.Actor.playerCount) {
			bool isDead = CrimsonGameplay::CanBeRevived(players);
			//GUI_PushDisable(!isDead);
			std::string inputLabel = "复活玩家 " + std::to_string(players + 1) + "##playerReviveLabel";
			if (GUI_Button(inputLabel.c_str())) {
				CrimsonGameplay::RevivePlayer(players);
			}

			//GUI_PopDisable(!isDead);
		}



		GUI_PopDisable(condition);
		if (ImGui::Button("heheh")) {
			CrimsonDetours::SampleModDetour1();
		}

		CrimsonEfk::EffekDrawImgui();
		// if (ImGui::Button("EnableCrazyComboHook")){
		//	HoldToCrazyComboHook->Toggle(true);
		// }

		// Move these variables if you wanted
		static int vfxBank = 0;
		static int vfxId = 0;
		static int boneIdx = 0;
		static bool enableCustomColor = false;

		ImGui::InputInt("效果库", &vfxBank);
		ImGui::InputInt("效果 ID", &vfxId);
		ImGui::InputInt("效果骨骼", &boneIdx);
		static int effectPlayerID = 0;
		ImGui::InputInt("玩家", &effectPlayerID);
		ImGui::Checkbox("CustomColor", &enableCustomColor);
		auto pPlayer = (void*)crimsonPlayer[effectPlayerID].playerPtr;
		uint32 vfxColor = CrimsonUtil::Uint8toAABBGGRR(activeCrimsonConfig.StyleSwitchFX.Flux.fluxColor[0]);
		if (ImGui::Button("CreateEffect")) {
			CrimsonDetours::CreateEffectDetour(pPlayer, vfxBank, vfxId, boneIdx, enableCustomColor, vfxColor, 1);
		}

		ImGui::Text("");

		GUI_Checkbox2("JF 的原始 JDC 特效参考", activeCrimsonConfig.VFX.originalJDCReference,
			queuedCrimsonConfig.VFX.originalJDCReference);

		GUI_Checkbox("g_noTeleport", g_noTeleport);


		GUI_Checkbox("g_showShop", g_showShop);

		GUI_Input<float>("saveTimeout", GUI::saveTimeout, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);


		{
			static int32 group = 0;
			static int32 index = 0;

			ImGui::PushItemWidth(150);
			GUI_Input<int32>("group", group, 1, "%d", ImGuiInputTextFlags_EnterReturnsTrue);
			GUI_Input<int32>("index", index, 1, "%d", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();
			ImGui::Text("");

			if (GUI_Button("播放音效")) {
				FMOD_PlaySound(group, index);
			}
		}

		ImGui::Text("");


		GUI_Checkbox2("欢迎", activeConfig.welcome, queuedConfig.welcome);
		ImGui::Text("");


		if (GUI_Button("实体")) {
			showActorWindow = true;
		}
		ImGui::SameLine();

		if (GUI_Button("EventData")) {
			showEventDataWindow = true;
		}
		ImGui::SameLine();

		if (GUI_Button("经验内容")) {
			showExpWindow = true;
		}
		ImGui::SameLine();

		if (GUI_Button("文件数据")) {
			showFileDataWindow = true;
		}
		ImGui::SameLine();

		if (GUI_Button("任务数据")) {
			showMissionDataWindow = true;
		}
		ImGui::SameLine();

		if (GUI_Button("区域数据")) {
			showRegionDataWindow = true;
		}
		ImGui::SameLine();

		if (GUI_Button("音效")) {
			showSoundWindow = true;
		}
		ImGui::Text("");

		if (GUI_Button("全部关闭")) {
			showActorWindow = false;
			showEventDataWindow = false;
			showExpWindow = false;
			showFileDataWindow = false;
			showMissionDataWindow = false;
			showRegionDataWindow = false;
			showSoundWindow = false;
		}
		ImGui::Text("");


		auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);


		GUI_Checkbox("一击必杀", sessionData.oneHitKill);
		ImGui::Text("");

		GUI_Checkbox("unlockDevilTrigger", sessionData.unlockDevilTrigger);
		ImGui::SameLine();							
		if (GUI_Button("修复魔人化")) {
			sessionData.unlockDevilTrigger = true;
			sessionData.magicPoints = 3000;
		}
		ImGui::SameLine();
		TooltipHelper("(?)", "作用于 sessionData，需重启任务才能生效。");
		ImGui::Text("");


		ImGui::Text("");

		if (GUI_Checkbox2(
			"禁用玩家实体待机计时器", activeConfig.disablePlayerActorIdleTimer, queuedConfig.disablePlayerActorIdleTimer)) {
			ToggleDisablePlayerActorIdleTimer(activeConfig.disablePlayerActorIdleTimer);
		}
		ImGui::Text("");


		{
			static ModelData* modelDataAddr = 0;
			static BodyPartData* bodyPartDataAddr = 0;
			static NewArchiveMetadata* archiveAddr = 0;

			static uint16 cacheFileIndex = em034;

			static char buffer[8];
			static byte8* file = 0;
			static uint32 fileIndex = 0;

			static char subbuffer[8];
			static byte8* subfile = 0;
			static uint32 subfileIndex = 0;


			GUI_Input<byte64>("modelDataAddr", *reinterpret_cast<byte64*>(&modelDataAddr), 0, "%llX",
				ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsHexadecimal);

			GUI_Input<byte64>("bodyPartDataAddr", *reinterpret_cast<byte64*>(&bodyPartDataAddr), 0, "%llX",
				ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsHexadecimal);

			GUI_Input<byte64>("archiveAddr", *reinterpret_cast<byte64*>(&archiveAddr), 0, "%llX",
				ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsHexadecimal);


			// @Todo: Set Archive.

			GUI_Input<uint16>("cacheFileIndex", cacheFileIndex, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);


			ImGui::PushItemWidth(50);
			ImGui::InputText("", buffer, sizeof(buffer));
			ImGui::PopItemWidth();
			ImGui::SameLine();

			ImGui::PushItemWidth(150);
			GUI_Input<byte64>("", *reinterpret_cast<byte64*>(&file), 0, "%llX",
				ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::SameLine();
			GUI_Input<uint32>("", fileIndex);
			ImGui::PopItemWidth();
			ImGui::SameLine();
			if (GUI_Button("设置文件")) {
				[&]() {
					if (!archiveAddr) {
						return;
					}
					auto& archive = *archiveAddr;


					file = archive[fileIndex];

					SetMemory(buffer, 0, sizeof(buffer));


					if (!file) {
						return;
					}

					CopyMemory(buffer, file, 4);
					}();
			}


			ImGui::PushItemWidth(50);
			ImGui::InputText("", subbuffer, sizeof(subbuffer));
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::PushItemWidth(150);
			GUI_Input<uint64>("", *reinterpret_cast<uint64*>(&subfile), 0, "%llX",
				ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::SameLine();
			GUI_Input<uint32>("", subfileIndex);
			ImGui::PopItemWidth();
			ImGui::SameLine();
			// @Todo: Update.
			if (GUI_Button("设置子文件")) {
				[&]() {
					auto subarchive = file;
					if (!subarchive) {
						return;
					}

					auto& subarchiveMetadata = *reinterpret_cast<ArchiveMetadata*>(subarchive);

					if ((subarchiveMetadata.signature[0] != 'P') || (subarchiveMetadata.signature[1] != 'A') ||
						(subarchiveMetadata.signature[2] != 'C')) {
						MessageBoxA(0, "签名错误", 0, 0);

						return;
					}

					if (subfileIndex >= subarchiveMetadata.fileCount) {
						MessageBoxA(0, "超出范围。", 0, 0);

						return;
					}

					auto subfileOff = subarchiveMetadata.fileOffs[subfileIndex];
					if (!subfileOff) {
						return;
					}

					subfile = (file + subfileOff);

					SetMemory(subbuffer, 0, sizeof(subbuffer));

					if (!subfile) {
						return;
					}

					CopyMemory(subbuffer, (subfile + 4), 3);
					}();
			}


			if (GUI_Button("播放文件动作")) {
				[&]() {
					// IntroduceMainActorData(actorBaseAddr, actorData, return);
					if (!modelDataAddr) {
						return;
					}

					func_8AC80(
						// actorData.newModelData[0],
						*modelDataAddr, UPPER_BODY, file, 0, false);
					func_8AC80(
						// actorData.newModelData[0],
						*modelDataAddr, LOWER_BODY, file, 0, false);
					}();
			}

			if (GUI_Button("播放子文件动作")) {
				[&]() {
					if (!modelDataAddr) {
						return;
					}
					// IntroduceMainActorData(actorBaseAddr, actorData, return);

					func_8AC80(
						// actorData.newModelData[0],
						*modelDataAddr, UPPER_BODY, subfile, 0, false);
					func_8AC80(
						// actorData.newModelData[0],
						*modelDataAddr, LOWER_BODY, subfile, 0, false);

					// auto bodyPartData2

					auto& done1 = *reinterpret_cast<bool*>(reinterpret_cast<byte8*>(&bodyPartDataAddr[0]) + 0xBA) = false;
					auto& done2 = *reinterpret_cast<bool*>(reinterpret_cast<byte8*>(&bodyPartDataAddr[1]) + 0xBA) = false;
					}();
			}
		}


		{
			static vec4 __declspec(align(16)) position = {};
			static vec4 __declspec(align(16)) multiplier = {};
			static __m128 __declspec(align(16)) height = {};


			GUI_Input("position x", position.x, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
			GUI_Input("position y", position.y, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
			GUI_Input("position z", position.z, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
			GUI_Input("position a", position.a, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);

			GUI_Input("multiplier x", multiplier.x, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
			GUI_Input("multiplier y", multiplier.y, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
			GUI_Input("multiplier z", multiplier.z, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
			GUI_Input("multiplier a", multiplier.a, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);


			GUI_Input("height x", height.m128_f32[0], 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
			GUI_Input("height y", height.m128_f32[1], 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
			GUI_Input("height z", height.m128_f32[2], 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
			GUI_Input("height a", height.m128_f32[3], 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);


			if (GUI_Button("手雷")) {
				// HoboBreak();

				func_175210(&position, &multiplier, height);
			}
		}


		{
			static uint32 event = 0;
			static uint8 action = 0;
			static uint64 actorIndex = 0;

			GUI_Input<decltype(actorIndex)>("actorIndex", actorIndex, 1, "%llu", ImGuiInputTextFlags_EnterReturnsTrue);

			GUI_Input<uint32>("event", event, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);

			GUI_Input<decltype(action)>("action", action, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);


			if (GUI_Button("事件")) {

				[&]() {
					if (!g_playerActorBaseAddrs[actorIndex]) {
						return;
					}
					auto& actorData = *reinterpret_cast<PlayerActorData*>(g_playerActorBaseAddrs[actorIndex]);

					actorData.action = action;

					func_1E09D0(actorData, event);
					}();
			}
		}

		ImGui::PopFont();

		
	}

	ImGui::PopFont();


	ImGui::PopItemWidth();
	ImGui::PopFont();
	ImGui::PopStyleColor();
	ImGui::Text("");
    
}

#pragma endregion

#pragma region Enemy

void EnemySpawnerToolSection() {
	auto& defaultFontSize = UI::g_UIContext.DefaultFontSize;
	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);
	float smallerComboMult = 0.8f;

	if (GUI_TitleCheckbox2("敌人生成器工具", activeCrimsonGameplay.Cheats.General.enemySpawnerTool,
		queuedCrimsonGameplay.Cheats.General.enemySpawnerTool, false)) {
		if (!activeCrimsonGameplay.Cheats.General.enemySpawnerTool) {
			activeConfig.enemyAutoSpawn = false;
			queuedConfig.enemyAutoSpawn = false;
		}
		CopyMemory(&queuedConfig.enemyCount, &defaultConfig.enemyCount, sizeof(queuedConfig.enemyCount));
		CopyMemory(&activeConfig.enemyCount, &queuedConfig.enemyCount, sizeof(activeConfig.enemyCount));

		CopyMemory(&queuedConfig.configCreateEnemyActorData, &defaultConfig.configCreateEnemyActorData, sizeof(queuedConfig.configCreateEnemyActorData));
		CopyMemory(&activeConfig.configCreateEnemyActorData, &queuedConfig.configCreateEnemyActorData, sizeof(activeConfig.configCreateEnemyActorData));

		CopyMemory(&queuedConfig.enemyAutoSpawn, &defaultConfig.enemyAutoSpawn, sizeof(queuedConfig.enemyAutoSpawn));
		CopyMemory(&activeConfig.enemyAutoSpawn, &queuedConfig.enemyAutoSpawn, sizeof(activeConfig.enemyAutoSpawn));
	}

	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);
	GUI_PushDisable(!activeCrimsonGameplay.Cheats.General.enemySpawnerTool);

	const float columnWidth = 0.48f * queuedConfig.globalScale;
	const float rowHeight = 40.0f * queuedConfig.globalScale;

	if (ImGui::BeginTable("EnemySpawnerTable", 2)) {
		ImGui::TableSetupColumn("左", 0, columnWidth * 2.0f);
		ImGui::TableSetupColumn("右", 0, columnWidth * 2.0f);

		ImGui::TableNextRow(0, rowHeight);
		ImGui::TableNextColumn();
		GUI_Slider2<uint8>("敌人数量", activeConfig.enemyCount, queuedConfig.enemyCount, 1, static_cast<uint8>(countof(activeConfig.configCreateEnemyActorData)));

		ImGui::TableNextColumn();
		GUI_Checkbox2("自动生成", activeConfig.enemyAutoSpawn, queuedConfig.enemyAutoSpawn);

		ImGui::TableNextRow(0, rowHeight * 0.9f);
		ImGui::TableNextColumn();
		if (GUI_Button("全部生成", ImVec2(150.0f * scaleFactorY, 30.0f * scaleFactorY))) {
			old_for_all(uint8, index, activeConfig.enemyCount) {
				auto& data = activeConfig.configCreateEnemyActorData[index];
				CreateEnemyActor(data);
			}
		}

		ImGui::TableNextColumn();
		if (GUI_Button("击杀所有蕾蒂")) {
			auto pool_9347 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
			if (pool_9347 && pool_9347[8]) {
				auto& enemyVectorData = *reinterpret_cast<EnemyVectorData*>(pool_9347[8]);
				old_for_all(uint32, enemyIndex, countof(enemyVectorData.metadata)) {
					auto& metadata = enemyVectorData.metadata[enemyIndex];
					if (!metadata.baseAddr) continue;

					auto& actor = *reinterpret_cast<EnemyActorDataLady*>(metadata.baseAddr);
					if (!actor.baseAddr || actor.enemy != ENEMY::LADY) continue;

					actor.event = EVENT_BOSS_LADY::DEATH;
					actor.state = 0;
					actor.friendly = false;
				}
			}
		}
		ImGui::EndTable();
	}

	ImGui::Spacing();

	// Enemy Entries
	ImGui::PushItemWidth(200);
	if (ImGui::BeginTable("EnemySpawnerTable2", 3)) {
		ImGui::TableSetupColumn("左", 0, columnWidth * 2.0f);
		ImGui::TableSetupColumn("右", 0, columnWidth * 2.0f);

		ImGui::TableNextRow(0, rowHeight);
		ImGui::TableNextColumn();
		old_for_all(uint8, index, activeConfig.enemyCount) {
			auto& activeData = activeConfig.configCreateEnemyActorData[index];
			auto& queuedData = queuedConfig.configCreateEnemyActorData[index];

			ImGui::PushID(index);

			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("敌人槽位 %u", index + 1);
			ImGui::PopFont();

			ImGui::PushItemWidth(itemWidth * 1.7f);
			UI::Combo2("敌人", enemyNames, activeData.enemy, queuedData.enemy, ImGuiComboFlags_HeightLarge);
			ImGui::PopItemWidth();

			ImGui::PushItemWidth(itemWidth * 0.9f);
			GUI_Input2<uint32>("变体", activeData.variant, queuedData.variant, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();
			GUI_Checkbox2("使用 1P 角色位置", activeData.useMainActorData, queuedData.useMainActorData);

			bool disablePos = activeData.useMainActorData;
			GUI_PushDisable(disablePos);

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_Input2<float>("X", activeData.position.x, queuedData.position.x, 10.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();
			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_Input2<float>("Y", activeData.position.y, queuedData.position.y, 10.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();
			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_Input2<float>("Z", activeData.position.z, queuedData.position.z, 10.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();
			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_Input2<uint16>("旋转", activeData.rotation, queuedData.rotation, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

			GUI_PopDisable(disablePos);

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_Input2<uint16>("生成方式", activeData.spawnMethod, queuedData.spawnMethod, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

			if (GUI_Button("生成", ImVec2(150.0f * scaleFactorY, 50.0f * scaleFactorY))) {
				CreateEnemyActor(activeData);
			}

			ImGui::PopID();
			ImGui::TableNextColumn();
		}

		ImGui::EndTable();
	}
	ImGui::PopItemWidth();

	GUI_PopDisable(!activeCrimsonGameplay.Cheats.General.enemySpawnerTool);
	ImGui::PopStyleColor();
	ImGui::PopFont();
}


#pragma endregion

#pragma region Jukebox

void FMODJukeboxSection() {
	auto& defaultFontSize = UI::g_UIContext.DefaultFontSize;
	float smallerComboMult = 0.8f;
	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);

	GUI_Title("FMOD 点唱机工具", false);

	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

	static char location[512];
	static char fileName[256];
	static new_size_t index = 0;
	static bool run = false;

	if (!run) {
		run = true;
		snprintf(fileName, sizeof(fileName), "%s", trackFilenames[index]);
	}

	const float columnWidth = 0.5f * queuedConfig.globalScale;
	const float rowHeight = 40.0f * queuedConfig.globalScale;

	if (ImGui::BeginTable("JukeboxTable", 2)) {
		ImGui::TableSetupColumn("c1", 0, columnWidth * 2.0f);
		ImGui::TableSetupColumn("c2", 0, columnWidth * 2.0f);

		ImGui::TableNextRow(0, rowHeight * 0.75f);
		ImGui::TableNextColumn();
		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
		ImGui::Text("曲目选择");
		ImGui::PopFont();

		ImGui::TableNextColumn();
		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
		ImGui::Text("控制");
		ImGui::PopFont();

		ImGui::TableNextRow(0, rowHeight);
		ImGui::TableNextColumn();

		ImGui::PushItemWidth(itemWidth * smallerComboMult);
		ImGui::InputText("##Filename", fileName, sizeof(fileName));
		ImGui::PopItemWidth();

		ImGui::PushItemWidth(itemWidth * 1.3f);
		if (UI::Combo("", trackNames, index, ImGuiComboFlags_HeightLarge)) {
			snprintf(fileName, sizeof(fileName), "%s", trackFilenames[index]);
		}
		ImGui::PopItemWidth();

		ImGui::TableNextColumn();

		if (GUI_Button("播放", ImVec2(150.0f * scaleFactorY, 50.0f * scaleFactorY))) {
			snprintf(location, sizeof(location), "afs/sound/%s", fileName);
			PlayTrack(location);
		}

		if (GUI_Button("停止", ImVec2(150.0f * scaleFactorY, 50.0f * scaleFactorY))) {
			PlayTrack("");
		}

		ImGui::EndTable();
	}

	ImGui::PopStyleColor();
	ImGui::PopFont();
}

#pragma endregion

#pragma region Lady

void LegacyDDMKCharactersSection() {
	auto defaultFontSize = UI::g_UIContext.DefaultFontSize;
	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);

	ImGui::Text("");

	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);

	GUI_PushDisable(!activeConfig.Actor.enable);
	if (GUI_TitleCheckbox2("旧版 DDMK 角色", activeCrimsonGameplay.Cheats.General.legacyDDMKCharacters,
		queuedCrimsonGameplay.Cheats.General.legacyDDMKCharacters, true, true, "These are incomplete playable characters adapted from their respective bosses in DDMK.\n"
		"使其可在 玩家 -> 角色 中选择。")) {
		if (!activeCrimsonGameplay.Cheats.General.legacyDDMKCharacters) {
			CopyMemory(&queuedConfig.kalinaAnnHookMultiplier, &defaultConfig.kalinaAnnHookMultiplier,
				sizeof(queuedConfig.kalinaAnnHookMultiplier));
			CopyMemory(&activeConfig.kalinaAnnHookMultiplier, &queuedConfig.kalinaAnnHookMultiplier,
				sizeof(activeConfig.kalinaAnnHookMultiplier));

			CopyMemory(&queuedConfig.kalinaAnnHookGrenadeHeight, &defaultConfig.kalinaAnnHookGrenadeHeight,
				sizeof(queuedConfig.kalinaAnnHookGrenadeHeight));
			CopyMemory(&activeConfig.kalinaAnnHookGrenadeHeight, &queuedConfig.kalinaAnnHookGrenadeHeight,
				sizeof(activeConfig.kalinaAnnHookGrenadeHeight));

			CopyMemory(&queuedConfig.kalinaAnnHookGrenadeTime, &defaultConfig.kalinaAnnHookGrenadeTime,
				sizeof(queuedConfig.kalinaAnnHookGrenadeTime));
			CopyMemory(&activeConfig.kalinaAnnHookGrenadeTime, &queuedConfig.kalinaAnnHookGrenadeTime,
				sizeof(activeConfig.kalinaAnnHookGrenadeTime));

			CopyMemory(&queuedConfig.enableBossVergilFixes, &defaultConfig.enableBossVergilFixes,
				sizeof(queuedConfig.enableBossVergilFixes));

			CopyMemory(&queuedConfig.enableBossLadyFixes, &defaultConfig.enableBossLadyFixes,
				sizeof(queuedConfig.enableBossLadyFixes));
			
		}
	}

	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);
	ImGui::PushItemWidth(itemWidth * 0.5f);

	// Helper for Lady Options
	auto LadyInput = [](const char* label, float& active, float& queued, float& defaultVar, float step = 1.0f) {
		const float smallerComboMult = 0.9f;
		ImGui::PushItemWidth(itemWidth * smallerComboMult);
		GUI_InputDefault2(label, active, queued, defaultVar, step, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::PopItemWidth();
		};

	GUI_PushDisable(!activeCrimsonGameplay.Cheats.General.legacyDDMKCharacters);
	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
	ImGui::Text("蕾蒂选项");
	ImGui::PopFont();

	// Table layout for Lady's Kalina Ann options
	const float columnWidth = 0.5f * queuedConfig.globalScale;
	const float rowHeight = 40.0f * queuedConfig.globalScale * 0.5f;

	if (ImGui::BeginTable("LadyOptionsTable", 3)) {
		ImGui::TableSetupColumn("col1", 0, columnWidth * 2.0f);
		ImGui::TableSetupColumn("col2", 0, columnWidth * 2.0f);

		ImGui::TableNextRow(0, rowHeight);
		ImGui::TableNextColumn();
		LadyInput("钩索倍率 X", activeConfig.kalinaAnnHookMultiplier.x,
			queuedConfig.kalinaAnnHookMultiplier.x, defaultConfig.kalinaAnnHookMultiplier.x);

		ImGui::TableNextColumn();
		LadyInput("钩索倍率 Y", activeConfig.kalinaAnnHookMultiplier.y,
			queuedConfig.kalinaAnnHookMultiplier.y, defaultConfig.kalinaAnnHookMultiplier.y);

		ImGui::TableNextColumn();
		LadyInput("钩索倍率 Z", activeConfig.kalinaAnnHookMultiplier.z,
			queuedConfig.kalinaAnnHookMultiplier.z, defaultConfig.kalinaAnnHookMultiplier.z);

		ImGui::TableNextColumn();
		LadyInput("钩索倍率 A", activeConfig.kalinaAnnHookMultiplier.a,
			queuedConfig.kalinaAnnHookMultiplier.a, defaultConfig.kalinaAnnHookMultiplier.a);

		ImGui::TableNextColumn();
		LadyInput("钩索手雷高度", activeConfig.kalinaAnnHookGrenadeHeight,
			queuedConfig.kalinaAnnHookGrenadeHeight, defaultConfig.kalinaAnnHookGrenadeHeight, 10.0f);

		ImGui::TableNextColumn();
		LadyInput("钩索手雷时间", activeConfig.kalinaAnnHookGrenadeTime,
			queuedConfig.kalinaAnnHookGrenadeTime, defaultConfig.kalinaAnnHookGrenadeTime, 10.0f);

		ImGui::EndTable();
	}

	ImGui::Text("");
	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
	ImGui::Text("角色修复");
	ImGui::PopFont();

	if (ImGui::BeginTable("CharacterFixesTable", 3)) {
		ImGui::TableSetupColumn("col1", 0, columnWidth * 2.0f);
		ImGui::TableSetupColumn("col2", 0, columnWidth * 2.0f);

		ImGui::TableNextRow(0, rowHeight);
		ImGui::TableNextColumn();
		if (GUI_Checkbox2("首领蕾蒂修复", activeConfig.enableBossLadyFixes, queuedConfig.enableBossLadyFixes)) {
			ToggleBossLadyFixes(activeConfig.enableBossLadyFixes);
		}
		ImGui::SameLine();
		TooltipHelper("(?) WARNING", "Allows you to more properly test Legacy Playable Boss Lady.\n"
			"警告：此选项会破坏真正的首领蕾蒂（敌人），请谨慎使用。", 2048.0f, true);

		ImGui::TableNextColumn();


		if (GUI_Checkbox2("首领维吉尔修复", activeConfig.enableBossVergilFixes, queuedConfig.enableBossVergilFixes)) {
			ToggleBossVergilFixes(activeConfig.enableBossVergilFixes);
		}
		ImGui::SameLine();
		TooltipHelper("(?) WARNING", "Allows you to more properly test Legacy Playable Boss Vergil.\n"
			"警告：此选项会破坏真正的首领维吉尔（敌人），请谨慎使用。", 2048.0f, true);

		ImGui::EndTable();
	}

	GUI_PopDisable(!activeCrimsonGameplay.Cheats.General.legacyDDMKCharacters);

	ImGui::PopItemWidth();
	ImGui::PopStyleColor();
	ImGui::PopFont();
	GUI_PopDisable(!activeConfig.Actor.enable);

	ImGui::Text("");
}

#pragma endregion

#pragma region Mobility

void CustomMobilitySection() {
	auto defaultFontSize = UI::g_UIContext.DefaultFontSize;

	float smallerComboMult = 0.7f;
	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);
	GUI_PushDisable(!activeConfig.Actor.enable);
	//Gui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);
	ImGui::PushItemWidth(itemWidth);

	if (GUI_TitleCheckbox2("自定义移动", activeCrimsonGameplay.Cheats.General.customMobility,
		queuedCrimsonGameplay.Cheats.General.customMobility, true)) {
		if (!activeCrimsonGameplay.Cheats.General.customMobility) {
			CopyMemory(&queuedCrimsonGameplay.Cheats.Mobility, &defaultCrimsonGameplay.Cheats.Mobility, sizeof(queuedCrimsonGameplay.Cheats.Mobility));
			CopyMemory(&activeCrimsonGameplay.Cheats.Mobility, &queuedCrimsonGameplay.Cheats.Mobility, sizeof(activeCrimsonGameplay.Cheats.Mobility));

			if (activeCrimsonGameplay.Gameplay.Dante.dmc4Mobility) {
				queuedCrimsonGameplay.Cheats.Mobility.airHikeCount[1] = 2;
				queuedCrimsonGameplay.Cheats.Mobility.wallHikeCount[1] = 2;
				queuedCrimsonGameplay.Cheats.Mobility.skyStarCount[1] = 2;
				queuedCrimsonGameplay.Cheats.Mobility.danteAirTrickCount[1] = 2;

				activeCrimsonGameplay.Cheats.Mobility.airHikeCount[1] = 2;
				activeCrimsonGameplay.Cheats.Mobility.wallHikeCount[1] = 2;
				activeCrimsonGameplay.Cheats.Mobility.skyStarCount[1] = 2;
				activeCrimsonGameplay.Cheats.Mobility.danteAirTrickCount[1] = 2;
			}
		}
	}

	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);

	auto MobilityDataInput = [](const char* label, uint8(&active)[2], uint8(&queued)[2], uint8(&defaultVar)[2]) {
		auto defaultFontSize = UI::g_UIContext.DefaultFontSize;

		const float rowWidth = 40.0f * queuedConfig.globalScale * 0.5f;
		float smallerComboMult = 0.7f;

		for (size_t i = 0; i < 2; ++i) {
			if (i == 0) {
				ImGui::TableNextRow(0, rowWidth);
				ImGui::TableNextColumn();
			}

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_InputDefault2("", active[i], queued[i], defaultVar[i], (uint8)1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

			if (i == 0) {
				ImGui::TableNextColumn();
			} else {
				ImGui::SameLine();
				ImGui::Text(label);
			}
		}
		};

	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);
	{
		// Get initial cursor position for manual layout
		ImVec2 initialPos = ImGui::GetCursorPos();
		const float columnWidth = 0.15f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale * 0.5f;
		GUI_PushDisable(!activeCrimsonGameplay.Cheats.General.customMobility);

		ImGui::SetCursorPos(initialPos);
		// Start first table (left side)
		if (ImGui::BeginTable("MobilityOptionsTable1", 2)) {
			ImGui::TableSetupColumn("b1", 0, columnWidth);
			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("人类");
			ImGui::TableNextColumn();
			ImGui::Text("魔人化");
			ImGui::PopFont();

			// First half of mobility options
			MobilityDataInput("二段跳次数", activeCrimsonGameplay.Cheats.Mobility.airHikeCount, 
				queuedCrimsonGameplay.Cheats.Mobility.airHikeCount, defaultCrimsonGameplay.Cheats.Mobility.airHikeCount);
			MobilityDataInput("踩跳次数", activeCrimsonGameplay.Cheats.Mobility.kickJumpCount, 
				queuedCrimsonGameplay.Cheats.Mobility.kickJumpCount, defaultCrimsonGameplay.Cheats.Mobility.kickJumpCount);
			MobilityDataInput("蹬墙跳次数", activeCrimsonGameplay.Cheats.Mobility.wallHikeCount, 
				queuedCrimsonGameplay.Cheats.Mobility.wallHikeCount, defaultCrimsonGameplay.Cheats.Mobility.wallHikeCount);
			MobilityDataInput("冲刺次数", activeCrimsonGameplay.Cheats.Mobility.dashCount, 
				queuedCrimsonGameplay.Cheats.Mobility.dashCount, defaultCrimsonGameplay.Cheats.Mobility.dashCount);
			MobilityDataInput("空中冲刺次数", activeCrimsonGameplay.Cheats.Mobility.skyStarCount, 
				queuedCrimsonGameplay.Cheats.Mobility.skyStarCount, defaultCrimsonGameplay.Cheats.Mobility.skyStarCount);

			ImGui::EndTable();
		}

		// Add some space between the tables
		ImGui::SetCursorPos(ImVec2(initialPos.x + (420 * queuedConfig.globalScale * scaleFactorY), initialPos.y));

		// Start second table (right side)
		if (ImGui::BeginTable("MobilityOptionsTable2", 2)) {
			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.3f);
			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("人类");
			ImGui::TableNextColumn();
			ImGui::Text("魔人化");
			ImGui::PopFont();

			// Second half of mobility options
			MobilityDataInput("但丁空中骗术次数", activeCrimsonGameplay.Cheats.Mobility.danteAirTrickCount, 
				queuedCrimsonGameplay.Cheats.Mobility.danteAirTrickCount, defaultCrimsonGameplay.Cheats.Mobility.danteAirTrickCount);
			MobilityDataInput("维吉尔空中骗术次数", activeCrimsonGameplay.Cheats.Mobility.vergilAirTrickCount, 
				queuedCrimsonGameplay.Cheats.Mobility.vergilAirTrickCount, defaultCrimsonGameplay.Cheats.Mobility.vergilAirTrickCount);
			MobilityDataInput("上移次数", activeCrimsonGameplay.Cheats.Mobility.trickUpCount, 
				queuedCrimsonGameplay.Cheats.Mobility.trickUpCount, defaultCrimsonGameplay.Cheats.Mobility.trickUpCount);
			MobilityDataInput("下移次数", activeCrimsonGameplay.Cheats.Mobility.trickDownCount, 
				queuedCrimsonGameplay.Cheats.Mobility.trickDownCount, defaultCrimsonGameplay.Cheats.Mobility.trickDownCount);

			ImGui::EndTable();
		}

		GUI_PopDisable(!activeCrimsonGameplay.Cheats.General.customMobility);
	}

	//GUI_InputDefault2("", vars[0], vars2[0], defaultVars[0], step, format, flags);
	GUI_PopDisable(!activeConfig.Actor.enable);
	ImGui::PopItemWidth();
	ImGui::PopStyleColor();
	ImGui::PopFont();
	ImGui::Text("");
}


#pragma endregion

#pragma region MiscCheats

const char* dotShadowNames[] = {"默认", "禁用", "仅禁用玩家实体"};


void MiscCheatsSection() {
	auto defaultFontSize = UI::g_UIContext.DefaultFontSize;
	ImGui::Text("");
	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);

	if (GUI_TitleCheckbox2("其他作弊", activeCrimsonGameplay.Cheats.General.miscCheats,
		queuedCrimsonGameplay.Cheats.General.miscCheats, false)) {
		if (!activeCrimsonGameplay.Cheats.General.miscCheats) {
			CopyMemory(&queuedCrimsonGameplay.Cheats.Misc, &defaultCrimsonGameplay.Cheats.Misc, sizeof(queuedCrimsonGameplay.Cheats.Misc));
			CopyMemory(&activeCrimsonGameplay.Cheats.Misc, &queuedCrimsonGameplay.Cheats.Misc, sizeof(activeCrimsonGameplay.Cheats.Misc));
		}
	}
	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

	ImGui::PushItemWidth(itemWidth * 0.5f);

	auto MiscCheatInput = [](const char* label, float(&active), float(&queued), float(&defaultVar)) {
		auto defaultFontSize = UI::g_UIContext.DefaultFontSize;
		const float rowWidth = 40.0f * queuedConfig.globalScale * 0.5f;
		float smallerComboMult = 0.9f;
		
		ImGui::PushItemWidth(itemWidth * smallerComboMult);
		GUI_InputDefault2(label, active, queued, defaultVar, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::PopItemWidth();
			
		};

	const float columnWidth = 0.5f * queuedConfig.globalScale;
	const float rowHeight = 40.0f * queuedConfig.globalScale * 0.5f;

	GUI_PushDisable(!activeCrimsonGameplay.Cheats.General.miscCheats);

	if (ImGui::BeginTable("MiscCheatsOptionsTable", 3)) {
		ImGui::TableSetupColumn("col1", 0, columnWidth * 2.0f);
		ImGui::TableSetupColumn("col2", 0, columnWidth * 2.0f);

		ImGui::TableNextRow(0, rowHeight);
		ImGui::TableNextColumn();

		

		auto name_6975 = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E30);
		if (!name_6975) {
			
		}
		else {
			auto& missionData = *reinterpret_cast<MissionData*>(name_6975);
			ImGui::PushItemWidth(200);
			GUI_Input<uint32>("红魂", missionData.redOrbs, 1000, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
			GUI_Input<uint8>("金魂", missionData.itemCounts[ITEM::GOLD_ORB], 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
			GUI_Input<uint8>("小生命之星", missionData.itemCounts[ITEM::VITAL_STAR_SMALL], 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
			GUI_Input<uint8>("大生命之星", missionData.itemCounts[ITEM::VITAL_STAR_LARGE], 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
			GUI_Input<uint8>("魔力之星", missionData.itemCounts[ITEM::DEVIL_STAR], 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
			GUI_Input<uint8>("圣水", missionData.itemCounts[ITEM::HOLY_WATER], 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();
		}


		MiscCheatInput("水银消耗", activeCrimsonGameplay.Cheats.Misc.quicksilverDepletion, queuedCrimsonGameplay.Cheats.Misc.quicksilverDepletion,
			defaultCrimsonGameplay.Cheats.Misc.quicksilverDepletion);

		ImGui::TableNextColumn();
		MiscCheatInput("分身消耗", activeCrimsonGameplay.Cheats.Misc.doppelgangerDepletion, queuedCrimsonGameplay.Cheats.Misc.doppelgangerDepletion,
			defaultCrimsonGameplay.Cheats.Misc.doppelgangerDepletion);

		ImGui::TableNextColumn();
		MiscCheatInput("魔力消耗", activeCrimsonGameplay.Cheats.Misc.dTDepletion, queuedCrimsonGameplay.Cheats.Misc.dTDepletion,
			defaultCrimsonGameplay.Cheats.Misc.dTDepletion);

		ImGui::TableNextColumn();
		MiscCheatInput("魂吸取距离", activeCrimsonGameplay.Cheats.Misc.orbReach, queuedCrimsonGameplay.Cheats.Misc.orbReach,
			defaultCrimsonGameplay.Cheats.Misc.orbReach);

		ImGui::TableNextColumn();

		GUI_PushDisable(!activeConfig.Actor.enable);
		GUI_Checkbox2("重置动作状态", activeCrimsonGameplay.Cheats.Misc.resetMotionState, queuedCrimsonGameplay.Cheats.Misc.resetMotionState);
		ImGui::SameLine();
		GUI_CCSRequirementButton();
		ImGui::SameLine();
		GUI_LegacyButton();
		ImGui::SameLine();
		TooltipHelper("(?)", "Deprecated in favor of 'Improved Cancels'. Also called 'Remove Busy Flag'.\n"
			"按方向键任意方向可取消任何动作，会显著影响游戏平衡。");
		GUI_PopDisable(!activeConfig.Actor.enable);

		ImGui::EndTable();
	}

	GUI_PopDisable(!activeCrimsonGameplay.Cheats.General.miscCheats);

	ImGui::PopItemWidth();
	ImGui::PopStyleColor();
	ImGui::PopFont();

	ImGui::Text("");
}

#pragma endregion

#pragma region Interface

const char* mainOverlayLabel = "DebugOverlay";

void DebugOverlayWindow(size_t defaultFontSize) {
    
    auto Function = [&]() {
		ImGui::SetWindowFontScale(scaleFactorY);
        if (activeConfig.debugOverlayData.showFocus) {
            auto color = ImVec4(0, 1, 0, 1);
            if (GetForegroundWindow() != appWindow) {
                color = ImVec4(1, 0, 0, 1);
            }
            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::Text("专注");
            ImGui::PopStyleColor();
        }


        if (activeConfig.debugOverlayData.showFPS) {
            ImGui::Text("%.2f FPS", ImGui::GetIO().Framerate);
        }

        if (activeConfig.debugOverlayData.showSizes) {
            ImGui::Text("g_windowSize %g %g", g_windowSize.x, g_windowSize.y);
            ImGui::Text("g_clientSize %g %g", g_clientSize.x, g_clientSize.y);
            ImGui::Text("g_renderSize %g %g", g_renderSize.x, g_renderSize.y);
        }


        if (activeConfig.debugOverlayData.showFrameRateMultiplier) {
            ImGui::Text("g_frameRateMultiplier %g", g_frameRateMultiplier);
			ImGui::Text("g_FrameRateTimeMultiplier %g", g_FrameRateTimeMultiplier);
			ImGui::Text("g_FrameRateTimeMultiplierRounded %g", g_FrameRateTimeMultiplierRounded);
        }

        if (activeConfig.debugOverlayData.showFocus || activeConfig.debugOverlayData.showFPS || activeConfig.debugOverlayData.showSizes ||
            activeConfig.debugOverlayData.showFrameRateMultiplier) {
            ImGui::Text("");
        }

        if (activeConfig.debugOverlayData.showEventData) {
            if (g_scene >= SCENE::COUNT) {
                ImGui::Text("未知");
            } else {
                auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);
				auto& gamepad = GetGamepad(0);
                
				auto name_7058 = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E30);

				
				ImGui::Text("inMainMenu: %u", g_inMainMenu);
				ImGui::Text("inMissionResultScreen: %u", g_inMissionResultScreen);
                ImGui::Text(sceneNames[g_scene]);
                ImGui::Text("sessionData mission:  %u", sessionData.mission);
                ImGui::Text("场景：  %u", g_scene);
                ImGui::Text("TRACK PLAYING: %s", g_gameTrackPlaying.c_str());
				ImGui::Text("sessionData.expertise[1]: %x", sessionData.expertise[1]);

// 				for (int i = 0; i < 8; i++) {
// 					ImGui::Text("sessionData expertise[%u]:  %x", i, sessionData.expertise[i]);
// 				}
// 
// 				for (int i = 0; i < 5; i++) {
// 					ImGui::Text("sessionData rangedWeaponLevels[%u]:  %x", i, sessionData.rangedWeaponLevels[i]);
// 				}

//                 for (int i = 0; i < 14; i++) {
//                     ImGui::Text("sessionData unlock[%u] : %u", i, sessionData.weaponStyleUnlocks[i]);
//                 }
//                 ImGui::Text("SessionData 皇家护卫风格等级：%u", ExpConfig::missionExpDataDante.styleLevels[3]);
// 				ImGui::Text("Cerbus 已解锁会话？%u", sessionData.weaponAndStyleUnlocks[WEAPONANDSTYLEUNLOCKS::CERBERUS]);
// 				ImGui::Text("sessionData.quicksilver? %u", sessionData.weaponAndStyleUnlocks[WEAPONANDSTYLEUNLOCKS::QUICKSILVER]);
// 				ImGui::Text("sessionData.doppelganger? %u", sessionData.weaponAndStyleUnlocks[WEAPONANDSTYLEUNLOCKS::DOPPELGANGER]);
// 				ImGui::Text("sessionData.quicksilverLevel: %u", sessionData.styleLevels[STYLE::QUICKSILVER]);
// 				ImGui::Text("sessionData.doppelgangerLevel: %u", sessionData.styleLevels[STYLE::DOPPELGANGER]);
				ImGui::Text("GunUnlockedQtt：%u", weaponProgression.gunsUnlockedQtt);
				ImGui::Text("sessionData.character: %u", sessionData.character);
				ImGui::Text("sessionData.unlockDevilTrigger: %u", sessionData.unlockDevilTrigger);
				ImGui::Text("sessionData.magicPoints: %g", sessionData.magicPoints);
				ImGui::Text("queuedCharacterData.rangedWeaponCount: %u", queuedConfig.Actor.playerData[0].characterData[0][0].rangedWeaponCount);
// 				for (auto cheat : gameModeData.currentlyUsedCheats) {
// 					ImGui::Text("Cheat: %s", gameModeData.cheatsNames[cheat]);
// 				}
// 				for (auto cheat : gameModeData.missionUsedCheats) {
// 					ImGui::Text("Mission Cheat: %s", gameModeData.cheatsNames[cheat]);
// 				}
// 				for (int i = 0; i < weaponProgression.rangedWeaponIds.size(); i++) {
// 					ImGui::Text("RangedWeaponId[%u]：%u", i, weaponProgression.rangedWeaponIds[i]);
// 				}
//  				for (int i = 0; i < weaponProgression.meleeWeaponIds.size(); i++) {
// 					ImGui::Text("MeleeWeaponName[%u]: %s", i, weaponProgression.meleeWeaponNames[i]);
//  				}
// 				for (size_t i = 0; i < queuedConfig.Actor.playerData[0].characterData[0][0].rangedWeaponCount; i++) {
// 					ImGui::Text("RangedWeaponQeued[%u]：%u", i, queuedConfig.Actor.playerData[0].characterData[0][0].rangedWeapons[i]);
// 				} 
				
               // ImGui::Text("手柄风格键：%u", gamepad.buttons[0] & GetBinding(BINDING::STYLE_ACTION));
// 				ImGui::Text("activePreset: %u", activeCrimsonGameplay.GameMode.preset);
// 				ImGui::Text("queuedPreset: %u", queuedCrimsonGameplay.GameMode.preset);
//                 ImGui::Text("水银等级：%u", sessionData.styleLevels[4]);


                /*ImGui::Text("空中起跳：  %u", executingSkyLaunch);
                ImGui::Text("空中起跳追踪运行中：  %u", skyLaunchTrackerRunning);
                ImGui::Text("皇家释放：  %u", executingRoyalRelease);
                ImGui::Text("皇家释放追踪运行中：  %u", royalReleaseTrackerRunning);
                ImGui::Text("强制精准帧皇家释放：  %u", forcingJustFrameRoyalRelease);
                ImGui::Text("skyLaunchSetJustFrameTrue:  %u", skyLaunchSetJustFrameTrue);
                ImGui::Text("皇家释放已执行：  %u", royalReleaseExecuted);*/


                /*ImGui::Text("backtoforward Back:  %u", b2F.backCommand);
                ImGui::Text("backtoforward Back Buffer:  %g", b2F.backBuffer);
                ImGui::Text("backtoforward Forward:  %u", b2F.forwardCommand);
                ImGui::Text("backtoforward Forward Buffer:  %g", b2F.forwardBuffer);
                ImGui::Text("backtoforward Back Direction Changed:  %u", b2F.backDirectionChanged);
                ImGui::Text("backtoforward Forward Direction Changed:  %u", b2F.backDirectionChanged);*/

                // 					ImGui::Text("delayed Combo Effect timer:  %g", delayedComboFX.timer);
                // 					ImGui::Text("delayed Combo Effect Start Timer:  %u", delayedComboFX.startTimer);
                // 					ImGui::Text("delayed Combo Effect Duration:  %g", delayedComboFX.duration);
                // 					ImGui::Text("delayed Combo Effect Weapon:  %u",
                // delayedComboFX.weaponThatStartedMove); 					ImGui::Text("trickCancel Cooldown:  %u", trickUpCancel.cooldown);

                ImGui::Text("sprint Can Sprint:  %u", crimsonPlayer[0].sprint.canSprint);
                ImGui::Text("冲刺计时器：%g", crimsonPlayer[0].sprint.timer);
                ImGui::Text("冲刺跑计时器：%u", crimsonPlayer[0].sprint.runTimer);
				ImGui::Text("missionStyleLevels sword: %u", heldStyleExpDataDante.missionStyleLevels[0]);
				ImGui::Text("missionStyleLevels gun: %u", heldStyleExpDataDante.missionStyleLevels[1]);
				ImGui::Text("missionStyleLevels trick: %u", heldStyleExpDataDante.missionStyleLevels[2]);
				ImGui::Text("missionStyleLevels royal: %u", heldStyleExpDataDante.missionStyleLevels[3]);
				ImGui::Text("accumulatedStylePts sword: %u", heldStyleExpDataDante.accumulatedStylePoints[0]);


                if (CrimsonSDL::IsMusicPlaying() == 0) {
                    ImGui::Text("no music playing");
                } else {
                    ImGui::Text("音乐播放中");
                }
            }

            [&]() {
                if (g_scene != SCENE::GAME) {
                    return;
                }

				auto pool_10298 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
				if (!pool_10298 || !pool_10298[8]) {
					return;
				}
				auto& eventData = *reinterpret_cast<EventData*>(pool_10298[8]);

                auto pool_10329 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
                if (!pool_10329 || !pool_10329[12]) {
                    return;
                }
                auto& nextEventData = *reinterpret_cast<NextEventData*>(pool_10329[12]);


                if (eventData.event >= EVENT::COUNT) {
                    ImGui::Text("未知");
                } else {
                    ImGui::Text(eventNames[eventData.event]);
                }

				ImGui::Text("subEvent:  %u", eventData.subevent);

                ImGui::Text("");

                ImGui::Text("room         %u", eventData.room);
                ImGui::Text("position     %u", eventData.position);
                ImGui::Text("event	      %u", eventData.event);
                ImGui::Text("nextRoom     %u", nextEventData.room);
                ImGui::Text("nextPosition %u", nextEventData.position);
				ImGui::Text("镜头索引 %u", CrimsonCameraController::g_currentCameraIndex);
				ImGui::Text("镜头类型 %u", CrimsonCameraController::g_currentCameraType);
            }();

            ImGui::Text("");
        }
// 		if (activeConfig.Actor.enable) {
// 			[&]() {
// 				ImGui::Text("阿卡姆 CCS 战斗阶段			%u", arkhamFightData.fightPhase);
// 				ImGui::Text("阿卡姆 CCS 下一战斗阶段	%u", arkhamFightData.nextFightPhase);
// 				ImGui::Text("阿卡姆 CCS 战斗激活		%u", arkhamFightData.fightActive);
// 				ImGui::Text("阿卡姆 CCS 战斗结束		%u", arkhamFightData.fightEnding);
// 				}();
// 		}
        if (activeConfig.debugOverlayData.showPosition) {
            [&]() {
                auto pool_10213 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
                if (!pool_10213 || !pool_10213[3]) {
                    return;
                }
                auto& actorData = *reinterpret_cast<PlayerActorData*>(pool_10213[3]);

                ImGui::Text("X        %g", actorData.position.x);
                ImGui::Text("Y        %g", actorData.position.y);
                ImGui::Text("Z        %g", actorData.position.z);
                ImGui::Text("旋转 %u", actorData.rotation);
            }();


            auto pool_10222 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
            if (!pool_10222 || !pool_10222[3]) {
                return;
            }
            auto& actorData      = *reinterpret_cast<PlayerActorData*>(pool_10222[3]);
            auto& characterData  = GetCharacterData(actorData);
            auto& cloneActorData = *reinterpret_cast<PlayerActorData*>(actorData.cloneActorBaseAddr);
            auto& gamepad        = GetGamepad(0);
            auto tiltDirection   = GetRelativeTiltDirection(actorData);

            auto pool_10320 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
            if (!pool_10320 || !pool_10320[8]) {
                return;
            }
            auto& enemyVectorData = *reinterpret_cast<EnemyVectorData*>(pool_10320[8]);

            auto pool_10371 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
            if (!pool_10371 || !pool_10371[8]) {
                return;
            }
            auto& eventData = *reinterpret_cast<EventData*>(pool_10371[8]);

            auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);


            auto& metadata = enemyVectorData.metadata[0];


            auto pool_12857 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
            if (!pool_12857 || !pool_12857[3]) {
                return;
            }
            auto& mainActorData = *reinterpret_cast<PlayerActorDataDante*>(pool_12857[3]);

			auto name_7058 = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E30);

			auto& missionData = *reinterpret_cast<MissionData*>(name_7058);
            auto& queuedMissionActorData = *reinterpret_cast<QueuedMissionActorData*>(name_7058 + 0xC0);
			auto& activeMissionActorData = *reinterpret_cast<ActiveMissionActorData*>(name_7058 + 0x16C);

            auto name_80 = *reinterpret_cast<byte8**>(appBaseAddr + 0xCF2680);
            if (!name_80) {
                return;
            }
            auto& hudData = *reinterpret_cast<HUDData*>(name_80);

// 			if (!actorData.lockOnData.targetBaseAddr60) {
// 				return;
// 			}
// 
// 			auto& lockedOnEnemyData = *reinterpret_cast<EnemyActorData*>(actorData.lockOnData.targetBaseAddr60 - 0x60);

			//ImGui::Text("目标位置Z %g", actorData.lockOnData.targetPosition.z);

//             for (int i = 0; i < 8; i++) {
//                 ImGui::Text("expertise[%u]:  %x", i, actorData.activeExpertise[i]);
//             }
// 
// 			for (int i = 0; i < 10; i++) {
// 				ImGui::Text("actorData rangedWeaponLevels[%u]:  %x", i, actorData.newWeaponLevels[i]);
// 			}
			auto pool_4449 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC8FBD0);
			if (!pool_4449 || !pool_4449[147]) return;
			auto& cameraData = *reinterpret_cast<CameraData*>(pool_4449[147]); // 0x498 = 1176 / 8 = 147
			auto& cameraControlMetadata = *reinterpret_cast<CameraControlMetadata*>(pool_4449);

			auto& costume = activeConfig.Actor.playerData[0].characterData[0]->costume;

// 			auto& enemy = enemyVectorData.metadata[0];
// 			if (!enemy.baseAddr) return;
// 			auto& enemyData = *reinterpret_cast<EnemyActorData*>(enemy.baseAddr);
// 			if (!enemyData.baseAddr) return;
			
            // crazyComboHold = g_HoldToCrazyComboFuncA();
// 			if (meleeWeaponWheel[0]) {
// 				ImGui::Text("轮盘 0 计时: %g", meleeWeaponWheel[0]->m_SinceLatestChangeMs);
// 			}
// 			if (meleeWeaponWheel[1]) {
// 				ImGui::Text("轮盘 1 计时: %g", meleeWeaponWheel[1]->m_SinceLatestChangeMs);
// 			}
			auto savingInGameDataAddr = *reinterpret_cast<byte8**>(appBaseAddr + 0xCF2548);
			if (!savingInGameDataAddr) {
				return;
			}
			auto& savingInGameData = *reinterpret_cast<SavingInGameData*>(savingInGameDataAddr);

			ImGui::Text("spiralHoldTimer: %g", *reinterpret_cast<const float*>(actorData + 0x68F8));
			ImGui::Text("spiralSwordsActive: %u", *reinterpret_cast<bool*>(actorData + 0xB598));
			ImGui::Text("desiredFormation: %u", crimsonPlayer[0].swordFormationTracker.bufferedFormation);
			ImGui::Text("gracePeriodTimer: %g", crimsonPlayer[0].swordFormationTracker.gracePeriodTimer);
			ImGui::Text("formationBuffered: %u", crimsonPlayer[0].swordFormationTracker.formationBuffered);
			ImGui::Text("trickUpCount: %u", actorData.newTrickUpCount);
			ImGui::Text("水平拉力  %g", actorData.horizontalPull);
			ImGui::Text("水平拉力倍率 %g", actorData.horizontalPullMultiplier);
			ImGui::Text("垂直拉力  %g", actorData.verticalPull);
			ImGui::Text("垂直拉力倍率 %g", actorData.verticalPullMultiplier);
			ImGui::Text("action Timer Main Actor:  %g", crimsonPlayer[0].actionTimer);
			ImGui::Text("动作:  %u", actorData.action);
			ImGui::Text("移动部件 %u", actorData.recoverState[0]);
			ImGui::Text("cameraLag: %g", cameraData.cameraLag);
			ImGui::Text("fixedCamAddr: %x", cameraControlMetadata.fixedCameraAddr);
			ImGui::Text("in JustFrame JDC:  %u", crimsonPlayer[0].jCut.inJustFrameJDC);
			ImGui::Text("bufferedAction:  %u", actorData.bufferedAction);
			ImGui::Text("gunButtonTimer : % g", crimsonPlayer[0].bulletMagnetism.gunButtonTimer);
			ImGui::Text("inertiaRotation : % x", actorData.inertiaRotation);
			ImGui::Text("动作计时:  %g", crimsonPlayer[0].motionTimer);
			ImGui::Text("动作组:  %u", actorData.motionData[0].group);
			ImGui::Text("动作索引:  %u", actorData.motionData[0].index);
			ImGui::Text("drive inPart2 : %d", crimsonPlayer[0].drive.inPart2);
			ImGui::Text("drive inPart3 : %d", crimsonPlayer[0].drive.inPart3);
			ImGui::Text("yamatoHighTimeClone : %d", crimsonPlayer[0].inYamatoHighTimeClone);
			ImGui::Text("yamatoMotionState : %d", actorData.weaponMotionState[7]);
			ImGui::Text("airSwordAttackCount : %d", actorData.airSwordAttackCount);
			ImGui::Text("airGunAttackCount: %d", actorData.airGunAttackCount);
			ImGui::Text("inYamatoHighTime : %d", crimsonPlayer[0].inYamatoHighTime);
			ImGui::Text("drive levelTimer: %g", crimsonPlayer[0].drive.levelTimer);
			ImGui::Text("actor speed: %g", actorData.speed);
			ImGui::Text("inertia Rotation : %g", actorData.inertiaRotation);
			ImGui::Text("Jcut 近战保持时间:  %g", crimsonPlayer[0].jCut.meleeHoldTime);
			ImGui::Text("meleeHold jcut %g", crimsonPlayer[0].jCut.meleeButtonHold);
			ImGui::Text("meleeHold stinger %g", crimsonPlayer[0].stingerInput.meleeButtonHold);
			ImGui::Text("melee Released Stinger %u", crimsonPlayer[0].stingerInput.meleeReleasedStinger);
			ImGui::Text("action Timer Trick not change:  %g", crimsonPlayer[0].actionTimerNotTrickChange);
			ImGui::Text("事件数据 %u", actorData.eventData[0].event);
			ImGui::Text("Actor Data Permissions: %d", actorData.permissions);
			ImGui::Text("isAJustFrameCharged %d", crimsonPlayer[0].jCut.isJustFrameCharged);
			ImGui::Text("isAfterJustFrameCharged %d", crimsonPlayer[0].jCut.isAfterJustFrameCharged);
			ImGui::Text("动作数据 1: %u", crimsonPlayer[0].motion);
			ImGui::Text("characterData.weapon %d", characterData.meleeWeapons[characterData.meleeWeaponIndex]);
			ImGui::Text("Crazy announcer timer %g \ was hit %d", rankAnnouncer[1].timer, rankAnnouncer[1].wasHit);
			ImGui::Text("actorData event %d", actorData.eventData[0].event);
			ImGui::Text("actorData.state %d", actorData.state);
			ImGui::Text("air Counts Rising Sun %d", actorData.newAirRisingSunCount);
			ImGui::Text("inRisingStar %d", crimsonPlayer[0].inRisingStar);
			ImGui::Text("lastActionTime %g", crimsonPlayer[0].lastActionTime);
			ImGui::Text("actionTImerNotEventChange %g", crimsonPlayer[0].actionTimerNotEventChange);
			ImGui::Text("closeToEnemy %u", crimsonPlayer[0].isCloseToEnemy);
			ImGui::Text("guardTimer %g", actorData.guardTimer);
			ImGui::Text("skyLaunch Executing %u", crimsonPlayer[0].skyLaunch.executing);
			ImGui::Text("inertia airguard %g", crimsonPlayer[0].inertia.airGuard.cachedPull);
			ImGui::Text("mirageGauge %g", crimsonPlayer[0].vergilDoppelganger.miragePoints);
			ImGui::Text("maxMirageGauge %g", crimsonPlayer[0].vergilDoppelganger.maxMiragePoints);
			ImGui::Text("垂直拉力倍率 %g", actorData.verticalPullMultiplier);
			ImGui::Text("cameraHittingWall: %u", g_cameraHittingWall);
			ImGui::Text("actorData styleLevel: %u", actorData.styleLevel);
			ImGui::Text("frameCOunt Mission: %u", missionData.frameCount);
			ImGui::Text("heldStyleLevels sword: %u", heldStyleExpDataDante.accumulatedStyleLevels[0]);
			ImGui::Text("expDataStyleLevels sword: %u", ExpConfig::missionExpDataDante.styleLevels[0]);
			ImGui::Text("royalguardDamage: %g", actorData.royalguardReleaseDamage);
			ImGui::Text("styleExpPoints: %g", actorData.styleExpPoints);
			ImGui::Text("角色基址: %x", actorData.baseAddr);
			ImGui::Text("骗术冲刺计时: %g", crimsonPlayer[0].trickDashTimer);
			ImGui::Text("lockOnEnemyMinusStun: %g", crimsonPlayer[0].lockedOnEnemyMinusStun);
			ImGui::Text("lockOnEnemyMinusDisplacement: %g", crimsonPlayer[0].lockedOnEnemyMinusDisplacement);
			ImGui::Text("gameModeData.mustStyleMissionResult: %u", gameModeData.mustStyleMissionResult);
			ImGui::Text("gameModeData.enemyDTMisionResult: %u", gameModeData.enemyDTMissionResult);
			ImGui::Text("lockedEnemyScreenPositionX: %g", crimsonPlayer[0].lockedEnemyScreenPosition.x);
			ImGui::Text("lockedEnemyScreenPositionX: %g", actorData.lockOnData.targetPosition.x);
			ImGui::Text("lockOnEnemyStun: %g", crimsonPlayer[0].lockedOnEnemyStun);
			ImGui::Text("lockOnEnemyDisplacement: %g", crimsonPlayer[0].lockedOnEnemyDisplacement);
			ImGui::Text("lockOnEnemyHP: %g", crimsonPlayer[0].lockedOnEnemyHP);
			ImGui::Text("lockOnEnemyMaxHP: %g", crimsonPlayer[0].lockedOnEnemyMaxHP);
			ImGui::Text("savingInGameData.expertise[1]: %x", savingInGameData.expertise[1]);
			ImGui::Text("missionData frameCount: %u", missionData.frameCount);
			ImGui::Text("artemis Charge: %g", actorData.artemisCharge);
			ImGui::Text("fixedCameraAddr: %x", cameraControlMetadata.fixedCameraAddr);
			ImGui::Text("地面起步: %u", crimsonPlayer[0].vergilMoves.startingRisingSunFromGround);
// 			ImGui::Text("敌人速度: %g", enemyData.speed);
// 			ImGui::Text("敌人基址: %x", enemyData.baseAddr);
// 			ImGui::Text("敌人目标位置 Y: %g", enemyData.targetPosition.y);
			ImGui::Text("全部角色已生成: %u", g_allActorsSpawned);
			ImGui::Text("costume: %u", costume);
			ImGui::Text("下一动作策略·骗术师: %u", actorData.nextActionRequestPolicy[NEXT_ACTION_REQUEST_POLICY::TRICKSTER_DARK_SLAYER]);
			ImGui::Text("下一动作策略·近战: %u", actorData.nextActionRequestPolicy[NEXT_ACTION_REQUEST_POLICY::MELEE_ATTACK]);
			ImGui::Text("预输入动作: %u", actorData.bufferedAction);
			ImGui::Text("空中骗术次数 : %u", actorData.newAirTrickCount);
			ImGui::Text("rainstorm cached inertia:  %g", crimsonPlayer[0].inertia.rainstorm.cachedPull);
			ImGui::Text("空中剑击次数: %u", actorData.airSwordAttackCount);
			ImGui::Text("后退预输入: %g", crimsonPlayer[0].b2F.backBuffer);
			ImGui::Text("前进预输入: %g", crimsonPlayer[0].b2F.forwardBuffer);
			ImGui::Text("前进指令: %u", crimsonPlayer[0].b2F.forwardCommand);
			ImGui::Text("后退预输入·分身: %g", crimsonPlayer[0].b2FClone.backBuffer);
			ImGui::Text("前进预输入·分身: %g", crimsonPlayer[0].b2FClone.forwardBuffer);
			ImGui::Text("前进指令·分身: %u", crimsonPlayer[0].b2FClone.forwardCommand);
			ImGui::Text("皇家格挡: %u", actorData.royalBlock);
			ImGui::Text("防御: %u", actorData.guard);
			ImGui::Text("maxDT: %g", actorData.maxMagicPoints);
			ImGui::Text("anim Timer Clone:  %g", crimsonPlayer[0].motionTimerClone);
			ImGui::Text("action Timer Clone:  %g", crimsonPlayer[0].actionTimerClone);
			ImGui::Text("空中防御: %u", actorData.airGuard);
			ImGui::Text("防御飞行拉力: %g", crimsonPlayer[0].inertia.airGuard.cachedPull);
			ImGui::Text("防御飞行计时: %g", crimsonPlayer[0].inertia.guardflyTimer);
			ImGui::Text("actorCameraDirection: %u", actorData.actorCameraDirection);
			ImGui::Text("旋转偏移: %u", actorData.rotationOffset);
			ImGui::Text("朝向敌人2旋转: %u", crimsonPlayer[0].rotationTowardsEnemy);
			ImGui::Text("isMPActive:  %u", g_isMPCamActive);
			ImGui::Text("isPanoramicCamActive:  %u", g_isParanoramicCamActive);
			ImGui::Text("镜头延迟:  %g", cameraData.cameraLag);
			ImGui::Text("锁定镜头:  %u", cameraData.transitionToLockOnCam);
			ImGui::Text("角色速度:  %g", actorData.speed);
			ImGui::Text("全部实体数:  %u", g_activeAllEntitiesCount);
			ImGui::Text("可操作实体数:  %u", g_activePlayableEntitiesCount);
			ImGui::Text("avgPos x: %g", g_customCameraPos[0]);
			ImGui::Text("avgPos y: %g", g_customCameraPos[1]);
			ImGui::Text("avgPos z: %g", g_customCameraPos[2]);
			ImGui::Text("红魂透明度:  %u", crimsonHud.redOrbAlpha);
			ImGui::Text("空中起击执行中:  %u", crimsonPlayer[0].skyLaunch.executing);
			ImGui::Text("皇家释放执行中:  %u", crimsonPlayer[0].royalRelease.executing);
			ImGui::Text("蓄能释放等级:  %u", crimsonPlayer[0].skyLaunch.storedReleaseLevel);
			ImGui::Text("皇家计量:  %g", actorData.royalguardReleaseDamage);
            ImGui::Text("action Timer Main Actor:  %g", crimsonPlayer[0].actionTimer);
            ImGui::Text("DTE 充能: %g", actorData.dtExplosionCharge);
            ImGui::Text("DTE 充能: %g", crimsonPlayer[0].dtExplosionCharge);
            // ImGui::Text("crazy combo hold:  %u", crazyComboHold);
            ImGui::Text("连击数（权重）:  %u", actorData.airSwordAttackCount);
            ImGui::Text("空中防御:  %u", actorData.airGuard);
            // ImGui::Text("重力微调:  %g", crimsonPlayer[0].airRaveTweak.gravity);
            // ImGui::Text("drive timer:  %g", crimsonPlayer[0].drive.timer);
            // ImGui::Text("角色速度 %g", actorData.speed);
            ImGui::Text("左上透明度: %u", hudData.topLeftAlpha);
            ImGui::Text("distance: %u", crimsonPlayer[0].cameraPlayerDistanceClamped);
            ImGui::Text("释放伤害: %g", actorData.royalguardReleaseDamage);
			ImGui::Text("角色状态: %u", actorData.status);
            ImGui::Text("幻影计量: %g", crimsonPlayer[0].vergilDoppelganger.miragePoints);
            ImGui::Text("维吉尔分身计时启动: %u", crimsonPlayer[0].vergilDoppelganger.drainStart);
            ImGui::Text("维吉尔分身计时: %g", crimsonPlayer[0].vergilDoppelganger.drainTime);
            ImGui::Text("HP: %g", actorData.hitPoints);
            ImGui::Text("DT: %g", actorData.magicPoints);
            ImGui::Text("storedHP: %g", storedHP);
            ImGui::Text("storedDT: %g", storedDT);
            ImGui::Text("missionData DAMAGE: %u", missionData.damage);
            ImGui::Text("missionData FRAME: %u", missionData.frameCount);
            ImGui::Text("处于皇家格挡: %u", ((actorData.eventData[0].event == 20 && actorData.motionData[0].index == 32) || (actorData.eventData[0].event == 20 && actorData.motionData[0].index == 34)));
            ImGui::Text("处于普通格挡: %u", ((actorData.eventData[0].event == 20 && actorData.motionData[0].index == 2) || (actorData.eventData[0].event == 20 && actorData.motionData[0].index == 7) || (actorData.eventData[0].event == 20 && actorData.motionData[0].index == 12)));
            ImGui::Text("处于防御: %u", actorData.guard);
            ImGui::Text("处于战斗: %u", g_inCombat);
            ImGui::Text("时间扭曲: %g", crimsonPlayer[0].fluxtime);
            ImGui::Text("骗术师时间: %g", crimsonPlayer[0].styleSwitchText.time[0]);
            ImGui::Text("骗术师透明度: %g", crimsonPlayer[0].styleSwitchText.alpha[0]);
            // 					ImGui::Text("骗术冷却 %g", crimsonPlayer[1].cancels.trickCooldown);
            // 					ImGui::Text("枪械冷却 %g", crimsonPlayer[1].cancels.gunsCooldown);
            // 					ImGui::Text("弹雨冷却 %g", crimsonPlayer[1].cancels.rainstormCooldown);
            // ImGui::Text("武器 %u", actorData.newWeapons[actorData.meleeWeaponIndex]);
            ImGui::Text("远程武器 %u", actorData.newWeapons[actorData.rangedWeaponIndex]);
            ImGui::Text("实际近战武器 %u", characterData.lastMeleeWeaponIndex);
            ImGui::Text("阿尔忒弥斯状态 %u", mainActorData.artemisStatus);
            ImGui::Text("lock On: %u", crimsonPlayer[1].lockOn);
            ImGui::Text("tilt direction %u", crimsonPlayer[1].tiltDirection);
            ImGui::Text("敌人数量 %u", enemyVectorData.count);
            ImGui::Text("enemy distance %g", distanceToEnemy);
            // ImGui::Text("enemy vertical Pull Multiplier %g", enemyData.verticalPullMultiplier);
            ImGui::Text("rainstormCancel Cooldown %u", rainstormCancel.cooldown);
            ImGui::Text("处于皇家格挡:  %u", inRoyalBlock);
            ImGui::Text("枪击反向 %u", gunShootInverted);
            ImGui::Text("枪击归一化 %u", gunShootNormalized);

            ImGui::Text("new speed Set Rapid Slash %u", fasterRapidSlash.newSpeedSet);
            ImGui::Text("inRapidSlash %u", inRapidSlash);
            ImGui::Text("fRapidSlash storedSpeedDevil %g", fasterRapidSlash.storedSpeedDevil[0]);
            ImGui::Text("fDarklasyer storedSpeedDevil %g", fasterDarkslayer.storedSpeedDevil[0]);
            ImGui::Text("上一事件数据 %u", actorData.eventData[0].lastEvent);
            ImGui::Text("上上次事件 %u", crimsonPlayer[0].lastLastEvent);
            ImGui::Text("状态 %u", actorData.state);
            ImGui::Text("上一状态 %u", actorData.lastState);
            ImGui::Text("上上次状态 %u", crimsonPlayer[0].lastLastState);
            ImGui::Text("角色动作 %u", crimsonPlayer[0].action);
            ImGui::Text("角色上一动作 %u", actorData.lastAction);
            ImGui::Text("位置  %g", actorData.position);
            ImGui::Text("旋转 %g", actorData.rotation);
            ImGui::Text("镜头方向 %u", actorData.cameraDirection);
            ImGui::Text("角色镜头方向 %u", actorData.actorCameraDirection);
            ImGui::Text("相对倾斜 %u", relativeTiltController);
            ImGui::Text("左摇杆位置 %u", gamepad.leftStickPosition);
		
            ImGui::Text("皇家格挡类型:  %u", actorData.royalguardBlockType);
            ImGui::Text("防御飞行:  %u", inGuardfly);
            // ImGui::Text("Track %s", eventData.track);

            ImGui::Text("空中防御惯性  %g", crimsonPlayer[0].inertia.airGuard.cachedPull);
            ImGui::Text("武器角色数据 %u", characterData.meleeWeaponIndex);
            ImGui::Text("当前武器 %u", actorData.activeWeapon);
            ImGui::Text("上挑骗术次数 %u", actorData.newTrickUpCount);
            ImGui::Text("天星次数 %u", actorData.newSkyStarCount);
            ImGui::Text("二段跳次数 %u", actorData.newAirHikeCount);
            ImGui::Text("空中旭日次数 %u", actorData.newAirRisingSunCount);


            ImGui::Text("配置无限生命: %u", activeCrimsonGameplay.Cheats.Training.infiniteDT);

            /*ImGui::Text("styleRankCount Dismal %u", rankAnnouncer[0].count);
            ImGui::Text("styleRankCount Crazy %u", rankAnnouncer[1].count);
            ImGui::Text("styleRankCount Dismal Off Cooldown %u", rankAnnouncer[0].offCooldown);
            ImGui::Text("styleRankCount Crazy Off Cooldown %u", rankAnnouncer[1].offCooldown);*/

            /*ImGui::Text("SDL2 %s", SDL2Initialization);
            ImGui::Text("Mixer  %s", MixerInitialization);
            ImGui::Text("Mixer2  %s", MixerInitialization2);*/

            ImGui::Text("快速双击预输入 %u", quickDoubleTap[0].buffer);
            ImGui::Text("分身双击预输入 %u", doppDoubleTap[0].buffer);

            ImGui::Text("分身魔力值 %g", currentDTDoppOn);
            ImGui::Text("分身魔人魔力值 %g", currentDTDoppDTOn);
            ImGui::Text("分身毫秒 %g", doppSeconds);
            ImGui::Text("分身魔人毫秒 %g", doppSecondsDT);
            ImGui::Text("分身追踪运行中 %u", doppTimeTrackerRunning);
            ImGui::Text("styleVFXCount %u", styleVFXCount);


            using namespace ACTION_DANTE;
            using namespace ACTION_VERGIL;

            auto name_10438 = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E30);
            if (!name_10438) {
                return;
            }
           


            ImGui::Text("分身激活 %u", actorData.doppelganger);
            ImGui::Text("角色模式 %u", actorData.mode);


            ImGui::Text("是否真·魔人 %u", actorData.devil);
            ImGui::Text("风格 %u", actorData.style);
            ImGui::Text("风格评级 %u", actorData.styleData.rank);
            ImGui::Text("风格评级计量 %g", actorData.styleData.meter);
            ImGui::Text("风格评级商 %g", actorData.styleData.quotient);
            ImGui::Text("风格评级被除数 %g", actorData.styleData.dividend);
            ImGui::Text("风格评级除数 %g", actorData.styleData.divisor);
            ImGui::Text("权限 %u", actorData.permissions);
            if (actorData.cloneActorBaseAddr) {
                ImGui::Text("分身风格评级 %u", cloneActorData.styleData.rank);
                ImGui::Text("分身风格评级计量 %g", cloneActorData.styleData.meter);
                ImGui::Text("分身风格评级商 %g", cloneActorData.styleData.quotient);
            }


            /*
            ImGui::Text("风格索引 0 %u", characterData.styleButtons[0]);
            ImGui::Text("风格索引 1 %u", characterData.styleButtons[1]);
            ImGui::Text("风格索引 2 %u", characterData.styleButtons[2]);
            ImGui::Text("风格索引 3 %u", characterData.styleButtons[3]);
            ImGui::Text("风格索引 4 %u", characterData.styleButtons[4]);
            ImGui::Text("风格索引 5 %u", characterData.styleButtons[5]);*/


            ImGui::Text("");
        }


        if (activeConfig.debugOverlayData.showRegionData) {
            ImGui::Text("区域数据");

            auto Function = [&](RegionData& regionData) -> void {
                ImGui::Text("metadataAddr %llX", regionData.metadataAddr);
                ImGui::Text("dataAddr     %llX", regionData.dataAddr);
                ImGui::Text("capacity     %u", regionData.capacity);
                ImGui::Text("boundary     %X", regionData.boundary);
                ImGui::Text("size         %X", regionData.size);
                ImGui::Text("pipe         %u", regionData.pipe);
                ImGui::Text("count        %u", regionData.count);
            };

            auto regionDataAddr = reinterpret_cast<RegionData*>(appBaseAddr + 0xCA8910);
            /*
            dmc3.exe+2C61BF - 4C 8D 25 4A279E00 - lea r12,[dmc3.exe+CA8910]
            dmc3.exe+2C61C6 - 44 8B C5          - mov r8d,ebp
            */

            constexpr new_size_t count = 3;

            for_all(index, count) {
                auto& regionData = regionDataAddr[index];

                ImGui::Text("%.4u", index);
                Function(regionData);
                ImGui::Text("");
            }
        }

//         if constexpr (!debug) {
//             return;
//         }

        ImGui::Text("g_saveIndex     %llu", g_saveIndex);
        ImGui::Text("g_lastSaveIndex %llu", g_lastSaveIndex);
        ImGui::Text("");

        ImGui::Text("g_show     %u", g_show);
        ImGui::Text("g_lastShow %u", g_lastShow);
        ImGui::Text("g_showMain %u", g_showMain);
        ImGui::Text("g_showShop %u", g_showShop);


        {
            auto& io = ImGui::GetIO();

            ImGui::Text("io.NavActive  %u", io.NavActive);
            ImGui::Text("io.NavVisible %u", io.NavVisible);


            [&]() {
                auto contextAddr = ImGui::GetCurrentContext();
                if (!contextAddr) {
                    return;
                }
                auto& context = *contextAddr;

                ImGui::Text("context.NavId %X", context.NavId);
            }();
        }


#ifdef SINGLE

        ImGui::Text("visible     %u", visible);
        ImGui::Text("lastVisible %u", lastVisible);
#else


        ImGui::Text("visibleMain     %u", visibleMain);
        ImGui::Text("lastVisibleMain %u", lastVisibleMain);

        ImGui::Text("visibleShop     %u", visibleShop);
        ImGui::Text("lastVisibleShop %u", lastVisibleShop);

#endif


        [&]() {
            auto pool_10480 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
            if (!pool_10480 || !pool_10480[3]) {
                return;
            }
            auto& actorData = *reinterpret_cast<PlayerActorData*>(pool_10480[3]);

            ImGui::Text("action %u", actorData.action);
        }();

        ImGui::Text("g_shopTimer   %g", g_shopTimer);
        ImGui::Text("g_shopTimeout %g", g_shopTimeout);
        ImGui::Text("");


        ImGui::Text("g_secretMission %u", g_secretMission);


        // [&]()
        // {
        // 	if (g_scene != SCENE::GAME)
        // 	{
        // 		return;
        // 	}

        // 	IntroduceEventData(return);
        // 	IntroduceNextEventData(return);

        // 	ImGui::Text("room          %u", eventData.room        );
        // 	ImGui::Text("position      %u", eventData.position    );
        // 	ImGui::Text("next room     %u", nextEventData.room    );
        // 	ImGui::Text("next position %u", nextEventData.position);
        // }();
    };

    
    OverlayFunction(mainOverlayLabel, activeConfig.debugOverlayData, queuedConfig.debugOverlayData, Function);
}

void MainOverlaySettings() {
    auto Function = [&]() {
        GUI_Checkbox2("显示焦点", activeConfig.debugOverlayData.showFocus, queuedConfig.debugOverlayData.showFocus);
        GUI_Checkbox2("显示 FPS", activeConfig.debugOverlayData.showFPS, queuedConfig.debugOverlayData.showFPS);
        GUI_Checkbox2("显示尺寸", activeConfig.debugOverlayData.showSizes, queuedConfig.debugOverlayData.showSizes);
        GUI_Checkbox2("显示帧率倍率", activeConfig.debugOverlayData.showFrameRateMultiplier,
            queuedConfig.debugOverlayData.showFrameRateMultiplier);
        GUI_Checkbox2("显示事件数据", activeConfig.debugOverlayData.showEventData, queuedConfig.debugOverlayData.showEventData);
        GUI_Checkbox2("显示位置", activeConfig.debugOverlayData.showPosition, queuedConfig.debugOverlayData.showPosition);
        GUI_Checkbox2("显示区域数据", activeConfig.debugOverlayData.showRegionData, queuedConfig.debugOverlayData.showRegionData);
    };

    OverlaySettings(mainOverlayLabel, activeConfig.debugOverlayData, queuedConfig.debugOverlayData, defaultConfig.debugOverlayData, Function);
}


const char* missionOverlayLabel = "MissionOverlay";

void MissionOverlayWindow(size_t defaultFontSize) {
    auto Function = [&]() {
        ImGui::Text("任务");

        ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 1.1f]);


        auto name_10723 = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E30);
        if (!name_10723) {
            return;
        }
        auto& missionData = *reinterpret_cast<MissionData*>(name_10723);


        auto timeData = TimeData(static_cast<float>(missionData.frameCount), g_FrameRate
            // 60.0f // @Update
        );

        ImGui::Text("时间           %02u:%02u:%02u.%03u", timeData.hours, timeData.minutes, timeData.seconds, timeData.milliseconds);


        ImGui::Text("伤害         %u", missionData.damage);
        ImGui::Text("红魂收集 %u", missionData.orbsCollected);
        ImGui::Text("道具使用     %u", missionData.itemsUsed);
        ImGui::Text("击杀数     %u", missionData.killCount);

        auto pool_10621 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
        if (!pool_10621 || !pool_10621[3]) {
            return;
        }
        auto& actorData = *reinterpret_cast<PlayerActorData*>(pool_10621[3]);

        auto stylePoints = (actorData.styleData.quotient * 100.0f);

        ImGui::Text("风格分   %.2f", stylePoints);
    };

    OverlayFunction(missionOverlayLabel, activeConfig.missionOverlayData, queuedConfig.missionOverlayData, Function);
}

void MissionOverlaySettings() {
    OverlaySettings(
        missionOverlayLabel, activeConfig.missionOverlayData, queuedConfig.missionOverlayData, defaultConfig.missionOverlayData);
}


const char* bossLadyActionsOverlayLabel = "BossLadyActionsOverlay";

void BossLadyActionsOverlayWindow() {
    auto Function = [&]() {
        ImGui::Text("Boss Lady Actions\n"
                    "\n"
                    "Lock-On + Forward + A: Trooper Roll\n"
                    "Lock-On + Right   + A: Wheel Right\n"
                    "Lock-On + Back    + A: Wheel Back\n"
                    "Lock-On + Left    + A: Wheel Left\n"
                    "\n"
                    "Right Trigger: Reload Pistol\n"
                    "Left  Trigger: Reload SMG\n"
                    "\n"
                    "Lock-On + Back + X: Pistol Fall Back Shoot (hold for crossbow)\n"
                    "\n"
                    "B: SMG Roundhouse\n"
                    "\n"
                    "                    Y: Kalina Ann Hook\n"
                    "Lock-On + Forward + Y: Kalina Ann Charged Shot\n"
                    "Lock-On + Back    + Y: Kalina Ann Hysteric\n"
                    "\n"
                    "Left Shoulder: Grenades\n");
    };

    OverlayFunction(
        bossLadyActionsOverlayLabel, activeConfig.bossLadyActionsOverlayData, queuedConfig.bossLadyActionsOverlayData, Function);
}

void BossLadyActionsOverlaySettings() {
    OverlaySettings(bossLadyActionsOverlayLabel, activeConfig.bossLadyActionsOverlayData, queuedConfig.bossLadyActionsOverlayData,
        defaultConfig.bossLadyActionsOverlayData);
}


const char* bossVergilActionsOverlayLabel = "BossVergilActionsOverlay";

void BossVergilActionsOverlayWindow() {
    auto Function = [&]() {
        ImGui::Text("Boss Vergil Actions\n"
                    "\n"
                    "                    Y: Yamato Deflect\n"
                    "Lock-On + Forward + Y: Yamato Super Judgement Cut Follow\n"
                    "Lock-On + Back    + Y: Yamato Super Judgement Cut\n"
                    "\n"
                    "B: Block\n"
                    "\n"
                    "                    X: Shield\n"
                    "Lock-On + Forward + X: Strong Shield\n"
                    "Lock-On + Back    + X: Strong Shield 2\n"
                    "\n"
                    "Left Shoulder: Toggle Devil Form\n"
                    "\n"
                    "Left  Trigger: Taunt\n"
                    "Right Trigger: Rest in Peace\n");
    };

    OverlayFunction(
        bossVergilActionsOverlayLabel, activeConfig.bossVergilActionsOverlayData, queuedConfig.bossVergilActionsOverlayData, Function);
}

void BossVergilActionsOverlaySettings() {
    OverlaySettings(bossVergilActionsOverlayLabel, activeConfig.bossVergilActionsOverlayData, queuedConfig.bossVergilActionsOverlayData,
        defaultConfig.bossVergilActionsOverlayData);
}

ImVec4 GetCrimsonGUIBgColor(float opacity = 1.0f) {
	return ImVec4(0.169f, 0.125f, 0.133f, opacity);
}

void AdjustBackgroundColorAndTransparency() {
	// previously ImVec4(0.207f, 0.156f, 0.168f
   	
    switch (queuedCrimsonConfig.GUI.transparencyMode) {
        // OFF
    case 0 :
        ImGui::PushStyleColor(ImGuiCol_WindowBg, GetCrimsonGUIBgColor());
        break;
        
        // STATIC
    case 1 :
        ImGui::PushStyleColor(ImGuiCol_WindowBg, GetCrimsonGUIBgColor(queuedCrimsonConfig.GUI.opacity));
        break;

        //DYNAMIC
    case 2 :
        if (g_inGameDelayed) {
            ImGui::PushStyleColor(ImGuiCol_WindowBg, GetCrimsonGUIBgColor(queuedCrimsonConfig.GUI.opacity));
        }
		else {
			ImGui::PushStyleColor(ImGuiCol_WindowBg, GetCrimsonGUIBgColor());
		}
        
        break;
    }

}


void InterfaceSection(size_t defaultFontSize, ID3D11Device* pDevice) {

    ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);


    ImGui::Text("CRIMSON 界面选项");

	ImGui::PopFont();

	UI::SeparatorEx(defaultFontSize * 23.35f);


	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

	ImGui::PushItemWidth(itemWidth);
	ImGui::Text("");


	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;
		
		if (ImGui::BeginTable("GUIOptiomsTable", 3)) {

			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			ImGui::TableNextRow(0, rowWidth * 0.5f);
			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * 0.6f);
			UI::Combo2("透明模式", GUITransparencyNames, activeCrimsonConfig.GUI.transparencyMode, queuedCrimsonConfig.GUI.transparencyMode);
			ImGui::PopItemWidth();
			ImGui::SameLine();
			TooltipHelper("(?)", "动态：仅在游戏内应用透明。");

			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * 0.8f);
			GUI_InputDefault2<float>("不透明度", activeCrimsonConfig.GUI.opacity, queuedCrimsonConfig.GUI.opacity,defaultCrimsonConfig.GUI.opacity, 0.1f, "%g",
				ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();


			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * 0.8f);
			if (GUI_InputDefault2("全局缩放", activeConfig.globalScale, queuedConfig.globalScale, defaultConfig.globalScale, 0.1f, "%g",
				ImGuiInputTextFlags_EnterReturnsTrue)) {
				UpdateGlobalScale();
			}
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			if (GUI_Checkbox2("打开时暂停", activeCrimsonConfig.GUI.pauseWhenOpened, queuedCrimsonConfig.GUI.pauseWhenOpened)) {
				if (g_inGameDelayed) {
					activeCrimsonGameplay.Cheats.Speed.defaultGame = queuedCrimsonGameplay.Cheats.Speed.defaultGame; // This resumes the game speed
					activeCrimsonGameplay.Cheats.Speed.turboGame = queuedCrimsonGameplay.Cheats.Speed.turboGame;
					Speed::Toggle(true); // Toggle Speed on and off to set the new speed
					Speed::Toggle(false);
				}
			}

			ImGui::TableNextColumn();

			GUI_Checkbox2("界面音效", 
				activeCrimsonConfig.GUI.sounds, queuedCrimsonConfig.GUI.sounds);

			ImGui::TableNextColumn();

			GUI_Checkbox2("快捷键作弊弹窗",
				activeCrimsonConfig.GUI.cheatsPopup, queuedCrimsonConfig.GUI.cheatsPopup);

			ImGui::TableNextColumn();

			GUI_Checkbox2("禁用手柄快捷键",
				activeCrimsonConfig.GUI.disableGamepadShortcut, queuedCrimsonConfig.GUI.disableGamepadShortcut);
			ImGui::SameLine();
			TooltipHelper("(?)", "适合想把 LS + RS 用作切换键的玩家。");
	

			ImGui::EndTable();
		}
	}

    ImGui::PopFont();
    ImGui::PopStyleColor();

    ImGui::Text("");

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);


	ImGui::Text("HUD 选项");

	ImGui::PopFont();

	UI::SeparatorEx(defaultFontSize * 23.35f);

    ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
    ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

    ImGui::Text("");

	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;


		if (ImGui::BeginTable("HUDOptiomsTable", 3)) {

			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			ImGui::TableNextRow(0, rowWidth * 0.5f);
			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth);
			UI::ComboVectorString("选择 HUD", CrimsonFiles::HUDdirectories, queuedCrimsonConfig.HudOptions.selectedHUD);
			if (queuedCrimsonConfig.HudOptions.selectedHUD != activeCrimsonConfig.HudOptions.selectedHUD) {
				activeCrimsonConfig.HudOptions.selectedHUD = queuedCrimsonConfig.HudOptions.selectedHUD;
				// Copy renamed files to GData.afs for backward compatibility
				CrimsonFiles::CopyHUDFilePactoGameFolder();
				// Load dynamically and hot-reload icons immediately
				CrimsonFiles::ApplySelectedHUD(activeCrimsonConfig.HudOptions.selectedHUD);
				HUD_ReloadIcons();
			}
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth);
			if (GUI_Checkbox2("隐藏主 HUD", activeCrimsonConfig.HudOptions.hideMainHUD, queuedCrimsonConfig.HudOptions.hideMainHUD)) {
				ToggleHideMainHUD(activeCrimsonConfig.HudOptions.hideMainHUD);
			}
			ImGui::PopItemWidth();


			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth);
			if (GUI_Checkbox2("始终显示主 HUD", activeCrimsonConfig.HudOptions.forceVisibleHUD, queuedCrimsonConfig.HudOptions.forceVisibleHUD)) {
				ToggleForceVisibleHUD(activeCrimsonConfig.HudOptions.forceVisibleHUD);
			}

			ImGui::PopItemWidth();

			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			if (GUI_Checkbox2("隐藏锁定", activeCrimsonConfig.HudOptions.hideLockOn, queuedCrimsonConfig.HudOptions.hideLockOn)) {
				CrimsonPatches::ToggleHideLockOn(activeCrimsonConfig.HudOptions.hideLockOn);
			}

			ImGui::TableNextColumn();

			if (GUI_Checkbox2("隐藏风格槽", activeCrimsonConfig.HudOptions.hideStyleMeter, queuedCrimsonConfig.HudOptions.hideStyleMeter)) {
				CrimsonDetours::ToggleHideStyleRankHUD(activeCrimsonConfig.HudOptions.hideStyleMeter);
			}

			ImGui::TableNextColumn();

			if (GUI_Checkbox2("隐藏首领血条", activeCrimsonConfig.HudOptions.hideBossHUD, queuedCrimsonConfig.HudOptions.hideBossHUD)) {
				ToggleHideBossHUD(activeCrimsonConfig.HudOptions.hideBossHUD);
			}

			ImGui::TableNextColumn();

			GUI_Checkbox2("禁用风格评级淡出", activeCrimsonConfig.HudOptions.disableStyleRankHudFadeout, queuedCrimsonConfig.HudOptions.disableStyleRankHudFadeout);

			ImGui::EndTable();
		}
	}


    ImGui::PopFont();
    ImGui::PopStyleColor();
    
	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);
	ImGui::Text("CRIMSON HUD 1.0 附加项");
	ImGui::PopFont();
	

	UI::SeparatorEx(defaultFontSize * 23.35f);

	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

	ImGui::Text("");

 	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;


		if (ImGui::BeginTable("CrimsonHUDAddonsTable", 3)) {

			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			ImGui::TableNextRow(0, rowWidth * 0.5f);
			ImGui::TableNextColumn();

			if (GUI_Checkbox2("HUD 位置", activeCrimsonConfig.CrimsonHudAddons.positionings, queuedCrimsonConfig.CrimsonHudAddons.positionings)) {
				CrimsonDetours::ToggleClassicHUDPositionings(!activeCrimsonConfig.CrimsonHudAddons.positionings);
			}
			ImGui::SameLine();
			TooltipHelper("(?)", "使用经典 HUD 时请取消勾选。");

			ImGui::TableNextColumn();

			UI::ComboMapValue2("风格显示", stylesDisplayStateNames, stylesDisplayStateMap,
				activeCrimsonConfig.CrimsonHudAddons.stylesDisplay,
				queuedCrimsonConfig.CrimsonHudAddons.stylesDisplay);
				

			ImGui::TableNextColumn();

			GUI_Checkbox2("显示风格名称", activeCrimsonConfig.CrimsonHudAddons.displayStyleNames, queuedCrimsonConfig.CrimsonHudAddons.displayStyleNames);

			ImGui::TableNextColumn();

			if (UI::ComboMapValue2("风格名称集", styleNamesTextureSetNames, styleNamesTextureSetMap,
				activeCrimsonConfig.CrimsonHudAddons.styleNamesSet,
				queuedCrimsonConfig.CrimsonHudAddons.styleNamesSet)) {
				activeCrimsonConfig.CrimsonHudAddons.styleNamesSet = queuedCrimsonConfig.CrimsonHudAddons.styleNamesSet;
				CrimsonHUD::InitStyleNameTextures(pDevice);
			}

			ImGui::TableNextColumn();

			//GUI_PushDisable((activeCrimsonGameplay.GameMode.preset >= GAMEMODEPRESETS::STYLE_SWITCHER) || (activeConfig.Actor.enable != queuedConfig.Actor.enable));

			GUI_Checkbox2("红魂计数器", activeCrimsonConfig.CrimsonHudAddons.redOrbCounter, queuedCrimsonConfig.CrimsonHudAddons.redOrbCounter);

			//GUI_PopDisable((activeCrimsonGameplay.GameMode.preset >= GAMEMODEPRESETS::STYLE_SWITCHER) || (activeConfig.Actor.enable != queuedConfig.Actor.enable));

			ImGui::TableNextColumn();

			GUI_Checkbox2("皇家计量表", activeCrimsonConfig.CrimsonHudAddons.royalGauge, queuedCrimsonConfig.CrimsonHudAddons.royalGauge);

			ImGui::TableNextColumn();

			if (GUI_Checkbox2("风格评级槽", activeCrimsonConfig.CrimsonHudAddons.styleRanksMeter, queuedCrimsonConfig.CrimsonHudAddons.styleRanksMeter)) {
				CrimsonDetours::ToggleHideStyleRankHUD(activeCrimsonConfig.CrimsonHudAddons.styleRanksMeter);
			}

			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth);
			if (UI::ComboVectorString2("选择计量表", CrimsonFiles::StyleRanksdirectories, activeCrimsonConfig.CrimsonHudAddons.selectedStyleRanks,
				queuedCrimsonConfig.CrimsonHudAddons.selectedStyleRanks)) {
				CrimsonFiles::GetStyleRanksAccoladesDirectories();
				activeCrimsonConfig.CrimsonHudAddons.selectedStyleRanksAccolades = CrimsonFiles::StyleRanksAccoladesdirectories[0];
				queuedCrimsonConfig.CrimsonHudAddons.selectedStyleRanksAccolades = CrimsonFiles::StyleRanksAccoladesdirectories[0];
				CrimsonHUD::InitStyleRankTextures(pDevice);
			}
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth);
			if (UI::ComboVectorString2("选择计量表称号", CrimsonFiles::StyleRanksAccoladesdirectories, activeCrimsonConfig.CrimsonHudAddons.selectedStyleRanksAccolades,
				queuedCrimsonConfig.CrimsonHudAddons.selectedStyleRanksAccolades)) {
				CrimsonHUD::InitStyleRankTextures(pDevice);
			}
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			GUI_Checkbox2("风格分计数器", activeCrimsonConfig.CrimsonHudAddons.stylishPtsCounter, queuedCrimsonConfig.CrimsonHudAddons.stylishPtsCounter);

			ImGui::TableNextColumn();

			UI::ComboMapValue2("任务计时显示", missionTimerDisplayStateNames, missionTimerDisplayStateMap,
				activeCrimsonConfig.CrimsonHudAddons.missionTimerDisplay, queuedCrimsonConfig.CrimsonHudAddons.missionTimerDisplay);

			ImGui::TableNextColumn();
			
			if (GUI_Checkbox2("Lock-On", activeCrimsonConfig.CrimsonHudAddons.lockOn, queuedCrimsonConfig.CrimsonHudAddons.lockOn)) {
				CrimsonPatches::ToggleHideLockOn(activeCrimsonConfig.CrimsonHudAddons.lockOn);
			}

			ImGui::TableNextColumn();

			GUI_Checkbox2("Stun/Displacement Lock-On", activeCrimsonConfig.CrimsonHudAddons.lockOnStunDisplacement,
				queuedCrimsonConfig.CrimsonHudAddons.lockOnStunDisplacement);

			ImGui::TableNextColumn();

			GUI_Checkbox2("Stun/Displacement Numeric HUD", activeCrimsonConfig.CrimsonHudAddons.stunDisplacementNumericHud, queuedCrimsonConfig.CrimsonHudAddons.stunDisplacementNumericHud);
			
			ImGui::SameLine();
			TooltipHelper("(?)", "Show numerical stun and displacement values on lock-on (Hells enemies only).\nLast Move minus value updates each 200ms. Requires CrimsonHUD's Lock-On.");

			ImGui::TableNextColumn();

			GUI_PushDisable(!activeCrimsonConfig.CrimsonHudAddons.lockOn);

			GUI_Checkbox2("角色锁定颜色", activeCrimsonConfig.CrimsonHudAddons.lockOnColorsCharacter, 
				queuedCrimsonConfig.CrimsonHudAddons.lockOnColorsCharacter);
			ImGui::SameLine();
			TooltipHelper("(?)", "SINGLE PLAYER ONLY: Changes the color of the Lock-On reticle to match the character's color.\n"
				"Disabling this effectively allows you to change the Lock-On color at your will.\n"
				"多人游戏或关闭此项时，锁定颜色始终基于玩家颜色。");

			GUI_PopDisable(!activeCrimsonConfig.CrimsonHudAddons.lockOn);


			ImGui::EndTable();
		}
	}

	ImGui::Text("");

	GUI_PushDisable(!activeConfig.Actor.enable);
	GUI_Title("武器轮盘", true);

	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;

		if (ImGui::BeginTable("WeaponWheelOptionsTable", 3)) {

			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			ImGui::TableNextRow(0, rowWidth * 0.5f);
			ImGui::TableNextColumn();

			// First Row - Analog Switching & Disable Camera Control
			ImGui::PushItemWidth(itemWidth * 0.8f);
			if (GUI_Checkbox2(
				"摇杆切换", activeCrimsonConfig.WeaponWheel.analogSwitching,
				queuedCrimsonConfig.WeaponWheel.analogSwitching)) {

				if (!queuedCrimsonConfig.WeaponWheel.analogSwitching) {
					activeCrimsonConfig.WeaponWheel.disableCameraRotation = false;
					queuedCrimsonConfig.WeaponWheel.disableCameraRotation = false;
				} else {
					activeCrimsonConfig.WeaponWheel.disableCameraRotation = true;
					queuedCrimsonConfig.WeaponWheel.disableCameraRotation = true;
				}
			}
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			GUI_PushDisable(!activeCrimsonConfig.WeaponWheel.analogSwitching);
			ImGui::PushItemWidth(itemWidth * 0.8f);
			GUI_Checkbox2("打开时禁用镜头控制",
				activeCrimsonConfig.WeaponWheel.disableCameraRotation,
				queuedCrimsonConfig.WeaponWheel.disableCameraRotation);
			ImGui::PopItemWidth();
			GUI_PopDisable(!activeCrimsonConfig.WeaponWheel.analogSwitching);

			ImGui::TableNextColumn();
			ImGui::PushItemWidth(itemWidth * 0.8f);
			UI::ComboVectorString2("主题", weaponWheelThemeNames,
				activeCrimsonConfig.WeaponWheel.theme,
				queuedCrimsonConfig.WeaponWheel.theme);
			ImGui::PopItemWidth();

			// Second Row - Melee/Ranged Always Show
			ImGui::TableNextRow(0, rowWidth * 0.5f);
			ImGui::TableNextColumn();

			UI::ComboMapValue2("轮盘始终显示", hudElementShowStateNames, hudElementShowStateMap,
				activeCrimsonConfig.WeaponWheel.alwaysShow,
				queuedCrimsonConfig.WeaponWheel.alwaysShow);

			ImGui::TableNextColumn();
		
			ImGui::PushItemWidth(itemWidth * 0.8f);
			UI::ComboMapValue2("缩放", hudElementScaleStateNames, hudElementScaleStateMap,
				activeCrimsonConfig.WeaponWheel.scale,
				queuedCrimsonConfig.WeaponWheel.scale);
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();
			GUI_Checkbox2("Force 1P Multiplayer Positioning/Scale",
				activeCrimsonConfig.WeaponWheel.force1PMultiplayerPosScale,
				queuedCrimsonConfig.WeaponWheel.force1PMultiplayerPosScale);

			ImGui::TableNextRow(0, rowWidth * 0.5f);

			ImGui::TableNextColumn();
			GUI_Checkbox2("隐藏武器轮盘 HUD",
				activeCrimsonConfig.WeaponWheel.hide,
				queuedCrimsonConfig.WeaponWheel.hide);

			ImGui::TableNextColumn();
			ImGui::PushItemWidth(itemWidth * 1.0f);
			UI::ComboMapValue2("世界空间轮盘", hudElementShowStateNames, hudElementShowStateMap,
				activeCrimsonConfig.WeaponWheel.worldSpaceWheels,
				queuedCrimsonConfig.WeaponWheel.worldSpaceWheels);
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			UI::ComboMapValue2("世界轮盘始终显示", hudElementShowStateNames, hudElementShowStateMap,
				activeCrimsonConfig.WeaponWheel.worldSpaceAlwaysShow,
				queuedCrimsonConfig.WeaponWheel.worldSpaceAlwaysShow);


			ImGui::EndTable();
		}
	}
	GUI_PopDisable(!activeConfig.Actor.enable);

	ImGui::Text("");

	ImGui::PopStyleColor();

	ImGui::PopFont();
	ImGui::PopStyleColor();

	ImGui::PopItemWidth();
	//ImGui::PopFont();

	BarsSection(defaultFontSize);

	CrimsonVisualStyle::TrackVisualStyle();
}

#pragma endregion

#pragma region Repair

void Repair() {
    if (ImGui::CollapsingHeader("修复")) {
        ImGui::Text("");

        DescriptionHelper("修正错误数值。"
                          "可在任务选择菜单中访问。"
                          "应用修复后请保存游戏。");
        ImGui::Text("");


        bool condition = (g_scene != SCENE::MISSION_SELECT);

        GUI_PushDisable(condition);

        if (GUI_Button("重置武器")) {
            [&]() {
                auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);


                SetMemory(sessionData.weapons, 0, sizeof(sessionData.weapons));

                switch (sessionData.character) {
                case CHARACTER::DANTE: {
                    sessionData.weapons[0] = WEAPON::REBELLION;
                    sessionData.weapons[1] = WEAPON::CERBERUS;
                    sessionData.weapons[2] = WEAPON::EBONY_IVORY;
                    sessionData.weapons[3] = WEAPON::SHOTGUN;
                    sessionData.weapons[4] = WEAPON::VOID;

                    break;
                }
                case CHARACTER::VERGIL: {
                    sessionData.weapons[0] = WEAPON::YAMATO_VERGIL;
                    sessionData.weapons[1] = WEAPON::BEOWULF_VERGIL;
                    sessionData.weapons[2] = WEAPON::YAMATO_FORCE_EDGE;
                    sessionData.weapons[3] = WEAPON::VOID;
                    sessionData.weapons[4] = WEAPON::VOID;

                    break;
                }
                }

                sessionData.meleeWeaponIndex  = 0;
                sessionData.rangedWeaponIndex = 2;
            }();
        }


        if (GUI_Button("重置武器等级")) {
            [&]() {
                auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);


                SetMemory(sessionData.rangedWeaponLevels, 0, sizeof(sessionData.rangedWeaponLevels));
            }();
        }

        GUI_PopDisable(condition);


        ImGui::Text("");
    }
}

#pragma endregion

#pragma region Speed

const char* devilSpeedNamesDante[] = {
    "叛逆",
    "刻耳柏洛斯",
    "阿耆尼与楼陀罗",
    "涅梵",
    "贝奥武夫",
    "斯巴达",
};

const char* devilSpeedNamesVergil[] = {
    "阎魔刀",
    "贝奥武夫",
    "阎魔刀与力量之刃",
    "尼禄·安杰洛阎魔刀",
    "尼禄·安杰洛贝奥武夫",
};


// @Todo: EnterReturnsTrue.
// @Todo: Apply rounding.

void CustomSpeedSection() {
	auto defaultFontSize = UI::g_UIContext.DefaultFontSize;
	const float columnWidth = 0.15f * queuedConfig.globalScale;
	const float rowHeight = 40.0f * queuedConfig.globalScale * 0.5f;
	float smallerComboMult = 0.7f;
	ImGui::PushItemWidth(itemWidth);
	// Speed Group Title
	if (GUI_TitleCheckbox2("自定义游戏速度作弊", activeCrimsonGameplay.Cheats.General.customSpeed,
		queuedCrimsonGameplay.Cheats.General.customSpeed)) {
		if (!activeCrimsonGameplay.Cheats.General.customSpeed) {
			CopyMemory(&queuedCrimsonGameplay.Cheats.Speed, &defaultCrimsonGameplay.Cheats.Speed, sizeof(queuedCrimsonGameplay.Cheats.Speed));
			CopyMemory(&activeCrimsonGameplay.Cheats.Speed, &queuedCrimsonGameplay.Cheats.Speed, sizeof(activeCrimsonGameplay.Cheats.Speed));
			Speed::Toggle(true);
		}
	}
	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	// Reusable speed input
	auto SpeedInput = [](const char* label, float& active, float& queued, float& defaultVal) {
		auto defaultFontSize = UI::g_UIContext.DefaultFontSize;
		ImGui::PushItemWidth(itemWidth * 1.0f);
		GUI_InputDefault2(label, active, queued, defaultVal, 0.1f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::PopItemWidth();
		};
	GUI_PushDisable(!activeCrimsonGameplay.Cheats.General.customSpeed);
	// --- Main Speed Table ---
	if (ImGui::BeginTable("SpeedTable", 2)) {
		ImGui::TableSetupColumn("左", 0, columnWidth * 3.5f);
		ImGui::TableSetupColumn("右", 0, columnWidth * 3.5f);
		ImGui::TableNextRow(0, rowHeight);
		ImGui::TableNextColumn();
		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
		ImGui::Text("默认游戏");
		ImGui::PopFont();
		ImGui::TableNextColumn();
		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
		ImGui::Text("加速游戏"); ImGui::SameLine();
		ImGui::PopFont();

		ImGui::TableNextRow(0, rowHeight);

		ImGui::TableNextColumn();
		GUI_PushDisable(IsTurbo());
		ImGui::PushItemWidth(itemWidth * 1.0f);

		GUI_InputDefault2SpeedCalc("", activeCrimsonGameplay.Cheats.Speed.defaultGame, queuedCrimsonGameplay.Cheats.Speed.defaultGame, 
			defaultCrimsonGameplay.Cheats.Speed.defaultGame, 0.1f,
				"%g", ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::PopItemWidth();
		GUI_PopDisable(IsTurbo());

		ImGui::TableNextColumn();

		GUI_PushDisable(!IsTurbo());

		ImGui::PushItemWidth(itemWidth * 1.0f);
		GUI_InputDefault2SpeedCalc("", activeCrimsonGameplay.Cheats.Speed.turboGame, queuedCrimsonGameplay.Cheats.Speed.turboGame,
			defaultCrimsonGameplay.Cheats.Speed.turboGame, 0.1f,
			"%g", ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::PopItemWidth();

		GUI_PopDisable(!IsTurbo());
		ImGui::EndTable();
	}

	// --- Main Speed Table ---
	if (ImGui::BeginTable("SpeedTable2", 2)) {
		ImGui::TableSetupColumn("左", 0, columnWidth * 3.5f);
		ImGui::TableSetupColumn("右", 0, columnWidth * 3.5f);
		ImGui::TableNextRow(0, rowHeight);
		ImGui::TableNextColumn();
		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
		ImGui::Text("敌人角色");
		ImGui::PopFont();
		ImGui::TableNextColumn();
		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
		ImGui::Text("玩家角色"); ImGui::SameLine();
		ImGui::PopFont();
		GUI_CCSRequirementButton();
		ImGui::TableNextRow(0, rowHeight);
		ImGui::TableNextColumn();
		SpeedInput("敌人", activeCrimsonGameplay.Cheats.Speed.enemy,
			queuedCrimsonGameplay.Cheats.Speed.enemy, defaultCrimsonGameplay.Cheats.Speed.enemy);
		ImGui::TableNextColumn();
		SpeedInput("人类", activeCrimsonGameplay.Cheats.Speed.human, 
			queuedCrimsonGameplay.Cheats.Speed.human, defaultCrimsonGameplay.Cheats.Speed.human);
		ImGui::EndTable();
	}
	ImGui::Text("");
	if (ImGui::BeginTable("SpeedTableDT", 2)) {
		ImGui::TableSetupColumn("左", 0, columnWidth * 3.5f);
		ImGui::TableSetupColumn("右", 0, columnWidth * 3.5f);
		// Row 1 - Dante
		ImGui::TableNextRow(0, rowHeight);
		ImGui::TableNextColumn();
		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
		ImGui::Text("魔人化但丁");
		ImGui::PopFont();
		for (uint8 i = 0; i < countof(activeCrimsonGameplay.Cheats.Speed.dTDante); ++i) {
			SpeedInput(devilSpeedNamesDante[i], activeCrimsonGameplay.Cheats.Speed.dTDante[i], 
				queuedCrimsonGameplay.Cheats.Speed.dTDante[i], defaultCrimsonGameplay.Cheats.Speed.dTDante[i]);
		}
		// Row 1 - Vergil
		ImGui::TableNextColumn();
		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
		ImGui::Text("魔人化维吉尔");
		ImGui::PopFont();
		for (uint8 i = 0; i < countof(activeCrimsonGameplay.Cheats.Speed.dTVergil); ++i) {
			SpeedInput(devilSpeedNamesVergil[i], activeCrimsonGameplay.Cheats.Speed.dTVergil[i], 
				queuedCrimsonGameplay.Cheats.Speed.dTVergil[i], defaultCrimsonGameplay.Cheats.Speed.dTVergil[i]);
		}
		// Row 2 - Quicksilver
		ImGui::TableNextRow(0, rowHeight);
		ImGui::TableNextColumn();
		ImGui::Text("");
		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
		ImGui::Text("水银");
		ImGui::PopFont();
		SpeedInput("玩家角色", activeCrimsonGameplay.Cheats.Speed.quicksilverPlayer, 
			queuedCrimsonGameplay.Cheats.Speed.quicksilverPlayer, defaultCrimsonGameplay.Cheats.Speed.quicksilverPlayer);
		SpeedInput("敌人角色", activeCrimsonGameplay.Cheats.Speed.quicksilverEnemy, 
			queuedCrimsonGameplay.Cheats.Speed.quicksilverEnemy, defaultCrimsonGameplay.Cheats.Speed.quicksilverEnemy);
		ImGui::EndTable();
	}
	GUI_PopDisable(!activeCrimsonGameplay.Cheats.General.customSpeed);
	ImGui::PopFont();
	ImGui::PopItemWidth();
	ImGui::Text("");
}

#pragma endregion

#pragma region System

const char* Graphics_vSyncNames[] = {
    "自动",
    "强制关闭",
    "强制开启",
};

const char* Sound_channelNames[CHANNEL::MAX] = {
    "系统",
    "通用",
    "风格武器",
    "武器 1",
    "武器 2",
    "武器 3",
    "武器 4",
    "敌人",
    "房间",
    "音乐",
    "演示",
};


void SystemSection(size_t defaultFontSize) {

	float smallerComboMult = 0.7f;

	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);


	ImGui::Text("GRAPHICS / WINDOW");

	ImGui::PopFont();

	UI::SeparatorEx(defaultFontSize * 23.35f);


	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

	ImGui::PushItemWidth(itemWidth * smallerComboMult);
	ImGui::Text("");

	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;

		if (ImGui::BeginTable("GraphicsWindowOptionsTable", 3)) {

			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			//ImGui::TableNextRow(0, rowWidth * 0.5f);
			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * 0.9f);
			if (GUI_InputDefault2<float>("帧率", activeCrimsonConfig.System.fpsCap, queuedCrimsonConfig.System.fpsCap, defaultCrimsonConfig.System.fpsCap, 1, "%.2f",
				ImGuiInputTextFlags_EnterReturnsTrue)) {
				Speed::Toggle(true);
				CrimsonOnTick::inputtingFPS = true;
			} else {
				CrimsonOnTick::inputtingFPS = false;
			}
			if (GUI_Button("60 FPS")) {
				activeCrimsonConfig.System.fpsCap = 60.0f;
				queuedCrimsonConfig.System.fpsCap = 60.0f;
			}
			ImGui::SameLine();
			if (GUI_Button("80 FPS")) {
				activeCrimsonConfig.System.fpsCap = 80.0f;
				queuedCrimsonConfig.System.fpsCap = 80.0f;
			}
			ImGui::SameLine();
			if (GUI_Button("120 FPS")) {
				activeCrimsonConfig.System.fpsCap = 120.0f;
				queuedCrimsonConfig.System.fpsCap = 120.0f;
			}

			ImGui::PopItemWidth();

            ImGui::TableNextColumn();

			GUI_Checkbox2("解锁帧率", activeCrimsonConfig.System.fpsUnlocked, queuedCrimsonConfig.System.fpsUnlocked);
			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("This neat experimental feature completely eliminates input lag and greatly boosts game responsiveness.\n"
					"We're actively polishing it, some bugs may still occur.\n"
					"如发现问题，请在我们的 GitHub 问题追踪器或 Discord 服务器上报告。");
			}
			ImGui::SameLine();
			GUI_WIPButton();

			ImGui::TableNextColumn();


			ImGui::PushItemWidth(itemWidth * 0.8f);
			UI::Combo2("V-Sync", Graphics_vSyncNames, activeConfig.vSync, queuedConfig.vSync);
			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Synchronizes Monitor Refresh Rate with Frame Rate to eliminate Screen Tearing.\n"
					"警告：开启会引入大量输入延迟。想降低延迟请保持“强制关闭”。");
			}

			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			GUI_Checkbox("DXGI 翻转模型呈现", queuedCrimsonConfig.System.flipModelPresentation);
			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Alters how Exclusive Fullscreen works by forcing usage of Flip Model Presentation.\n"
					"This setting may improve alt-tab issues, latency, enables quicker toggling between fullscreen/windowed with Alt + Enter,"
					"\nand adds support for HDR and VRR/GSync.");
			}

			if (queuedCrimsonConfig.System.flipModelPresentation != activeCrimsonConfig.System.flipModelPresentation) {
				ImGui::TextColored(CrimsonUtil::HexToImVec4(0x1DD6FFFF), "需重启游戏。");
			}

			ImGui::TableNextColumn();

	
			GUI_Checkbox2("禁用菜单淡出", activeCrimsonConfig.System.disableMenuFadeouts, queuedCrimsonConfig.System.disableMenuFadeouts);
				
			
			ImGui::TableNextColumn();


			if (GUI_Checkbox2("强制聚焦", activeConfig.forceWindowFocus, queuedConfig.forceWindowFocus)) {
				ToggleForceWindowFocus(activeConfig.forceWindowFocus);
			}

			ImGui::TableNextColumn();

			GUI_Checkbox2("隐藏鼠标光标", activeConfig.hideMouseCursor, queuedConfig.hideMouseCursor);


			ImGui::EndTable();
		}
	}


	ImGui::Text("");

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);


	ImGui::Text("杂项");

	ImGui::PopFont();

	UI::SeparatorEx(defaultFontSize * 23.35f);

    ImGui::Text("");

	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;

		if (ImGui::BeginTable("MiscOptionsTable", 2)) {

			ImGui::TableSetupColumn("b1", 0, columnWidth);
			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * 0.8f);
			if (GUI_Checkbox2("跳过片头", activeConfig.skipIntro, queuedConfig.skipIntro)) {
				ToggleSkipIntro(activeConfig.skipIntro);
			}
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			if (GUI_Checkbox2("跳过过场", activeConfig.skipCutscenes, queuedConfig.skipCutscenes)) {
				ToggleSkipCutscenes(activeConfig.skipCutscenes);
			}
			ImGui::SameLine();
			TooltipHelper("(?) WARNING", "This option has been known to cause memory corruption on some PCs,\n"
				"causing you to crash when reading an in-game message or interacting with an item, be wary of it.", 2048.0f, true);

			ImGui::TableNextColumn();

			bool* turboSetting = reinterpret_cast<bool*>(appBaseAddr + 0xD6CEA9);

			GUI_Checkbox("加速模式", *turboSetting);

			// Enable File Mods will be a json file only option for now, don't see much need for it in the GUI. - Mia
// 			ImGui::TableNextRow(0, rowWidth);
// 			ImGui::TableNextColumn();
// 
//             GUI_Checkbox2("启用文件模组", activeConfig.enableFileMods, queuedConfig.enableFileMods);
//             ImGui::SameLine();
// 			TooltipHelper("(?)", "WARNING: Necessary for any and all file mods you may have, \n"
// 				"disabling this will disable any file replacement mod, including custom HUDs and Models.");

			ImGui::EndTable();
		}

		ImGui::Text("");

		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);


		ImGui::Text("混合特效");

		ImGui::PopFont();

		UI::SeparatorEx(defaultFontSize * 23.35f);

		ImGui::Text("");

		{
			const float columnWidth = 0.5f * queuedConfig.globalScale;
			const float rowWidth = 40.0f * queuedConfig.globalScale;

			if (ImGui::BeginTable("BlendingEffectsOptionsTable", 2)) {

				ImGui::TableSetupColumn("b1", 0, columnWidth);
				ImGui::TableNextColumn();

				GUI_PushDisable(activeCrimsonConfig.System.BlendingEffects.disableAll);

				GUI_Checkbox2("重影", activeCrimsonConfig.System.BlendingEffects.ghosting,
					queuedCrimsonConfig.System.BlendingEffects.ghosting);
				if (ImGui::IsItemHovered()) {
					ImGui::SetTooltip("PS2-like Ghosting Blur present in some scenes, making the game feel kind of ethereal in movement.\n"
						"This toggle won't apply to screen transitions or cutscenes. Needs room reload to apply.\n"
						"警告：建议保持关闭，以获得更流畅、无卡顿的游玩体验。——Berthrage");
				}

				ImGui::TableNextColumn();

				GUI_Checkbox2("色彩滤镜", activeCrimsonConfig.System.BlendingEffects.colorFilter,
					queuedCrimsonConfig.System.BlendingEffects.colorFilter);
				if (ImGui::IsItemHovered()) {
					ImGui::SetTooltip("Color filter applied in some scenes.\n" 
						"此开关不影响场景切换或过场动画，需重新加载房间生效。");
				}

				ImGui::TableNextColumn();

				GUI_Checkbox2("泛光", activeCrimsonConfig.System.BlendingEffects.bloom,
					queuedCrimsonConfig.System.BlendingEffects.bloom);
				if (ImGui::IsItemHovered()) {
					ImGui::SetTooltip("此开关不影响场景切换或过场动画，需重新加载房间生效。");
				}

				ImGui::TableNextColumn();

				// Buggy, it's stopping teleports at mission 7, needs testing before it gets enabled again for users
// 				GUI_Checkbox2("Fog / Mist", activeCrimsonConfig.System.BlendingEffects.fogMist,
// 					queuedCrimsonConfig.System.BlendingEffects.fogMist);
// 				if (ImGui::IsItemHovered()) {
// 					ImGui::SetTooltip("此开关不影响场景切换或过场动画，需重新加载房间生效。");
// 				}
// 
// 				ImGui::TableNextColumn();

				GUI_Checkbox2("扭曲", activeCrimsonConfig.System.BlendingEffects.warp,
					queuedCrimsonConfig.System.BlendingEffects.warp);
				if (ImGui::IsItemHovered()) {
					ImGui::SetTooltip("此开关不影响场景切换或过场动画，需重新加载房间生效。");
				}

				ImGui::TableNextColumn();

				GUI_PopDisable(activeCrimsonConfig.System.BlendingEffects.disableAll);

				GUI_Checkbox2("禁用所有混合特效", activeCrimsonConfig.System.BlendingEffects.disableAll,
					queuedCrimsonConfig.System.BlendingEffects.disableAll);
				ImGui::SameLine();
				if (ImGui::IsItemHovered()) {
					ImGui::SetTooltip("禁用所有后期处理特效，可略微提升性能。"
						"\nThis toggle won't apply to screen transitions or cutscenes. Doesn't need room reload to apply.");
				}

				ImGui::EndTable();
			}
		}

		
	}

	

	ImGui::PopFont();
	ImGui::PopStyleColor();
    
}

#pragma endregion

#pragma region TeleporterTool

void TeleporterToolSection() {
	auto& defaultFontSize = UI::g_UIContext.DefaultFontSize;
	float smallerComboMult = 0.8f;
	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);

	GUI_TitleCheckbox2("传送工具", activeCrimsonGameplay.Cheats.General.teleporterTool,
		queuedCrimsonGameplay.Cheats.General.teleporterTool, false);

	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

	GUI_PushDisable(!activeCrimsonGameplay.Cheats.General.teleporterTool);

	if (!InGame()) {
		ImGui::Text("指针无效，需在游戏内使用。");
		ImGui::PopStyleColor();
		GUI_PopDisable(!activeCrimsonGameplay.Cheats.General.teleporterTool);
		ImGui::PopFont();
		return;
	}

	auto pool_11962 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
	if (!pool_11962 || !pool_11962[8]) {
		ImGui::PopStyleColor();
		ImGui::PopFont();
		GUI_PopDisable(!activeCrimsonGameplay.Cheats.General.teleporterTool);
		return;
	}
	auto& eventData = *reinterpret_cast<EventData*>(pool_11962[8]);

	auto pool_12013 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
	if (!pool_12013 || !pool_12013[12]) {
		ImGui::PopStyleColor();
		ImGui::PopFont();
		GUI_PopDisable(!activeCrimsonGameplay.Cheats.General.teleporterTool);
		return;
	}
	auto& nextEventData = *reinterpret_cast<NextEventData*>(pool_12013[12]);

	const float columnWidth = 0.5f * queuedConfig.globalScale;
	const float rowHeight = 40.0f * queuedConfig.globalScale;

	if (ImGui::BeginTable("TeleporterTable", 2)) {
		ImGui::TableSetupColumn("c1", 0, columnWidth * 2.0f);
		ImGui::TableSetupColumn("c2", 0, columnWidth * 2.0f);

		ImGui::TableNextRow(0, rowHeight * 0.75f);
		ImGui::TableNextColumn();

		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
		ImGui::Text("当前房间");
		ImGui::PopFont();

		ImGui::TableNextColumn();
		
		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
		ImGui::Text("下一房间");
		ImGui::PopFont();

		ImGui::TableNextRow(0, rowHeight);
		ImGui::TableNextColumn();

		ImGui::PushItemWidth(itemWidth * smallerComboMult);
		GUI_Input<uint32>("", eventData.room, 0, "%u", ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		size_t roomIndex = -1;
		for (size_t i = 0; i < ROOMS_COUNT; i++) {
			if (roomsMap[i] == eventData.room) {
				roomIndex = i;
				break;
			}
		}
		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
		if (roomIndex != -1) {
			const char* currentRoomName = roomNames[roomIndex];
			ImGui::Text("%s", currentRoomName);
		} else {
			ImGui::Text("Unknown Room (%d)", eventData.room);
		}
		ImGui::PopFont();
		
		GUI_Input<uint32>("位置", eventData.position, 0, "%u", ImGuiInputTextFlags_ReadOnly);
		ImGui::PopItemWidth();

		ImGui::TableNextColumn();

		ImGui::PushItemWidth(itemWidth * 0.6f);
		bool updatedRoom = GUI_Input<uint16>("", nextEventData.room, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushItemWidth(itemWidth * 1.3f);
		updatedRoom = UI::ComboMapValue("", roomNames, roomsMap, nextEventData.room, 0);
		ImGui::PopItemWidth();

		ImGui::SameLine();
		ImGui::Text("房间");

		ImGui::PushItemWidth(itemWidth * 1.1f);
		bool updatedPos = GUI_Input<uint16>("位置", nextEventData.position, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::PopItemWidth();

		if (GUI_Button("传送", ImVec2(150.0f * scaleFactorY, 50.0f * scaleFactorY))) {
			eventData.event = EVENT::TELEPORT;
		}

		if (GUI_Button("使用当前位置")) {
			nextEventData.room = static_cast<uint16>(eventData.room);
			nextEventData.position = static_cast<uint16>(eventData.position);
		}
		ImGui::SameLine();
		if (GUI_Button("清除")) {
			nextEventData.position = nextEventData.room = 0;
		}

		ImGui::EndTable();
	}

	GUI_PopDisable(!activeCrimsonGameplay.Cheats.General.teleporterTool);
	ImGui::PopStyleColor();
	ImGui::PopFont();
}

#pragma endregion

#pragma region Training

void TrainingSection() {
	auto defaultFontSize = UI::g_UIContext.DefaultFontSize;

	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);

	GUI_Title("训练作弊");
	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

	ImGui::PushItemWidth(itemWidth);

	float smallerComboMult = 0.7f;

	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale * 0.5f;

		if (ImGui::BeginTable("TrainingCheatsOptionsTable", 3)) {

			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			if (GUI_Checkbox2("禁用所有伤害", activeCrimsonGameplay.Cheats.Training.infiniteHP, queuedCrimsonGameplay.Cheats.Training.infiniteHP)) {
				ToggleInfiniteHitPoints(activeCrimsonGameplay.Cheats.Training.infiniteHP);
			}

			ImGui::TableNextColumn();

			if (GUI_Checkbox2("无限魔人化", activeCrimsonGameplay.Cheats.Training.infiniteDT, queuedCrimsonGameplay.Cheats.Training.infiniteDT)) {
				ToggleInfiniteMagicPoints(activeCrimsonGameplay.Cheats.Training.infiniteDT);
				//This might cause vanilla to crash I don't care though sucks to suck
				if (activeCrimsonGameplay.Cheats.Training.infiniteDT && InGame() && activeConfig.Actor.enable) {
					ForEachSpawnedPlayerActor([&](PlayerActorData& playerActor, NewActorData&, uint8 playerIndex, uint8 characterIndex,
						uint8 entityIndex) {
							playerActor.magicPoints = playerActor.maxMagicPoints;
						});
				}
				if (activeCrimsonGameplay.Cheats.Training.infiniteDT && InGame() && !activeConfig.Actor.enable) {
					PlayerActorData* actorData = GetVanillaPlayerActor();
					actorData->magicPoints = actorData->maxMagicPoints;
				}
			}

			ImGui::TableNextColumn();

			if (GUI_Checkbox2("禁用普通敌人攻击", activeCrimsonGameplay.Cheats.Training.disableRegularEnemyAttacks, queuedCrimsonGameplay.Cheats.Training.disableRegularEnemyAttacks)) {
				CrimsonPatches::DisableRegularEnemyAttacks(activeCrimsonGameplay.Cheats.Training.disableRegularEnemyAttacks);
			}
			ImGui::SameLine();
			GUI_WIPButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "并非对所有普通敌人生效。");

			ImGui::TableNextColumn();

			if (GUI_Checkbox2("禁用计时器", activeCrimsonGameplay.Cheats.Training.disableTimers, queuedCrimsonGameplay.Cheats.Training.disableTimers)) {
				ToggleDisableTimer(activeCrimsonGameplay.Cheats.Training.disableTimers);
			}
			
			if (activeCrimsonGameplay.Cheats.General.legacyDDMKCharacters) {
				ImGui::TableNextColumn();

				GUI_PushDisable(!activeCrimsonGameplay.Cheats.General.legacyDDMKCharacters);
				if (GUI_Checkbox2("无限 B. Lady 子弹", activeCrimsonGameplay.Cheats.Training.infiniteBossLadyBullets, 
					queuedCrimsonGameplay.Cheats.Training.infiniteBossLadyBullets)) {
					ToggleInfiniteBullets(activeCrimsonGameplay.Cheats.Training.infiniteBossLadyBullets);
				}
				ImGui::SameLine();
				GUI_PopDisable(!activeCrimsonGameplay.Cheats.General.legacyDDMKCharacters);
				TooltipHelper("(?)", "用于 Boss Lady（旧版 DDMK 角色）。");
			}
			

			ImGui::EndTable();
		}
	}

	ImGui::PopItemWidth();
	ImGui::PopStyleColor();
	ImGui::PopFont();

	ImGui::Text("");
    
}

#pragma endregion

#pragma region Training

void SoundSection(size_t defaultFontSize) {
	const char* changeGunNewNames[] = { "DMC3 默认", "Crimson" };
	const char* changeDevilArmNewNames[] = { "DMC3 默认", "Crimson" };

	const char* styleSwitchNewNames[] = { "DMC3 切换", "Crimson" };

	float smallerComboMult = 0.7f;

	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);

	ImGui::Text("Crimson 音效选项");

	ImGui::PopFont();

	UI::SeparatorEx(defaultFontSize * 23.35f);

	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

	ImGui::PushItemWidth(itemWidth);
	ImGui::Text("");

	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;

		if (ImGui::BeginTable("WeaponwheelSFXTable", 3)) {
			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			ImGui::TableNextRow(0, rowWidth * 0.5f);
			ImGui::TableNextColumn();

			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("武器轮盘音效");
			ImGui::PopFont();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			UI::Combo2<uint8>("切换枪械", changeGunNewNames, activeCrimsonConfig.SFX.changeGunNew, queuedCrimsonConfig.SFX.changeGunNew);
			ImGui::PopItemWidth();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			UI::Combo2<uint8>("切换魔具", changeDevilArmNewNames, activeCrimsonConfig.SFX.changeDevilArmNew, queuedCrimsonConfig.SFX.changeDevilArmNew);
			ImGui::PopItemWidth();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_Slider2<uint8>("音量", activeCrimsonConfig.SFX.changeWeaponEffectVolume, queuedCrimsonConfig.SFX.changeWeaponEffectVolume, 0, 100);
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("风格切换音效");
			ImGui::PopFont();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			UI::Combo2<uint8>("风格切换特效", styleSwitchNewNames, activeCrimsonConfig.SFX.styleChangeNew, queuedCrimsonConfig.SFX.styleChangeNew);
			ImGui::PopItemWidth();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_Slider2<uint8>("特效音量", activeCrimsonConfig.SFX.styleChangeVolume, queuedCrimsonConfig.SFX.styleChangeVolume, 0, 100);
			ImGui::PopItemWidth();
			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_Slider2<uint8>("切换音量", activeCrimsonConfig.SFX.styleChangeSnapVolume, queuedCrimsonConfig.SFX.styleChangeSnapVolume, 0, 100);
			ImGui::PopItemWidth();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_Slider2<uint8>("语音音量", activeCrimsonConfig.SFX.styleChangeVoiceOverVolume, queuedCrimsonConfig.SFX.styleChangeVoiceOverVolume, 0, 100);
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("风格评级播报");
			ImGui::PopFont();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_Slider2<uint8>("音量", activeCrimsonConfig.SFX.announcerVolume, queuedCrimsonConfig.SFX.announcerVolume, 0, 100);
			ImGui::PopItemWidth();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_InputDefault2<uint32>("冷却", activeCrimsonConfig.SFX.styleRankAnnouncerCooldownSec, queuedCrimsonConfig.SFX.styleRankAnnouncerCooldownSec, defaultCrimsonConfig.SFX.styleRankAnnouncerCooldownSec, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::SameLine();
			TooltipHelper("(?)", "Duration until the Announcer can repeat the same line.\n\n(in Seconds)");
			ImGui::PopItemWidth();

			GUI_Checkbox2("仅受击时重置冷却", activeCrimsonConfig.SFX.onlyResetAnnouncerWhenHit, queuedCrimsonConfig.SFX.onlyResetAnnouncerWhenHit);
			ImGui::SameLine();
			TooltipHelper("(?)", "模仿 DMC5：仅在冷却结束且被敌人击中时真正重置播报");

			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			ImGui::Text("");
			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("魔人化音效");
			ImGui::PopFont();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_Slider2<uint8>("入 L1 音量", activeCrimsonConfig.SFX.dTInL1Volume, queuedCrimsonConfig.SFX.dTInL1Volume, 0, 100);
			ImGui::PopItemWidth();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_Slider2<uint8>("入 L2 音量", activeCrimsonConfig.SFX.dTInL2Volume, queuedCrimsonConfig.SFX.dTInL2Volume, 0, 100);
			ImGui::PopItemWidth();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_Slider2<uint8>("退出音量", activeCrimsonConfig.SFX.dTOutVolume, queuedCrimsonConfig.SFX.dTOutVolume, 0, 100);
			ImGui::PopItemWidth();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_Slider2<uint8>("魔人待命音量", activeCrimsonConfig.SFX.dTReadyVolume, queuedCrimsonConfig.SFX.dTReadyVolume, 0, 100);
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			ImGui::Text("");

			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("分身音效");
			ImGui::PopFont();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_Slider2<uint8>("进入音量", activeCrimsonConfig.SFX.doppelInVolume, queuedCrimsonConfig.SFX.doppelInVolume, 0, 100);
			ImGui::PopItemWidth();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_Slider2<uint8>("退出音量", activeCrimsonConfig.SFX.doppelOutVolume, queuedCrimsonConfig.SFX.doppelOutVolume, 0, 100);
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			ImGui::Text("");

			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("水银音效");
			ImGui::PopFont();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_Slider2<uint8>("进入音量", activeCrimsonConfig.SFX.quickInVolume, queuedCrimsonConfig.SFX.quickInVolume, 0, 100);
			ImGui::PopItemWidth();

			ImGui::PushItemWidth(itemWidth* smallerComboMult);
			GUI_Slider2<uint8>("退出音量", activeCrimsonConfig.SFX.quickOutVolume, queuedCrimsonConfig.SFX.quickOutVolume, 0, 100);
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			ImGui::Text("");

			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("延迟连击音效");
			ImGui::PopFont();

			ImGui::PushItemWidth(itemWidth* smallerComboMult);
			UI::ComboMapValue2("音效类型",
				delayedComboSFXNames,
				delayedComboSFXMap,
				activeCrimsonConfig.SFX.delayedComboEffectType,
				queuedCrimsonConfig.SFX.delayedComboEffectType);

			GUI_Slider2<uint8>("音量", activeCrimsonConfig.SFX.delayedComboIndicatorVolume, queuedCrimsonConfig.SFX.delayedComboIndicatorVolume, 0, 100);
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();
			ImGui::Text("");

			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("皇家护卫音效");
			ImGui::PopFont();

			ImGui::PushItemWidth(itemWidth* smallerComboMult);
			GUI_Slider2<uint8>("皇家格挡音量", activeCrimsonConfig.SFX.royalBlockVolume, queuedCrimsonConfig.SFX.royalBlockVolume, 0, 100);
			ImGui::PopItemWidth();

			ImGui::PushItemWidth(itemWidth* smallerComboMult);
			GUI_Slider2<uint8>("普通格挡音量", activeCrimsonConfig.SFX.normalBlockVolume, queuedCrimsonConfig.SFX.normalBlockVolume, 0, 100);
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();
			ImGui::Text("");

			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("魔人爆发音效");
			ImGui::PopFont();

			ImGui::PushItemWidth(itemWidth* smallerComboMult);
			GUI_Slider2<uint8>("Start/Loop Volume", activeCrimsonConfig.SFX.dTEStartLoopVolume, queuedCrimsonConfig.SFX.dTEStartLoopVolume, 0, 100);
			ImGui::PopItemWidth();

			ImGui::PushItemWidth(itemWidth* smallerComboMult);
			GUI_Slider2<uint8>("终结音量", activeCrimsonConfig.SFX.dTEFinishVolume, queuedCrimsonConfig.SFX.dTEFinishVolume, 0, 100);
			ImGui::PopItemWidth();

			ImGui::PushItemWidth(itemWidth* smallerComboMult);
			GUI_Slider2<uint8>("释放音量", activeCrimsonConfig.SFX.dTEReleaseVolume, queuedCrimsonConfig.SFX.dTEReleaseVolume, 0, 100);
			ImGui::PopItemWidth();

			ImGui::EndTable();
		}
	}

	ImGui::Text("");

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);

	ImGui::Text("游戏内音量");

	ImGui::PopFont();

	UI::SeparatorEx(defaultFontSize * 23.35f);

	ImGui::Text("");

	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;

		if (ImGui::BeginTable("InGameVolumesTable", 3)) {
			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			ImGui::TableNextRow(0, rowWidth * 0.1f);

			for (int channelIndex = 0; channelIndex < CHANNEL::MAX; channelIndex++) {
				if (channelIndex % 3 == 0) {
					ImGui::TableNextRow(0, rowWidth * 0.1f);
				}

				ImGui::TableNextColumn();

				ImGui::PushItemWidth(itemWidth* smallerComboMult);
				if (GUI_Slider2<uint8>(Sound_channelNames[channelIndex],
					activeCrimsonConfig.Sound.channelVolumes[channelIndex], queuedCrimsonConfig.Sound.channelVolumes[channelIndex], 0, 100)) {
					SetVolume(channelIndex, activeCrimsonConfig.Sound.channelVolumes[channelIndex] / 100.0f);
				}

				ImGui::PopItemWidth();
			}

			ImGui::EndTable();
		}
	}

	GUI_Checkbox2("忽略敌人数据", activeConfig.soundIgnoreEnemyData, queuedConfig.soundIgnoreEnemyData);
	ImGui::SameLine();
	TooltipHelper("(?)", "Do not look at enemy data when updating the global indices.\nMost, if not all enemies will lose their sound effects if enabled.\nIntended as a workaround for playable bosses when the sound effect\ninterferences from other enemies get too annoying. - serpentiem");

	GUI_Checkbox2("覆盖任务开始音乐", activeCrimsonConfig.Sound.overrideMissionStartSong, queuedCrimsonConfig.Sound.overrideMissionStartSong);
	ImGui::SameLine();
	GUI_CCSRequirementButton();
	ImGui::SameLine();
	TooltipHelper("(?)", "在自定义菜单中，用女神像音乐覆盖任务开始音乐。");

	GUI_Checkbox2("Crimson 模式更改音乐", activeCrimsonConfig.Sound.crimsonModeChangesMusic, queuedCrimsonConfig.Sound.crimsonModeChangesMusic);
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("仅更改任务结算画面音乐。");
	}
	ImGui::SameLine();
	GUI_CCSRequirementButton();

	ImGui::Text("");

	ImGui::PopFont();
	ImGui::PopStyleColor();
}

const char* dergilNames[] = {
	"默认",
	"强制关闭",
	"强制开启",
};

void VisualSection(size_t defaultFontSize) {

    float smallerComboMult = 0.7f;

	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);


	ImGui::Text("特效颜色");

	ImGui::PopFont();

	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

	UI::SeparatorEx(defaultFontSize * 23.35f);

	
	ImGui::Text("");

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
    GUI_Checkbox2("风格切换特效", activeCrimsonConfig.StyleSwitchFX.Flux.enable, queuedCrimsonConfig.StyleSwitchFX.Flux.enable);
	ImGui::PopFont();

	ImGui::SameLine();
	ImGui::PushItemWidth(itemWidth);
	UI::ComboMapValue2("类型", styleSwitchTypeNames, styleSwitchTypeMap,
		activeCrimsonConfig.StyleSwitchFX.Flux.type, queuedCrimsonConfig.StyleSwitchFX.Flux.type);
	ImGui::PopItemWidth();

	for (int style = 0; style < 6; style++) {
	
		if (style > 0) {
			ImGui::SameLine();
		}
		GUI_Color2("", activeCrimsonConfig.StyleSwitchFX.Flux.fluxColor[style], queuedCrimsonConfig.StyleSwitchFX.Flux.fluxColor[style]);
		ImGui::SameLine();
		ImGui::Text(styleNamesFX[style]);
	}

	if (activeCrimsonConfig.StyleSwitchFX.Flux.type == STYLESWITCHVFXTYPE::CRIMSON) {
		ImGui::SameLine();
		GUI_Checkbox2("自定义挥砍颜色", activeCrimsonConfig.StyleSwitchFX.Flux.customSwooshColors,
			queuedCrimsonConfig.StyleSwitchFX.Flux.customSwooshColors);

		if (activeCrimsonConfig.StyleSwitchFX.Flux.customSwooshColors) {
			for (int style = 0; style < 6; style++) {

				if (style > 0) {
					ImGui::SameLine();
				}
				GUI_Color2("", activeCrimsonConfig.StyleSwitchFX.Flux.fluxSwooshColor[style], queuedCrimsonConfig.StyleSwitchFX.Flux.fluxSwooshColor[style]);
				ImGui::SameLine();
				ImGui::Text(styleNamesFX[style]);
			}
		}
	}

	if (GUI_Button("默认颜色")) {
		CopyMemory(&queuedCrimsonConfig.StyleSwitchFX.Flux.fluxColor, &colorPresets.StyleSwitchFlux.defaultColors, sizeof(queuedCrimsonConfig.StyleSwitchFX.Flux.fluxColor));
		CopyMemory(&activeCrimsonConfig.StyleSwitchFX.Flux.fluxColor, &colorPresets.StyleSwitchFlux.defaultColors, sizeof(activeCrimsonConfig.StyleSwitchFX.Flux.fluxColor));
	}

	ImGui::SameLine();
	if (GUI_Button("DMC3 切换")) {
		CopyMemory(&queuedCrimsonConfig.StyleSwitchFX.Flux.fluxColor, &colorPresets.StyleSwitchFlux.dMC3Switch, sizeof(queuedCrimsonConfig.StyleSwitchFX.Flux.fluxColor));
		CopyMemory(&activeCrimsonConfig.StyleSwitchFX.Flux.fluxColor, &colorPresets.StyleSwitchFlux.dMC3Switch, sizeof(activeCrimsonConfig.StyleSwitchFX.Flux.fluxColor));
	}

    ImGui::SameLine();
	if (GUI_Button("全红")) {
		CopyMemory(&queuedCrimsonConfig.StyleSwitchFX.Flux.fluxColor, &colorPresets.StyleSwitchFlux.allRed, sizeof(queuedCrimsonConfig.StyleSwitchFX.Flux.fluxColor));
		CopyMemory(&activeCrimsonConfig.StyleSwitchFX.Flux.fluxColor, &colorPresets.StyleSwitchFlux.allRed, sizeof(activeCrimsonConfig.StyleSwitchFX.Flux.fluxColor));
	}

	
	ImGui::Text("");

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
    GUI_Checkbox2("风格切换文字", activeCrimsonConfig.StyleSwitchFX.Text.textEnabled, queuedCrimsonConfig.StyleSwitchFX.Text.textEnabled);
	ImGui::PopFont();

	for (int style = 0; style < 9; style++) {
		if (style > 0) {
			ImGui::SameLine();
		}
		GUI_Color2("", activeCrimsonConfig.StyleSwitchFX.Text.color[style], queuedCrimsonConfig.StyleSwitchFX.Text.color[style]);
		ImGui::SameLine();
		ImGui::Text(styleNamesFX[style]);
	}

	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;


		if (ImGui::BeginTable("StyleSwitchTextPropertiesTable", 2)) {

			ImGui::TableSetupColumn("b1", 0, columnWidth * 0.5f);
			ImGui::TableNextRow(0, rowWidth * 0.1f);

            ImGui::TableNextColumn();
			
			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_InputDefault2<float>("透明度", activeCrimsonConfig.StyleSwitchFX.Text.maxAlpha, queuedCrimsonConfig.StyleSwitchFX.Text.maxAlpha,
				defaultCrimsonConfig.StyleSwitchFX.Text.maxAlpha, 0.1f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

            ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_InputDefault2<float>("大小", activeCrimsonConfig.StyleSwitchFX.Text.size, queuedCrimsonConfig.StyleSwitchFX.Text.size,
				defaultCrimsonConfig.StyleSwitchFX.Text.size, 0.1f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

			ImGui::EndTable();
		}
	}
	

	if (GUI_Button("午夜")) {
		CopyMemory(&queuedCrimsonConfig.StyleSwitchFX.Text.color, &colorPresets.StyleSwitchText.midnight, sizeof(queuedCrimsonConfig.StyleSwitchFX.Text.color));
		CopyMemory(&activeCrimsonConfig.StyleSwitchFX.Text.color, &colorPresets.StyleSwitchText.midnight, sizeof(activeCrimsonConfig.StyleSwitchFX.Text.color));
	}

    ImGui::SameLine();
	if (GUI_Button("全白")) {
		CopyMemory(&queuedCrimsonConfig.StyleSwitchFX.Text.color, &colorPresets.StyleSwitchText.allWhite, sizeof(queuedCrimsonConfig.StyleSwitchFX.Text.color));
		CopyMemory(&activeCrimsonConfig.StyleSwitchFX.Text.color, &colorPresets.StyleSwitchText.allWhite, sizeof(activeCrimsonConfig.StyleSwitchFX.Text.color));
	}

    ImGui::SameLine();
	if (GUI_Button("彩色通透")) {
		CopyMemory(&queuedCrimsonConfig.StyleSwitchFX.Text.color, &colorPresets.StyleSwitchText.colorfulClear, sizeof(queuedCrimsonConfig.StyleSwitchFX.Text.color));
		CopyMemory(&activeCrimsonConfig.StyleSwitchFX.Text.color, &colorPresets.StyleSwitchText.colorfulClear, sizeof(activeCrimsonConfig.StyleSwitchFX.Text.color));
	}

	ImGui::SameLine();
	if (GUI_Button("彩色")) {
		CopyMemory(&queuedCrimsonConfig.StyleSwitchFX.Text.color, &colorPresets.StyleSwitchText.colorful, sizeof(queuedCrimsonConfig.StyleSwitchFX.Text.color));
		CopyMemory(&activeCrimsonConfig.StyleSwitchFX.Text.color, &colorPresets.StyleSwitchText.colorful, sizeof(activeCrimsonConfig.StyleSwitchFX.Text.color));
	}

    ImGui::Text("");

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
	ImGui::Text("魔人化特效");
	ImGui::PopFont();

	for (int dt = 0; dt < 5; dt++) {
		if (dt > 0) {
			ImGui::SameLine();
		}
        GUI_Color2("", activeConfig.Color.Aura.dante[dt], queuedConfig.Color.Aura.dante[dt]);
		ImGui::SameLine();
		ImGui::Text(meleeWeaponNamesDante[dt]);
	}
    ImGui::SameLine();
	GUI_Color2("", activeConfig.Color.Aura.sparda, queuedConfig.Color.Aura.sparda);
    ImGui::SameLine();
    ImGui::Text("斯巴达");

	for (int dt = 0; dt < 3; dt++) {
		if (dt > 0) {
			ImGui::SameLine();
		}
		GUI_Color2("", activeConfig.Color.Aura.vergil[dt], queuedConfig.Color.Aura.vergil[dt]);
		ImGui::SameLine();
		ImGui::Text(meleeWeaponNamesVergil[dt]);
	}
    ImGui::SameLine();
	GUI_Color2("", activeConfig.Color.Aura.neroAngelo, queuedConfig.Color.Aura.neroAngelo);
	ImGui::SameLine();
	ImGui::Text("黑天使");

	ImGui::Text("");

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
	ImGui::Text("空中跳跃");
	ImGui::PopFont();

	for (int airhike = 0; airhike < 5; airhike++) {
		if (airhike > 0) {
			ImGui::SameLine();
		}
		GUI_Color2("", activeConfig.Color.airHike[airhike], queuedConfig.Color.airHike[airhike]);
		ImGui::SameLine();
		ImGui::Text(meleeWeaponNamesDante[airhike]);
	}

    ImGui::Text("");

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
	ImGui::Text("其他招式");
	ImGui::PopFont();

	GUI_Color2("", activeConfig.Color.Trickster.skyStar, queuedConfig.Color.Trickster.skyStar);
	ImGui::SameLine();
	ImGui::Text("空中之星");

    ImGui::SameLine();
	GUI_Color2("", activeConfig.Color.Royalguard.ultimate, queuedConfig.Color.Royalguard.ultimate);
	ImGui::SameLine();
	ImGui::Text("皇家护卫·终极");

    ImGui::SameLine();
	GUI_Color2("", activeConfig.Color.Doppelganger.clone, queuedConfig.Color.Doppelganger.clone);
	ImGui::SameLine();
	ImGui::Text("分身");


	if (GUI_Button("DMC3 默认")) {
		CopyMemory(&queuedConfig.Color, &colorPresets.ColorDMC3Default, sizeof(queuedConfig.Color));
		CopyMemory(&activeConfig.Color, &colorPresets.ColorDMC3Default, sizeof(activeConfig.Color));
	}

    ImGui::SameLine();
	if (GUI_Button("Crimson")) {
		CopyMemory(&queuedConfig.Color, &colorPresets.ColorCrimson, sizeof(queuedConfig.Color));
		CopyMemory(&activeConfig.Color, &colorPresets.ColorCrimson, sizeof(activeConfig.Color));
	}
    

	ImGui::Text("");

	GUI_Title("Crimson 特效选项");

	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;


		if (ImGui::BeginTable("MiscVisualOptionsTable", 3)) {

			ImGui::TableSetupColumn("h1", 0, columnWidth * 2.5f);
			ImGui::TableNextRow(0, rowWidth * 0.1f);

			ImGui::TableNextColumn();

			GUI_Checkbox2("Delayed Combo VFX / Vibration", activeCrimsonConfig.VFX.delayedComboVFX, queuedCrimsonConfig.VFX.delayedComboVFX);

			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			GUI_Color2("", activeCrimsonConfig.VFX.delayedComboColor, queuedCrimsonConfig.VFX.delayedComboColor);
			ImGui::SameLine();
			if (GUI_Button("D")) {
				CopyMemory(&activeCrimsonConfig.VFX.delayedComboColor, &defaultCrimsonConfig.VFX.delayedComboColor,
					sizeof(activeCrimsonConfig.VFX.delayedComboColor));
				CopyMemory(&queuedCrimsonConfig.VFX.delayedComboColor, &defaultCrimsonConfig.VFX.delayedComboColor,
					sizeof(queuedCrimsonConfig.VFX.delayedComboColor));
			}

			ImGui::TableNextColumn();

			GUI_Checkbox2("皇家格挡特效", activeCrimsonConfig.VFX.royalBlockVFX, queuedCrimsonConfig.VFX.royalBlockVFX);

			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			GUI_Color2("", activeCrimsonConfig.VFX.royalBlockColor, queuedCrimsonConfig.VFX.royalBlockColor);
			ImGui::SameLine();
			if (GUI_Button("D")) {
				CopyMemory(&activeCrimsonConfig.VFX.royalBlockColor, &defaultCrimsonConfig.VFX.royalBlockColor,
					sizeof(activeCrimsonConfig.VFX.royalBlockColor));
				CopyMemory(&queuedCrimsonConfig.VFX.royalBlockColor, &defaultCrimsonConfig.VFX.royalBlockColor,
					sizeof(queuedCrimsonConfig.VFX.royalBlockColor));
			}

			ImGui::TableNextColumn();

			GUI_Checkbox2("魔人爆发特效", activeCrimsonConfig.VFX.dtExplosionVFX, queuedCrimsonConfig.VFX.dtExplosionVFX);

			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			GUI_Color2("", activeCrimsonConfig.VFX.dtExplosionColorDante, queuedCrimsonConfig.VFX.dtExplosionColorDante);
			ImGui::SameLine();
			GUI_Color2("", activeCrimsonConfig.VFX.dtExplosionColorVergil, queuedCrimsonConfig.VFX.dtExplosionColorVergil);
			ImGui::SameLine();
			if (GUI_Button("D")) {
				CopyMemory(&activeCrimsonConfig.VFX.dtExplosionColorDante, &defaultCrimsonConfig.VFX.dtExplosionColorDante,
					sizeof(activeCrimsonConfig.VFX.dtExplosionColorDante));
				CopyMemory(&queuedCrimsonConfig.VFX.dtExplosionColorDante, &defaultCrimsonConfig.VFX.dtExplosionColorDante,
					sizeof(queuedCrimsonConfig.VFX.dtExplosionColorDante));
				
				CopyMemory(&activeCrimsonConfig.VFX.dtExplosionColorVergil, &defaultCrimsonConfig.VFX.dtExplosionColorVergil,
					sizeof(activeCrimsonConfig.VFX.dtExplosionColorVergil));
				CopyMemory(&queuedCrimsonConfig.VFX.dtExplosionColorVergil, &defaultCrimsonConfig.VFX.dtExplosionColorVergil,
					sizeof(queuedCrimsonConfig.VFX.dtExplosionColorVergil));
			}

			ImGui::TableNextColumn();

			GUI_Checkbox2("魔人激活震动", activeCrimsonConfig.VFX.dtActivationVibration, queuedCrimsonConfig.VFX.dtActivationVibration);

			ImGui::SameLine();
			GUI_CCSRequirementButton();


			ImGui::PopItemWidth();

			ImGui::EndTable();
		}
	}
	
	ImGui::Text("");


	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);


	ImGui::Text("杂项");

	ImGui::PopFont();

	UI::SeparatorEx(defaultFontSize * 23.35f);
    

    //GUI_Color2("FluxColorTrick", activeConfig.FluxColor.trick, queuedConfig.FluxColor.trick, defaultConfig.FluxColor.trick);


	ImGui::Text("");

	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;


		if (ImGui::BeginTable("MiscVisualOptionsTable", 3)) {

			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			ImGui::TableNextRow(0, rowWidth * 0.1f);

            ImGui::TableNextColumn();

            ImGui::PushItemWidth(itemWidth * smallerComboMult);
            GUI_Checkbox2("但丁隐藏贝奥武夫", activeConfig.hideBeowulfDante, queuedConfig.hideBeowulfDante);
            ImGui::PopItemWidth();

            ImGui::TableNextColumn();

            ImGui::PushItemWidth(itemWidth * smallerComboMult);
            GUI_Checkbox2("维吉尔隐藏贝奥武夫", activeConfig.hideBeowulfVergil, queuedConfig.hideBeowulfVergil);
            ImGui::PopItemWidth();

            ImGui::TableNextColumn();
		
			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			if (GUI_Checkbox2("魔人化无恶魔外观", activeConfig.noDevilForm, queuedConfig.noDevilForm)) {
				ToggleNoDevilForm(activeConfig.noDevilForm);
			}
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_Checkbox2("精细布料物理", activeCrimsonConfig.Visual.clothPhysicsEnhancement, queuedCrimsonConfig.Visual.clothPhysicsEnhancement);
			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Relaxes cloth physics slightly, giving them smoother and more natural movement,\n"
					"also reduces Dante's coat weight. Can cause more clipping.");
			}
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			GUI_Checkbox2("更多嘲讽动作", activeCrimsonConfig.Visual.moreTauntsAnimations, queuedCrimsonConfig.Visual.moreTauntsAnimations);
			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("引入其他 DMC 游戏的多个嘲讽动作，通过伪随机循环触发。");
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();

            ImGui::TableNextRow(0, rowWidth * 0.1f);
            ImGui::TableNextColumn();
            
			ImGui::PushItemWidth(itemWidth * smallerComboMult);
            UI::Combo2("点状阴影", dotShadowNames, activeConfig.dotShadow, queuedConfig.dotShadow);
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();
			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			if (UI::Combo2("红衣维吉尔 Boss", dergilNames, activeConfig.dergil, queuedConfig.dergil)) {
				ToggleDergil(activeConfig.dergil);
			}
			ImGui::PopItemWidth();

			ImGui::EndTable();
		}
	}


    ImGui::PopFont();
    ImGui::PopStyleColor();	
}

// General gameplay options section
void GeneralGameplayOptions() {
	auto& defaultFontSize = UI::g_UIContext.DefaultFontSize;

	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);

	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);
	ImGui::Text("通用玩法选项");
	ImGui::PopFont();
	UI::SeparatorEx(defaultFontSize * 23.35f);
	ImGui::Text("");

	// General Options Section
	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowHeight = 40.0f * queuedConfig.globalScale;

		if (ImGui::BeginTable("GameplayOptionsTable", 3)) {
			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			ImGui::TableSetupColumn("b2", 0, columnWidth * 2.0f);
			ImGui::TableSetupColumn("b3", 0, columnWidth * 2.0f);

			// First row
			ImGui::TableNextRow(0, rowHeight * 0.5f);
			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("惯性",
				activeCrimsonGameplay.Gameplay.General.inertia,
				queuedCrimsonGameplay.Gameplay.General.inertia,
				activeCrimsonGameplayMask.Gameplay.General.inertia)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Changes how physics behave during almost all aerial moves. Spices up your combos with more aerial momentum variance.\n"
				"启用格挡飞行与子弹吸附。");
			GUI_PopDisable(!activeConfig.Actor.enable);

			//ImGui::TableNextColumn();

			//GUI_PushDisable(!activeConfig.Actor.enable);
			//if (GUI_Checkbox2("冲刺",
			//	activeCrimsonGameplay.Gameplay.General.sprint,
			//	queuedCrimsonGameplay.Gameplay.General.sprint,
			//	activeCrimsonGameplayMask.Gameplay.General.sprint)) {
			//}
			//ImGui::SameLine();
			//GUI_CCSRequirementButton();
			//ImGui::SameLine();
			//TooltipHelper("(?)", "脱离战斗时冲刺，类似 DMC4/5 的加速能力。");
			//GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("扩展招式表",
				activeCrimsonGameplay.Gameplay.General.extramoves,
				queuedCrimsonGameplay.Gameplay.General.extramoves,
				activeCrimsonGameplayMask.Gameplay.General.extramoves)) {
			}

			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "为但丁和维吉尔启用可在商店购买的新招式。");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();


			if (GUI_Checkbox2("按住射击",
				activeCrimsonGameplay.Gameplay.General.holdToShoot,
				queuedCrimsonGameplay.Gameplay.General.holdToShoot,
				activeCrimsonGameplayMask.Gameplay.General.holdToShoot)) {
				CrimsonPatches::HoldToAutoFire(activeCrimsonGameplay.Gameplay.General.holdToShoot);
			}

			ImGui::TableNextColumn();

			if (activeCrimsonGameplay.Gameplay.General.holdToCrazyCombo && (activeCrimsonGameplay.Gameplay.General.crazyComboMashRequirement != 3 ||
				queuedCrimsonGameplay.Gameplay.General.crazyComboMashRequirement != 3)) {
				activeCrimsonGameplay.Gameplay.General.crazyComboMashRequirement = 3;
				queuedCrimsonGameplay.Gameplay.General.crazyComboMashRequirement = 3;
				UpdateCrazyComboLevelMultiplier();
			}

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("按住疯狂连击",
				activeCrimsonGameplay.Gameplay.General.holdToCrazyCombo,
				queuedCrimsonGameplay.Gameplay.General.holdToCrazyCombo,
				activeCrimsonGameplayMask.Gameplay.General.holdToCrazyCombo)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			GUI_PushDisable(activeCrimsonGameplay.Gameplay.General.holdToCrazyCombo);
			ImGui::PushItemWidth(itemWidth * 0.8f);
			if (GUI_InputDefault2("疯狂连击连按需求",
				activeCrimsonGameplay.Gameplay.General.crazyComboMashRequirement,
				queuedCrimsonGameplay.Gameplay.General.crazyComboMashRequirement,
				defaultCrimsonGameplay.Gameplay.General.crazyComboMashRequirement)) {
				UpdateCrazyComboLevelMultiplier();
			}
			ImGui::PopItemWidth();
			ImGui::SameLine();
			TooltipHelper("(?)", "执行疯狂连击所需连按强度的倍率，数值越低越容易。");
			GUI_PopDisable(activeCrimsonGameplay.Gameplay.General.holdToCrazyCombo);

			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("自由软锁定",
				activeCrimsonGameplay.Gameplay.General.freeformSoftLock,
				queuedCrimsonGameplay.Gameplay.General.freeformSoftLock,
				activeCrimsonGameplayMask.Gameplay.General.freeformSoftLock)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "未锁定时绕过自动转向的软锁定，启用无预输入反击。");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			if (!activeCrimsonGameplay.Gameplay.General.freeformSoftLock) {
				activeCrimsonGameplay.Gameplay.General.bufferlessReversals = false;
				queuedCrimsonGameplay.Gameplay.General.bufferlessReversals = false;
			}

			GUI_PushDisable(!activeConfig.Actor.enable || !activeCrimsonGameplay.Gameplay.General.freeformSoftLock);
			if (GUI_Checkbox2("无预输入反击",
				activeCrimsonGameplay.Gameplay.General.bufferlessReversals,
				queuedCrimsonGameplay.Gameplay.General.bufferlessReversals,
				activeCrimsonGameplayMask.Gameplay.General.bufferlessReversals)) {
				activeCrimsonGameplay.Gameplay.General.freeformSoftLock = true;
				queuedCrimsonGameplay.Gameplay.General.freeformSoftLock = true;
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "允许在不预先输入招式的情况下执行反击。");
			GUI_PopDisable(!activeConfig.Actor.enable || !activeCrimsonGameplay.Gameplay.General.freeformSoftLock);

			
			if (activeCrimsonGameplay.Gameplay.General.bufferlessReversals) {
				ImGui::TableNextColumn();

             ImGui::PushItemWidth(itemWidth * 0.8f);
				GUI_Slider2FloatDefault("反击窗口",
					activeCrimsonGameplay.Gameplay.General.reversalWindow,
					queuedCrimsonGameplay.Gameplay.General.reversalWindow,
					defaultCrimsonGameplay.Gameplay.General.reversalWindow,
					100.0f, 300.0f, 5.0f, "%.0f");
				ImGui::SameLine();
				TooltipHelper("(?)", "执行无预输入反击的时间窗口（毫秒）。");
				ImGui::PopItemWidth();
			}

			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("强化预输入反击",
				activeCrimsonGameplay.Gameplay.General.improvedBufferedReversals,
				queuedCrimsonGameplay.Gameplay.General.improvedBufferedReversals,
				activeCrimsonGameplayMask.Gameplay.General.improvedBufferedReversals)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "提高攻击时的转向速度，更易完成预输入反击。");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			if (GUI_Checkbox2("DMC4/5 Lock On Direction",
				activeCrimsonGameplay.Gameplay.General.dmc4LockOnDirection,
				queuedCrimsonGameplay.Gameplay.General.dmc4LockOnDirection,
				activeCrimsonGameplayMask.Gameplay.General.dmc4LockOnDirection)) {
				CrimsonDetours::ToggleDMC4LockOnDirection(activeCrimsonGameplay.Gameplay.General.dmc4LockOnDirection);
			}
			ImGui::SameLine();
			TooltipHelper("(?)", "方向招式采用敌人镜头方向而非玩家镜头方向。");

			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("扩大跳跃取消判定框",
				activeCrimsonGameplay.Gameplay.General.increasedJCSpheres,
				queuedCrimsonGameplay.Gameplay.General.increasedJCSpheres,
				activeCrimsonGameplayMask.Gameplay.General.increasedJCSpheres)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "扩大判定框，更易跳跃取消。");
			GUI_PopDisable(!activeConfig.Actor.enable);


			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("禁用高度限制",
				activeCrimsonGameplay.Gameplay.General.disableHeightRestriction,
				queuedCrimsonGameplay.Gameplay.General.disableHeightRestriction,
				activeCrimsonGameplayMask.Gameplay.General.disableHeightRestriction)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "允许在低空也能使用空中招式。");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("禁用跳跃取消限制",
				activeCrimsonGameplay.Gameplay.General.disableJCRestriction,
				queuedCrimsonGameplay.Gameplay.General.disableJCRestriction,
				activeCrimsonGameplayMask.Gameplay.General.disableJCRestriction)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "禁用跳跃取消的冷却。");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			if (GUI_Checkbox2("延长敌人浮空时间",
				activeCrimsonGameplay.Gameplay.General.increasedEnemyJuggleTime,
				queuedCrimsonGameplay.Gameplay.General.increasedEnemyJuggleTime,
				activeCrimsonGameplayMask.Gameplay.General.increasedEnemyJuggleTime)) {
				CrimsonPatches::ToggleIncreasedEnemyJuggleTime(activeCrimsonGameplay.Gameplay.General.increasedEnemyJuggleTime);
			}
			ImGui::SameLine();
			TooltipHelper("(?)", "提高空中招式的硬直值，微妙改变战斗中的重力。");

			ImGui::TableNextColumn();

			GUI_Checkbox2("更快转向速度",
				activeCrimsonGameplay.Gameplay.General.fasterTurnRate,
				queuedCrimsonGameplay.Gameplay.General.fasterTurnRate,
				activeCrimsonGameplayMask.Gameplay.General.fasterTurnRate);
			
			ImGui::SameLine();
			TooltipHelper("(?)", "提高角色转向速度。");

			ImGui::TableNextColumn();
			GUI_PushDisable(activeConfig.Actor.enable);
			ImGui::PushItemWidth(itemWidth * 0.7f);
			GUI_InputDefault2("原版武器切换延迟", activeCrimsonGameplay.Gameplay.General.vanillaWeaponSwitchDelay,
				queuedCrimsonGameplay.Gameplay.General.vanillaWeaponSwitchDelay, 
				defaultCrimsonGameplay.Gameplay.General.vanillaWeaponSwitchDelay, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::SameLine();
			ImGui::PopItemWidth();
			GUI_PopDisable(activeConfig.Actor.enable);
			TooltipHelper("(?)", "Decreases the cooldown between switching weapons.\n"
				"不影响武器轮盘，且仅作用于原版游戏。");

			ImGui::TableNextColumn();
			if (GUI_Checkbox2("禁用噬魂者隐身",
				activeCrimsonGameplay.Gameplay.General.disableSoulEaterInvis,
				queuedCrimsonGameplay.Gameplay.General.disableSoulEaterInvis,
				activeCrimsonGameplayMask.Gameplay.General.disableSoulEaterInvis)) {
				CrimsonPatches::ToggleDisableSoulEaterInvis(activeCrimsonGameplay.Gameplay.General.disableSoulEaterInvis);
			}
			
			ImGui::TableNextColumn();
			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("多人伤害缩放",
				activeCrimsonGameplay.Gameplay.General.multiplayerDamageScaling,
				queuedCrimsonGameplay.Gameplay.General.multiplayerDamageScaling,
				activeCrimsonGameplayMask.Gameplay.General.multiplayerDamageScaling)) {

				if (!activeCrimsonGameplay.Cheats.General.customDamage && 
					!queuedCrimsonGameplay.Gameplay.General.multiplayerDamageScaling) {
					queuedCrimsonGameplay.Cheats.Damage.enemyReceivedDmgMult = 1.0f;
					activeCrimsonGameplay.Cheats.Damage.enemyReceivedDmgMult = 1.0f;
				}
			}
			GUI_PopDisable(!activeConfig.Actor.enable);
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Scales Enemy Damage Taken multiplier in proportion to how many Players are active in a session:\n"
				"2 Players -> 0.7\n"
				"3 Players -> 0.5\n"
				"4 Players -> 0.4");

			ImGui::TableNextColumn();
			GUI_PushDisable(!activeConfig.Actor.enable);
			GUI_Checkbox2("连续方向招式",
				activeCrimsonGameplay.Gameplay.General.consecutiveDirectionalMoves,
				queuedCrimsonGameplay.Gameplay.General.consecutiveDirectionalMoves,
				activeCrimsonGameplayMask.Gameplay.General.consecutiveDirectionalMoves);
			GUI_PopDisable(!activeConfig.Actor.enable);
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "允许连续执行刺击等方向招式，中间无需其他招式。");

			ImGui::EndTable();
		}
	}

	ImGui::Text("");

	ImGui::PopStyleColor();
	ImGui::PopFont();
}



// Dante gameplay options section
void DanteGameplayOptions() {
	auto& defaultFontSize = UI::g_UIContext.DefaultFontSize;
	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);

	// Dante Options Section
	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);
	ImGui::Text("但丁玩法选项");
	ImGui::PopFont();

	UI::SeparatorEx(defaultFontSize * 23.35f);

	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

	ImGui::Text("");

	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowHeight = 40.0f * queuedConfig.globalScale;

		if (ImGui::BeginTable("DanteOptionsTable", 3)) {
			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			ImGui::TableSetupColumn("b2", 0, columnWidth * 2.0f);
			ImGui::TableSetupColumn("b3", 0, columnWidth * 2.0f);

			// First row
			ImGui::TableNextRow(0, rowHeight * 0.5f);
			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("强化取消",
				activeCrimsonGameplay.Gameplay.Dante.improvedCancels,
				queuedCrimsonGameplay.Gameplay.Dante.improvedCancels,
				activeCrimsonGameplayMask.Gameplay.Dante.improvedCancels)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Enables a series of animation cancels for Dante, especially for moves between different styles.\n"
				"We have cautiously added each cancel, taking buffers and timers into account to ensure game balance is maintained.\n"
				"Check out the Patch Notes for more info. Replaces DDMK's Remove Busy Flag/Reset Motion State.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("空中招式调整",
				activeCrimsonGameplay.Gameplay.Dante.aerialMovesTweaks,
				queuedCrimsonGameplay.Gameplay.Dante.aerialMovesTweaks,
				activeCrimsonGameplayMask.Gameplay.Dante.aerialMovesTweaks)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "This option tweaks several aerial moves at once: \n"
				"Tweaks Aerial Rave Gravity to be similar to DMC4/5, taking weights into account.\n"
				"Tweaks Air Flicker Gravity, taking weights into account. Initial windup has less gravity than vanilla.\n"
				"Tweaks E&I Air Shots Gravity slightly, taking weights into account.\n"
				"空中舞第三段改为独立技能，锁定+前+风格触发；调整空中舞重力并计入负重。");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("下坠惯性",
				activeCrimsonGameplay.Gameplay.Dante.downertiaD,
				queuedCrimsonGameplay.Gameplay.Dante.downertiaD,
				activeCrimsonGameplayMask.Gameplay.Dante.downertiaD)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Makes the next move executed before 0.30 seconds of uptime on Air Flicker or Sky Dance have increased downwards inertia.\n"
				"Jump Cancelling won't interrupt this behavior.\n");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("霰弹枪空中射击调整",
				activeCrimsonGameplay.Gameplay.Dante.shotgunAirShotTweaks,
				queuedCrimsonGameplay.Gameplay.Dante.shotgunAirShotTweaks,
				activeCrimsonGameplayMask.Gameplay.Dante.shotgunAirShotTweaks)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "降低空中霰弹枪重力，使上升幅度更小。");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("冲击波重做",
				activeCrimsonGameplay.Gameplay.Dante.driveRework,
				queuedCrimsonGameplay.Gameplay.Dante.driveRework,
				activeCrimsonGameplayMask.Gameplay.Dante.driveRework)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Drive is now inputted by Lock On + Back to Forward + Melee, can be held for more damage and is now a three part attack like DMC4/5.\n"
				"在叛逆攻击后使用冲击波，可发动快速冲击波。");
			GUI_PopDisable(!activeConfig.Actor.enable);

			// Third row
			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("子弹停顿",
				activeCrimsonGameplay.Gameplay.Dante.bulletStop,
				queuedCrimsonGameplay.Gameplay.Dante.bulletStop,
				activeCrimsonGameplayMask.Gameplay.Dante.bulletStop)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "黑檀木与白象牙等招式的射击会阻止敌人空中移动。");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("暴雨升腾",
				activeCrimsonGameplay.Gameplay.Dante.rainstormLift,
				queuedCrimsonGameplay.Gameplay.Dante.rainstormLift,
				activeCrimsonGameplayMask.Gameplay.Dante.rainstormLift)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "暴雨会令你在空中略微上升。");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			if (GUI_Checkbox2("无限暴雨",
				activeCrimsonGameplay.Gameplay.Dante.infiniteRainstorm,
				queuedCrimsonGameplay.Gameplay.Dante.infiniteRainstorm,
				activeCrimsonGameplayMask.Gameplay.Dante.infiniteRainstorm)) {
				ToggleEbonyIvoryInfiniteRainStorm(activeCrimsonGameplay.Gameplay.Dante.infiniteRainstorm);
			}
			ImGui::SameLine();
			TooltipHelper("(?)", "使暴雨在空中无限持续。");

			// Fourth row
			ImGui::TableNextColumn();

			if (GUI_Checkbox2("黑檀木与白象牙·四人时间",
				activeCrimsonGameplay.Gameplay.Dante.foursomeTime,
				queuedCrimsonGameplay.Gameplay.Dante.foursomeTime,
				activeCrimsonGameplayMask.Gameplay.Dante.foursomeTime)) {
				ToggleEbonyIvoryFoursomeTime(activeCrimsonGameplay.Gameplay.Dante.foursomeTime);
			}
			ImGui::SameLine();
			TooltipHelper("(?)", "黑檀木与白象牙的双人时间额外发射 2 发。");

			ImGui::TableNextColumn();

			/*GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("空中刺击",
				activeCrimsonGameplay.Gameplay.Dante.airStinger,
				queuedCrimsonGameplay.Gameplay.Dante.airStinger,
				activeCrimsonGameplayMask.Gameplay.Dante.airStinger)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "叛逆：空中锁定+前+近战。");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();*/

			//GUI_PushDisable(!activeConfig.Actor.enable);
			//if (GUI_Checkbox2("空中左轮",
			//	activeCrimsonGameplay.Gameplay.Dante.airRevolver,
			//	queuedCrimsonGameplay.Gameplay.Dante.airRevolver,
			//	activeCrimsonGameplayMask.Gameplay.Dante.airRevolver)) {
			//}
			//ImGui::SameLine();
			//GUI_CCSRequirementButton();
			//ImGui::SameLine();
			//TooltipHelper("(?)", "刻耳柏洛斯：空中锁定+前+近战。");
			//GUI_PopDisable(!activeConfig.Actor.enable);

			//// Fifth row
			//ImGui::TableNextColumn();

			/*GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("空中阿耆尼与楼陀罗旋风",
				activeCrimsonGameplay.Gameplay.Dante.airAgniRudraWhirlwind,
				queuedCrimsonGameplay.Gameplay.Dante.airAgniRudraWhirlwind,
				activeCrimsonGameplayMask.Gameplay.Dante.airAgniRudraWhirlwind)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "阿耆尼与楼陀罗：空中锁定+后+近战。");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();*/

			//GUI_PushDisable(!activeConfig.Actor.enable);
			//if (GUI_Checkbox2("空中挑衅·升空",
			//	activeCrimsonGameplay.Gameplay.Dante.skyLaunchAirTaunt,
			//	queuedCrimsonGameplay.Gameplay.Dante.skyLaunchAirTaunt,
			//	activeCrimsonGameplayMask.Gameplay.Dante.skyLaunchAirTaunt)) {
			//}
			//ImGui::SameLine();
			//GUI_CCSRequirementButton();
			//ImGui::SameLine();
			//TooltipHelper("(?)", "任意武器或风格：空中按挑衅键。");
			//GUI_PopDisable(!activeConfig.Actor.enable);

			//ImGui::TableNextColumn();

			/*GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("空中龙卷",
				activeCrimsonGameplay.Gameplay.Dante.airTornado,
				queuedCrimsonGameplay.Gameplay.Dante.airTornado,
				activeCrimsonGameplayMask.Gameplay.Dante.airTornado)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "贝奥武夫：空中中位+风格。全面降低龙卷伤害，保留惯性。");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();*/

			GUI_PushDisable(!activeConfig.Actor.enable || !(activeCrimsonGameplay.Gameplay.General.extramoves && ExpConfig::missionExpDataDante.unlocks[UNLOCK_DANTE::SWORDMASTER_MODDED_MOVES]));
			if (GUI_Checkbox2("交换锤击与火山输入",
				activeCrimsonGameplay.Gameplay.Dante.swapHammerVocalnoInputs,
				queuedCrimsonGameplay.Gameplay.Dante.swapHammerVocalnoInputs,
				activeCrimsonGameplayMask.Gameplay.Dante.swapHammerVocalnoInputs)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Requires Air Tornado. Makes Hammer input become Air Lock On + Style + Forward,\n"
				"and Volcano input to be Air Lock On + Style + Back.\n"
				"Ground Volcano will still be Lock On + Forward + Style not to confuse with Real Impact.\n"
				"这将交换两个招式的解锁顺序。");
			GUI_PopDisable(!activeConfig.Actor.enable || !(activeCrimsonGameplay.Gameplay.General.extramoves && ExpConfig::missionExpDataDante.unlocks[UNLOCK_DANTE::SWORDMASTER_MODDED_MOVES]));

			/*ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("空中升龙发射",
				activeCrimsonGameplay.Gameplay.Dante.airRisingDragonLaunch,
				queuedCrimsonGameplay.Gameplay.Dante.airRisingDragonLaunch,
				activeCrimsonGameplayMask.Gameplay.Dante.airRisingDragonLaunch)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "贝奥武夫：空中锁定+后+近战。");
			GUI_PopDisable(!activeConfig.Actor.enable);*/

			// Sixth row
			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("DMC4 机动性",
				activeCrimsonGameplay.Gameplay.Dante.dmc4Mobility,
				queuedCrimsonGameplay.Gameplay.Dante.dmc4Mobility,
				activeCrimsonGameplayMask.Gameplay.Dante.dmc4Mobility)) {
				CrimsonGameplay::AdjustDMC4MobilitySettings();
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "地面空中瞬移不消耗空中瞬移次数；魔人化时空中瞬移、空中跳跃与空中之星次数设为 2。");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			/*GUI_PushDisable(!activeConfig.Actor.enable);
			if (!activeConfig.Actor.enable) {
				activeCrimsonGameplay.Gameplay.Dante.dTInfusedRoyalguard = false;
			}
			if (GUI_Checkbox2("魔人灌注皇家护卫",
				activeCrimsonGameplay.Gameplay.Dante.dTInfusedRoyalguard,
				queuedCrimsonGameplay.Gameplay.Dante.dTInfusedRoyalguard,
				activeCrimsonGameplayMask.Gameplay.Dante.dTInfusedRoyalguard)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "皇家护卫普通格挡改为消耗魔人槽，直至魔人槽偏低；破防也会转为普通格挡。");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();*/

			// Adding Air Hike section
			if (GUI_Checkbox2("空中跳跃核心技能",
				activeCrimsonGameplay.Gameplay.Dante.airHikeCoreAbility,
				queuedCrimsonGameplay.Gameplay.Dante.airHikeCoreAbility,
				activeCrimsonGameplayMask.Gameplay.Dante.airHikeCoreAbility)) {
				ToggleAirHikeCoreAbility(activeCrimsonGameplay.Gameplay.Dante.airHikeCoreAbility);
			}
			ImGui::SameLine();
			TooltipHelper("(?)", "使所有近战武器都可使用空中跳跃。");

			// Seventh row - Last weapon options
			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("备用涅梵漩涡",
				activeCrimsonGameplay.Gameplay.Dante.altNevanVortex,
				queuedCrimsonGameplay.Gameplay.Dante.altNevanVortex,
				activeCrimsonGameplayMask.Gameplay.Dante.altNevanVortex)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "持涅梵且处于魔人化：空中锁定 + 任意方向 + 近战攻击。");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("交换舞蹈突刺输入",
				activeCrimsonGameplay.Gameplay.Dante.swapDancePierceInputs,
				queuedCrimsonGameplay.Gameplay.Dante.swapDancePierceInputs,
				activeCrimsonGameplayMask.Gameplay.Dante.swapDancePierceInputs)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Swaps Dance Macabre to: Lock On + Forward + Style(Swordmaster)\n"
				"and Sword Pierce to: Lock On + Back + Style(Swordmaster)\n"
				"这将交换两个招式的解锁顺序。");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			if (GUI_Checkbox2("阿尔忒弥斯重做",
				activeCrimsonGameplay.Gameplay.Dante.artemisRework,
				queuedCrimsonGameplay.Gameplay.Dante.artemisRework,
				activeCrimsonGameplayMask.Gameplay.Dante.artemisRework)) {
				CrimsonDetours::ToggleArtemisInstantFullCharge(activeCrimsonGameplay.Gameplay.Dante.artemisRework);
				CrimsonPatches::ReduceArtemisProjectileDamage(activeCrimsonGameplay.Gameplay.Dante.artemisRework);
			}
			ImGui::SameLine();
			TooltipHelper("(?)", "Makes Artemis' Multi-Lock and Normal Shot charge instantly.\n"
				"作为平衡，阿尔忒弥斯的多重锁定与普通射击弹道伤害降低90%。");

			ImGui::TableNextColumn();

			/*GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("地面瞬移",
				activeCrimsonGameplay.Gameplay.Dante.groundTrick,
				queuedCrimsonGameplay.Gameplay.Dante.groundTrick,
				activeCrimsonGameplayMask.Gameplay.Dante.groundTrick)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "With Trickster: Lock On + Back to Forward + Style.\n"
				"但丁瞬移至敌人附近的地面。需要骗术师等级3。");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();*/

			if (GUI_Checkbox2("交换阿尔忒弥斯输入",
				activeCrimsonGameplay.Gameplay.Dante.swapArtemisMultiLockNormalShot,
				queuedCrimsonGameplay.Gameplay.Dante.swapArtemisMultiLockNormalShot,
				activeCrimsonGameplayMask.Gameplay.Dante.swapArtemisMultiLockNormalShot)) {
				ToggleArtemisSwapNormalShotAndMultiLock(activeCrimsonGameplay.Gameplay.Dante.swapArtemisMultiLockNormalShot);
			}
			ImGui::SameLine();
			TooltipHelper("(?)", "Swaps Artemis' Multi-Lock and Normal Shot inputs/funcionality.\n"
				"多重锁定改由普通射击发动，普通射击移至枪神。");

			ImGui::TableNextColumn();

			if (GUI_Checkbox2("移除涅梵空中斩击击退",
				activeCrimsonGameplay.Gameplay.Dante.disableAirSlashKnockback,
				queuedCrimsonGameplay.Gameplay.Dante.disableAirSlashKnockback,
				activeCrimsonGameplayMask.Gameplay.Dante.disableAirSlashKnockback)) {
				CrimsonPatches::DisableAirSlashKnockback(activeCrimsonGameplay.Gameplay.Dante.disableAirSlashKnockback);
			}
			ImGui::SameLine();
			TooltipHelper("(?)", "移除涅梵空中斩击第一段的击退效果。");

			ImGui::TableNextColumn();

			GUI_Checkbox2("蓄力霰弹击飞",
				activeCrimsonGameplay.Gameplay.Dante.chargedShotgunLaunches,
				queuedCrimsonGameplay.Gameplay.Dante.chargedShotgunLaunches,
				activeCrimsonGameplayMask.Gameplay.Dante.chargedShotgunLaunches);
			ImGui::SameLine();
			TooltipHelper("(?)", "使蓄力霰弹射击将敌人击飞，而非击退。"
				"\nharkening to the old DMC1 days. Makes shotgun charge slightly faster.");
			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			GUI_Checkbox2("分身分裂同步",
				activeCrimsonGameplay.Gameplay.Dante.doppelgangerSplitSync,
				queuedCrimsonGameplay.Gameplay.Dante.doppelgangerSplitSync,
				activeCrimsonGameplayMask.Gameplay.Dante.doppelgangerSplitSync);
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "(Dante Only) Doppelganger spawns fully synchronized with your current Style setup. \n"
				"While in Doppelganger Style, press Lock-On + Style Action to summon or dismiss Doppelganger. \n"
				"Pressing Style Action without Lock-On lets you change Weapons and Styles exclusively for Doppelganger,\n"
				"causing it to desync from Dante's current Style configuration until resummoned.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			GUI_Checkbox2("快速抓取",
				activeCrimsonGameplay.Gameplay.Dante.quickGrapple,
				queuedCrimsonGameplay.Gameplay.Dante.quickGrapple,
				activeCrimsonGameplayMask.Gameplay.Dante.quickGrapple);
			ImGui::SameLine();
			TooltipHelper("(?)", "使但丁的抓取跳过起手动作，发动更快。");

			ImGui::EndTable();
		}
	}

	ImGui::Text("");
	ImGui::PopStyleColor();
	ImGui::PopFont();
}

void DanteCheatOptions() {
	auto& defaultFontSize = UI::g_UIContext.DefaultFontSize;
	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);

	// Dante Cheats Section
	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);
	ImGui::Text("但丁作弊选项");
	ImGui::PopFont();

	UI::SeparatorEx(defaultFontSize * 23.35f);

	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

	ImGui::Text("");

	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowHeight = 40.0f * queuedConfig.globalScale;

		if (ImGui::BeginTable("DanteCheatsTable", 3)) {
			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			ImGui::TableSetupColumn("b2", 0, columnWidth * 2.0f);
			ImGui::TableSetupColumn("b3", 0, columnWidth * 2.0f);

			// First row
			ImGui::TableNextRow(0, rowHeight * 0.5f);
			ImGui::TableNextColumn();

			if (GUI_Checkbox2("强制皇家释放",
				activeCrimsonGameplay.Cheats.Dante.forceRoyalRelease,
				queuedCrimsonGameplay.Cheats.Dante.forceRoyalRelease,
				activeCrimsonGameplayMask.Cheats.Dante.forceRoyalRelease)) {
				CrimsonPatches::ToggleRoyalguardForceJustFrameRelease(activeCrimsonGameplay.Cheats.Dante.forceRoyalRelease);
			}
			ImGui::SameLine();
			TooltipHelper("(?)", "使皇家护卫释放始终为皇家（精准帧）释放。");

			ImGui::TableNextColumn();

			if (GUI_Checkbox2("无限撕裂",
				activeCrimsonGameplay.Cheats.Dante.infiniteShredder,
				queuedCrimsonGameplay.Cheats.Dante.infiniteShredder,
				activeCrimsonGameplayMask.Cheats.Dante.infiniteShredder)) {
				ToggleRebellionInfiniteShredder(activeCrimsonGameplay.Cheats.Dante.infiniteShredder);
			}
			ImGui::SameLine();
			TooltipHelper("(?)", "使叛逆的撕裂持续无限时间。");

			ImGui::TableNextColumn();

			if (GUI_Checkbox2("无限剑刺",
				activeCrimsonGameplay.Cheats.Dante.infiniteSwordPierce,
				queuedCrimsonGameplay.Cheats.Dante.infiniteSwordPierce,
				activeCrimsonGameplayMask.Cheats.Dante.infiniteSwordPierce)) {
				ToggleRebellionInfiniteSwordPierce(activeCrimsonGameplay.Cheats.Dante.infiniteSwordPierce);
			}
			ImGui::SameLine();
			TooltipHelper("(?)", "使叛逆的剑刺持续无限时间。");

			ImGui::EndTable();
		}
	}

	ImGui::Text("");
	ImGui::PopStyleColor();
	ImGui::PopFont();
}

// Vergil gameplay options section
void VergilGameplayOptions() {
	auto& defaultFontSize = UI::g_UIContext.DefaultFontSize;
	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);
	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);
	ImGui::Text("维吉尔玩法选项");
	ImGui::PopFont();
	UI::SeparatorEx(defaultFontSize * 23.35f);
	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);
	ImGui::Text("");

	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowHeight = 40.0f * queuedConfig.globalScale;
		if (ImGui::BeginTable("VergilOptionsTable", 3)) {
			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			ImGui::TableSetupColumn("b2", 0, columnWidth * 2.0f);
			ImGui::TableSetupColumn("b3", 0, columnWidth * 2.0f);

			// First row
			ImGui::TableNextRow(0, rowHeight * 0.5f);
			ImGui::TableNextColumn();
			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("暗黑骑士瞬移全能取消",
				activeCrimsonGameplay.Gameplay.Vergil.darkslayerTrickCancels,
				queuedCrimsonGameplay.Gameplay.Vergil.darkslayerTrickCancels,
				activeCrimsonGameplayMask.Gameplay.Vergil.darkslayerTrickCancels)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "允许随时用任意暗黑骑士瞬移取消任何招式。");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();
			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("魔人化连斩加速",
				activeCrimsonGameplay.Gameplay.Vergil.fasterDTRapidSlash,
				queuedCrimsonGameplay.Gameplay.Vergil.fasterDTRapidSlash,
				activeCrimsonGameplayMask.Gameplay.Vergil.fasterDTRapidSlash)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "魔人化时连斩执行更快。");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();
			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("回旋剑调整",
				activeCrimsonGameplay.Gameplay.Vergil.roundTripTweaks,
				queuedCrimsonGameplay.Gameplay.Vergil.roundTripTweaks,
				activeCrimsonGameplayMask.Gameplay.Vergil.roundTripTweaks)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "持力量之刃：锁定 + 后向前 + 近战。");
			GUI_PopDisable(!activeConfig.Actor.enable);

			// Second row
			ImGui::TableNextRow(0, rowHeight * 0.5f);
			ImGui::TableNextColumn();
			/*GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("空中刺击",
				activeCrimsonGameplay.Gameplay.Vergil.airStinger,
				queuedCrimsonGameplay.Gameplay.Vergil.airStinger,
				activeCrimsonGameplayMask.Gameplay.Vergil.airStinger)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "持力量之刃：空中锁定 + 前 + 近战。");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();*/
			/*GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("空中旭日",
				activeCrimsonGameplay.Gameplay.Vergil.airRisingSun,
				queuedCrimsonGameplay.Gameplay.Vergil.airRisingSun,
				activeCrimsonGameplayMask.Gameplay.Vergil.airRisingSun)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "持贝奥武夫：空中锁定 + 后 + 近战。");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();
			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("空中月相",
				activeCrimsonGameplay.Gameplay.Vergil.airLunarPhase,
				queuedCrimsonGameplay.Gameplay.Vergil.airLunarPhase,
				activeCrimsonGameplayMask.Gameplay.Vergil.airLunarPhase)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "持贝奥武夫：空中锁定 + 前 + 近战。");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();*/
			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("次元斩重做",
				activeCrimsonGameplay.Gameplay.Vergil.judgementCutRework,
				queuedCrimsonGameplay.Gameplay.Vergil.judgementCutRework,
				activeCrimsonGameplayMask.Gameplay.Vergil.judgementCutRework)) {

				if (activeCrimsonGameplay.Gameplay.Vergil.judgementCutRework) {
					queuedCrimsonGameplay.Cheats.Vergil.judgementCutCount[0] = 2;
					queuedCrimsonGameplay.Cheats.Vergil.judgementCutCount[1] = 3;

					activeCrimsonGameplay.Cheats.Vergil.judgementCutCount[0] = 2;
					activeCrimsonGameplay.Cheats.Vergil.judgementCutCount[1] = 3;
				}
				else {
					queuedCrimsonGameplay.Cheats.Vergil.judgementCutCount[0] = 2;
					queuedCrimsonGameplay.Cheats.Vergil.judgementCutCount[1] = 2;

					activeCrimsonGameplay.Cheats.Vergil.judgementCutCount[0] = 2;
					activeCrimsonGameplay.Cheats.Vergil.judgementCutCount[1] = 2;
				}
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Makes Judgement Cut behave like it does in 4SE/5 with Just Frame inputs.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();
			/*GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("阎魔刀 升星",
				activeCrimsonGameplay.Gameplay.Vergil.yamatoRisingStar,
				queuedCrimsonGameplay.Gameplay.Vergil.yamatoRisingStar,
				activeCrimsonGameplayMask.Gameplay.Vergil.yamatoRisingStar)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "持阎魔刀：连斩中长按近战。");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();
			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("阎魔刀 上挑",
				activeCrimsonGameplay.Gameplay.Vergil.yamatoHighTime,
				queuedCrimsonGameplay.Gameplay.Vergil.yamatoHighTime,
				activeCrimsonGameplayMask.Gameplay.Vergil.yamatoHighTime)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "持阎魔刀：锁定 + 后 + 长按近战");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();*/

			//GUI_PushDisable(!activeConfig.Actor.enable);
			//if (GUI_Checkbox2("空中嘲讽旭日",
			//	activeCrimsonGameplay.Gameplay.Vergil.airTauntRisingSun,
			//	queuedCrimsonGameplay.Gameplay.Vergil.airTauntRisingSun,
			//	activeCrimsonGameplayMask.Gameplay.Vergil.airTauntRisingSun)) {
			//}
			//ImGui::SameLine();
			//GUI_CCSRequirementButton();
			//ImGui::SameLine();
			//TooltipHelper("(?)", "With any Weapon: Press Taunt button while in air.\n"
			//	"与空中旭日的站位略有不同。");
			//GUI_PopDisable(!activeConfig.Actor.enable);

			//ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("幻影魔人化",
				activeCrimsonGameplay.Gameplay.Vergil.mirageTrigger,
				queuedCrimsonGameplay.Gameplay.Vergil.mirageTrigger,
				activeCrimsonGameplayMask.Gameplay.Vergil.mirageTrigger)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Press Left or Right D-Pad to summon Doppelganger.\n"
			"Vergil's Doppelganger uses a separate resource called Mirage Gauge, indicated on his HUD as a white bar.\n"
			"Mirage Gauge refills like DT (but is separate from it), and can be turned on/off at will, without a minimum amount required.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();
			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("下坠惯性",
				activeCrimsonGameplay.Gameplay.Vergil.downertia,
				queuedCrimsonGameplay.Gameplay.Vergil.downertia,
				activeCrimsonGameplayMask.Gameplay.Vergil.downertia)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Executing another move before about 0.35 seconds of executing Lunar Phase or Air Taunt Rising Sun will send you downwards quickly.\n"
				"跳跃取消不会中断此效果。");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();
			if (GUI_Checkbox2("未锁定时上瞬移",
				activeCrimsonGameplay.Gameplay.Vergil.trickUpNoLockOn,
				queuedCrimsonGameplay.Gameplay.Vergil.trickUpNoLockOn,
				activeCrimsonGameplayMask.Gameplay.Vergil.trickUpNoLockOn)) {
			}
			ImGui::SameLine();
			TooltipHelper("(?)", "Allows Vergil to Trick Up instead of Air Tricking when using the Style Button without Lock On.\n"
				"类似《鬼泣5》。");

			ImGui::TableNextColumn();
			GUI_PushDisable(!activeConfig.Actor.enable);
			ImGui::PushItemWidth(itemWidth * 0.5f);
			GUI_Checkbox2("空中月相无上升",
				activeCrimsonGameplay.Gameplay.Vergil.noAirLunarPhaseLift,
				queuedCrimsonGameplay.Gameplay.Vergil.noAirLunarPhaseLift,
				activeCrimsonGameplayMask.Gameplay.Vergil.noAirLunarPhaseLift);
			ImGui::PopItemWidth();
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "阻止月相向上移动，敌人行为不受影响。");
			GUI_PopDisable(!activeConfig.Actor.enable);


			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * 0.5f);
			GUI_ComboVectorString2("剑阵快捷",
				SwordFormationShortcutNames,
				activeCrimsonGameplay.Gameplay.Vergil.swordFormationsShortcut,
				queuedCrimsonGameplay.Gameplay.Vergil.swordFormationsShortcut,
				activeCrimsonGameplayMask.Gameplay.Vergil.swordFormationsShortcut);
			ImGui::PopItemWidth();
			ImGui::SameLine();
			TooltipHelper("(?)", "Go directly to Blistering Swords or Storm Swords without needing Spiral Swords beforehand.\n"
				"Lock On + Forward + Shoot: Blistering Swords, Lock On + Backwards + Shoot: Storm Swords. \n"
				"此选项可反转两种输入，并大幅缩短进入任意剑阵所需的射击键长按时间。");

			GUI_Checkbox2("风暴之剑倒地修复",
			activeCrimsonGameplay.Gameplay.Vergil.stormSwordsDownedEnemyFix,
			queuedCrimsonGameplay.Gameplay.Vergil.stormSwordsDownedEnemyFix,
			activeCrimsonGameplayMask.Gameplay.Vergil.stormSwordsDownedEnemyFix);
			ImGui::SameLine();
			TooltipHelper("(?)", "修正风暴之剑的判定半径，使其能正确命中倒地敌人。");

			ImGui::EndTable();
		}
	}

	ImGui::Text("");
	ImGui::PopStyleColor();
	ImGui::PopFont();
}

void VergilCheatOptions() {
	auto& defaultFontSize = UI::g_UIContext.DefaultFontSize;
	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);
	ImGui::Text("维吉尔作弊选项");
	ImGui::PopFont();

	UI::SeparatorEx(defaultFontSize * 23.35f);

	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);
	ImGui::Text("");

	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowHeight = 40.0f * queuedConfig.globalScale;
		if (ImGui::BeginTable("VergilCheatsTable", 3)) {
			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			ImGui::TableSetupColumn("b2", 0, columnWidth * 2.0f);
			ImGui::TableSetupColumn("b3", 0, columnWidth * 2.0f);

			// First row
			ImGui::TableNextRow(0, rowHeight * 0.5f);
			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("水银风格",
				activeCrimsonGameplay.Cheats.Vergil.quicksilverStyle,
				queuedCrimsonGameplay.Cheats.Vergil.quicksilverStyle,
				activeCrimsonGameplayMask.Cheats.Vergil.quicksilverStyle)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "为维吉尔启用：水银。十字键下切换至水银，十字键上切回暗黑骑士。");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("时序之剑",
				activeCrimsonGameplay.Cheats.Vergil.chronoSwords,
				queuedCrimsonGameplay.Cheats.Vergil.chronoSwords,
				activeCrimsonGameplayMask.Cheats.Vergil.chronoSwords)) {
				ToggleChronoSwords(activeCrimsonGameplay.Cheats.Vergil.chronoSwords);
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "水银生效期间，幻影剑将静止不动。");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();
			if (GUI_Checkbox2("无限回旋剑",
				activeCrimsonGameplay.Cheats.Vergil.infiniteRoundTrip,
				queuedCrimsonGameplay.Cheats.Vergil.infiniteRoundTrip,
				activeCrimsonGameplayMask.Cheats.Vergil.infiniteRoundTrip)) {
				ToggleYamatoForceEdgeInfiniteRoundTrip(activeCrimsonGameplay.Cheats.Vergil.infiniteRoundTrip);
			}

			ImGui::EndTable();
		}
	}

	// Add special inputs for Rising Sun and Judgement Cut with HUMAN/DEVIL TRIGGER columns
	ImGui::Text("");

	auto VergilActionDataInput = [](const char* label, uint8(&active)[2], uint8(&queued)[2], uint8(&defaultVar)[2], bool disabled = false) {
		auto defaultFontSize = UI::g_UIContext.DefaultFontSize;
		const float rowWidth = 40.0f * queuedConfig.globalScale * 0.5f;
		float smallerComboMult = 0.7f;

		GUI_PushDisable(disabled);

		for (size_t i = 0; i < 2; ++i) {
			if (i == 0) {
				ImGui::TableNextRow(0, rowWidth);
				ImGui::TableNextColumn();
			}

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_InputDefault2("", active[i], queued[i], defaultVar[i], (uint8)1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

			if (i == 0) {
				ImGui::TableNextColumn();
			} else {
				ImGui::SameLine();
				ImGui::Text(label);
				ImGui::SameLine();
				GUI_CCSRequirementButton();
			}
		}

		GUI_PopDisable(disabled);
		};

		// Get initial cursor position for manual layout
		ImVec2 initialPos = ImGui::GetCursorPos();
		const float columnWidth = 0.15f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale * 0.5f;

		GUI_PushDisable(!activeConfig.Actor.enable);
		ImGui::SetCursorPos(initialPos);
		// Start table for Rising Sun and Judgement Cut
		if (ImGui::BeginTable("VergilActionTable", 2)) {
			ImGui::TableSetupColumn("b1", 0, columnWidth);
			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("人类");
			ImGui::TableNextColumn();
			ImGui::Text("魔人化");
			ImGui::PopFont();

			bool beowulfDisabled = !activeCrimsonGameplay.Gameplay.Vergil.airRisingSun;
			VergilActionDataInput("空中旭日次数",
				activeCrimsonGameplay.Cheats.Vergil.airRisingSunCount,
				queuedCrimsonGameplay.Cheats.Vergil.airRisingSunCount,
				defaultCrimsonGameplay.Cheats.Vergil.airRisingSunCount,
				beowulfDisabled);

			VergilActionDataInput("次元斩次数",
				activeCrimsonGameplay.Cheats.Vergil.judgementCutCount,
				queuedCrimsonGameplay.Cheats.Vergil.judgementCutCount,
				defaultCrimsonGameplay.Cheats.Vergil.judgementCutCount);

			ImGui::EndTable();
		}
		GUI_PopDisable(!activeConfig.Actor.enable);

		ImGui::Text("");
		ImGui::PopStyleColor();
		ImGui::PopFont();
}

void ExtraDifficultyGameplayOptions() {
	auto& defaultFontSize = UI::g_UIContext.DefaultFontSize;
	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);
	ImGui::Text("额外难度选项");
	ImGui::PopFont();
	UI::SeparatorEx(defaultFontSize * 23.35f);

	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);
	ImGui::Text("");

	const float columnWidth = 0.5f * queuedConfig.globalScale;
	const float rowHeight = 40.0f * queuedConfig.globalScale;

	if (ImGui::BeginTable("ExtraDifficultyOptionsTable", 3)) {
		ImGui::TableSetupColumn("c1", 0, columnWidth * 2.0f);

		ImGui::TableNextRow(0, rowHeight * 0.5f);
		ImGui::TableNextColumn();

		ImGui::PushItemWidth(itemWidth * 1.0f);
		UI::ComboMapValue2("传奇黑骑士",
			ldkModeNames,
			ldkModes,
			activeCrimsonGameplay.Gameplay.ExtraDifficulty.ldkMode,
			queuedCrimsonGameplay.Gameplay.ExtraDifficulty.ldkMode);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		TooltipHelper("(?)", "Spawns large waves of enemies like in DMC5's LDK Mode.\n"
			"超级黑骑士会生成更多敌人；超级黑骑士+Boss会复制部分Boss。");

		ImGui::TableNextColumn();

		ImGui::PushItemWidth(itemWidth * 0.8f);
		UI::Combo2("风格要求", styleRankNames, activeCrimsonGameplay.Gameplay.ExtraDifficulty.mustStyleMode, queuedCrimsonGameplay.Gameplay.ExtraDifficulty.mustStyleMode);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		TooltipHelper("(?)", "Restrict your damage to only apply at specific Style Ranks. \n"
			"风格要求生效时，魔人化可保护你受击时不掉风格评级。");

		ImGui::TableNextColumn();

		ImGui::PushItemWidth(itemWidth * 0.93f);
		if (UI::ComboMapValue2("敌人魔人化模式",
			enemyDTModeNames,
			enemyDTModes,
			activeCrimsonGameplay.Gameplay.ExtraDifficulty.enemyDTMode,
			queuedCrimsonGameplay.Gameplay.ExtraDifficulty.enemyDTMode)) {
			CrimsonPatches::SetEnemyDTMode(activeCrimsonGameplay.Gameplay.ExtraDifficulty.enemyDTMode);
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();
		TooltipHelper("(?)", "Default setting will apply Enemy DT as normal in Dante Must Die Difficulty.\n"
			"Instant Enemy DT will apply Enemy DT instantly when they spawn on DMD.\n"
		"选择无敌人魔人化后，即使在DMD难度敌人也不会魔人化。");

		//ImGui::TableNextColumn();
		//GUI_Checkbox2("强化阿卡姆2",
		//	activeCrimsonGameplay.Gameplay.ExtraDifficulty.betterArkham2,
		//	queuedCrimsonGameplay.Gameplay.ExtraDifficulty.betterArkham2);
		//ImGui::TableNextColumn();
		//CrimsonBetterArkham2::DebugGui();
		
		ImGui::TableNextColumn();
		ImGui::PushItemWidth(itemWidth * 0.93f);
		UI::ComboMapValue2("强制难度",
			forceDifficultyNames,
			forceDifficultyModes,
			activeCrimsonGameplay.Gameplay.ExtraDifficulty.forceDifficultyMode,
			queuedCrimsonGameplay.Gameplay.ExtraDifficulty.forceDifficultyMode);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		TooltipHelper("(?)", "在游戏过程中强制指定任意难度。适用于DMD血宫。");

		ImGui::EndTable();
	}

	ImGui::Text("");
	ImGui::PopStyleColor();
	ImGui::PopFont();
}

struct buttonRemapStruct {
	uint16_t item; // 0 dpad up
	uint16_t equip; // 1 dpad down
	uint16_t map; // 2 dpad right
	uint16_t file; // 3 dpad left
	uint16_t melee; // 4 Y
	uint16_t jump; // 5 A
	uint16_t style; // 6 B
	uint16_t shoot; // 7 X
	uint16_t dt; // 8 L1
	uint16_t gunChange; // 9 L2
	uint16_t targetChange; // A L3
	uint16_t lockOn; // B R1
	uint16_t swordChange; // C R2
	uint16_t camReset; // D R3
	uint16_t taunt; // E Select
};

const char* GetButtonNameFromValue(uint16_t value, const std::vector<std::pair<uint16_t, const char*>>& buttonPairs) {
    for (const auto& pair : buttonPairs) {
        if (pair.first == value) {
            return pair.second;
        }
    }
    return "未知";
}

void DrawButtonCombo(const char* label, uint16_t& current_value, const std::vector<std::pair<uint16_t, const char*>>& buttonPairs) {
    bool value_changed = false;
    const char* current_button_name = GetButtonNameFromValue(current_value, buttonPairs);
    if (ImGui::BeginCombo(label, current_button_name)) {
        for (const auto& pair : buttonPairs) {
            const bool is_selected = (current_value == pair.first);
            if (ImGui::Selectable(pair.second, is_selected)) {
                current_value = pair.first;
                value_changed = true;
            }
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
}

struct PlayerBindingUI {
    uint16_t source;
    uint16_t target;
    const char* name;
};

static std::vector<PlayerBindingUI> playerBindingsUI[4][2];

void InitPlayerBindingsUI() {
    const struct {
        uint16_t button;
        const char* name;
    } danteBindings[] = {
        { XINPUT_GAMEPAD_DPAD_UP,        "道具界面（上）" }, // XINPUT buttons are not what uint16_t, const char*>> buttonPairs uses, so this displays incorrectly atm
        { XINPUT_GAMEPAD_DPAD_DOWN,      "装备界面（下）" },
        { XINPUT_GAMEPAD_DPAD_RIGHT,     "地图界面（右）" },
        { XINPUT_GAMEPAD_DPAD_LEFT,      "文件界面（左）" },
        { XINPUT_GAMEPAD_Y,              "近战攻击" },
        { XINPUT_GAMEPAD_A,              "跳跃" },
        { XINPUT_GAMEPAD_B,              "风格动作" },
        { XINPUT_GAMEPAD_X,              "射击" },
        { XINPUT_GAMEPAD_LEFT_SHOULDER,  "魔人化" },
        { XINPUT_GAMEPAD_LEFT_THUMB,     "切换目标" },
        { XINPUT_GAMEPAD_RIGHT_SHOULDER, "锁定" },
        { XINPUT_GAMEPAD_RIGHT_THUMB,    "默认镜头" },
        { XINPUT_GAMEPAD_BACK,           "嘲讽" },
        // { XINPUT_GAMEPAD_LEFT_TRIGGER,   "切换枪械" },
        // { XINPUT_GAMEPAD_RIGHT_TRIGGER,  "切换刀剑" }
    };
    
    const struct {
        uint16_t button;
        const char* name;
    } vergilBindings[] = {
        { XINPUT_GAMEPAD_DPAD_UP,        "道具界面（上）" },
        { XINPUT_GAMEPAD_DPAD_DOWN,      "装备界面（下）" },
        { XINPUT_GAMEPAD_DPAD_RIGHT,     "地图界面（右）" },
        { XINPUT_GAMEPAD_DPAD_LEFT,      "文件界面（左）" },
        { XINPUT_GAMEPAD_Y,              "近战攻击" },
        { XINPUT_GAMEPAD_A,              "跳跃" },
        { XINPUT_GAMEPAD_B,              "暗黑骑士动作" },
        { XINPUT_GAMEPAD_X,              "幻影剑" },
        { XINPUT_GAMEPAD_LEFT_SHOULDER,  "魔人化" },
        { XINPUT_GAMEPAD_LEFT_THUMB,     "切换目标" },
        { XINPUT_GAMEPAD_RIGHT_SHOULDER, "锁定" },
        { XINPUT_GAMEPAD_RIGHT_THUMB,    "默认镜头" },
        { XINPUT_GAMEPAD_BACK,           "嘲讽" },
        // { XINPUT_GAMEPAD_LEFT_TRIGGER,   "切换武器1" },
        // { XINPUT_GAMEPAD_RIGHT_TRIGGER,  "切换武器2" }
    };
    
    for (int player = 0; player < 4; player++) {
        playerBindingsUI[player][0].clear();
        playerBindingsUI[player][1].clear();
        
        for (const auto& binding : danteBindings) {
            playerBindingsUI[player][0].push_back({binding.button, binding.button, binding.name});
        }
        
        for (const auto& binding : vergilBindings) {
            playerBindingsUI[player][1].push_back({binding.button, binding.button, binding.name});
        }
    }
}

void DrawKeybindEditor(const std::vector<std::pair<uint16_t, const char*>>& buttonPairs) {
    static bool initialized = false;
    if (!initialized) {
        InitPlayerBindingsUI(); // load game binds here
        initialized = true;
    }
    
    static int currentPlayer = 0;
    static int currentCharacter = 0;

    if (ImGui::BeginTabBar("PlayerTabs")) {
        for (int i = 0; i < 4; i++) {
            char tabName[32];
            sprintf(tabName, "Player %d", i + 1);
            if (ImGui::BeginTabItem(tabName)) {
                currentPlayer = i;
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }

    if (ImGui::BeginTabBar("CharacterTabs")) {
        if (ImGui::BeginTabItem("但丁")) {
            currentCharacter = 0;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("维吉尔")) {
            currentCharacter = 1;
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    if (ImGui::BeginTable("playerBindingsTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("动作");
        ImGui::TableSetupColumn("手柄按键");
        ImGui::TableSetupColumn("映射至");
        ImGui::TableHeadersRow();
            
        for (size_t i = 0; i < playerBindingsUI[currentPlayer][currentCharacter].size(); i++) {
            auto& binding = playerBindingsUI[currentPlayer][currentCharacter][i];
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", binding.name);
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", GetButtonNameFromValue(binding.source, buttonPairs));
            ImGui::TableSetColumnIndex(2);
            char comboLabel[64];
            sprintf(comboLabel, "##playerCombo%zu", i);
            DrawButtonCombo(comboLabel, binding.target, buttonPairs);
        }
        ImGui::EndTable();
    }
    
    char resetButtonLabel[64];
    sprintf(resetButtonLabel, "Reset Player %i %s Bindings to Default", currentPlayer + 1, currentCharacter == 0 ? "但丁" : "维吉尔");
    if (ImGui::Button(resetButtonLabel)) {
        const struct {
            uint16_t button;
            const char* name;
        } danteBindings[] = {
            { XINPUT_GAMEPAD_DPAD_UP,        "道具界面（上）" },
            { XINPUT_GAMEPAD_DPAD_DOWN,      "装备界面（下）" },
            { XINPUT_GAMEPAD_DPAD_RIGHT,     "地图界面（右）" },
            { XINPUT_GAMEPAD_DPAD_LEFT,      "文件界面（左）" },
            { XINPUT_GAMEPAD_Y,              "近战攻击" },
            { XINPUT_GAMEPAD_A,              "跳跃" },
            { XINPUT_GAMEPAD_B,              "风格动作" },
            { XINPUT_GAMEPAD_X,              "射击" },
            { XINPUT_GAMEPAD_LEFT_SHOULDER,  "魔人化" },
            { XINPUT_GAMEPAD_LEFT_THUMB,     "切换目标" },
            { XINPUT_GAMEPAD_RIGHT_SHOULDER, "锁定" },
            { XINPUT_GAMEPAD_RIGHT_THUMB,    "默认镜头" },
            { XINPUT_GAMEPAD_BACK,           "嘲讽" },
            // { XINPUT_GAMEPAD_LEFT_TRIGGER,   "切换枪械" },
            // { XINPUT_GAMEPAD_RIGHT_TRIGGER,  "切换刀剑" }
        };
        
        const struct {
            uint16_t button;
            const char* name;
        } vergilBindings[] = {
            { XINPUT_GAMEPAD_DPAD_UP,        "道具界面（上）" },
            { XINPUT_GAMEPAD_DPAD_DOWN,      "装备界面（下）" },
            { XINPUT_GAMEPAD_DPAD_RIGHT,     "地图界面（右）" },
            { XINPUT_GAMEPAD_DPAD_LEFT,      "文件界面（左）" },
            { XINPUT_GAMEPAD_Y,              "阎魔刀攻击" },
            { XINPUT_GAMEPAD_A,              "跳跃" },
            { XINPUT_GAMEPAD_B,              "暗黑骑士动作" },
            { XINPUT_GAMEPAD_X,              "召唤幻影剑" },
            { XINPUT_GAMEPAD_LEFT_SHOULDER,  "魔人化" },
            { XINPUT_GAMEPAD_LEFT_THUMB,     "切换目标" },
            { XINPUT_GAMEPAD_RIGHT_SHOULDER, "锁定" },
            { XINPUT_GAMEPAD_RIGHT_THUMB,    "默认镜头" },
            { XINPUT_GAMEPAD_BACK,           "嘲讽" },
            // { XINPUT_GAMEPAD_LEFT_TRIGGER,   "切换武器1" },
            // { XINPUT_GAMEPAD_RIGHT_TRIGGER,  "WEAPON CHANGE 2 }
        };
        
        playerBindingsUI[currentPlayer][currentCharacter].clear();
        
		if (currentCharacter == 0) {
			for (const auto& binding : danteBindings) {
				playerBindingsUI[currentPlayer][currentCharacter].push_back({binding.button, binding.button, binding.name});
			}
		} else {
			for (const auto& binding : vergilBindings) {
				playerBindingsUI[currentPlayer][currentCharacter].push_back({binding.button, binding.button, binding.name});
			}
		}
    }
	//Commented out because siyan said to do so -Hitch
    //ImGui::End();
}

void GameplaySection() {
 	GeneralGameplayOptions();
	DanteGameplayOptions();
	VergilGameplayOptions();
	ExtraDifficultyGameplayOptions();
}

#pragma endregion

#pragma region Indicators



void RenderMainMenuInfo(IDXGISwapChain* pSwapChain) {
	static bool wasInMenu = false;
	static bool resized = false;
	ID3D11Device* pDevice = nullptr;
	pSwapChain->GetDevice(IID_PPV_ARGS(&pDevice));
	static const Texture2DD3D11 logo(g_Image_CrimsonMainLogo.GetRGBAData(), g_Image_CrimsonMainLogo.GetWidth(), g_Image_CrimsonMainLogo.GetHeight(), pDevice);
	if (!resized) {
		g_Image_CrimsonMainLogo.Resize(1924.0f, 509.0f);
		resized = true;
	}
	auto textColor = ImColor(LerpFadeToBlack(UI::ColorToImVec4(IM_COL32(255, 255, 255, 255))));
	if (g_inMainMenu != 1) {
		// Reset fade when not in menu
		wasInMenu = false;
		return;
	}

	static float fadeTimer = 0.0f;
	static const float fadeDuration = 0.7f; // seconds

	// Reset fade when menu is entered
	if (!wasInMenu) {
		fadeTimer = 0.0f;
		wasInMenu = true;

	}
	
	// Advance fade timer
	float deltaTime = ImGui::GetIO().DeltaTime;
	if (fadeTimer < fadeDuration)
		fadeTimer += deltaTime;
	float fadeProgress = (std::min)(fadeTimer / fadeDuration, 1.0f);

	// Logo Window
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;


	auto logoWidth = (logo.GetWidth() * 0.68f) * scaleFactorY;
	auto logoHeight = (logo.GetHeight() * 0.68f) * scaleFactorY;
	ImVec2 logoWindowSize = ImVec2(logoWidth + 50.0f, logoHeight + 50.0f);
	ImVec2 logoWindowPos = ImVec2((g_renderSize.x - (logoWidth)) * 0.5f, 310.0 * scaleFactorY);
	auto logoSize = ImVec2(logoWindowPos.x + logoWidth, logoWindowPos.y + logoHeight);

	ImGui::SetNextWindowPos(logoWindowPos);
	ImGui::SetNextWindowSize(logoWindowSize);

	ImGui::Begin("LogoWindow", nullptr, windowFlags);

	// Fade from white to normal color
	ImVec4 tintColor = ImLerp(ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1), fadeProgress); // default is white
	tintColor = ImVec4(1, 1, 1, fadeProgress);
	tintColor = LerpFadeToBlack(tintColor);
	ImGui::GetWindowDrawList()->AddImage(
		logo,
		logoWindowPos,
		logoSize,
		ImVec2(0, 0), ImVec2(1, 1),
		ImGui::ColorConvertFloat4ToU32(tintColor)
	);

	ImGui::End();

	//// C•Team 'A Berthrage Project' window
	//float cTeamBerthrageFontSize = 30.0f;
	//int cTeamBerthrageAlpha = static_cast<int>(fadeProgress * 255.0f);
	//ImFont* berthrageFont = UI::g_ImGuiFont_RussoOne[cTeamBerthrageFontSize];
	//float scaledFontSize = berthrageFont->FontSize * scaleFactorY;
	//auto cTeamBerthrageText = u8"C•Team 出品 • Berthrage 项目";

	//// Calculate text size with scaling
	//ImVec2 cTeamBerthrageTextSize = berthrageFont->CalcTextSizeA(
	//	scaledFontSize, FLT_MAX, 0.0f, (const char*)cTeamBerthrageText
	//);

	//// Calculate centered position in screen space
	//ImVec2 berthrageTitleTextScreenPos = ImVec2(
	//	(g_renderSize.x - cTeamBerthrageTextSize.x) * 0.5f,
	//	logoWindowPos.y - (40.0f * scaleFactorY) // Y position as before
	//);

	//// Setup window (make sure it's big enough, but position doesn't matter for AddText)
	//ImVec2 cTeamBerthrageTextWindowSize = ImVec2(
	//	cTeamBerthrageTextSize.x + 20.0f * scaleFactorY,
	//	cTeamBerthrageTextSize.y + 20.0f * scaleFactorY
	//);
	//ImVec2 cTeamBerthrageTextWindowPos = ImVec2(
	//	berthrageTitleTextScreenPos.x - 10.0f * scaleFactorY,
	//	berthrageTitleTextScreenPos.y - 10.0f * scaleFactorY
	//);

	//ImGui::SetNextWindowPos(cTeamBerthrageTextWindowPos);
	//ImGui::SetNextWindowSize(cTeamBerthrageTextWindowSize);
	//ImGui::Begin("CTeamBerthrageProjectWindow", nullptr, windowFlags);

	//// Draw shadow
	//ImVec2 berthrageTitleTextShadowOffset = ImVec2(2.0f * scaleFactorY, 2.0f * scaleFactorY);
	//ImGui::GetWindowDrawList()->AddText(
	//	berthrageFont,
	//	scaledFontSize,
	//	berthrageTitleTextScreenPos + berthrageTitleTextShadowOffset,
	//	IM_COL32(0, 0, 0, cTeamBerthrageAlpha),
	//	(const char*)cTeamBerthrageText
	//);

	//// Draw normal text
	//ImGui::GetWindowDrawList()->AddText(
	//	berthrageFont,
	//	scaledFontSize,
	//	berthrageTitleTextScreenPos,
	//	IM_COL32(255, 255, 255, cTeamBerthrageAlpha),
	//	(const char*)cTeamBerthrageText
	//);

	//
	//ImGui::End();

	auto onNewGame = *reinterpret_cast<float*>(appBaseAddr + 0xC0D1E0);
	if (onNewGame != 0.0f) {
		// Game Mode Text Window
		float gameModeFontSize = 55.0f;
		//ImGui::PushStyleColor(ImGuiCol_Text, *reinterpret_cast<ImVec4*>(&gameModeData.colors[activeCrimsonGameplay.GameMode.preset]));
		ImGui::PushFont(UI::g_ImGuiFont_Benguiat256);
		std::string gameModeText = gameModeData.names[activeCrimsonGameplay.GameMode.preset];
		ImVec2 gameModeTextSize = ImGui::CalcTextSize(gameModeText.c_str());
		ImVec2 gameModeTextSizeWindowSize = ImVec2((gameModeTextSize.x + 20.0f) * scaleFactorY, 100.0f * scaleFactorY);
		ImVec2 gameModeTextSizeWindowPos = ImVec2(-5.0f * scaleFactorY, 869.0f * scaleFactorY);

		ImVec2 gameModeGradientSize = ImVec2((700.0f) * scaleFactorY, 69.0f * scaleFactorY);
		ImVec2 gameModeGradientPos = ImVec2((g_renderSize.x) * 0.0f, 869.0f * scaleFactorY);
		if (gameModeTextSizeWindowSize.x < gameModeGradientSize.x) {
			gameModeTextSizeWindowSize.x = gameModeGradientSize.x + 5.0f;
		}
		ImGui::SetNextWindowPos(gameModeTextSizeWindowPos);
		ImGui::SetNextWindowSize(gameModeTextSizeWindowSize);

		ImGui::Begin("GameModeTextWindow", nullptr, windowFlags);
		ImGui::SetWindowFontScale(scaleFactorY);
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		static ImU32 styleswitchcolor = 0xE8BA18FF;
		static ImU32 vanillacolor = 0xBDC0C1FF;
		// Draw shadow
		ImU32 gradientColor = gameModeData.colors[activeCrimsonGameplay.GameMode.preset];
		if (activeCrimsonGameplay.GameMode.preset == GAMEMODEPRESETS::VANILLA) {
			gradientColor = vanillacolor;
		}
		if (activeCrimsonGameplay.GameMode.preset == GAMEMODEPRESETS::STYLE_SWITCHER) {
			gradientColor = styleswitchcolor;
		}
		ImVec2 gamemodeTextShadowOffset = ImVec2(6.0f * scaleFactorY, 6.0f * scaleFactorY);
		ImVec2 gamemodePadding = ImVec2(15.0f * scaleFactorY, 8.0f * scaleFactorY);
		auto gradientaddr = CrimsonHUD::getCrimsonGradient();
		if (gradientaddr != nullptr) {
			ImGui::GetWindowDrawList()->AddImage(
				gradientaddr->GetTexture(),
				gameModeGradientPos,
				ImVec2(gameModeGradientPos.x + gameModeGradientSize.x, gameModeGradientPos.y + gameModeGradientSize.y),
				ImVec2(0, 0), ImVec2(1, 1),
				ImColor(LerpFadeToBlack(UI::ColorToImVec4(gradientColor)))

			);
		}
		ImGui::GetWindowDrawList()->AddText(
			UI::g_ImGuiFont_Benguiat256,
			gameModeFontSize * scaleFactorY,
			gameModeTextSizeWindowPos + gamemodeTextShadowOffset + gamemodePadding,
			IM_COL32(0, 0, 0, 155),
			gameModeText.c_str()
		);

		ImGui::GetWindowDrawList()->AddText(
			UI::g_ImGuiFont_Benguiat256,
			gameModeFontSize * scaleFactorY,
			gameModeTextSizeWindowPos + gamemodePadding,
			ImColor(LerpFadeToBlack(ImVec4(1, 1, 1, 1))),
			gameModeText.c_str()
		);

		//ImGui::PopStyleColor();
		ImGui::PopFont();

		ImGui::End();
	}
	

	const float legalFontSize = 20.0f;

	// Version Text Window
	float versionFontSize = legalFontSize;
	ImFont* versionFont = UI::g_ImGuiFont_Benguiat[versionFontSize];
	float versionScaledFontSize = versionFont->FontSize * scaleFactorY;
	ImGui::PushFont(versionFont);

	// Format the version string
	std::string versionStr = std::format("v{}.{}{}", UI::g_UIContext.CurrentVersion.Major, UI::g_UIContext.CurrentVersion.Minor, UI::g_UIContext.CurrentVersion.PatchLetter);
	std::string versionText = "" + versionStr;

	// Calculate text size
	ImVec2 versionTextSize = ImGui::CalcTextSize(versionText.c_str());
	ImVec2 versionTextSizeWindowSize = ImVec2((versionTextSize.x + 20.0f) * scaleFactorY, 100.0f * scaleFactorY);

	// Clamp the render width to a maximum of 1920.0f
	float clampedRenderWidth = (std::min)(g_renderSize.x, 1920.0f);

	// Calculate the center position for a 16:9 aspect ratio
	float aspectRatio = 16.0f / 9.0f;
	float targetHeight = clampedRenderWidth / aspectRatio; // Height based on 16:9 aspect ratio

	// Calculate the center position
	float centerX = (clampedRenderWidth / 2.0f) * scaleFactorY;
	float centerY = targetHeight / 2.0f;

	// Offset the position to align the text correctly
	ImVec2 versionTextSizeWindowPos = ImVec2(
		centerX + (800.0f * scaleFactorY), // Center horizontally
		979.0f * scaleFactorY // Center vertically
	);

	// Ensure the position stays within bounds
	versionTextSizeWindowPos.x = (std::max)(0.0f, versionTextSizeWindowPos.x);
	versionTextSizeWindowPos.y = (std::max)(0.0f, versionTextSizeWindowPos.y);

	// Set the window position and size
	ImGui::SetNextWindowPos(versionTextSizeWindowPos);
	ImGui::SetNextWindowSize(versionTextSizeWindowSize);

	ImGui::Begin("VersionTextWindow", nullptr, windowFlags);

	ImGui::SetWindowFontScale(scaleFactorY);
	ImVec2 versionTextScreenPos = ImGui::GetCursorScreenPos();
	ImVec2 versionTextShadowOffset = ImVec2(2.0f * scaleFactorY, 2.0f * scaleFactorY);
	ImDrawList* versionDrawList = ImGui::GetWindowDrawList();
	versionDrawList->AddText(
		versionFont,
		versionScaledFontSize,
		versionTextScreenPos + versionTextShadowOffset,
		IM_COL32(0, 0, 0, 255),
		versionText.c_str()
	);
	versionDrawList->AddText(
		versionFont,
		versionScaledFontSize,
		versionTextScreenPos,
		ImColor(LerpFadeToBlack(ImVec4(0.46f, 0.46f, 0.46f, 1.0f))),
		versionText.c_str()
	);


	ImGui::End();
	ImGui::PopFont();

	// GUI Hotket Text Window
	static bool guiHotkeyRun = false;
	float guiKeyFontSize = 24.0f;
	if (!guiHotkeyRun) keyBindings[0].UpdateBuffer(keyBindings[0].mainInfo, keyBindings[0].activeKeyData);
	
	float guiHotkeyFontSize = 24.0f;
	int guiHotkeyAlpha = 255; // Fixed alpha

	ImFont* guiHotkeyFont = UI::g_ImGuiFont_Benguiat[guiHotkeyFontSize];
	float guiHotkeyScaledFontSize = guiHotkeyFont->FontSize * scaleFactorY;

	auto guiHotkey = keyBindings[0].mainInfo.buffer;
	std::string guiHotkeyTextKeyboard = "" + (std::string)guiHotkey;
	std::string guiHotkeyTextComplete = guiHotkeyTextKeyboard + "/LS + RS:\nTOGGLE OVERLAY";
	std::string guiHotkeyTextKeyboardOnly = "" + (std::string)keyBindings[0].mainInfo.buffer + ":\nTOGGLE OVERLAY";
	ImVec2 guiTextSize = ImGui::CalcTextSize(guiHotkeyTextComplete.c_str());
	ImVec2 guiTextSize2 = ImGui::CalcTextSize(guiHotkeyTextKeyboardOnly.c_str());

	// Calculate text size with scaling
	ImVec2 guiHotkeyTextCompleteSize = guiHotkeyFont->CalcTextSizeA(
		guiHotkeyScaledFontSize, FLT_MAX, 0.0f, guiHotkeyTextComplete.c_str()
	);
	ImVec2 guiHotkeyTextKeyboardOnlySize = guiHotkeyFont->CalcTextSizeA(
		guiHotkeyScaledFontSize, FLT_MAX, 0.0f, guiHotkeyTextKeyboardOnly.c_str()
	);

	// Choose which text to use
	bool useKeyboardOnly = activeCrimsonConfig.GUI.disableGamepadShortcut;
	const char* guiHotkeyTextFinal = useKeyboardOnly ? guiHotkeyTextKeyboardOnly.c_str() : guiHotkeyTextComplete.c_str();
	ImVec2 guiHotkeyTextFinalSize = useKeyboardOnly ? guiHotkeyTextKeyboardOnlySize : guiHotkeyTextCompleteSize;

	// Calculate centered position in screen space
	ImVec2 guiHotkeyTextScreenPos = ImVec2(
		(g_renderSize.x - guiHotkeyTextFinalSize.x) * 0.97f,
		879.0f * scaleFactorY
	);

	// Setup window (make sure it's big enough, but position doesn't matter for AddText)
	ImVec2 guiHotkeyWindowSize = ImVec2(
		guiHotkeyTextFinalSize.x + 20.0f * scaleFactorY,
		guiHotkeyTextFinalSize.y + 20.0f * scaleFactorY
	);
	ImVec2 guiHotkeyWindowPos = ImVec2(
		guiHotkeyTextScreenPos.x - 10.0f * scaleFactorY,
		guiHotkeyTextScreenPos.y - 10.0f * scaleFactorY
	);

	ImGui::SetNextWindowPos(guiHotkeyWindowPos);
	ImGui::SetNextWindowSize(guiHotkeyWindowSize);
	ImGui::Begin("GuiKeyTextWindow", nullptr, windowFlags);

	// Draw shadow
	ImVec2 guiHotkeyTextShadowOffset = ImVec2(2.0f * scaleFactorY, 2.0f * scaleFactorY);
	ImGui::GetWindowDrawList()->AddText(
		guiHotkeyFont,
		guiHotkeyScaledFontSize,
		guiHotkeyTextScreenPos + guiHotkeyTextShadowOffset,
		IM_COL32(0, 0, 0, guiHotkeyAlpha),
		guiHotkeyTextFinal
	);

	// Draw normal text
	ImGui::GetWindowDrawList()->AddText(
		guiHotkeyFont,
		guiHotkeyScaledFontSize,
		guiHotkeyTextScreenPos,
		ImColor(LerpFadeToBlack(ImVec4(0.46, 0.46, 0.46, 1))),
		guiHotkeyTextFinal
	);

	ImGui::End();

	// Credits Text Window
	float creditsFontSize = 20.0f;
	float creditsCapCoFontSize = legalFontSize;
	ImGui::PushFont(UI::g_ImGuiFont_Benguiat[creditsFontSize]);
	auto OGcreditsText = u8"©CAPCOM CO., LTD. 2005, 2018 ALL RIGHTS RESERVED./ILLUSTRATIONS: Kazuma Kaneko/ATLUS";
	auto creditsText = u8"C•TEAM • 由 BERTHRAGE 执导 • PROJECT CRIMSON © • 2025";

	auto creditsCapCo = "《鬼泣》为 CAPCOM CO., LTD. 的财产。所有素材归其各自所有者所有。";

	// Calculate text sizes with correct fonts
	ImVec2 OGcreditsTextSize = ImGui::CalcTextSize((const char*)OGcreditsText);
	ImVec2 creditsTextSize = ImGui::CalcTextSize((const char*)creditsText);
	ImGui::PopFont();

	ImGui::PushFont(UI::g_ImGuiFont_Benguiat[creditsCapCoFontSize]);
	ImVec2 creditsCapCoSize = ImGui::CalcTextSize(creditsCapCo);
	ImGui::PopFont();

	ImVec2 creditsWindowSize = ImVec2(
		g_renderSize.x,
		180.0f * scaleFactorY
	);
	ImVec2 creditsWindowPos = ImVec2(
		(g_renderSize.x - (creditsWindowSize.x)) * 0.5f,
		940.0f * scaleFactorY
	);

	ImGui::SetNextWindowPos(creditsWindowPos);
	ImGui::SetNextWindowSize(creditsWindowSize);

	// Main credits text (centered)

	ImGui::Begin("CreditsTextWindow", nullptr, windowFlags);

	// Main credits text (centered)
	ImFont* creditsFont = UI::g_ImGuiFont_Benguiat[creditsFontSize];
	float creditsScaledFontSize = creditsFont->FontSize * scaleFactorY;
	ImVec2 OGcreditsTextSizeScaled = creditsFont->CalcTextSizeA(
		creditsScaledFontSize, FLT_MAX, 0.0f, (const char*)OGcreditsText
	);

	ImVec2 creditsTextSizeScaled = creditsFont->CalcTextSizeA(
		creditsScaledFontSize, FLT_MAX, 0.0f, (const char*)creditsText
	);

	ImVec2 OGcreditsTextScreenPos = ImVec2(
		(g_renderSize.x - OGcreditsTextSizeScaled.x) * 0.5f,
		creditsWindowPos.y + ImGui::GetCursorPosY()
	);

	ImVec2 creditsTextScreenPos = ImVec2(
		(g_renderSize.x - creditsTextSizeScaled.x) * 0.5f,
		creditsWindowPos.y + ImGui::GetCursorPosY()+ OGcreditsTextSizeScaled.y
	);

	// Draw shadow
	ImVec2 creditsTextShadowOffset = ImVec2(2.0f * scaleFactorY, 2.0f * scaleFactorY);

	ImGui::GetWindowDrawList()->AddText(
		creditsFont,
		creditsScaledFontSize,
		OGcreditsTextScreenPos + creditsTextShadowOffset,
		IM_COL32(0, 0, 0, 255),
		(const char*)OGcreditsText
	);
	//ImGui::GetColorU32()
	// 
	// Draw main text

	ImGui::GetWindowDrawList()->AddText(
		creditsFont,
		creditsScaledFontSize,
		OGcreditsTextScreenPos,
		ImColor(LerpFadeToBlack(ImGui::GetStyleColorVec4(ImGuiCol_Text))),
		(const char*)OGcreditsText
	);

	ImGui::GetWindowDrawList()->AddText(
		creditsFont,
		creditsScaledFontSize,
		creditsTextScreenPos + creditsTextShadowOffset,
		IM_COL32(0, 0, 0, 255),
		(const char*)creditsText
	);
	// Draw main text
	ImGui::GetWindowDrawList()->AddText(
		creditsFont,
		creditsScaledFontSize,
		creditsTextScreenPos,
		ImColor(LerpFadeToBlack(ImGui::GetStyleColorVec4(ImGuiCol_Text))),
		(const char*)creditsText
	);


	// CapCo copyright (smaller, centered)
	ImFont* capcoFont = UI::g_ImGuiFont_Benguiat[creditsCapCoFontSize];
	float capcoScaledFontSize = capcoFont->FontSize * scaleFactorY;
	ImVec2 capcoTextSizeScaled = capcoFont->CalcTextSizeA(
		capcoScaledFontSize, FLT_MAX, 0.0f, creditsCapCo
	);
	ImVec2 capcoTextScreenPos = ImVec2(
		(g_renderSize.x - capcoTextSizeScaled.x) * 0.5f,
		creditsWindowPos.y + ImGui::GetCursorPosY() + OGcreditsTextSizeScaled.y + creditsTextSizeScaled.y
	);

	// Draw CapCo main text
	ImGui::GetWindowDrawList()->AddText(
		capcoFont,
		capcoScaledFontSize,
		capcoTextScreenPos,
		ImColor(LerpFadeToBlack(ImGui::GetStyleColorVec4(ImGuiCol_Text))),
		creditsCapCo
	);

	ImGui::End();
}

#pragma endregion

#pragma region Hotkeys

// @Move

void ReloadRoom() {
    if (!InGame()) {
        return;
    }

    auto pool_12898 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
    if (!pool_12898 || !pool_12898[8]) {
        return;
    }
    auto& eventData = *reinterpret_cast<EventData*>(pool_12898[8]);

    auto pool_12959 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
    if (!pool_12959 || !pool_12959[12]) {
        return;
    }
    auto& nextEventData = *reinterpret_cast<NextEventData*>(pool_12959[12]);


    nextEventData.room     = static_cast<uint16>(eventData.room);
    nextEventData.position = static_cast<uint16>(eventData.position);


    eventData.event = EVENT::TELEPORT;
}

void FreezeEnemies() {
	if (!InGame()) {
		return;
	}
	if (activeCrimsonGameplay.Cheats.Speed.enemy != 0.0f) {
		activeCrimsonGameplay.Cheats.General.customSpeed = true;
		activeCrimsonGameplay.Cheats.Speed.enemy = 0.0f;
		cheatsPopUp.cheatText = "已开启冻结敌人";
	}
	else {
		activeCrimsonGameplay.Cheats.General.customSpeed = queuedCrimsonGameplay.Cheats.General.customSpeed;
		activeCrimsonGameplay.Cheats.Speed.enemy = queuedCrimsonGameplay.Cheats.Speed.enemy;
		cheatsPopUp.cheatText = "已关闭冻结敌人";
	}
	if (activeCrimsonConfig.GUI.sounds) FMOD_PlaySound(0, 25);
	cheatsPopUp.showPopUp = true;
}


void MoveToMainActor() {
    if (!activeConfig.Actor.enable || !InGame()) {
        return;
    }

    LogFunction();


    byte8* mainActorBaseAddr = 0;

    {
        auto& playerData = GetPlayerData(0);

        auto& characterData = GetCharacterData(0, playerData.characterIndex, ENTITY::MAIN);
        auto& newActorData  = GetNewActorData(0, playerData.characterIndex, ENTITY::MAIN);

        auto& activeCharacterData = GetCharacterData(0, playerData.activeCharacterIndex, ENTITY::MAIN);
        auto& activeNewActorData  = GetNewActorData(0, playerData.activeCharacterIndex, ENTITY::MAIN);

        auto& leadCharacterData = GetCharacterData(0, 0, ENTITY::MAIN);
        auto& leadNewActorData  = GetNewActorData(0, 0, ENTITY::MAIN);

        auto& mainCharacterData = GetCharacterData(0, playerData.characterIndex, ENTITY::MAIN);
        auto& mainNewActorData  = GetNewActorData(0, playerData.characterIndex, ENTITY::MAIN);


        mainActorBaseAddr = activeNewActorData.baseAddr;
    }

    if (!mainActorBaseAddr) {
        return;
    }
    auto& mainActorData = *reinterpret_cast<PlayerActorData*>(mainActorBaseAddr);


    old_for_each(uint8, playerIndex, 1, activeConfig.Actor.playerCount) {
        auto& playerData = GetActivePlayerData(playerIndex);

        old_for_all(uint8, characterIndex, playerData.characterCount) {
            old_for_all(uint8, entityIndex, ENTITY_COUNT) {
                auto& playerData = GetPlayerData(playerIndex);

                auto& characterData = GetCharacterData(playerIndex, characterIndex, entityIndex);
                auto& newActorData  = GetNewActorData(playerIndex, characterIndex, entityIndex);

                auto& activeCharacterData = GetCharacterData(playerIndex, playerData.activeCharacterIndex, ENTITY::MAIN);
                auto& activeNewActorData  = GetNewActorData(playerIndex, playerData.activeCharacterIndex, ENTITY::MAIN);

                auto& leadCharacterData = GetCharacterData(playerIndex, 0, ENTITY::MAIN);
                auto& leadNewActorData  = GetNewActorData(playerIndex, 0, ENTITY::MAIN);

                auto& mainCharacterData = GetCharacterData(playerIndex, characterIndex, ENTITY::MAIN);
                auto& mainNewActorData  = GetNewActorData(playerIndex, characterIndex, ENTITY::MAIN);


                if (!newActorData.baseAddr) {
                    continue;
                }
                auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);

                actorData.position = mainActorData.position;
            }
        }
    }
}


void ToggleInfiniteHealth() {

    if (activeCrimsonGameplay.Cheats.Damage.enemyRecieveDmgDisable || activeCrimsonGameplay.Cheats.Damage.playerReceiveDmgDisable) {
		//activeCrimsonGameplay.Cheats.Training.infiniteHP = false;
		activeCrimsonGameplay.Cheats.Damage.enemyRecieveDmgDisable = false;
		activeCrimsonGameplay.Cheats.Damage.playerReceiveDmgDisable = false;
		cheatsPopUp.cheatText = "已关闭无限生命";
    } else {
		activeCrimsonGameplay.Cheats.Damage.playerReceiveDmgDisable = true;
		activeCrimsonGameplay.Cheats.Damage.enemyRecieveDmgDisable = true;
		cheatsPopUp.cheatText = "已开启无限生命";
    }
	if (activeCrimsonConfig.GUI.sounds) FMOD_PlaySound(0, 25); 

    //ToggleInfiniteHitPoints(activeCrimsonGameplay.Cheats.Training.infiniteHP);
	cheatsPopUp.showPopUp = true;
}

void ToggleOneHitKill() {
	static bool toggled = activeCrimsonGameplay.Cheats.Damage.enemyReceivedDmgMult == 9999.0f ? true : false;

	if (!toggled) {
		toggled = true;
		activeCrimsonGameplay.Cheats.General.customDamage = true;
		activeCrimsonGameplay.Cheats.Damage.enemyReceivedDmgMult = 9999.0f;
		activeCrimsonGameplay.Cheats.Damage.enemyRecieveDmgDisable = false;
		cheatsPopUp.cheatText = "已开启一击必杀";
	} else {
		toggled = false;
		activeCrimsonGameplay.Cheats.General.customDamage = queuedCrimsonGameplay.Gameplay.General.multiplayerDamageScaling;
		activeCrimsonGameplay.Cheats.Damage.enemyReceivedDmgMult = defaultCrimsonGameplay.Cheats.Damage.enemyReceivedDmgMult;
		//if it's on in the trainer GUI, this should toggle it back on.
		if (queuedCrimsonGameplay.Cheats.Damage.enemyRecieveDmgDisable) {
			activeCrimsonGameplay.Cheats.Damage.enemyRecieveDmgDisable = true;
		}
		//this should trigger only when the hotkey for inf hp is set.
		if (!queuedCrimsonGameplay.Cheats.Damage.playerReceiveDmgDisable && activeCrimsonGameplay.Cheats.Damage.playerReceiveDmgDisable)
			activeCrimsonGameplay.Cheats.Damage.enemyRecieveDmgDisable = true;
		
		cheatsPopUp.cheatText = "已关闭一击必杀";
	}
	if (activeCrimsonConfig.GUI.sounds) FMOD_PlaySound(0, 25); 
	cheatsPopUp.showPopUp = true;
}

void GamepadToggleShowMain() {

	if (activeCrimsonConfig.GUI.disableGamepadShortcut) {
		return;
	}

    static bool gamepadCombinationMainRelease[PLAYER_COUNT] = { false };

	//prevents double toggle
	bool updatinggui = false;
	// Loop through each controller
	for (int i = 0; i < 4; ++i) {
		if (CrimsonSDL::controllers[i] != NULL) {
			// Combination of buttons to check
			bool combination = (
				CrimsonSDL::IsControllerButtonDown(i, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_LEFTSTICK) &&
				CrimsonSDL::IsControllerButtonDown(i, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSTICK) &&
				!CrimsonSDL::IsControllerButtonDown(i, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_TOUCHPAD)
				);

			// Combination pressed and was not pressed before, toggle GUI and set window focus
			if (combination && !g_showMain && gamepadCombinationMainRelease[i]) {
				if(!updatinggui){
					updatinggui = true;
					ToggleCrimsonGUI();
					ImGui::SetWindowFocus(DMC3C_TITLE);
				}
				gamepadCombinationMainRelease[i] = false;
			}

			// Combination released, update release state
			if (!combination && !gamepadCombinationMainRelease[i]) {
				gamepadCombinationMainRelease[i] = true;
			}

			// Combination pressed, GUI shown, and was not pressed before, toggle GUI
			if (combination && g_showMain && gamepadCombinationMainRelease[i]) {
				if (!updatinggui) {
					updatinggui = true;
					ToggleCrimsonGUI();
				}
				gamepadCombinationMainRelease[i] = false;
			}
		}
	}


}

std::vector<KeyBinding> keyBindings = {
    {
        "切换绯红叠加层",
        activeConfig.keyData[0],
        queuedConfig.keyData[0],
        defaultConfig.keyData[0],
        ToggleCrimsonGUI,
    },
	{"切换无限生命值", activeConfig.keyData[1], queuedConfig.keyData[1], defaultConfig.keyData[1], ToggleInfiniteHealth},
	{"切换一击必杀", activeConfig.keyData[2], queuedConfig.keyData[2], defaultConfig.keyData[2], ToggleOneHitKill},
    {"重载房间", activeConfig.keyData[3], queuedConfig.keyData[3], defaultConfig.keyData[3], ReloadRoom},
    {"移至主角", activeConfig.keyData[4], queuedConfig.keyData[4], defaultConfig.keyData[4], MoveToMainActor},
	{"冻结敌人", activeConfig.keyData[5], queuedConfig.keyData[5], defaultConfig.keyData[5], FreezeEnemies},
	
};

void HotkeysSection() {
	auto& defaultFontSize = UI::g_UIContext.DefaultFontSize;

	GUI_Title("键盘快捷键");

	ImGui::PushStyleColor(ImGuiCol_Button, UI::SwapColorEndianness(0x333642FF));

	bool condition = false;

	for_all(index, keyBindings.size()) {
		auto& keyBinding = keyBindings[index];

		if (keyBinding.showPopup) {
			condition = true;

			break;
		}
	}

	GUI_PushDisable(condition);

	ImVec2 startPos = ImGui::GetCursorScreenPos();

	for_all(index, keyBindings.size()) {
		auto& keyBinding = keyBindings[index];

		ImGui::SetCursorScreenPos(startPos);
		keyBinding.Main();

		startPos.y += 120 * scaleFactorY; // <- matches ImVec2{300 * scaleFactorY, 100 * scaleFactorY} from the keybinding's button height
	}

	GUI_PopDisable(condition);
	ImGui::PopStyleColor();
}

void CommonCheatsSection() {
	TrainingSection();
	CustomDamageSection();
	CustomSpeedSection();
	CustomMobilitySection();
	MiscCheatsSection();
	//if constexpr (debug) {
	DebugSection();
	//}
	//Repair();
}

void CharacterCheatsSection() {
	DanteCheatOptions();
	VergilCheatOptions();
	LegacyDDMKCharactersSection();
}

void PreventEmptyCrimsonGUIHotkey() {
	if (queuedConfig.keyData[0].IsEmpty()) {
		queuedConfig.keyData[0] = defaultConfig.keyData[0];
	}
}

//void CloseTraining() {
//	g_showTraining = false;
//	if (g_scene == SCENE::GAME) {
//		PlayTrack("");
//	}
//}

void TrainingWindow() {
	static bool run = false;
	static std::chrono::steady_clock::time_point trainingOpenedTime;
	static bool trainingCooldownActive = false;

	//if (!g_showTraining) {
		run = false;
		trainingCooldownActive = false;
		return;
	//}
	auto& io = ImGui::GetIO();

	auto missionDataPtr = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E30);
	if (!missionDataPtr) {
		return;
	}
	auto& missionData = *reinterpret_cast<MissionData*>(missionDataPtr);
	auto& queuedMissionActorData = *reinterpret_cast<QueuedMissionActorData*>(missionDataPtr + 0xC0);
	auto& activeMissionActorData = *reinterpret_cast<ActiveMissionActorData*>(missionDataPtr + 0x16C);

	bool unlockDevilTrigger = (activeMissionActorData.maxMagicPoints >= 3000);

	if (!run) {
		// TODO: Play Divine Statue Track - Mia
		//PlayTrack("afs/sound/Jikushinzou.ogg");
		run = true;
		trainingOpenedTime = std::chrono::steady_clock::now();
		trainingCooldownActive = true;
	}

	// Check if cooldown has expired (0.2 seconds)
	if (trainingCooldownActive) {
		auto now = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - trainingOpenedTime).count();
		if (elapsed >= 200) {
			trainingCooldownActive = false;
		}
	}

	float width = 1000 * scaleFactorY;
	float height = 900 * scaleFactorY;

	ImGui::SetNextWindowSize(ImVec2(width, height));
	ImGui::SetNextWindowPos(ImVec2(((g_renderSize.x - width) / 2), ((g_renderSize.y - height) / 2)));

	if (io.NavInputs[ImGuiNavInput_Cancel] && (io.NavInputsDownDuration[ImGuiNavInput_Cancel] == 0.0f)) {
		Log("controller back button");
		if (!io.NavVisible && !trainingCooldownActive) {
			//CloseShop();
			run = false;
			trainingCooldownActive = false;
		}
	}
	//Replace this with a map option at some point. 
	if (io.KeysDown[DI8::KEY::L] && (io.KeysDownDuration[DI8::KEY::L] == 0.0f)) {
		Log("keyboard back button");
		if (!trainingCooldownActive) {
			//CloseShop();
			run = false;
			trainingCooldownActive = false;
		}
	}

	if (ImGui::Begin("TrainingWindow", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize)) {
		ImGui::SetWindowFontScale(scaleFactorY);
		if (GUI_Button("关闭")) {
			if (!trainingCooldownActive) {
				//CloseShop();
				run = false;
				trainingCooldownActive = false;
			}
		}

		auto BACKGROUND_FADED_TEXT = u8"虚空";
		ImFont* fadedFont = UI::g_ImGuiFont_RussoOne256;
		float fadedFontSize = scaledFontSize * 4.8f;

		ImVec2 bgFadedTextSize = ImGui::CalcTextSize((const char*)BACKGROUND_FADED_TEXT,
			nullptr, false, fadedFontSize);

		// Get window position and size
		ImVec2 winPos = ImGui::GetWindowPos();
		ImVec2 winSize = ImGui::GetWindowSize();

		float rightMargin = 600.0f * scaleFactorY;

		float x = winPos.x + winSize.x - bgFadedTextSize.x - rightMargin;
		float y = winPos.y + 15.0f * scaleFactorY; // top margin

		// Only show Divinity Statue text if not in mission start scene

		ImGui::GetWindowDrawList()->AddText(
			fadedFont, fadedFontSize,
			ImVec2(x, y),
			UI::SwapColorEndianness(0xFFFFFF10),
			(const char*)BACKGROUND_FADED_TEXT
		);
		auto& defaultFontSize = UI::g_UIContext.DefaultFontSize;
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowHeight = 40.0f * queuedConfig.globalScale;

		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);

		GUI_Title("训练选项");

		{
			const float columnWidth = 0.5f * queuedConfig.globalScale;
			const float rowWidth = 40.0f * queuedConfig.globalScale * 0.5f;

			if (ImGui::BeginTable("TrainingCheatsOptionsTable", 3)) {

				ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
				ImGui::TableNextRow(0, rowWidth);
				ImGui::TableNextColumn();

				GUI_Checkbox2("玩家免疫伤害", activeCrimsonGameplay.Cheats.Damage.playerReceiveDmgDisable, queuedCrimsonGameplay.Cheats.Damage.playerReceiveDmgDisable);
				ImGui::TableNextColumn();
				GUI_Checkbox2("敌人免疫伤害", activeCrimsonGameplay.Cheats.Damage.enemyRecieveDmgDisable, queuedCrimsonGameplay.Cheats.Damage.enemyRecieveDmgDisable);

				ImGui::TableNextColumn();
				if (GUI_Checkbox2("禁用所有伤害", activeCrimsonGameplay.Cheats.Training.infiniteHP, queuedCrimsonGameplay.Cheats.Training.infiniteHP)) {
					ToggleInfiniteHitPoints(activeCrimsonGameplay.Cheats.Training.infiniteHP);
				}

				ImGui::TableNextColumn();

				if (GUI_Checkbox2("无限魔人化", activeCrimsonGameplay.Cheats.Training.infiniteDT, queuedCrimsonGameplay.Cheats.Training.infiniteDT)) {
					ToggleInfiniteMagicPoints(activeCrimsonGameplay.Cheats.Training.infiniteDT);
				}

				ImGui::TableNextColumn();

				if (GUI_Checkbox2("禁用普通敌人攻击", activeCrimsonGameplay.Cheats.Training.disableRegularEnemyAttacks, queuedCrimsonGameplay.Cheats.Training.disableRegularEnemyAttacks)) {
					CrimsonPatches::DisableRegularEnemyAttacks(activeCrimsonGameplay.Cheats.Training.disableRegularEnemyAttacks);
				}
				ImGui::SameLine();
				GUI_WIPButton();
				ImGui::SameLine();
				TooltipHelper("(?)", "并非对所有普通敌人生效。");

				ImGui::TableNextColumn();

				if (GUI_Checkbox2("禁用计时器", activeCrimsonGameplay.Cheats.Training.disableTimers, queuedCrimsonGameplay.Cheats.Training.disableTimers)) {
					ToggleDisableTimer(activeCrimsonGameplay.Cheats.Training.disableTimers);
				}
				ImGui::TableNextColumn();
				//GUI_Checkbox("turn on void - debug, variable might be removed", CrimsonTrainingRoom::trainingRoomEnabled);
				//CrimsonTrainingRoom::DrawShopWidget();

				if (activeCrimsonGameplay.Cheats.General.legacyDDMKCharacters) {
					ImGui::TableNextColumn();

					GUI_PushDisable(!activeCrimsonGameplay.Cheats.General.legacyDDMKCharacters);
					if (GUI_Checkbox2("无限 B. Lady 子弹", activeCrimsonGameplay.Cheats.Training.infiniteBossLadyBullets,
						queuedCrimsonGameplay.Cheats.Training.infiniteBossLadyBullets)) {
						ToggleInfiniteBullets(activeCrimsonGameplay.Cheats.Training.infiniteBossLadyBullets);
					}
					ImGui::SameLine();
					GUI_PopDisable(!activeCrimsonGameplay.Cheats.General.legacyDDMKCharacters);
					TooltipHelper("(?)", "用于 Boss Lady（旧版 DDMK 角色）。");
				}


				ImGui::EndTable();
			}
		}

		ImGui::PopFont();

		if (ImGui::BeginTable("TeleporterTable", 2)) {
			ImGui::TableSetupColumn("c1", 0, columnWidth * 2.0f);
			ImGui::TableSetupColumn("c2", 0, columnWidth * 2.0f);

			ImGui::TableNextRow(0, rowHeight * 0.75f);
			ImGui::TableNextColumn();

			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("房间设置");
			ImGui::PopFont();
			//CrimsonTrainingRoom::DrawRoomSelect();

			ImGui::TableNextColumn();
			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("敌人设置");
			ImGui::PopFont();
			if (GUI_Button("圣水"))
			{
				//UseHolyWater();
			}
			EnemySpawnerToolSection();
			ImGui::EndTable();

		}
		// 		for (uint8 playerIndex = 0; playerIndex < activeConfig.Actor.playerCount; ++playerIndex) {
		// 			auto& gamepad = GetGamepad(playerIndex);
		// 
		// 			if ((gamepad.buttons[0] & GetBinding(BINDING::STYLE_ACTION))) {
		// 				CloseShop();
		// 			}
		// 		}

		ImGui::End();
	}
}


#pragma endregion

#pragma region Main

void Main(IDXGISwapChain* pSwapChain) {
    if (!g_showMain) {
        return;
    }
    // ImGui::InputScalar("呵呵呵", ImGuiDataType_U64, &g_SampleMod_ReturnAddr1);


    static bool doOnce = false;
	static std::thread versionCheckerThread;

	auto& io = ImGui::GetIO();
	int keyboardBackKey = activeCrimsonConfig.System.KeyboardConfig.keybinds[11];

	// Allow keyboard exit
	if ((io.KeysDown[DI8::KEY::ESCAPE] && (io.KeysDownDuration[DI8::KEY::ESCAPE] == 0.0f))) {
		CloseMain();
	}

    if (!doOnce) {
        doOnce = true;

        // Originally 800x725, or screenWidth / 2.4 x screenHeight / 1.4 for 1080p
		float width = (activeCrimsonConfig.System.windowSizeX != 9999.0f) ? activeCrimsonConfig.System.windowSizeX :g_renderSize.x / 1.40;
		float height = (activeCrimsonConfig.System.windowSizeY != 9999.0f) ? activeCrimsonConfig.System.windowSizeY : g_renderSize.y / 1.20;
		float xpos = (activeCrimsonConfig.System.windowPosX != 9999.0f) ? activeCrimsonConfig.System.windowPosX : g_renderSize.x *0.5f;
		float ypos = (activeCrimsonConfig.System.windowPosY != 9999.0f) ? activeCrimsonConfig.System.windowPosY : g_renderSize.y * 0.5f;


        ImGui::SetNextWindowSize(ImVec2(width, height));

        //((g_renderSize.x - width) / 3)

//         if constexpr (debug) {
//             ImGui::SetNextWindowPos(ImVec2(((g_renderSize.x) / 2), 100)); // Don't know why you'd spawn the screen that way
//         } else {
// 
        // CENTER MAIN SCREEN
		if (activeCrimsonConfig.System.windowPosX == 9999.0f && activeCrimsonConfig.System.windowPosY == 9999.0f) {
			ImGui::SetNextWindowPos(ImVec2(xpos, ypos), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		}
		else {
			ImGui::SetNextWindowPos(ImVec2(xpos, ypos), ImGuiCond_Always, ImVec2(0.0f, 0.0f));
		}



        // ImGuiIO & io = ImGui::GetIO();
        // io.FontDefault = io.Fonts->Fonts[FONT::MAIN];
        // ImGui::PushFont(io.Fonts->Fonts[FONT::OVERLAY_8 + activeConfig.Tools.Overlay.fontSizeIndex]);

        // ImGui::SetCurrentFont(io.Fonts->Fonts[FONT::OVERLAY_8]);

		WebAPICalls::GetInstance().SetVersionCallback([&](WebAPIResult res, Version_t latestVersion) {
			    if (res == WebAPIResult::Success) {
					UI::g_UIContext.LatestVersion.Major = latestVersion.Major;
					UI::g_UIContext.LatestVersion.Minor = latestVersion.Minor;
					UI::g_UIContext.LatestVersion.PatchLetter = latestVersion.PatchLetter;

			    	UI::g_UIContext.LatestUpdateDate.Year = latestVersion.PublishTime.Local.tm_year + 1900;
			    	UI::g_UIContext.LatestUpdateDate.Month = latestVersion.PublishTime.Local.tm_mon + 1;
			    	UI::g_UIContext.LatestUpdateDate.Day = latestVersion.PublishTime.Local.tm_mday;

                    UI::g_UIContext.LatestVersionURL = latestVersion.DownloadURL;

					if (UI::g_UIContext.LatestVersion.Major > UI::g_UIContext.CurrentVersion.Major)
                    {
						UI::g_UIContext.NewVersionAvailable = true;
					}
					else if (UI::g_UIContext.LatestVersion.Major == UI::g_UIContext.CurrentVersion.Major &&
						        UI::g_UIContext.LatestVersion.Minor > UI::g_UIContext.CurrentVersion.Minor)
                    {
						UI::g_UIContext.NewVersionAvailable = true;
					}
                    else if (UI::g_UIContext.LatestVersion.Major == UI::g_UIContext.CurrentVersion.Major &&
                                UI::g_UIContext.LatestVersion.Minor == UI::g_UIContext.CurrentVersion.Minor &&
								UI::g_UIContext.LatestVersion.PatchLetter > UI::g_UIContext.CurrentVersion.PatchLetter)
                    {
                        UI::g_UIContext.NewVersionAvailable = true;
                    }
					else 
                    {
						UI::g_UIContext.NewVersionAvailable = false;
					}
                }

			    UI::versionCheckResult = res;
			}
		);

		WebAPICalls::GetInstance().SetPatronsCallback([&](WebAPIResult res, std::vector<Patron_t> patrons) {
			if (res == WebAPIResult::Success)
			{
				UI::g_UIContext.PatronsDT.clear();
				UI::g_UIContext.PatronsSDT.clear();
				UI::g_UIContext.PatronsLDK.clear();

				for (const auto &patron : patrons)
				{
					switch (patron.Tier)
					{
					case PatreonTiers_t::DT:
					{
						UI::g_UIContext.PatronsDT.push_back(patron.ShownName);

						// Set the name of the tier to be displayed
						UI::g_UIContext.TierNames[(size_t)patron.Tier] = patron.TierName;
					}
					break;

					case PatreonTiers_t::SDT:
					{
						UI::g_UIContext.PatronsSDT.push_back(patron.ShownName);

						// Set the name of the tier to be displayed
						UI::g_UIContext.TierNames[(size_t)patron.Tier] = patron.TierName;
					}
					break;

					case PatreonTiers_t::LDK:
					{
						UI::g_UIContext.PatronsLDK.push_back(patron.ShownName);

						// Set the name of the tier to be displayed
						UI::g_UIContext.TierNames[(size_t)patron.Tier] = patron.TierName;
					}
					break;

					default:
						break;
					}
				}

				UI::patronsQueueResult = res;
			}
			else
			{
				// API failed — use compiled-in data from src/patrons.json
				UI::LoadDefaultPatrons();
				UI::patronsQueueResult = WebAPIResult::Success;
			}
		}
		);

		versionCheckerThread = std::thread{
			[&] {
				WebAPICalls::GetInstance().QueueLatestRelease(60000);
                WebAPICalls::GetInstance().QueuePatrons(60000);
			}
		};
    }

    UI::DrawCrimson(pSwapChain, DMC3C_TITLE, &g_showMain);
}

void DrawMainContent(ID3D11Device* pDevice, UI::UIContext& context) {
	ImGuiWindow* cntWindow = ImGui::GetCurrentWindow();
	const ImRect cntRegion = cntWindow->Rect();
	const ImGuiStyle& style = ImGui::GetStyle();
	float scaleFactorY = ImGui::GetIO().DisplaySize.y / 1080;
	
	float scaledFontSize = context.DefaultFontSize * scaleFactorY;

	// Calculate base font size once - don't multiply by scale factor since that's handled by SetWindowFontScale
	float baseFontSize = context.DefaultFontSize;

	static bool uiElementsInitialized = false;
	if (!uiElementsInitialized) {
		size_t mainLogoWidth = size_t(scaledFontSize * 37.0f);

		g_Image_CrimsonMainLogo.ResizeByRatioW((uint32_t)mainLogoWidth);
		g_Image_VanillaLogo.ResizeByRatioW((uint32_t)mainLogoWidth);
		g_Image_StyleSwitcherLogo.ResizeByRatioW((uint32_t)mainLogoWidth);
		g_Image_SocialIcons.ResizeByRatioW(size_t(scaledFontSize * 10.0f));
		g_Image_SocialsTwitch.ResizeByRatioW(size_t(scaledFontSize * 10.0f));
		g_Image_SocialsBluesky.ResizeByRatioW(size_t(scaledFontSize * 10.0f));

		uiElementsInitialized = true;
	}

	

	switch (context.SelectedTab) {
	case UI::UIContext::MainTabs::GameMode:
	{
		// Reset font scaling for this tab since SetWindowFontScale was applied outside
		ImGui::SetWindowFontScale(1.0f);

		constexpr float align = 0.5f; // Center = 0.5f

		constexpr const char* MODE_SELECTION_TEXT = "Choose your desired Devil May Cry 3 version! GAME MODES govern your style of gameplay.\n"
			"This will affect the entire Gameplay Options globally and tag you at the Mission Result screen.\n\n"
			"If Gameplay Options diverge too much from any preset, the 'CUSTOM MODE' Game Mode will be selected instead automatically.\n";

		float width = ImGui::CalcTextSize(MODE_SELECTION_TEXT).x;

		ImGui::Text("");
		ImGui::Text("");

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (cntRegion.GetWidth() - width) * align);
		ImVec2 spacing = ImGui::GetStyle().ItemSpacing;
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(spacing.x, 0.3f * scaleFactorY));
		ImGui::TextWrapped(MODE_SELECTION_TEXT);
		ImGui::PopStyleVar();
		ImGui::Text("");

		// Use the base font size but apply the scale factor directly to compensate for the reset
		ImGui::PushFont(UI::g_ImGuiFont_Benguiat[baseFontSize * 1.7f]);

		float comboBoxWidth = width * 0.8f;

		std::array<const char*, 3> modes{ "原版模式", "风格切换模式", "CRIMSON 模式" };
		std::array<const char*, 4> modesWCustom{ "原版模式", "风格切换模式", "CRIMSON 模式", "自定义模式" };

		ImGui::SetNextItemWidth(comboBoxWidth);

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (cntRegion.GetWidth() - comboBoxWidth) * align);

		ImU32 frameBG = 0;
		ImU32 frameBGHovered = 0;
		ImU32 textColor = 0;

		switch (context.SelectedGameMode) {
		case UI::UIContext::GameModes::Vanilla:
		{
			frameBG = UI::SwapColorEndianness(0xFFFFFFFF);
			frameBGHovered = UI::SwapColorEndianness(0xFFFFFFAA);
			textColor = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_WindowBgText]);
		}
		break;

		case UI::UIContext::GameModes::StyleSwitcher:
		{
			frameBG = UI::SwapColorEndianness(0xE8BA18FF);
			frameBGHovered = UI::SwapColorEndianness(0xE8BA18AA);
			textColor = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_WindowBgText]);
		}
		break;

		case UI::UIContext::GameModes::Crimson:
		{
			frameBG = UI::SwapColorEndianness(0xDA1B53FF);
			frameBGHovered = UI::SwapColorEndianness(0xDA1B53AA);
			textColor = UI::SwapColorEndianness(0xFFFFFFFF);
		}
		break;

		case UI::UIContext::GameModes::Custom:
		{
			frameBG = UI::SwapColorEndianness(0x4050FFFF);
			frameBGHovered = UI::SwapColorEndianness(0x4050FFAA);
			textColor = UI::SwapColorEndianness(0xFFFFFFFF);
		}
		break;

		default:
		{
			frameBG = UI::SwapColorEndianness(0xFFFFFFFF);
			frameBGHovered = UI::SwapColorEndianness(0xFFFFFFAA);
			textColor = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_WindowBgText]);
		}
		break;
		}

		ImGui::PushStyleColor(ImGuiCol_FrameBg, frameBG);
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, frameBGHovered);
		ImGui::PushStyleColor(ImGuiCol_Text, textColor);;

		context.SelectedGameMode = (UI::UIContext::GameModes)activeCrimsonGameplay.GameMode.preset;
		auto previewValue = context.SelectedGameMode == UI::UIContext::GameModes::Custom ? modesWCustom[size_t(context.SelectedGameMode)] :
			modes[size_t(context.SelectedGameMode)];

		if (UI::BeginCombo("##Game Mode", previewValue, { 0.5f, 0.5f }, 0.9f)) {
			ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, { 0.5f, 0.5f });

			for (size_t i = 0; i < modes.size(); i++) {
				ImU32 headerColor = 0;
				ImU32 headerHoveredColor = 0;
				ImU32 headerActiveColor = 0;
				ImU32 headerTextColor = 0;

				switch (UI::UIContext::GameModes(i)) {
				case UI::UIContext::GameModes::Vanilla:
				{
					headerColor = UI::SwapColorEndianness(0x979797FF);
					headerHoveredColor = UI::SwapColorEndianness(0x979797CC);
					headerActiveColor = UI::SwapColorEndianness(0x979797FF);
					headerTextColor = UI::SwapColorEndianness(0xFFFFFFFF);
				}
				break;

				case UI::UIContext::GameModes::StyleSwitcher:
				{
					headerColor = UI::SwapColorEndianness(0x856E1CFF);
					headerHoveredColor = UI::SwapColorEndianness(0x856E1CCC);
					headerActiveColor = UI::SwapColorEndianness(0x856E1CFF);
					headerTextColor = UI::SwapColorEndianness(0xFFFFFFFF);
				}
				break;

				case UI::UIContext::GameModes::Crimson:
				{
					headerColor = UI::SwapColorEndianness(0x821031FF);
					headerHoveredColor = UI::SwapColorEndianness(0x821031CC);
					headerActiveColor = UI::SwapColorEndianness(0x821031FF);
					headerTextColor = UI::SwapColorEndianness(0xFFFFFFFF);
				}
				break;

				default:
				{
					headerColor = UI::SwapColorEndianness(0xB7B7B7FF);
					headerHoveredColor = UI::SwapColorEndianness(0xB7B7B7CC);
					headerActiveColor = UI::SwapColorEndianness(0xB7B7B7FF);
					headerTextColor = UI::SwapColorEndianness(0xFFFFFFFF);
				}
				break;
				}

				ImGui::PushStyleColor(ImGuiCol_Header, headerColor);
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, headerHoveredColor);
				ImGui::PushStyleColor(ImGuiCol_HeaderActive, headerActiveColor);
				ImGui::PushStyleColor(ImGuiCol_Text, headerTextColor);

				bool isSelected = size_t(context.SelectedGameMode) == i;

				if (ImGui::Selectable(modes[i], isSelected)) {
					CrimsonGameModes::SetGameModePreset((uint8)i);
					CrimsonGameplay::AdjustDMC4MobilitySettings();
					if (activeCrimsonConfig.VisualStyle.tieVisualStyleToGameMode) {
						CrimsonVisualStyle::SetVisualStylePreset((uint8)i); // maps Vanilla->Classic, SS->Hybrid, Crimso->Modern
					}
					::GUI::save = true;
				}

				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}

				ImGui::PopStyleColor(4);
			}

			ImGui::PopStyleVar();

			ImGui::EndCombo();
		}

		ImGui::PopStyleColor(3);
		ImGui::PopFont(); // Pop Benguiat[baseFontSize * 1.7f] from the Game Mode section above

		ImGui::PushFont(UI::g_ImGuiFont_Benguiat[baseFontSize * 0.9f]);
		float visualStyleComboBoxWidth = width * 0.4f;

		// Visual Style dropdown
		{
			// Preview images for Visual Styles tooltips
			static const Texture2DD3D11 vsClassicPreview((std::string(Paths::visualStylesPreviews) + "\\Classic.png").c_str(), pDevice);
			static const Texture2DD3D11 vsHybridPreview((std::string(Paths::visualStylesPreviews) + "\\Hybrid.png").c_str(), pDevice);
			static const Texture2DD3D11 vsModernPreview((std::string(Paths::visualStylesPreviews) + "\\Modern.png").c_str(), pDevice);

			std::array<const char*, 3> visualModes{ "经典画面风格", "混合画面风格", "现代画面风格" };
			std::array<const char*, 4> visualModesWCustom{ "经典画面风格", "混合画面风格", "现代画面风格", "自定义画面风格" };

			uint8 currentPreset = activeCrimsonConfig.VisualStyle.preset;
			auto previewValue = (currentPreset == VISUALSTYLEPRESETS::CUSTOM)
				? visualModesWCustom[size_t(currentPreset)]
				: visualModes[size_t(currentPreset)];

			ImGui::SetNextItemWidth(visualStyleComboBoxWidth);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (cntRegion.GetWidth() - visualStyleComboBoxWidth) * align);

			ImU32 vsFrameBG, vsFrameBGHovered, vsTextColor;
			switch (currentPreset) {
			case VISUALSTYLEPRESETS::CLASSIC:
				vsFrameBG = UI::SwapColorEndianness(0xFFFFFFFF);
				vsFrameBGHovered = UI::SwapColorEndianness(0x979797AA);
				vsTextColor = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_WindowBgText]);
				break;
			case VISUALSTYLEPRESETS::HYBRID:
				vsFrameBG = UI::SwapColorEndianness(0xE8BA18FF);
				vsFrameBGHovered = UI::SwapColorEndianness(0xE8BA18AA);
				vsTextColor = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_WindowBgText]);
				break;
			case VISUALSTYLEPRESETS::MODERN:
				vsFrameBG = UI::SwapColorEndianness(0xDA1B53FF);
				vsFrameBGHovered = UI::SwapColorEndianness(0xDA1B53AA);
				vsTextColor = UI::SwapColorEndianness(0xFFFFFFFF);
				break;
			case VISUALSTYLEPRESETS::CUSTOM:
			default:
				vsFrameBG = UI::SwapColorEndianness(0x4050FFFF);
				vsFrameBGHovered = UI::SwapColorEndianness(0x4050FFAA);
				vsTextColor = UI::SwapColorEndianness(0xFFFFFFFF);
				break;
			}

			ImGui::PushStyleColor(ImGuiCol_FrameBg, vsFrameBG);
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, vsFrameBGHovered);
			ImGui::PushStyleColor(ImGuiCol_Text, vsTextColor);

			GUI_PushDisable(activeCrimsonConfig.VisualStyle.tieVisualStyleToGameMode);

			if (UI::BeginCombo("##Visual Style", previewValue, { 0.5f, 0.5f }, 0.9f)) {
				ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, { 0.5f, 0.5f });

				for (size_t i = 0; i < visualModes.size(); i++) {
					ImU32 headerColor, headerHoveredColor, headerTextColor, headerActiveColor;
					switch ((int)i) {
					case VISUALSTYLEPRESETS::CLASSIC:
						headerColor = UI::SwapColorEndianness(0x6B6B6BFF);
						headerHoveredColor = UI::SwapColorEndianness(0x6B6B6BCC);
						headerTextColor = UI::SwapColorEndianness(0xFFFFFFFF);
						headerActiveColor = UI::SwapColorEndianness(0x6B6B6BFF);
						break;
					case VISUALSTYLEPRESETS::HYBRID:
						headerColor = UI::SwapColorEndianness(0x856E1CFF);
						headerHoveredColor = UI::SwapColorEndianness(0x856E1CCC);
						headerTextColor = UI::SwapColorEndianness(0xFFFFFFFF);
						headerActiveColor = UI::SwapColorEndianness(0x856E1CFF);
						break;
					case VISUALSTYLEPRESETS::MODERN:
						headerColor = UI::SwapColorEndianness(0x821031FF);
						headerHoveredColor = UI::SwapColorEndianness(0x821031CC);
						headerTextColor = UI::SwapColorEndianness(0xFFFFFFFF);
						headerActiveColor = UI::SwapColorEndianness(0x821031FF);
						break;
					default:
						headerColor = UI::SwapColorEndianness(0xB7B7B7FF);
						headerHoveredColor = UI::SwapColorEndianness(0xB7B7B7CC);
						headerTextColor = UI::SwapColorEndianness(0xFFFFFFFF);
						headerActiveColor = UI::SwapColorEndianness(0xB7B7B7FF);
						break;
					}

					ImGui::PushStyleColor(ImGuiCol_Header, headerColor);
					ImGui::PushStyleColor(ImGuiCol_HeaderHovered, headerHoveredColor);
					ImGui::PushStyleColor(ImGuiCol_Text, headerTextColor);
					ImGui::PushStyleColor(ImGuiCol_HeaderActive, headerActiveColor);

					bool isSelected = size_t(currentPreset) == i;

					if (ImGui::Selectable(visualModes[i], isSelected)) {
						CrimsonVisualStyle::SetVisualStylePreset((uint8)i);
						::GUI::save = true;
					}

					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::SetWindowFontScale(scaleFactorY);
						const Texture2DD3D11* previewTex = nullptr;
						if (i == VISUALSTYLEPRESETS::CLASSIC) previewTex = &vsClassicPreview;
						else if (i == VISUALSTYLEPRESETS::HYBRID) previewTex = &vsHybridPreview;
						else if (i == VISUALSTYLEPRESETS::MODERN) previewTex = &vsModernPreview;
						ImGui::Text("预览");
						if (previewTex && previewTex->IsValid()) {
							ImVec2 tipSize = ImVec2(400.0f * scaleFactorY, 225.0f * scaleFactorY);
							ImGui::Image(*previewTex, tipSize);
						}
						ImGui::EndTooltip();
					}

					if (isSelected) {
						ImGui::SetItemDefaultFocus();
					}

					ImGui::PopStyleColor(4);
				}

				ImGui::PopStyleVar();
				ImGui::EndCombo();
			}

			GUI_PopDisable(activeCrimsonConfig.VisualStyle.tieVisualStyleToGameMode);

			ImGui::PopStyleColor(3);
			ImGui::PopFont(); // Pop Benguiat[baseFontSize * 0.9f] from the Visual Style section above

			ImGui::SameLine();
			ImGui::PushFont(UI::g_ImGuiFont_Roboto[baseFontSize]);
			if (GUI_Checkbox2("画面风格随游戏模式", activeCrimsonConfig.VisualStyle.tieVisualStyleToGameMode, 
				queuedCrimsonConfig.VisualStyle.tieVisualStyleToGameMode)) {
				if (activeCrimsonConfig.VisualStyle.tieVisualStyleToGameMode) {
					CrimsonVisualStyle::SetVisualStylePreset(activeCrimsonGameplay.GameMode.preset);
					::GUI::save = true;
				}
			}
			ImGui::PopFont();
		}

		const Texture2DD3D11* pMainLogo = nullptr;
		{
			static const Texture2DD3D11 vanillaLogo(g_Image_VanillaLogo.GetRGBAData(), g_Image_VanillaLogo.GetWidth(), g_Image_VanillaLogo.GetHeight(), pDevice);
			static const Texture2DD3D11 styleSwitcherLogo(g_Image_StyleSwitcherLogo.GetRGBAData(), g_Image_StyleSwitcherLogo.GetWidth(), g_Image_StyleSwitcherLogo.GetHeight(), pDevice);
			static const Texture2DD3D11 crimsonLogo(g_Image_CrimsonMainLogo.GetRGBAData(), g_Image_CrimsonMainLogo.GetWidth(), g_Image_CrimsonMainLogo.GetHeight(), pDevice);

			float heightOffset = 0.0f;

			switch (context.SelectedGameMode) {
			case UI::UIContext::GameModes::Vanilla:
				pMainLogo = &vanillaLogo;
				heightOffset = baseFontSize * scaleFactorY * 2.0f;
				break;

			case UI::UIContext::GameModes::StyleSwitcher:
				pMainLogo = &styleSwitcherLogo;
				heightOffset = -(baseFontSize * scaleFactorY * 3.2f);
				break;

			case UI::UIContext::GameModes::Crimson:
				pMainLogo = &crimsonLogo;
				heightOffset = baseFontSize * scaleFactorY * 2.0f;
				break;

			default:
				pMainLogo = &vanillaLogo;
				break;
			}

			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2{ (cntRegion.GetWidth() - pMainLogo->GetWidth()) * align, heightOffset });

			ImGui::Image(*pMainLogo, pMainLogo->GetSize());
		}

		// Bottom text
		{
			constexpr auto MODE_INFO_TEXT_VANILLA = "体验游戏最初的模样。";
			constexpr auto MODE_INFO_TEXT_SW_LINE1 = "这是你可能熟悉的DMC3，更接近Switch版与DDMK的手感。";
			constexpr auto MODE_INFO_TEXT_SW_LINE2 = "Vanilla + Style / Full Weapon Switching.";
			constexpr auto MODE_INFP_TEXT_CRIMSON = "享受终极DMC3体验！启用全部新玩法改进与扩展。";

			auto descriptionVanilla =
				u8"• No Style Switching\n"
				u8"• Weapons Slots Restricted (2-Slots).\n"
				u8"• Small Quality of Life Changes (like DMC4/5 Lock On Direction).\n";
			
			auto descriptionStyleSwitcher =
				u8"• Style Switching\n"
				u8"• Weapon Wheel (up to 5-Slots)\n"
				u8"• Improved Cancels\n";

			auto descriptionCrimson =
				u8"• Style Switching\n"
				u8"• Inertia / Guardflying\n"
				u8"• Expanded Moveset (Dante & Vergil)\n"
				u8"• Mirage Trigger\n"
				u8"• Increased Jump Cancel Hitboxes\n"
				u8"• Aerial Combat Changes\n"
				u8"• Small Enemy Alterations\n"
				u8"• Weapon Wheel (up to 5-Slots)\n"
				u8"• Improved Cancels\n";

			ImGui::PushFont(UI::g_ImGuiFont_Roboto[baseFontSize]);

			const float vanillaWidth = ImGui::CalcTextSize(MODE_INFO_TEXT_VANILLA).x;
			const float swWidthLine1 = ImGui::CalcTextSize(MODE_INFO_TEXT_SW_LINE1).x;
			const float swWidthLine2 = ImGui::CalcTextSize(MODE_INFO_TEXT_SW_LINE2).x;
			const float crimsonWidth = ImGui::CalcTextSize(MODE_INFP_TEXT_CRIMSON).x;
			const float descriptionVanillaWidth = ImGui::CalcTextSize((const char*)descriptionVanilla).x;
			const float descriptionStyleSwitcherWidth = ImGui::CalcTextSize((const char*)descriptionStyleSwitcher).x;
			const float descriptionCrimsonWidth = ImGui::CalcTextSize((const char*)descriptionCrimson).x;

			switch (context.SelectedGameMode) {
			case UI::UIContext::GameModes::Vanilla:
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (cntRegion.GetWidth() - vanillaWidth) * align);
				ImGui::Text(MODE_INFO_TEXT_VANILLA);
				ImGui::Text("");
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (cntRegion.GetWidth() - descriptionVanillaWidth) * align);
				ImGui::Text((const char*)descriptionVanilla);
				break;

			case UI::UIContext::GameModes::StyleSwitcher:
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - baseFontSize * scaleFactorY * 3.0f);

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (cntRegion.GetWidth() - swWidthLine1) * align);
				ImGui::Text(MODE_INFO_TEXT_SW_LINE1);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (cntRegion.GetWidth() - swWidthLine2) * align);
				ImGui::Text(MODE_INFO_TEXT_SW_LINE2);
				ImGui::Text("");
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (cntRegion.GetWidth() - descriptionStyleSwitcherWidth) * align);
				ImGui::Text((const char*)descriptionStyleSwitcher);
				break;

			case UI::UIContext::GameModes::Crimson:
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + baseFontSize * scaleFactorY * 1.0f);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (cntRegion.GetWidth() - crimsonWidth) * align);
				ImGui::Text(MODE_INFP_TEXT_CRIMSON);
				ImGui::Text("");
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (cntRegion.GetWidth() - descriptionCrimsonWidth) * align);
				ImGui::Text((const char*)descriptionCrimson);
				break;

			default:
				break;
			}

			ImGui::PopFont();
		}

		// Restore window scale for other tabs
		ImGui::SetWindowFontScale(scaleFactorY);
	}
	break;

	case UI::UIContext::MainTabs::Player:
	{
		// Widget area
		{
			const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.7f, 0.98f };
			const ImVec2 areaMin{ cntWindow->Pos.x + 0.1f * context.DefaultFontSize,
									 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
			ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
			ImGui::BeginChildEx("控件区域", cntWindow->GetID("控件区域"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
			ImGui::PopStyleVar();
			{
                CharacterSection(context.DefaultFontSize);
			}
			ImGui::EndChild();
		}

		// Tooltip area
		{
			const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.3f, 0.98f };
			const ImVec2 areaMin{ cntWindow->Pos.x + cntWindow->Size.x - areaSize.x - 0.1f * context.DefaultFontSize,
									 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

			cntWindow->DrawList->AddRect(areaMin, areaMin + areaSize, UI::SwapColorEndianness(0x585152FF));

			ImVec2 padding{ context.DefaultFontSize * 0.8f, context.DefaultFontSize * 0.8f };

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
			ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
			ImGui::BeginChildEx("提示区域", cntWindow->GetID("提示区域"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
			ImGui::PopStyleVar();
			{
                constexpr auto ACTOR_SYSTEM_INTRO = u8"由DDMK的Serpentiem（原“Actor System”）主导开发的自定义角色创建与生成系统。";
                constexpr auto FEATURE_STYLE_SWITCH = u8"   • Style Switching;";
                constexpr auto FEATURE_WEAPONS = u8"   • Custom Weapon Loadouts;";
                constexpr auto FEATURE_CHAR_SWITCH = u8"   • Character Switching;";
                constexpr auto FEATURE_MULTIPLAYER = u8"   • Local Multiplayer;";
                constexpr auto FEATURE_DOPPEL_TWEAKS = u8"   • Doppelganger Tweaks;";

				ImGui::PushFont(UI::g_ImGuiFont_RussoOne[size_t(context.DefaultFontSize * 1.0f)]);
				ImGui::Text("绯红角色系统");
				ImGui::PopFont();


				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.8f);
				ImGui::PushFont(UI::g_ImGuiFont_Roboto[size_t(context.DefaultFontSize * 0.9f)]);

                ImGui::TextWrapped((const char*) ACTOR_SYSTEM_INTRO);
                ImGui::TextWrapped("启用大多数玩法选项的必要前提。");
                ImGui::TextWrapped("");
                ImGui::TextWrapped("Features:");
                ImGui::TextWrapped((const char*) FEATURE_STYLE_SWITCH);
                ImGui::TextWrapped((const char*) FEATURE_WEAPONS);
                ImGui::TextWrapped((const char*) FEATURE_CHAR_SWITCH);
                ImGui::TextWrapped((const char*) FEATURE_MULTIPLAYER);
                ImGui::TextWrapped((const char*) FEATURE_DOPPEL_TWEAKS);
                ImGui::TextWrapped("");
                ImGui::TextWrapped("");
                ImGui::TextWrapped("绯红与风格切换模式所必需。");
                ImGui::TextWrapped("注意：会改变女神像（商店）界面。");
				ImGui::TextWrapped("为保持稳定，在兄弟之战与片尾字幕期间会自动禁用。");


				ImGui::PopFont();
			}
			ImGui::EndChild();
		}
	}
	break;

	case UI::UIContext::MainTabs::Quickplay:
	{
		// Widget area
		{
			const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.7f, 0.98f };
			const ImVec2 areaMin{ cntWindow->Pos.x + 0.1f * context.DefaultFontSize,
									 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
			ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
			ImGui::BeginChildEx("控件区域", cntWindow->GetID("控件区域"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
			ImGui::PopStyleVar();
            {
                ArcadeSection(context.DefaultFontSize);
                BossRushSection(context.DefaultFontSize);
            }
			ImGui::EndChild();
		}

		// Tooltip area
		{
			const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.3f, 0.98f };
			const ImVec2 areaMin{ cntWindow->Pos.x + cntWindow->Size.x - areaSize.x - 0.1f * context.DefaultFontSize,
									 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

			cntWindow->DrawList->AddRect(areaMin, areaMin + areaSize, UI::SwapColorEndianness(0x585152FF));

			ImVec2 padding{ context.DefaultFontSize * 0.8f, context.DefaultFontSize * 0.8f };

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
			ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
			ImGui::BeginChildEx("提示区域", cntWindow->GetID("提示区域"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
			ImGui::PopStyleVar();
			{
				ImGui::PushFont(UI::g_ImGuiFont_RussoOne[size_t(context.DefaultFontSize * 1.0f)]);
				ImGui::Text("街机模式");
                ImGui::PopFont();
				

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.8f);
                ImGui::PushFont(UI::g_ImGuiFont_Roboto[size_t(context.DefaultFontSize * 0.9f)]);

				ImGui::TextWrapped("Jumps you directly to a specific point in the game, triggered in the Main Menu. \n\nThis will tag you at the Mission End Screen.");
                ImGui::PopFont();
			}
			ImGui::EndChild();
		}
	}
	break;

	case UI::UIContext::MainTabs::MusicSwitcher:
	{

	}
	break;

	case UI::UIContext::MainTabs::Options:
	{
        if (context.SelectedOptionsSubTab == UI::UIContext::OptionsSubTabs::Gameplay) {
			// Widget area
			{
				const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.7f, 0.98f };
				const ImVec2 areaMin{ cntWindow->Pos.x + 0.1f * context.DefaultFontSize,
										 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
				ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
				ImGui::BeginChildEx("控件区域", cntWindow->GetID("控件区域"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
				ImGui::PopStyleVar();
				{
					{
						//GamepadClose(visibleMain, lastVisibleMain, CloseMain);

						GameplaySection();
					}
				}
				ImGui::EndChild();
			}

			// Tooltip area
            {
                const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.3f, 0.98f };
                const ImVec2 areaMin{ cntWindow->Pos.x + cntWindow->Size.x - areaSize.x - 0.1f * context.DefaultFontSize,
                                         cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

                cntWindow->DrawList->AddRect(areaMin, areaMin + areaSize, UI::SwapColorEndianness(0x585152FF));

                ImVec2 padding{ context.DefaultFontSize * 0.8f, context.DefaultFontSize * 0.8f };

                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
                ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
                ImGui::BeginChildEx("提示区域", cntWindow->GetID("提示区域"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
                ImGui::PopStyleVar();
                {
                    ImGui::PushFont(UI::g_ImGuiFont_RussoOne[size_t(context.DefaultFontSize * 1.0f)]);
                    ImGui::Text("玩法选项");
                    ImGui::PopFont();


                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.8f);
                    ImGui::PushFont(UI::g_ImGuiFont_Roboto[size_t(context.DefaultFontSize * 0.9f)]);

                    ImGui::TextWrapped("These options will largely affect your gameplay experience and govern the Game Modes.\n\n"
						"部分玩法选项在各游戏模式下可选择是否生效，部分则不可。");
                    ImGui::PopFont();
                }
                ImGui::EndChild();
            }
        }
        else if (context.SelectedOptionsSubTab == UI::UIContext::OptionsSubTabs::Camera) {
			// Widget area
			{
				const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.7f, 0.98f };
				const ImVec2 areaMin{ cntWindow->Pos.x + 0.1f * context.DefaultFontSize,
										 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
				ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
				ImGui::BeginChildEx("控件区域", cntWindow->GetID("控件区域"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
				ImGui::PopStyleVar();
				{
					{
                        CameraSection(context.DefaultFontSize);
					}
				}
				ImGui::EndChild();
			}

			// Tooltip area
			{
				const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.3f, 0.98f };
				const ImVec2 areaMin{ cntWindow->Pos.x + cntWindow->Size.x - areaSize.x - 0.1f * context.DefaultFontSize,
										 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

				cntWindow->DrawList->AddRect(areaMin, areaMin + areaSize, UI::SwapColorEndianness(0x585152FF));

				ImVec2 padding{ context.DefaultFontSize * 0.8f, context.DefaultFontSize * 0.8f };

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
				ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
				ImGui::BeginChildEx("提示区域", cntWindow->GetID("提示区域"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
				ImGui::PopStyleVar();
				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[size_t(context.DefaultFontSize * 1.0f)]);
					ImGui::Text("镜头选项");
					ImGui::PopFont();


					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.8f);
					ImGui::PushFont(UI::g_ImGuiFont_Roboto[size_t(context.DefaultFontSize * 0.9f)]);

					ImGui::TextWrapped("与镜头行为相关的设置。");
					ImGui::PopFont();
				}
				ImGui::EndChild();
			}
        }
        else if (context.SelectedOptionsSubTab == UI::UIContext::OptionsSubTabs::Hotkeys) {
			// Widget area
			{
				const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.7f, 0.98f };
				const ImVec2 areaMin{ cntWindow->Pos.x + 0.1f * context.DefaultFontSize,
										 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
				ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
				ImGui::BeginChildEx("控件区域", cntWindow->GetID("控件区域"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
				ImGui::PopStyleVar();
				{
					{
						HotkeysSection();
					}
				}
				ImGui::EndChild();
			}

			//Tooltip area
			{
				const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.3f, 0.98f };
				const ImVec2 areaMin{ cntWindow->Pos.x + cntWindow->Size.x - areaSize.x - 0.1f * context.DefaultFontSize,
										 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

				cntWindow->DrawList->AddRect(areaMin, areaMin + areaSize, UI::SwapColorEndianness(0x585152FF));

				ImVec2 padding{ context.DefaultFontSize * 0.8f, context.DefaultFontSize * 0.8f };

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
				ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
				ImGui::BeginChildEx("提示区域", cntWindow->GetID("提示区域"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
				ImGui::PopStyleVar();
				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[size_t(context.DefaultFontSize * 1.0f)]);
					ImGui::Text("快捷键选项");
					ImGui::PopFont();


					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.8f);
					ImGui::PushFont(UI::g_ImGuiFont_Roboto[size_t(context.DefaultFontSize * 0.9f)]);

					ImGui::TextWrapped("将各种设置绑定到键盘快捷键。");
					ImGui::PopFont();
				}
				ImGui::EndChild();
			}
        }
        else if (context.SelectedOptionsSubTab == UI::UIContext::OptionsSubTabs::Interface) {
            CrimsonFiles::GetHUDsDirectories();
			CrimsonFiles::GetStyleRanksDirectories();
			CrimsonFiles::GetStyleRanksAccoladesDirectories();

			// Widget area
			{
				const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.7f, 0.98f };
				const ImVec2 areaMin{ cntWindow->Pos.x + 0.1f * context.DefaultFontSize,
										 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
				ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
				ImGui::BeginChildEx("控件区域", cntWindow->GetID("控件区域"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
				ImGui::PopStyleVar();
				{
					{
						InterfaceSection(context.DefaultFontSize, pDevice);
					}
				}
				ImGui::EndChild();
			}

			//Tooltip area
			{
				const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.3f, 0.98f };
				const ImVec2 areaMin{ cntWindow->Pos.x + cntWindow->Size.x - areaSize.x - 0.1f * context.DefaultFontSize,
										 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

				cntWindow->DrawList->AddRect(areaMin, areaMin + areaSize, UI::SwapColorEndianness(0x585152FF));

				ImVec2 padding{ context.DefaultFontSize * 0.8f, context.DefaultFontSize * 0.8f };

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
				ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
				ImGui::BeginChildEx("提示区域", cntWindow->GetID("提示区域"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
				ImGui::PopStyleVar();
				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[size_t(context.DefaultFontSize * 1.0f)]);
					ImGui::Text("界面选项");
					ImGui::PopFont();


					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.8f);
					ImGui::PushFont(UI::g_ImGuiFont_Roboto[size_t(context.DefaultFontSize * 0.9f)]);

					ImGui::TextWrapped("更改绯红界面的设置。");
					ImGui::PopFont();
				}
				ImGui::EndChild();
			}
        }
        else if (context.SelectedOptionsSubTab == UI::UIContext::OptionsSubTabs::Sound) {
			// Widget area
			{
				const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.7f, 0.98f };
				const ImVec2 areaMin{ cntWindow->Pos.x + 0.1f * context.DefaultFontSize,
										 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
				ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
				ImGui::BeginChildEx("控件区域", cntWindow->GetID("控件区域"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
				ImGui::PopStyleVar();
				{
					{
						SoundSection(context.DefaultFontSize);
					}
				}
				ImGui::EndChild();
			}

			// Tooltip area
			{
				const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.3f, 0.98f };
				const ImVec2 areaMin{ cntWindow->Pos.x + cntWindow->Size.x - areaSize.x - 0.1f * context.DefaultFontSize,
										 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

				cntWindow->DrawList->AddRect(areaMin, areaMin + areaSize, UI::SwapColorEndianness(0x585152FF));

				ImVec2 padding{ context.DefaultFontSize * 0.8f, context.DefaultFontSize * 0.8f };

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
				ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
				ImGui::BeginChildEx("提示区域", cntWindow->GetID("提示区域"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
				ImGui::PopStyleVar();
				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[size_t(context.DefaultFontSize * 1.0f)]);
					ImGui::Text("音效选项");
					ImGui::PopFont();


					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.8f);
					ImGui::PushFont(UI::g_ImGuiFont_Roboto[size_t(context.DefaultFontSize * 0.9f)]);

					ImGui::TextWrapped("与音效相关的设置。");
					ImGui::PopFont();
				}
				ImGui::EndChild();
			}
        }
        else if (context.SelectedOptionsSubTab == UI::UIContext::OptionsSubTabs::Visual) {
			// Widget area
			{
				const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.7f, 0.98f };
				const ImVec2 areaMin{ cntWindow->Pos.x + 0.1f * context.DefaultFontSize,
										 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
				ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
				ImGui::BeginChildEx("控件区域", cntWindow->GetID("控件区域"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
				ImGui::PopStyleVar();
				{
					{
						VisualSection(context.DefaultFontSize);
					}
				}
				ImGui::EndChild();
			}

			// Tooltip area
			{
				const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.3f, 0.98f };
				const ImVec2 areaMin{ cntWindow->Pos.x + cntWindow->Size.x - areaSize.x - 0.1f * context.DefaultFontSize,
										 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

				cntWindow->DrawList->AddRect(areaMin, areaMin + areaSize, UI::SwapColorEndianness(0x585152FF));

				ImVec2 padding{ context.DefaultFontSize * 0.8f, context.DefaultFontSize * 0.8f };

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
				ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
				ImGui::BeginChildEx("提示区域", cntWindow->GetID("提示区域"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
				ImGui::PopStyleVar();
				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[size_t(context.DefaultFontSize * 1.0f)]);
					ImGui::Text("画面选项");
					ImGui::PopFont();


					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.8f);
					ImGui::PushFont(UI::g_ImGuiFont_Roboto[size_t(context.DefaultFontSize * 0.9f)]);

					ImGui::TextWrapped("与各种视觉效果相关的设置。");
					ImGui::PopFont();
				}
				ImGui::EndChild();
			}
        }
        else if (context.SelectedOptionsSubTab == UI::UIContext::OptionsSubTabs::System) {
			// Widget area
			{
				const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.7f, 0.98f };
				const ImVec2 areaMin{ cntWindow->Pos.x + 0.1f * context.DefaultFontSize,
										 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
				ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
				ImGui::BeginChildEx("控件区域", cntWindow->GetID("控件区域"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
				ImGui::PopStyleVar();
				{
					{
						SystemSection(context.DefaultFontSize);
					}
				}
				ImGui::EndChild();
			}

			// Tooltip area
			{
				const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.3f, 0.98f };
				const ImVec2 areaMin{ cntWindow->Pos.x + cntWindow->Size.x - areaSize.x - 0.1f * context.DefaultFontSize,
										 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

				cntWindow->DrawList->AddRect(areaMin, areaMin + areaSize, UI::SwapColorEndianness(0x585152FF));

				ImVec2 padding{ context.DefaultFontSize * 0.8f, context.DefaultFontSize * 0.8f };

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
				ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
				ImGui::BeginChildEx("提示区域", cntWindow->GetID("提示区域"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
				ImGui::PopStyleVar();
				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[size_t(context.DefaultFontSize * 1.0f)]);
					ImGui::Text("系统选项");
					ImGui::PopFont();


					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.8f);
					ImGui::PushFont(UI::g_ImGuiFont_Roboto[size_t(context.DefaultFontSize * 0.9f)]);

					ImGui::TextWrapped("与程序行为相关的设置。");
					ImGui::PopFont();
				}
				ImGui::EndChild();
			}
        }


	}
	break;

	case UI::UIContext::MainTabs::CheatsAndDebug:
	{
		if (context.SelectedCheatsAndDebugSubTab == UI::UIContext::CheatsAndDebugSubTabs::CommonCheats) {
			// Widget area
			{
				const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.7f, 0.98f };
				const ImVec2 areaMin{ cntWindow->Pos.x + 0.1f * context.DefaultFontSize,
										 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
				ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
				ImGui::BeginChildEx("控件区域", cntWindow->GetID("控件区域"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
				ImGui::PopStyleVar();
				{
					{
						CommonCheatsSection();
					}
				}
				ImGui::EndChild();
			}

			// Tooltip area
			{
				const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.3f, 0.98f };
				const ImVec2 areaMin{ cntWindow->Pos.x + cntWindow->Size.x - areaSize.x - 0.1f * context.DefaultFontSize,
										 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

				cntWindow->DrawList->AddRect(areaMin, areaMin + areaSize, UI::SwapColorEndianness(0x585152FF));

				ImVec2 padding{ context.DefaultFontSize * 0.8f, context.DefaultFontSize * 0.8f };

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
				ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
				ImGui::BeginChildEx("提示区域", cntWindow->GetID("提示区域"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
				ImGui::PopStyleVar();
				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[size_t(context.DefaultFontSize * 1.0f)]);
					ImGui::Text("通用作弊");
					ImGui::PopFont();


					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.8f);
					ImGui::PushFont(UI::g_ImGuiFont_Roboto[size_t(context.DefaultFontSize * 0.9f)]);

					ImGui::TextWrapped("Various Cheats for you to customize the game to your liking.\n"
						"这会在任务结算界面标记你，并可能将游戏模式改为“自定义模式”。");
					ImGui::PopFont();
				}
				ImGui::EndChild();
			}
		}
		else if (context.SelectedCheatsAndDebugSubTab == UI::UIContext::CheatsAndDebugSubTabs::CharacterCheats) {
			// Widget area
			{
				const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.7f, 0.98f };
				const ImVec2 areaMin{ cntWindow->Pos.x + 0.1f * context.DefaultFontSize,
										 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
				ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
				ImGui::BeginChildEx("控件区域", cntWindow->GetID("控件区域"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
				ImGui::PopStyleVar();
				{
					{
						CharacterCheatsSection();
					}
				}
				ImGui::EndChild();
			}

			// Tooltip area
			{
				const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.3f, 0.98f };
				const ImVec2 areaMin{ cntWindow->Pos.x + cntWindow->Size.x - areaSize.x - 0.1f * context.DefaultFontSize,
										 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

				cntWindow->DrawList->AddRect(areaMin, areaMin + areaSize, UI::SwapColorEndianness(0x585152FF));

				ImVec2 padding{ context.DefaultFontSize * 0.8f, context.DefaultFontSize * 0.8f };

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
				ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
				ImGui::BeginChildEx("提示区域", cntWindow->GetID("提示区域"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
				ImGui::PopStyleVar();
				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[size_t(context.DefaultFontSize * 1.0f)]);
					ImGui::Text("角色作弊选项");
					ImGui::PopFont();


					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.8f);
					ImGui::PushFont(UI::g_ImGuiFont_Roboto[size_t(context.DefaultFontSize * 0.9f)]);

					ImGui::TextWrapped("Cheats for several Character-based Gameplay changes.\n"
						"这会在任务结算界面标记你，并可能将游戏模式改为“自定义模式”。");
					ImGui::PopFont();
				}
				ImGui::EndChild();
			}
		}
		else if (context.SelectedCheatsAndDebugSubTab == UI::UIContext::CheatsAndDebugSubTabs::Teleporter) {
			// Widget area
			{
				const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.7f, 0.98f };
				const ImVec2 areaMin{ cntWindow->Pos.x + 0.1f * context.DefaultFontSize,
										 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
				ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
				ImGui::BeginChildEx("控件区域", cntWindow->GetID("控件区域"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
				ImGui::PopStyleVar();
				{
					{
						TeleporterToolSection();
					}
				}
				ImGui::EndChild();
			}

			// Tooltip area
			{
				const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.3f, 0.98f };
				const ImVec2 areaMin{ cntWindow->Pos.x + cntWindow->Size.x - areaSize.x - 0.1f * context.DefaultFontSize,
										 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

				cntWindow->DrawList->AddRect(areaMin, areaMin + areaSize, UI::SwapColorEndianness(0x585152FF));

				ImVec2 padding{ context.DefaultFontSize * 0.8f, context.DefaultFontSize * 0.8f };

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
				ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
				ImGui::BeginChildEx("提示区域", cntWindow->GetID("提示区域"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
				ImGui::PopStyleVar();
				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[size_t(context.DefaultFontSize * 1.0f)]);
					ImGui::Text("传送工具");
					ImGui::PopFont();


					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.8f);
					ImGui::PushFont(UI::g_ImGuiFont_Roboto[size_t(context.DefaultFontSize * 0.9f)]);

					ImGui::TextWrapped("Teleport anywhere.\nEnabling this will tag you at the Mission Result screen.");
					ImGui::PopFont();
				}
				ImGui::EndChild();
			}

		}
		else if (context.SelectedCheatsAndDebugSubTab == UI::UIContext::CheatsAndDebugSubTabs::EnemySpawner) {
			// Widget area
			{
				const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.7f, 0.98f };
				const ImVec2 areaMin{ cntWindow->Pos.x + 0.1f * context.DefaultFontSize,
										 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
				ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
				ImGui::BeginChildEx("控件区域", cntWindow->GetID("控件区域"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
				ImGui::PopStyleVar();
				{
					{
						EnemySpawnerToolSection();
					}
				}
				ImGui::EndChild();
			}

			// Tooltip area
			{
				const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.3f, 0.98f };
				const ImVec2 areaMin{ cntWindow->Pos.x + cntWindow->Size.x - areaSize.x - 0.1f * context.DefaultFontSize,
										 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

				cntWindow->DrawList->AddRect(areaMin, areaMin + areaSize, UI::SwapColorEndianness(0x585152FF));

				ImVec2 padding{ context.DefaultFontSize * 0.8f, context.DefaultFontSize * 0.8f };

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
				ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
				ImGui::BeginChildEx("提示区域", cntWindow->GetID("提示区域"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
				ImGui::PopStyleVar();
				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[size_t(context.DefaultFontSize * 1.0f)]);
					ImGui::Text("敌人生成器工具");
					ImGui::PopFont();


					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.8f);
					ImGui::PushFont(UI::g_ImGuiFont_Roboto[size_t(context.DefaultFontSize * 0.9f)]);

					ImGui::TextWrapped("Spawn new enemies at your leisure.\nEnabling this will tag you at the Mission Result screen.");
					ImGui::PopFont();
				}
				ImGui::EndChild();
			}
		}
		else if (context.SelectedCheatsAndDebugSubTab == UI::UIContext::CheatsAndDebugSubTabs::FMODJukebox) {
			// Widget area
			{
				const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.7f, 0.98f };
				const ImVec2 areaMin{ cntWindow->Pos.x + 0.1f * context.DefaultFontSize,
										 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
				ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
				ImGui::BeginChildEx("控件区域", cntWindow->GetID("控件区域"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
				ImGui::PopStyleVar();
				{
					{
						FMODJukeboxSection();
					}
				}
				ImGui::EndChild();
			}

			// Tooltip area
			{
				const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.3f, 0.98f };
				const ImVec2 areaMin{ cntWindow->Pos.x + cntWindow->Size.x - areaSize.x - 0.1f * context.DefaultFontSize,
										 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

				cntWindow->DrawList->AddRect(areaMin, areaMin + areaSize, UI::SwapColorEndianness(0x585152FF));

				ImVec2 padding{ context.DefaultFontSize * 0.8f, context.DefaultFontSize * 0.8f };

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
				ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
				ImGui::BeginChildEx("提示区域", cntWindow->GetID("提示区域"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
				ImGui::PopStyleVar();
				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[size_t(context.DefaultFontSize * 1.0f)]);
					ImGui::Text("FMOD 点唱机工具");
					ImGui::PopFont();


					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.8f);
					ImGui::PushFont(UI::g_ImGuiFont_Roboto[size_t(context.DefaultFontSize * 0.9f)]);

					ImGui::TextWrapped("使用原版游戏的音频系统（FMOD），尽情播放原版游戏歌曲。");
					ImGui::PopFont();
				}
				ImGui::EndChild();
			}
		}
	}
	break;

	case UI::UIContext::MainTabs::None:
	{
		// If none of the tabs are selected, draw the about page
		static const Texture2DD3D11 socialIcons(g_Image_SocialIcons.GetRGBAData(), g_Image_SocialIcons.GetWidth(), g_Image_SocialIcons.GetWidth(), pDevice);
		static const Texture2DD3D11 socialsTwitch(g_Image_SocialsTwitch.GetRGBAData(), g_Image_SocialsTwitch.GetWidth(), g_Image_SocialsTwitch.GetWidth(), pDevice);
		static const Texture2DD3D11 socialsBluesky(g_Image_SocialsBluesky.GetRGBAData(), g_Image_SocialsBluesky.GetWidth(), g_Image_SocialsBluesky.GetWidth(), pDevice);

		const float areaPaddingXRation = 0.326f;
		const float areaPaddingX = (1.0f - 3.0f * (areaPaddingXRation)) * 0.25f * cntWindow->Size.x;

		// C Team area
		{
			const ImVec2 areaSize = cntWindow->Size * ImVec2{ areaPaddingXRation, 1.0f };
			const ImVec2 areaMin{ cntWindow->Pos.x + areaPaddingX,
									 cntWindow->Pos.y + scaledFontSize * 0.1f };

			//cntWindow->DrawList->AddRect(areaMin, areaMin + areaSize, UI::SwapColorEndianness(0x00FF00FF));

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { scaledFontSize * 0.4f, scaledFontSize * 0.4f });
			ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
			   // Make the C Team Area child window auto-scroll vertically, show scroll bar, pause on user scroll, linger at start/end
			   static float creditsScroll = 0.0f;
			   static double lastTime = ImGui::GetTime();
			   static float pauseTimer = 0.0f;
			   static float lingerStart = 1.5f; // seconds to linger at start
			   static float lingerEnd = 1.5f;   // seconds to linger at end
			   static bool atStart = true;
			   static bool atEnd = false;

			   double currentTime = ImGui::GetTime();
			   float delta = float(currentTime - lastTime);
			   lastTime = currentTime;
			   float scrollSpeed = 25.0f; // pixels per second, adjust as needed

			   ImGui::BeginChildEx("C Team 区域", cntWindow->GetID("C Team 区域"), ImVec2(areaSize.x, 650 * scaleFactorY), false,
				   ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_AlwaysVerticalScrollbar);

			   float maxScroll = ImGui::GetScrollMaxY();
			   float userScroll = ImGui::GetScrollY();

			   if (fabsf(userScroll - creditsScroll) > 1.0f) {
				   // User scrolled, pause auto-scroll for 2 seconds
				   pauseTimer = 2.0f;
				   creditsScroll = userScroll;
			   }

			   if (pauseTimer > 0.0f) {
				   pauseTimer -= delta;
			   } else {
				   if (creditsScroll <= 0.0f) {
					   if (!atStart) { pauseTimer = lingerStart; atStart = true; }
				   } else if (creditsScroll >= maxScroll) {
					   if (!atEnd) { pauseTimer = lingerEnd; atEnd = true; }
				   } else {
					   atStart = false;
					   atEnd = false;
				   }
				   if (pauseTimer <= 0.0f && !atEnd) {
					   creditsScroll += scrollSpeed * delta;
					   if (creditsScroll > maxScroll) {
						   creditsScroll = maxScroll;
					   }
				   }
				   if (atEnd && pauseTimer <= 0.0f) {
					   creditsScroll = 0.0f;
					   atStart = true;
					   atEnd = false;
					   pauseTimer = lingerStart;
				   }
			   }

			   ImGui::SetScrollY(creditsScroll);
			ImGui::PopStyleVar();
			{
				auto window = ImGui::GetCurrentWindow();
				ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 1.4f)]);
				{
					ImGui::Text("制作人员");
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + scaledFontSize * 0.2f);
					ImGui::Text((const char*)u8"C•Team");
				}
				ImGui::PopFont();

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + scaledFontSize * 0.8f);

				auto fnDrawSocialButton = [window](const void* id, const size_t socialID, const ImVec2 size, const char* tooltip = nullptr) -> bool {
					const auto bbTuple = g_Image_SocialIcons.GetUVRect(socialID);

					const ImRect bbUV{
						ImVec2{ std::get<0>(std::get<0>(bbTuple)), std::get<1>(std::get<0>(bbTuple)) },
						ImVec2{ std::get<0>(std::get<1>(bbTuple)), std::get<1>(std::get<1>(bbTuple)) }
					};

					bool clicked = false;

					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
					//ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
					{
						ImGui::PushID(id); // Ensure unique ID context for each button
						const ImGuiID calcedID = window->GetID("#socialButton");

						clicked = ImGui::ImageButtonEx(calcedID, socialIcons.GetTexture(), size,
							bbUV.Min, bbUV.Max, ImVec2{ 2.0f, 2.0f },
							ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f }, ImVec4{ 1.0f , 1.0f, 1.0f, 1.0f });

						if (tooltip != nullptr && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
							ImGui::SetTooltip(tooltip);
						}
						ImGui::PopID(); // Pop after button creation to maintain correct ID scope
					}
					//ImGui::PopStyleColor();
					ImGui::PopStyleVar();

					return clicked;
					};

				auto fnDrawSocialButtonNonUV = [window](const void* id, const Texture2DD3D11& texture, const ImVec2 size, const char* tooltip = nullptr) -> bool {
					bool clicked = false;

					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
					{
						ImGui::PushID(id);
						const ImGuiID calcedID = window->GetID("#socialButton");
						ImGui::PopID();

						clicked = ImGui::ImageButtonEx(
							calcedID,
							texture.GetTexture(),
							size,
							ImVec2{ 0.0f, 0.0f }, ImVec2{ 1.0f, 1.0f },
							ImVec2{ 2.0f, 2.0f },
							ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f },
							ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f }
						);

						if (tooltip != nullptr && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
							ImGui::SetTooltip("%s", tooltip);
						}
					}
					ImGui::PopStyleVar();

					return clicked;
					};

				auto fnDrawSocialBluesky = [window](const void* id, const ImVec2 size, const char* tooltip = nullptr) -> bool {
					bool clicked = false;

					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
					{
						ImGui::PushID(id);
						const ImGuiID calcedID = window->GetID("#socialButton");
						ImGui::PopID();

						// Use the full image, so UVs are (0,0) to (1,1)
						clicked = ImGui::ImageButtonEx(
							calcedID,
							socialsBluesky.GetTexture(),
							size,
							ImVec2{ 0.0f, 0.0f }, ImVec2{ 1.0f, 1.0f },
							ImVec2{ 2.0f, 2.0f },
							ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f },
							ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f }
						);

						if (tooltip != nullptr && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
							ImGui::SetTooltip("%s", tooltip);
						}
					}
					ImGui::PopStyleVar();

					return clicked;
					};

				// Mia
				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 0.8f)]);
					{
						ImGui::Text("项目总监、首席程序员、游戏设计、逆向工程");
					}
					ImGui::PopFont();

					ImGui::Separator();

					ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
					{
						ImGui::Text("Berthrage");

						ImGui::SameLine();

						const ImVec2 socialsBBFrameSize{ 6.0f * 4.0f + 6.0f * ImGui::GetFontSize(), 4.0f + ImGui::GetFontSize() };
						const ImVec2 currentCursorPos = ImGui::GetCursorScreenPos();

						ImGui::SetCursorScreenPos(ImVec2{ window->ContentRegionRect.Max.x - socialsBBFrameSize.x, currentCursorPos.y });

						if (fnDrawSocialButton("miatwitter", SocialsIcons::ID_Twitter, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
							ShellExecute(0, 0, "https://twitter.com/Berthrage", 0, 0, SW_SHOW);
						}

						ImGui::SameLine(0.0f, 0.0f);

						if (fnDrawSocialButton("miagithub", SocialsIcons::ID_Github, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
							ShellExecute(0, 0, "https://github.com/berthrage", 0, 0, SW_SHOW);
						}

						ImGui::SameLine(0.0f, 0.0f);

						if (fnDrawSocialButton("miayt", SocialsIcons::ID_YouTube, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
							ShellExecute(0, 0, "https://www.youtube.com/@Berthrage", 0, 0, SW_SHOW);
						}

						ImGui::SameLine(0.0f, 0.0f);

						if (fnDrawSocialButton("miareddit", SocialsIcons::ID_Reddit, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
							ShellExecute(0, 0, "https://www.reddit.com/user/Berthrage", 0, 0, SW_SHOW);
						}

						ImGui::SameLine(0.0f, 0.0f);

						if (fnDrawSocialButton("miadiscord", SocialsIcons::ID_Discord, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() }, "@miaberth")) {
							// TODO:: Copy
						}

						ImGui::SameLine(0.0f, 0.0f);

						if (fnDrawSocialButton("mianexus", SocialsIcons::ID_Nexusmods, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
							ShellExecute(0, 0, "https://www.nexusmods.com/users/95598128", 0, 0, SW_SHOW);
						}
					}
					ImGui::PopFont();
				}

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + scaledFontSize * 0.8f);

				// Sarah
				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 0.8f)]);
					{
						ImGui::Text("逆向工程、首席玩法程序员、QA、测试");
					}
					ImGui::PopFont();

					ImGui::Separator();

					ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
					{
						ImGui::Text("Siyan");
					}
					ImGui::PopFont();

					ImGui::SameLine();

					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 0.9f)]);

					const float socialIconSize = ImGui::GetFontSize();
					const float kofiBtnWidth = ImGui::CalcTextSize("KO-FI").x + ImGui::GetStyle().FramePadding.x * 2.0f;
					const float totalBtnWidth = socialIconSize + 4.0f + kofiBtnWidth + 4.0f;
					const ImVec2 currentCursorPos = ImGui::GetCursorScreenPos();

					ImGui::SetCursorScreenPos(ImVec2{ window->ContentRegionRect.Max.x - totalBtnWidth, currentCursorPos.y });

					if (fnDrawSocialButton("sarahtwitter", SocialsIcons::ID_Twitter, ImVec2{ socialIconSize, socialIconSize })) {
						ShellExecute(0, 0, "https://twitter.com/SSSiyan", 0, 0, SW_SHOW);
					}

					ImGui::SameLine(0.0f, 0.0f);

					if (ImGui::Button("KO-FI", ImVec2{ kofiBtnWidth, socialIconSize + 4.0f })) {
						ShellExecute(0, 0, "https://ko-fi.com/sssiyan", 0, 0, SW_SHOW);
					}

					ImGui::PopFont();
				}

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + scaledFontSize * 0.8f);

				// Deep
				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 0.9f)]);
					{
						ImGui::Text("逆向工程、工具开发、通用程序员");
					}
					ImGui::PopFont();

					ImGui::Separator();

					ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
					{
						ImGui::Text("deepdarkkapustka");
					}
					ImGui::PopFont();

					ImGui::SameLine();

					const ImVec2 socialsBBFrameSize{ 4.0f * 2.0f + 2.0f * ImGui::GetFontSize(), 4.0f + ImGui::GetFontSize() };
					const ImVec2 currentCursorPos = ImGui::GetCursorScreenPos();

					ImGui::SetCursorScreenPos(ImVec2{ window->ContentRegionRect.Max.x - socialsBBFrameSize.x, currentCursorPos.y });

					if (fnDrawSocialButton("deepgit", SocialsIcons::ID_Github, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
						ShellExecute(0, 0, "https://github.com/muhopensores", 0, 0, SW_SHOW);
					}

					ImGui::SameLine(0.0f, 0.0f);

					if (fnDrawSocialButton("deepyt", SocialsIcons::ID_YouTube, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
						ShellExecute(0, 0, "https://www.youtube.com/@mstislavcapusta7573", 0, 0, SW_SHOW);
					}
				}

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + scaledFontSize * 0.8f);

				// My smooth shiny bald ass
				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 0.9f)]);
					{
						ImGui::Text("后端、DevOps、通用程序员、逆向工程");
					}
					ImGui::PopFont();

					ImGui::Separator();

					ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
					{
						ImGui::Text("Darkness");
					}
					ImGui::PopFont();

					ImGui::SameLine();

					const ImVec2 socialsBBFrameSize{ 4.0f * 2.0f + 2.0f * ImGui::GetFontSize(), 4.0f + ImGui::GetFontSize() };
					const ImVec2 currentCursorPos = ImGui::GetCursorScreenPos();

					ImGui::SetCursorScreenPos(ImVec2{ window->ContentRegionRect.Max.x - socialsBBFrameSize.x, currentCursorPos.y });

					if (fnDrawSocialButton("darknesstwitter", SocialsIcons::ID_Twitter, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
						ShellExecute(0, 0, "https://twitter.com/Darknes30239448", 0, 0, SW_SHOW);
					}

					ImGui::SameLine(0.0f, 0.0f);

					if (fnDrawSocialButton("darknessgithub", SocialsIcons::ID_Github, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
						ShellExecute(0, 0, "https://github.com/amir-120", 0, 0, SW_SHOW);
					}
				}

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + scaledFontSize * 0.8f);

				// Charlie
				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 0.9f)]);
					{
						ImGui::Text("社区经理、测试、QA、游戏设计");
					}
					ImGui::PopFont();

					ImGui::Separator();

					ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
					{
						ImGui::Text("Charlie");
					}
					ImGui::PopFont();

					ImGui::SameLine();

					const ImVec2 socialsBBFrameSize{ 4.0f * 2.0f + 2.0f * ImGui::GetFontSize(), 4.0f + ImGui::GetFontSize() };
					const ImVec2 currentCursorPos = ImGui::GetCursorScreenPos();

					ImGui::SetCursorScreenPos(ImVec2{ window->ContentRegionRect.Max.x - socialsBBFrameSize.x, currentCursorPos.y });

					if (fnDrawSocialButton("charlieyoutube", SocialsIcons::ID_YouTube, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
						ShellExecute(0, 0, "https://www.youtube.com/@DMC_Charlie", 0, 0, SW_SHOW);
					}

					ImGui::SameLine(0.0f, 0.0f);

					if (fnDrawSocialButtonNonUV("charlietwitch", socialsTwitch, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
						ShellExecute(0, 0, "https://www.twitch.tv/charlie_decker", 0, 0, SW_SHOW);
					}
				}

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + scaledFontSize * 0.8f);


				// Hitchhiker
				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 0.9f)]);
					{
						ImGui::Text("通用程序员、逆向工程、游戏设计");
					}
					ImGui::PopFont();

					ImGui::Separator();

					ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
					{
						ImGui::Text("The Hitchhiker");
					}
					ImGui::PopFont();

					ImGui::SameLine();

					const ImVec2 socialsBBFrameSize{ 4.0f * 2.0f + 3.0f * ImGui::GetFontSize(), 4.0f + ImGui::GetFontSize() };
					const ImVec2 currentCursorPos = ImGui::GetCursorScreenPos();

					ImGui::SetCursorScreenPos(ImVec2{ window->ContentRegionRect.Max.x - socialsBBFrameSize.x, currentCursorPos.y });


					if (fnDrawSocialButton("hitchyoutube", SocialsIcons::ID_YouTube, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
						ShellExecute(0, 0, "https://www.youtube.com/@TheHitchhiker", 0, 0, SW_SHOW);
					}

					ImGui::SameLine(0.0f, 0.0f);

					if (fnDrawSocialButtonNonUV("hitchtwitch", socialsTwitch, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
						ShellExecute(0, 0, "https://www.twitch.tv/hitchhikingthrough", 0, 0, SW_SHOW);
					}

					ImGui::SameLine(0.0f, 0.0f);

					if (fnDrawSocialButtonNonUV("hitchbluesky", socialsBluesky, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
						ShellExecute(0, 0, "https://bsky.app/profile/the-hitchhiker.bsky.social", 0, 0, SW_SHOW);
					}
				
				}

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + scaledFontSize * 0.8f);

				// RaccMoon
				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 0.9f)]);
					{
						ImGui::Text("测试、QA");
					}
					ImGui::PopFont();

					ImGui::Separator();

					ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
					{
						ImGui::Text("RaccMoon");
					}
					ImGui::PopFont();

					ImGui::SameLine();

					const ImVec2 socialsBBFrameSize{ 4.0f + ImGui::GetFontSize(), 4.0f + ImGui::GetFontSize() };
					const ImVec2 currentCursorPos = ImGui::GetCursorScreenPos();

					ImGui::SetCursorScreenPos(ImVec2{ window->ContentRegionRect.Max.x - socialsBBFrameSize.x, currentCursorPos.y });

					if (fnDrawSocialButton("raccyoutube", SocialsIcons::ID_YouTube, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
						ShellExecute(0, 0, "https://www.youtube.com/@rakunuki3565", 0, 0, SW_SHOW);
					}
				}

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + scaledFontSize * 0.8f);

				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 0.9f)]);
					{
						ImGui::Text("首席2D美术");
					}
					ImGui::PopFont();

					ImGui::Separator();

					ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
					{
						ImGui::Text("Lenam Lorbington");
					}
					ImGui::PopFont();

				}

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + scaledFontSize * 0.8f);

				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 0.9f)]);
					{
						ImGui::Text("3D & 2D Artist, Animation");
					}
					ImGui::PopFont();

					ImGui::Separator();

					ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
					{
						ImGui::Text("Airdragon");

						ImGui::SameLine();

						const ImVec2 socialsBBFrameSize{ 4.0f + ImGui::GetFontSize(), 4.0f + ImGui::GetFontSize() };
						const ImVec2 currentCursorPos = ImGui::GetCursorScreenPos();

						ImGui::SetCursorScreenPos(ImVec2{ window->ContentRegionRect.Max.x - socialsBBFrameSize.x, currentCursorPos.y });

						if (fnDrawSocialButton("airdragongithub", SocialsIcons::ID_Github, ImVec2{ ImGui::GetFontSize() + 2, ImGui::GetFontSize() })) {
							ShellExecute(0, 0, "https://github.com/Airdragon50", 0, 0, SW_SHOW);
						}

						ImGui::SameLine();

						if (fnDrawSocialButton("airdragonnexus", SocialsIcons::ID_Nexusmods, ImVec2{ ImGui::GetFontSize() + 2, ImGui::GetFontSize() })) {
							ShellExecute(0, 0, "https://www.nexusmods.com/profile/Airdragon50/mods", 0, 0, SW_SHOW);
						}
					}
					ImGui::PopFont();
				}

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + scaledFontSize * 0.8f);

				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 0.9f)]);
					{
						ImGui::Text("首席特效美术");
					}
					ImGui::PopFont();

					ImGui::Separator();

					ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
					{
						ImGui::Text("ZippoIG");

						ImGui::SameLine();

						const ImVec2 socialsBBFrameSize{ 4.0f + ImGui::GetFontSize(), 4.0f + ImGui::GetFontSize() };
						const ImVec2 currentCursorPos = ImGui::GetCursorScreenPos();

						ImGui::SetCursorScreenPos(ImVec2{ window->ContentRegionRect.Max.x - socialsBBFrameSize.x, currentCursorPos.y });

						if (fnDrawSocialButton("zippoigtwitter", SocialsIcons::ID_Twitter, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
							ShellExecute(0, 0, "https://x.com/ZippoIG", 0, 0, SW_SHOW);
						}
					}
					ImGui::PopFont();
				}

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + scaledFontSize * 0.8f);

				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 0.9f)]);
					{
						ImGui::Text("3D & 2D Artist, Animation");
					}
					ImGui::PopFont();

					ImGui::Separator();

					ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
					{
						ImGui::Text("Vainiuss1");

						ImGui::SameLine();

						const ImVec2 socialsBBFrameSize{ 4.0f + ImGui::GetFontSize(), 4.0f + ImGui::GetFontSize() };
						const ImVec2 currentCursorPos = ImGui::GetCursorScreenPos();

						ImGui::SetCursorScreenPos(ImVec2{ window->ContentRegionRect.Max.x - socialsBBFrameSize.x, currentCursorPos.y });

						if (fnDrawSocialButton("vainiusstwitter", SocialsIcons::ID_Twitter, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
							ShellExecute(0, 0, "https://x.com/vainiuss1", 0, 0, SW_SHOW);
						}

						ImGui::SameLine();

						if (fnDrawSocialButton("vainiussnexus", SocialsIcons::ID_Nexusmods, ImVec2{ ImGui::GetFontSize() + 2, ImGui::GetFontSize() })) {
							ShellExecute(0, 0, "https://www.nexusmods.com/profile/vainiuss1/mods", 0, 0, SW_SHOW);
						}
					}
					ImGui::PopFont();
				}

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + scaledFontSize * 0.9f);

				// Additional Work
				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 1.2f)]);
					{
						ImGui::Text("额外参与者");
					}
					ImGui::PopFont();

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + scaledFontSize * 0.8f);

					{
						ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 0.9f)]);
						{
							ImGui::Text("视频制作、推广、顾问");
						}
						ImGui::PopFont();

						ImGui::Separator();

						ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
						{
							ImGui::Text("Dany Sterkhov");

							ImGui::SameLine();

							const ImVec2 socialsBBFrameSize{ 4.0f + ImGui::GetFontSize(), 4.0f + ImGui::GetFontSize() };
							const ImVec2 currentCursorPos = ImGui::GetCursorScreenPos();

							ImGui::SetCursorScreenPos(ImVec2{ window->ContentRegionRect.Max.x - socialsBBFrameSize.x, currentCursorPos.y });

							if (fnDrawSocialButton("danycuhrayzeeyoutube", SocialsIcons::ID_YouTube, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
								ShellExecute(0, 0, "https://www.youtube.com/@Cuhrayzee", 0, 0, SW_SHOW);
							}

							ImGui::SameLine();

							if (fnDrawSocialButton("danycuhrayzeetwitter", SocialsIcons::ID_Twitter, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
								ShellExecute(0, 0, "https://x.com/cuhrayzeee", 0, 0, SW_SHOW);
							}

						}
						ImGui::PopFont();
					}

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + scaledFontSize * 0.8f);

					{
						ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 0.9f)]);
						{
							ImGui::Text("动画与贴图、Mod工具、顾问");
						}
						ImGui::PopFont();

						ImGui::Separator();

						ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
						{
							ImGui::Text("Che");
						}
						ImGui::PopFont();
					}

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + scaledFontSize * 0.8f);

					{
						ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 0.9f)]);
						{
							ImGui::Text("美术");
						}
						ImGui::PopFont();

						ImGui::Separator();

						ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
						{
							ImGui::Text("Garlic Goblin");

							ImGui::SameLine();

							const ImVec2 socialsBBFrameSize{ 4.0f + ImGui::GetFontSize(), 4.0f + ImGui::GetFontSize() };
							const ImVec2 currentCursorPos = ImGui::GetCursorScreenPos();

							ImGui::SetCursorScreenPos(ImVec2{ window->ContentRegionRect.Max.x - socialsBBFrameSize.x, currentCursorPos.y });

							if (fnDrawSocialButton("garlicgoblinlinktree", socialsBluesky, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
								ShellExecute(0, 0, "https://linktr.ee/thegarlicgoblin", 0, 0, SW_SHOW);
							}
						}
						ImGui::PopFont();
					}

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + scaledFontSize * 0.8f);

					{
						ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 0.9f)]);
						{
							ImGui::Text("美术");
						}
						ImGui::PopFont();

						ImGui::Separator();

						ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
						{
							ImGui::Text("Cynuma");

							ImGui::SameLine();

							const ImVec2 socialsBBFrameSize{ 4.0f + ImGui::GetFontSize(), 4.0f + ImGui::GetFontSize() };
							const ImVec2 currentCursorPos = ImGui::GetCursorScreenPos();

							ImGui::SetCursorScreenPos(ImVec2{ window->ContentRegionRect.Max.x - socialsBBFrameSize.x, currentCursorPos.y });

							if (fnDrawSocialButton("cynumatwitter", SocialsIcons::ID_Twitter, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
								ShellExecute(0, 0, "https://x.com/Cynumaa", 0, 0, SW_SHOW);
							}
						}
						ImGui::PopFont();
					}

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + scaledFontSize * 0.8f);

					{
						ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 0.9f)]);
						{
							ImGui::Text("3D Artist");
						}
						ImGui::PopFont();

						ImGui::Separator();

						ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
						{
							ImGui::Text("Omar Nabelse");

						}
						ImGui::PopFont();
					}

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + scaledFontSize * 0.8f);

					{
						ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 0.9f)]);
						{
							ImGui::Text("程序员");
						}
						ImGui::PopFont();

						ImGui::Separator();

						ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
						{
							ImGui::Text("adil");

							ImGui::SameLine();

							const ImVec2 socialsBBFrameSize{ 4.0f + ImGui::GetFontSize(), 4.0f + ImGui::GetFontSize() };
							const ImVec2 currentCursorPos = ImGui::GetCursorScreenPos();

							ImGui::SetCursorScreenPos(ImVec2{ window->ContentRegionRect.Max.x - socialsBBFrameSize.x, currentCursorPos.y });

							if (fnDrawSocialButton("adilgithub", SocialsIcons::ID_Github, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
								ShellExecute(0, 0, "https://github.com/adilahmeddev", 0, 0, SW_SHOW);
							}
						}
						ImGui::PopFont();
					}

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + scaledFontSize * 0.8f);

					{
						ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 0.9f)]);
						{
							ImGui::Text("音效制作");
						}
						ImGui::PopFont();

						ImGui::Separator();

						ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
						{
							ImGui::Text("Tihan");

							ImGui::SameLine();

							const ImVec2 socialsBBFrameSize{ 4.0f + ImGui::GetFontSize(), 4.0f + ImGui::GetFontSize() };
							const ImVec2 currentCursorPos = ImGui::GetCursorScreenPos();

							ImGui::SetCursorScreenPos(ImVec2{ window->ContentRegionRect.Max.x - socialsBBFrameSize.x, currentCursorPos.y });

							if (fnDrawSocialButton("tihantwitter", SocialsIcons::ID_Twitter, ImVec2{ ImGui::GetFontSize() + 2, ImGui::GetFontSize() })) {
								ShellExecute(0, 0, "https://x.com/t1h4n", 0, 0, SW_SHOW);
							}
						}
						ImGui::PopFont();
					}

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + scaledFontSize * 0.8f);

					{
						ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 0.9f)]);
						{
							ImGui::Text("程序员");
						}
						ImGui::PopFont();

						ImGui::Separator();

						ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
						{
							ImGui::Text("mos9527");

							ImGui::SameLine();

							const ImVec2 socialsBBFrameSize{ 4.0f + ImGui::GetFontSize(), 4.0f + ImGui::GetFontSize() };
							const ImVec2 currentCursorPos = ImGui::GetCursorScreenPos();

							ImGui::SetCursorScreenPos(ImVec2{ window->ContentRegionRect.Max.x - socialsBBFrameSize.x, currentCursorPos.y });

							if (fnDrawSocialButton("mos9527github", SocialsIcons::ID_Github, ImVec2{ ImGui::GetFontSize() + 2, ImGui::GetFontSize() })) {
								ShellExecute(0, 0, "https://github.com/mos9527", 0, 0, SW_SHOW);
							}
						}
						ImGui::PopFont();
					}

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + scaledFontSize * 0.9f);

					// PlayTesters
					{
						ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 1.2f)]);
						{
							ImGui::Text("PlayTesters");
						}
						ImGui::PopFont();

						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + scaledFontSize * 0.8f);

						ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
						{
							ImGui::Separator();

							ImGui::Text("Akaragitsune");
						}
						ImGui::PopFont();

						ImGui::SameLine();

						const ImVec2 socialsBBFrameSize{ 4.0f * 2.0f + 2.0f * ImGui::GetFontSize(), 4.0f + ImGui::GetFontSize() };
						const ImVec2 currentCursorPos = ImGui::GetCursorScreenPos();

						ImGui::SetCursorScreenPos(ImVec2{ window->ContentRegionRect.Max.x - socialsBBFrameSize.x, currentCursorPos.y });

						if (fnDrawSocialButton("akaragitsunetwitter", SocialsIcons::ID_Twitter, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
							ShellExecute(0, 0, "https://x.com/Akaragitsune", 0, 0, SW_SHOW);
						}

						ImGui::SameLine(0.0f, 0.0f);

						if (fnDrawSocialButton("akaragitsuneyoutube", SocialsIcons::ID_YouTube, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
							ShellExecute(0, 0, "https://www.youtube.com/@akaragitsune", 0, 0, SW_SHOW);
						}

						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + scaledFontSize * 0.8f);

						ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
						{
							ImGui::Separator();

							ImGui::Text("Danliam");

							ImGui::SameLine();

							const ImVec2 socialsBBFrameSize{ 4.0f + ImGui::GetFontSize(), 4.0f + ImGui::GetFontSize() };
							const ImVec2 currentCursorPos = ImGui::GetCursorScreenPos();

							ImGui::SetCursorScreenPos(ImVec2{ window->ContentRegionRect.Max.x - socialsBBFrameSize.x, currentCursorPos.y });

							if (fnDrawSocialButton("danliamyoutube", SocialsIcons::ID_YouTube, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
								ShellExecute(0, 0, "https://www.youtube.com/@danliamdmc", 0, 0, SW_SHOW);
							}
						}
						ImGui::PopFont();

						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + scaledFontSize * 0.8f);

						ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
						{
							ImGui::Separator();

							ImGui::Text("Nelo47Angelo");

							ImGui::SameLine();

							const ImVec2 socialsBBFrameSize{ 4.0f + ImGui::GetFontSize(), 4.0f + ImGui::GetFontSize() };
							const ImVec2 currentCursorPos = ImGui::GetCursorScreenPos();

							ImGui::SetCursorScreenPos(ImVec2{ window->ContentRegionRect.Max.x - socialsBBFrameSize.x, currentCursorPos.y });

							if (fnDrawSocialButton("nelo47angeloyoutube", SocialsIcons::ID_YouTube, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
								ShellExecute(0, 0, "https://www.youtube.com/channel/UCiwtZmUDGGuEP0Ze2KsBSSQ", 0, 0, SW_SHOW);
							}
						}
						ImGui::PopFont();
					}

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + scaledFontSize * 0.8f);

					{
						ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 0.9f)]);
						{
							ImGui::Text("Thanks to Spritzkrieg for the express permission\n"
								"to airdragon50 to use and modify his Original PS2 \n"
								"贴图包、字幕对齐与语法修正。");
						}
						ImGui::PopFont();
					}
				}


				ImGui::Text("");

				// Serp
				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 1.2f)]);
					{
						ImGui::Text("原DDMK开发者");
					}
					ImGui::PopFont();

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + scaledFontSize * 0.2f);

					ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
					{
						ImGui::Separator();

						ImGui::Text("serpentiem");

						ImGui::SameLine();

						const ImVec2 socialsBBFrameSize{ 4.0f + ImGui::GetFontSize(), 4.0f + ImGui::GetFontSize() };
						const ImVec2 currentCursorPos = ImGui::GetCursorScreenPos();

						ImGui::SetCursorScreenPos(ImVec2{ window->ContentRegionRect.Max.x - socialsBBFrameSize.x, currentCursorPos.y });

						if (fnDrawSocialButton("serpgithub", SocialsIcons::ID_Github, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
							ShellExecute(0, 0, "https://github.com/serpentiem", 0, 0, SW_SHOW);
						}
					}
					ImGui::PopFont();
				}

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + scaledFontSize * 0.8f);

				// Lyall
				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 1.2f)]);
					{
						ImGui::Text("DMCHDFix 开发者");
					}
					ImGui::PopFont();

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + scaledFontSize * 0.2f);

					ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
					{
						ImGui::Separator();

						ImGui::Text("Lyall");

						ImGui::SameLine();

						const ImVec2 socialsBBFrameSize{ 4.0f + ImGui::GetFontSize(), 4.0f + ImGui::GetFontSize() };
						const ImVec2 currentCursorPos = ImGui::GetCursorScreenPos();

						ImGui::SetCursorScreenPos(ImVec2{ window->ContentRegionRect.Max.x - socialsBBFrameSize.x, currentCursorPos.y });

						if (fnDrawSocialButton("lyallgithub", SocialsIcons::ID_Github, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
							ShellExecute(0, 0, "https://github.com/serpentiem", 0, 0, SW_SHOW);
						}
					}
					ImGui::PopFont();
				}
			}
			ImGui::EndChild();
		}

		// Patrons area
		{
			const ImVec2 areaSize = cntWindow->Size * ImVec2{ areaPaddingXRation, 0.8f };
			const ImVec2 areaMin{ cntWindow->Pos.x + cntWindow->Size.x * areaPaddingXRation + areaPaddingX * 2.0f,
									 cntWindow->Pos.y + scaledFontSize * 0.1f };

			//cntWindow->DrawList->AddRect(areaMin, areaMin + areaSize, UI::SwapColorEndianness(0x00FF00FF));

			ImVec2 padding{ scaledFontSize * 0.8f, scaledFontSize * 0.8f };

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { scaledFontSize * 0.4f, scaledFontSize * 0.4f });
			ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
			   // Auto-scroll logic for Patrons Area
			   static float patronsScroll = 0.0f;
			   static double patronsLastTime = ImGui::GetTime();
			   static float patronsPauseTimer = 0.0f;
			   static float patronsLingerStart = 1.5f; // seconds to linger at start
			   static float patronsLingerEnd = 1.5f;   // seconds to linger at end
			   static bool patronsAtStart = true;
			   static bool patronsAtEnd = false;

			   double patronsCurrentTime = ImGui::GetTime();
			   float patronsDelta = float(patronsCurrentTime - patronsLastTime);
			   patronsLastTime = patronsCurrentTime;
			   float patronsScrollSpeed = 25.0f;

			   ImGui::BeginChildEx("赞助者区域", cntWindow->GetID("赞助者区域"), areaSize, false,
				   ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_AlwaysVerticalScrollbar);

			   float patronsMaxScroll = ImGui::GetScrollMaxY();
			   float patronsUserScroll = ImGui::GetScrollY();

			   if (fabsf(patronsUserScroll - patronsScroll) > 1.0f) {
				   // User scrolled, pause auto-scroll for 4 seconds
				   patronsPauseTimer = 2.0f;
				   patronsScroll = patronsUserScroll;
			   }

			   if (patronsPauseTimer > 0.0f) {
				   patronsPauseTimer -= patronsDelta;
			   } else {
				   if (patronsScroll <= 0.0f) {
					   if (!patronsAtStart) { patronsPauseTimer = patronsLingerStart; patronsAtStart = true; }
				   } else if (patronsScroll >= patronsMaxScroll) {
					   if (!patronsAtEnd) { patronsPauseTimer = patronsLingerEnd; patronsAtEnd = true; }
				   } else {
					   patronsAtStart = false;
					   patronsAtEnd = false;
				   }
				   if (patronsPauseTimer <= 0.0f && !patronsAtEnd) {
					   patronsScroll += patronsScrollSpeed * patronsDelta;
					   if (patronsScroll > patronsMaxScroll) {
						   patronsScroll = patronsMaxScroll;
					   }
				   }
				   if (patronsAtEnd && patronsPauseTimer <= 0.0f) {
					   patronsScroll = 0.0f;
					   patronsAtStart = true;
					   patronsAtEnd = false;
					   patronsPauseTimer = patronsLingerStart;
				   }
			   }

			   ImGui::SetScrollY(patronsScroll);
			   ImGui::PopStyleVar();
			   {
				ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 1.4f)]);
				{
					ImGui::Text("Patreon 支持者");
					ImGui::Separator();
				}
				ImGui::PopFont();

				if (UI::patronsQueueResult == WebAPIResult::Success || UI::patronsQueueResult == WebAPIResult::Awaiting)
				{
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + scaledFontSize * 0.5f);

					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[context.DefaultFontSize]);
					{
						ImGui::Text(UI::g_UIContext.TierNames[(size_t)PatreonTiers_t::LDK].c_str());
					}
					ImGui::PopFont();

					ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
					{
						for (const auto &richAF : context.PatronsLDK)
						{
							ImGui::Text(richAF.c_str());
						}
					}
					ImGui::PopFont();

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + scaledFontSize * 0.5f);

					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[context.DefaultFontSize]);
					{
						ImGui::Text(UI::g_UIContext.TierNames[(size_t)PatreonTiers_t::SDT].c_str());
					}
					ImGui::PopFont();

					ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
					{
						for (const auto &rich : context.PatronsSDT)
						{
							ImGui::Text(rich.c_str());
						}
					}
					ImGui::PopFont();

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + scaledFontSize * 0.5f);

					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[context.DefaultFontSize]);
					{
						ImGui::Text(UI::g_UIContext.TierNames[(size_t)PatreonTiers_t::DT].c_str());
					}
					ImGui::PopFont();

					ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
					{
						for (const auto &rich : context.PatronsDT)
						{
							ImGui::Text(rich.c_str());
						}
					}
					ImGui::PopFont();
				}
			}
			ImGui::EndChild();
		}

		// Special Thanks area
		{
			const ImVec2 areaSize = cntWindow->Size * ImVec2{ areaPaddingXRation, 0.8f };
			const ImVec2 areaMin{ cntWindow->Pos.x + cntWindow->Size.x * areaPaddingXRation * 2.0f + areaPaddingX * 3.0f,
									 cntWindow->Pos.y + scaledFontSize * 0.1f };

			//cntWindow->DrawList->AddRect(areaMin, areaMin + areaSize, UI::SwapColorEndianness(0x00FF00FF));

			ImVec2 padding{ context.DefaultFontSize * 0.8f, scaledFontSize * 0.8f };

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { scaledFontSize * 0.4f, scaledFontSize * 0.4f });
			ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
			ImGui::BeginChildEx("特别鸣谢区域", cntWindow->GetID("特别鸣谢区域"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
			ImGui::PopStyleVar();
			{
				ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 1.4f)]);
				{
					ImGui::Text("特别鸣谢");
					ImGui::Separator();
				}
				ImGui::PopFont();

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + scaledFontSize * 0.5f);

				ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
				{
					for (const auto& name : context.SpecialThanksNames) {
						ImGui::Text(name);
					}
				}
				ImGui::PopFont();
			}
			ImGui::EndChild();
		}

		// Footer area
		{
			const ImVec2 areaSize{ cntWindow->Size.x - areaPaddingX * 2.0f, scaledFontSize * 4.4f };
			const ImVec2 areaMin{ cntWindow->Pos.x + areaPaddingX,
									 cntWindow->Pos.y + cntWindow->Size.y - cntWindow->Size.y * 0.18f };

			//cntWindow->DrawList->AddRect(areaMin, areaMin + areaSize, UI::SwapColorEndianness(0x00FF00FF));

			ImVec2 padding{ context.DefaultFontSize * 0.8f, scaledFontSize * 0.8f };

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { scaledFontSize * 0.4f, scaledFontSize * 0.4f });
			ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
			ImGui::BeginChildEx("页脚区域", cntWindow->GetID("页脚区域"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
			ImGui::PopStyleVar();
			{
				auto window = ImGui::GetCurrentWindow();
				constexpr const char* THX_TEXT = "感谢大家游玩并支持本项目！";
				constexpr const char* LICENSE_BUTTON_TEXT = "许可证";
				constexpr const char* COPYRIGHT_TEXT = "Copyright (c) 2026 Berthrage";
				static bool showLicense = false;

				{
					const float thxTextWidth = ImGui::CalcTextSize(THX_TEXT).x;
					window->DrawList->AddText(ImVec2{ areaMin.x + (areaSize.x - thxTextWidth) * 0.5f, areaMin.y + scaledFontSize * 0.6f }, 0xFFFFFFFF, THX_TEXT);
				}

				ImGui::PushFont(UI::g_ImGuiFont_RussoOne[context.DefaultFontSize]);
				{
					const float licenseButtonWidth = ImGui::CalcTextSize(LICENSE_BUTTON_TEXT).x + style.FramePadding.x * 2.0f;

					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
					ImGui::SetCursorScreenPos(ImVec2{ areaMin.x + (areaSize.x - licenseButtonWidth) * 0.5f, areaMin.y + scaledFontSize * 2.0f });
					if (ImGui::Button(LICENSE_BUTTON_TEXT)) {
						showLicense = !showLicense;
					}
					ImGui::PopStyleVar();
				}
				ImGui::PopFont();

				// License popup window
				if (showLicense) {
					const float licenseScaleFactor = (scaleFactorY + (ImGui::GetIO().DisplaySize.x / 1920.0f)) * 0.5f;
					const float licW = 650.0f * licenseScaleFactor;
					const float licH = 500.0f * licenseScaleFactor;
					const float licCenterX = (g_renderSize.x - licW) * 0.5f;
					const float licCenterY = (g_renderSize.y - licH) * 0.5f;

					ImGui::SetNextWindowSize(ImVec2(licW, licH));
					ImGui::SetNextWindowPos(ImVec2(licCenterX, licCenterY));

					ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f * licenseScaleFactor, 20.0f * licenseScaleFactor));
					ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 20.0f * licenseScaleFactor);
					ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
					ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 0));

					ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.85f));

					if (ImGui::Begin("LicenseWindow", &showLicense, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize)) {
						ImGui::SetWindowFontScale(licenseScaleFactor);

						ImGui::PushFont(UI::g_ImGuiFont_RussoOne[context.DefaultFontSize * 1.2f]);
						ImGui::Text("许可证");
						ImGui::SameLine();

						const float xBtnSize = 24.0f * licenseScaleFactor;
						ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x - xBtnSize);
						if (ImGui::Button("X", ImVec2(xBtnSize, xBtnSize))) {
							showLicense = false;
						}
						ImGui::PopFont();

						ImGui::Separator();

						ImGui::PushFont(UI::g_ImGuiFont_Roboto[context.DefaultFontSize * 0.85f]);

						ImVec2 childSize = ImGui::GetContentRegionAvail();
						ImGui::BeginChild("LicenseTextArea", childSize, false, ImGuiWindowFlags_AlwaysVerticalScrollbar);
						ImGui::SetWindowFontScale(licenseScaleFactor);

						ImGui::PushTextWrapPos(0.0f);
						ImGui::TextUnformatted(g_LicenseText);
						ImGui::PopTextWrapPos();

						ImGui::EndChild();
						ImGui::PopFont();
					}
					ImGui::End();

					ImGui::PopStyleColor();
					ImGui::PopStyleVar(4);
				}

				ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 0.8f)]);
				{
					const float crTextWidth = ImGui::CalcTextSize(COPYRIGHT_TEXT).x;
					window->DrawList->AddText(ImVec2{ areaMin.x + (areaSize.x - crTextWidth) * 0.5f, areaMin.y + scaledFontSize * 3.5f }, 0xFFFFFFFF, COPYRIGHT_TEXT);
				}
				ImGui::PopFont();
			}
			ImGui::EndChild();
		}
	}
	break;

	default:
		context.SelectedTab = UI::UIContext::MainTabs::None;
		break;
	}
}

#pragma endregion


void ExperimentalInputUpdate() {
	static bool hasEnteredMainScene = false;
	auto pool_C90E10 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
	if (!pool_C90E10 || !pool_C90E10[5]) {
		return;
	}
	auto& eventData = *reinterpret_cast<CSceneGameMain*>(pool_C90E10[5]);
	if (eventData.event == EVENT::MAIN) {
		hasEnteredMainScene = true;
	}
	if (hasEnteredMainScene) {
		for (uint8 playerIndex = 0; playerIndex < 4; playerIndex++) {
			CrimsonReversedCalls::InputsUpdate_sub_14032CFE0(0x140D54A10, playerIndex, 0);
		}
	}
}

void GUI_Render(IDXGISwapChain* pSwapChain) {
    if (g_scene != SCENE::GAME) {
        devilTriggerReadyPlayed = !activeConfig.playDTReadySFXAtMissionStart;
    }
	g_inMainMenu = *reinterpret_cast<bool*>(appBaseAddr + 0x5D9213);

	UI::ResetID(0);

	PreventEmptyCrimsonGUIHotkey();
    AdjustBackgroundColorAndTransparency();

    Welcome();
	CrimsonOnboarding::Run(pSwapChain);
	StoreHDCKeybinds();
	OverrideHDCKeybinds();
    Main(pSwapChain);
    Shop::ShopWindow();
	ShowButtonConfigWindow();
	RenderMainMenuInfo(pSwapChain);

	ActorWindow();
	EventDataWindow();
	ExpWindow();
	FileDataWindow();
	MissionDataWindow();
	RegionDataWindow();
	SoundWindow();
	
	scaleFactorX = ImGui::GetIO().DisplaySize.x / 1920;
	scaleFactorY = ImGui::GetIO().DisplaySize.y / 1080;
	scaleFactor = (scaleFactorX + scaleFactorY) / 2.0f;
	scaledFontSize = UI::g_UIContext.DefaultFontSize * scaleFactorY;
	itemWidth = scaledFontSize * 8.0f;

    PauseWhenGUIOpened();
    GamepadToggleShowMain();
	if (activeConfig.debugOverlayData.enable) {
		DebugOverlayWindow(UI::g_UIContext.DefaultFontSize);
	}
    MissionOverlayWindow(UI::g_UIContext.DefaultFontSize);
    BossLadyActionsOverlayWindow();
    BossVergilActionsOverlayWindow();
    
	// Calling this from GUI Render is the safest way to ensure this will run on-tick properly
	// outside of In Game.32CFE0
	//ExperimentalInputUpdate();

	CrimsonOnTick::FrameResponsiveGameSpeed();
	CrimsonOnTick::LevelFullyLoadedDelay();
	CrimsonBetterArkham2::OnTick();
    // TIMERS
    CrimsonTimers::CallAllTimers();

    MultiplayerBars(pSwapChain);
	DummyWeaponWheelController(pSwapChain);
	WeaponWheels1PController(pSwapChain);
	WeaponWheelsMultiplayerController(pSwapChain);
	WorldSpaceWeaponWheels1PController(pSwapChain);
	WorldSpaceWeaponWheelsController(pSwapChain);
	CrimsonHUD::RedOrbCounterWindow();
	CrimsonBetterArkham2::BlackoutArkham2OriginalScene();
	CrimsonHUD::CheatsHUDIndicatorWindow();
	CrimsonHUD::CheatHotkeysPopUpWindow();
	CrimsonHUD::StyleMeterWindows();
	CrimsonHUD::StylishPointsWindow();
	CrimsonHUD::MissionTimerDisplay();
	CrimsonHUD::LockOnWindows();
	CrimsonHUD::StunDisplacementLockOnWindows();
	CrimsonHUD::ShieldLockOnWindows();
	CrimsonHUD::StyleDisplayWindow();
	CrimsonHUD::StyleTextDisplayWindow();
	CrimsonHUD::StyleLvlDispWindow();
	CrimsonHUD::StyleEXPDisplayWindow();
	CrimsonHUD::RoyalGaugeDispWindow();
	CrimsonHUD::MirageGaugeMainPlayer();

	UI::g_UIContext.SelectedGameMode = (UI::UIContext::GameModes)activeCrimsonGameplay.GameMode.preset;
	RenderMissionResultGameModeStats();
	RenderMissionResultVersionDisplay();
	RenderMissionResultCheatsUsed();
	//CrimsonGameModes::TrackGameMode();
	//CrimsonGameModes::TrackCheats();
	//CrimsonGameModes::TrackMissionResultGameMode();
	//CrimsonOnTick::CrimsonMissionClearSong();
	//CrimsonOnTick::DivinityStatueSong();
	//CrimsonSDL::ReduceMusicVolumeInPause();

	//CrimsonSDL::CheckAndOpenControllers();
	//CrimsonSDL::UpdateJoysticks();


    HandleKeyBindings(keyBindings.data(), keyBindings.size());


    if (g_shopTimer > 0) {
        g_shopTimer -= 1.0f;
    }

    HandleSaveTimer(g_FrameRate);
    ImGui::PopStyleColor();
    // static bool enable = true;
    // ImGui::ShowDemoWindow(&enable);
}

void GUI_Init() {
    LogFunction();

    //BuildFonts();

    UpdateGlobalScale();

    UI::SetStyleCrimson();
	UI::BuildFontsCrimson();

    Actor_UpdateIndices();
    Arcade_UpdateIndices();
}
