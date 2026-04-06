#include "murdoch_scene.hpp"
#include "../Physics/CollisionCallbacks/log_collision_callback.hpp"
#include "../Scripts/grounded_player_script.hpp"
#include "ScriptingAPI/scripting_murdoch_scene.hpp"
#include <Agent/Dog/dog.hpp>
#include <Affordance/affordance.hpp>
#include <Debug/debug.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Agent/Dog/dog.hpp"
#include <Agent/Dog/dog_behaviour_component.hpp>


MurdochScene::MurdochScene()
    : 
    m_app(1920, 1080), 
    m_physicsSystem(&m_entityManager), 
    m_player(0),
    m_COMEntity(1),
    m_posEntity(2),
    m_testRigidBodyEntity(3),
    m_aim(),
    m_scripting(nullptr)
{

}

void MurdochScene::Run()
{
    bool restart = false;
    std::string input;

    std::cout << "[Loading] Generating CPU Data...\n";

    GenerateCPUData();

    do
    {
        std::cout << "[Loading] Loading scene...\n";

        m_app = Program(1920, 1080);
        InitialiseEngine();
        UploadAssetsToGPU();
        AssignPipelines();

        // Reconstruct ECS and physics system before initialization
        m_entityManager = EntityManager();
        m_physicsSystem.Reset(&m_entityManager); // ensures fresh world

        m_app.SetQuit(false);

        std::cout << "[Loading] Setting up entities...\n";
        SetupECS();

        // Initialize scripting.
        m_scripting = new MurdochScripting();
        m_scripting->Init(shared_from_this());
        //Dog::AddToScriptingAPI(*m_scripting);

        // Call starting lua script.
        std::cout << "[Scripting] Execute start script." << std::endl;
        m_scripting->Execute("scripts/start.lua");

        MainLoop();
        CleanUp();

        std::cout << "[Menu] Do you wish to restart?\nEnter YES to restart.\n Enter anything else to quit\n";
        std::cin >> input;

        if (input == "YES")
        {
            restart = true;
        }
        else
        {
            restart = false;
            std::cout << "\n[Menu] Quitting program...\n";
        }

    } while (restart);

    Shutdown();
}

void MurdochScene::RegisterUpdateCallback(std::function<void(float)> func)
{
    updateCallbacks.push_back(func);
}

EntityManager& MurdochScene::GetECS()
{
    return m_entityManager;
}

PhysicsSystem& MurdochScene::GetPhysicsSystem()
{
    return m_physicsSystem;
}

void MurdochScene::GenerateCPUData()
{
    sol::state lua;
    lua.open_libraries(sol::lib::base);

    m_meshFiles.clear(); // Optional: clear old data if reused

    m_meshFiles.push_back(MeshFile());
    m_meshFiles[0].GenerateCPUData("Mesh_Files/sack.obj");

    m_meshFiles.push_back(MeshFile());
    m_meshFiles[1].GenerateCPUData("Mesh_Files/Balls/projectile.obj");

    m_meshFiles.push_back(MeshFile());
    m_meshFiles[2].GenerateCPUData("Mesh_Files/Crates/crate.obj");

    m_meshFiles.push_back(MeshFile());
    m_meshFiles[3].GenerateCPUData("Mesh_Files/Maps/Lounge_Room/lounge_room.obj");

    m_meshFiles.push_back(MeshFile());
    m_meshFiles[4].GenerateCPUData("Mesh_Files/TestMeshes/Cylinder/cylinder.obj");

    m_meshFiles.push_back(MeshFile());
    m_meshFiles[5].GenerateCPUData("Mesh_Files/Chairs/chair.obj");

    m_meshFiles.push_back(MeshFile());
    m_meshFiles[6].GenerateCPUData("Mesh_Files/Bins/landfill_bin.obj");

    m_meshFiles.push_back(MeshFile());
    m_meshFiles[7].GenerateCPUData("Mesh_Files/Bins/recycling_bin.obj");

    m_meshFiles.push_back(MeshFile());
    m_meshFiles[8].GenerateCPUData("Mesh_Files/Snowman/snowman.obj");

    m_meshFiles.push_back(MeshFile());
    m_meshFiles[9].GenerateCPUData("Mesh_Files/DogBed/dog_bed.obj");

    m_meshFiles.push_back(MeshFile());
    m_meshFiles[10].GenerateCPUData("Mesh_Files/DogWaterBowl/water_bowl.obj");

    m_meshFiles.push_back(MeshFile());
    m_meshFiles[11].GenerateCPUData("Mesh_Files/DogFoodBowl/food_bowl.obj");

    //m_meshFileMap["dog_sit"] = 12;
    //m_meshFiles.push_back(MeshFile());
    //m_meshFiles.back().GenerateCPUData("DogSprites/GermanShepherd/sit.obj");
    //
    //m_meshFileMap["dog_walk"] = 13;
    //m_meshFiles.push_back(MeshFile());
    //m_meshFiles.back().GenerateCPUData("DogSprites/GermanShepherd/walk.obj");
    //
    //m_meshFileMap["dog_play"] = 14;
    //m_meshFiles.push_back(MeshFile());
    //m_meshFiles.back().GenerateCPUData("DogSprites/GermanShepherd/play.obj");
    //
    //m_meshFileMap["dog_rest"] = 15;
    //m_meshFiles.push_back(MeshFile());
    //m_meshFiles.back().GenerateCPUData("DogSprites/GermanShepherd/rest.obj");
    //
    //m_meshFileMap["dog_eat"] = 16;
    //m_meshFiles.push_back(MeshFile());
    //m_meshFiles.back().GenerateCPUData("DogSprites/GermanShepherd/eat.obj");
    //
    //m_meshFileMap["dog_eat"] = 17;
    //m_meshFiles.push_back(MeshFile());
    //m_meshFiles.back().GenerateCPUData("DogSprites/GermanShepherd/clean.obj");

    m_sky.GenerateCPUData(30, 30, 500.0f, "Mesh_Files/Skies/sky1.jpg");

}

