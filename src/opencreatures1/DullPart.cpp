#include "DullPart.h"

#include "EngineContext.h"
#include "ViewportManager.h"
#include "common/NumericCast.h"
#include "common/render/RenderSystem.h"

#include <fmt/format.h>
#include <math.h>

DullPart::DullPart() = default;

DullPart::DullPart(const ImageGallery& gallery, int32_t sprite_base, float x, float y, int32_t z) {
	gallery_ = gallery;
	base_ = sprite_base;
	x_ = x;
	y_ = y;
	z_ = z;

	renderitem_ = get_rendersystem()->render_item_create(LAYER_OBJECTS);
	update_renderitem();
}

void DullPart::set_position(float x, float y) {
	// TODO: better way of handling world wrap?
	x = remainderf(x, numeric_cast<float>(CREATURES1_WORLD_WIDTH));

	x_ = x;
	y_ = y;
	update_renderitem();
}

float DullPart::x() const {
	return x_;
}
float DullPart::y() const {
	return y_;
}

int32_t DullPart::z_order() const {
	return z_;
}

void DullPart::set_base(int DullPart_sprite_base) {
	base_ = DullPart_sprite_base;
}

int32_t DullPart::base() const {
	return base_;
}

const ImageGallery& DullPart::gallery() const {
	return gallery_;
}

int32_t DullPart::frame() const {
	return base_ + pose_;
}

int32_t DullPart::width() const {
	if (!gallery_) {
		return 0;
	}
	return gallery_.width(frame());
}

int32_t DullPart::height() const {
	if (!gallery_) {
		return 0;
	}
	return gallery_.height(frame());
}

void DullPart::set_animation(uint8_t animation_frame, std::string animation_string) {
	has_animation_ = true;
	animation_frame_ = animation_frame;
	animation_string_ = animation_string;

	// Change the sprite index immediately, because some scripts are written poorly and will
	// stop their animations on their next tick anyways. We assume that this has been called
	// from a CAOS macro, and that macros are run after object updates (e.g. if we don't set
	// the sprite index now, it won't get set until the next tick when the object updates).
	//
	// For example, the Creatures 1 Harp runs something like this when clicked:
	//
	//     anim [303030], snde hrp2, endi, else, endi, stim writ ..., stim shou ..., pose 0
	//
	// Whoops! The ANIM command will consume a tick, but ENDI, ELSE, SNDE, and STIM all run
	// instantly. So, the Harp gets a single frame to animate before resetting it's pose. This
	// has been confirmed on the official engine.
	update_animation();
}

uint8_t DullPart::animation_frame() const {
	return animation_frame_;
}

const std::string& DullPart::animation_string() const {
	return animation_string_;
}

bool DullPart::has_animation() const {
	return has_animation_;
}

void DullPart::clear_animation() {
	has_animation_ = false;
	animation_frame_ = 0;
	animation_string_ = {};
}

void DullPart::update_animation() {
	if (!has_animation_) {
		return;
	}

	if (animation_frame_ >= animation_string_.size()) {
		// already done
		// TODO: are we on the correct frame already?
		clear_animation();
		return;
	}

	// some objects in Eden.sfc start at the 'R' character, so set frame
	// before incrementing.
	// TODO: assert isdigit
	if (animation_string_[animation_frame_] == 'R') {
		animation_frame_ = 0;
	}
	set_pose(animation_string_[animation_frame_] - '0');
	animation_frame_ += 1;
}

void DullPart::set_pose(int sprite_index) {
	pose_ = sprite_index;
	update_renderitem();
}

int32_t DullPart::pose() const {
	return pose_;
}

void DullPart::update_renderitem() {
	get_rendersystem()->render_item_set_texture(renderitem_,
		gallery_.texture,
		gallery_.texture_locations[numeric_cast<size_t>(frame())]);
	get_rendersystem()->render_item_set_position(renderitem_, x_, y_, z_);
}

std::string format_as(const DullPart& r) {
	if (r.has_animation()) {
		return fmt::format(
			"<DullPart x={} y={} z={} abba={} base={} pose={} gallery={} animation={} anim_index={}>",
			r.x_, r.y_, r.z_, r.gallery_.absolute_base, r.base_, r.pose_, r.gallery_.name, r.animation_string_, r.animation_frame_);
	}
	return fmt::format(
		"<DullPart x={} y={} z={} abba={} base={} pose={} gallery={} animation=false>",
		r.x_, r.y_, r.z_, r.gallery_.absolute_base, r.base_, r.pose_, r.gallery_.name);
}