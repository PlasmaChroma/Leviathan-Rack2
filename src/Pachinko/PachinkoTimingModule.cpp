#include "PachinkoTimingModule.hpp"
#include <algorithm>
#include <cmath>
#include <random>

PachinkoTimingModule::PachinkoTimingModule() {
    // Initialize balls
    balls.resize(maxBalls);
    for (auto& ball : balls) {
        ball.active = false;
    }
    std::fill(bucketOutputs.begin(), bucketOutputs.end(), 0.0f);
    std::fill(outputDecay.begin(), outputDecay.end(), 0.0f);
    lastBallTime = 0.0f;
    generatePegs();
}

void PachinkoTimingModule::onReset(const ResetEvent& e) {
    for (auto& ball : balls) {
        ball.active = false;
        ball.position = math::Vec(0.0f, -120.0f);
        ball.velocity = math::Vec(0.0f, 0.0f);
        ball.lifetime = 0.0f;
    }
    std::fill(bucketOutputs.begin(), bucketOutputs.end(), 0.0f);
    std::fill(outputDecay.begin(), outputDecay.end(), 0.0f);
    lastBallTime = 0.0f;
    generatePegs();
    Module::onReset(e);
}

void PachinkoTimingModule::onRandomize(const RandomizeEvent& e) {
    // Randomize board type
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, BOARD_NUM_TYPES - 1);
    boardType = static_cast<BoardType>(dist(gen));
    generatePegs();
    Module::onRandomize(e);
}

void PachinkoTimingModule::generatePegs() {
    pegs.clear();
    
    switch (boardType) {
        case BOARD_SYMMETRIC:
            generateSymmetricPegs();
            break;
        case BOARD_ZIGZAG:
            generateZigzagPegs();
            break;
        case BOARD_HONEYCOMB:
            generateHoneycombPegs();
            break;
        default:
            generateSymmetricPegs();
            break;
    }
}

void PachinkoTimingModule::generateSymmetricPegs() {
    // Symmetric pyramid pattern
    float pegSpacingX = 15.0f;
    float pegSpacingY = 18.0f;
    float startX = -60.0f;
    float startY = -100.0f;
    
    for (int row = 0; row < 10; row++) {
        int pegsInRow = 3 + row;
        float rowWidth = (pegsInRow - 1) * pegSpacingX;
        float rowStartX = startX - rowWidth / 2.0f;
        
        for (int col = 0; col < pegsInRow; col++) {
            Peg peg;
            peg.position = math::Vec(rowStartX + col * pegSpacingX, startY + row * pegSpacingY);
            peg.radius = 2.5f;
            pegs.push_back(peg);
        }
    }
}

void PachinkoTimingModule::generateZigzagPegs() {
    // Zigzag pattern
    float pegSpacingX = 18.0f;
    float pegSpacingY = 20.0f;
    float startX = -60.0f;
    float startY = -100.0f;
    
    for (int row = 0; row < 10; row++) {
        int pegsInRow = 4 + (row % 2);
        float rowWidth = (pegsInRow - 1) * pegSpacingX;
        float rowStartX = startX - rowWidth / 2.0f + (row % 2) * (pegSpacingX / 2.0f);
        
        for (int col = 0; col < pegsInRow; col++) {
            Peg peg;
            peg.position = math::Vec(rowStartX + col * pegSpacingX, startY + row * pegSpacingY);
            peg.radius = 2.5f;
            pegs.push_back(peg);
        }
    }
}

void PachinkoTimingModule::generateHoneycombPegs() {
    // Honeycomb/hex pattern
    float pegSpacingX = 16.0f;
    float pegSpacingY = 14.0f * 1.1547f; // sqrt(3)/2 * spacing
    float startX = -60.0f;
    float startY = -100.0f;
    
    for (int row = 0; row < 10; row++) {
        int pegsInRow = 4 + (row % 2);
        float rowWidth = (pegsInRow - 1) * pegSpacingX;
        float rowStartX = startX - rowWidth / 2.0f + (row % 2) * (pegSpacingX / 2.0f);
        
        for (int col = 0; col < pegsInRow; col++) {
            Peg peg;
            peg.position = math::Vec(rowStartX + col * pegSpacingX, startY + row * pegSpacingY);
            peg.radius = 2.5f;
            pegs.push_back(peg);
        }
    }
}

