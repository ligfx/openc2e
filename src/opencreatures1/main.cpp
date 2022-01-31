#include "C1MusicManager.h"
#include "ImageManager.h"
#include "PathManager.h"
#include "SDLBackend.h"
#include "common/Ascii.h"
#include "common/Repr.h"
#include "common/case_insensitive_filesystem.h"
#include "common/optional.h"
#include "fileformats/ImageUtils.h"
#include "fileformats/NewSfcFile.h"
#include "fileformats/paletteFile.h"
#include "openc2e-audiobackend/SDLMixerBackend.h"
#include "openc2e-core/creaturesImage.h"
#include "openc2e-core/keycodes.h"

#include <SDL.h>
#include <chrono>
#include <fmt/core.h>
#include <ghc/filesystem.hpp>

namespace fs = ghc::filesystem;

static constexpr int CREATURES1_WORLD_WIDTH = 8352;
static constexpr int CREATURES1_WORLD_HEIGHT = 1200;

class C1ScrollManager {
  public:
	C1ScrollManager(std::shared_ptr<Backend> backend)
		: m_backend(backend) {}
	void handle_event(const BackendEvent& event) {
		if (!(event.type == eventrawkeyup || event.type == eventrawkeydown)) {
			return;
		}
		if (event.key == OPENC2E_KEY_LEFT) {
			scroll_left = (event.type == eventrawkeydown);
		} else if (event.key == OPENC2E_KEY_RIGHT) {
			scroll_right = (event.type == eventrawkeydown);
		} else if (event.key == OPENC2E_KEY_UP) {
			scroll_up = (event.type == eventrawkeydown);
		} else if (event.key == OPENC2E_KEY_DOWN) {
			scroll_down = (event.type == eventrawkeydown);
		}
	}
	void tick() {
		// scroll left-right
		if (scroll_left) {
			scroll_velx -= SCROLL_ACCEL;
		}
		if (scroll_right) {
			scroll_velx += SCROLL_ACCEL;
		}
		if (!scroll_left && !scroll_right) {
			scroll_velx *= SCROLL_DECEL;
			if (-0.1 < scroll_velx && scroll_velx < 0.1) {
				scroll_velx = 0;
			}
		}
		// scroll up-down
		if (scroll_up) {
			scroll_vely -= SCROLL_ACCEL;
		}
		if (scroll_down) {
			scroll_vely += SCROLL_ACCEL;
		}
		if (!scroll_up && !scroll_down) {
			scroll_vely *= SCROLL_DECEL;
			if (-0.1 < scroll_vely && scroll_vely < 0.1) {
				scroll_vely = 0;
			}
		}
		// enforce scroll speed limits
		if (scroll_velx >= SCROLL_MAX) {
			scroll_velx = SCROLL_MAX;
		} else if (scroll_velx <= -SCROLL_MAX) {
			scroll_velx = -SCROLL_MAX;
		}
		if (scroll_vely >= SCROLL_MAX) {
			scroll_vely = SCROLL_MAX;
		} else if (scroll_vely <= -SCROLL_MAX) {
			scroll_vely = -SCROLL_MAX;
		}

		// do the actual movement
		if (scroll_velx || scroll_vely) {
			scrollx += scroll_velx;
			scrolly += scroll_vely;
		}

		// fix scroll
		int window_height = m_backend->getMainRenderTarget()->getHeight();
		// can't go past top or bottom
		if (scrolly < 0) {
			scrolly = 0;
			scroll_vely = 0;
		} else if (CREATURES1_WORLD_HEIGHT - scrolly < window_height) {
			scrolly = CREATURES1_WORLD_HEIGHT - window_height;
			scroll_vely = 0;
		}
		// wraparound left and right
		if (scrollx < 0) {
			scrollx = CREATURES1_WORLD_WIDTH + scrollx;
		} else if (scrollx >= CREATURES1_WORLD_WIDTH) {
			scrollx -= CREATURES1_WORLD_WIDTH;
		}
	}
	int scrollx = 0;
	int scrolly = 0;

  private:
	static constexpr float SCROLL_ACCEL = 8;
	static constexpr float SCROLL_DECEL = 0.5;
	static constexpr float SCROLL_MAX = 64;

