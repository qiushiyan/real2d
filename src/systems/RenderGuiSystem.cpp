#include "ECS.hpp"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"

RenderGuiSystem::RenderGuiSystem()
{
}

void RenderGuiSystem::update(std::shared_ptr<Registry> registry, SDL_Rect &camera)
{
    using namespace constants;
    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    static int enemy_position_x = 50;
    static int enemy_position_y = 100;
    static int enemy_scale_x = 1;
    static int enemy_scale_y = 1;

    static int enemy_speed_x = 50;
    static int enemy_speed_y = 0;
    static float enemy_rotation = 0;
    static int enemy_health = 100;
    const char *enemy_images[]{"tank-image-right", "tank-image-left", "truck-image-right", "truck-image-left"};
    static int enemy_image_index = 0;

    static float projectile_speed = 100;
    static float projectile_angle = 0;
    static int projectile_duration = 0;
    static int projectile_freq = 0;
    static bool projectile_friendly = false;
    static int projectile_damage = 10;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize;
    if (ImGui::Begin("Control panel", NULL, window_flags))
    {
        if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Combo("image name", &enemy_image_index, enemy_images, IM_ARRAYSIZE(enemy_images));
        }
        ImGui::Spacing();

        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::InputInt("position x", &enemy_position_x);
            ImGui::InputInt("position y", &enemy_position_y);
            ImGui::SliderInt("scale x", &enemy_scale_x, 1, 10);
            ImGui::SliderInt("scale y", &enemy_scale_y, 1, 10);
            ImGui::SliderAngle("rotation (deg)", &enemy_rotation, 0, 360);
        }
        ImGui::Spacing();

        if (ImGui::CollapsingHeader("Projectile emitter", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::SliderAngle("angle (deg)", &projectile_angle, 0, 360);
            ImGui::SliderFloat2("speed (px/sec)", &projectile_speed, 10, 500);
            ImGui::InputInt("frequency (secs/emit)", &projectile_freq);
            ImGui::InputInt("duration (secs)", &projectile_duration);
            ImGui::Checkbox("friendly", &projectile_friendly);
        }
        ImGui::Spacing();

        if (ImGui::CollapsingHeader("Health", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::SliderInt("%", &enemy_health, 0, 100);
        }
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Create new enemy"))
        {
            auto enemy = registry->create_entity();
            enemy.group("enemies");
            enemy.add_component<TransformComponent>(vec2(enemy_position_x, enemy_position_y), vec2(enemy_scale_x, enemy_scale_y), glm::degrees(enemy_rotation));
            enemy.add_component<RigidBodyComponent>(vec2(enemy_speed_x, enemy_speed_y));
            enemy.add_component<SpriteComponent>(enemy_images[enemy_image_index], tile_size, tile_size,
                                                 3);
            enemy.add_component<BoxColliderComponent>(tile_size * enemy_scale_x, tile_size * enemy_scale_y);
            enemy.add_component<ProjectileEmitterComponent>(vec2(cos(projectile_angle) * projectile_speed, sin(projectile_angle) * projectile_speed), projectile_freq * 1000, projectile_duration * 1000,
                                                            projectile_friendly, projectile_damage);
            enemy.add_component<HealthComponent>(enemy_health);

            // reset values
            enemy_scale_x = enemy_scale_y = 1;
            projectile_speed = projectile_angle = projectile_freq = projectile_duration = 0;
            projectile_friendly = false;
            projectile_damage = 10;
            enemy_health = 100;
        }
    }

    ImGui::End();

    // window showing map coordinates
    window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav;
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always, ImVec2(0, 0));
    ImGui::SetNextWindowBgAlpha(0.9f);

    if (ImGui::Begin("Map coordinates", NULL, window_flags))
    {
        ImGui::Text(
            "(x=%.1f, y=%.1f)",
            ImGui::GetIO().MousePos.x + camera.x,
            ImGui::GetIO().MousePos.y + camera.y);
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
}