void PachinkoTimingModule::process(const ProcessArgs& args) {
    float dt = args.sampleTime;
    double currentTime = static_cast<double>(args.frame) / args.sampleRate;
    
    // Process clock input
    float clockValue = inputs[CLOCK_INPUT].value;
    bool clockTriggered = clockPulse.process(clockValue);
    
    // Process reset input
    bool resetTriggered = resetPulse.process(inputs[RESET_INPUT].value);
    if (resetTriggered) {
        onReset(ResetEvent{});
    }
    
    // Generate new balls based on clock
    if (clockTriggered || (clockValue > 2.0f && currentTime - lastBallTime > 1.0f / ballRate)) {
        // Find inactive ball
        for (auto& ball : balls) {
            if (!ball.active) {
                ball.position = math::Vec(0.0f, -120.0f);
                std::mt19937 gen(static_cast<unsigned int>(currentTime * 1000));
                std::uniform_real_distribution<float> randX(-10.0f, 10.0f);
                ball.velocity = math::Vec(
                    randX(gen),
                    50.0f
                );
                ball.lifetime = 0.0f;
                ball.active = true;
                lastBallTime = currentTime;
                break;
            }
        }
    }
    
    // Physics constants
    const float gravity = 1500.0f; // pixels/s^2
    const float damping = params[DAMPING_PARAM].value;
    const float ballRadius = 4.0f;
    const float pegRadius = 2.5f;
    const float restitution = 0.6f;
    const float friction = 0.98f;
    const float minBallDist = ballRadius * 2.0f;
    const float minBallDistSq = minBallDist * minBallDist;
    
    // Update active balls
    for (auto& ball : balls) {
        if (!ball.active) continue;
        
        // Apply gravity
        ball.velocity.y += gravity * dt;
        
        // Apply damping to velocity
        ball.velocity *= std::pow(damping, dt * 5.0f);
        
        // Update position
        ball.position += ball.velocity * dt;
        ball.lifetime += dt;
        
        // Check peg collisions
        for (const auto& peg : pegs) {
            math::Vec delta = ball.position - peg.position;
            float distSq = delta.x * delta.x + delta.y * delta.y;
            float minDist = pegRadius + ballRadius;
            
            if (distSq < minDist * minDist && distSq > 0.0f) {
                float dist = std::sqrt(distSq);
                math::Vec normal = delta / dist;
                
                // Resolve overlap
                float overlap = minDist - dist;
                ball.position += normal * overlap;
                
                // Reflect velocity
                float dot = ball.velocity.x * normal.x + ball.velocity.y * normal.y;
                ball.velocity -= normal * (2.0f * dot);
                
                // Apply restitution and friction
                ball.velocity *= restitution;
                ball.velocity.x *= friction;
                
                // Add randomness for pachinko effect
                std::mt19937 ballGen(static_cast<unsigned int>(ball.position.x * 1000 + ball.position.y * 1000));
                std::uniform_real_distribution<float> rand(-1.0f, 1.0f);
                ball.velocity.x += rand(ballGen) * 30.0f;
            }
        }
        
        // Check ball-to-ball collisions if enabled
        if (ballToBallCollisions) {
            for (size_t i = 0; i < balls.size(); i++) {
                if (!balls[i].active || &balls[i] == &ball) continue;
                
                math::Vec ballDelta = ball.position - balls[i].position;
                float ballDistSq = ballDelta.x * ballDelta.x + ballDelta.y * ballDelta.y;
                
                if (ballDistSq < minBallDistSq && ballDistSq > 0.0f) {
                    float ballDist = std::sqrt(ballDistSq);
                    math::Vec ballNormal = ballDelta / ballDist;
                    
                    // Resolve overlap
                    float ballOverlap = minBallDist - ballDist;
                    math::Vec correction = ballNormal * (ballOverlap * 0.5f);
                    ball.position += correction;
                    balls[i].position -= correction;
                    
                    // Elastic collision response
                    math::Vec relVel = ball.velocity - balls[i].velocity;
                    float velAlongNormal = relVel.x * ballNormal.x + relVel.y * ballNormal.y;
                    
                    if (velAlongNormal < 0) {
                        float j = -(1.0f + restitution) * velAlongNormal;
                        j /= 2.0f; // Equal mass
                        math::Vec impulse = ballNormal * j;
                        ball.velocity += impulse;
                        balls[i].velocity -= impulse;
                    }
                }
            }
        }
        
        // Check if ball is out of bounds or too old
        if (ball.position.y > 150.0f || ball.lifetime > 10.0f) {
            ball.active = false;
            
            // Determine which bucket (if any)
            if (ball.position.y > 150.0f) {
                float bucketWidth = 180.0f / numBuckets;
                int bucketIndex = static_cast<int>((ball.position.x + 90.0f) / bucketWidth);
                bucketIndex = rack::math::clamp(bucketIndex, 0, numBuckets - 1);
                bucketOutputs[bucketIndex] = 10.0f;
            }
        }
    }
    
    // Update outputs
    const float decayRate = 20.0f; // decay per second
    for (int i = 0; i < numBuckets; i++) {
        // Decay previous output
        outputDecay[i] = std::max(0.0f, outputDecay[i] - decayRate * dt);
        
        // If new bucket hit, set output
        if (bucketOutputs[i] > 0.0f) {
            outputs[GATE_1_OUTPUT + i].value = bucketOutputs[i];
            lights[GATE_1_LIGHT + i].value = 1.0f;
            outputDecay[i] = bucketOutputs[i];
            bucketOutputs[i] = 0.0f;
        } else {
            outputs[GATE_1_OUTPUT + i].value = outputDecay[i];
            lights[GATE_1_LIGHT + i].value = (outputDecay[i] > 1.0f) ? 1.0f : 0.0f;
        }
    }
}

json_t* PachinkoTimingModule::dataToJson() {
    json_t* rootJ = json_object();
    
    json_object_set_new(rootJ, "boardType", json_integer(boardType));
    json_object_set_new(rootJ, "maxBalls", json_integer(maxBalls));
    json_object_set_new(rootJ, "ballToBallCollisions", json_boolean(ballToBallCollisions));
    
    return rootJ;
}

void PachinkoTimingModule::dataFromJson(json_t* rootJ) {
    json_t* boardTypeJ = json_object_get(rootJ, "boardType");
    if (boardTypeJ) boardType = static_cast<BoardType>(json_integer_value(boardTypeJ));
    
    json_t* maxBallsJ = json_object_get(rootJ, "maxBalls");
    if (maxBallsJ) maxBalls = json_integer_value(maxBallsJ);
    
    json_t* ballToBallJ = json_object_get(rootJ, "ballToBallCollisions");
    if (ballToBallJ) ballToBallCollisions = json_boolean_value(ballToBallJ);
    
    // Regenerate pegs on load
    generatePegs();
}