	bool scroll_left = false;
	bool scroll_right = false;
	bool scroll_up = false;
	bool scroll_down = false;

	float scroll_velx = 0;
	float scroll_vely = 0;

	std::shared_ptr<Backend> m_backend;
};


class Entity {
  public:
	int x;
	int y;
	int z;
	int object_sprite_base;
	int part_sprite_base;
	int sprite_index;
	creaturesImage sprite;
	bool has_animation;
	unsigned int animation_frame; // only if has_animation is true
	std::string animation_string; // only if has_animation is true
};

class EntityManager {
  public:
	void add_entity(const Entity& e) {
		m_entities.push_back(e);
		m_entities_zorder.clear();
	}

	void tick() {
		for (auto& e : m_entities) {
			if (!e.has_animation) {
				continue;
			}
			if (e.animation_frame >= e.animation_string.size()) {
				// already done
				// TODO: are we on the correct frame already?
				// TODO: clear animation?
				e.has_animation = false;
				e.animation_string = {};
				e.animation_frame = 0;
				continue;
			}
			e.animation_frame += 1;
			if (e.animation_frame >= e.animation_string.size()) {
				// done!
				continue;
			}
			if (e.animation_string[e.animation_frame] == 'R') {
				e.animation_frame = 0;
			}

			// TODO: assert isdigit
			e.sprite_index = e.animation_string[e.animation_frame] - '0';
		}
	}

	const std::vector<Entity*>& entities_zorder() {
		if (m_entities_zorder.empty()) {
			fmt::print("* [EntityManager] Resorting entities by z-order\n");
			for (auto& e : m_entities) {
				m_entities_zorder.push_back(&e);
			}
			std::sort(m_entities_zorder.begin(), m_entities_zorder.end(), [](Entity* left, Entity* right) { return left->z < right->z; });
		}
		return m_entities_zorder;
	}

  private:
	std::vector<Entity> m_entities;
	std::vector<Entity*> m_entities_zorder;
};


static Entity entity_from_sfc(std::shared_ptr<ImageManager> image_manager, sfc::EntityV1& part) {
	if (part.x >= CREATURES1_WORLD_WIDTH) {
		throw Exception(fmt::format("Expected x to be between [0, {}), but got {}", CREATURES1_WORLD_WIDTH, part.x));
	}
	if (part.y >= CREATURES1_WORLD_HEIGHT) {
		throw Exception(fmt::format("Expected y to be between [0, {}), but got {}", CREATURES1_WORLD_HEIGHT, part.y));
	}

	Entity e;
	e.x = part.x;
	e.y = part.y;
	e.z = part.z_order;
	e.object_sprite_base = part.sprite->first_sprite;
	e.part_sprite_base = part.image_offset;
	e.sprite_index = part.current_sprite - part.image_offset;
	e.sprite = image_manager->get_image(part.sprite->filename, ImageManager::IMAGE_SPR);
	e.has_animation = part.has_animation;
	if (part.has_animation) {
		e.animation_frame = part.animation_frame;
		e.animation_string = part.animation_string;
	}
	return e;
}