void MurdochScene::InitialiseEngine()
{
    //mApp.Initialise();
    m_app.InitialiseWindow();
    m_app.InitialiseGraphics();
    m_app.LockMouse(true);
    m_aim.Init();
    //Initialise End Scene
    m_texture.LoadFromFile("Mesh_Files/creditScene.png");
    m_texture.UploadToGPU();
    m_image2D.Init(m_app.GetScreenWidth(), m_app.GetScreenHeight());
    //playerScript.SetImageRenderer(&image2D, &mTexture);

    m_app.CreateGraphicsPipeline("object", "objectVS.glsl", "objectFS.glsl");
    m_app.CreateGraphicsPipeline("terrain", "terrainVS.glsl", "terrainFS.glsl");
    m_app.CreateGraphicsPipeline("skydome", "skyDomeVS.glsl", "skyDomeFS.glsl");

    //Image
    m_image2D.Init(m_app.GetScreenWidth(), m_app.GetScreenHeight(), "2DimageVS.glsl", "2DimageFS.glsl");

    //Initialise renderer
    Dog::InitRenderer();
}

void MurdochScene::RenderAllDogs() 
{
    const auto& cam = m_entityManager.GetComponent<Camera>(m_player);
    const auto& camTf = m_entityManager.GetComponent<Transform>(m_player);
    const float* view = glm::value_ptr(cam.GetViewMatrix(camTf.GetPosition()));
    const float* proj = glm::value_ptr(cam.GetProjectionMatrix());

    auto dogs = m_entityManager.GetAllComponents<Dog::DogImage>();
    for (auto& [e, img] : dogs)
    {
        if (!m_entityManager.HasComponent<Transform>(e))
        {
            continue;
        }
        const Transform& tf = m_entityManager.GetComponent<Transform>(e);
        //For update texture you need to passing the dogImage structure to it
        //Dog::setTexture(img,"sit");
        Dog::RenderDogImage(img, tf, view, proj);
        if (m_entityManager.HasComponent<Dog::DogEmojiImage>(e)) 
        {
            auto& emojiImg = m_entityManager.GetComponent<Dog::DogEmojiImage>(e);
            Vector2f dogSize = tf.GetScale();
            Vector2f emojiSize = Vector2f(0.8f, 0.8f);
            Dog::RenderDogEmoji(emojiImg.emojiImage, tf, view, proj, dogSize, emojiSize, 5.f, .5f);
        }
    }
}

void MurdochScene::UploadAssetsToGPU()
{
    for (auto& mesh : m_meshFiles)
    {
        mesh.UploadToGPU();
    }
    m_sky.UploadToGPU();
}

void MurdochScene::AssignPipelines()
{
    for (auto& mesh : m_meshFiles)
    {
        mesh.SetPipeline(m_app.GetShaderProgram("object"));
    }
    m_sky.SetPipeline(m_app.GetShaderProgram("skydome"));
}

void MurdochScene::SetupECS()
{
    SetupSkyDomeEntity();
    SetupPlayer();
    //RegisterTerrainToECS();

    std::srand(static_cast<unsigned>(std::time(nullptr)));
    SpawnSacks();

    //SpawnMassiveCrates();
    //SpawnTrees();

    SpawnCrates();

    SpawnBalls();

    SpawnFlatGround();

    SpawnMap();

    SpawnChair(Vector3f(495.f, 4.f, 515.f));
    SpawnChair(Vector3f(490.f, 4.f, 515.f));
    SpawnChair(Vector3f(480.f, 4.f, 515.f));

    SpawnBin(Vector3f(501.f, 2.f, 520.f), 6);
    SpawnBin(Vector3f(501.f, 2.f, 523.f), 7);

    SpawnSnowman(Vector3f(540.f, 7.f, 520.f));
}

void MurdochScene::MainLoop()
{
    m_inputManager = InputManager();
    m_inputManager.MapAction(InputAction::MoveForward, KeyCode::W);
    m_inputManager.MapAction(InputAction::MoveBackward, KeyCode::S);
    m_inputManager.MapAction(InputAction::MoveLeft, KeyCode::A);
    m_inputManager.MapAction(InputAction::MoveRight, KeyCode::D);
    m_inputManager.MapAction(InputAction::Jump, KeyCode::Space);
    m_inputManager.MapAction(InputAction::E_Key, KeyCode::E);
    m_inputManager.MapAction(InputAction::Quit, KeyCode::Escape);

    Entity player;

    Debug debug(shared_from_this());
    debug.Init();

    Vector3f pos = Vector3f(485.0f, 0.6f, 517.0f);
    Dog::SpawnDog(shared_from_this(), pos);

    Entity dog = SpawnDog(485.0f, 0.6f, 509.0f);
    SetDogBreed(dog, Dog::Breed::ToyPoodle);


    const auto& cameras = m_entityManager.GetAllComponents<Camera>();
    for (const auto& [entity, cam] : cameras)
    {
        if (!m_entityManager.HasComponent<Camera>(entity))
        {
            continue;
        }

        Camera tempCam = m_entityManager.GetComponent<Camera>(entity);
        if (tempCam.IsEnabled())
        {
            player = entity;
        }
    }

    GroundedPlayerScript playerScript(player, &m_entityManager, &m_physicsSystem);
    playerScript.SetImageRenderer(&m_image2D, &m_texture);
    //bool firstFrame = true;

    const float fixedStep = 1.0f / 120.0f; // simulate at 120Hz
    float accumulator = 0.0f;

    while (!m_app.GetQuit())
    {
        m_app.UpdateDeltaTime();


        float dt = std::min(0.0333f, m_app.GetDeltaTime() * 2.5f);  // clamp to 33ms
        accumulator = std::min(accumulator + dt, 0.25f);    // cap catch-up ~0.25s

        bool quit = playerScript.CaptureInput(m_inputManager);
        m_app.SetQuit(quit);

        while (accumulator >= fixedStep)
        {
            m_physicsSystem.Update(fixedStep);
            accumulator -= fixedStep;
        }

        // Call all update functions.
        for (std::function<void(float)> func : updateCallbacks) 
        {
            func(dt);
        }

        UpdateTestColliders();
        
        // Update all dogs
        for (auto& dog : m_dogs) 
        {
            if(!m_entityManager.HasComponent<Dog::DogBehaviour>(dog)) 
            {
                continue;
            }
            Dog::DogBehaviour& behaviour = m_entityManager.GetComponent<Dog::DogBehaviour>(dog);
            behaviour.Update(dt);
        }

        //gApp.mQuit = playerScript.Input(input, entityManager, physicsSystem, dt);
        playerScript.PostPhysicsUpdate(fixedStep);
        //animations.Update(mEntityManager, dt);
        //mEnemySystem.Update(dt);
        RenderSystem(m_app, player, m_entityManager);
        RenderAllDogs();
        m_aim.Render();
        //glClear(GL_DEPTH_BUFFER_BIT);
        playerScript.DrawUI(m_app.GetScreenWidth(), m_app.GetScreenHeight());
        if (playerScript.EndScreen(dt, m_app.GetScreenWidth(), m_app.GetScreenHeight()))
        {
            m_app.SetQuit(true);
            std::cout << "Quit Game\n";
        }

        auto deadEntities = m_entityManager.GetAllComponents<Dead>();

        if (m_entityManager.HasComponent<Dead>(player))
        {
            m_app.SetQuit(true);
            std::cout << "GAME OVER! THE PLAYER DIED!\n";
        }


        for (auto& [e, dead] : deadEntities) 
        {
            if (m_entityManager.HasComponent<Physics::RigidBody*>(e))
                m_physicsSystem.DestroyEntity(e);
            m_entityManager.DestroyEntity(e);
        }
        debug.UpdateStart();
        debug.UpdateEnd();
        m_app.SwapWindow();
    }
    debug.Destroy();
}

