#pragma once

#include <stdio.h>

#include <d3d9.h>
#include <d3dx9.h>
#include <dwmapi.h>
#include <dinput.h>


#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dwmapi.lib")

#include "drawing.h"
#include "main.h"

#define OVERLAY_TARGET "Rust" /* window name */
#define CROSSHAIR_THICKNESS 20
#define CROSSHAIR_WIDTH 2
#define WINDOW_ADJUST_RATE 100
#define DIRECTINPUT_VERSION 0x0800

namespace Overlay {
	void Init();
}