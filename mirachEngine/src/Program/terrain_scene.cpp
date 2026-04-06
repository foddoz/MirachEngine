#include "terrain_scene.hpp"
#include "../Scripts/grounded_player_script.hpp"

TerrainScene::TerrainScene()
    : 
    m_app(1920, 1080), 
    m_physicsSystem(&m_entityManager), 
    m_player(0)
{

}

void TerrainScene::Run()
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

void TerrainScene::GenerateCPUData()
{
    sol::state lua;
    lua.open_libraries(sol::lib::base);

    m_npc.LoadFromFile("Mesh_Files/Animation/berserk/tris.md2", "Mesh_Files/Animation/berserk/skin.png");
    m_firefly.LoadFromFile("Mesh_Files/Animation/hover/tris.md2", "Mesh_Files/Animation/hover/skin.png");


    m_meshFiles.clear(); // Optional: clear old data if reused

    m_meshFiles.push_back(MeshFile());
    m_meshFiles[0].GenerateCPUData("Mesh_Files/sack.obj");

    //m_meshFiles.push_back(MeshFile());
    //m_meshFiles[1].GenerateCPUData("Mesh_Files/Robot/robot.obj");

    m_meshFiles.push_back(MeshFile());
    m_meshFiles[1].GenerateCPUData("Mesh_Files/Trees/Winter_Tree/winter_tree.obj");

    m_meshFiles.push_back(MeshFile());
    m_meshFiles[2].GenerateCPUData("Mesh_Files/Crates/crate.obj");

    m_sky.GenerateCPUData(30, 30, 500.0f, "Mesh_Files/Skies/sky1.jpg");

    int iterations = 500;
    float filter = 0.5f;
    int terrainSize = 1024;
    int chunkSize = 256;
    float minHeight = -20.f;
    float maxHeight = 80.f;
    float textureScale = 4.0f;

    std::vector<std::string> terrainTextures = {
        "Mesh_Files/Terrain/concrete.jpg",
        "Mesh_Files/Terrain/grass.png",
        "Mesh_Files/Terrain/rock.jpg",
        "Mesh_Files/Terrain/water.png"
    };

    bool randomize = true;
    //terrain.GenerateCPUData(terrainSize, iterations, minHeight, maxHeight, filter, textureScale, terrainTextures, randomize);
    m_terrain.CreateFaultFormation(iterations, minHeight, maxHeight, filter, terrainSize, randomize);

    m_terrainManager.Initialize(m_terrain.GetHeightMap(), terrainSize, chunkSize, minHeight, maxHeight, textureScale, terrainTextures);
    m_terrainManager.GenerateAllChunks();
}

void TerrainScene::InitialiseEngine()
{
    //m_app.Initialise();
    m_app.InitialiseWindow();
    m_app.InitialiseGraphics();
    m_app.LockMouse(true);
    m_app.CreateGraphicsPipeline("object", "objectVS.glsl", "objectFS.glsl");
    m_app.CreateGraphicsPipeline("terrain", "terrainVS.glsl", "terrainFS.glsl");
    m_app.CreateGraphicsPipeline("skydome", "skyDomeVS.glsl", "skyDomeFS.glsl");
}


void TerrainScene::UploadAssetsToGPU()
{
    for (auto& mesh : m_meshFiles)
    {
        mesh.UploadToGPU();
    }
    m_sky.UploadToGPU();
    m_terrainManager.UploadChunksToGPU();
    m_npc.UploadToGPU();
    m_firefly.UploadToGPU();
}

void TerrainScene::AssignPipelines()
{
    for (auto& mesh : m_meshFiles)
    {
        mesh.SetPipeline(m_app.GetShaderProgram("object"));
    }
    m_sky.SetPipeline(m_app.GetShaderProgram("skydome"));
    m_npc.SetPipeline(m_app.GetShaderProgram("object"));
    m_firefly.SetPipeline(m_app.GetShaderProgram("object"));
}

void TerrainScene::SetupECS()
{
    SetupSkyDomeEntity();
    SetupPlayer();
    RegisterTerrainToECS();

    std::srand(static_cast<unsigned>(std::time(nullptr)));
    SpawnSacks();

    SpawnMassiveCrates();
    SpawnTrees();

    SpawnCrates();

    SpawnFlatGround();

    //SpawnRobots();
    //SpawnFireFlies();
}