void MurdochScene::UpdateTestColliders()
{
    if (!m_entityManager.HasComponent<Physics::RigidBody*>(m_testRigidBodyEntity)) 
    {
        return;
    }

    Physics::RigidBody* body = m_entityManager.GetComponent<Physics::RigidBody*>(m_testRigidBodyEntity);

    if (!body || m_testColliderEntities.empty())
    {
        return;
    }

    std::vector<Physics::Collider*> colliders = body->GetColliders();
    for (int i = 0; i < colliders.size(); i++)
    {
        Physics::Collider* col = colliders[i];
        Transform& tf = m_entityManager.GetComponent<Transform>(m_testColliderEntities[i]);

        Vector3f position = col->GetWorldCentre();
        Quaternion rotation = ToQuaternion(col->GetCompositeRotation());

        tf.SetPosition(position);
        tf.SetRotation(rotation);
    }

    Transform& com = m_entityManager.GetComponent<Transform>(m_COMEntity);
    com.SetPosition(body->GetWorldCentreOfMass());

    Transform& pos = m_entityManager.GetComponent<Transform>(m_posEntity);
    pos.SetPosition(body->GetPosition());
}

void MurdochScene::CleanUp()
{
    for (Entity e : m_entityManager.GetAllEntities()) 
    {
        m_physicsSystem.DestroyEntity(e);
        m_entityManager.DestroyEntity(e);
    }
    m_app.Delete();
    m_app.DeleteWindow();
}

void MurdochScene::Shutdown()
{
    for (auto& mesh : m_meshFiles)
    {
        mesh.Delete();
    }
    //mTerrain.Delete();
    m_sky.Delete();
    m_app.DeleteGraphics();
    m_aim.Shutdown();
    m_texture.Delete();
    m_image2D.Shutdown();
}


void MurdochScene::SetupSkyDomeEntity()
{
    Transform skyTransform;
    skyTransform.SetPosition(0.0f, -150.0f, 0.0f);

    Entity skyEntity = m_entityManager.CreateEntity();
    m_entityManager.AddComponent<Transform>(skyEntity, skyTransform);
    m_entityManager.AddComponent<MeshRenderer>(skyEntity, m_sky.ToRenderer());
}

void MurdochScene::SetupPlayer()
{
    m_player = m_entityManager.CreateEntity();
    Camera cam(Vector3f(0.f, 2.f, 0.f));
    cam.SetProjectionMatrix(75.f, m_app.GetScreenWidth() / m_app.GetScreenHeight(), 0.1f, 99999.f);
    m_entityManager.AddComponent<Camera>(m_player, cam);

    float x = 512.f;
    //Minimum x = 475.f;
    //Maximum x = 545.f;

    float z = 512.f;
    //Maximum z = 520.f;
    //Minimum z = 502.f;
    float y = 10.f;

    Transform tf;
    tf.SetPosition(x, y, z);
    m_entityManager.AddComponent<Transform>(m_player, tf);

    Physics::RigidBody* body = m_physicsSystem.GetWorld()->CreateRigidBody(tf.GetPosition(), tf.GetQuaternionRotation(), BodyType::DYNAMIC);
    body->SetAngularFreeAxis(Vector3f(0.f, 0.f, 0.f));
    body->SetAngularDamping(0.2f);
    m_entityManager.AddComponent<Physics::RigidBody*>(m_player, body);

    AABB playerBounds;
    playerBounds.min = { -0.5f, -3.f, -0.5f };
    playerBounds.max = { 0.5f, 3.f, 0.5f };

    //Physics::Collider* collider = m_physicsSystem.GetWorld()->AddCapsuleCollider(body, 1.f, 5.f, 1.f);
    Physics::Collider* collider = m_physicsSystem.GetWorld()->AddBoxCollider(body, playerBounds, 1.f);
    //collider->SetMass(5.f);
    collider->SetFriction(0.5f);
    collider->SetBounciness(0.01f);

    //collider->SetCategoryBits(PhysicsSystem::PLAYER_CATEGORY);
    //collider->SetCollisionMask(PhysicsSystem::TERRAIN_CATEGORY | PhysicsSystem::OBJECT_CATEGORY | PhysicsSystem::ROBOT_CATEGORY);

    m_entityManager.AddComponent<Physics::Collider*>(m_player, collider);

    Health playerHealth;
    playerHealth.max = 100.f;
    playerHealth.current = 100.f;
    m_entityManager.AddComponent<Health>(m_player, playerHealth);
}

