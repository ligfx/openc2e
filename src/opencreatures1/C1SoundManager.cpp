#include "C1SoundManager.h"

#include "PathManager.h"
#include "ViewportManager.h"
#include "common/Exception.h"
#include "common/Repr.h"
#include "openc2e-audiobackend/AudioBackend.h"

#include <cmath>
#include <fmt/core.h>

C1SoundManager::C1SoundManager(std::shared_ptr<AudioBackend> audio_,
	std::shared_ptr<PathManager> paths_, std::shared_ptr<ViewportManager> viewport_)
	: audio(audio_), paths(paths_), viewport(viewport_) {
}
C1SoundManager::~C1SoundManager() {
}

bool C1SoundManager::is_muted() {
	return muted;
}

void C1SoundManager::set_muted(bool muted_) {
	muted = muted_;
	update_volumes();
}

bool C1SoundManager::is_alive(SoundData& sound_data) {
	if (sound_data.handle && audio->getChannelState(sound_data.handle) == AUDIO_PLAYING) {
		return true;
	} else {
		return false;
	}
}

C1SoundManager::SoundData* C1SoundManager::get_sound_data(C1Sound& sound) {
	static_assert(sizeof(C1Sound::id) == sizeof(C1SoundManager::SoundId), "");
	SoundId id = *reinterpret_cast<SoundId*>(&sound.id); // TODO

	SoundData* data = sources.try_get(id);
	if (!data) {
		return nullptr;
	}

	return data;
}

C1Sound C1SoundManager::get_new_sound(AudioChannel handle) {
	SoundData data;
	data.handle = handle;
	update_volume(data);

	SoundId id = sources.add(std::move(data));

	C1Sound sound;
	static_assert(sizeof(C1Sound::id) == sizeof(C1SoundManager::SoundId), "");
	sound.id = *reinterpret_cast<uint32_t*>(&id); // TODO
	sound.soundmanager = this;
	return sound;
}

template <typename T, typename U, typename V>
auto clamp(T value, U low, V high) {
	assert(!(high < low));
	return (value < low) ? low : (high < value) ? high : value;
}

void C1SoundManager::update_volume(SoundData& s) {
	if (!is_alive(s)) {
		return;
	}

	float volume = muted ? 0 : 1;

	if (s.positioned) {
		const auto centerx = viewport->centerx();
		const auto centery = viewport->centery();

		// std::remainder gives the distance between x and centerx, taking
		// into account world wraparound ("modular distance")
		const float distx = std::remainder(s.x + s.width / 2 - centerx, CREATURES1_WORLD_WIDTH);
		const float disty = s.y + s.height / 2 - centery;

		const float screen_width = viewport->width();
		const float screen_height = viewport->height();

		// If a sound is on-screen, then play it at full volume.
		// If it's more than a screen offscreen, then mute it.
		// TODO: Does this sound right?
		const float starts_fading = 0.5;
		const float cutoff = 1.5;
		// the math
		float volx = 1.0;
		if (std::abs(distx) / screen_width > starts_fading) {
			volx = clamp(1 - (std::abs(distx) / screen_width - starts_fading) / (cutoff - starts_fading), 0, 1);
		}
		float voly = 1.0;
		if (std::abs(disty) / screen_height > starts_fading) {
			voly = clamp(1 - (std::abs(disty) / screen_height - starts_fading) / (cutoff - starts_fading), 0, 1);
		}
		volume *= std::min(volx, voly);

		// Pan sound as we get closer to screen edge
		// TODO: Does this sound right?
		float pan = clamp(distx / screen_width, -1, 1);
		audio->setChannelPan(s.handle, pan);
	}

	if (s.fade_start != decltype(s.fade_start)()) {
		float volume_multiplier = 1 - (std::chrono::steady_clock::now() - s.fade_start) / s.fade_length;
		if (volume_multiplier <= 0) {
			audio->stopChannel(s.handle);
		}
		volume *= volume_multiplier;
	}

	audio->setChannelVolume(s.handle, volume);
}

void C1SoundManager::update_volumes() {
	for (auto i : sources.enumerate()) {
		if (!is_alive(*i.value)) {
			// Make sure erasing during enumeration is okay!!!
			sources.erase(i.id);
			continue;
		}
		update_volume(*i.value);
	}
}

C1Sound C1SoundManager::play_sound(std::string name, bool loop) {
	if (name.size() == 0)
		return {};

	std::string filename = paths->find_path(PATH_TYPE_SOUND, name + ".wav");
	if (filename.empty()) {
		// creatures 1 ignores non-existent audio clips
		fmt::print("WARNING: couldn't find audio clip {}\n", repr(name));
		return {};
	}

	auto handle = audio->playClip(filename, loop);
	if (!handle) {
		// note that more specific error messages can be thrown by implementations of playClip
		throw_exception("failed to play audio clip {}{}", filename, loop ? " (loop)" : "");
	}

	return get_new_sound(handle);
}

void C1SoundManager::stop(SoundData& source_data) {
	return audio->stopChannel(source_data.handle);
}

AudioState C1SoundManager::get_channel_state(SoundData& source_data) {
	return audio->getChannelState(source_data.handle);
}

void C1SoundManager::tick() {
	// update sounds
	// TODO: is this slow?
	update_volumes();
}

/* vim: set noet: */