void TerrainScene::MainLoop()
{
    InputManager input;
    input.MapAction(InputAction::MoveForward, KeyCode::W);
    input.MapAction(InputAction::MoveBackward, KeyCode::S);
    input.MapAction(InputAction::MoveLeft, KeyCode::A);
    input.MapAction(InputAction::MoveRight, KeyCode::D);
    input.MapAction(InputAction::Jump, KeyCode::Space);
    input.MapAction(InputAction::E_Key, KeyCode::E);

    Entity player;

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

    //bool firstFrame = true;

    const float fixedStep = 1.0f / 120.0f; // simulate at 120Hz
    float accumulator = 0.0f;

    AnimationSystem animations;

    EnemySystem mEnemySystem(m_entityManager);

    const float fallThreshold = -100.f;

    while (!m_app.GetQuit())
    {
        m_app.UpdateDeltaTime();
        //float dt = std::max(0.016f, gApp.GetDeltaTime());
        //float dt = m_app.GetDeltaTime() * 2.5;
        //accumulator += dt;

        float dt = std::min(0.0333f, m_app.GetDeltaTime() * 2.5f);  // clamp to 33ms
        accumulator = std::min(accumulator + dt, 0.25f);    // cap catch-up ~0.25s

        bool quit = playerScript.CaptureInput(input);
        m_app.SetQuit(quit);

        while (accumulator >= fixedStep)
        {
            m_physicsSystem.Update(fixedStep);
            accumulator -= fixedStep;
        }

        //gApp.m_quit = playerScript.Input(input, entityManager, physicsSystem, dt);
        playerScript.PostPhysicsUpdate(fixedStep);
        animations.Update(m_entityManager, dt);
        mEnemySystem.Update(dt);
        RenderSystem(m_app, player, m_entityManager);
        //glClear(GL_DEPTH_BUFFER_BIT);

        auto deadEntities = m_entityManager.GetAllComponents<Dead>();

        if (m_entityManager.HasComponent<Dead>(player))
        {
            m_app.SetQuit(true);
            std::cout << "GAME OVER! THE PLAYER DIED!\n";
        }

        for (auto& [e, dead] : deadEntities) 
        {
            if (m_entityManager.HasComponent<Physics::RigidBody*>(e))
            {
                m_physicsSystem.DestroyEntity(e);
            }
            m_entityManager.DestroyEntity(e);
        }

        m_app.SwapWindow();
    }
}

void TerrainScene::CleanUp()
{
    for (Entity e : m_entityManager.GetAllEntities()) {
        m_physicsSystem.DestroyEntity(e);
        m_entityManager.DestroyEntity(e);
    }
    m_app.Delete();
    m_app.DeleteWindow();
}

void TerrainScene::Shutdown()
{
    for (auto& mesh : m_meshFiles)
    {
        mesh.Delete();
    }
    m_terrain.Delete();
    m_sky.Delete();
    m_app.DeleteGraphics(); 
}


void TerrainScene::SetupSkyDomeEntity()
{
    Transform skyTransform;
    skyTransform.SetPosition(0.0f, -150.0f, 0.0f);

    Entity skyEntity = m_entityManager.CreateEntity();
    m_entityManager.AddComponent<Transform>(skyEntity, skyTransform);
    m_entityManager.AddComponent<MeshRenderer>(skyEntity, m_sky.ToRenderer());
}

void TerrainScene::SetupPlayer()
{
    m_player = m_entityManager.CreateEntity();
    Camera cam;
    cam.SetProjectionMatrix(75.f, m_app.GetScreenWidth() / m_app.GetScreenHeight(), 0.1f, 99999.f);
    m_entityManager.AddComponent<Camera>(m_player, cam);

    //float x = m_terrain.GetWidth() * 0.5f;
    float x = 512.f;
    //float z = m_terrain.GetDepth() * 0.5f;
    float z = 512.f;
    //float y = m_terrain.GetHeightInterpolated(x, z) + 20.f;
    float y = 150.f;

    Transform tf;
    tf.SetPosition(x, y, z);
    m_entityManager.AddComponent<Transform>(m_player, tf);

    Physics::RigidBody* body = m_physicsSystem.GetWorld()->CreateRigidBody(tf.GetPosition(), tf.GetQuaternionRotation(), BodyType::DYNAMIC);
    body->SetAngularFreeAxis(Vector3f(1.f, 0.f, 1.f));
    body->SetAngularDamping(0.2f);
    m_entityManager.AddComponent<Physics::RigidBody*>(m_player, body);

    Physics::Collider* collider = m_physicsSystem.GetWorld()->AddCapsuleCollider(body, 0.3f, 10.f, 1.f);
    //collider->SetMass(5.f);
    collider->SetFriction(0.5f);
    collider->SetBounciness(0.2f);

    //collider->SetCategoryBits(PhysicsSystem::PLAYER_CATEGORY);
    //collider->SetCollisionMask(PhysicsSystem::TERRAIN_CATEGORY | PhysicsSystem::OBJECT_CATEGORY | PhysicsSystem::ROBOT_CATEGORY);

    m_entityManager.AddComponent<Physics::Collider*>(m_player, collider);

    Health playerHealth;
    playerHealth.max = 100.f;
    playerHealth.current = 100.f;
    m_entityManager.AddComponent<Health>(m_player, playerHealth);
}