void MurdochScene::SpawnFlatGround()
{


    Entity e = m_entityManager.CreateEntity();

    Transform tf;
    tf.SetPosition(512.f, -2.5f, 512.f);

    m_entityManager.AddComponent<Transform>(e, tf);

    // Create a dynamic rigid body using the physics backend
    Physics::RigidBody* body = m_physicsSystem.GetWorld()->CreateRigidBody(tf.GetPosition(), tf.GetQuaternionRotation(), BodyType::KINEMATIC);  // Assuming true = dynamic


    AABB boundingbox;
    boundingbox.min = { -512.f, -5.f, -512.f };
    boundingbox.max = { 512.f, 0.f, 512.f };
    // Create a sphere collider for the rigid body using the physics backend
    Physics::Collider* collider = m_physicsSystem.GetWorld()->AddBoxCollider(body, boundingbox, 500.f);
    collider->SetFriction(0.9f);

    // Set collision category and mask bits
    //collider->SetCategoryBits(PhysicsSystem::TERRAIN_CATEGORY);
    //collider->SetCollisionMask(PhysicsSystem::PLAYER_CATEGORY | PhysicsSystem::OBJECT_CATEGORY | PhysicsSystem::ROBOT_CATEGORY);


    boundingbox.min = { -512.f, 0.f, -10.f };
    boundingbox.max = { 512.f, 200.f, 10.f };
    collider = m_physicsSystem.GetWorld()->AddBoxCollider(body, boundingbox, 100.f, Vector3f(0.f, 0.f, -512.f));
    collider = m_physicsSystem.GetWorld()->AddBoxCollider(body, boundingbox, 100.f, Vector3f(0.f, 0.f, 512.f));

    boundingbox.min = { -10.f, 0.f, -512.f };
    boundingbox.max = { 10.f, 200.f, 512.f };
    collider = m_physicsSystem.GetWorld()->AddBoxCollider(body, boundingbox, 100.f, Vector3f(-512.f, 0.f, 0.f));
    collider = m_physicsSystem.GetWorld()->AddBoxCollider(body, boundingbox, 100.f, Vector3f(512.f, 0.f, 0.f));


    // Add the Rigidbody and Collider components to the entity
    m_entityManager.AddComponent<Physics::RigidBody*>(e, body);
    m_entityManager.AddComponent<Physics::Collider*>(e, collider);
}

void MurdochScene::SpawnMap()
{
    Entity e = m_entityManager.CreateEntity();

    Transform tf;
    tf.SetPosition(512.f, 0.f, 512.f);
    tf.SetScale(3.f, 3.f, 3.f);
    m_entityManager.AddComponent<Transform>(e, tf);

    MeshRenderer renderer = m_meshFiles[3].ToRenderer();
    m_entityManager.AddComponent<MeshRenderer>(e, renderer);

    Physics::RigidBody* body = m_physicsSystem.GetWorld()->CreateRigidBody(tf.GetPosition(), tf.GetQuaternionRotation(), BodyType::KINEMATIC);  // Assuming true = dynamic


    // Left wall (Z-)
    Vector3f boundsPosition = { 0.f, 8.f, -12.5f };
    Vector3f boundsScale = { 37.f, 8.2f, 1.f };
    AddBounds(body, boundsPosition, boundsScale, false);

    // Ceiling (Y+)
    boundsPosition = { 0.f, 17.f, 0.f };
    boundsScale = { 37.f, 1.f, 12.f };
    AddBounds(body, boundsPosition, boundsScale, false);

    //North Pillar
    boundsPosition = { 21.f, 8.f, -1.f };
    boundsScale = { 2.4f, 8.2f, 2.4f };
    AddBounds(body, boundsPosition, boundsScale, false);

    //South wall (X-)
    boundsPosition = { -37.5f, 8.f, -3.f };
    boundsScale = { 1.f, 8.2f, 9.f };
    AddBounds(body, boundsPosition, boundsScale, false);

    //South corner pillar
    boundsPosition = { -36.f, 8.f, 9.f };
    boundsScale = { 2.f, 8.2f, 3.5f };
    AddBounds(body, boundsPosition, boundsScale, false);

    //Top wood
    boundsPosition = { -23.f, 11.8f, 11.5f };
    boundsScale = { 11.5f, 4.3f, 2.f };
    AddBounds(body, boundsPosition, boundsScale, false);

    //wood shelf
    boundsPosition = { -23.f, 7.5f, 12.3f };
    boundsScale = { 11.5f, 1.f, 1.f };
    AddBounds(body, boundsPosition, boundsScale, false);

    //middle wood
    boundsPosition = { -23.f, 5.5f, 11.5f };
    boundsScale = { 11.5f, 1.4f, 2.f };
    AddBounds(body, boundsPosition, boundsScale, false);

    //bottom wood
    boundsPosition = { -23.f, 2.2f, 13.5f };
    boundsScale = { 11.5f, 2.1f, 2.f };
    AddBounds(body, boundsPosition, boundsScale, false);

    //wood table
    boundsPosition = { -23.f, 4.2f, 9.2f };
    boundsScale = { 11.5f, 0.2f, 2.f };
    AddBounds(body, boundsPosition, boundsScale, false);

    //table stand
    boundsPosition = { -23.f, 4.2f, 9.2f };
    boundsScale = { 11.5f, 0.2f, 2.f };
    AddBounds(body, boundsPosition, boundsScale, false);

    //Wood table stand
    boundsPosition = { -23.f, 2.2f, 11.f };
    boundsScale = { 0.2f, 2.1f, 1.f };
    AddBounds(body, boundsPosition, boundsScale, false);

    //East corner pillar
    boundsPosition = { -11.4f, 8.f, 9.5f };
    boundsScale = { 0.35f, 8.2f, 2.5f };
    AddBounds(body, boundsPosition, boundsScale, false);

    //East door
    boundsPosition = { -6.2f, 8.f, 12.2f };
    boundsScale = { 5.f, 8.2f, 0.5f };
    AddBounds(body, boundsPosition, boundsScale, false);

    //East TV wall
    boundsPosition = { 9.3f, 8.f, 10.2f };
    boundsScale = { 10.5f, 8.2f, 1.5f };
    AddBounds(body, boundsPosition, boundsScale, false);

    //East TV shelf
    boundsPosition = { 6.f, 3.5f, 8.f };
    boundsScale = { 6.f, 1.f, 1.f };
    AddBounds(body, boundsPosition, boundsScale, false);

    //North door
    boundsPosition = { 26.f, 8.f, 12.2f };
    boundsScale = { 6.2f, 8.2f, 0.5f };
    AddBounds(body, boundsPosition, boundsScale, false);

    //Kitchen cabinet
    boundsPosition = { 34.5f, 8.f, 4.f };
    boundsScale = { 2.2f, 8.2f, 8.f };
    AddBounds(body, boundsPosition, boundsScale, false);

    //Kitchen sink
    boundsPosition = { 34.5f, 2.5f, -5.f };
    boundsScale = { 2.2f, 3.f, 8.f };
    AddBounds(body, boundsPosition, boundsScale, false);

    //North wall
    boundsPosition = { 37.f, 10.5f, -5.f };
    boundsScale = { 0.5f, 5.f, 8.f };
    AddBounds(body, boundsPosition, boundsScale, false);

    //Pillar counter
    boundsPosition = { 21.25f, 1.5f, 10.5f };
    boundsScale = { 1.5f, 4.f, 2.f };
    AddBounds(body, boundsPosition, boundsScale, false);

    //Pillar counter pt 2
    boundsPosition = { 21.75f, 1.5f, 5.f };
    boundsScale = { 1.f, 4.f, 3.7f };
    AddBounds(body, boundsPosition, boundsScale, false);

    //Pillar countertop
    boundsPosition = { 20.8f, 5.3f, 5.f };
    boundsScale = { 2.f, 0.1f, 3.7f };
    AddBounds(body, boundsPosition, boundsScale, false);
}

