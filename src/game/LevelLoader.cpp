#include <fstream>
#include "LevelLoader.hpp"
#include "constants.hpp"

LevelLoader::LevelLoader(std::shared_ptr<Registry> registry, std::shared_ptr<AssetStore> asset_store, SDL_Renderer *renderer)
{
    this->registry = registry;
    this->asset_store = asset_store;
    this->renderer = renderer;
}

void LevelLoader::load(sol::state &lua, int level)
{
    using namespace constants;
    std::string script_path = "./scripts/level" + std::to_string(level) + ".lua";
    sol::load_result script = lua.load_file(script_path);
    if (!script.valid())
    {
        sol::error err = script;
        std::string errorMessage = err.what();
        Logger::error("Error loading the lua script: " + errorMessage);
        return;
    }

    lua.script_file(script_path);

    sol::table Level = lua["level"];
    sol::table assets = Level["assets"];

    int i = 1;
    while (true)
    {
        sol::optional<sol::table> has_asset = assets[i];
        if (has_asset == sol::nullopt)
        {
            break;
        }
        sol::table asset = assets[i];
        std::string asset_type = asset["type"];
        std::string asset_id = asset["id"];
        if (asset_type == "texture")
        {
            asset_store->add_texture(renderer, asset_id, asset["file"]);
        }
        if (asset_type == "font")
        {
            asset_store->add_font(renderer, asset_id, asset["file"], asset["font_size"]);
        }
        i++;
    }

    // load tilemap and create entities

    sol::table map = Level["tilemap"];
    std::string map_file_path = map["map_file"];
    std::string map_texture_asset_id = map["texture_asset_id"];
    std::fstream map_file;
    map_file.open(map_file_path);

    if (!map_file.is_open())
    {
        Logger::error("Failed to open map file."s);
    }

    for (int y = 0; y < map_rows; y++)
    {
        for (int x = 0; x < map_cols; x++)
        {
            char ch;
            map_file.get(ch);
            int src_rect_y = std::atoi(&ch) * tile_size;
            map_file.get(ch);
            int src_rect_x = std::atoi(&ch) * tile_size;
            map_file.ignore();

            Entity tile = registry->create_entity();
            tile.group("tiles");
            tile.add_component<TransformComponent>(
                vec2(x * (tile_scale * tile_size), y * (tile_scale * tile_size)),
                vec2(tile_scale, tile_scale), 0.0);
            tile.add_component<SpriteComponent>(map_texture_asset_id, tile_size, tile_size,
                                                0, false, src_rect_x, src_rect_y);
        }
    }
    map_file.close();

    sol::table entities = Level["entities"];
    i = 1;
    while (true)
    {
        sol::optional<sol::table> maybe_entity = entities[i];
        if (maybe_entity == sol::nullopt)
        {
            break;
        }

        sol::table entity = entities[i];

        Entity new_entity = registry->create_entity();

        // Tag
        sol::optional<std::string> tag = entity["tag"];
        if (tag != sol::nullopt)
        {
            new_entity.tag(entity["tag"]);
        }

        // Group
        sol::optional<std::string> group = entity["group"];
        if (group != sol::nullopt)
        {
            new_entity.group(entity["group"]);
        }

        // Components
        sol::optional<sol::table> components = entity["components"];
        if (components != sol::nullopt)
        {
            // Transform
            sol::optional<sol::table> maybe_transform = entity["components"]["transform"];
            if (maybe_transform != sol::nullopt)
            {
                sol::table transform = maybe_transform.value();
                new_entity.add_component<TransformComponent>(
                    glm::vec2(
                        transform["position"]["x"],
                        transform["position"]["y"]),
                    glm::vec2(
                        transform["scale"]["x"].get_or(1.0),
                        transform["scale"]["y"].get_or(1.0)),
                    transform["rotation"].get_or(0.0));
            }

            // RigidBody
            sol::optional<sol::table> maybe_rigid_body = entity["components"]["rigid_body"];
            if (maybe_rigid_body != sol::nullopt)
            {
                sol::table rigid_body = maybe_rigid_body.value();
                new_entity.add_component<RigidBodyComponent>(
                    glm::vec2(
                        rigid_body["velocity"]["x"].get_or(0.0),
                        rigid_body["velocity"]["y"].get_or(0.0)));
            }

            // sprint
            sol::optional<sol::table> maybe_sprint = entity["components"]["sprint"];
            if (maybe_sprint != sol::nullopt)
            {
                sol::table sprint = maybe_sprint.value();
                new_entity.add_component<SprintComponent>(
                    sprint["in_sprint"].get_or(false),
                    sprint["sprint_speed"].get_or(2),
                    sprint["sprint_duration"].get_or(3) * 1000,
                    sprint["sprint_cooldown"].get_or(5) * 1000);
            }

            // Sprite
            sol::optional<sol::table> maybe_sprite = entity["components"]["sprite"];
            if (maybe_sprite != sol::nullopt)
            {
                sol::table sprite = maybe_sprite.value();
                new_entity.add_component<SpriteComponent>(
                    sprite["texture_asset_id"],
                    sprite["width"],
                    sprite["height"],
                    sprite["z_index"].get_or(1),
                    sprite["is_fixed"].get_or(false),
                    sprite["src_rect_x"].get_or(0),
                    sprite["src_rect_y"].get_or(0));
            }

            // Animation
            sol::optional<sol::table> maybe_animation = entity["components"]["animation"];
            if (maybe_animation != sol::nullopt)
            {
                sol::table animation = maybe_animation.value();
                new_entity.add_component<AnimationComponent>(
                    animation["num_frames"].get_or(1),
                    animation["frame_rate"].get_or(1));
            }

            // BoxCollider
            sol::optional<sol::table> maybe_collider = entity["components"]["box_collider"];
            if (maybe_collider != sol::nullopt)
            {
                sol::table collider = maybe_collider.value();
                new_entity.add_component<BoxColliderComponent>(
                    collider["width"],
                    collider["height"],
                    glm::vec2(
                        collider["offset"]["x"].get_or(0),
                        collider["offset"]["y"].get_or(0)));
            }

            // Health
            sol::optional<sol::table> maybe_health = entity["components"]["health"];
            if (maybe_health != sol::nullopt)
            {
                sol::table health = maybe_health.value();
                new_entity.add_component<HealthComponent>(static_cast<int>(health["health_percentage"].get_or(100)));
            }

            // ProjectileEmitter
            sol::optional<sol::table> maybe_projectile_emitter = entity["components"]["projectile_emitter"];
            if (maybe_projectile_emitter != sol::nullopt)
            {
                sol::table projectile_emitter = maybe_projectile_emitter.value();
                new_entity.add_component<ProjectileEmitterComponent>(
                    glm::vec2(
                        projectile_emitter["projectile_velocity"]["x"].get_or(10) * 10,
                        projectile_emitter["projectile_velocity"]["y"].get_or(10) * 10),
                    static_cast<int>(projectile_emitter["projectile_freq"].get_or(1)) * 500,
                    static_cast<int>(projectile_emitter["projectile_duration"].get_or(10)) * 1000,
                    projectile_emitter["projectile_friendly"].get_or(false),
                    static_cast<int>(projectile_emitter["projectile_damage"].get_or(10)));
            }

            // CameraFollow
            sol::optional<sol::table> maybe_camera_follow = entity["components"]["camera_follow"];
            if (maybe_camera_follow != sol::nullopt)
            {
                new_entity.add_component<CameraFollowComponent>();
            }

            // KeyboardControll
            sol::optional<sol::table> maybe_keyboard = entity["components"]["keyboard_control"];
            if (maybe_keyboard != sol::nullopt)
            {
                sol::table keyboard = maybe_keyboard.value();
                new_entity.add_component<KeyboardControlComponent>(
                    glm::vec2(
                        keyboard["up_velocity"]["x"],
                        keyboard["up_velocity"]["y"]),
                    glm::vec2(
                        keyboard["right_velocity"]["x"],
                        keyboard["right_velocity"]["y"]),
                    glm::vec2(
                        keyboard["down_velocity"]["x"],
                        keyboard["down_velocity"]["y"]),
                    glm::vec2(
                        keyboard["left_velocity"]["x"],
                        keyboard["left_velocity"]["y"]));
            }

            // MouseControl
            sol::optional<sol::table> maybe_mouse = entity["components"]["mouse_control"];
            if (maybe_mouse != sol::nullopt)
            {
                new_entity.add_component<MouseControlComponent>();
            }

            // Script
            sol::optional<sol::table> maybe_script = entity["components"]["on_update_script"];
            if (maybe_script != sol::nullopt)
            {
                sol::function fun = entity["components"]["on_update_script"]["fun"];
                new_entity.add_component<ScriptComponent>(fun);
            }
        }
        i++;
    }
}