void TerrainScene::RegisterTerrainToECS()
{
    std::cout << "[Loading] Setting up Terrain...\n";
    m_terrainManager.RegisterChunksToECS(m_entityManager, m_physicsSystem, m_app.GetShaderProgram("terrain"));
    std::cout << "[Loading] Terrain Complete\n";
}

void TerrainScene::SpawnRobots()
{

    int count = 15;

    auto renderer = m_npc.ToRenderer();
    const MeshLOD& baseLOD = renderer.lods[0];

    for (int i = 0; i < count; ++i)
    {
        float x = static_cast<float>(std::rand() % m_terrain.GetWidth());
        float z = static_cast<float>(std::rand() % m_terrain.GetDepth());
        //float y = m_terrain.GetHeightInterpolated(x, z);

        float y = 150.f;

        Transform tf;
        tf.SetYAxisUp(false);
        tf.SetPosition(x, y + 10.f, z);
        tf.SetRotation(0.f, static_cast<float>(std::rand() % 360), 0.f);

        Vector3f scale = Vector3f(0.1f, 0.1f, 0.1f);
        tf.SetScale(scale.x, scale.y, scale.z);

        scale = Vector3f(0.3f, 0.2f, 0.2f);

        Entity e = m_entityManager.CreateEntity();
        m_entityManager.AddComponent<Transform>(e, tf);

        MD2Animator animator;
        animator.mesh = &m_npc;
        animator.currentAnim = "standb";
        animator.animatedVBO = Graphics::CreateEmptyBuffer(sizeof(Vector3f) * m_npc.GetVertexCount());
        animator.interpolatedVerts.resize(m_npc.GetVertexCount());

        m_entityManager.AddComponent<MD2Animator>(e, animator);
        m_entityManager.AddComponent<MeshRenderer>(e, renderer);

        Vector3f offset = Vector3f(0.0f, 0.f, 0.0f);

        // Create a dynamic rigid body using the physics backend
        Physics::RigidBody* body = m_physicsSystem.GetWorld()->CreateRigidBody(tf.GetPosition(), tf.GetQuaternionRotation(), BodyType::DYNAMIC);  // Assuming true = dynamic
        body->SetAngularFreeAxis(Vector3f(1.0f, 1.0f, 0.0f));
        body->SetTotalMass(1.f);

        //Physics::Collider* collider = m_physicsSystem.GetWorld()->AddCapsuleCollider(body, baseLOD.LocalAABB, scale, offset);
        Physics::Collider* collider = m_physicsSystem.GetWorld()->AddCapsuleCollider(body, 0.3f, 10.f, 5.f);
        //collider->SetMass(5.f);

        //collider->SetCategoryBits(PhysicsSystem::ROBOT_CATEGORY);
        //collider->SetCollisionMask(PhysicsSystem::TERRAIN_CATEGORY | PhysicsSystem::PLAYER_CATEGORY | PhysicsSystem::OBJECT_CATEGORY | PhysicsSystem::ROBOT_CATEGORY);

        // Add the Rigidbody and Collider components to the entity
        m_entityManager.AddComponent<Physics::RigidBody*>(e, body);
        m_entityManager.AddComponent<Physics::Collider*>(e, collider);

        Health robotHealth;
        robotHealth.max = 100.f;
        robotHealth.current = 100.f;

        m_entityManager.AddComponent<Health>(e, robotHealth);

        Enemy* robot = new Enemy("Robot" + std::to_string(i), e, &m_player, &m_entityManager, &m_enemyAI, &m_physicsSystem);
        //mEnemySystem.Register(robot);
        m_entityManager.AddComponent<Enemy*>(e, robot);
    }
}

