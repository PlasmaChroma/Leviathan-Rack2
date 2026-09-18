#include "../src/SibylEvolution.hpp"
#include <cassert>
#include <cmath>
#include <iostream>

int main() {
    sibyl::Pattern p;
    sibyl::TrackDef t;
    t.defaultGate = .5f;
    t.defaultVelocity = .6f;
    sibyl::StepEvent e;
    e.compiledPitchV = -2.f;
    e.hasMod = true; e.mod = 2.f;
    p.evolution.velocity = .2f;
    p.evolution.gate = .25f;
    p.evolution.glideMs = 50.f;
    p.evolution.mod[0] = 1.f;
    p.evolution.mod[1] = 2.f;
    p.evolution.mod[2] = 3.f;
    auto first = sibyl::evolveExpression(p, e, t, 303, 0, 0);
    assert(first.gate == .5f && first.velocity == .6f && first.glideMs == 0.f && first.mod[0] == 2.f);
    auto second = sibyl::evolveExpression(p, e, t, 303, 1, 0);
    auto replay = sibyl::evolveExpression(p, e, t, 303, 1, 0);
    assert(second.gate == replay.gate && second.velocity == replay.velocity && second.mod[2] == replay.mod[2]);
    assert(second.velocity != first.velocity && second.mod[0] != first.mod[0]);
    bool sawSlide = false;
    for (uint64_t pass = 1; pass < 1000; ++pass) {
        auto value = sibyl::evolveExpression(p, e, t, 303, pass, 0);
        assert(value.velocity >= .4f && value.velocity <= .8f);
        assert(value.gate >= .25f && value.gate <= .75f);
        assert(value.glideMs >= 0.f && value.glideMs <= 50.f);
        assert(value.mod[0] >= 1.f && value.mod[0] <= 3.f);
        assert(value.mod[1] >= -2.f && value.mod[1] <= 2.f);
        assert(value.mod[2] >= -3.f && value.mod[2] <= 3.f);
        sawSlide |= value.glideMs > 0.f;
    }
    assert(sawSlide);
    assert(e.compiledPitchV == -2.f && e.step == 0 && !e.tie && e.ratchets == 1);
    e.evolve = false;
    auto locked = sibyl::evolveExpression(p, e, t, 303, 900, 0);
    assert(locked.gate == first.gate && locked.mod[0] == first.mod[0]);
    e.evolve = true;
    e.hasGate = true; e.gate = 0.f;
    assert(sibyl::evolveExpression(p, e, t, 303, 900, 0).gate == 0.f);
    e.gate = 1.f; e.ratchets = 4; p.evolution.gate = 1024.f;
    for (int pass = 1; pass < 100; ++pass) {
        auto value = sibyl::evolveExpression(p, e, t, 42, pass, 0);
        assert(value.gate >= .01f && value.gate <= 1.f);
    }
    // Probability-only evolution enables the pattern and stays bounded/replayable.
    p.evolution = {};
    p.evolution.probability = .2f;
    e.hasProbability = true; e.probability = .7f;
    assert(p.evolution.enabled());
    assert(sibyl::evolveExpression(p, e, t, 303, 0, 0).probability == .7f);
    bool below = false, above = false;
    for (uint64_t pass = 1; pass < 1000; ++pass) {
        float value = sibyl::evolveExpression(p, e, t, 303, pass, 0).probability;
        assert(value >= .5f && value <= .9f);
        assert(value == sibyl::evolveExpression(p, e, t, 303, pass, 0).probability);
        below |= value < .7f; above |= value > .7f;
    }
    assert(below && above);
    e.evolve = false;
    assert(sibyl::evolveExpression(p, e, t, 303, 42, 0).probability == .7f);
    e.evolve = true;
    p.evolution.probability = 1.f;
    for (float base : {0.f, 1.f}) {
        e.probability = base;
        bool clamped = false;
        for (uint64_t pass = 1; pass < 100; ++pass) {
            float value = sibyl::evolveExpression(p, e, t, 303, pass, 0).probability;
            assert(value >= 0.f && value <= 1.f);
            clamped |= value == base;
        }
        assert(clamped);
    }
    e.hasProbability = false;
    assert(sibyl::evolveExpression(p, e, t, 303, 0, 0).probability == 1.f);
    sibyl::EvolutionCursor cursor;
    cursor.observe(-1, 16); assert(cursor.pass == 0);
    cursor.observe(0, 16); assert(cursor.pass == 1);
    cursor.observe(15, 16); assert(cursor.pass == 1);
    cursor.observe(16, 16); assert(cursor.pass == 2);
    cursor.newTraversal = true;
    cursor.observe(0, 16); assert(cursor.pass == 3);
    cursor.rebase = true;
    cursor.observe(4, 16); assert(cursor.pass == 3);
    cursor = {};
    cursor.observe(0, 16); assert(cursor.pass == 0);
    std::cout << "PASS: bounded repeat variation, replay, protected events, zero gates and ratchets\n";
}
