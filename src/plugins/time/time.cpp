#include "time.hpp"

#include "game.hpp"

#include "resources/time.hpp"

void plugins::Time::build(Game& game) {
  resources::Time time;
  game.addResource(time);

  game.addSystem(Schedule::Startup, [](resources::Time& time) {
    time.currentTime = glfwGetTime();
    time.lastTime = time.currentTime;
    time.deltaTime = 0.0f;
  });

  game.addSystem(Schedule::Update, [](resources::Time& time) {
    time.currentTime = glfwGetTime();
    time.deltaTime = time.currentTime - time.lastTime;
    time.lastTime = time.currentTime;
  });
}
