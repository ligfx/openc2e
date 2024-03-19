#pragma once

#include "Renderable.h"
#include "SFCSerialization.h"
#include "fileformats/NewSFCFile.h"
#include "objects/Object.h"

#include <assert.h>

// currently treated as a component on the main Object class, could
// be a subclass in the future

struct BodyPart {
	// not implemented
	int32_t angle;
	int32_t view;

	// sorta implemented
	Renderable renderable;
};

struct BodyData {
	inline auto attachment_for_pose_and_part(int32_t pose, int32_t part) {
		// TODO: part should be an enum class!
		return data[numeric_cast<size_t>(part)][numeric_cast<size_t>(pose)];
	}

	void sfc_serialize(std::string direction, std::array<std::array<sfc::Vector2i8, 10>, 6>& body_data) {
		// SFC files store body data in column-major order, unlike ATT
		// files which are in row-major order
		if (direction == "from") {
			data = body_data;
		} else {
			body_data = data;
		}
	}

  private:
	// store body data in column-major order, like SFC files but unlike
	// ATT files which are in row-major order
	std::array<std::array<sfc::Vector2i8, 10>, 6> data;
};

struct Body : BodyPart {
	BodyData body_data;
};

struct Limb : BodyPart {
	std::array<sfc::LimbData, 10> limb_data;
	std::unique_ptr<Limb> next_limb;

	auto startx() const {
		return limb_data[(size_t)renderable.get_pose()].startx;
	}
	auto starty() const {
		return limb_data[(size_t)renderable.get_pose()].starty;
	}
	auto endx() const {
		return limb_data[(size_t)renderable.get_pose()].endx;
	}
	auto endy() const {
		return limb_data[(size_t)renderable.get_pose()].endy;
	}
};

struct Creature : Object {
	// TODO: lots of things!

	// not implemented
	std::string moniker;
	std::string mother;
	std::string father;

	// sorta implemented
	std::unique_ptr<Body> body;
	std::unique_ptr<Limb> head;
	std::unique_ptr<Limb> left_thigh;
	std::unique_ptr<Limb> right_thigh;
	std::unique_ptr<Limb> left_arm;
	std::unique_ptr<Limb> right_arm;
	std::unique_ptr<Limb> tail;

	Limb* left_shin() { return left_thigh ? left_thigh->next_limb.get() : nullptr; }
	Limb* left_foot() { return left_shin() ? left_shin()->next_limb.get() : nullptr; }
	Limb* right_shin() { return right_thigh ? right_thigh->next_limb.get() : nullptr; }
	Limb* right_foot() { return right_shin() ? right_shin()->next_limb.get() : nullptr; }
	Limb* left_hand() { return left_arm ? left_arm->next_limb.get() : nullptr; }
	Limb* right_hand() { return right_arm ? right_arm->next_limb.get() : nullptr; }

	uint8_t direction;
	bool downfoot_left;
	int32_t footx;
	int32_t footy;

	// not implemented
	uint32_t z_order;
	std::string current_pose;
	uint8_t expression;
	uint8_t eyes_open;
	uint8_t asleep;

	// sorta implemented
	std::array<std::string, 100> poses;
	std::array<std::string, 8> gait_animations;

	// not implemented
	std::array<sfc::VocabWordV1, 80> vocabulary;
	std::array<sfc::Vector2i, 40> object_positions;
	std::array<sfc::StimulusV1, 36> stimuli;
	// std::unique_ptr<Brain> brain;
	// std::unique_ptr<Biochemistry> biochemistry;
	std::shared_ptr<sfc::CBrainV1> brain;
	std::shared_ptr<sfc::CBiochemistryV1> biochemistry;
	uint8_t sex;
	uint8_t age;

	// sorta implemented
	uint32_t biotick;

	// not implemented
	std::string gamete;
	std::string zygote;
	uint8_t dead;

	// sorta implemented
	uint32_t age_ticks;

	// not implemented
	uint32_t dreaming;
	std::vector<std::shared_ptr<sfc::CInstinctV1>> instincts;
	std::array<std::array<uint32_t, 16>, 40> goals;
	std::shared_ptr<sfc::SimpleObjectV1> zzzz;
	std::array<std::array<uint32_t, 3>, 27> voices_lookup;
	std::array<sfc::VoiceV1, 32> voices;
	std::string history_moniker;
	std::string history_name;
	std::string history_moms_moniker;
	std::string history_dads_moniker;
	std::string history_birthday;
	std::string history_birthplace;
	std::string history_owner_name;
	std::string history_owner_phone;
	std::string history_owner_address;
	std::string history_owner_email;

	void load(SFCLoader&, const sfc::CreatureV1*);
	void save(SFCSaver&, sfc::CreatureV1*) const;
	void creature_tick();
};
