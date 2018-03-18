
#include "Warrior.h"

#include <iostream>

std::string Warrior::get_name(void) const {
    return "Warrior";
}

int Warrior::get_strength_modifier(void) const {
    return 3;
}

int Warrior::get_agility_modifier(void) const {
    return 0;
}

int Warrior::get_stamina_modifier(void) const {
    return 2;
}

int Warrior::get_intellect_modifier(void) const {
    return 0;
}

int Warrior::get_spirit_modifier(void) const {
    return 0;
}

int Warrior::get_melee_ap(void) {
    // TODO: Include AP from buffs (Crusader, Might Rage Potion, world buffs, etc.)
    return get_strength() * 2;
}

void Warrior::gain_rage(const int gained_rage) {
    assert(gained_rage >= 0);
    this->rage += gained_rage;

    // TODO: Add statistics for rage lost due to overcapping.
    if (this->rage > 100)
        rage = 100;
}

void Warrior::lose_rage(const int lost_rage) {
    assert(lost_rage > 0);
    this->rage -= lost_rage;
    assert(this->rage >= 0);
}

int Warrior::rage_gained_from_dd(const int damage_dealt) const {
    // Using the 307 damage dealt = 10 rage formula.
    // TODO: add max(1, round(damage_dealt/30.7))
    return int(ceil(damage_dealt/30.7));
}

void Warrior::rotation() {
    // TODO: Some classes will need "special gcd" for stances, totems, shapeshifts, auras(?), etc.
    // Fury warriors need this for overpower modelling.

    if (!is_melee_attacking())
        start_attack();

    if (!action_ready()) {
        return;
    }

    // TODO: Check if execute is available. Requires target health.

    if (bt->is_available(rage))
        lose_rage(bt->perform(rage));
}

void Warrior::start_mh_attack(void) {
    // TODO: Apply Flurry buffs, etc to base weapon speed.
    float wpn_speed = equipment->get_mainhand()->get_base_weapon_speed();
    float next_timestamp = mh_attack->get_last_used() + wpn_speed;

    if (next_timestamp < engine->get_current_priority())
        next_timestamp = engine->get_current_priority();

    MainhandMeleeHit* new_event = new MainhandMeleeHit(this, next_timestamp);
    this->get_engine()->add_event(new_event);
}

void Warrior::start_oh_attack(void) {
    // TODO: Apply Flurry buffs, etc to base weapon speed.
    // TODO: Check if Heroic Strike "buff" up
    float wpn_speed = equipment->get_offhand()->get_base_weapon_speed();
    float next_timestamp = oh_attack->get_last_used() + wpn_speed;

    if (next_timestamp < engine->get_current_priority())
        next_timestamp = engine->get_current_priority();

    OffhandMeleeHit* new_event = new OffhandMeleeHit(this, next_timestamp);
    this->get_engine()->add_event(new_event);
}

void Warrior::mh_auto_attack() {
    // TODO: Check for invalid attacks before performing.
    gain_rage(mh_attack->perform(rage));

    if (action_ready()) {
        PlayerAction* new_event = new PlayerAction(this, 0.1);
        this->get_engine()->add_event(new_event);
    }

    start_mh_attack();
}

void Warrior::oh_auto_attack() {
    // TODO: Check for invalid attacks before performing.
    gain_rage(oh_attack->perform(rage));

    if (action_ready()) {
        PlayerAction* new_event = new PlayerAction(this, 0.1);
        this->get_engine()->add_event(new_event);
    }

    start_oh_attack();
}

float Warrior::global_cooldown() const {
    return 1.5;
}

int Warrior::get_curr_rage() const {
    return this->rage;
}
