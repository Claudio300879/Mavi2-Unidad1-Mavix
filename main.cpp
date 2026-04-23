#ifndef NDEBUG
#include <vld.h>
#endif

#include "raylib.h"
#include <box2d.h>
#include <vector>

// Estructura para representar nuestras cajas en ambos mundos (Box2D y Raylib)
struct PhysicsBox
{
    b2Body* body;
    float width;
    float height;
    Color color;
};

int main(void)
{
    // --- CONFIGURACIÓN INICIAL ---
    const int screenWidth = 1000;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Mavix - El Despertar ");
    SetTargetFPS(60);

    // Colores del mundo de Mavix
    Color fondo = { 110, 100, 215, 255 };
    Color sueloColor = DARKGREEN;

    // 1. MUNDO FÍSICO 
    b2Vec2 gravity(0.0f, 50.0f);
    b2World world(gravity);

    // 2. SUELO ESTÁTICO 
    b2BodyDef groundDef;
    groundDef.type = b2_staticBody;
    groundDef.position.Set(screenWidth / 2.0f, screenHeight - 20.0f);
    b2Body* groundBody = world.CreateBody(&groundDef);

    b2PolygonShape groundShape;
    groundShape.SetAsBox(screenWidth / 2.0f, 20.0f);
    groundBody->CreateFixture(&groundShape, 0.0f);

    std::vector<PhysicsBox> boxes;

    // Variable para la orientación previa 
    float anguloMavix = 0.0f;

    // --- CICLO PRINCIPAL ---
    while (!WindowShouldClose())
    {
        // --- LÓGICA DE MAVIX (INPUT) ---

        // CAMBIO AQUÍ: Control de rotación cíclica (0-360 grados)
        if (IsKeyDown(KEY_LEFT)) anguloMavix -= 2.0f;
        if (IsKeyDown(KEY_RIGHT)) anguloMavix += 2.0f;

        // Si el ángulo se pasa de 360 vuelve a 0, si baja de 0 sube a 360
        if (anguloMavix >= 360.0f) anguloMavix = 0.0f;
        if (anguloMavix < 0.0f) anguloMavix = 358.0f; 
        // -----------------------------------------------------------

        // Crear nueva caja (ESPACIO)
        if (IsKeyPressed(KEY_SPACE))
        {
            b2BodyDef boxDef;
            boxDef.type = b2_dynamicBody;
            boxDef.position.Set(screenWidth / 2.0f, 50.0f);

            // Se aplica el ángulo actual (convertido a radianes) al nacer la caja
            boxDef.angle = anguloMavix * DEG2RAD;

            b2Body* boxBody = world.CreateBody(&boxDef);

            b2PolygonShape boxShape;
            boxShape.SetAsBox(20.0f, 20.0f);

            b2FixtureDef boxFixture;
            boxFixture.shape = &boxShape;
            boxFixture.density = 1.0f;
            boxFixture.friction = 0.3f;
            boxFixture.restitution = 0.4f;

            boxBody->CreateFixture(&boxFixture);

            boxes.push_back({ boxBody, 40.0f, 40.0f, SKYBLUE });
        }

        // --- SIMULACIÓN 
        world.Step(1.0f / 60.0f, 8, 3);

        // --- DIBUJO 
        BeginDrawing();
        ClearBackground(fondo);

        DrawRectangle(0, screenHeight - 40, screenWidth, 40, sueloColor);

        for (const auto& box : boxes)
        {
            b2Vec2 pos = box.body->GetPosition();
            float angle = box.body->GetAngle() * RAD2DEG;

            Rectangle rect = { pos.x, pos.y, box.width, box.height };
            Vector2 origin = { box.width / 2.0f, box.height / 2.0f };

            DrawRectanglePro(rect, origin, angle, box.color);
            DrawRectangleLinesEx(rect, 2, DARKBLUE);
        }

        // Interfaz de Mavix
        DrawText(" [ESPACIO] Crear Caja -------------- [FLECHAS] Rotar", 20, 20, 20, RAYWHITE);
        DrawText(TextFormat("Angulo proxima caja: %.1f grados", anguloMavix), 20, 50, 18, DARKPURPLE);

        // Previsualización de la rotación
        DrawRectanglePro({ 150, 110, 40, 40 }, { 20, 20 }, anguloMavix, Fade(SKYBLUE, 0.5f));

        EndDrawing();
    }

    CloseWindow();
    return 0;
}