void TerrainScene::SpawnFlatGround()
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

void TerrainScene::SpawnSacks()
{
    int count = 120;
    auto renderer = m_meshFiles[0].ToRenderer();
    const MeshLOD& baseLOD = renderer.lods[0];

    for (int i = 0; i < count; ++i)
    {
        float x = static_cast<float>(std::rand() % m_terrain.GetWidth());
        float z = static_cast<float>(std::rand() % m_terrain.GetDepth());
        //float y = m_terrain.GetHeightInterpolated(x, z);

        float y = 150.f;

        Transform tf;
        tf.SetPosition(x, y + 5.f, z);
        tf.SetRotation(0.f, static_cast<float>(std::rand() % 360), 0.f);
        //tf.SetScale(10.f, 10.f, 10.f);

        Entity e = m_entityManager.CreateEntity();
        m_entityManager.AddComponent<Transform>(e, tf);
        m_entityManager.AddComponent<MeshRenderer>(e, renderer);

        // Create a dynamic rigid body using the physics backend
        Physics::RigidBody* body = m_physicsSystem.GetWorld()->CreateRigidBody(tf.GetPosition(), tf.GetQuaternionRotation(), BodyType::DYNAMIC);  // Assuming true = dynamic

        // Create a sphere collider for the rigid body using the physics backend
        Physics::Collider* collider = m_physicsSystem.GetWorld()->AddSphereCollider(body, baseLOD.localAABB, 1.f);
        collider->SetFriction(0.5f);

        // Set collision category and mask bits
        //collider->SetCategoryBits(PhysicsSystem::OBJECT_CATEGORY);
        //collider->SetCollisionMask(PhysicsSystem::TERRAIN_CATEGORY | PhysicsSystem::OBJECT_CATEGORY | PhysicsSystem::ROBOT_CATEGORY);

        // Add the Rigidbody and Collider components to the entity
        m_entityManager.AddComponent<Physics::RigidBody*>(e, body);
        m_entityManager.AddComponent<Physics::Collider*>(e, collider);
    }
}

void TerrainScene::SpawnCrates()
{
    int count = 120;
    auto renderer = m_meshFiles[2].ToRenderer();
    const MeshLOD& baseLOD = renderer.lods[0];

    for (int i = 0; i < count; ++i)
    {
        float x = static_cast<float>(std::rand() % m_terrain.GetWidth());
        float z = static_cast<float>(std::rand() % m_terrain.GetDepth());
        //float y = m_terrain.GetHeightInterpolated(x, z);

        float y = 150.f;

        Transform tf;
        tf.SetPosition(x, y + 5.f, z);
        tf.SetRotation(0.f, static_cast<float>(std::rand() % 360), 0.f);
        //tf.SetScale(10.f, 10.f, 10.f);

        Entity e = m_entityManager.CreateEntity();
        m_entityManager.AddComponent<Transform>(e, tf);
        m_entityManager.AddComponent<MeshRenderer>(e, renderer);

        // Create a dynamic rigid body using the physics backend
        Physics::RigidBody* body = m_physicsSystem.GetWorld()->CreateRigidBody(tf.GetPosition(), tf.GetQuaternionRotation(), BodyType::DYNAMIC);  // Assuming true = dynamic

        // Create a sphere collider for the rigid body using the physics backend
        Physics::Collider* collider = m_physicsSystem.GetWorld()->AddBoxCollider(body, baseLOD.localAABB, 2.f);
        collider->SetFriction(0.9f);
        //collider->SetBounciness(0.95f);

        // Set collision category and mask bits
        //collider->SetCategoryBits(PhysicsSystem::OBJECT_CATEGORY);
        //collider->SetCollisionMask(PhysicsSystem::TERRAIN_CATEGORY | PhysicsSystem::OBJECT_CATEGORY | PhysicsSystem::ROBOT_CATEGORY);

        // Add the Rigidbody and Collider components to the entity
        m_entityManager.AddComponent<Physics::RigidBody*>(e, body);
        m_entityManager.AddComponent<Physics::Collider*>(e, collider);
    }
}