void MurdochScene::AddBounds(Physics::RigidBody* body, const Vector3f& boundsPosition, const Vector3f& boundsScale, bool testing)
{
    if (testing)
    {
        Entity boundEntity = m_entityManager.CreateEntity();
        Transform boundsTransform;
        boundsTransform.SetPosition(body->GetPosition() + boundsPosition);
        boundsTransform.SetScale(boundsScale);
        m_entityManager.AddComponent<Transform>(boundEntity, boundsTransform);

        MeshRenderer boundsRenderer = m_meshFiles[2].ToRenderer();
        m_entityManager.AddComponent<MeshRenderer>(boundEntity, boundsRenderer);
    }

    AABB bounds;
    bounds.max = Vector3f(1.f, 1.f, 1.f) * boundsScale;
    bounds.min = Vector3f(-1.f, -1.f, -1.f) * boundsScale;

    Physics::Collider* collider = m_physicsSystem.GetWorld()->AddBoxCollider(body, bounds, 1.f, boundsPosition);
}

void MurdochScene::AddBounds(Entity e, const Transform& localTF, float mass, bool testing)
{
    Transform parentTF = m_entityManager.GetComponent<Transform>(e);

    Transform tf;
    tf.SetPosition(localTF.GetPosition() * parentTF.GetScale());
    tf.SetRotation(localTF.GetQuaternionRotation());
    tf.SetScale(localTF.GetScale() * parentTF.GetScale());

    if (testing)
    {
        Entity boundEntity = m_entityManager.CreateEntity();
        Transform boundsTransform;

        boundsTransform.SetScale(tf.GetScale());
        m_entityManager.AddComponent<Transform>(boundEntity, boundsTransform);

        MeshRenderer boundsRenderer = m_meshFiles[2].ToRenderer();
        m_entityManager.AddComponent<MeshRenderer>(boundEntity, boundsRenderer);

        m_testColliderEntities.push_back(boundEntity);
    }

    AABB bounds;
    bounds.max = Vector3f(1.f, 1.f, 1.f) * tf.GetScale();
    bounds.min = Vector3f(-1.f, -1.f, -1.f) * tf.GetScale();

    Physics::RigidBody* body = m_entityManager.GetComponent<Physics::RigidBody*>(e);
    Physics::Collider* collider = m_physicsSystem.GetWorld()->AddBoxCollider(body, bounds, mass, tf.GetPosition(), tf.GetQuaternionRotation());
}

void MurdochScene::AddSphericalBounds(Entity e, const Transform& localTF, float mass, bool testing)
{
    Transform parentTF = m_entityManager.GetComponent<Transform>(e);

    Transform tf;
    tf.SetPosition(localTF.GetPosition() * parentTF.GetScale());
    tf.SetRotation(localTF.GetQuaternionRotation());
    tf.SetScale(localTF.GetScale() * parentTF.GetScale());

    if (testing)
    {
        Entity boundEntity = m_entityManager.CreateEntity();
        Transform boundsTransform;

        boundsTransform.SetScale(tf.GetScale());
        m_entityManager.AddComponent<Transform>(boundEntity, boundsTransform);

        MeshRenderer boundsRenderer = m_meshFiles[1].ToRenderer();
        m_entityManager.AddComponent<MeshRenderer>(boundEntity, boundsRenderer);

        m_testColliderEntities.push_back(boundEntity);
    }

    float radius = tf.GetScale().x;

    Physics::RigidBody* body = m_entityManager.GetComponent<Physics::RigidBody*>(e);
    Physics::Collider* collider = m_physicsSystem.GetWorld()->AddSphereCollider(body, radius, mass, tf.GetPosition(), tf.GetQuaternionRotation());
}

