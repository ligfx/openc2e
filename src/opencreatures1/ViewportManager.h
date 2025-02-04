#pragma once

#include <stdint.h>

struct BackendEvent;

static constexpr int CREATURES1_WORLD_WIDTH = 8352;
static constexpr int CREATURES1_WORLD_HEIGHT = 1200;

class ViewportManager {
  public:
	ViewportManager() = default;
	void handle_event(const BackendEvent& event);
	void tick();

	float window_x_to_world_x(float) const;
	float window_y_to_world_y(float) const;

	void set_scroll_position(int32_t scrollx, int32_t scrolly);

	void set_margin_top(float);
	void set_margin_bottom(float);

	int32_t get_scrollx() const { return scrollx; }
	int32_t get_scrolly() const { return scrolly; }

  private:
	void update();

	static constexpr float SCROLL_ACCEL = 8;
	static constexpr float SCROLL_DECEL = 0.5;
	static constexpr float SCROLL_MAX = 64;

	bool scroll_left = false;
	bool scroll_right = false;
	bool scroll_up = false;
	bool scroll_down = false;

	int32_t scrollx = 0;
	int32_t scrolly = 0;

	float scroll_velx = 0;
	float scroll_vely = 0;

	float margin_top = 0;
	float margin_bottom = 0;
};