void TerrainScene::SpawnMassiveCrates()
{
    int count = 20;
    auto renderer = m_meshFiles[2].ToRenderer();
    const MeshLOD& baseLOD = renderer.lods[0];

    for (int i = 0; i < count; ++i)
    {
        float x = static_cast<float>(std::rand() % m_terrain.GetWidth());
        float z = static_cast<float>(std::rand() % m_terrain.GetDepth());
        //float y = m_terrain.GetHeightInterpolated(x, z);

        float y = -20.f;

        Transform tf;
        //tf.SetPosition(x, y + 5.f, z);
        tf.SetPosition(x, 5.f, z);
        tf.SetRotation(0.f, static_cast<float>(std::rand() % 360), 0.f);
        //tf.SetRotation(0.f, i % 2 == 0 ? 45.f : 0.f, 0.f);
        tf.SetScale(10.f, 10.f, 10.f);

        Entity e = m_entityManager.CreateEntity();
        m_entityManager.AddComponent<Transform>(e, tf);
        m_entityManager.AddComponent<MeshRenderer>(e, renderer);

        // Create a dynamic rigid body using the physics backend
        Physics::RigidBody* body = m_physicsSystem.GetWorld()->CreateRigidBody(tf.GetPosition(), tf.GetQuaternionRotation(), BodyType::KINEMATIC);  // Assuming true = dynamic

        /*
        Quaternion q = tf.GetQuaternionRotation();
        std::cout << "Crate Transform Quaternion = [" << q.w << "," << q.x << "," << q.y << "," << q.z << "]\n";
        q = body->GetOrientation();
        std::cout << "Crate Rigidbody Quaternion = [" << q.w << "," << q.x << "," << q.y << "," << q.z << "]\n";
        */
        Vector3f scale = Vector3f(10.0f, 10.0f, 10.0f);
        AABB bounds = baseLOD.localAABB;

        bounds.max = bounds.max * scale;
        bounds.min = bounds.min * scale;

        // Create a sphere collider for the rigid body using the physics backend
        //Physics::Collider* collider = m_physicsSystem.GetWorld()->AddBoxCollider(body, baseLOD.LocalAABB, scale);
        Physics::Collider* collider = m_physicsSystem.GetWorld()->AddBoxCollider(body, bounds, 50.f);

        // Set collision category and mask bits
        collider->SetCategoryBits(PhysicsSystem::OBJECT_CATEGORY);
        collider->SetCollisionMask(PhysicsSystem::PLAYER_CATEGORY | PhysicsSystem::OBJECT_CATEGORY | PhysicsSystem::ROBOT_CATEGORY);

        // Add the Rigidbody and Collider components to the entity
        m_entityManager.AddComponent<Physics::RigidBody*>(e, body);
        m_entityManager.AddComponent<Physics::Collider*>(e, collider);
    }
}

void TerrainScene::SpawnTrees()
{
    int count = 30;
    auto renderer = m_meshFiles[1].ToRenderer();
    const MeshLOD& baseLOD = renderer.lods[0];

    for (int i = 0; i < count; ++i)
    {
        float x = static_cast<float>(std::rand() % m_terrain.GetWidth());
        float z = static_cast<float>(std::rand() % m_terrain.GetDepth());
        //float y = m_terrain.GetHeightInterpolated(x, z);

        float y = -20.f;

        Transform tf;
        //tf.SetPosition(x, y + 5.f, z);
        tf.SetPosition(x, 5.f, z);
        //tf.SetRotation(0.f, static_cast<float>(std::rand() % 360), 0.f);
        tf.SetScale(100.f, 100.f, 100.f);

        Entity e = m_entityManager.CreateEntity();
        m_entityManager.AddComponent<Transform>(e, tf);
        m_entityManager.AddComponent<MeshRenderer>(e, renderer);

        // Create a dynamic rigid body using the physics backend
        Physics::RigidBody* body = m_physicsSystem.GetWorld()->CreateRigidBody(tf.GetPosition(), tf.GetQuaternionRotation(), BodyType::KINEMATIC);  // Assuming true = dynamic

        Vector3f scale = Vector3f(100.0f, 100.0f, 100.0f);
        AABB bounds = baseLOD.localAABB;

        bounds.max = bounds.max * scale;
        bounds.min = bounds.min * scale;

        
        // Create a sphere collider for the rigid body using the physics backend
        //Physics::Collider* collider = m_physicsSystem.GetWorld()->AddBoxCollider(body, baseLOD.LocalAABB, scale);
        Physics::Collider* collider = m_physicsSystem.GetWorld()->AddBoxCollider(body, bounds, 50.f);

        // Set collision category and mask bits
        //collider->SetCategoryBits(PhysicsSystem::OBJECT_CATEGORY);
        //collider->SetCollisionMask(PhysicsSystem::PLAYER_CATEGORY | PhysicsSystem::OBJECT_CATEGORY | PhysicsSystem::ROBOT_CATEGORY);

        // Add the Rigidbody and Collider components to the entity
        m_entityManager.AddComponent<Physics::RigidBody*>(e, body);
        m_entityManager.AddComponent<Physics::Collider*>(e, collider);
    }
}