void MurdochScene::SpawnSacks()
{
    int count = 5;

    for (int i = 0; i < count; ++i)
    {
        float x = (std::rand() % 2 ? 1.f : -1.f) * static_cast<float>(std::rand() % 27);
        float z = (std::rand() % 2 ? 1.f : -1.f) * static_cast<float>(std::rand() % 9);
        float y = 10.f;

        SpawnSack(Vector3f(502.f + x, y, 511.f + z));
    }
}

void MurdochScene::SpawnSack(const Vector3f& position)
{
    MeshRenderer renderer = m_meshFiles[0].ToRenderer();
    const MeshLOD& baseLOD = renderer.lods[0];

    Transform tf;
    tf.SetPosition(position);
    tf.SetRotation(0.f, static_cast<float>(std::rand() % 360), 0.f);
    tf.SetScale(1.5f, 1.5f, 1.5f);

    Entity e = m_entityManager.CreateEntity();
    m_entityManager.AddComponent<Transform>(e, tf);
    m_entityManager.AddComponent<MeshRenderer>(e, renderer);

    // Create a dynamic rigid body using the physics backend
    Physics::RigidBody* body = m_physicsSystem.GetWorld()->CreateRigidBody(tf.GetPosition(), tf.GetQuaternionRotation(), BodyType::DYNAMIC);  // Assuming true = dynamic

    AABB bounds = baseLOD.localAABB;
    bounds.max = bounds.max * tf.GetScale();
    bounds.min = bounds.min * tf.GetScale();

    // Create a sphere collider for the rigid body using the physics backend
    Physics::Collider* collider = m_physicsSystem.GetWorld()->AddSphereCollider(body, 1.5f, 1.f);
    collider->SetFriction(1.0f);

    // Add the Rigidbody and Collider components to the entity
    m_entityManager.AddComponent<Physics::RigidBody*>(e, body);
    m_entityManager.AddComponent<Physics::Collider*>(e, collider);
}

void MurdochScene::SpawnSnowman(const Vector3f& position)
{
    MeshRenderer renderer = m_meshFiles[8].ToRenderer();
    const MeshLOD& baseLOD = renderer.lods[0];

    Transform tf;
    tf.SetPosition(position);
    //tf.SetRotation(0.f, static_cast<float>(std::rand() % 360), 0.f);
    tf.SetScale(2.5f, 2.5f, 2.5f);

    Entity e = m_entityManager.CreateEntity();
    m_entityManager.AddComponent<Transform>(e, tf);
    m_entityManager.AddComponent<MeshRenderer>(e, renderer);

    Physics::RigidBody* body = m_physicsSystem.GetWorld()->CreateRigidBody(tf.GetPosition(), tf.GetQuaternionRotation(), BodyType::DYNAMIC);
    m_entityManager.AddComponent<Physics::RigidBody*>(e, body);

    //Bottom plate
    Transform localTF;
    localTF.SetPosition(0.f, -0.91f, 0.f);
    localTF.SetScale(0.3f, 0.1f, 0.3f);
    AddBounds(e, localTF, 0.5f, false);

    //Bottom snowball
    localTF.SetPosition(0.f, 0.f, 0.f);
    localTF.SetScale(1.f, 1.f, 1.f);
    AddSphericalBounds(e, localTF, 2.5f, false);

    //Middle snowball
    localTF.SetPosition(0.f, 1.34f, 0.f);
    localTF.SetScale(0.75f, 0.75f, 0.75f);
    AddSphericalBounds(e, localTF, 2.f, false);

    //Top snowball
    localTF.SetPosition(0.f, 2.41f, 0.f);
    localTF.SetScale(0.5f, 0.5f, 0.5f);
    AddSphericalBounds(e, localTF, 1.5f, false);

    //Carrot
    localTF.SetPosition(-0.6f, 2.5f, 0.f);
    localTF.SetScale(0.2f, 0.05f, 0.05f);
    AddBounds(e, localTF, 0.1f, false);

    //Left arm
    localTF.SetPosition(0.f, 1.34f, 0.8f);
    localTF.SetRotation(-10.f, 0.f, 0.f);
    localTF.SetScale(0.05f, 0.05f, 1.f);
    AddBounds(e, localTF, 0.1f, false);

    //Right arm
    localTF.SetPosition(0.f, 1.34f, -0.8f);
    localTF.SetRotation(10.f, 0.f, 0.f);
    localTF.SetScale(0.05f, 0.05f, 1.f);
    AddBounds(e, localTF, 0.1f, false);

    //SetupTestColliders(e);
}

void MurdochScene::SpawnBalls()
{
    int count = 10;

    for (int i = 0; i < count; ++i)
    {
        float x = (std::rand() % 2 ? 1.f : -1.f) * static_cast<float>(std::rand() % 27);
        float z = (std::rand() % 2 ? 1.f : -1.f) * static_cast<float>(std::rand() % 9);
        float y = 10.f;

        SpawnBall(Vector3f(502.f + x, y, 511.f + z));
    }
}

void MurdochScene::SpawnBall(const Vector3f& position)
{
    MeshRenderer renderer = m_meshFiles[1].ToRenderer();
    const MeshLOD& baseLOD = renderer.lods[0];

    Transform tf;
    tf.SetPosition(position);
    tf.SetRotation(0.f, static_cast<float>(std::rand() % 360), 0.f);
    float radius = 0.3f;
    tf.SetScale(radius, radius, radius);

    Entity e = m_entityManager.CreateEntity();
    m_entityManager.AddComponent<Transform>(e, tf);
    m_entityManager.AddComponent<MeshRenderer>(e, renderer);

    // Create a dynamic rigid body using the physics backend
    Physics::RigidBody* body = m_physicsSystem.GetWorld()->CreateRigidBody(tf.GetPosition(), tf.GetQuaternionRotation(), BodyType::DYNAMIC);  // Assuming true = dynamic

    AABB bounds = baseLOD.localAABB;
    bounds.max = bounds.max * tf.GetScale();
    bounds.min = bounds.min * tf.GetScale();

    // Create a sphere collider for the rigid body using the physics backend
    Physics::Collider* collider = m_physicsSystem.GetWorld()->AddSphereCollider(body, radius, 1.f);
    collider->SetFriction(0.5f);

    // Add the Rigidbody and Collider components to the entity
    m_entityManager.AddComponent<Physics::RigidBody*>(e, body);
    m_entityManager.AddComponent<Physics::Collider*>(e, collider);
}

