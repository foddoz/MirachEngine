#include "dog.hpp"
#include <memory>
#include <Agent/Dog/dog_behaviour_component.hpp>
#include <Wellness/wellness.hpp>

Entity Dog::SpawnDog(std::shared_ptr<MurdochScene> scene, const Vector3f& pos, Breed breed)
{
	EntityManager& ECS = scene->GetECS();

	
	// Create Entity.
	Entity dog = ECS.CreateEntity();
	scene->AddDogToUpdate(dog);

	Transform transform = Transform();
	transform.SetPosition(pos);
	transform.SetScale(Vector3f(3.f));
	ECS.AddComponent<Transform>(dog, transform);

	DogBehaviour behaviour = DogBehaviour();
	behaviour.Init(scene, dog);
	ECS.AddComponent<DogBehaviour>(dog, behaviour);

	// Add image component here.
	DogImage dogImage{};
	dogImage.name = ToString(breed);
	ECS.AddComponent<DogImage>(dog, dogImage);

	// Add image renderer component and setup texture
	DogImage& img = ECS.GetComponent<DogImage>(dog);
	const std::string path = "DogSprites/" + img.name + "/walk.jpg";
	img.texture->LoadFromFile(path);
	img.texture->UploadToGPU();

	//Add emotion model(PAD) component
	//Initialize emotion
	EmotionModel emo{};
	emo.pad = { 0.f, 0.f, 0.f };
	emo.emotion = Emotion::Happy;
	ECS.AddComponent<EmotionModel>(dog, emo);

	//Add emotion renderer component
	DogEmojiImage emoImg{};
	emoImg.emojiImage.name = ToString(emo.emotion);
	Dog::ChangeEmojiImage(emoImg.emojiImage);
	emoImg.emotion = emo.emotion;
	ECS.AddComponent<DogEmojiImage>(dog, emoImg);

	PhysicsSystem& physics = scene->GetPhysicsSystem();

	Physics::RigidBody* body = physics.GetWorld()->CreateRigidBody(transform.GetPosition(), transform.GetQuaternionRotation(), BodyType::KINEMATIC);  // Assuming true = dynamic

	// Create a sphere collider for the rigid body using the physics backend
	Physics::Collider* collider = physics.GetWorld()->AddSphereCollider(body, 2.5f, 1.f);
	collider->SetFriction(1.0f);

	// Add the Rigidbody and Collider components to the entity
	ECS.AddComponent<Physics::RigidBody*>(dog, body);
	ECS.AddComponent<Physics::Collider*>(dog, collider);

	// Add wellness component
	ECS.AddComponent<Wellness>(dog, Wellness());

	// Add nature component
	ECS.AddComponent<Nature>(dog, Nature());

	return dog;
}

void Dog::AddToScriptingAPI(MurdochScripting& API)
{
	API.AddGlobalFunction("spawn_dog", &SpawnDog);
}

std::string Dog::ToString(Breed breed) 
{
	switch (breed) 
	{
	case Breed::GermanShepherd:
		return "GermanShepherd";
	case Breed::GoldenRetriever:
		return "GoldenRetriever";
	case Breed::ToyPoodle:
		return "ToyPoodle";
	default:
		return "GermanShepherd";
	}
}

std::string Dog::ToString(State state) 
{
	switch (state) 
	{
	case State::Think:
		return "think";
	case State::Sit:
		return "sit";
	case State::Walk:
		return "walk";
	case State::Rest:
		return "rest";
	case State::Eat:
		return "eat";
	case State::Play:
		return "play";
	} 
	return "sit";
}

std::string Dog::ToString(Emotion emotion) 
{
	switch (emotion) 
	{
	case Emotion::Happy:
		return "happy";
	case Emotion::Angry:
		return "angry";
	case Emotion::Scared:
		return "scared";
	case Emotion::Sad:
		return "sad";
	} 
	return "happy";
}

//Initialize renderer
bool Dog::InitRenderer()
{
	renderer.InitWorldQuad("3DimageVS.glsl", "3DimageFS.glsl");
	renderer.InitImageFaceToPlayer("3DimageFTP_VS.glsl", "3DimageFTP_FS.glsl");

	return true;
}

//Update Texture
void Dog::ChangeImage(EntityManager& ecs, Entity dog, State state)
{
	if (!ecs.HasComponent<DogImage>(dog)) 
	{
		std::cerr << "[Dog] : entity has no DogImage component\n";
		return;
	}

	auto& image = ecs.GetComponent<DogImage>(dog);

	if (image.texture && image.texture->GetID() != 0) 
	{
		image.texture->Delete();
	}

	std::string path = "DogSprites/" + image.name + "/";

	if (state == State::Think) 
	{
		path += ToString(State::Sit) + ".jpg";
	}
	else 
	{
		path += ToString(state) + ".jpg";
	}
	image.texture->LoadFromFile(path);
	image.texture->UploadToGPU();
}


void Dog::ChangeEmojiImage(DogImage& Emoji)
{
	if (Emoji.texture->GetID() != 0) 
	{
		Emoji.texture->Delete();
	}

	const std::string path = "EmotionSprites/" + Emoji.name + ".png";
	Emoji.texture->LoadFromFile(path);
	Emoji.texture->UploadToGPU();
}

//Rendering Image
void Dog::RenderDogImage(DogImage& image, const Transform& transform,
	const float* view, const float* proj)
{
	glDisable(GL_CULL_FACE);
	if (image.faceToPlayer)
	{
		renderer.DrawImageFaceToPlayer(*image.texture, transform.GetPosition(), transform.GetScale(), view, proj);
	}
	else
	{
		renderer.DrawQuad3D(*image.texture, glm::value_ptr(transform.GetModelMatrix()), view, proj);
	}
}

//Rendering Emoji
void Dog::RenderDogEmoji(DogImage& image, const Transform& transform, const float* view, 
	const float* proj, Vector2f dogSize, Vector2f emojiSize, float yOffset, float gap) 
{
	float y = dogSize.y * 0.5f + emojiSize.y * 0.5f + gap;
	const Vector3f dogPos = transform.GetPosition();
	const Vector3f emojiPos = dogPos + Vector3f(0.f, y, 0.f);
	renderer.DrawImageFaceToPlayer(*image.texture, emojiPos, emojiSize, view, proj);
}