#include "pad_sprite.h"
#include "../hmi/hmi_manager.h"

void PadSprite::init_sprite() {
	lightning.init();
	lightning.set_duration(500);
	lightning.set_knee(0.2);
}

void PadSprite::fire(int size) {
	this->size = size;
	lightning.start();
}

bool PadSprite::can_fire() {
	return !lightning.running();
}

void PadSprite::animate() {
	lightning.animate();
	int imax = size * lightning;
	unsigned char r, g, b;
	r = get_reversed() ? 0xff : 0x00;
	g = get_reversed() ? 0x00 : 0xff;
	b = get_reversed() ? 0xff : 0xff;
	for (int i = HMI_WIDTH; i > HMI_WIDTH - imax; --i)
		draw(i, r * lightning, g * lightning, b * lightning);
}