void MurdochScene::SpawnChair(const Vector3f& position)
{
    MeshRenderer renderer = m_meshFiles[5].ToRenderer();

    Transform tf;
    tf.SetPosition(position);
    tf.SetRotation(Vector3f(0.f, -90.f, 0.f));
    tf.SetScale(Vector3f(1.5f));

    Entity e = m_entityManager.CreateEntity();
    m_entityManager.AddComponent<Transform>(e, tf);
    m_entityManager.AddComponent<MeshRenderer>(e, renderer);

    Physics::RigidBody* body = m_physicsSystem.GetWorld()->CreateRigidBody(tf.GetPosition(), tf.GetQuaternionRotation(), BodyType::DYNAMIC);
    m_entityManager.AddComponent<Physics::RigidBody*>(e, body);

    //Back rest
    Transform localTF;
    localTF.SetRotation(0.f, 0.f, 12.f);
    localTF.SetScale(0.1f, 0.75f, 1.f);
    AddBounds(e, localTF, 0.5f, false);
    //Seat
    localTF.SetPosition(1.2f, -0.9f, 0.f);
    localTF.SetRotation(0.f, 0.f, 0.f);
    localTF.SetScale(0.9f, 0.08f, 1.0f);
    AddBounds(e, localTF, 1.f, false);
    //Front right leg
    localTF.SetPosition(2.f, -1.5f, 1.f);
    localTF.SetRotation(0.f, 0.f, 13.f);
    localTF.SetScale(0.1f, 0.8f, 0.1f);
    AddBounds(e, localTF, 0.25f, false);
    //Front left leg
    localTF.SetPosition(2.f, -1.5f, -1.f);
    localTF.SetRotation(0.f, 0.f, 13.f);
    localTF.SetScale(0.1f, 0.8f, 0.1f);
    AddBounds(e, localTF, 0.25f, false);
    //Back right leg
    localTF.SetPosition(0.3f, -1.5f, 1.f);
    localTF.SetRotation(0.f, 0.f, -13.f);
    localTF.SetScale(0.1f, 0.8f, 0.1f);
    AddBounds(e, localTF, 0.25f, false);
    //Back left leg
    localTF.SetPosition(0.3f, -1.5f, -1.f);
    localTF.SetRotation(0.f, 0.f, -13.f);
    localTF.SetScale(0.1f, 0.8f, 0.1f);
    AddBounds(e, localTF, 0.25f, false);
}

void MurdochScene::SetupTestColliders(Entity e)
{
    m_testRigidBodyEntity = e;

    Transform tf;
    tf.SetScale(0.5f, 0.5f, 0.5f);

    m_COMEntity = m_entityManager.CreateEntity();
    m_entityManager.AddComponent<Transform>(m_COMEntity, tf);
    m_entityManager.AddComponent<MeshRenderer>(m_COMEntity, m_meshFiles[0].ToRenderer());

    m_posEntity = m_entityManager.CreateEntity();
    m_entityManager.AddComponent<Transform>(m_posEntity, tf);
    m_entityManager.AddComponent<MeshRenderer>(m_posEntity, m_meshFiles[0].ToRenderer());

}

void MurdochScene::SpawnCrates()
{
    int count = 7;

    for (int i = 0; i < count; ++i)
    {
        float x = (std::rand() % 2 ? 1.f : -1.f) * static_cast<float>(std::rand() % 27);
        float z = (std::rand() % 2 ? 1.f : -1.f) * static_cast<float>(std::rand() % 9);
        float y = 10.f;

        SpawnCrate(Vector3f(502.f + x, y, 511.f + z));
    }
}

void MurdochScene::SpawnCrate(const Vector3f& position)
{
    auto renderer = m_meshFiles[2].ToRenderer();
    const MeshLOD& baseLOD = renderer.lods[0];

    Transform tf;
    tf.SetPosition(position);
    tf.SetRotation(0.f, static_cast<float>(std::rand() % 360), 0.f);

    Entity e = m_entityManager.CreateEntity();
    m_entityManager.AddComponent<Transform>(e, tf);
    m_entityManager.AddComponent<MeshRenderer>(e, renderer);

    // Create a dynamic rigid body using the physics backend
    Physics::RigidBody* body = m_physicsSystem.GetWorld()->CreateRigidBody(tf.GetPosition(), tf.GetQuaternionRotation(), BodyType::DYNAMIC);  // Assuming true = dynamic

    // Create a sphere collider for the rigid body using the physics backend
    Physics::Collider* collider = m_physicsSystem.GetWorld()->AddBoxCollider(body, baseLOD.localAABB, 2.f);
    collider->SetFriction(0.9f);

    // Add the Rigidbody and Collider components to the entity
    m_entityManager.AddComponent<Physics::RigidBody*>(e, body);
    m_entityManager.AddComponent<Physics::Collider*>(e, collider);
}

