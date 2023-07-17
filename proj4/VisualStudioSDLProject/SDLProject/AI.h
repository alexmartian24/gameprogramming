#ifndef AI_ENTITY_H
#define AI_ENTITY_H

#include "Entity.h"

enum AIType { WALKER, GUARD };
enum AIState { WALKING, IDLE, ATTACKING };

class AIEntity : public Entity {
private:
    AIType m_ai_type;
    AIState m_ai_state;

public:
    AIEntity();

    void ai_walk();
    void ai_guard(Entity* player);

    AIType get_ai_type() const;
    AIState get_ai_state() const;

    void set_ai_type(AIType new_ai_type);
    void set_ai_state(AIState new_state);
};

#endif // AI_ENTITY_H