void TerrainScene::SpawnFireFlies()
{
    int count = 20;

    auto renderer = m_firefly.ToRenderer();
    const MeshLOD& baseLOD = renderer.lods[0];

    for (int i = 0; i < count; ++i)
    {
        float x = static_cast<float>(std::rand() % m_terrain.GetWidth());
        float z = static_cast<float>(std::rand() % m_terrain.GetDepth());
        float y = m_terrain.GetHeightInterpolated(x, z);

        Transform tf;
        tf.SetYAxisUp(false);
        tf.SetPosition(x, y + 50.f, z);
        tf.SetRotation(0.f, static_cast<float>(std::rand() % 360), 0.f);

        Vector3f scale = Vector3f(0.1f, 0.1f, 0.1f);
        tf.SetScale(scale.x, scale.y, scale.z);

        //scale *= 2;
        scale = Vector3f(0.3f, 0.2f, 0.2f);

        Entity e = m_entityManager.CreateEntity();
        m_entityManager.AddComponent<Transform>(e, tf);

        MD2Animator animator;
        animator.mesh = &m_firefly;
        animator.currentAnim = "forwrd";
        /*
            stand: 0 to 29
                forwrd : 30 to 64
                death : 162 to 172
                pain : 113 to 161
                stop : 65 to 81
                takeof : 82 to 111
                land : 112 to 112
                backwd : 173 to 196
                attak : 197 to 204
                */
        animator.animatedVBO = Graphics::CreateEmptyBuffer(sizeof(Vector3f) * m_npc.GetVertexCount());
        animator.interpolatedVerts.resize(m_npc.GetVertexCount());

        m_entityManager.AddComponent<MD2Animator>(e, animator);
        m_entityManager.AddComponent<MeshRenderer>(e, renderer);


        Vector3f offset = Vector3f(0.0f, 0.f, 0.0f);
        //Vector3f bodyPosition = tf.GetPosition() - offset;

        // Create a dynamic rigid body using the physics backend
        Physics::RigidBody* body = m_physicsSystem.GetWorld()->CreateRigidBody(tf.GetPosition(), tf.GetQuaternionRotation(), BodyType::DYNAMIC);  // Assuming true = dynamic
        body->SetAngularFreeAxis(Vector3f(0.0f, 0.0f, 1.0f));
        body->EnableGravity(false);

        //Physics::Collider* collider = m_physicsSystem.GetWorld()->AddCapsuleCollider(body, baseLOD.LocalAABB, scale, offset);
        Physics::Collider* collider = m_physicsSystem.GetWorld()->AddCapsuleCollider(body, 0.3f, 10.f, 5.f);

        //collider->SetCategoryBits(PhysicsSystem::ROBOT_CATEGORY);
        //collider->SetCollisionMask(PhysicsSystem::TERRAIN_CATEGORY | PhysicsSystem::PLAYER_CATEGORY | PhysicsSystem::OBJECT_CATEGORY | PhysicsSystem::ROBOT_CATEGORY);

        // Add the Rigidbody and Collider components to the entity
        m_entityManager.AddComponent<Physics::RigidBody*>(e, body);
        m_entityManager.AddComponent<Physics::Collider*>(e, collider);

        Health robotHealth;
        robotHealth.max = 50.f;
        robotHealth.current = 50.f;

        m_entityManager.AddComponent<Health>(e, robotHealth);

        Enemy* robot = new Enemy("Robot" + std::to_string(i), e, &m_player, &m_entityManager, &m_hoverEnemyAI, &m_physicsSystem);
        //mEnemySystem.Register(robot);
        m_entityManager.AddComponent<Enemy*>(e, robot);
    }

}