void MurdochScene::SpawnBin(const Vector3f& position, int fileIndex)
{
    MeshRenderer renderer = m_meshFiles[fileIndex].ToRenderer();

    Transform tf;
    tf.SetPosition(position);
    tf.SetScale(Vector3f(3.f));

    Entity e = m_entityManager.CreateEntity();
    m_entityManager.AddComponent<Transform>(e, tf);
    m_entityManager.AddComponent<MeshRenderer>(e, renderer);

    Physics::RigidBody* body = m_physicsSystem.GetWorld()->CreateRigidBody(tf.GetPosition(), tf.GetQuaternionRotation(), BodyType::DYNAMIC);
    m_entityManager.AddComponent<Physics::RigidBody*>(e, body);

    Transform localTF;
    //Front side
    localTF.SetPosition(0.3f, 0.f, 0.f);
    localTF.SetScale(0.05f, 0.75f, 0.35f);
    AddBounds(e, localTF, 1.f, false);

    //Back side
    localTF.SetPosition(-0.3f, 0.f, 0.f);
    localTF.SetScale(0.05f, 0.75f, 0.35f);
    AddBounds(e, localTF, 1.f, false);

    //Left side
    localTF.SetPosition(0.f, 0.f, 0.3f);
    localTF.SetScale(0.35f, 0.75f, 0.05f);
    AddBounds(e, localTF, 1.f, false);

    //Right side
    localTF.SetPosition(0.f, 0.f, -0.3f);
    localTF.SetScale(0.35f, 0.75f, 0.05f);
    AddBounds(e, localTF, 1.f, false);

    //Bottom side
    localTF.SetPosition(0.f, -0.7f, 0.f);
    localTF.SetScale(0.35f, 0.05f, 0.35f);
    AddBounds(e, localTF, 1.f, false);

    //SetupTestColliders(e);
}

void MurdochScene::SpawnDogBed(float x, float y, float z)
{
    MeshRenderer renderer = m_meshFiles[9].ToRenderer();

    Transform tf;
    tf.SetPosition(Vector3f(x, y, z));
    tf.SetScale(Vector3f(2.f));

    Entity e = m_entityManager.CreateEntity();
    m_entityManager.AddComponent<Transform>(e, tf);
    m_entityManager.AddComponent<MeshRenderer>(e, renderer);

    // Add affordance component.
    Affordance affordance;
    /*
    affordance.m_action = Affordance::Action::Sleep;
    affordance.m_position = Vector3f(x + 0.5, y, z);
    affordance.m_wellnessEffect[Wellness::Component::Sleep] = 0.5;
    */
    affordance.SetAction(Affordance::Action::Sleep);
    affordance.SetPosition(Vector3f(x + 0.5, y, z));
    affordance.AddWellnessEffect(Wellness::Component::Sleep, 0.5f);
    m_entityManager.AddComponent<Affordance>(e, affordance);
}

void MurdochScene::SpawnWaterBowl(float x, float y, float z)
{
    MeshRenderer renderer = m_meshFiles[10].ToRenderer();

    Transform tf;
    tf.SetPosition(Vector3f(x, y, z));
    tf.SetScale(Vector3f(2.f));

    Entity e = m_entityManager.CreateEntity();
    m_entityManager.AddComponent<Transform>(e, tf);
    m_entityManager.AddComponent<MeshRenderer>(e, renderer);

    Affordance affordance;
    /*
    affordance.m_action = Affordance::Action::Eat;
    affordance.m_position = Vector3f(x + 0.5, y, z);
    affordance.m_wellnessEffect[Wellness::Component::Water] = 0.5;
    */
    affordance.SetAction(Affordance::Action::Eat);
    affordance.SetPosition(Vector3f(x + 0.5, y, z));
    affordance.AddWellnessEffect(Wellness::Component::Water, 0.5f);
    m_entityManager.AddComponent<Affordance>(e, affordance);
}

void MurdochScene::SpawnFoodBowl(float x, float y, float z)
{
    MeshRenderer renderer = m_meshFiles[11].ToRenderer();

    Transform tf;
    tf.SetPosition(Vector3f(x, y, z));
    tf.SetScale(Vector3f(2.f));

    Entity e = m_entityManager.CreateEntity();
    m_entityManager.AddComponent<Transform>(e, tf);
    m_entityManager.AddComponent<MeshRenderer>(e, renderer);

    Affordance affordance;
    /*
    affordance.m_action = Affordance::Action::Eat;
    affordance.m_position = Vector3f(x + 0.5, y, z);
    affordance.m_wellnessEffect[Wellness::Component::Food] = 0.5;
    */
    affordance.SetAction(Affordance::Action::Eat);
    affordance.SetPosition(Vector3f(x + 0.5, y, z));
    affordance.AddWellnessEffect(Wellness::Component::Food, 0.5f);
    m_entityManager.AddComponent<Affordance>(e, affordance);
}

Entity MurdochScene::SpawnDog(float x, float y, float z) 
{
    Vector3f pos{ x, y, z };
    Entity dog = Dog::SpawnDog(shared_from_this(), pos, Dog::Breed::GermanShepherd);
}

void MurdochScene::SetDogBreed(Entity dog, int breed)
{
    if (!m_entityManager.HasComponent<Dog::DogImage>(dog)) 
    {
        std::cout << "Warning: Cannot change dog breed. Entity is not dog." << std::endl;
        return;
    }
    Dog::DogImage& image = m_entityManager.GetComponent<Dog::DogImage>(dog);
    image.name = Dog::ToString((Dog::Breed) breed);

    if (!m_entityManager.HasComponent<Dog::DogBehaviour>(dog)) 
    {
        std::cout << "Warning: Dog has no behaviour component." << std::endl;
        return;
    }

    Dog::DogBehaviour& behaviour = m_entityManager.GetComponent<Dog::DogBehaviour>(dog);
    
    behaviour.UpdateImage();

    std::cout << "Changed dog breed to: " << image.name << std::endl;
}

MeshFile& MurdochScene::GetMeshFile(int index)
{
    return m_meshFiles[index];
}

MeshFile* MurdochScene::GetMeshFile(const char* name)
{
    auto it = m_meshFileMap.find(name);

    if (it == m_meshFileMap.end()) 
    {
        std::cout << "Warning: Attempting to get non-existent meshFile." << std::endl;
        return nullptr;
    }
    return &m_meshFiles[m_meshFileMap[name]];
}

void MurdochScene::AddDogToUpdate(Entity dog)
{
    m_dogs.insert(dog);
}

void MurdochScene::RemoveDogToUpdate(Entity dog)
{
    m_dogs.erase(dog);
}