int main(int argc, char** argv) {
	if (argc != 2) {
		fmt::print(stderr, "Usage: {} path-to-creatures1-data\n", argv[0]);
		return 1;
	}

	std::string datapath = argv[1];
	if (!fs::exists(datapath)) {
		fmt::print(stderr, "* Error: Data path {} does not exist\n", repr(datapath));
		return 1;
	}

	fmt::print("* Creatures 1 Data: {}\n", repr(datapath));

	// set up global objects
	auto g_backend = std::make_shared<SDLBackend>();
	auto g_audio_backend = SDLMixerBackend::getInstance();
	auto g_path_manager = std::make_shared<PathManager>(datapath);
	auto g_image_manager = std::make_shared<ImageManager>(g_path_manager);
	auto g_music_manager = std::make_shared<C1MusicManager>(g_path_manager, g_audio_backend);
	auto g_scroll_manager = std::make_shared<C1ScrollManager>(g_backend);
	auto g_entity_manager = std::make_shared<EntityManager>();

	// load palette
	g_image_manager->load_default_palette();

	// load Eden.sfc
	auto eden_sfc_path = g_path_manager->find_path(PATH_TYPE_BASE, "Eden.sfc");
	if (eden_sfc_path.empty()) {
		fmt::print(stderr, "* Error: Couldn't find Eden.sfc\n");
		return 1;
	}
	fmt::print("* Found Eden.sfc: {}\n", repr(eden_sfc_path));
	auto sfc = sfc::read_sfc_v1_file(eden_sfc_path);

	// load world data
	g_scroll_manager->scrollx = sfc.scrollx;
	g_scroll_manager->scrolly = sfc.scrolly;
	std::chrono::time_point<std::chrono::steady_clock> time_of_last_tick{};

	std::vector<Entity> entities;
	for (auto* scen : sfc.sceneries) {
		g_entity_manager->add_entity(entity_from_sfc(g_image_manager, *scen->part));
	}
	for (auto* obj : sfc.objects) {
		if (auto* simp = dynamic_cast<sfc::SimpleObjectV1*>(obj)) {
			g_entity_manager->add_entity(entity_from_sfc(g_image_manager, *simp->part));
		} else if (auto* comp = dynamic_cast<sfc::CompoundObjectV1*>(obj)) {
			for (auto& part : comp->parts) {
				g_entity_manager->add_entity(entity_from_sfc(g_image_manager, *part.entity));
			}
		}
	}

	// load background
	auto background_name = sfc.map->background->filename;
	fmt::print("* Background sprite: {}\n", repr(background_name));
	auto background = g_image_manager->get_image(background_name, ImageManager::IMAGE_SPR);
	// TODO: do any C1 metarooms have non-standard sizes?
	if (background.width(0) != CREATURES1_WORLD_WIDTH || background.height(0) != CREATURES1_WORLD_HEIGHT) {
		throw Exception(fmt::format("Expected Creatures 1 background size to be 8352x1200 but got {}x{}", background.width(0), background.height(0)));
	}

	// run loop
	g_backend->init();
	g_audio_backend->init();
	uint32_t last_frame_end = SDL_GetTicks();
	while (true) {
		// handle ui events
		BackendEvent event;
		bool should_quit = false;
		while (g_backend->pollEvent(event)) {
			g_scroll_manager->handle_event(event);
			if (event.type == eventquit) {
				should_quit = true;
			}
		}
		if (should_quit) {
			break;
		}

		// update world
		auto time_since_last_tick = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now() - time_of_last_tick)
										.count();
		if (time_since_last_tick >= 100) {
			time_of_last_tick = std::chrono::steady_clock::now();
			g_scroll_manager->tick();
			g_entity_manager->tick();
		}

		// // world music
		g_music_manager->update();

		// draw world (twice, to handle wraparound)
		g_backend->getMainRenderTarget()->renderCreaturesImage(background, 0, -g_scroll_manager->scrollx, -g_scroll_manager->scrolly);
		g_backend->getMainRenderTarget()->renderCreaturesImage(background, 0, -g_scroll_manager->scrollx + CREATURES1_WORLD_WIDTH, -g_scroll_manager->scrolly);

		// draw entities
		for (auto* part : g_entity_manager->entities_zorder()) {
			int x = part->x - g_scroll_manager->scrollx;
			int frame = part->object_sprite_base + part->part_sprite_base + part->sprite_index;
			// what to do if it's near the wraparound? just draw multiple times?
			g_backend->getMainRenderTarget()->renderCreaturesImage(part->sprite, frame, x, part->y - g_scroll_manager->scrolly);
			g_backend->getMainRenderTarget()->renderCreaturesImage(part->sprite, frame, x + CREATURES1_WORLD_WIDTH, part->y - g_scroll_manager->scrolly);
		}

		// present and wait
		SDL_RenderPresent(g_backend->renderer);

		static constexpr int OPENC2E_MAX_FPS = 60;
		static constexpr int OPENC2E_MS_PER_FRAME = 1000 / OPENC2E_MAX_FPS;

		Uint32 frame_end = SDL_GetTicks();
		if (frame_end - last_frame_end < OPENC2E_MS_PER_FRAME) {
			SDL_Delay(OPENC2E_MS_PER_FRAME - (frame_end - last_frame_end));
		}
		last_frame_end = frame_end;
	}

	return 0;
}