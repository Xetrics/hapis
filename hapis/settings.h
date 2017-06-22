#pragma once

struct weather_settings {
	bool always_day = false;
	bool freeze_time = false;
	bool no_fog = false;
	bool no_rain = false;
	bool no_clouds = false;
	bool no_wind = false;
};

class Settings {
public:
	// Setting sub-categories
	weather_settings* weather;

	bool crosshair = false;
	bool esp = false;
	bool debug = false;
	bool radar = false;

	Settings() {
		weather = new weather_settings();
	}
};

extern Settings* settings;